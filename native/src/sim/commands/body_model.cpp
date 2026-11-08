// Modelling a block as SketchUp and Blender do (the 4.0's extrudeFace, offsetPlan, bevelPlan, changePlanVertex, turnBlock, copyBlock and opMoveBlock): faces pushed and pulled,
// the plan grown and bevelled, its points moved, the block turned, copied and slid with what stands on it.
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include "core/math.h"
#include "core/units.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_BLOCK = "tool.failed", *NOT_THAT_PLAN = "building.problem.outline";
constexpr float RADIANS_PER_DEG = DEG_TO_RAD, TOUCH_AREA_M2 = 1e-3f, CLICK_IN_M = 1.2f, NO_SHIFT = 1e-6f, SIZE_SLACK_M = 1e-3f;
constexpr int LEAST_CORNERS = 3, FRAME_BAYS = 2;
constexpr uint32_t KEY_FIELD = (1u << KEY_SIDE_SHIFT) - 1;

bool has(const Body &b, int vol) { return vol >= 0 && vol < int(b.volumes.size()); }

bool can_stand(const Ring &ring) { return ring.size() >= size_t(LEAST_CORNERS) && int(ring.size()) <= MAX_FACES && ring_area(ring) > 0 && simple_ring(ring); }

// `v`'s ring moved to `ring`, kept to what a plan may be.
std::string replan(Volume &v, const Ring &ring) {
	if (!can_stand(ring)) return NOT_THAT_PLAN;
	set_block_ring(v, ring);
	return "";
}

// The block a block is measured from: the first on the ground, or the one under most of it.
const Volume *reference_of(const Body &b, const Volume &v) {
	if (v.base == 0) {
		for (const Volume &o : b.volumes)
			if (o.base == 0) return &o == &v ? nullptr : &o;
		return nullptr;
	}
	const Volume *best = nullptr;
	float most = 0;
	for (const Volume &o : b.volumes) {
		if (&o == &v || o.base >= v.base || o.top() < v.base) continue;
		const float area = overlap_area(ring_of(o), ring_of(v));
		if (area > most) {
			most = area;
			best = &o;
		}
	}
	return best;
}

// Where a slid block clicks into place: sides on sides and middles on middles of the other blocks.
V2 click_in(const Body &b, const std::set<int> &moving, const Volume &v) {
	V2 shift;
	float best_x = CLICK_IN_M, best_y = CLICK_IN_M;
	const float mine_x[] = { v.x, v.x + v.w / 2, v.x + v.w }, mine_y[] = { v.y, v.y + v.d / 2, v.y + v.d };
	for (int k = 0; k < int(b.volumes.size()); k++) {
		if (moving.count(k) != 0) continue;
		const Volume &o = b.volumes[size_t(k)];
		const float theirs_x[] = { o.x, o.x + o.w / 2, o.x + o.w }, theirs_y[] = { o.y, o.y + o.d / 2, o.y + o.d };
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) {
				if ((i == 1) != (j == 1)) continue;
				const float ex = theirs_x[j] - mine_x[i], ey = theirs_y[j] - mine_y[i];
				if (std::fabs(ex) < best_x) {
					best_x = std::fabs(ex);
					shift.x = ex;
				}
				if (std::fabs(ey) < best_y) {
					best_y = std::fabs(ey);
					shift.y = ey;
				}
			}
	}
	return shift;
}

void shift_block(Volume &v, float dx, float dy) {
	v.x += dx;
	v.y += dy;
	for (RoofPart &p : v.gear) {
		p.x += dx;
		p.y += dy;
	}
}

// A side's key through a corner added or taken: the side split goes to both halves, the side gone to none.
std::vector<int> sides_after(int side, int index, bool remove) {
	if (remove) return side == index ? std::vector<int>{} : std::vector<int>{ side > index ? side - 1 : side };
	return side == index ? std::vector<int>{ side, side + 1 } : std::vector<int>{ side > index ? side + 1 : side };
}

template <typename M>
M remap_sides(const M &old, int index, bool remove) {
	std::map<uint32_t, typename M::mapped_type> sorted(old.begin(), old.end()); // two old sides may land on one: the same one wins every time
	M next;
	for (const auto &e : sorted)
		for (int to : sides_after(int(e.first & KEY_FIELD), index, remove)) next[side_key(int(e.first >> KEY_SIDE_SHIFT), to)] = e.second;
	return next;
}
} // namespace

BlockWall block_wall(const Volume &v, int side) {
	const Ring ring = ring_of(v);
	const V2 a = ring[size_t(side) % ring.size()], c = ring[(size_t(side) + 1) % ring.size()];
	const float length = std::hypot(c.x - a.x, c.y - a.y), tx = (c.x - a.x) / length, ty = (c.y - a.y) / length;
	return { a, tx, ty, ty, -tx, length };
}

// What stands on a block, and on that: it goes where the block goes.
std::set<int> riding_on(const Body &b, int vol) {
	std::set<int> moving{ vol };
	for (bool grew = true; grew;) {
		grew = false;
		for (int i = 0; i < int(b.volumes.size()); i++) {
			if (moving.count(i) != 0) continue;
			for (int u : moving)
				if (b.volumes[size_t(i)].base == b.volumes[size_t(u)].top() && b.volumes[size_t(i)].lift == b.volumes[size_t(u)].lift && overlap_area(ring_of(b.volumes[size_t(i)]), ring_of(b.volumes[size_t(u)])) > TOUCH_AREA_M2) {
					moving.insert(i);
					grew = true;
					break;
				}
		}
	}
	return moving;
}

std::string op_extrude_face(Body &b, const Tuning &t, int vol, int side, float distance) {
	if (!has(b, vol) || side < 0 || side >= b.volumes[size_t(vol)].sides()) return "builder.pickFace";
	Volume &v = b.volumes[size_t(vol)];
	if (v.outline.empty()) return op_resize(b, t, vol, side, distance, false);
	Ring ring = ring_of(v);
	const BlockWall e = block_wall(v, side);
	for (size_t i : { size_t(side), (size_t(side) + 1) % ring.size() }) {
		ring[i].x += e.nx * distance;
		ring[i].y += e.ny * distance;
	}
	return replan(v, ring);
}

// A brick the face's width and the block's height, set against the face.
std::string op_extrude_block(Body &b, const Tuning &t, int vol, int side, float depth) {
	if (!has(b, vol) || side < 0 || side >= b.volumes[size_t(vol)].sides()) return "builder.pickFace";
	if (int(b.volumes.size()) >= t.build_max_volumes) return "building.problem.size";
	const Volume &v = b.volumes[size_t(vol)];
	const BlockWall e = block_wall(v, side);
	const V2 far{ e.a.x + e.tx * e.length, e.a.y + e.ty * e.length };
	Volume brick = block_like(v, v.storeys);
	for (FaceGeometry &f : brick.faces) f.bays = -1;
	set_block_ring(brick, { e.a, far, { far.x + e.nx * depth, far.y + e.ny * depth }, { e.a.x + e.nx * depth, e.a.y + e.ny * depth } });
	brick.roof = v.roof;
	b.volumes.push_back(std::move(brick));
	return "";
}

// The face set back inside a frame of one bay at each end and the top and bottom storeys.
std::string op_inset_face(Body &b, const Tuning &t, int vol, int side, float depth) {
	if (!has(b, vol) || side < 0 || side >= b.volumes[size_t(vol)].sides()) return "builder.pickFace";
	const Volume &v = b.volumes[size_t(vol)];
	const int bays = bays_of(v, side);
	Relief r;
	r.side = side;
	r.bay0 = bays > FRAME_BAYS ? 1 : 0;
	r.bay1 = bays > FRAME_BAYS ? bays - FRAME_BAYS : bays - 1;
	r.storey0 = v.storeys > FRAME_BAYS ? 1 : 0;
	r.storey1 = v.storeys > FRAME_BAYS ? v.storeys - FRAME_BAYS : v.storeys - 1;
	r.depth = -std::fabs(depth);
	return op_set_relief(b, t, vol, r, false);
}

std::string op_offset_block(Body &b, int vol, float distance) {
	if (!has(b, vol)) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	if (v.outline.empty()) {
		v.x -= distance;
		v.y -= distance;
		v.w += 2 * distance;
		v.d += 2 * distance;
		return "";
	}
	return replan(v, offset_ring(ring_of(v), distance));
}

std::string op_bevel(Body &b, int vol, float distance, int only) {
	if (!has(b, vol) || distance <= 0) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	const Ring ring = ring_of(v);
	const size_t n = ring.size();
	Ring out;
	for (size_t i = 0; i < n; i++) {
		const V2 p = ring[i], prev = ring[(i + n - 1) % n], next = ring[(i + 1) % n];
		const float to_prev = std::hypot(prev.x - p.x, prev.y - p.y), to_next = std::hypot(next.x - p.x, next.y - p.y);
		const float a = std::min(distance, to_prev / 2 - SIZE_SLACK_M), c = std::min(distance, to_next / 2 - SIZE_SLACK_M);
		if ((only >= 0 && only != int(i)) || a <= 0 || c <= 0) {
			out.push_back(p);
			continue;
		}
		out.push_back({ p.x + (prev.x - p.x) / to_prev * a, p.y + (prev.y - p.y) / to_prev * a });
		out.push_back({ p.x + (next.x - p.x) / to_next * c, p.y + (next.y - p.y) / to_next * c });
	}
	if (out.size() == n) return NOT_THAT_PLAN;
	const std::string problem = replan(v, out);
	if (problem.empty()) plain_walls(v);
	return problem;
}

std::string op_move_point(Body &b, const Tuning &t, int vol, int index, float x, float y, bool snap) {
	if (!has(b, vol)) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	Ring ring = ring_of(v);
	if (index < 0 || index >= int(ring.size())) return NO_BLOCK;
	const float step = snap ? float(t.build_grid_m) : 0;
	ring[size_t(index)] = step > 0 ? V2{ std::round(x / step) * step, std::round(y / step) * step } : V2{ x, y };
	return replan(v, ring);
}

// A corner added on side `index`, in its middle, or the corner `index` taken away: what was set on the walls follows them.
std::string op_change_point(Body &b, int vol, int index, bool remove) {
	if (!has(b, vol)) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	Ring ring = ring_of(v);
	const int old = int(ring.size());
	if (index < 0 || index >= old || (remove && old <= LEAST_CORNERS) || (!remove && old >= MAX_FACES)) return NOT_THAT_PLAN;
	if (remove) ring.erase(ring.begin() + index);
	else {
		const V2 p = ring[size_t(index)], q = ring[(size_t(index) + 1) % ring.size()];
		ring.insert(ring.begin() + index + 1, V2{ (p.x + q.x) / 2, (p.y + q.y) / 2 });
	}
	const Volume was = v;
	const std::string problem = replan(v, ring);
	if (!problem.empty()) return problem;
	v.side_fill = remap_sides(was.side_fill, index, remove);
	v.storey_paint = remap_sides(was.storey_paint, index, remove);
	v.bays.clear();
	std::map<uint32_t, uint8_t> sorted(was.bays.begin(), was.bays.end());
	for (const auto &e : sorted) {
		const int side = int((e.first >> KEY_SIDE_SHIFT) & KEY_FIELD);
		for (int to : sides_after(side, index, remove)) v.bays[bay_key(int(e.first >> KEY_STOREY_SHIFT), to, int(e.first & KEY_FIELD))] = e.second;
	}
	for (FaceGeometry &f : v.faces) f = FaceGeometry();
	for (int side = 0; side < old; side++)
		for (int to : sides_after(side, index, remove)) {
			v.faces[to] = was.faces[side];
			v.faces[to].bays = -1;
		}
	v.reliefs.clear();
	for (const Relief &r : was.reliefs)
		for (int to : sides_after(r.side, index, remove)) {
			Relief moved = r;
			moved.side = to;
			v.reliefs.push_back(moved);
		}
	return "";
}

std::string op_copy_block(Body &b, const Tuning &t, int vol) {
	if (!has(b, vol)) return NO_BLOCK;
	if (int(b.volumes.size()) >= t.build_max_volumes) return "building.problem.size";
	Volume copy = b.volumes[size_t(vol)];
	shift_block(copy, copy.w, 0);
	b.volumes.push_back(std::move(copy));
	return "";
}

std::string op_center_block(Body &b, int vol) {
	if (!has(b, vol)) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	const Volume *r = reference_of(b, v);
	if (r == nullptr) return "building.problem.noReference";
	shift_block(v, r->x + r->w / 2 - v.x - v.w / 2, r->y + r->d / 2 - v.y - v.d / 2);
	return "";
}

std::string op_turn_block(Body &b, int vol, float degrees) {
	if (!has(b, vol)) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	const float cx = v.x + v.w / 2, cy = v.y + v.d / 2, a = degrees * RADIANS_PER_DEG, c = std::cos(a), s = std::sin(a);
	auto turned = [&](float x, float y) { return V2{ cx + (x - cx) * c - (y - cy) * s, cy + (x - cx) * s + (y - cy) * c }; };
	Ring ring;
	for (const V2 &p : ring_of(v)) ring.push_back(turned(p.x, p.y));
	for (RoofPart &p : v.gear) {
		const V2 at = turned(p.x, p.y);
		p.x = at.x;
		p.y = at.y;
		p.turn += a;
	}
	v.reliefs.clear();
	for (FaceGeometry &f : v.faces) f.bays = -1;
	return replan(v, ring);
}

std::string op_slide_block(Body &b, const Tuning &t, int vol, float dx, float dy, bool snap) {
	if (!has(b, vol)) return NO_BLOCK;
	const float step = snap ? float(t.build_grid_m) : 0;
	float mx = step > 0 ? std::round(dx / step) * step : dx, my = step > 0 ? std::round(dy / step) * step : dy;
	const std::set<int> moving = riding_on(b, vol);
	if (snap) {
		Volume at = b.volumes[size_t(vol)];
		at.x += mx;
		at.y += my;
		const V2 fit = click_in(b, moving, at);
		mx += fit.x;
		my += fit.y;
	}
	if (std::fabs(mx) < NO_SHIFT && std::fabs(my) < NO_SHIFT) return "";
	for (int i : moving) shift_block(b.volumes[size_t(i)], mx, my);
	return "";
}

} // namespace city
