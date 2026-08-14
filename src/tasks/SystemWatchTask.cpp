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
        // Otherwise shutdown would wait out a full reporting interval.
        if (m_taskHandle != nullptr) xTaskNotifyGive(m_taskHandle);
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
    line.reserve(320);

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

    // Filesystem. Already mounted in setup() — re-running begin() here just makes
    // LittleFS log a warning, which routes straight back into this logger.
    const size_t total = LittleFS.totalBytes();

    if (total > 0)
    {
        line += "| FS:";
        line += LittleFS.usedBytes();
        line += "/";
        line += total;
        line += "B";
    }

    fileLog.infoln(line);
}

void SystemWatchTask::setup() {}

void SystemWatchTask::run()
{
    while (m_running)
    {
        log();
        ulTaskNotifyTake(pdTRUE, reportingFrequency);
    }

    SystemManager::ReportReadyForRestart(m_id);
}
