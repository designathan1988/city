// `probes` area (`bodies_audit`): every piece of the street of the whole map as a body standing in the world
// (sim/roads/standing_body.h), whichever segment placed it, and every two of them standing nearer than the hand's gap two pieces of
// the street keep (items.gap_m): how many, by the two kinds, and the first of each pair of kinds with where they stand. What a
// player sees as a hydrant against a post or a bench against a pole (log/pendencias.md V2, V3).
#include "api/city_probes.h"

#include "sim/roads/standing_body.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {

constexpr int BESIDE = 1; // a cell and the cells beside it

struct Piece {
	StandingBody body;
	FurnitureSpot spot;
	std::vector<BodyBox> boxes; // the piece in volume: its parts as boxes
};

// The cell a body's middle is filed in: cells wider than the longest body and the gap, so two that meet are in neighbouring cells.
std::pair<int, int> cell_of(const V2 &at, float cell) { return { int(std::floor(at.x / cell)), int(std::floor(at.y / cell)) }; }

Array told(const Piece &p) { // a piece of a case: its kind, its segment, its arc and place across, where it stands
	return Array::make(int(p.spot.kind), p.spot.seg, p.spot.v, p.spot.u, Vector2(p.body.at.x, p.body.at.y));
}

} // namespace

Dictionary CityProbes::bodies_audit(float clearance_m) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const float gap = s.catalog.furniture.items.gap_m;
	std::vector<Piece> pieces;
	std::vector<FurnitureSpot> spots;
	float longest = 0, reach = 0;
	int boxes = 0;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		spots.clear();
		traffic::street_furniture_of(s, sid, FURNITURE_ALL, spots);
		for (const FurnitureSpot &p : spots) {
			Piece piece{ body_of(s.roads, p), p, {} };
			body_boxes(s.roads, s.catalog.furniture, p, piece.boxes);
			reach = std::max(reach, boxes_reach(piece.boxes, piece.body.at));
			boxes += int(piece.boxes.size());
			longest = std::max({ longest, 2 * std::hypot(p.half_v, p.half_u), 2 * reach });
			pieces.push_back(std::move(piece));
		}
	}
	const float cell = longest + std::max(gap, clearance_m);
	std::map<std::pair<int, int>, std::vector<int>> grid;
	for (int i = 0; i < int(pieces.size()); i++) grid[cell_of(pieces[size_t(i)].body.at, cell)].push_back(i);
	int near = 0, inside = 0, volume = 0;
	Dictionary by_kinds, cases, volume_by_kinds, volume_cases;
	for (int i = 0; i < int(pieces.size()); i++) {
		const Piece &a = pieces[size_t(i)];
		const auto [cx, cz] = cell_of(a.body.at, cell);
		for (int ix = cx - BESIDE; ix <= cx + BESIDE; ix++)
			for (int iz = cz - BESIDE; iz <= cz + BESIDE; iz++) {
				const auto found = grid.find({ ix, iz });
				if (found == grid.end()) continue;
				for (int k : found->second) {
					if (k <= i) continue; // each pair once
					const Piece &b = pieces[size_t(k)];
					const Vector2i kinds(std::min(int(a.spot.kind), int(b.spot.kind)), std::max(int(a.spot.kind), int(b.spot.kind)));
					// in volume: a part of one through a part of the other, at whatever height (a signal's arm through a pole)
					const float through = volumes_apart(a.boxes, b.boxes);
					if (through <= clearance_m) {
						volume++;
						volume_by_kinds[kinds] = int(volume_by_kinds.get(kinds, 0)) + 1;
						if (!volume_cases.has(kinds)) volume_cases[kinds] = Array::make(told(a), told(b), through);
					}
					const float apart = bodies_apart(a.body, b.body);
					if (apart >= gap - BODY_ROUNDING_M) continue;
					near++;
					inside += apart <= 0 ? 1 : 0;
					by_kinds[kinds] = int(by_kinds.get(kinds, 0)) + 1;
					if (!cases.has(kinds)) cases[kinds] = Array::make(told(a), told(b), apart);
				}
			}
	}
	Dictionary d;
	d["bodies"] = int(pieces.size());
	d["gap_m"] = gap;
	d["near"] = near; // pairs nearer than the gap, those in one another among them
	d["inside"] = inside;
	d["by_kinds"] = by_kinds; // (the lower kind, the higher): pairs
	d["cases"] = cases; // (the lower kind, the higher): the first pair, each piece told, and how far apart
	d["boxes"] = boxes; // the pieces' parts, each a box
	d["reach_m"] = reach; // how far a piece's parts reach from its foot in plan, at the most
	d["volume"] = volume; // pairs with a part of one through a part of the other, or nearer it than `clearance_m`
	d["volume_by_kinds"] = volume_by_kinds;
	d["volume_cases"] = volume_cases; // the first pair of each two kinds, and how deep
	return d;
}

} // namespace city
