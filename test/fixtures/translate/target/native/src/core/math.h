// Fixture: the target's value type the spec maps Vec2 to.
#pragma once

#include <cmath>

namespace city {

struct V2 {
	float x = 0, y = 0;
	V2() = default;
	V2(float px, float py) : x(px), y(py) {}
};

} // namespace city
