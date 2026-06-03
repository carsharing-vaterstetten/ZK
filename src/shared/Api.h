#pragma once

#include <mutex>
#include <utility>
#include <WString.h>

#include "ImeiStore.h"
#include "../abstract/ApiStreams.h"

class LocalConfig;
class WdClient;
class Modem;

class ApiClient
{
public:
    explicit ApiClient(HttpClient& httpClient, const ImeiStore& imeiStore, const String& serverPassword) :
        imeiStore(imeiStore), serverPassword(serverPassword), httpClient(httpClient) {}

    [[nodiscard]] ApiResponse makeRequest(const HttpRequest& request, bool ignoreResponseHeaders = false,
                                          ulong timeout = 5 * 60, bool addUsernameAndPassword = true) const;
    static uint fetch(const ApiResponse& resp, Stream& destination, ulong timeout = 5 * 60, size_t bufferSize = 512);

private:
    mutable std::mutex mtx;

    const ImeiStore& imeiStore;
    const String& serverPassword;
    HttpClient& httpClient;
};
