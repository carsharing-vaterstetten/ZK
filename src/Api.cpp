#include "Api.h"

#include "Globals.h"

ApiClient::ApiClient(const size_t writeBufferSize, const size_t readBufferSize) : writeBufferSize(writeBufferSize),
    readBufferSize(readBufferSize)
{
}

void ApiClient::begin(const String& server, const uint16_t port, const String& username, const String& password)
{
    delete httpClient;
    httpClient = new WdClient{modem.gsmClient, server, port};
    defaultBasicAuthUsername.emplace(username);
    defaultBasicAuthPassword.emplace(password);
    isReady = true;
}

ApiResponse ApiClient::makeRequest(const HttpRequest& request, const bool ignoreResponseHeaders) const
{
    if (!isReady || httpClient == nullptr) return ApiResponse::failed();

    httpClient->beginRequest();

    int err = 0;

    switch (request.method)
    {
    case ApiHttpMethod::GET:
        err = httpClient->get(request.path);
        break;
    case ApiHttpMethod::POST:
        err = httpClient->post(request.path);
        break;
    case ApiHttpMethod::PUT:
        err = httpClient->put(request.path);
        break;
    case ApiHttpMethod::DELETE:
        err = httpClient->del(request.path);
        break;
    }

    if (err != 0)
    {
        serialOnlyLog.errorln("Request failed with code " + String(err));
        return ApiResponse::failed();
    }

    if (defaultBasicAuthUsername.has_value() && defaultBasicAuthPassword.has_value())
        httpClient->sendBasicAuth(defaultBasicAuthUsername.value(), defaultBasicAuthPassword.value());

    for (const auto& [key, value] : request.headers)
        httpClient->sendHeader(key, value);

    httpClient->beginBody();

    uint8_t buffer[writeBufferSize];

    const ulong uploadStartMs = millis();

    size_t totalBytesRead = 0;

    while (request.body.available() && totalBytesRead < request.bodyLength)
    {
        const size_t bytesRead = request.body.readBytes(buffer, writeBufferSize);
        totalBytesRead += bytesRead;

        size_t wrote = httpClient->write(buffer, bytesRead);

        constexpr uint maxWriteRetries = 100;
        uint retry = 0;

        for (; retry < maxWriteRetries && wrote == 0; ++retry)
        {
            yield();
            wrote = httpClient->write(buffer, bytesRead);
        }

        if (wrote == 0 && retry == maxWriteRetries)
        {
            return ApiResponse::failed();
        }
    }

    const uint uploadTimeMs = millis() - uploadStartMs;

    httpClient->endRequest();

    const int responseCode = httpClient->responseStatusCode();

    if (responseCode <= 0)
    {
        serialOnlyLog.errorln("Response code " + String(responseCode));
        return ApiResponse::failed();
    }

    std::map<String, String> headers{};

    while (!ignoreResponseHeaders && httpClient->headerAvailable())
        headers[httpClient->readHeaderName()] = httpClient->readHeaderValue();

    const int contentLength = httpClient->contentLength();

    if (ignoreResponseHeaders)
        httpClient->skipResponseHeaders();

    if (contentLength <= 0)
    {
        return ApiResponse::empty(responseCode, headers, uploadTimeMs);
    }

    return ApiResponse{responseCode, headers, *httpClient, static_cast<uint32_t>(contentLength), uploadTimeMs};
}

uint ApiClient::fetch(const ApiResponse& resp, Stream& destination) const
{
    WdClient& downloadStream = resp.body;

    uint8_t buf[readBufferSize];

    uint downloaded = 0;

    while (downloadStream.connected() || downloadStream.available() > 0)
    {
        int len = downloadStream.read(buf, readBufferSize);
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
