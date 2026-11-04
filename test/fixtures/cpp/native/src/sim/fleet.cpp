// Fixture: definitions of fleet.h.
#include "sim/fleet.h"

#include <cmath>

namespace city {

float clampf(float v, float lo, float hi) {
	return v < lo ? lo : v > hi ? hi : v;
}

float Cruise::step(Car &c, float dt) {
	c.v = clampf(c.v + dt, 0, SPEED_LIMIT);
	return c.v;
}

void Fleet::tick(Mover &m, float dt) {
	for (Car &c : cars) {
		std::vector<float> trail;
		total += m.step(c, dt);
		for (Car &o : cars) {
			total += std::hypot(c.s - o.s, c.v - o.v);
		}
	}
	g_counter++;
}

void Driver::_process(double delta) {
	fleet.tick(cruise, float(delta));
}

float Fleet::drain() {
	float sum = 0;
	while (!queue.empty()) {
		sum += float(queue.front());
		queue.erase(queue.begin());
	}
	return sum;
}

} // namespace city
