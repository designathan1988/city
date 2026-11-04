// A table of the day (HOURS entries, one an hour from midnight) read at an hour: linear between two hours, the day wrapping round.
#pragma once

#include "sim/catalog/traffic_params.h"

#include <cmath>

namespace city {
namespace traffic {

inline float at_hour(const float *table, float hour) {
	const float h = std::fmod(std::fmod(hour, float(HOURS)) + float(HOURS), float(HOURS)), u = h - std::floor(h);
	const int i = int(h);
	return table[i] * (1 - u) + table[(i + 1) % HOURS] * u;
}

} // namespace traffic
} // namespace city
