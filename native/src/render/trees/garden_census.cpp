// The gardens' plants against their rooms (see garden_census.h).
#include "render/trees/garden_census.h"

#include "sim/buildings/b40/civic.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float ROOM_SLACK_M = 0.01f; // a plant drawn this far past its room is past it: under it, the sums' rounding
constexpr float HALF = 0.5f;
constexpr float NOWHERE = 1e9f;

// What a plant draws, in its lot's frame: across (x), from the street (y), and how high above its foot.
struct Drawn {
	float x0 = NOWHERE, y0 = NOWHERE, x1 = -NOWHERE, y1 = -NOWHERE, top = 0;
	int instances = 0;
	// The instance's every model point through its basis, a card's leaves as far round the point as the shader spreads them.
	void take(const TreeKit &kit, const TreeInstance &t, const b40::LotFrame &f) {
		const TreeBasis b = tree_basis(t);
		for (const DrawnPoint &m : kit.species(t.species).drawn.points()) {
			const Vector3 w = t.p + b.x * m.x + b.y * m.y + b.z * m.z;
			const V2 rel = V2(w.x, w.z) - f.o;
			const float x = rel.dot(f.e0), y = rel.dot(f.e1), r = m.reach * t.width;
			x0 = std::min(x0, x - r); x1 = std::max(x1, x + r);
			y0 = std::min(y0, y - r); y1 = std::max(y1, y + r);
			top = std::max(top, w.y - t.p.y + r);
		}
		instances++;
	}
	// How far past the element's rectangle it stands, at the most; 0 inside it.
	float past(const b40::LotElement &p) const {
		if (instances == 0) return 0;
		return std::max({ 0.f, p.x - p.w * HALF - x0, x1 - (p.x + p.w * HALF), p.y - p.d * HALF - y0, y1 - (p.y + p.d * HALF) });
	}
	float over(const b40::LotElement &p) const { return instances == 0 ? 0 : std::max(0.f, top - p.h); }
};

const char *kind_name(b40::Element k) {
	return k == b40::EL_TREE ? "tree" : k == b40::EL_SHRUB ? "shrub" : k == b40::EL_HEDGE ? "hedge" : "flowers";
}

// Where a capture looks: of each kind, the plant whose room has the longest side and the one whose room has the shortest (a long
// hedge, a tree in a planter), as (building, x, z, the room's width, depth and height).
struct Sights {
	Dictionary where;
	void take(int id, const b40::LotFrame &f, const b40::LotElement &p, const std::vector<TreeInstance> &made) {
		const float side = std::max(p.w, p.d);
		const V2 at = f.at(p.x, p.y);
		Array here = Array::make(id, at.x, at.y, p.w, p.d, p.h, int(made.size()));
		if (!made.empty()) here.append_array(Array::make(made[0].p, made[0].width, made[0].height, made[0].depth, made[0].yaw));
		const String longest = String(kind_name(p.kind)) + "_longest", least = String(kind_name(p.kind)) + "_least";
		if (!where.has(longest) || side > room_side(where[longest])) where[longest] = here;
		if (!where.has(least) || side < room_side(where[least])) where[least] = here;
	}
	static float room_side(const Variant &seen) {
		const Array a = seen;
		return std::max(float(a[ROOM_W]), float(a[ROOM_D]));
	}
	enum : int { ROOM_W = 3, ROOM_D = 4 }; // their places in a sight
};
} // namespace

Dictionary garden_rooms(const CityView &s, const TreeLook &T, const TreeKit &kit) {
	int lots = 0, plants = 0, out = 0, over = 0, open_lots = 0, open_trees = 0;
	float worst = 0, tallest = 0;
	Array first;
	Dictionary by_kind;
	Sights sights;
	std::vector<TreeInstance> made;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const b40::LotFrame f = b40::lot_frame(rec);
		const int before = plants, trees_before = open_trees;
		const bool open = b40::is_open_lot(rec.fn);
		each_garden_plant(s, rec, [&](const b40::LotElement &p, uint32_t draws) {
			made.clear();
			place_plant(s, T, kit, f, p, draws, GardenView::NEAR, made);
			if (open && p.kind == b40::EL_TREE && !made.empty()) open_trees++;
			const PlantRoom room = plant_room(f, p); // an element laid at an angle is held in its own turned frame
			Drawn d;
			for (const TreeInstance &t : made) d.take(kit, t, room.f);
			plants++;
			sights.take(id, f, p, made);
			const float past = d.past(room.p), high = p.kind == b40::EL_FLOWERS ? 0.f : d.over(room.p);
			if (high > ROOM_SLACK_M) over++;
			tallest = std::max(tallest, high);
			if (past <= ROOM_SLACK_M) return;
			out++;
			by_kind[kind_name(p.kind)] = int(by_kind.get(kind_name(p.kind), 0)) + 1;
			if (first.is_empty()) {
				const V2 at = f.at(p.x, p.y);
				first = Array::make(id, kind_name(p.kind), at.x, at.y, past);
			}
			worst = std::max(worst, past);
		});
		if (plants > before) lots++;
		if (open_trees > trees_before) open_lots++;
	}
	Dictionary r;
	r["lots"] = lots; r["plants"] = plants; r["out"] = out; r["worst_m"] = worst; r["first"] = first; r["by_kind"] = by_kind;
	r["over"] = over; r["over_m"] = tallest;
	r["open_lots"] = open_lots; r["open_trees"] = open_trees;
	r["sights"] = sights.where;
	return r;
}

Dictionary garden_fit(const CityView &s, const TreeLook &T, const TreeKit &kit, int kind, float w, float d, float h, float turn, int seed) {
	b40::LotElement p;
	p.kind = b40::Element(kind);
	p.x = p.y = 0; p.w = w; p.d = d; p.h = h; p.angle = turn;
	const b40::LotFrame lot{ V2(0, 0), V2(1, 0), V2(0, 1) };
	std::vector<TreeInstance> made;
	if (planted_kind(p.kind)) place_plant(s, T, kit, lot, p, uint32_t(seed), GardenView::NEAR, made);
	const PlantRoom room = plant_room(lot, p);
	p = room.p;
	Drawn box, across; // in the element's own frame, and in the lot's
	for (const TreeInstance &t : made) {
		box.take(kit, t, room.f);
		across.take(kit, t, lot);
	}
	Dictionary r;
	r["instances"] = box.instances;
	r["box"] = box.instances > 0 ? Array::make(box.x0, box.y0, box.x1, box.y1) : Array();
	r["lot_box"] = box.instances > 0 ? Array::make(across.x0, across.y0, across.x1, across.y1) : Array();
	r["top"] = box.top;
	r["past_m"] = box.past(p);
	r["over_m"] = box.over(p);
	return r;
}

} // namespace city
