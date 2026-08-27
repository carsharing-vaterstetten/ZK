#include "util/CrashLog.h"

#include <esp_core_dump.h>
#include <xtensa/corebits.h>

#include "logging/Loggers.h"

namespace
{
    /// The handful of causes worth naming outright — covers every crash class
    /// already found in this codebase (unaligned packed-struct writes, a
    /// divide-by-zero, a bad pointer). Anything else is logged as the raw
    /// EXCCAUSE number, which is enough to look up in corebits.h.
    String describeExcCause(const uint32_t cause)
    {
        switch (cause)
        {
        case EXCCAUSE_ILLEGAL: return "illegal instruction";
        case EXCCAUSE_INSTR_ERROR: return "instruction fetch error";
        case EXCCAUSE_LOAD_STORE_ERROR: return "load/store error";
        case EXCCAUSE_DIVIDE_BY_ZERO: return "divide by zero";
        case EXCCAUSE_PRIVILEGED: return "privileged instruction";
        case EXCCAUSE_UNALIGNED: return "unaligned load/store";
        case EXCCAUSE_LOAD_PROHIBITED: return "load from prohibited address (null/bad pointer?)";
        case EXCCAUSE_STORE_PROHIBITED: return "store to prohibited address (null/bad pointer?)";
        default: return "cause " + String(cause);
        }
    }
}

void CrashLog::logAndClearPending()
{
    if (esp_core_dump_image_check() != ESP_OK) return; // nothing waiting

    esp_core_dump_summary_t summary{};

    if (esp_core_dump_get_summary(&summary) == ESP_OK)
    {
        String backtrace;
        const uint32_t depth = std::min<uint32_t>(summary.exc_bt_info.depth,
                                                   sizeof(summary.exc_bt_info.bt) / sizeof(summary.exc_bt_info.bt[0]));

        for (uint32_t i = 0; i < depth; ++i)
            backtrace += "0x" + String(summary.exc_bt_info.bt[i], HEX) + " ";

        if (summary.exc_bt_info.corrupted)
            backtrace += "(corrupted)";

        logger.criticalln(
            "Previous boot crashed — task '" + String(summary.exc_task) +
            "' at PC 0x" + String(summary.exc_pc, HEX) +
            ", " + describeExcCause(summary.ex_info.exc_cause) +
            " at address 0x" + String(summary.ex_info.exc_vaddr, HEX) +
            ", build " + String(reinterpret_cast<const char*>(summary.app_elf_sha256)) +
            ", backtrace: " + backtrace);
    }
    else
    {
        logger.criticalln("Previous boot left a core dump behind, but it could not be read");
    }

    // Without this, the same crash gets logged again on every boot from here
    // on rather than once.
    esp_core_dump_image_erase();
}
