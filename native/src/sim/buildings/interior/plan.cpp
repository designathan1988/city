// The floor being furnished: what fits where, the grid of cells that keeps the test near what stands, and the plan of a
// residential floor as homes (a corridor down the long axis from 11 m deep, rooms of whole bays, so no wall cuts a window).
#include "sim/buildings/interior/plan.h"

#include "sim/buildings/b40/gen.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float ITEM_CELL = 2.f; // the side of a cell of the pieces' hash, metres
// A grid may end this far past a wall; a piece turned more than 30 degrees (sin 0.5) lies across; a cell key's row stride;
// the volume sides that run along x and along y.
constexpr float GRID_EPS = 1e-6f, TURNED_SIN = 0.5f;
constexpr int64_t CELL_STRIDE = 1 << 20, SIDE_ALONG_X = 0, SIDE_ALONG_Y = 3;

bool turned(float angle) { return std::abs(std::sin(angle)) > TURNED_SIN; }
int cell_of(float v) { return int(std::floor(v / ITEM_CELL)); }
int64_t cell_key(int gx, int gy) { return int64_t(gx) * CELL_STRIDE + gy; }

bool in_ring(const std::vector<V2> &ring, float x, float y) {
	bool in = false;
	for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
		const V2 &a = ring[i], &b = ring[j];
		if ((a.y > y) != (b.y > y) && x < (b.x - a.x) * (y - a.y) / (b.y - a.y) + a.x) in = !in;
	}
	return in;
}
} // namespace

FlatPlan flat_plan(const Data &data, const Volume &v, int group_bays) {
	const FloorPlanShape &K = data.shapes.floor_plan;
	FlatPlan plan;
	const bool along_x = v.w >= v.d;
	const float L = along_x ? v.w : v.d, D = along_x ? v.d : v.w;
	const int bays = std::max(1, bays_of(v, int(along_x ? SIDE_ALONG_X : SIDE_ALONG_Y)));
	const float bay = L / float(bays);
	std::vector<int> groups;
	for (int left = bays; left > 0;) { // rooms of `group_bays`; what is left too narrow for one more goes to the last
		const int take = left < group_bays * 2 ? left : group_bays;
		groups.push_back(take);
		left -= take;
	}
	plan.corridor = D >= K.double_loaded && bays >= 2;
	const float row_depth = plan.corridor ? (D - K.corridor) / 2 : D;
	const int rows = plan.corridor ? 2 : 1;
	for (int row = 0; row < rows; row++) {
		const float s0 = row == 0 ? 0 : D, inward = row == 0 ? 1.f : -1.f;
		float u = 0;
		for (int g : groups) {
			const float width = float(g) * bay;
			const float a0 = (along_x ? v.x : v.y) + u, c0 = (along_x ? v.y : v.x) + s0, lo = std::min(c0, c0 + inward * row_depth);
			FlatCell f;
			f.x = along_x ? a0 : lo; f.y = along_x ? lo : a0;
			f.w = along_x ? width : row_depth; f.d = along_x ? row_depth : width;
			f.ox = along_x ? a0 : c0; f.oy = along_x ? c0 : a0;
			f.tx = along_x ? 1.f : 0.f; f.ty = along_x ? 0.f : 1.f;
			f.nx = along_x ? 0.f : inward; f.ny = along_x ? inward : 0.f;
			f.width = width; f.depth = row_depth; f.bay = bay;
			plan.flats.push_back(f);
			u += width;
		}
	}
	return plan;
}

Plan::Plan(const Data &tables, const Body &body, const Volume &block, int at, float module) : data(tables), b(body), v(block), level(at) {
	const FloorPlanShape &K = tables.shapes.floor_plan;
	x0 = v.x + K.inset; y0 = v.y + K.inset; x1 = v.x + v.w - K.inset; y1 = v.y + v.d - K.inset;
	if (v.outline.empty()) ring_ = { { v.x, v.y }, { v.x + v.w, v.y }, { v.x + v.w, v.y + v.d }, { v.x, v.y + v.d } };
	else
		for (const V2 &q : v.outline) ring_.push_back({ v.x + q.x * v.w, v.y + q.y * v.d });
	for (const Core &c : b.cores)
		if (c.to >= level) cores_.push_back({ c.x - K.core_x, c.y - K.core_y, c.x + core_width(c, module) + K.core_x, c.y + module + K.core_x });
}

float Plan::y_start() const { return y0 + (level == 0 ? data.shapes.floor_plan.front_room : 0); }

bool Plan::inside(float x, float y) const { return in_ring(ring_, x, y); }

bool Plan::free(float x, float y, float w, float d, float angle) const {
	const FloorPlanShape &K = data.shapes.floor_plan;
	const bool t = turned(angle);
	const float hw = (t ? d : w) / 2, hd = (t ? w : d) / 2;
	if (!inside(x - hw, y - hd) || !inside(x + hw, y - hd) || !inside(x + hw, y + hd) || !inside(x - hw, y + hd)) return false;
	for (const std::array<float, 4> &c : cores_)
		if (x + hw > c[0] && x - hw < c[2] && y + hd > c[1] && y - hd < c[3]) return false;
	for (int gx = cell_of(x - hw); gx <= cell_of(x + hw); gx++)
		for (int gy = cell_of(y - hd); gy <= cell_of(y + hd); gy++) {
			auto found = filed_.find(cell_key(gx, gy));
			if (found == filed_.end()) continue;
			for (int i : found->second) {
				const Furniture &it = items[size_t(i)];
				const bool t2 = turned(it.angle);
				const float iw = (t2 ? it.d : it.w) / 2 + K.pad, id = (t2 ? it.w : it.d) / 2 + K.pad;
				if (std::abs(it.x - x) < hw + iw && std::abs(it.y - y) < hd + id) return false;
			}
		}
	return true;
}

bool Plan::put(FurnitureKind kind, float x, float y, float angle, float scale) {
	const FloorPlanShape &K = data.shapes.floor_plan;
	if (walls_only) return false;
	const float *size = data.interior.size[kind];
	const float fw = size[0] * scale, fd = size[1] * scale;
	if (!free(x, y, fw, fd, angle)) return false;
	items.push_back({ kind, x, y, fw, fd, size[2], angle });
	const bool t = turned(angle);
	const float iw = (t ? fd : fw) / 2 + K.pad, id = (t ? fw : fd) / 2 + K.pad;
	for (int gx = cell_of(x - iw); gx <= cell_of(x + iw); gx++)
		for (int gy = cell_of(y - id); gy <= cell_of(y + id); gy++) filed_[cell_key(gx, gy)].push_back(int(items.size()) - 1);
	return true;
}

void Plan::grid(FurnitureKind kind, float ax, float ay, float bx, float by, float gx, float gy, float angle) {
	for (float y = ay + gy / 2; y <= by - gy / 2 + GRID_EPS; y += gy)
		for (float x = ax + gx / 2; x <= bx - gx / 2 + GRID_EPS; x += gx) put(kind, x, y, angle);
}

void Plan::along_back(FurnitureKind kind, float from, float to, float gap) {
	const FloorPlanShape &K = data.shapes.floor_plan;
	const float y = y1 - data.interior.size[kind][1] / 2 - K.off_wall;
	for (float x = from; x <= to + GRID_EPS; x += gap) put(kind, x, y, PI_F);
}

void Plan::along_side(FurnitureKind kind, bool left, float from, float to, float gap) {
	const FloorPlanShape &K = data.shapes.floor_plan;
	const float d = data.interior.size[kind][1];
	const float x = left ? x0 + d / 2 + K.off_wall : x1 - d / 2 - K.off_wall;
	for (float y = from; y <= to + GRID_EPS; y += gap) put(kind, x, y, left ? -RIGHT_ANGLE : RIGHT_ANGLE);
}

void Plan::wall_across(float y, float ax, float bx, float door) {
	if (door - HALF_DOOR > ax) wall(ax, y, door - HALF_DOOR, y);
	if (door + HALF_DOOR < bx) wall(door + HALF_DOOR, y, bx, y);
}

void Plan::wall_along(float x, float ay, float by, float door) {
	if (door - HALF_DOOR > ay) wall(x, ay, x, door - HALF_DOOR);
	if (door + HALF_DOOR < by) wall(x, door + HALF_DOOR, x, by);
}

void RoomCell::put(FurnitureKind kind, float u, float s, float du, float ds) const {
	const float fx = f.tx * du + f.nx * ds, fy = f.ty * du + f.ny * ds;
	p.put(kind, f.ox + f.tx * u + f.nx * s, f.oy + f.ty * u + f.ny * s, std::atan2(fx, -fy));
}

void RoomCell::wall(float u0, float s0, float u1, float s1) const {
	p.wall(f.ox + f.tx * u0 + f.nx * s0, f.oy + f.ty * u0 + f.ny * s0, f.ox + f.tx * u1 + f.nx * s1, f.oy + f.ty * u1 + f.ny * s1);
}

} // namespace city::b40
