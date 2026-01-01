#include "Api.h"

#include "Globals.h"

void ApiClient::begin(const String& server, const uint16_t port, const String& username, const String& password)
{
    delete httpClient;
    httpClient = new WdClient{modem.gsmClient, server, port};
    defaultBasicAuthUsername.emplace(username);
    defaultBasicAuthPassword.emplace(password);
    isReady = true;
}

ApiResponse ApiClient::makeRequest(const HttpRequest& request) const
{
    if (!isReady || httpClient == nullptr) return ApiResponse::failed();

    httpClient->beginRequest();

    int err = 0;

    switch (request.method)
    {
    case GET:
        err = httpClient->get(request.path);
        break;
    case POST:
        err = httpClient->post(request.path);
        break;
    case PUT:
        err = httpClient->put(request.path);
        break;
    case DELETE:
        err = httpClient->del(request.path);
        break;
    }

    if (err != 0)
    {
        serialOnlyLog.errorln("Request failed with code " + String(err));
        return ApiResponse::failed();
    }

    if (defaultBasicAuthUsername.has_value() && defaultBasicAuthPassword.has_value())
    {
        httpClient->sendBasicAuth(defaultBasicAuthUsername.value(), defaultBasicAuthPassword.value());
    }

    for (const auto& [key, value] : request.headers)
        httpClient->sendHeader(key, value);

    httpClient->beginBody();

    constexpr size_t bufferSize = 512;
    uint8_t buffer[bufferSize];

    const uint64_t uploadStartMs = millis();

    uint32_t totalBytesRead = 0;

    while (request.body.available() && totalBytesRead < request.bodyLength)
    {
        const size_t bytesRead = request.body.readBytes(buffer, bufferSize);
        totalBytesRead += bytesRead;

        size_t wrote = httpClient->write(buffer, bytesRead);

        constexpr size_t maxWriteRetries = 100;
        size_t retry = 0;

        for (; retry < maxWriteRetries && wrote == 0; ++retry)
        {
            wrote = httpClient->write(buffer, bytesRead);
        }

        if (wrote == 0 && retry == maxWriteRetries)
        {
            return ApiResponse::failed();
        }
    }

    const uint32_t uploadTimeMs = millis() - uploadStartMs;

    httpClient->endRequest();

    const int responseCode = httpClient->responseStatusCode();

    if (responseCode <= 0)
    {
        serialOnlyLog.errorln("Response code " + String(responseCode));
        return ApiResponse::failed();
    }

    std::map<String, String> headers{};

    while (httpClient->headerAvailable())
    {
        headers[httpClient->readHeaderName()] = httpClient->readHeaderValue();
    }

    const int contentLength = httpClient->contentLength();

    if (contentLength <= 0)
    {
        return ApiResponse::empty(responseCode, headers, uploadTimeMs);
    }

    return ApiResponse{responseCode, headers, *httpClient, static_cast<uint32_t>(contentLength), uploadTimeMs};
}

size_t ApiClient::fetch(const ApiResponse& resp, Stream& destination)
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
