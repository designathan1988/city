// Places to wait at the kerb of a crosswalk (SUMO's walking area before a crossing):
// the walkers about to cross stand on their own right half of the zebra, in columns kerb_wait_gap_m apart within
// kerb_wait_half_m of its middle line, front row first and further rows back from the kerb, as many rows as leave a
// walker its way along the sidewalk behind them; never on street furniture. The other half is the way of those
// arriving from the other kerb, so the two ways never meet in a crowd. A waiting walker steps forward when a place
// nearer the kerb frees, and crosses keeping its place's line (its own right half).
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int MAX_PLACES = 32; // bits of TrafficState::kerb_slots
constexpr float ZERO_SPAN_M = 1e-4f; // a way across shorter than this has no direction
constexpr int KERB_NODE_BITS = 32; // a kerb's key: its node above these bits, the arm crossed and which way below

struct Kerb {
	V2 d, right; // across the road from this kerb, and its right
	int64_t key = -1;
	int cols = 1, places = 0;
	float gap = 0;
};

// The kerb `w` heads for (ways[wi]) before crossing to ways[wi + 1]: its places and their key.
bool kerb_of(const CityState &s, const Pedestrian &w, Kerb &k) {
	if (w.wi + 1 >= w.wn || w.ways[w.wi].cross >= 0 || w.ways[w.wi + 1].cross < 0) return false;
	const TrafficParams &p = s.catalog.traffic;
	const Waypoint &kerb = w.ways[w.wi], &far = w.ways[w.wi + 1];
	V2 d(far.x - kerb.x, far.z - kerb.z);
	const float len = d.len();
	if (len < ZERO_SPAN_M) return false;
	k.d = d * (1.f / len);
	k.right = k.d.perp(); // the right of the way across (left is (d.z, -d.x), lane_geometry.cpp)
	k.key = (int64_t(w.way_node) << KERB_NODE_BITS) | (int64_t(far.cross) << 1) | ((k.d.x > 0 || (k.d.x == 0 && k.d.y > 0)) ? 1 : 0);
	k.gap = p.kerb_wait_gap_m;
	const float widest = std::min(p.kerb_wait_half_m, s.catalog.furniture.crossing.half() - p.ped_body_m);
	k.cols = std::max(1, int((widest - k.gap * 0.5f) / k.gap) + 1);
	// rows back from the kerb while a walker still has its way along the sidewalk behind the last one
	const Profile &pf = s.roads.profile_of(far.cross);
	const StripEdge &e = kerb.side == 1 ? pf.edges.back() : pf.edges.front();
	const float aisle = 2 * p.ped_body_m;
	const float depth = (e.u1 - e.u0) - p.ped_margin_m - p.kerb_stand_m - p.ped_body_m - aisle;
	const int rows = std::clamp(depth > 0 ? int(depth / k.gap) + 1 : 1, 1, int(p.kerb_wait_rows));
	k.places = std::min(MAX_PLACES, k.cols * rows);
	return true;
}

// Place `n` of a kerb: how far right of the crosswalk's middle line and how far back from the kerb.
void place_at(const Kerb &k, int n, float &right, float &back) {
	right = k.gap * 0.5f + float(n % k.cols) * k.gap;
	back = float(n / k.cols) * k.gap;
}

// Moves the kerb waypoint to place `n` (sign +1) or back from it (-1); the far one keeps the line across.
void shift(Pedestrian &w, const Kerb &k, int n, float sign) {
	float right, back;
	place_at(k, n, right, back);
	Waypoint &kerb = w.ways[w.wi], &far = w.ways[w.wi + 1];
	kerb.x += sign * (k.right.x * right - k.d.x * back);
	kerb.z += sign * (k.right.y * right - k.d.y * back);
	far.x += sign * k.right.x * right;
	far.z += sign * k.right.y * right;
	w.line_off = sign > 0 ? -right : 0.f;
}

// The nearest free place to the kerb not on street furniture, or -1.
int free_place(CityState &s, const Pedestrian &w, const Kerb &k, uint32_t taken) {
	const TrafficParams &p = s.catalog.traffic;
	const Waypoint &kerb = w.ways[w.wi];
	for (int n = 0; n < k.places; n++) {
		if ((taken >> n) & 1u) continue;
		float right, back;
		place_at(k, n, right, back);
		const float x = kerb.x + k.right.x * right - k.d.x * back, z = kerb.z + k.right.y * right - k.d.y * back;
		if (!on_furniture(s, w.ways[w.wi + 1].cross, kerb.side, x, z, p.ped_body_m)) return n;
	}
	return -1;
}

} // namespace

void take_kerb_place(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	Kerb k;
	if (!kerb_of(s, w, k)) return;
	if (w.slot >= 0 && w.slot_key == k.key) shift(w, k, w.slot, -1.f); // back to the kerb point: it may take a better one
	free_kerb_place(t, id);
	w.line_off = 0; // no place yet: it would cross along the middle line
	const int n = free_place(s, w, k, t.kerb_slots[k.key]);
	if (n < 0) { // every place taken: it heads for the kerb point and stops behind those waiting
		if (t.kerb_slots[k.key] == 0) t.kerb_slots.erase(k.key);
		return;
	}
	t.kerb_slots[k.key] |= 1u << n;
	w.slot_key = k.key;
	w.slot = n;
	shift(w, k, n, 1.f);
}

bool step_to_nearer_place(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	if (w.slot <= 0 || w.wi < 1) return false;
	w.wi--; // waiting, it heads for ways[wi]: the far kerb; its own kerb is the one before
	Kerb k;
	bool ahead = false;
	if (kerb_of(s, w, k) && k.key == w.slot_key) {
		auto it = t.kerb_slots.find(k.key);
		const int n = free_place(s, w, k, it == t.kerb_slots.end() ? 0u : it->second);
		ahead = n >= 0 && n < w.slot;
	}
	if (!ahead) w.wi++;
	return ahead;
}

void free_kerb_place(TrafficState &t, int id) {
	Pedestrian &w = t.peds[size_t(id)];
	if (w.slot < 0) return;
	auto it = t.kerb_slots.find(w.slot_key);
	if (it != t.kerb_slots.end()) {
		it->second &= ~(1u << w.slot);
		if (it->second == 0) t.kerb_slots.erase(it);
	}
	w.slot = -1;
	w.slot_key = -1;
}

} // namespace traffic
} // namespace city
