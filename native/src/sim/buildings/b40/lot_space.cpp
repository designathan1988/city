// LotSpace: the lot in its own frame (edges, zones, side strips, ground volumes, front, back and loading doors) and the
// rule every part obeys - nothing solid in a way kept clear, on the building, on another part or across the lot's edge.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>
#include <cstring>

namespace city::b40 {

namespace {
constexpr float LOT_CLEAR_IN = 0.15f; // a part keeps this far inside the lot's edge (the boundary stands on it)
constexpr uint32_t BYTE = 0xff;
constexpr int STOREY_SHIFT = KEY_STOREY_SHIFT, SIDE_SHIFT = KEY_SIDE_SHIFT, BACK_SIDE = 2;
constexpr float HALF = 0.5f;
constexpr float GATE_STEP = 0.1f; // a people's gate moved off the door's line is looked for along the front this often
const char *const SOFT_COVER = "lawn"; // the one cover that is planted: every other is hard
constexpr float LAWN_LEAST = 0.6f; // a band of ground narrower than this is too thin to plant: the hard cover beside it takes it
constexpr float EDGE_BOX = 1.f; // a lot's edge stands as a box this thick just outside it, among what ends a band
constexpr size_t NO_COVER = size_t(-1);
constexpr int SLIVER_ROUNDS = 4; // a piece laid may face another across a corner: looked at again this often at the most
constexpr float POST_TRY_M = 0.1f; // a post moved out from under what hangs over it is tried this often
constexpr int ROOM_HALVINGS = 10; // of the span between a room that is clear and one that is not: to a thousandth of it

Rect footprint(const Volume &v) { return { v.x, v.y, v.x + v.w, v.y + v.d }; }
} // namespace

LotSpace::LotSpace(const Data &d, Body &body, Xoshiro128 &rng) : data(d), b(body), r(rng), rules(d.lots), plan(body.plan) {
	L = -plan.W / 2;
	R = plan.W / 2;
	D = plan.D;
	F = plan.front_depth;
	B = D - plan.back_depth;
	for (int s = 0; s < 2; s++) {
		const SidePlan &side = s == 0 ? plan.left : plan.right;
		Strip &st = strips[s];
		st.left = s == 0;
		st.use = side.use;
		float x0 = s == 0 ? L : R - side.width, x1 = s == 0 ? L + side.width : R;
		st.r = { x0, F, x1, B };
		if (side.use == SIDE_DRIVE || side.use == SIDE_DRIVE_PATH) {
			float walk = side.use == SIDE_DRIVE_PATH ? side.walk : 0;
			st.drive = s == 0 ? Rect{ x0, 0, x1 - walk, B } : Rect{ x0 + walk, 0, x1, B };
			if (walk > 0) st.walk = s == 0 ? Rect{ x1 - walk, F, x1, B } : Rect{ x0, F, x0 + walk, B };
		} else if (side.use == SIDE_PATH) {
			st.walk = st.r;
		}
	}
	for (const Volume &v : b.volumes) {
		if (v.base != 0) continue;
		Rect f = footprint(v);
		built.push_back(f);
		built_box = built_box.empty() ? f : Rect{ std::min(built_box.x0, f.x0), std::min(built_box.y0, f.y0), std::max(built_box.x1, f.x1), std::max(built_box.y1, f.y1) };
		int n = bays_of(v, BACK_SIDE);
		for (const auto &bay : v.bays) {
			int storey = int(bay.first >> STOREY_SHIFT), side = int((bay.first >> SIDE_SHIFT) & BYTE), index = int(bay.first & BYTE);
			if (storey != 0 || side != BACK_SIDE) continue;
			float x = v.x + v.w - (float(index) + HALF) * v.w / float(n); // the back face runs from +x to -x
			if (bay.second == BAY_DOOR || bay.second == BAY_DOUBLE_DOOR) { back_door = true; back_door_x = x; }
			if (bay.second == BAY_LOADING_DOOR) { loading_x.push_back(x); loading_y = v.y + v.d; }
		}
	}
	std::sort(loading_x.begin(), loading_x.end());
	own_parts = built.size();
	air_ = body_overhangs(b);
	door_x = b.entrance_x;
	door_wall = F;
	bool walled = false;
	for (const Rect &f : built)
		if (door_x >= f.x0 && door_x <= f.x1 && (!walled || f.y0 < door_wall)) {
			door_wall = f.y0;
			walled = true;
		}
	gate_x = people_gate_x();
}

float LotSpace::people_gate_x() const {
	if (F <= 0) return door_x; // no front zone: no people's gate
	const float g = rules.person_gate[b.kind == KIND_HOUSE ? 0 : 1] * HALF;
	if (!plan.edges.shut(LotEdges::WALK, door_x - g, door_x + g)) return door_x;
	const auto opens = [&](float x) {
		if (x - g < L + HEDGE_THICK || x + g > R - HEDGE_THICK || plan.edges.shut(LotEdges::WALK, x - g, x + g)) return false;
		for (const Strip &st : strips)
			if (!st.drive.empty() && x - g < st.drive.x1 && st.drive.x0 < x + g) return false;
		return true;
	};
	for (float d = GATE_STEP; d < R - L; d += GATE_STEP)
		for (float x : { door_x - d, door_x + d })
			if (opens(x)) return x;
	return door_x;
}

std::vector<Rect> LotSpace::people_way(float half, float depth) const {
	if (gate_x == door_x) return { { door_x - half, 0, door_x + half, depth } };
	const float link = door_wall * HALF;
	return { { gate_x - half, 0, gate_x + half, link - half }, { std::min(gate_x, door_x) - half, link - half, std::max(gate_x, door_x) + half, link + half },
		{ door_x - half, link + half, door_x + half, depth } };
}

int LotSpace::paint(const char *name) {
	for (const auto &p : paints_)
		if (std::strcmp(p.first, name) == 0) return p.second;
	int i = b.add_paint(data.color(name));
	paints_.push_back({ name, i });
	return i;
}

void LotSpace::add(Element k, const Rect &rc, float h, int p, int facing, float z, float base) {
	LotElement e;
	e.kind = k; e.x = rc.cx(); e.y = rc.cy(); e.w = rc.w(); e.d = rc.d(); e.h = h; e.z = z; e.paint = p;
	e.facing = facing; e.base = base;
	b.elements.push_back(e);
}

void LotSpace::cover(const Rect &rc, const char *name, CoverLayer layer) {
	Rect c{ std::max(rc.x0, L), std::max(rc.y0, 0.f), std::min(rc.x1, R), std::min(rc.y1, D) };
	if (c.empty() || full()) return;
	add(EL_SURFACE, c, cover_h(layer), paint(name), FACE_STREET, 0, 0);
	covers_.push_back({ b.elements.size() - 1, std::strcmp(name, SOFT_COVER) != 0, false });
}

void LotSpace::cover_lot(const char *name) {
	cover({ L, 0, R, D }, name);
	if (!covers_.empty()) covers_.back().whole = true;
}

void LotSpace::close_slivers() {
	const auto rect_of = [&](const Laid &c) {
		const LotElement &e = b.elements[c.element];
		return Rect::around(e.x, e.y, e.w, e.d);
	};
	// What ends a band beside a hard cover: the lot's four edges (a box just outside each), every ground volume, every other hard cover.
	struct Stop {
		Rect r;
		size_t cover; // its place among the covers, NO_COVER for an edge or a volume
	};
	const auto thin = [](float gap) { return gap > EPS && gap < LAWN_LEAST; };
	// A band some hard cover already lies over is closed.
	const auto closed = [&](const Rect &band) {
		for (const Laid &c : covers_) {
			const Rect r = rect_of(c);
			if (c.hard && !c.whole && r.x0 <= band.x0 + EPS && r.x1 >= band.x1 - EPS && r.y0 <= band.y0 + EPS && r.y1 >= band.y1 - EPS) return true;
		}
		return false;
	};
	// A piece laid is a hard cover too: where two bands meet at a corner the next round closes the square between their pieces.
	for (int round = 0; round < SLIVER_ROUNDS; round++) {
		const size_t laid = covers_.size();
		std::vector<Stop> stops{ { { L - EDGE_BOX, 0, L, D }, NO_COVER }, { { R, 0, R + EDGE_BOX, D }, NO_COVER }, { { L, -EDGE_BOX, R, 0 }, NO_COVER },
			{ { L, D, R, D + EDGE_BOX }, NO_COVER } };
		for (const Rect &f : built) stops.push_back({ f, NO_COVER });
		for (size_t i = 0; i < laid; i++)
			if (covers_[i].hard && !covers_[i].whole) stops.push_back({ rect_of(covers_[i]), i });
		for (size_t i = 0; i < laid; i++) {
			if (!covers_[i].hard || covers_[i].whole) continue;
			const Rect r = rect_of(covers_[i]);
			const LotElement like = b.elements[covers_[i].element]; // a copy: the pieces laid below grow the list
			for (const Stop &o : stops) {
				if (o.cover != NO_COVER && o.cover <= i) continue; // two covers' band is closed once, by the earlier of them
				// The band between the two where they face one another: across (a side of one against a side of the other), else along.
				const float ya = std::max(o.r.y0, r.y0), yb = std::min(o.r.y1, r.y1), xa = std::max(o.r.x0, r.x0), xb = std::min(o.r.x1, r.x1);
				Rect band;
				if (yb - ya > EPS && thin(r.x0 - o.r.x1)) band = { o.r.x1, ya, r.x0, yb };
				else if (yb - ya > EPS && thin(o.r.x0 - r.x1)) band = { r.x1, ya, o.r.x0, yb };
				else if (xb - xa > EPS && thin(r.y0 - o.r.y1)) band = { xa, o.r.y1, xb, r.y0 };
				else if (xb - xa > EPS && thin(o.r.y0 - r.y1)) band = { xa, r.y1, xb, o.r.y0 };
				if (band.empty() || full() || closed(band)) continue;
				add(EL_SURFACE, band, like.h, like.paint, FACE_STREET, 0, 0);
				covers_.push_back({ b.elements.size() - 1, true, false });
			}
		}
		if (covers_.size() == laid) break;
	}
}

bool LotSpace::on_building(const Rect &rc) const {
	for (const Rect &f : built)
		if (f.meets(rc)) return true;
	return false;
}

bool LotSpace::open_above(const Rect &rc, float top) const {
	for (const Overhang &o : air_)
		if (top > o.under && o.over(rc.x0, rc.y0, rc.x1, rc.y1)) return false;
	return true;
}

void LotSpace::block_added(const Rect &footprint) {
	built.push_back(footprint);
	air_ = body_overhangs(b);
}

void LotSpace::hang(const LotElement &piece) {
	b.elements.push_back(piece);
	air_ = body_overhangs(b);
}

bool LotSpace::ground_free(const Rect &rc) const {
	if (rc.x0 < L + LOT_CLEAR_IN || rc.x1 > R - LOT_CLEAR_IN || rc.y0 < LOT_CLEAR_IN || rc.y1 > D - LOT_CLEAR_IN) return false;
	if (on_building(rc)) return false;
	for (const Rect &w : ways_)
		if (w.meets(rc)) return false;
	Rect g = rc.grown(PART_GAP);
	for (const Rect &s : solid_)
		if (s.meets(g)) return false;
	return true;
}

bool LotSpace::put(Element k, const Rect &rc, float h, int p, int facing, float z) {
	if (full() || rc.empty() || !clear(rc, z + h)) return false;
	add(k, rc, h, p, facing, z, 0);
	solid_.push_back(rc);
	return true;
}

namespace {
// The largest share of a room, between `least` and the whole, that `fits`; 0 when not even the least does. A smaller share of a
// room, lower in proportion, lies inside a larger one: what fits at a share fits at every smaller one.
template <typename F>
float largest_share(float least, F &&fits) {
	if (fits(1.f)) return 1.f;
	if (least >= 1.f || !fits(least)) return 0.f;
	float lo = least, hi = 1.f; // lo fits, hi does not
	for (int i = 0; i < ROOM_HALVINGS; i++) {
		const float mid = (lo + hi) * HALF;
		(fits(mid) ? lo : hi) = mid;
	}
	return lo;
}
} // namespace

bool LotSpace::post(Element k, float x, float y, float side, float h, int p, float dx, float dy) {
	for (float out = 0; out <= DEEP_BALCONY_OUT_M + side; out += POST_TRY_M)
		if (put(k, Rect::around(x + dx * out, y + dy * out, side, side), h, p)) return true;
	return false;
}

bool LotSpace::plant(Element k, float x, float y, float wide, float tall, int p, float least) {
	const auto room = [&](float share) { return Rect::around(x, y, wide * share, wide * share); };
	if (full() || wide <= 0 || !ground_free(room(1.f))) return false;
	const float share = largest_share(least / wide, [&](float f) { return open_above(room(f), tall * f); });
	return share > 0 && put(k, room(share), tall * share, p);
}

bool LotSpace::planter(const Rect &box, float h, int p, Element grows, const Rect &room, float tall, int plant_paint) {
	if (!put(EL_PLANTER, box, h, p)) return false;
	const float soil = h - PLANTER_SOIL_DROP_M;
	const auto part = [&](float share) { return Rect::around(room.cx(), room.cy(), room.w() * share, room.d() * share); };
	const float least = grows == EL_TREE ? TREE_ROOM_LEAST / std::max(room.w(), room.d()) : 1.f;
	const float share = largest_share(least, [&](float f) { return open_above(part(f), soil + tall * f); });
	if (share > 0) edge(grows, part(share), tall * share, plant_paint, 0, soil);
	else {
		float head = PLANTER_SHRUB_H; // what hangs over the planter's box, over its soil: the shrub keeps under it
		for (const Overhang &o : air_)
			if (o.over(box.x0, box.y0, box.x1, box.y1)) head = std::min(head, o.under - soil);
		edge(EL_SHRUB, box, head, plant_paint, 0, soil);
	}
	grown_++;
	return true;
}

bool LotSpace::flat(Element k, const Rect &rc, float h, int p, int facing) {
	if (full() || rc.empty() || on_building(rc)) return false;
	if (rc.x0 < L || rc.x1 > R || rc.y0 < 0 || rc.y1 > D) return false;
	add(k, rc, h, p, facing, 0, 0);
	if (k == EL_STALL || k == EL_DOCK) ways_.push_back(rc);
	return true;
}

void LotSpace::edge(Element k, const Rect &rc, float h, int p, float base, float z) {
	if (rc.empty()) return;
	add(k, rc, h, p, FACE_STREET, z, base);
	solid_.push_back(rc);
}

float LotSpace::built_back(float x0, float x1) const {
	float back = F;
	for (const Rect &f : built)
		if (f.x0 < x1 && x0 < f.x1) back = std::max(back, f.y1);
	return back;
}

void lay_out_lot(const Data &data, Body &b, Xoshiro128 &r) {
	LotSpace s(data, b, r);
	lay_ground(s);
	open_boundary(s);
	lay_cars(s);
	lay_front(s);
	lay_back(s);
	lay_yard(s);
	s.close_slivers(); // every zone's covers are laid
	lay_boundary(s); // last: every block that stands on an edge (a garage) is known
}

} // namespace city::b40
