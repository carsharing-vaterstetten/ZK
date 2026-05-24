#pragma once

#include <WString.h>

#include "../abstract/ApiStreams.h"

class WdClient;
class Modem;

class ApiClient
{
protected:
    HttpClient& httpClient;
    String defaultBasicAuthUsername, defaultBasicAuthPassword;
    size_t writeBufferSize = 512, readBufferSize = 1460;

public:
    explicit ApiClient(
        HttpClient& httpClient,
        String defaultBasicAuthUsername = "",
        String defaultBasicAuthPassword = "",
        size_t writeBufferSize = 512,
        size_t readBufferSize = 1460
    );

    [[nodiscard]] ApiResponse makeRequest(const HttpRequest& request, bool ignoreResponseHeaders = false,
                                          ulong timeout = 5 * 60) const;
    uint fetch(const ApiResponse& resp, Stream& destination, ulong timeout = 5 * 60) const;
};
