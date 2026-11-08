// What a blow does to the ground: the crater (recorded, and dug into the terrain on bare earth), the street furniture it
// knocks out (poles, signal posts, hydrants, benches, bins, trees: DamageState::gone), the utility poles the player built
// (taken down through the journal, the next pole pulled over by its wires now and then) with the wires they leave hanging,
// and the roads it cuts.
#include "sim/commands/strike_internal.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/nature_commands.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/roads/street_furniture.h"
#include "sim/street/pole_arms.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace city {

namespace {
constexpr float MIN_NORMAL_M = 1.f; // a direction is taken over at least a metre (no division by a near zero)
constexpr float ARC_DRAW_SCALE = 8.f; // the arc a piece stands at, as the draw's identity
constexpr uint32_t SALT_CRATER = 0x63726174U; // a stream's salt: the word it names, in ASCII
constexpr uint32_t SALT_THROW = 0x7468726fU; // a stream's salt: the word it names, in ASCII
constexpr uint32_t SALT_SNAP = 0x736e6170U; // a stream's salt: the word it names, in ASCII
constexpr uint32_t SALT_PULL = 0x70756c6cU; // a stream's salt: the word it names, in ASCII
constexpr uint32_t POLE_WHO_BASE = 0x40000000U; // a utility pole's draws, apart from the street furniture's (by its arc)

// How a pole at `d` from the burst goes (`who` its draw): one pulled over from past the reach falls whole; near the burst it
// bursts into splinters now and then; else it snaps or falls whole.
float pole_fate(const Blow &b, float d, float reach, uint32_t who) {
	const Tuning &t = b.s.catalog.tune;
	if (d > reach) return FURNITURE_WHOLE;
	if (d < reach * t.strike_splinter_share && b.roll(SALT_THROW, who) < t.strike_splinter_chance) return FURNITURE_SPLINTERS;
	return b.roll(SALT_SNAP, who) < t.strike_pole_snap_chance ? FURNITURE_SNAPPED : FURNITURE_WHOLE;
}

// A wire torn loose, hanging from the pole at (x, z) that stands (its seat line `arm`) to the fallen one at (tx, tz).
void hang_wire(const Blow &b, float x, float z, ArmDir arm, float tx, float tz, bool rural) {
	DamageEvent e;
	e.kind = DamageEvent::WIRE;
	e.x = x; e.z = z; e.y = b.s.terrain.height_at(x, z);
	e.dx = arm.x; e.dz = arm.z;
	e.b = tx; e.c = tz; e.d = b.s.terrain.height_at(tx, tz);
	e.a = std::atan2(tz - b.z, tx - b.x); // the way out from the burst at the fallen end, which the wire is thrown
	e.u = rural ? 1u : 0u;
	b.emit(e);
}

// The wires of a lamp row cut at the stations knocked out: each span to a neighbour left standing hangs from it.
void hang_row_wires(const Blow &b, int sid, const std::vector<FurnitureSpot> &spots, const std::vector<char> &knocked) {
	for (size_t i = 0; i < spots.size(); i++) {
		const FurnitureSpot &f = spots[i];
		if (!knocked[i] || f.kind != FurnitureKind::LAMP || f.row != 0) continue;
		float tx, tz;
		if (!traffic::furniture_xz(b.s, sid, f, tx, tz)) continue;
		for (size_t j = 0; j < spots.size(); j++) {
			const FurnitureSpot &o = spots[j];
			if (knocked[j] || o.kind != FurnitureKind::LAMP || o.row != 0 || std::abs(o.station - f.station) != 1) continue;
			float x, z, fx, fz;
			if (traffic::furniture_facing(b.s, sid, o, x, z, fx, fz)) hang_wire(b, x, z, ArmDir{ fx, fz }, tx, tz, false);
		}
	}
}

// A lamp pole knocked out at the end of its street's row, at node `node`, where it stood.
struct RowEnd {
	int sid = 0, node = 0;
	float x = 0, z = 0;
};

// A lamp row going on through its node (a street of two arms there) is strung from the last pole of one street to the first of
// the next (the render's join_rows): each such span that lost a pole at one end hangs from the pole standing at the other, read
// after the whole blow (a pole knocked out on the next street leaves nothing to hang from).
void hang_joined_wires(const Blow &b, const std::vector<RowEnd> &ends) {
	std::vector<FurnitureSpot> spots;
	for (const RowEnd &k : ends) {
		const std::vector<int> &arms = b.s.roads.nodes[size_t(k.node)].segs;
		if (arms.size() != JOINT_ARMS) continue;
		const int other = arms[0] == k.sid ? arms[1] : arms[0];
		const RoadSegment &g = b.s.roads.segs[size_t(k.sid)], &o = b.s.roads.segs[size_t(other)];
		const LampRow mine = lamp_row(b.s.roads, k.sid), theirs = lamp_row(b.s.roads, other);
		const bool same_way = (g.b == k.node) == (o.a == k.node); // the sides keep their names through the node
		if (!theirs.any || theirs.stations == 0 || theirs.side != (same_way ? mine.side : 1 - mine.side)) continue;
		const int station = o.a == k.node ? 0 : theirs.stations - 1;
		spots.clear();
		traffic::street_furniture_of(b.s, other, furniture_bit(FurnitureKind::LAMP), spots, FurnitureRead::PLACED);
		for (const FurnitureSpot &p : spots) {
			float x, z, fx, fz;
			if (p.row == 0 && p.station == station && traffic::furniture_facing(b.s, other, p, x, z, fx, fz)) hang_wire(b, x, z, ArmDir{ fx, fz }, k.x, k.z, false);
		}
	}
}

// Poles, posts, trees and the rest within the overpressure that topples them.
void knock_out_furniture(Blow &b, float reach) {
	std::vector<FurnitureSpot> spots;
	std::vector<char> knocked;
	std::vector<RowEnd> ends; // the lamps knocked out at a row's end, for the spans through their nodes
	b.s.roads.for_segments_near(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(b.s, sid, FURNITURE_ALL, spots, FurnitureRead::PLACED); // the blow knocks pieces out as it goes
		knocked.assign(spots.size(), 0);
		for (size_t i = 0; i < spots.size(); i++) {
			const FurnitureSpot &f = spots[i];
			float x, z;
			if (!knockable(f.kind) || !traffic::furniture_xz(b.s, sid, f, x, z)) continue;
			const float d = std::hypot(x - b.x, z - b.z);
			if (d > reach) continue;
			knocked[i] = 1;
			j_add_gone(b.s, { uint8_t(gone_kind(f.kind)), x, z });
			if (f.kind == FurnitureKind::LAMP && f.row == 0) {
				const int last = lamp_row(b.s.roads, sid).stations - 1;
				const RoadSegment &g = b.s.roads.segs[size_t(sid)];
				if (f.station == 0) ends.push_back({ sid, g.a, x, z });
				if (f.station == last) ends.push_back({ sid, g.b, x, z });
			}
			DamageEvent e;
			e.kind = DamageEvent::FURNITURE;
			e.x = x; e.z = z; e.y = b.s.terrain.height_at(x, z);
			e.dx = (x - b.x) / std::max(d, MIN_NORMAL_M);
			e.dz = (z - b.z) / std::max(d, MIN_NORMAL_M);
			e.u = uint32_t(gone_kind(f.kind));
			e.a = pole_fate(b, d, reach, uint32_t(f.v * ARC_DRAW_SCALE));
			e.v = f.kind == FurnitureKind::LAMP && f.lit ? 1u : 0u;
			b.emit(e);
			if (f.kind != FurnitureKind::HYDRANT) continue;
			DamageEvent g; // a hydrant broken off: a jet of water
			g.kind = DamageEvent::GEYSER;
			g.x = x; g.z = z; g.y = e.y;
			b.emit(g);
		}
		hang_row_wires(b, sid, spots, knocked);
	});
	hang_joined_wires(b, ends);
}

// The utility poles the player built (sim/street): those within the reach break, and the wires torn off a broken pole pull the
// next one over in its turn, now and then, out to a share past the reach. Each span left with one
// end standing hangs from that end; the poles go through the journal, their spans with them, so undoing the blow stands them again.
void knock_out_poles(Blow &b, float reach) {
	const Tuning &t = b.s.catalog.tune;
	const StreetWorks &w = b.s.street;
	std::vector<int> down;
	w.poles_in(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) {
		const UtilityPole &p = w.poles.items[size_t(id)];
		if (p.alive && std::hypot(p.x - b.x, p.z - b.z) <= reach) down.push_back(id);
	});
	if (down.empty()) return;
	std::sort(down.begin(), down.end());
	std::unordered_set<int> broken(down.begin(), down.end());
	const float pull = reach * t.strike_pole_pull_reach_k;
	for (size_t k = 0; k < down.size(); k++) { // the queue grows as poles are pulled over
		std::vector<int> spans = w.poles.items[size_t(down[k])].spans;
		std::sort(spans.begin(), spans.end());
		for (int span : spans) {
			const UtilitySpan &sp = w.spans.items[size_t(span)];
			const int other = sp.a == down[k] ? sp.b : sp.a;
			const UtilityPole &o = w.poles.items[size_t(other)];
			if (broken.count(other) || std::hypot(o.x - b.x, o.z - b.z) > pull || b.roll(SALT_PULL, uint32_t(span)) >= t.strike_pole_pull_chance) continue;
			broken.insert(other);
			down.push_back(other);
		}
	}
	for (int id : down) {
		const UtilityPole &p = w.poles.items[size_t(id)];
		for (int span : p.spans) {
			const UtilitySpan &sp = w.spans.items[size_t(span)];
			const int other = sp.a == id ? sp.b : sp.a;
			if (broken.count(other)) continue;
			const UtilityPole &o = w.poles.items[size_t(other)];
			const size_t at = size_t(std::find(o.spans.begin(), o.spans.end(), span) - o.spans.begin());
			hang_wire(b, o.x, o.z, seat_arm(pole_stand(w, b.s.roads, b.s.catalog.furniture, other), at), p.x, p.z, o.kind == PoleKind::RURAL);
		}
		const float d = std::hypot(p.x - b.x, p.z - b.z);
		DamageEvent e;
		e.kind = DamageEvent::FURNITURE;
		e.x = p.x; e.z = p.z; e.y = b.s.terrain.height_at(p.x, p.z);
		e.dx = (p.x - b.x) / std::max(d, MIN_NORMAL_M);
		e.dz = (p.z - b.z) / std::max(d, MIN_NORMAL_M);
		e.u = uint32_t(GoneKind::POLE);
		e.a = pole_fate(b, d, reach, POLE_WHO_BASE + uint32_t(id));
		e.v = p.lamp ? 1u : 0u;
		b.emit(e);
	}
	for (int id : down) j_remove_pole(b.s, id);
}

// The trees within the reach: the planted ones thrown each from where it stood, then a clearing there takes them and the woods'
// own (cut_trees_at, through the journal: undoing the blow stands them again), which the render lists and throws.
void fell_trees(Blow &b, float reach) {
	const PlantedTrees &p = b.s.nature.planted;
	p.for_trees(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) {
		const PlantedTree &t = p.trees[size_t(id)];
		const float d = std::hypot(t.x - b.x, t.z - b.z);
		if (d >= reach) return;
		DamageEvent e;
		e.kind = DamageEvent::TREE_FELL;
		e.x = t.x; e.z = t.z; e.y = b.s.terrain.height_at(t.x, t.z);
		e.a = t.height;
		e.dx = (t.x - b.x) / std::max(d, MIN_NORMAL_M);
		e.dz = (t.z - b.z) / std::max(d, MIN_NORMAL_M);
		b.emit(e);
	});
	const int clearing = cut_trees_at(b.s, b.x, b.z, reach);
	if (clearing < 0) return; // the map holds as many clearings as it may: the woods stand
	DamageEvent e;
	e.kind = DamageEvent::TREES_FELLED;
	e.x = b.x; e.z = b.z; e.y = b.ground;
	e.a = reach;
	e.u = uint32_t(clearing);
	b.emit(e);
}
} // namespace

void strike_ground(Blow &b) {
	const BlastParams &p = b.p;
	const float reach = std::min(blast::range_at_pressure(p, b.kg, p.pole_kpa), b.s.catalog.tune.strike_furniture_reach_m);
	knock_out_furniture(b, reach);
	knock_out_poles(b, reach);
	fell_trees(b, reach); // a tree falls to the blow that topples a pole
	if (b.on_building) return; // a burst on a roof digs no crater
	const float radius = blast::crater_radius(p, b.kg, b.soil), depth = blast::crater_depth(p, b.kg, b.soil);
	Crater c{ b.x, b.z, radius, depth, uint8_t(b.soil == blast::Surface::HARD), b.bits(SALT_CRATER, 0) };
	c.y = b.height; // on a deck over the ground (a bridge) it is the deck's: cut away, it leaves nothing on the ground under it
	j_add_crater(b.s, c);
	if (b.soil == blast::Surface::EARTH) {
		BrushDab d;
		d.x = b.x; d.z = b.z; d.radius = radius; d.strength = depth; d.dt = 1.f; d.mode = BrushMode::LOWER;
		j_terrain_touch(b.s, b.s.terrain.brush_rect(d.x, d.z, d.radius));
		b.s.terrain.brush(d);
		mark_ground(b.s, b.x - radius, b.z - radius, b.x + radius, b.z + radius);
	}
	cut_roads(b, radius, depth);
}

} // namespace city
