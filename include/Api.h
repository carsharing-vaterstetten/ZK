#pragma once

#include <optional>
#include <WString.h>

#include "ApiStreams.h"

class WdClient;
class Modem;

class ApiClient
{
protected:
    bool isReady = false;
    WdClient* httpClient = nullptr;
    std::optional<String> defaultBasicAuthUsername = std::nullopt, defaultBasicAuthPassword = std::nullopt;
    size_t writeBufferSize, readBufferSize;

public:
    explicit ApiClient(size_t writeBufferSize = 512, size_t readBufferSize = 1460);

    void begin(const String& server, uint16_t port, const String& username, const String& password);
    [[nodiscard]] ApiResponse makeRequest(const HttpRequest& request, bool ignoreResponseHeaders = false,
                                          ulong timeout = 5 * 60) const;
    uint fetch(const ApiResponse& resp, Stream& destination, ulong timeout = 5 * 60) const;
};
