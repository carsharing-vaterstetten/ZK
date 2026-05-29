#pragma once

#include "Log.h"
#include "SwappableFile.h"

inline Log serialOnlyLog{};
inline Log fileLog{};
inline SwappableFile swLog{"/primary.log", "/secondary.log"};
