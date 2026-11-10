// The handles of the block picked (the 4.0's world/buildings/handles.ts) and what a drag on one does: a draft of the body for the ghost, and the edit on release.
#include "api/body_api.h"
#include "api/city_builder.h"

#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/roof_shape.h"
#include "sim/commands/model_ops.h"
#include "sim/persistence/body_codec.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <algorithm>
#include <array>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float SIDE_OUT_M = 1.6f, CORNER_OUT_M = 1.4f, SCALE_OUT_M = 0.8f, ABOVE_ROOF_M = 1.8f, OFF_FLOOR_M = 0.3f, LIFT_OUT_M = 0.6f, RELIEF_OUT_M = 1.12f;
constexpr float SHORTEST_M = 1e-3f;
constexpr int MOST_SIDES_WITH_ARROWS = 8, BOX_CORNERS = 4;
constexpr const char *MOVE_LABEL = "cmd.place.editBuilding";

Dictionary handle(const char *kind, const Vector3 &at, const Vector3 &dir, int side, int vertex) {
	Dictionary d;
	d["kind"] = kind;
	d["at"] = at;
	d["dir"] = dir;
	d["side"] = side;
	d["vertex"] = vertex;
	return d;
}

HandleEdit edit_of(const CityState &s, int id, const Dictionary &spec) {
	HandleEdit e;
	e.kind = String(spec.get("op", "")).utf8().get_data();
	e.vol = int(spec.get("vol", 0));
	e.side = int(spec.get("side", 0));
	e.index = int(spec.get("index", 0));
	e.count = int(spec.get("count", 0));
	e.along = float(spec.get("along", 0.0));
	e.factor = float(spec.get("factor", 1.0));
	e.wing = bool(spec.get("wing", false));
	e.snap = bool(spec.get("snap", true));
	if (spec.has("card")) e.part = part_spec(Dictionary(spec["card"]));
	if (spec.has("shape")) e.shape = primitive_of(Dictionary(spec["shape"]));
	const V2 at = lot_point(s, id, Vector3(spec.get("at", Vector3())));
	e.x = at.x;
	e.y = at.y;
	return e;
}
} // namespace

Array CityBuilder::body_handles(int id, int vol, bool points, const Vector3 &eye, const Dictionary &pick) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Array out;
	const CityState &s = *state_;
	const std::shared_ptr<const b40::Body> held = body_of_record(s, id);
	if (!held || vol < 0 || vol >= int(held->volumes.size())) return out;
	const b40::Body &body = *held;
	const Building &rec = s.buildings.items[id];
	const b40::Volume &v = body.volumes[size_t(vol)];
	const b40::LotFrame f = b40::lot_frame(rec);
	const float floor = b40::lot_floor(body, rec, f, [&](float x, float z) { return s.terrain.height_at(x, z); });
	auto world = [&](float x, float y, float z) {
		const V2 p = f.at(x, y);
		return Vector3(p.x, z, p.y);
	};
	auto along = [&](float x, float y) {
		const V2 d = f.dir(x, y);
		return Vector3(d.x, 0, d.y);
	};
	out.push_back(handle("storeys", world(v.x + v.w / 2, v.y + v.d / 2, floor + body.block_z(v, v.top()) + b40::roof_rise(v) + ABOVE_ROOF_M), Vector3(0, 1, 0), -1, -1));
	const float foot = floor + body.block_z(v, v.base) + OFF_FLOOR_M;
	const b40::Ring ring = b40::ring_of(v);
	if (v.sides() <= MOST_SIDES_WITH_ARROWS)
		for (int side = 0; side < v.sides(); side++) {
			const BlockWall w = block_wall(v, side);
			const float mx = w.a.x + w.tx * w.length / 2 + w.nx * SIDE_OUT_M, my = w.a.y + w.ty * w.length / 2 + w.ny * SIDE_OUT_M;
			out.push_back(handle("side", world(mx, my, foot), along(w.nx, w.ny), side, -1));
		}
	if (points || v.sides() > BOX_CORNERS)
		for (int i = 0; i < int(ring.size()); i++) out.push_back(handle("vertex", world(ring[size_t(i)].x, ring[size_t(i)].y, foot), Vector3(), -1, i));
	float x0 = BIG, y0 = BIG, x1 = -BIG, y1 = -BIG, eaves = 0;
	for (const b40::Volume &o : body.volumes) {
		for (const b40::Point2 &p : b40::ring_of(o)) {
			x0 = std::min(x0, p.x);
			y0 = std::min(y0, p.y);
			x1 = std::max(x1, p.x);
			y1 = std::max(y1, p.y);
		}
		eaves = std::max(eaves, body.block_z(o, o.top()));
	}
	const std::array<b40::Point2, BOX_CORNERS> box{ b40::Point2{ x0, y0 }, b40::Point2{ x1, y0 }, b40::Point2{ x1, y1 }, b40::Point2{ x0, y1 } };
	std::array<int, BOX_CORNERS> order{ 0, 1, 2, 3 };
	auto eye_to = [&](int i) { return world(box[size_t(i)].x, box[size_t(i)].y, floor).distance_squared_to(eye); };
	std::sort(order.begin(), order.end(), [&](int a, int c) { return eye_to(a) < eye_to(c); });
	const float mid_x = (x0 + x1) / 2, mid_y = (y0 + y1) / 2;
	auto off_corner = [&](int i, float by, float z) {
		const b40::Point2 c = box[size_t(i)];
		const float dx = c.x - mid_x, dy = c.y - mid_y, length = std::max(std::hypot(dx, dy), SHORTEST_M);
		return world(c.x + dx / length * by, c.y + dy / length * by, z);
	};
	out.push_back(handle("move", off_corner(order[0], CORNER_OUT_M, floor), Vector3(), -1, -1));
	out.push_back(handle("rotate", off_corner(order[1], CORNER_OUT_M, floor), Vector3(), -1, -1));
	if (v.base == 0) { // the block's own floor, raised or lowered: at its nearest corner
		float lx = v.x, ly = v.y, nearest = BIG;
		for (const b40::Point2 &p : ring) {
			const float d = world(p.x, p.y, floor).distance_squared_to(eye);
			if (d < nearest) {
				nearest = d;
				lx = p.x;
				ly = p.y;
			}
		}
		const float dx = lx - (v.x + v.w / 2), dy = ly - (v.y + v.d / 2), length = std::max(std::hypot(dx, dy), SHORTEST_M);
		out.push_back(handle("lift", world(lx + dx / length * LIFT_OUT_M, ly + dy / length * LIFT_OUT_M, floor + body.block_z(v, 0) + body.level_h(0) / 2), Vector3(0, 1, 0), -1, -1));
	}
	const int side = int(pick.get("side", -1)), bay = int(pick.get("bay", -1)), storey = int(pick.get("storey", 0));
	if (side >= 0 && side < v.sides() && bay >= 0) { // the bay picked pushed and pulled: an arrow off its middle, at the depth it stands at
		const BlockWall w = block_wall(v, side);
		const float a = (float(bay) + 0.5f) * w.length / float(b40::bays_of(v, side)), depth = b40::relief_depth(v, side, bay, storey), off = depth + RELIEF_OUT_M;
		const float z = floor + body.block_z(v, v.base + storey) + body.level_h(v.base + storey) / 2;
		Dictionary h = handle("relief", world(w.a.x + w.tx * a + w.nx * off, w.a.y + w.ty * a + w.ny * off, z), along(w.nx, w.ny), side, -1);
		h["depth"] = depth;
		h["bay"] = bay;
		h["storey"] = storey;
		out.push_back(h);
	}
	const b40::Point2 near = box[size_t(order[0])];
	out.push_back(handle("scale", off_corner(order[0], SCALE_OUT_M, floor + eaves), along(near.x - mid_x, near.y - mid_y).normalized(), -1, -1));
	return out;
}

// The body as a drag has it, for the ghost; the record's frame goes with it, and why it could not stand.
Dictionary CityBuilder::body_draft(int id, const Dictionary &spec) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	const std::shared_ptr<const b40::Body> held = body_of_record(s, id);
	if (!held) return d;
	b40::Body body = *held; // a copy: the drag's draft is drawn, never kept
	const Building &rec = s.buildings.items[id];
	const std::string done = op_handle(body, s.catalog.tune, edit_of(s, id, spec));
	const std::string problem = !done.empty() ? done : body_problem(s.catalog.tune, body);
	ByteWriter w;
	write_body(w, body);
	PackedByteArray bytes;
	bytes.resize(int64_t(w.buf.size()));
	std::memcpy(bytes.ptrw(), w.buf.data(), w.buf.size());
	const b40::LotFrame f = b40::lot_frame(rec);
	d["bytes"] = bytes;
	d["problem"] = String(problem.c_str());
	d["x"] = f.o.x;
	d["z"] = f.o.y;
	d["y"] = b40::lot_floor(body, rec, f, [&](float x, float z) { return s.terrain.height_at(x, z); });
	d["rot"] = rec.rot;
	d["cx"] = rec.x;
	d["cz"] = rec.z;
	return d;
}

Dictionary CityBuilder::body_apply(int id, const Dictionary &spec) {
	const HandleEdit e = edit_of(*state_, id, spec);
	return edit_of_body(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_handle(b, t, e); });
}

// The building taken to another place: the middle of its front at (x, z), turned by `rot`. It lands as a body lands: on another building it welds.
Dictionary CityBuilder::building_place(int id, double x, double z, double rot) {
	return run_edit(*state_, MOVE_LABEL, [&](CityState &s) { return move_building(s, id, x, z, rot); });
}

} // namespace city
