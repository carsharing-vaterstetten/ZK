#include "GPSAlg.h"
#include <cmath>
#include <stdexcept>

static constexpr float EARTH_RADIUS_M = 6'371'000.0f;

// ─── Constructor ─────────────────────────────────────────────────────────────

GPSAlg::GPSAlg(float eval_window_secs)
    : eval_window_secs(eval_window_secs)
{
    if (eval_window_secs <= 0.0f)
        throw std::invalid_argument("GPSAlg: eval_window_secs must be > 0.");
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

float GPSAlg::haversineDistance(float lat1, float lon1, float lat2, float lon2)
{
    const float dLat = (lat2 - lat1) * DEG_TO_RAD;
    const float dLon = (lon2 - lon1) * DEG_TO_RAD;

    const float a = std::sin(dLat / 2.0f) * std::sin(dLat / 2.0f)
                  + std::cos(lat1 * DEG_TO_RAD) * std::cos(lat2 * DEG_TO_RAD)
                  * std::sin(dLon / 2.0f) * std::sin(dLon / 2.0f);

    return EARTH_RADIUS_M * 2.0f * std::atan2(std::sqrt(a), std::sqrt(1.0f - a));
}

bool GPSAlg::isSampleReliable(const GPS_DATA_t& sample) const
{
    if (sample.accuracy > MAX_ACCEPTABLE_ACCURACY)    return false;
    if (sample.accuracy < 0.0f)                       return false;
    if (sample.usat < 3)                              return false;
    if (sample.speed > 300.0f)                        return false;
    if (sample.alt > 9000.0f)                         return false;
    if (sample.alt < -500.0f)                         return false;
    if (sample.lat == 0.0f && sample.lon == 0.0f)     return false;
    if (sample.lat >  90.0f || sample.lat <  -90.0f)  return false;
    if (sample.lon > 180.0f || sample.lon < -180.0f)  return false;
    if (sample.unixTimestamp == 0)                    return false;

    return true;
}

// ─── Trip distance accumulation ──────────────────────────────────────────────

void GPSAlg::accumulateTripDistance(const GPS_DATA_t& sample)
{
    if (!trip_active || !isSampleReliable(sample))
        return;

    if (last_trip_sample.has_value())
    {
        const GPS_DATA_t& prev = last_trip_sample.value();
        const uint64_t    dt   = sample.unixTimestamp - prev.unixTimestamp;

        if (dt == 0 || dt > 60)
        {
            // Duplicate or gap too large - anchor here but don't count distance
            last_trip_sample = sample;
            return;
        }

        const float dist = haversineDistance(prev.lat, prev.lon, sample.lat, sample.lon);
        if (dist >= DISPLACEMENT_THRESHOLD_M)
            trip_distance_m += dist;
    }

    last_trip_sample = sample;
}

// ─── Movement detection ──────────────────────────────────────────────────────

GPSAlgPrediction GPSAlg::evaluateWindow() const
{
    if (data_buffer.empty()) return GPSAlgPrediction::Standing;

    const uint64_t newest = data_buffer.back().unixTimestamp;
    const uint64_t cutoff = newest - static_cast<uint64_t>(eval_window_secs);

    uint32_t movingVotes = 0, standingVotes = 0;

    const GPS_DATA_t* prev = nullptr;
    for (const auto& sample : data_buffer)
    {
        if (sample.unixTimestamp < cutoff) { prev = &sample; continue; }
        if (!isSampleReliable(sample))     { prev = &sample; continue; }

        // Signal 1: speed
        if (sample.speed >= SPEED_THRESHOLD_KMH)
            ++movingVotes;
        else
            ++standingVotes;

        // Signal 2: displacement
        if (prev && isSampleReliable(*prev))
        {
            const uint64_t dt = sample.unixTimestamp - prev->unixTimestamp;
            if (dt > 0 && dt <= 60)
            {
                const float dist = haversineDistance(prev->lat, prev->lon, sample.lat, sample.lon);
                if (dist >= DISPLACEMENT_THRESHOLD_M)
                    ++movingVotes;
                else
                    ++standingVotes;
            }
        }

        prev = &sample;
    }

    return (movingVotes > standingVotes) ? GPSAlgPrediction::Moving
                                         : GPSAlgPrediction::Standing;
}

// ─── Public API ──────────────────────────────────────────────────────────────

GPSAlgPrediction GPSAlg::pushData(const GPS_DATA_t& data)
{
    data_buffer.push_back(data);
    accumulateTripDistance(data);
    return evaluateWindow();
}

void GPSAlg::startTrip()
{
    if (trip_active)
        throw std::logic_error("GPSAlg: cannot start a trip - one is already active.");

    trip_active      = true;
    trip_distance_m  = 0.0f;
    last_trip_sample = std::nullopt;
}

float GPSAlg::endTrip()
{
    if (!trip_active)
        throw std::logic_error("GPSAlg: cannot end a trip - no trip is active.");

    trip_active      = false;
    last_trip_sample = std::nullopt;

    return trip_distance_m;
}