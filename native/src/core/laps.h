// Milliseconds between laps of the steady clock: the sections of the frame probes (api/city_core_probe.cpp).
#pragma once

#include <chrono>

namespace city {

struct Laps {
	std::chrono::steady_clock::time_point at = std::chrono::steady_clock::now();
	double lap() {
		const auto now = std::chrono::steady_clock::now();
		const double ms = std::chrono::duration<double, std::milli>(now - at).count();
		at = now;
		return ms;
	}
};

} // namespace city
