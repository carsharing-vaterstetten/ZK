#include "net/ApiClient.h"

#include <utility>

#include "domain/LocalConfig.h"
#include "logging/Loggers.h"

namespace
{
    /// ArduinoHttpClient's four error codes, named — see HttpClient.h. Turns
    /// "Request failed with code -1" into something that doesn't need the
    /// library's header open to make sense of.
    String describeHttpClientError(const int err)
    {
        switch (err)
        {
        case HTTP_SUCCESS: return "success";
        case HTTP_ERROR_CONNECTION_FAILED: return "could not connect to the server";
        case HTTP_ERROR_API: return "HttpClient used incorrectly (internal bug)";
        case HTTP_ERROR_TIMED_OUT: return "timed out waiting for a reply";
        case HTTP_ERROR_INVALID_RESPONSE: return "response wasn't valid HTTP";
        default: return "unknown error " + String(err);
        }
    }
}

ApiResponse ApiClient::makeRequest(const HttpRequest& request, const bool ignoreResponseHeaders,
                                   const ulong timeout, bool addUsernameAndPassword)
{
    const ulong requestStart = millis();

    // Helper lambda to check timeout
    auto hasTimedOut = [&]() -> bool
    {
        return millis() - requestStart > timeout * 1000;
    };

    httpClient.beginRequest();

    int err = 0;

    switch (request.method)
    {
    case ApiHttpMethod::GET:
        err = httpClient.get(request.path);
        break;
    case ApiHttpMethod::POST:
        err = httpClient.post(request.path);
        break;
    case ApiHttpMethod::PUT:
        err = httpClient.put(request.path);
        break;
    case ApiHttpMethod::DELETE:
        err = httpClient.del(request.path);
        break;
    }

    if (err != 0)
    {
        logger.errorln("Request to " + String(request.path) + " failed: " + describeHttpClientError(err));
        return ApiResponse::failed();
    }

    if (addUsernameAndPassword)
        httpClient.sendBasicAuth(imeiStore.waitForIMEI(), serverPassword);

    for (const auto& [key, value] : request.headers)
        httpClient.sendHeader(key, value);

    httpClient.beginBody();

    constexpr uint writeBufferSize = 512;
    uint8_t buffer[writeBufferSize];

    size_t totalBytesRead = 0;

    progress.store({0, request.bodyLength});
    state = ApiClientState::Uploading;

    const ulong uploadStartMs = millis();

    while (request.body.available() && totalBytesRead < request.bodyLength)
    {
        if (hasTimedOut())
        {
            logger.errorln("Timeout during body upload");
            state = ApiClientState::None;
            return ApiResponse::failed();
        }

        const size_t bytesRead = request.body.readBytes(buffer, writeBufferSize);
        totalBytesRead += bytesRead;
        progress.store({totalBytesRead, request.bodyLength});

        size_t wrote = httpClient.write(buffer, bytesRead);

        constexpr uint maxWriteRetries = 100;
        uint retry = 0;

        for (; retry < maxWriteRetries && wrote == 0; ++retry)
        {
            if (hasTimedOut())
            {
                logger.errorln("Timeout during write retry");
                state = ApiClientState::None;
                return ApiResponse::failed();
            }

            yield();
            wrote = httpClient.write(buffer, bytesRead);
        }

        if (wrote == 0 && retry == maxWriteRetries)
        {
            logger.errorln("Gave up writing " + String(bytesRead) + " B to " + String(request.path) +
                " after " + String(maxWriteRetries) + " retries — connection likely dropped mid-upload "
                "(" + String(totalBytesRead) + "/" + String(request.bodyLength) + " B sent)");
            state = ApiClientState::None;
            return ApiResponse::failed();
        }
    }

    const uint uploadTimeMs = millis() - uploadStartMs;
    state = ApiClientState::None;

    if (hasTimedOut())
    {
        logger.errorln("Timed out immediately after finishing the body upload to " + String(request.path) +
            " (" + String(totalBytesRead) + " B sent) — never got a chance to read the response");
        return ApiResponse::failed();
    }

    httpClient.endRequest();

    if (hasTimedOut())
    {
        logger.errorln("Timed out waiting for a response from " + String(request.path) +
            " after the request was fully sent");
        return ApiResponse::failed();
    }

    const int responseCode = httpClient.responseStatusCode();

    if (responseCode <= 0)
    {
        logger.errorln("Couldn't parse a response status line from " + String(request.path) +
            " (got " + String(responseCode) + ") — response likely wasn't HTTP, or the connection dropped");
        return ApiResponse::failed();
    }

    std::map<String, String> headers{};

    while (!ignoreResponseHeaders && httpClient.headerAvailable())
    {
        if (hasTimedOut())
        {
            logger.errorln("Timed out reading response headers from " + String(request.path) +
                " (status was " + String(responseCode) + ", got " + String(headers.size()) + " header(s))");
            return ApiResponse::failed();
        }
        headers[httpClient.readHeaderName()] = httpClient.readHeaderValue();
    }

    const int contentLength = httpClient.contentLength();

    if (ignoreResponseHeaders)
        httpClient.skipResponseHeaders();

    if (contentLength <= 0)
    {
        return ApiResponse::empty(responseCode, headers, uploadTimeMs);
    }

    return ApiResponse{responseCode, headers, httpClient, static_cast<uint32_t>(contentLength), uploadTimeMs};
}

uint ApiClient::fetch(const ApiResponse& resp, Stream& destination, const ulong timeout)
{
    HttpClient& downloadStream = resp.body;

    // Fixed size: this sits on a 4 KiB task stack.
    static constexpr size_t bufferSize = 512;
    uint8_t buf[bufferSize];

    uint downloaded = 0;

    progress.store({0,resp.bodyLength});
    state = ApiClientState::Downloading;

    const ulong start = millis();

    while (downloadStream.connected() || downloadStream.available() > 0)
    {
        if (millis() - start > timeout * 1000)
        {
            logger.errorln("Timeout during fetch");
            break;
        }

        int len = downloadStream.read(buf, bufferSize);
        if (len > 0)
        {
            destination.write(buf, len);
            downloaded += len;
            progress.store({downloaded, resp.bodyLength});
        }
        else
            yield(); // Prevent CPU spinning if the modem is mid-packet
    }

    state = ApiClientState::None;

    return downloaded;
}
