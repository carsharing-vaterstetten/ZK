#pragma once

#include <atomic>
#include <mutex>
#include <utility>
#include <WString.h>

#include "ImeiStore.h"
#include "../abstract/ApiStreams.h"

class LocalConfig;
class WdClient;
class Modem;

enum class ApiClientState
{
    None,
    Uploading,
    Downloading,
};

struct ApiClientProgress
{
    size_t bytesProcessed;
    size_t bytesTotal;
};

class ApiClient
{
public:
    explicit ApiClient(HttpClient& httpClient, const ImeiStore& imeiStore, const String& serverPassword) :
        imeiStore(imeiStore), serverPassword(serverPassword), httpClient(httpClient) {}

    [[nodiscard]] ApiResponse makeRequest(const HttpRequest& request, bool ignoreResponseHeaders = false,
                                          ulong timeout = 5 * 60, bool addUsernameAndPassword = true);
    uint fetch(const ApiResponse& resp, Stream& destination, ulong timeout = 5 * 60);

    [[nodiscard]] ApiClientState getState() const
    {
        return state;
    }

    [[nodiscard]] ApiClientProgress getProgress() const
    {
        return progress;
    }

private:
    std::atomic<ApiClientState> state;
    std::atomic<ApiClientProgress> progress;

    const ImeiStore& imeiStore;
    const String& serverPassword;
    HttpClient& httpClient;
};
