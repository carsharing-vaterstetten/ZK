#pragma once
#include <map>
#include <Arduino.h>

#include "services/ServiceTypese.h"

class SharedData
{
public:
    static void* receiveDataFromCommand(CommandId id, TickType_t maxTime = portMAX_DELAY);
    static void registerData(CommandId id, void* data);

private:
    inline static std::map<CommandId, void*> dataRegistry;
    inline static SemaphoreHandle_t registryMutex = xSemaphoreCreateMutex();   // To keep the map thread-safe
    inline static EventGroupHandle_t registryEvent = xEventGroupCreate();  // Single event group for everyone
};
