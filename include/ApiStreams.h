#pragma once

#include <map>
#include <HttpClient.h>

#include "WatchdogHandler.h"

enum ApiHttpMethod { GET, POST, PUT, DELETE };

class EmptyStream final : public Stream
{
public:
    int available() override { return 0; }
    int read() override { return -1; }
    int peek() override { return -1; }
    void flush() override {}
    size_t write(uint8_t) override { return 0; }
    size_t write(const uint8_t* buffer, const size_t size) override { return size; }
};

inline EmptyStream emptyStream{};

class RandomStream final : public Stream
{
public:
    int available() override { return 1; }
    int read() override { return static_cast<int>(random(0, 256)); }
    int peek() override { return static_cast<int>(random(0, 256)); }

    void flush() override
    {
    }

    size_t write(uint8_t) override { return 1; }
};

inline RandomStream randomStream{};

class WdClient final : public HttpClient
{
public:
    bool valid;

    WdClient(Client& client, const String& host, const uint16_t port) : HttpClient(client, host, port), valid(true)
    {
    }

    explicit WdClient(const HttpClient& other) : HttpClient(other), valid(true)
    {
    }

    // Safe “invalid client”
    WdClient() : HttpClient(*(Client*)nullptr, "", 0), valid(false)
    {
    }

    size_t write(const uint8_t b) override
    {
        if (!valid) return 0;
        WatchdogHandler::taskWDTReset();
        return HttpClient::write(b);
    }

    size_t write(const uint8_t* buf, const size_t size) override
    {
        if (!valid) return 0;
        WatchdogHandler::taskWDTReset();
        return HttpClient::write(buf, size);
    }

    int read() override
    {
        if (!valid) return -1;
        WatchdogHandler::taskWDTReset();
        return HttpClient::read();
    }

    int read(uint8_t* buf, const size_t size) override
    {
        if (!valid) return -1;
        WatchdogHandler::taskWDTReset();
        return HttpClient::read(buf, size);
    }

    unsigned int readBytes(char* buf, const size_t size) override
    {
        if (!valid) return 0;
        WatchdogHandler::taskWDTReset();
        return HttpClient::readBytes(buf, size);
    }
};

inline WdClient emptyClient{};

class HttpBase
{
public:
    std::map<String, String> headers;
    size_t bodyLength;

    HttpBase(std::map<String, String> headers, const size_t bodyLength)
        : headers(std::move(headers)), bodyLength(bodyLength)
    {
    }
};

class HttpResponse : public HttpBase
{
public:
    WdClient& body;
    const int responseCode;

    HttpResponse(const int responseCode, std::map<String, String> headers, WdClient& body, const size_t length)
        : HttpBase(std::move(headers), length), body(body), responseCode(responseCode)
    {
    }

    HttpResponse(const int responseCode, WdClient& body, const size_t length)
        : HttpBase({}, length), body(body), responseCode(responseCode)
    {
    }
};

class HttpRequest : public HttpBase
{
public:
    Stream& body;
    const char* path;
    ApiHttpMethod method;

    HttpRequest(const char* path, const ApiHttpMethod method, std::map<String, String> headers, Stream& body,
                const size_t bodyLength)
        : HttpBase(std::move(headers), bodyLength), body(body), path(path), method(method)
    {
        this->headers["Content-Length"] = String(bodyLength);
    }

    static HttpRequest get(const char* path, std::map<String, String> headers = {})
    {
        return HttpRequest{path, GET, std::move(headers), emptyStream, 0};
    }

    static HttpRequest post(const char* path, Stream& body, const size_t size, std::map<String, String> headers = {})
    {
        return HttpRequest{path, POST, std::move(headers), body, size};
    }
};

class ApiResponse : public HttpResponse
{
public:
    uint32_t uploadTimeMs;
    const bool valid;

    ApiResponse(const int code, std::map<String, String> headers, WdClient& body, const size_t length,
                const uint32_t uploadTimeMs, const bool valid)
        : HttpResponse(code, std::move(headers), body, length), uploadTimeMs(uploadTimeMs), valid(valid)
    {
    }

    ApiResponse(const int code, std::map<String, String> headers, WdClient& body, const size_t length,
                const uint32_t uploadTimeMs)
        : HttpResponse(code, std::move(headers), body, length), uploadTimeMs(uploadTimeMs), valid(true)
    {
    }

    static ApiResponse failed()
    {
        return ApiResponse{0, {}, emptyClient, 0, 0, false};
    }

    static ApiResponse empty(const int code, std::map<String, String> headers, const uint32_t uploadTimeMs)
    {
        return ApiResponse{code, std::move(headers), emptyClient, 0, uploadTimeMs};
    }
};
