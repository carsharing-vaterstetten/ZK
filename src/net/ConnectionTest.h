#pragma once

#include <cstddef>

class ApiClient;

namespace ConnectionTest
{
    /// Uploads then downloads `fileSize` bytes and logs the throughput. Purely a
    /// diagnostic; results go to the log, not to the caller.
    void run(ApiClient& api, size_t fileSize);
}
