// Physical and calendar units. The only place these conversion factors are written.
#pragma once

namespace city {

constexpr double MINUTES_PER_HOUR = 60.0;
constexpr double HOURS_PER_DAY = 24.0;
constexpr double MINUTES_PER_DAY = MINUTES_PER_HOUR * HOURS_PER_DAY;
constexpr double METRES_PER_KM = 1000.0;
constexpr double NS_PER_S = 1e9;
constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.f;

} // namespace city
