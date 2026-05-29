#include "SharedData.h"

#include "tasks/AccessControlTask.h"


void* SharedData::receiveDataFromCommand(const CommandId id, const TickType_t maxTime)
{
    void* data = nullptr;

    TickType_t startTime = xTaskGetTickCount();

    for (TickType_t passedTime = 0; maxTime > passedTime; passedTime = xTaskGetTickCount() - startTime)
    {
        xSemaphoreTake(registryMutex, maxTime);
        if (dataRegistry.count(id) > 0)
            data = dataRegistry[id];
        xSemaphoreGive(registryMutex);

        if (data != nullptr)
            return data;

        passedTime = xTaskGetTickCount() - startTime;

        if (passedTime >= maxTime) return nullptr;

        // sleep here (0% CPU) until *any* data gets registered.
        // Once xEventGroupSetBits is called by a sender, this task instantly wakes up
        // and loops back up to check the map again.
        xEventGroupWaitBits(registryEvent, 1, pdTRUE, pdFALSE, maxTime - passedTime);
    }

    return nullptr;
}

void SharedData::registerData(const CommandId id, void* data)
{
    xSemaphoreTake(registryMutex, portMAX_DELAY);
    dataRegistry[id] = data;
    xSemaphoreGive(registryMutex);

    xEventGroupSetBits(registryEvent, 1);
}
