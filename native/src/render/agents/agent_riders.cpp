// Who rides in a vehicle the agent renderer draws (agent_renderer.h): how full the model runs, what the kerb stop or the bus's own
// count changes, and the seats they take, sent to PeopleRenderer.
#include "render/agents/agent_renderer.h"
#include "core/shader_contracts/agents.h"

#include "core/random.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// streams of a vehicle's id (any distinct word; the seats' is 2^32 over the golden ratio), and the byte a load's order is read past
constexpr uint32_t LOAD_SALT = 0x10adu, ORDER_SHIFT = 8, SEAT_SALT = 0x9e3779b1u;
constexpr uint64_t SEAT_KEYS = 128; // more than any vehicle's seats: a rider's key is the vehicle's and the seat's
} // namespace

void AgentRenderer::riders(int type, uint32_t id, float &passengers, float &order) const {
	const Load &l = load_[type];
	passengers = order = 0;
	if (l.seats == 0) return;
	const uint32_t h = hash_u32(id ^ LOAD_SALT);
	const float u = hash_float(h), v = hash_float(h + 1);
	size_t band = 0;
	while (band + 1 < l.bands.size() && u >= l.bands[band]) band++;
	// how full it runs: a share in its band, as whole passengers out of seats + 1 steps (none .. every seat)
	const float share = (float(band) + v) / float(l.bands.size());
	passengers = float(std::min(l.seats, int(share * float(l.seats + 1))));
	const int stride = l.strides[(h >> ORDER_SHIFT) % l.strides.size()], first = l.shuffle ? int((h >> (ORDER_SHIFT * 2)) % uint32_t(l.seats)) : 0;
	order = float(stride * l.seats + first);
}

float AgentRenderer::passengers_of(const AgentRecord &rec, int type, float &order) const {
	float passengers;
	riders(type, rec.id, passengers, order);
	const uint32_t aboard = (rec.look >> LOOK_ABOARD_SHIFT) & LOOK_ABOARD_MASK;
	if (aboard == ABOARD_TO_SET_DOWN) passengers = std::max(passengers, 1.f);
	else if (aboard == ABOARD_SET_DOWN) passengers = std::max(passengers, 1.f) - 1;
	else if (aboard == ABOARD_PICKED_UP) passengers += 1;
	const uint32_t counted = (rec.look >> LOOK_RIDERS_SHIFT) & LOOK_RIDERS_MASK; // a bus: who the simulation says rides in it
	if (counted != LOOK_RIDERS_AS_DRAWN) passengers = float(std::min(int(counted) - 1, load_[type].seats));
	if ((rec.look & (1u << LOOK_EMPTY_SHIFT)) != 0) passengers = NOBODY_ABOARD;
	return passengers;
}

void AgentRenderer::seat_people(uint32_t id, int type, int model, const Pose &p, float dist, float passengers, float order, float spin) {
	const int seats = load_[type].seats;
	const int stride = seats > 0 ? int(order) / seats : 0, first = seats > 0 ? int(order) - stride * seats : 0;
	const Odometer &o = odo_[size_t(id)];
	const float turn = spin / TAU_F;
	for (const SeatSpot &s : seats_[model]) {
		// passenger seat j is taken when (stride * j + first) mod seats < passengers, so exactly that many are
		if (s.order != DRIVER_SEAT && (seats == 0 || (stride * s.order + first) % seats >= int(passengers))) continue;
		const uint64_t seat = uint64_t(s.order - DRIVER_SEAT);
		people_.seat(uint64_t(id) * SEAT_KEYS + seat, p, s, hash_u32(id * SEAT_SALT + uint32_t(seat)), dist, turn - std::floor(turn), o.still >= STILL_FRAMES);
	}
}

} // namespace city
