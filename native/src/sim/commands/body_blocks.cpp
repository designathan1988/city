// The blocks a rectangle dragged against a block makes (the 4.0's addPlanMass and planAction): a wing on the ground, a block on the roof, a void cut out; and how a block
// counts in the body (setBlockMode).
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_ops.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/model_ops.h"
#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include "core/polygon_ops.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_BLOCK = "tool.failed", *PRIMITIVE_ID = "primitive";
constexpr float TOUCH_AREA_M2 = 1e-3f;
constexpr int PRIMITIVE_ZONE = 1;

float on_grid(const Tuning &t, float x) { return std::round(x / float(t.build_grid_m)) * float(t.build_grid_m); }

// The body of a building that stands, or why not.
bool standing(const CityState &s, int id, Body &body, EditResult &r) {
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) {
		r.error = NO_BLOCK;
		return false;
	}
	record_body(s.buildings.designs, s.buildings.items[size_t(id)], body);
	return true;
}

// The biggest piece of `box` standing on the block: a mass on a roof is kept to the roof.
Ring on_roof(const Ring &box, const Volume &v) {
	PlanRing a, c;
	for (const Point2 &p : box) a.push_back(V2(p.x, p.y));
	for (const Point2 &p : ring_of(v)) c.push_back(V2(p.x, p.y));
	Ring best;
	float most = 0;
	for (const PlanRing &piece : plan_intersection({ a }, { c })) {
		Ring ring;
		for (const V2 &p : piece) ring.push_back({ p.x, p.y });
		const float area = ring_area(ring);
		if (area > most) {
			most = area;
			best = std::move(ring);
		}
	}
	return best;
}
} // namespace

std::string op_add_mass(Body &b, const Tuning &t, int vol, Point2 from, Point2 to, int action) {
	if (vol < 0 || vol >= int(b.volumes.size()) || action < MASS_GROUND || action > MASS_CUT) return NO_BLOCK;
	if (int(b.volumes.size()) >= int(t.build_max_volumes)) return "building.problem.size";
	Volume &source = b.volumes[size_t(vol)];
	const float x0 = on_grid(t, std::min(from.x, to.x)), x1 = on_grid(t, std::max(from.x, to.x)), y0 = on_grid(t, std::min(from.y, to.y)), y1 = on_grid(t, std::max(from.y, to.y));
	Ring ring{ { x0, y0 }, { x1, y0 }, { x1, y1 }, { x0, y1 } };
	if (x1 <= x0 || y1 <= y0) return "building.problem.size";
	if (action == MASS_ROOF) ring = on_roof(ring, source);
	if (ring.size() < 3) return "building.problem.outline";
	if (action == MASS_CUT && overlap_area(ring, ring_of(source)) <= TOUCH_AREA_M2) return "building.problem.cut";
	Volume mass = block_like(source, action == MASS_ROOF ? std::min(int(t.build_roof_mass_storeys), std::max(1, int(t.edit_max_floors) - source.top())) : source.storeys);
	for (FaceGeometry &f : mass.faces) f.bays = -1; // its openings' proportions, but as many bays as its own walls hold
	set_block_ring(mass, ring);
	if (action == MASS_GROUND) {
		mass.base = 0;
		std::fill(mass.fill.begin(), mass.fill.end(), source.fill.front());
	} else if (action == MASS_ROOF) {
		mass.base = source.top();
		mass.roof = ROOF_FLAT;
		std::fill(mass.fill.begin(), mass.fill.end(), next_fill(source));
		clear_roof_under(source, ring_of(mass));
		source.roof = ROOF_TERRACE;
	} else {
		mass.mode = BLOCK_VOID;
		mass.roof = ROOF_FLAT;
	}
	b.volumes.push_back(std::move(mass));
	return "";
}

// The whole plan bigger or smaller about its middle (opScalePlan): the width lands on the grid, so walls still meet.
std::string op_scale_plan(Body &b, const Tuning &t, float factor, bool snap) {
	if (b.volumes.empty() || factor <= 0) return NO_BLOCK;
	float x0 = std::numeric_limits<float>::max(), y0 = x0, x1 = -x0, y1 = -x0;
	for (const Volume &v : b.volumes)
		for (const Point2 &p : ring_of(v)) {
			x0 = std::min(x0, p.x);
			y0 = std::min(y0, p.y);
			x1 = std::max(x1, p.x);
			y1 = std::max(y1, p.y);
		}
	const float width = x1 - x0, grid = float(t.build_grid_m), cx = (x0 + x1) / 2, cy = (y0 + y1) / 2;
	const float k = snap ? std::max(grid, std::round(width * factor / grid) * grid) / width : factor;
	for (Volume &v : b.volumes) {
		v.x = cx + (v.x - cx) * k;
		v.y = cy + (v.y - cy) * k;
		v.w *= k;
		v.d *= k;
		v.reliefs.clear();
		for (RoofPart &p : v.gear) {
			p.x = cx + (p.x - cx) * k;
			p.y = cy + (p.y - cy) * k;
		}
		for (FaceGeometry &f : v.faces) f.bays = -1;
	}
	for (LotElement &e : b.elements) {
		e.x = cx + (e.x - cx) * k;
		e.y = cy + (e.y - cy) * k;
	}
	return "";
}

// A block's floors raised or lowered (a split level), by half ground storeys when snapped; what stands on it and what hangs on its walls go with it.
std::string op_set_lift(Body &b, const Tuning &t, int vol, float lift, bool snap) {
	if (vol < 0 || vol >= int(b.volumes.size())) return NO_BLOCK;
	const float most = float(t.build_max_lift_m), step = b.ground_h / 2;
	const float wanted = snap ? std::round(lift / step) * step : lift;
	const float limit = snap ? std::floor(most / step) * step : most;
	const float dz = std::clamp(wanted, -limit, limit) - b.volumes[size_t(vol)].lift;
	if (dz == 0) return "";
	const float reach = float(t.build_hung_reach_m);
	for (int i : riding_on(b, vol)) {
		Volume &v = b.volumes[size_t(i)];
		v.lift += dz;
		for (LotElement &e : b.elements)
			if (e.z > 0 && e.x >= v.x - reach && e.x <= v.x + v.w + reach && e.y >= v.y - reach && e.y <= v.y + v.d + reach) e.z += dz;
	}
	return "";
}

std::string op_handle(Body &b, const Tuning &t, const HandleEdit &e) {
	if (e.kind.empty()) return "";
	if (e.kind == "lift") return op_set_lift(b, t, e.vol, e.along, e.snap);
	if (e.kind == "relief") {
		Relief r;
		r.side = e.side;
		r.bay0 = r.bay1 = e.index;
		r.storey0 = r.storey1 = e.count;
		r.depth = e.along;
		return op_set_relief(b, t, e.vol, r, e.snap);
	}
	if (e.kind == "part") return op_add_part(b, t, e.vol, e.count, e.side, e.index, e.part);
	if (e.kind == "primitive") return op_primitive_on(b, t, e.vol, e.side, e.count, e.x, e.y, e.shape);
	if (e.kind == "storeys") return op_set_storeys(b, t, e.vol, e.count);
	if (e.kind == "vertex") return op_move_point(b, t, e.vol, e.index, e.x, e.y, e.snap);
	if (e.kind == "scale") return op_scale_plan(b, t, e.factor, e.snap);
	if (e.kind != "side") return NO_BLOCK;
	if (e.wing) return e.along >= float(t.build_min_side_m) ? op_add_wing(b, t, e.vol, e.side, e.along, 0) : "";
	const float grid = float(t.build_grid_m), along = e.snap ? std::round(e.along / grid) * grid : e.along;
	return along != 0 ? op_extrude_face(b, t, e.vol, e.side, along) : "";
}

// Out of where it stood and into the new place, as a body lands: on another building it welds, and it may leave a road behind it.
EditResult move_building(CityState &s, int id, double x, double z, double rot) {
	EditResult r;
	Body body;
	if (!standing(s, id, body, r)) return r;
	const Building rec = s.buildings.items[size_t(id)];
	r.error = place_problem(s, body, x, z, rot);
	if (!r.error.empty()) return r;
	j_remove_building(s, id);
	return place_body(s, body, x, z, rot, rec.fn, rec.zone, &rec);
}

std::string op_set_block_mode(Body &b, int vol, int mode) {
	if (vol < 0 || vol >= int(b.volumes.size()) || mode < BLOCK_SOLID || mode > BLOCK_XOR) return NO_BLOCK;
	b.volumes[size_t(vol)].mode = BlockMode(mode);
	const bool mass = std::any_of(b.volumes.begin(), b.volumes.end(), [](const Volume &v) { return v.mode == BLOCK_SOLID || v.mode == BLOCK_XOR; });
	return mass ? "" : "building.problem.noMass";
}

std::string op_primitive_on(Body &b, const Tuning &t, int vol, int side, int storey, float x, float y, const PrimitiveSpec &p) {
	if (vol < 0 || vol >= int(b.volumes.size())) return NO_BLOCK;
	if (int(b.volumes.size()) >= int(t.build_max_volumes)) return "building.problem.size";
	const Volume &source = b.volumes[size_t(vol)];
	const float size = float(t.build_primitive_m);
	const bool top = side < 0;
	float cx = x, cy = y, across = size, angle = 0;
	int storeys = int(t.build_roof_mass_storeys);
	if (!top) {
		const BlockWall e = block_wall(source, side);
		const float along = (x - e.a.x) * e.tx + (y - e.a.y) * e.ty, depth = float(t.build_primitive_depth_m);
		cx = e.a.x + e.tx * along + e.nx * depth / 2;
		cy = e.a.y + e.ty * along + e.ny * depth / 2;
		across = depth;
		angle = std::atan2(e.ty, e.tx);
		storeys = std::max(1, storey + 1);
	}
	const float c = std::cos(angle), s = std::sin(angle);
	Ring ring;
	for (const Point2 &q : plan_ring(p.shape)) {
		const float lx = (q.x - 0.5f) * size, ly = (q.y - 0.5f) * across;
		ring.push_back({ cx + lx * c - ly * s, cy + lx * s + ly * c });
	}
	Volume mass = block_like(source, storeys);
	for (FaceGeometry &f : mass.faces) f.bays = -1;
	set_block_ring(mass, ring);
	mass.base = top ? source.top() : source.base;
	mass.roof = Roof(p.roof);
	mass.pitch = p.roof == ROOF_FLAT ? -1 : p.pitch;
	b.volumes.push_back(std::move(mass));
	return "";
}

// The block and what stands on it taken out into a building of its own, in the same place, as one step of undo.
EditResult detach_block(CityState &s, int id, int vol) {
	EditResult r;
	Body body;
	if (!standing(s, id, body, r)) return r;
	const Building rec = s.buildings.items[size_t(id)];
	if (vol < 0 || vol >= int(body.volumes.size())) {
		r.error = NO_BLOCK;
		return r;
	}
	const std::set<int> moving = riding_on(body, vol);
	if (moving.size() == body.volumes.size()) {
		r.error = "building.problem.lastVolume";
		return r;
	}
	Body rest = body, own = body;
	rest.volumes.clear();
	own.volumes.clear();
	int lowest = std::numeric_limits<int>::max();
	for (int i : moving) lowest = std::min(lowest, body.volumes[size_t(i)].base);
	for (int i = 0; i < int(body.volumes.size()); i++) {
		Volume v = body.volumes[size_t(i)];
		if (moving.count(i) == 0) {
			rest.volumes.push_back(std::move(v));
			continue;
		}
		v.base -= lowest;
		own.volumes.push_back(std::move(v));
	}
	own.elements.clear();
	own.cores.clear();
	own.furnishing.clear();
	own.entrance_volume = rest.entrance_volume = 0;
	r.error = body_problem(s.catalog.tune, rest);
	if (r.error.empty()) r.error = body_problem(s.catalog.tune, own);
	if (!r.error.empty()) return r;
	auto top_of = [](const Body &b) {
		int top = 0;
		for (const Volume &v : b.volumes) top = std::max(top, v.top());
		return top;
	};
	Building kept = rec, other = rec;
	kept.floors = uint16_t(top_of(rest));
	other.floors = uint16_t(top_of(own));
	rest.signature = own.signature = 0;
	kept.body = s.buildings.designs.edited->add(std::move(rest));
	other.body = s.buildings.designs.edited->add(std::move(own));
	j_edit_building(s, id, kept);
	r.placed = j_add_building(s, other);
	r.ok = true;
	return r;
}

// A primitive alone on the ground is a body of one block, not a lot: a block of the size of the card stands where a lot could not.
EditResult place_primitive(CityState &s, double x, double z, const PrimitiveSpec &p) {
	const Tuning &t = s.catalog.tune;
	const Data &data = s.buildings.designs;
	ModelSpec m;
	m.id = PRIMITIVE_ID;
	m.kind = KIND_FLATS;
	m.ground_h = float(t.build_primitive_ground_m);
	m.storey_h = float(t.build_primitive_storey_m);
	m.wall = data.hard.front();
	m.roof = data.flat_roofs.front();
	m.trim = data.color("wall_coping");
	ModelBlock block;
	block.w = block.d = float(t.build_primitive_m);
	block.storeys = int(t.build_primitive_storeys);
	block.roof = Roof(p.roof);
	block.pitch = p.roof == ROOF_FLAT ? -1 : p.pitch;
	block.door_bay = MIDDLE_BAY;
	m.blocks.push_back(block);
	Body body;
	model_body(data, m, body);
	EditResult r;
	r.error = op_set_plan(body, t, 0, p.shape);
	if (!r.error.empty()) return r;
	const PlaceSnap at = snap_place(s, body.plan.W, body.plan.D, x, z, 0);
	return place_body(s, body, at.x, at.z, at.rot, 0, PRIMITIVE_ZONE);
}

} // namespace city
