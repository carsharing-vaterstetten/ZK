#pragma once

#include "ApiStreams.h"

class Modem;

class ApiClient
{
protected:
    bool isReady = false;
    WdClient* httpClient = nullptr;
    std::optional<String> defaultBasicAuthUsername = std::nullopt, defaultBasicAuthPassword = std::nullopt;

public:
    void begin(const String& server, uint16_t port, const String& username, const String& password);
    ApiResponse makeRequest(const HttpRequest& request) const;
    static uint32_t fetch(const ApiResponse& resp, Stream& destination);
};
