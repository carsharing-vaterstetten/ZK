#pragma once

#include <mutex>
#include <optional>

#include "domain/GpsLog.h"

enum class MotionState
{
    Standing,
    Moving,
};

class TripTracker
{
protected:
    std::vector<GPS_DATA_t> data_buffer;

    // --- Thresholds ---
    static constexpr float SPEED_THRESHOLD_KMH = 2.0f;
    static constexpr float DISPLACEMENT_THRESHOLD_M = 0.1f;
    static constexpr float MAX_ACCEPTABLE_ACCURACY = 3.0f;

    // --- Timing ---
    float eval_window_secs; // e.g. 10.0f seconds

    // --- Trip state ---
    bool trip_active = false;
    float trip_distance_m = 0.0f;
    std::optional<GPS_DATA_t> last_trip_sample;

    // Minimum fraction of votes needed to flip state (0.5 = simple majority)
    static constexpr float MOVING_VOTE_THRESHOLD = 0.4f;
    static constexpr float STANDING_VOTE_THRESHOLD = 0.7f;

    MotionState last_prediction = MotionState::Standing;

    mutable std::recursive_mutex algMutex;

    static float haversineDistance(float lat1, float lon1, float lat2, float lon2);
    MotionState evaluateWindow() const;
    static bool isSampleReliable(const GPS_DATA_t& sample);
    void accumulateTripDistance(const GPS_DATA_t& sample);

public:
    // eval_window_secs : how many seconds of history to evaluate (default 10s)
    explicit TripTracker(float eval_window_secs = 10.0f);

    MotionState pushData(const GPS_DATA_t& data);

    void startTrip();
    float endTrip();

    bool isTripActive() const { return trip_active; }

    static String motionStateToString(const MotionState pred)
    {
        switch (pred)
        {
        case MotionState::Standing:
            return "Standing";
        case MotionState::Moving:
            return "Moving";
        }

        return "Unknown";
    }
};
