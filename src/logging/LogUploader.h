#pragma once

#include <cstdint>

class ApiClient;
class SwappableFile;

namespace LogUploader
{
    /// Uploads the active log and swaps to the spare so logging continues during
    /// the transfer. Deletes the uploaded file even on failure only when flash is
    /// tight and the log is what is filling it.
    void uploadAndRotate(ApiClient& api, SwappableFile& swLog, unsigned retries = 2, bool deleteIfSuccess = true);
}
