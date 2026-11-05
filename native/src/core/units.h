// Physical and calendar units. The only place these conversion factors are written.
#pragma once

#include "core/math.h"

namespace city {

constexpr double MINUTES_PER_HOUR = 60.0;
constexpr double SECONDS_PER_HOUR = 3600.0;
constexpr double HOURS_PER_DAY = 24.0;
constexpr double MINUTES_PER_DAY = MINUTES_PER_HOUR * HOURS_PER_DAY;
constexpr double METRES_PER_KM = 1000.0;
constexpr float KMH_PER_MS = 3.6f; // km/h in one m/s
constexpr double NS_PER_S = 1e9;
constexpr double NS_PER_MS = 1e6;
constexpr double BYTES_PER_MIB = 1024.0 * 1024.0;
constexpr double DEG_PER_TURN = 360.0;
constexpr double DEG_PER_HALF_TURN = DEG_PER_TURN / 2;
constexpr float DEG_TO_RAD = PI_F / float(DEG_PER_HALF_TURN);
constexpr float RAD_TO_DEG = float(DEG_PER_HALF_TURN) / PI_F;

} // namespace city
