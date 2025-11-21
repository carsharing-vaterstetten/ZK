#pragma once
#include <utility>

#include "ApiStreams.h"
#include "LocalConfig.h"
#include "Modem.h"


class ApiClient
{
protected:
    bool isReady = false;

public:
    std::optional<WdClient> httpClient;
    Modem* modem = nullptr;
    std::optional<String> defaultBasicAuthUsername = std::nullopt, defaultBasicAuthPassword = std::nullopt;

    ApiClient() : httpClient(std::nullopt)
    {
    }

    void begin(const String& server, uint16_t port, Modem& m, String username, String password)
    {
        modem = &m;
        httpClient.emplace(modem->gsmClient, server, port);
        defaultBasicAuthUsername = std::move(username);
        defaultBasicAuthPassword = std::move(password);
        isReady = true;
    }

    ApiResponse makeRequest(HttpRequest& request);
    static uint32_t fetch(const ApiResponse& resp, Stream& destination);
};

inline ApiClient api;
