#include "util/Digest.h"

#include <mbedtls/base64.h>
#include <mbedtls/md5.h>

bool Digest::md5File(File file, uint8_t out[16])
{
    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);

    uint8_t buf[512];
    while (file.available())
    {
        const size_t len = file.read(buf, sizeof(buf));
        mbedtls_md5_update_ret(&ctx, buf, len);
    }

    mbedtls_md5_finish_ret(&ctx, out);
    mbedtls_md5_free(&ctx);
    file.close();
    return true;
}

String Digest::toBase64(const uint8_t* data, const size_t len)
{
    unsigned char encoded[64]; // plenty for a 16-byte MD5
    size_t outLen = 0;

    mbedtls_base64_encode(encoded, sizeof(encoded), &outLen, data, len);
    encoded[outLen] = '\0';

    return {reinterpret_cast<char*>(encoded)};
}
