#include "Api.h"

#include <utility>

#include "LocalConfig.h"
#include "shared/Globals.h"

ApiResponse ApiClient::makeRequest(const HttpRequest& request, const bool ignoreResponseHeaders,
                                   const ulong timeout, bool addUsernameAndPassword) const
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
        fileLog.errorln("Request failed with code " + String(err));
        return ApiResponse::failed();
    }

    if (addUsernameAndPassword)
        httpClient.sendBasicAuth(imeiStore.waitForIMEI(), serverPassword);

    for (const auto& [key, value] : request.headers)
        httpClient.sendHeader(key, value);

    httpClient.beginBody();

    constexpr uint writeBufferSize = 512;
    uint8_t buffer[writeBufferSize];

    const ulong uploadStartMs = millis();

    size_t totalBytesRead = 0;

    while (request.body.available() && totalBytesRead < request.bodyLength)
    {
        if (hasTimedOut())
        {
            fileLog.errorln("Timeout during body upload");
            return ApiResponse::failed();
        }

        const size_t bytesRead = request.body.readBytes(buffer, writeBufferSize);
        totalBytesRead += bytesRead;

        size_t wrote = httpClient.write(buffer, bytesRead);

        constexpr uint maxWriteRetries = 100;
        uint retry = 0;

        for (; retry < maxWriteRetries && wrote == 0; ++retry)
        {
            if (hasTimedOut())
            {
                fileLog.errorln("Timeout during write retry");
                return ApiResponse::failed();
            }

            yield();
            wrote = httpClient.write(buffer, bytesRead);
        }

        if (wrote == 0 && retry == maxWriteRetries)
        {
            return ApiResponse::failed();
        }
    }

    const uint uploadTimeMs = millis() - uploadStartMs;

    if (hasTimedOut()) return ApiResponse::failed();

    httpClient.endRequest();

    if (hasTimedOut()) return ApiResponse::failed();

    const int responseCode = httpClient.responseStatusCode();

    if (responseCode <= 0)
    {
        fileLog.errorln("Response code " + String(responseCode));
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

uint ApiClient::fetch(const ApiResponse& resp, Stream& destination, const ulong timeout, size_t bufferSize)
{
    HttpClient& downloadStream = resp.body;

    uint8_t buf[bufferSize];

    uint downloaded = 0;

    const ulong start = millis();

    while (downloadStream.connected() || downloadStream.available() > 0)
    {
        if (millis() - start > timeout * 1000)
        {
            fileLog.errorln("Timeout during fetch");
            break;
        }

        int len = downloadStream.read(buf, bufferSize);
        if (len > 0)
        {
            destination.write(buf, len);
            downloaded += len;
        }
        else
            yield(); // Prevent CPU spinning if the modem is mid-packet
    }

    return downloaded;
}
