#pragma once

#include "config/hw_config.h"
#include "domain/LocalConfig.h"

/// Everything that has to be up before the object graph can be built: serial,
/// the filesystem, the loggers, and the two pieces of configuration the rest of
/// the firmware is constructed from.
namespace Platform
{
    /// Serial, watchdogs, LittleFS and both log sinks, in that order. Nothing
    /// before this returns can be logged to flash.
    void begin();

    /// Compile-time when HW_REV is set, otherwise prompts over serial and blocks
    /// until a known revision is entered.
    const BoardConfig& selectBoard();

    /// Stored config, or the compiled-in default, or a serial prompt.
    LocalConfig loadConfig();
}
