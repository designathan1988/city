// Ordered detail levels with the same complementary hysteresis for live people and retained bodies.
#pragma once
namespace city {
template <typename LevelAt>
int held_detail_level(float distance, int previous, float hold, LevelAt &&at) {
	const int now = at(distance);
	if (previous < 0 || previous == now) return now;
	return now > previous ? (at(distance / (1 + hold)) > previous ? now : previous) :
		(at(distance * (1 + hold)) < previous ? now : previous);
}
} // namespace city
