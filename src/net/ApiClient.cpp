#include "net/ApiClient.h"

#include <utility>

#include "domain/LocalConfig.h"
#include "logging/Loggers.h"

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
        logger.errorln("Request failed with code " + String(err));
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
            state = ApiClientState::None;
            return ApiResponse::failed();
        }
    }

    const uint uploadTimeMs = millis() - uploadStartMs;
    state = ApiClientState::None;

    if (hasTimedOut()) return ApiResponse::failed();

    httpClient.endRequest();

    if (hasTimedOut()) return ApiResponse::failed();

    const int responseCode = httpClient.responseStatusCode();

    if (responseCode <= 0)
    {
        logger.errorln("Response code " + String(responseCode));
        return ApiResponse::failed();
    }

    std::map<String, String> headers{};

    while (!ignoreResponseHeaders && httpClient.headerAvailable())
    {
        if (hasTimedOut()) return ApiResponse::failed();
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

    // Fixed size rather than a runtime-sized array: this sits on a 4 KiB task
    // stack, and a caller-chosen length is a stack overflow waiting to happen.
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
