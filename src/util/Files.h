#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <LittleFS.h>

namespace Files
{
    bool move(const String& oldPath, const String& newPath, bool deleteIfNewExists = false);
    bool remove(const String& path, bool notExistingOk = true);
}
