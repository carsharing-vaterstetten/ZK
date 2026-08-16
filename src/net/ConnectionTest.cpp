#include "net/ConnectionTest.h"

#include "config/Backend.h"
#include "logging/Loggers.h"
#include "net/ApiClient.h"

void ConnectionTest::run(ApiClient& api, const size_t fileSize)
{
    logger.infoln("Performing connection speed test");

    const HttpRequest req = HttpRequest::post(CONNECTION_SPEED_TEST_ENDPOINT, randomStream, fileSize);
    const ApiResponse resp = api.makeRequest(req, true);

    if (!resp.valid)
    {
        logger.errorln("Request failed");
        return;
    }

    logger.infoln("Response code: " + String(resp.responseCode));

    if (resp.responseCode != 200)
    {
        logger.errorln("Unexpected status code");
        return;
    }

    // Integer division by zero traps on Xtensa, and a transfer that returns
    // instantly is exactly the case where the measurement is worthless anyway.
    if (resp.uploadTimeMs == 0)
        logger.warningln("Upload completed too fast to measure");
    else
        logger.infoln("Upload test complete. Estimated speed: " +
            String(static_cast<uint64_t>(fileSize) * 1000 / resp.uploadTimeMs) + " B/s");

    const ulong downloadStartMs = millis();
    const uint downloadedBytes = api.fetch(resp, emptyStream);
    const ulong downloadTimeMs = millis() - downloadStartMs;

    if (downloadTimeMs == 0)
        logger.warningln("Download completed too fast to measure");
    else
        logger.infoln("Download test complete. Estimated speed: " +
            String(static_cast<uint64_t>(downloadedBytes) * 1000 / downloadTimeMs) + " B/s");
}
