// Setting a body down in the city (model_ops.h): checked where it stands, and made one with what it lands on; a building copied beside
// itself, a primitive or a drawn outline set down as a building of one block, and what a building is for.
#include "core/random.h"
#include "sim/buildings/b40/body_table.h"
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
constexpr const char *ONE_BLOCK_ID = "primitive"; // the model id a body of one block is made under
constexpr int ONE_BLOCK_ZONE = 1; // the base zone a building of one block stands in
constexpr int LEAST_CORNERS = 3;

// A body of one block `w` by `d` in the look of a primitive (the Draw tab's): what a primitive and a drawn outline are made from.
Body one_block(const CityState &s, float w, float d, int roof, float pitch) {
	const Tuning &t = s.catalog.tune;
	const Data &data = s.buildings.designs;
	ModelSpec m;
	m.id = ONE_BLOCK_ID;
	m.kind = KIND_FLATS;
	m.ground_h = float(t.build_primitive_ground_m);
	m.storey_h = float(t.build_primitive_storey_m);
	m.wall = data.hard.front();
	m.roof = data.flat_roofs.front();
	m.trim = data.color("wall_coping");
	ModelBlock block;
	block.w = w;
	block.d = d;
	block.storeys = t.build_primitive_storeys;
	block.roof = Roof(roof);
	block.pitch = roof == ROOF_FLAT ? -1 : pitch;
	block.door_bay = MIDDLE_BAY;
	m.blocks.push_back(block);
	Body body;
	model_body(data, m, body);
	return body;
}

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

// Beside the building, a lot's width to its right, its left, behind, in front, then two widths out (the 4.0's duplicateBuilding): the
// first place where the copy stands alone. A copy never welds into a neighbour: a place where its lot meets another is passed by.
EditResult duplicate_building(CityState &s, int id) {
	EditResult r;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) {
		r.error = "tool.failed";
		return r;
	}
	const Building rec = s.buildings.items[size_t(id)];
	const std::shared_ptr<const Body> held = record_body(s.buildings.designs, rec);
	if (held->volumes.empty()) { // an empty lot: no building to copy
		r.error = "building.problem.noMass";
		return r;
	}
	const LotFrame f = lot_frame(rec);
	const float sn = std::sin(rec.rot), cs = std::cos(rec.rot);
	const V2 front{ rec.x + rec.d / 2 * sn, rec.z + rec.d / 2 * cs };
	const V2 shifts[] = { { rec.w, 0 }, { -rec.w, 0 }, { 0, rec.d }, { 0, -rec.d }, { 2 * rec.w, 0 }, { -2 * rec.w, 0 } };
	r.error = "building.problem.building";
	for (const V2 &lot : shifts) {
		const V2 at = front + f.dir(lot.x, lot.y);
		if (!landed_on(s, record_at(*held, at.x, at.y, rec.rot)).empty() || !place_problem(s, *held, at.x, at.y, rec.rot).empty()) continue;
		return place_body(s, *held, at.x, at.y, rec.rot, rec.fn, rec.zone);
	}
	return r;
}

// A primitive alone on the ground is a body of one block, not a lot: a block of the size of the card stands where a lot could not.
EditResult place_primitive(CityState &s, double x, double z, const PrimitiveSpec &p) {
	const Tuning &t = s.catalog.tune;
	Body body = one_block(s, float(t.build_primitive_m), float(t.build_primitive_m), p.roof, p.pitch);
	EditResult r;
	r.error = op_set_plan(body, t, 0, p.shape);
	if (!r.error.empty()) return r;
	const PlaceSnap at = snap_place(s, body.plan.W, body.plan.D, x, z, 0);
	return place_body(s, body, at.x, at.z, at.rot, 0, ONE_BLOCK_ZONE);
}

// A drawn outline as a building of one block (the Roadcraft's free plan): a block over the outline's box, its plan the outline in the
// lot's frame, the lot unturned with its front on the box's far side.
EditResult place_sketch(CityState &s, const std::vector<V2> &outline) {
	EditResult r;
	if (outline.size() < size_t(LEAST_CORNERS) || int(outline.size()) > MAX_FACES || !simple_ring(Ring(outline.begin(), outline.end()))) {
		r.error = "building.problem.outline";
		return r;
	}
	float x0 = outline[0].x, x1 = x0, z0 = outline[0].y, z1 = z0;
	for (const V2 &p : outline) {
		x0 = std::min(x0, p.x);
		x1 = std::max(x1, p.x);
		z0 = std::min(z0, p.y);
		z1 = std::max(z1, p.y);
	}
	Body body = one_block(s, x1 - x0, z1 - z0, ROOF_FLAT, -1);
	const double ax = (x0 + x1) / 2, az = z1;
	const LotFrame f = lot_frame(record_at(body, ax, az, 0));
	Ring ring;
	for (const V2 &p : outline) {
		const V2 rel = p - f.o;
		ring.push_back({ rel.x * f.e0.x + rel.y * f.e0.y, rel.x * f.e1.x + rel.y * f.e1.y });
	}
	set_block_ring(body.volumes.front(), ring);
	return place_body(s, body, ax, az, 0, 0, ONE_BLOCK_ZONE);
}

// What a building is for (the Roadcraft's setFunction): any building may be any of them, its body kept; its residents and jobs follow
// its use, as the store counts them when a record is put back.
EditResult set_function(CityState &s, int id, int fn) {
	EditResult r;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive || fn < 0 || fn >= FN_COUNT) {
		r.error = "tool.failed";
		return r;
	}
	Building b = s.buildings.items[size_t(id)];
	r.ok = true;
	if (b.fn == fn) return r;
	if (b.body == 0) b.body = s.buildings.designs.edited->add(Body(*record_body(s.buildings.designs, b))); // the body it has, kept as it is
	b.fn = uint8_t(fn);
	j_edit_building(s, id, b);
	return r;
}

} // namespace city
