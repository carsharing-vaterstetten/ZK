#include "SystemWatchTask.h"

#include <esp_heap_caps.h>
#include <LittleFS.h>
#include <esp_system.h>

#include "shared/Globals.h"

void SystemWatchTask::OnCommand(SystemCommand cmd)
{
    switch (cmd)
    {
    case SystemCommand::None:
        break;
    case SystemCommand::PrepareForHotRestart:
        m_running = false;
        break;
    case SystemCommand::EnterLowPower:
        break;
    case SystemCommand::ResumeNormalOperation:
        break;
    }
}

void SystemWatchTask::log()
{
    auto tasks = SystemManager::getAllTasks();

    String line;

    // Uptime
    line += "U:";
    line += millis() / 1000;

    // Heap
    line += " H:";
    line += ESP.getFreeHeap();
    line += "/";
    line += ESP.getHeapSize();
    line += "B";

    // Largest block
    line += " L:";
    line += heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    line += "B";

    // PSRAM
    if (psramFound())
    {
        line += " PS:";
        line += ESP.getFreePsram();
        line += "/";
        line += ESP.getPsramSize();
        line += "B";
    }

    // Number of tasks
    line += " T:";
    line += uxTaskGetNumberOfTasks();

    line += " | ";

    // Per task stack usage
    for (const auto* task : tasks)
    {
        if (!task)
            continue;

        TaskHandle_t h = task->getTaskHandle();
        if (!h)
            continue;

        line += task->getName();
        line += ":";

        // stack remaining
        line += uxTaskGetStackHighWaterMark(h) * sizeof(StackType_t);
        line += "B ";
    }

    // Filesystem
    if (LittleFS.begin(true))
    {
        size_t total = LittleFS.totalBytes();
        size_t used = LittleFS.usedBytes();

        line += "| FS:";
        line += used;
        line += "/";
        line += total;
        line += "B";
    }

    fileLog.infoln(line);
}

void SystemWatchTask::setup() {}

void SystemWatchTask::run()
{
    log();
    TickType_t lastLogTime = xTaskGetTickCount();

    while (m_running)
    {
        const TickType_t now = xTaskGetTickCount();

        if (now - lastLogTime > reportingFrequency)
        {
            log();
            lastLogTime = now;
        }

        vTaskDelay(checkingFreq);
    }
}