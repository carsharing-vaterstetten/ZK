#pragma once

#include <cstdint>
#include <FS.h>
#include <WString.h>

namespace Digest
{
    /// Streams the file through MD5 and closes it. `out` receives 16 bytes.
    bool md5File(File file, uint8_t out[16]);

    String toBase64(const uint8_t* data, size_t len);
}
