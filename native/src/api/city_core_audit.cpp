// The city's audit of its lots (CityCore::lot_audit, for tests/audit/city_audit.gd): what a picture shows as grass round a building, a
// front standing off its footway, a building on the paving or in another, read exactly from the records and the bodies that are
// drawn, each defect with where it stands. Nothing here is drawn or compared by eye: a defect is a measure of the state.
#include "api/city_core.h"

#include "sim/buildings/b40/civic.h"
#include "sim/buildings/b40/lot_layout.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/b40/types.h"

#include <algorithm>
#include <cmath>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float CELL_M = 1.f; // a lot's ground is read on cells of a metre: its covers are laid in whole and half metres
constexpr double REACH_M = 60; // a lot's street is looked for this far from its front: past any footway and half a carriageway
constexpr double FAR_M = 1e9; // no street within reach
constexpr int WORST_KEPT = 8; // the worst places of each defect, listed with where they stand
constexpr float TOUCH_M = 0.05f; // two bodies this far into one another only touch (the rounding of their corners)

// From (x, z) to the nearest street's paving edge on the point's side (under 0: on the paving), or FAR_M with no street in reach.
double paving_gap(const RoadNetwork &roads, double x, double z) {
	double t = 0, dist = 0;
	const int sid = roads.find_segment_near(x, z, REACH_M, t, dist);
	if (sid < 0) return FAR_M;
	const RoadSegment &g = roads.segs[size_t(sid)];
	double px, pz, dx, dz;
	g.q.at(t, px, pz);
	g.q.deriv(t, dx, dz);
	const Profile &p = roads.profile_of(sid);
	return dist - ((x - px) * dz - (z - pz) * dx > 0 ? p.left : p.right);
}

// A place and its measure, the worst first.
struct Worst {
	std::vector<std::pair<double, Vector3>> at;
	void add(double measure, const Vector3 &where) {
		at.push_back({ measure, where });
		std::sort(at.begin(), at.end(), [](const auto &a, const auto &b) { return a.first > b.first; });
		if (at.size() > size_t(WORST_KEPT)) at.resize(size_t(WORST_KEPT));
	}
	Array list() const {
		Array out;
		for (const auto &w : at) out.push_back(Array::make(w.first, w.second));
		return out;
	}
};

// The body's ground blocks' box in the lot's frame (x along the front from its middle, y into the lot); false with none.
bool ground_box(const b40::Body &body, b40::Rect &box) {
	bool any = false;
	for (const b40::Volume &v : body.volumes) {
		if (v.base != 0) continue;
		const b40::Rect r{ v.x, v.y, v.x + v.w, v.y + v.d };
		box = any ? b40::Rect{ std::min(box.x0, r.x0), std::min(box.y0, r.y0), std::max(box.x1, r.x1), std::max(box.y1, r.y1) } : r;
		any = true;
	}
	return any;
}

// The lot's ground cell by cell: under a block of the building, under the topmost cover (a lawn or a paved one), or bare (the
// terrain's own grass shows).
struct Ground {
	int built = 0, lawn = 0, paved = 0, bare = 0;
};
Ground ground_of(const b40::Body &body, float w, float d) {
	Ground g;
	for (float y = CELL_M / 2; y < d; y += CELL_M)
		for (float x = -w / 2 + CELL_M / 2; x < w / 2; x += CELL_M) {
			bool built = false;
			for (const b40::Volume &v : body.volumes) built = built || (v.base == 0 && x >= v.x && x <= v.x + v.w && y >= v.y && y <= v.y + v.d);
			if (built) {
				g.built++;
				continue;
			}
			const b40::LotElement *top = nullptr;
			for (const b40::LotElement &e : body.elements)
				if (e.kind == b40::EL_SURFACE && e.angle == 0 && std::fabs(x - e.x) <= e.w / 2 && std::fabs(y - e.y) <= e.d / 2 && (!top || e.h > top->h)) top = &e;
			if (!top) g.bare++;
			else if (top->paint >= 0 && body.paints[size_t(top->paint)].finish == b40::FIN_NONE) g.lawn++;
			else g.paved++;
		}
	return g;
}

struct Box { // a body's ground box in the world: its middle, its two half axes
	Vector2 c, u, v;
};
// Whether two boxes are more than TOUCH_M into one another (separating axes of the two).
bool overlap(const Box &a, const Box &b) {
	for (const Box *box : { &a, &b })
		for (const Vector2 &axis : { box->u.normalized(), box->v.normalized() }) {
			const float ra = std::fabs(a.u.dot(axis)) + std::fabs(a.v.dot(axis)), rb = std::fabs(b.u.dot(axis)) + std::fabs(b.v.dot(axis));
			if (std::fabs((b.c - a.c).dot(axis)) > ra + rb - TOUCH_M) return false;
		}
	return true;
}
} // namespace

// Every grown building's lot: `lots` read; by kind the ground's square metres built, paved, lawn and bare; the fronts off their
// footway (front_gap_m: how far the lot's front line stands from the paving it faces, the worst places); the bodies on a street's
// paving (on_paving: how far in) and in one another (in_another); the lots whose ground is mostly lawn though no house (lawn_lots:
// the share), each list the worst WORST_KEPT as [measure, where].
Dictionary CityCore::lot_audit() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const b40::Data &designs = s.buildings.designs;
	Worst gaps, on_paving, lawn_lots;
	Array kinds;
	std::vector<Ground> by_kind;
	by_kind.resize(b40::KIND_COUNT);
	std::vector<Box> boxes(s.buildings.items.size());
	std::vector<char> has_box(s.buildings.items.size(), 0);
	int lots = 0, off_front = 0, paved_over = 0, pairs = 0;
	Worst in_another;
	for (size_t id = 0; id < s.buildings.items.size(); id++) {
		const Building &rec = s.buildings.items[id];
		if (!rec.alive || b40::is_open_lot(rec.fn)) continue;
		const std::shared_ptr<const b40::Body> body = b40::record_body(designs, rec);
		if (!body) continue;
		lots++;
		const b40::LotFrame f = b40::lot_frame(rec);
		const auto world = [&](float x, float y) {
			const V2 p = f.at(x, y);
			return Vector3(p.x, 0, p.y);
		};
		// the front line's ends and middle against the paving they face
		double gap = 0;
		for (const float x : { -rec.w / 2, 0.f, rec.w / 2 }) {
			const Vector3 p = world(x, 0);
			const double here = paving_gap(s.roads, p.x, p.z);
			if (here < FAR_M) gap = std::max(gap, here);
		}
		if (gap > s.catalog.growth.shapes.lots.paving_gap + TOUCH_M) {
			off_front++;
			gaps.add(gap, world(0, 0));
		}
		const Ground g = ground_of(*body, rec.w, rec.d);
		Ground &sum = by_kind[size_t(body->kind)];
		sum.built += g.built; sum.lawn += g.lawn; sum.paved += g.paved; sum.bare += g.bare;
		const int all = g.built + g.lawn + g.paved + g.bare;
		if (body->kind != b40::KIND_HOUSE && all > 0 && 2 * (g.lawn + g.bare) > all) lawn_lots.add(double(g.lawn + g.bare) / all, world(0, rec.d / 2));
		b40::Rect box;
		if (!ground_box(*body, box)) continue;
		double deepest = 0;
		for (const float x : { box.x0, box.x1 })
			for (const float y : { box.y0, box.y1 }) {
				const Vector3 p = world(x, y);
				deepest = std::min(deepest, paving_gap(s.roads, p.x, p.z));
			}
		if (deepest < -double(TOUCH_M)) {
			paved_over++;
			on_paving.add(-deepest, world((box.x0 + box.x1) / 2, (box.y0 + box.y1) / 2));
		}
		const V2 c = f.at((box.x0 + box.x1) / 2, (box.y0 + box.y1) / 2), u = f.dir((box.x1 - box.x0) / 2, 0), v = f.dir(0, (box.y1 - box.y0) / 2);
		boxes[id] = { Vector2(c.x, c.y), Vector2(u.x, u.y), Vector2(v.x, v.y) };
		has_box[id] = 1;
	}
	for (size_t id = 0; id < boxes.size(); id++) {
		if (!has_box[id]) continue;
		const Box &a = boxes[id];
		const float r = a.u.length() + a.v.length();
		s.buildings.query(a.c.x - r - float(REACH_M), a.c.y - r - float(REACH_M), a.c.x + r + float(REACH_M), a.c.y + r + float(REACH_M), [&](int other) {
			if (size_t(other) <= id || !has_box[size_t(other)] || !overlap(a, boxes[size_t(other)])) return;
			pairs++;
			in_another.add(double(a.c.distance_to(boxes[size_t(other)].c)), Vector3(a.c.x, 0, a.c.y));
		});
	}
	for (int k = 0; k < b40::KIND_COUNT; k++) {
		const Ground &g = by_kind[size_t(k)];
		kinds.push_back(Array::make(g.built, g.paved, g.lawn, g.bare));
	}
	Dictionary d;
	d["lots"] = lots;
	d["ground_m2_by_kind"] = kinds; // [built, paved, lawn, bare] a kind, in b40::Kind's order
	d["fronts_off"] = off_front;
	d["front_gap_m"] = gaps.list();
	d["on_paving"] = paved_over;
	d["on_paving_m"] = on_paving.list();
	d["in_another"] = pairs;
	d["in_another_at"] = in_another.list();
	d["lawn_lots"] = lawn_lots.list();
	return d;
}

} // namespace city
