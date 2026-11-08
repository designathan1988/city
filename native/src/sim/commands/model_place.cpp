// Setting a body down in the city (model_ops.h): checked where it stands, and made one with what it lands on.
#include "core/random.h"
#include "sim/buildings/b40/civic.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_ops.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/model_ops.h"
#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr double SEED_GRID_M = 0.5;
constexpr uint32_t PLACE_SALT = 0x5bd1e995u; // a stream's salt: any fixed odd number apart from the others
constexpr float FACE_REACH_M = 40.f; // how far off a street a lot still takes that street as its own
constexpr float TOUCH_M2 = 1e-3f; // lots meeting at an edge do not overlap

Building record_at(const Body &body, double ax, double az, double rot) {
	Building b;
	const float sn = std::sin(float(rot)), cs = std::cos(float(rot));
	b.w = body.plan.W;
	b.d = body.plan.D;
	b.rot = float(rot);
	b.x = float(ax) - b.d / 2 * sn;
	b.z = float(az) - b.d / 2 * cs;
	return b;
}

// The lot's rectangle in the world.
Ring lot_ring(const Building &b) {
	LotFrame f = lot_frame(b);
	Ring ring;
	for (V2 c : { V2{ -b.w / 2, 0 }, V2{ b.w / 2, 0 }, V2{ b.w / 2, b.d }, V2{ -b.w / 2, b.d } }) {
		V2 p = f.at(c.x, c.y);
		ring.push_back(V2{ p.x, p.y });
	}
	if (ring_area(ring) < 0) std::reverse(ring.begin(), ring.end());
	return ring;
}

// How far a point lies outside the lot.
float outside(const Building &b, const LotFrame &f, double x, double z) {
	V2 rel = V2(float(x), float(z)) - f.o;
	float u = rel.x * f.e0.x + rel.y * f.e0.y, v = rel.x * f.e1.x + rel.y * f.e1.y;
	float du = std::max(std::fabs(u) - b.w / 2, 0.f), dv = std::max({ -v, v - b.d, 0.f });
	return std::hypot(du, dv);
}

bool on_road(const CityState &s, const Building &b) {
	const LotFrame f = lot_frame(b);
	const float reach = std::hypot(b.w, b.d) / 2, step = float(s.catalog.tune.build_road_probe_m);
	bool hit = false;
	s.roads.for_segments_near(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int sg) {
		if (hit) return;
		const Quad &q = s.roads.segs[sg].q;
		const float clear = s.roads.profile_of(sg).half();
		const int steps = std::max(1, int(std::ceil(quad_length(q) / step)));
		for (int i = 0; i <= steps && !hit; i++) {
			double x, z;
			q.at(double(i) / steps, x, z);
			hit = outside(b, f, x, z) < clear;
		}
	});
	return hit;
}

std::vector<int> landed_on(const CityState &s, const Building &b) {
	std::vector<int> ids;
	const Ring mine = lot_ring(b);
	const float reach = float(s.catalog.tune.build_max_side_m) * 2;
	s.buildings.query(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) {
		if (overlap_area(mine, lot_ring(s.buildings.items[size_t(id)])) > TOUCH_M2) ids.push_back(id);
	});
	return ids;
}
} // namespace

std::string place_problem(const CityState &s, const Body &body, double ax, double az, double rot) {
	std::string problem = body_problem(s.catalog.tune, body);
	if (!problem.empty()) return problem;
	const Tuning &t = s.catalog.tune;
	const Building b = record_at(body, ax, az, rot);
	const float reach = std::hypot(b.w, b.d) / 2, edge = s.map_half - float(t.build_world_margin_m);
	if (std::fabs(b.x) + reach > edge || std::fabs(b.z) + reach > edge) return "building.problem.bounds";
	if (on_road(s, b)) return "building.problem.road";
	float hi = 0, lo = 0;
	lot_ground(body, b, lot_frame(b), [&](float x, float z) { return s.terrain.height_at(x, z); }, hi, lo);
	return hi - lo > float(t.build_max_plinth_m) ? "building.problem.slope" : "";
}

EditResult place_body(CityState &s, const Body &src, double ax, double az, double rot, int fn, int zone, const Building *same) {
	EditResult r;
	if (fn < 0 || fn >= FN_COUNT || zone < 1 || zone >= ZONE_CODES) {
		r.error = "tool.failed";
		return r;
	}
	r.error = place_problem(s, src, ax, az, rot);
	if (!r.error.empty()) return r;
	Body body = src;
	Building b = record_at(body, ax, az, rot);
	const std::vector<int> landed = landed_on(s, b);
	const LotFrame frame = lot_frame(b);
	for (int id : landed) {
		const Building &other = s.buildings.items[size_t(id)];
		if (other.floors == 0 && other.body == 0 && !is_open_lot(other.fn)) continue;
		const std::shared_ptr<const Body> theirs_held = record_body(s.buildings.designs, other);
		const Body &theirs = *theirs_held;
		weld_body(body, frame, theirs, lot_frame(other), other.rot - b.rot);
	}
	if (!landed.empty()) {
		V2 front = fit_plan(body);
		V2 moved = frame.at(front.x, front.y);
		b = record_at(body, moved.x, moved.y, rot);
	}
	r.error = body_problem(s.catalog.tune, body);
	if (!r.error.empty()) return r;
	int top = 0;
	for (const Volume &v : body.volumes) top = std::max(top, v.top());
	for (int id : landed) j_remove_building(s, id);
	double t, dist;
	int street = s.roads.find_segment_near(ax, az, FACE_REACH_M, t, dist);
	b.profile = uint16_t(street >= 0 ? s.roads.segs[street].profile : 0);
	b.zone = uint8_t(zone);
	b.fn = uint8_t(fn);
	b.floors = uint16_t(top);
	b.seed = hash_combine(uint32_t(int32_t(std::lround(ax / SEED_GRID_M))), uint32_t(int32_t(std::lround(az / SEED_GRID_M))) ^ PLACE_SALT);
	b.style = uint16_t(b.seed & BUILDING_AUTO_STYLE_MASK);
	b.built = s.clock.minutes.load();
	if (same != nullptr) {
		b.seed = same->seed;
		b.style = same->style;
		b.built = same->built;
	}
	b.body = s.buildings.designs.edited->add(std::move(body));
	r.placed = j_add_building(s, b);
	r.ok = true;
	r.buildings_removed = int(landed.size());
	return r;
}

} // namespace city
