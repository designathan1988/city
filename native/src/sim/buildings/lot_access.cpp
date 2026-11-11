// A lot's gates, bays and docks with the way out of each (lot_access.h).
#include "sim/buildings/lot_access.h"

#include "sim/buildings/b40/record.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>

namespace city {

using namespace b40;

namespace {
constexpr float YARD_CELL = 0.25f; // grid cell (m): a gate a car fits through is a few cells wide
constexpr float OUTSIDE = 2.f; // the grid reaches this far out over the footway, past the front gates
constexpr float GATE_IN = 2.f; // a gate's way starts this far inside it
constexpr float LEAD = 1.f; // a bay's way starts this far in front of its open end
constexpr int FREE_LOOK = 6; // cells looked round a point for a free one
constexpr float GARAGE_WALL = 0.2f; // a garage's walls (its door side is open)
constexpr float LOOK_STEP = 0.5f; // cells between two looks along a line of sight
constexpr float HALF = 0.5f;
const float NONE = std::numeric_limits<float>::infinity();

bool solid(const LotElement &e, float headroom) {
	if (e.z >= headroom) return false; // over a car (a carport's roof, an awning, a wall lamp)
	switch (e.kind) {
		case EL_PAVEMENT: case EL_SURFACE: case EL_STALL: case EL_DOCK: case EL_DRAIN: case EL_CAR_GATE: case EL_CANOPY: case EL_AWNING: return false;
		default: return e.h > 0;
	}
}

bool is_garage(const Volume &v) {
	auto it = v.bays.find(bay_key(0, 0, 0));
	return v.keep && it != v.bays.end() && it->second == BAY_GARAGE_DOOR;
}

V2 opening(int facing) {
	switch (facing) {
		case FACE_STREET: return V2(0, -1);
		case FACE_BACK: return V2(0, 1);
		case FACE_RIGHT: return V2(1, 0);
		default: return V2(-1, 0);
	}
}

struct Grid {
	float x0 = 0, y0 = 0;
	int nx = 0, ny = 0;
	std::vector<uint8_t> blocked;
	std::vector<float> dist;
	std::vector<int> gate;
	int at(int i, int j) const { return j * nx + i; }
	bool inside(int i, int j) const { return i >= 0 && j >= 0 && i < nx && j < ny; }
	int cell_of(float x, float y) const { return at(int(std::floor((x - x0) / YARD_CELL)), int(std::floor((y - y0) / YARD_CELL))); }
	V2 centre(int c) const { return V2(x0 + (float(c % nx) + HALF) * YARD_CELL, y0 + (float(c / nx) + HALF) * YARD_CELL); }
	void block(float ax, float ay, float bx, float by) {
		int i0 = std::max(0, int(std::floor((ax - x0) / YARD_CELL))), i1 = std::min(nx - 1, int(std::floor((bx - x0) / YARD_CELL)));
		int j0 = std::max(0, int(std::floor((ay - y0) / YARD_CELL))), j1 = std::min(ny - 1, int(std::floor((by - y0) / YARD_CELL)));
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++) blocked[size_t(at(i, j))] = 1;
	}
	// The free cell nearest a point within FREE_LOOK cells, or -1.
	int free_near(float x, float y) const {
		int ci = int(std::floor((x - x0) / YARD_CELL)), cj = int(std::floor((y - y0) / YARD_CELL)), best = -1, best_d = FREE_LOOK * FREE_LOOK + 1;
		for (int dj = -FREE_LOOK; dj <= FREE_LOOK; dj++)
			for (int di = -FREE_LOOK; di <= FREE_LOOK; di++) {
				int i = ci + di, j = cj + dj;
				if (!inside(i, j) || blocked[size_t(at(i, j))] || di * di + dj * dj >= best_d) continue;
				best = at(i, j);
				best_d = di * di + dj * dj;
			}
		return best;
	}
	// The eight cells round `c` a step can reach (no corner cut past a blocked cell when `strict`), with its length.
	template <typename F>
	void steps(int c, bool strict, F &&fn) const {
		const int i = c % nx, j = c / nx;
		const float diag = YARD_CELL * std::sqrt(2.f);
		for (int dj = -1; dj <= 1; dj++)
			for (int di = -1; di <= 1; di++) {
				if ((!di && !dj) || !inside(i + di, j + dj)) continue;
				if (strict && di && dj && (blocked[size_t(at(i + di, j))] || blocked[size_t(at(i, j + dj))])) continue;
				fn(at(i + di, j + dj), di && dj ? diag : YARD_CELL);
			}
	}
	bool sees(const V2 &a, const V2 &b) const {
		float len = (b - a).len();
		int n = std::max(1, int(std::ceil(len / (YARD_CELL * LOOK_STEP))));
		for (int k = 0; k <= n; k++) {
			V2 p = lerp_v2(a, b, float(k) / float(n));
			int i = int(std::floor((p.x - x0) / YARD_CELL)), j = int(std::floor((p.y - y0) / YARD_CELL));
			if (!inside(i, j) || blocked[size_t(at(i, j))]) return false;
		}
		return true;
	}
};

// The lot's grid with every solid thing grown by `half`: the cells a vehicle's centre may stand on.
Grid grid_of(const Data &data, const Body &b, float half) {
	Grid g;
	g.x0 = -b.plan.W * HALF;
	g.y0 = -OUTSIDE;
	g.nx = int(std::ceil(b.plan.W / YARD_CELL));
	g.ny = int(std::ceil((b.plan.D + OUTSIDE) / YARD_CELL));
	g.blocked.assign(size_t(g.nx * g.ny), 0);
	for (const Volume &v : b.volumes) {
		if (v.base != 0) continue;
		if (!is_garage(v)) { g.block(v.x - half, v.y - half, v.x + v.w + half, v.y + v.d + half); continue; }
		g.block(v.x - half, v.y - half, v.x + GARAGE_WALL + half, v.y + v.d + half);
		g.block(v.x + v.w - GARAGE_WALL - half, v.y - half, v.x + v.w + half, v.y + v.d + half);
		g.block(v.x - half, v.y + v.d - GARAGE_WALL - half, v.x + v.w + half, v.y + v.d + half);
	}
	for (const LotElement &e : b.elements)
		if (solid(e, data.lots.car_clear_h)) g.block(e.x - e.w * HALF - half, e.y - e.d * HALF - half, e.x + e.w * HALF + half, e.y + e.d * HALF + half);
	return g;
}

// Distance from the car gates over the free cells (Dijkstra, eight neighbours, no corner cutting).
void spread(Grid &g, const std::vector<std::pair<int, int>> &sources) {
	g.dist.assign(g.blocked.size(), NONE);
	g.gate.assign(g.blocked.size(), -1);
	using Item = std::pair<float, int>;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
	for (const auto &s : sources) {
		g.dist[size_t(s.first)] = 0;
		g.gate[size_t(s.first)] = s.second;
		open.push({ 0.f, s.first });
	}
	while (!open.empty()) {
		const float d = open.top().first;
		const int c = open.top().second;
		open.pop();
		if (d > g.dist[size_t(c)]) continue;
		g.steps(c, true, [&](int n, float step) {
			if (g.blocked[size_t(n)] || d + step >= g.dist[size_t(n)]) return;
			g.dist[size_t(n)] = d + step;
			g.gate[size_t(n)] = g.gate[size_t(c)];
			open.push({ d + step, n });
		});
	}
}

// The way down the distances from cell `c` to its gate, pulled straight where the line of sight is free (lot frame).
std::vector<V2> walk_down(const Grid &g, int c) {
	std::vector<V2> cells{ g.centre(c) };
	while (g.dist[size_t(c)] > 0) {
		int next = -1;
		float best = g.dist[size_t(c)];
		g.steps(c, false, [&](int n, float) {
			if (g.dist[size_t(n)] < best) { best = g.dist[size_t(n)]; next = n; }
		});
		if (next < 0) break;
		c = next;
		cells.push_back(g.centre(c));
	}
	std::vector<V2> way{ cells.front() };
	for (size_t k = 0; k + 1 < cells.size();) {
		size_t far = k + 1;
		for (size_t n = cells.size() - 1; n > k + 1; n--)
			if (g.sees(cells[k], cells[n])) { far = n; break; }
		way.push_back(cells[far]);
		k = far;
	}
	return way;
}

// The ways of the bays (or, with `docks`, of the docks) to the car gates. `of`: each bay's element.
void find_ways(const Data &data, const Body &b, const LotFrame &f, float half, bool docks, const std::vector<int> &of, LotAccess &out) {
	Grid g = grid_of(data, b, half);
	std::vector<std::pair<int, int>> sources;
	std::vector<V2> gate_in(out.gates.size());
	int k = 0;
	for (const LotElement &e : b.elements) {
		if (e.kind != EL_GATE && e.kind != EL_CAR_GATE) continue;
		if (e.kind == EL_CAR_GATE) {
			gate_in[size_t(k)] = V2(e.x, e.y + GATE_IN);
			int c = g.free_near(e.x, e.y + GATE_IN);
			if (c >= 0) sources.push_back({ c, k });
		}
		k++;
	}
	if (sources.empty()) return;
	spread(g, sources);
	for (size_t i = 0; i < out.bays.size(); i++) {
		LotBay &bay = out.bays[i];
		if (bay.dock != docks) continue;
		const LotElement *e = &b.elements[size_t(of[i])];
		V2 open = opening(e->facing);
		V2 lead = V2(e->x, e->y) + open * ((e->facing == FACE_STREET || e->facing == FACE_BACK ? e->d : e->w) * HALF + LEAD);
		int c = g.free_near(lead.x, lead.y);
		if (c < 0 || g.dist[size_t(c)] == NONE) continue;
		bay.gate = g.gate[size_t(c)];
		for (const V2 &p : walk_down(g, c)) bay.way.push_back(f.at(p.x, p.y));
		const V2 &in = gate_in[size_t(bay.gate)];
		bay.way.push_back(f.at(in.x, in.y));
	}
}
} // namespace

float car_half_width(const TrafficParams &p) {
	float h = 0;
	for (int t = int(VehicleType::SEDAN); t <= int(VehicleType::PICKUP); t++) h = std::max(h, p.classes[t].half_width);
	return h;
}

float lorry_half_width(const TrafficParams &p) { return p.classes[int(VehicleType::TRUCK)].half_width; }

int LotAccess::reached() const {
	int n = 0;
	for (const LotBay &b : bays) n += b.gate >= 0 ? 1 : 0;
	return n;
}

void street_gates(const RoadNetwork &net, const BuildingRecords &lots, const Data &designs, int sid, std::vector<ArcSpan> gates[2]) {
	const RoadSegment &g = net.segs[size_t(sid)];
	const TrafficParams &tp = net.cat->traffic;
	const Profile &p = net.profile_of(sid);
	const float reach = std::max(p.left, p.right) + tp.kerb_gate_reach_m;
	lots.query(g.minx - reach, g.minz - reach, g.maxx + reach, g.maxz + reach, [&](int id) {
		const Building &rec = lots.items[size_t(id)];
		const std::shared_ptr<const std::vector<LotElement>> elements = record_elements(designs, rec);
		const LotFrame f = lot_frame(rec);
		for (const LotElement &e : *elements) {
			if (e.kind != EL_CAR_GATE) continue;
			const V2 w = f.at(e.x, e.y);
			double t, dist, x, z, dx, dz;
			closest_t(g.q, w.x, w.y, t, dist);
			if (dist > reach) continue;
			g.q.at(t, x, z);
			g.q.deriv(t, dx, dz);
			const int side = street_side(dz * (w.x - x) - dx * (w.y - z)); // the gate across the street's frame: +u on the left of a -> b
			const double at = arc_of_t(g, t), half = e.w * 0.5 + tp.kerb_gate_margin_m;
			gates[side].emplace_back(at - half, at + half);
		}
	});
}

void lot_bays(const std::vector<LotElement> &elements, const LotFrame &f, LotAccess &out) {
	out = LotAccess{};
	for (const LotElement &e : elements) {
		if (e.kind == EL_GATE || e.kind == EL_CAR_GATE) out.gates.push_back({ f.at(e.x, e.y), f.dir(0, 1), e.w, e.kind == EL_CAR_GATE });
		if (e.kind != EL_STALL && e.kind != EL_DOCK) continue;
		LotBay bay;
		V2 open = opening(e.facing);
		bay.at = f.at(e.x, e.y);
		bay.open = f.dir(open.x, open.y);
		bay.w = e.facing == FACE_STREET || e.facing == FACE_BACK ? e.w : e.d;
		bay.d = e.facing == FACE_STREET || e.facing == FACE_BACK ? e.d : e.w;
		bay.dock = e.kind == EL_DOCK;
		out.bays.push_back(bay);
	}
}

void lot_solids(const Data &, const Body &b, const LotFrame &f, float headroom, std::vector<std::vector<V2>> &out, std::vector<int> *kinds) {
	int kind = -1;
	auto box = [&](float x0, float y0, float x1, float y1) {
		out.push_back({ f.at(x0, y0), f.at(x1, y0), f.at(x1, y1), f.at(x0, y1) });
		if (kinds) kinds->push_back(kind);
	};
	for (const Volume &v : b.volumes) {
		if (!is_garage(v)) { box(v.x, v.y, v.x + v.w, v.y + v.d); continue; }
		box(v.x, v.y, v.x + GARAGE_WALL, v.y + v.d); // a car drives into a garage: its walls only, the door side open
		box(v.x + v.w - GARAGE_WALL, v.y, v.x + v.w, v.y + v.d);
		box(v.x, v.y + v.d - GARAGE_WALL, v.x + v.w, v.y + v.d);
	}
	for (const LotElement &e : b.elements) {
		if (!solid(e, headroom)) continue;
		kind = int(e.kind);
		box(e.x - e.w * HALF, e.y - e.d * HALF, e.x + e.w * HALF, e.y + e.d * HALF);
	}
}

void lot_access(const Data &data, const Body &b, const LotFrame &f, float car_half, float lorry_half, LotAccess &out) {
	lot_bays(b.elements, f, out);
	if (out.bays.empty()) return;
	std::vector<int> of;
	bool docks = false;
	for (int i = 0; i < int(b.elements.size()); i++)
		if (b.elements[size_t(i)].kind == EL_STALL || b.elements[size_t(i)].kind == EL_DOCK) of.push_back(i);
	for (const LotBay &bay : out.bays) docks = docks || bay.dock;
	find_ways(data, b, f, car_half, false, of, out);
	if (docks) find_ways(data, b, f, lorry_half, true, of, out);
}

} // namespace city
