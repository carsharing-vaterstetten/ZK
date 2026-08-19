#pragma once

/// Thin wrapper over the ESP-IDF Task Watchdog Timer (TWDT).
///
/// Deliberately scoped to the handful of tasks whose hang would either strand
/// a user at the car or leave the device unable to recover on its own —
/// AccessControlTask, CardReaderTask, KeyControlTask and ModemTask — not a
/// general "watch everything" facility. See SystemThread's watchdogCritical
/// constructor flag, which is what actually subscribes a task.
namespace Watchdog
{
    /// Starts the TWDT at the generous startup timeout (HW_WATCHDOG_INITIAL_-
    /// STARTUP_TIMEOUT) — sized to comfortably cover a cold modem boot: baud
    /// scanning, network attach, first RFID/firmware sync. Must run after the
    /// scheduler has started (i.e. from within setup(), not before) and before
    /// any critical task subscribes.
    void begin();

    /// Drops to the tighter HW_WATCHDOG_DEFAULT_TIMEOUT once boot has finished.
    /// Safe to call more than once; only the first call after begin() changes
    /// anything.
    void enterSteadyState();

    /// Subscribes the calling task. Meant to be called once, by SystemThread,
    /// on behalf of a task constructed with watchdogCritical = true.
    void watchCurrentTask();

    /// Unsubscribes the calling task, e.g. as it exits cleanly during shutdown
    /// — a task that has stopped running must not still be expected to reset
    /// the timer, or the shutdown handshake would race the watchdog itself.
    void unwatchCurrentTask();

    /// Resets the timer on the calling task's behalf. Safe to call from a task
    /// that was never subscribed — it is just a no-op there.
    void feed();
}
