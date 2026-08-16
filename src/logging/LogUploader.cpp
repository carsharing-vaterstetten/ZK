#include "logging/LogUploader.h"

#include <LittleFS.h>

#include "config/Backend.h"
#include "hal/Modem.h"
#include "logging/SwappableFile.h"

namespace
{
    void upload(ApiClient& api, SwappableFile& swLog, const bool deleteIfSuccess,
                const bool deleteAfterRetrying, const unsigned retries)
    {
        const std::optional<FileInfo> fileInfo = swLog.getCurrentFileInfo();
        if (!fileInfo.has_value()) return;

        // Logging moves to the spare file for the duration of the upload, which
        // over CAT-M can be minutes.
        swLog.swapToB();

        Modem::uploadFileAndDelete(api, LOG_FILE_UPLOAD_ENDPOINT, fileInfo->path.c_str(), deleteIfSuccess,
                                   deleteAfterRetrying, retries);

        swLog.appendBToAAndSwapToA();
    }
}

void LogUploader::uploadAndRotate(ApiClient& api, SwappableFile& swLog, const unsigned retries,
                                  const bool deleteIfSuccess)
{
    const size_t total = LittleFS.totalBytes();
    const size_t freeBytes = total - LittleFS.usedBytes();

    // Only discard an unsent log when flash is nearly full *and* the log is the
    // thing filling it.
    bool deletePressure = freeBytes < total / 10;

    if (const std::optional<FileInfo> logInfo = swLog.getCurrentFileInfo())
        deletePressure = deletePressure && logInfo->size > total / 5;

    upload(api, swLog, deleteIfSuccess, deletePressure, retries);
}
