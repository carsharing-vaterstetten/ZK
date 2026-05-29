#pragma once
#include <map>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include "services/ServiceTypese.h"

class SharedData
{
public:
    static void* receiveDataFromCommand(CommandId id, TickType_t maxTime = portMAX_DELAY);
    static void registerData(CommandId id, void* data);

private:
    inline static std::map<CommandId, void*> dataRegistry;
    static SemaphoreHandle_t registryMutex;   // To keep the map thread-safe
    static EventGroupHandle_t registryEvent;  // Single event group for everyone
};
