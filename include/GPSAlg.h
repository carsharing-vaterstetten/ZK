#pragma once
#include "GPS.h"
#include <optional>
#include <stdexcept>

enum class GPSAlgPrediction
{
    Standing,
    Moving,
};

class GPSAlg
{
protected:
    std::vector<GPS_DATA_t> data_buffer;

    // --- Thresholds ---
    static constexpr float SPEED_THRESHOLD_KMH     = 2.0f;
    static constexpr float DISPLACEMENT_THRESHOLD_M = 0.1f;
    static constexpr float MAX_ACCEPTABLE_ACCURACY  = 5.0f;

    // --- Timing ---
    float    eval_window_secs;  // e.g. 10.0f seconds

    // --- Trip state ---
    bool                      trip_active     = false;
    float                     trip_distance_m = 0.0f;
    std::optional<GPS_DATA_t> last_trip_sample;

    // Minimum fraction of votes needed to flip state (0.5 = simple majority)
    static constexpr float MOVING_VOTE_THRESHOLD  = 0.4f;
    static constexpr float STANDING_VOTE_THRESHOLD = 0.7f;

    GPSAlgPrediction last_prediction = GPSAlgPrediction::Standing;

    static float haversineDistance(float lat1, float lon1, float lat2, float lon2);
    GPSAlgPrediction evaluateWindow() const;
    bool isSampleReliable(const GPS_DATA_t& sample) const;
    void accumulateTripDistance(const GPS_DATA_t& sample);

public:
    // eval_window_secs : how many seconds of history to evaluate (default 10s)
    explicit GPSAlg(float eval_window_secs = 10.0f);

    GPSAlgPrediction pushData(const GPS_DATA_t& data);

    void  startTrip();
    float endTrip();

    bool  isTripActive()    const { return trip_active; }
    float getTripDistance() const { return trip_distance_m; }

    static String gpsAlgPredictionToStr(const GPSAlgPrediction pred)
    {
        switch (pred)
        {
        case GPSAlgPrediction::Standing:
            return "Standing";
        case GPSAlgPrediction::Moving:
            return "Moving";
        }

        return "Unknown";
    }
};
