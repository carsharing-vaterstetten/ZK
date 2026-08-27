#pragma once

/// Surfaces the ESP-IDF core dump the panic handler already writes to flash on
/// every crash (Guru Meditation, assert, or a watchdog timeout with panic
/// enabled — see system/Watchdog.h). The write side needs no code of ours: the
/// vendored SDK has CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH on by default and the
/// default partition table already reserves the space. This is only the read
/// side — turning whatever is sitting there into a normal log line.
namespace CrashLog
{
    /// Call once, early in boot, after a flash log sink exists. If the previous
    /// boot left a core dump behind, logs a summary (crashing task, program
    /// counter, exception cause, raw backtrace addresses) through the normal
    /// logger — so it rides the existing upload pipeline with no new plumbing —
    /// then erases the dump. Erasing matters: without it, the same crash would
    /// get logged again on every subsequent boot instead of once.
    ///
    /// The backtrace addresses aren't symbolized on-device; decode them later
    /// with `addr2line -e firmware.elf <addr>...` against the exact build that
    /// crashed — the logged SHA-256 identifies which one that is.
    void logAndClearPending();
}
