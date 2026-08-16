#include "tasks/KeyControlTask.h"

#include "logging/Loggers.h"

bool KeyControlTask::send(const KeyControlCommand cmd)
{
    if (xQueueSend(cmdQueue, &cmd, enqueueTimeout) == pdTRUE)
        return true;

    logger.errorln("Key control queue full, command dropped");
    return false;
}

bool KeyControlTask::lock()
{
    return send(KeyControlCommand::Lock);
}

bool KeyControlTask::unlock()
{
    return send(KeyControlCommand::Unlock);
}

void KeyControlTask::setup() {}

void KeyControlTask::run()
{
    KeyControlCommand cmd;

    while (isRunning())
    {
        if (xQueueReceive(cmdQueue, &cmd, pdMS_TO_TICKS(500)) == pdFALSE)
            continue;

        switch (cmd)
        {
        case KeyControlCommand::Lock:
            carKey.lock();
            break;
        case KeyControlCommand::Unlock:
            carKey.unlock();
            break;
        }
    }
}
