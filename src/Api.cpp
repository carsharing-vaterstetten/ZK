#include "Api.h"

ApiResponse ApiClient::makeRequest(HttpRequest& request)
{
    if (!isReady) return ApiResponse::failed();

    WdClient& client = httpClient.value();
    client.beginRequest();

    int err = 0;

    switch (request.method)
    {
    case GET:
        err = client.get(request.path);
        break;
    case POST:
        err = client.post(request.path);
        break;
    case PUT:
        err = client.put(request.path);
        break;
    case DELETE:
        err = client.del(request.path);
        break;
    }

    if (err != 0) return ApiResponse::failed();

    if (defaultBasicAuthUsername.has_value() && defaultBasicAuthPassword.has_value())
    {
        client.sendBasicAuth(defaultBasicAuthUsername.value(), defaultBasicAuthPassword.value());
    }

    for (const auto& [key, value] : request.headers)
        client.sendHeader(key, value);

    client.beginBody();

    constexpr size_t bufferSize = 512;
    uint8_t buffer[bufferSize];

    const uint64_t uploadStartMs = millis();

    uint32_t totalBytesRead = 0;

    while (request.body.available() && totalBytesRead < request.bodyLength)
    {
        const size_t bytesRead = request.body.readBytes(buffer, bufferSize);
        totalBytesRead += bytesRead;

        size_t wrote = client.write(buffer, bytesRead);

        constexpr size_t maxWriteRetries = 100;
        size_t retry = 0;

        for (; retry < maxWriteRetries && wrote == 0; ++retry)
        {
            wrote = client.write(buffer, bytesRead);
        }

        if (wrote == 0 && retry == maxWriteRetries)
        {
            return ApiResponse::failed();
        }
    }

    const uint32_t uploadTimeMs = millis() - uploadStartMs;

    client.endRequest();

    const int responseCode = client.responseStatusCode();

    if (responseCode <= 0) return ApiResponse::failed();

    std::map<String, String> headers{};

    while (client.headerAvailable())
    {
        headers[client.readHeaderName()] = client.readHeaderValue();
    }

    const int contentLength = client.contentLength();

    if (contentLength <= 0)
    {
        return ApiResponse::empty(responseCode, headers, uploadTimeMs);
    }

    return ApiResponse{responseCode, headers, client, static_cast<uint32_t>(contentLength), uploadTimeMs};
}

uint32_t ApiClient::fetch(const ApiResponse& resp, Stream& destination)
{
    WdClient& downloadStream = resp.body;

    constexpr size_t bufferSize = 512;
    uint8_t buf[bufferSize];

    size_t downloaded = 0;

    while (downloadStream.connected() || downloadStream.available())
    {
        while (downloadStream.available())
        {
            const size_t len = downloadStream.read(buf, bufferSize);
            destination.write(buf, len);
            downloaded += len;
        }
    }

    return downloaded;
}
