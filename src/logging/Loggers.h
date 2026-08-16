#pragma once

#include "logging/Log.h"
#include "logging/SwappableFile.h"

inline Log serialLogger{};
inline Log logger{};
inline SwappableFile swLog{"/primary.log", "/secondary.log"};
