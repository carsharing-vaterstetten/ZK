#pragma once

#include "logging/Log.h"
#include "logging/SwappableFile.h"

inline Log serialOnlyLog{};
inline Log fileLog{};
inline SwappableFile swLog{"/primary.log", "/secondary.log"};
