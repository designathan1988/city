// What a blow does to the buildings: each feels the blast at its standoff (blast_model.h felt_pressure: the peak, in part
// when the blow is over before the structure answers). What it feels adds to what it already took (DamageState::hurt):
// the share of the collapse limit (Glasstone: a house is demolished at 5 psi), so a small bomb must be repeated to bring
// a building down (the 4.0's "keep hitting"); at 1.0 it falls and leaves a ruin; past the structural limit it catches fire.
// A framed block answers at 20 psi, a house at 5 (Glasstone and Dolan 1977, as tabled by HYDESim and DTIC ADA072246). Close to the
// blow part of it breaks off first: a scar, and when little would be left standing, the rest comes down. Distances are to the walls
// of its blocks (the body's solids), never to the lot round them: a garden or a yard takes no load.
#include "sim/commands/strike_internal.h"

#include "sim/buildings/b40/record.h"
#include "sim/commands/edit_journal.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float LOT_REACH_M = 80.f; // the widest lot: a centre this far outside the reach may still have a wall inside it
constexpr float STANDOFF_DEPTH = 0.5f; // the load on a wall is that at its distance plus half the depth of its block
constexpr uint32_t SALT_FIRE = 0x66697265U; // a stream's salt: the word it names, in ASCII
constexpr uint32_t SALT_SHORT_ALONG = 0x73686f61U, SALT_SHORT_STOREY = 0x73686f73U, SALT_SHORT_KIND = 0x73686f6bU, SALT_SHORT_RISE = 0x73686f6cU,
		SALT_SHORT_TIME = 0x73686f74U; // streams' salts: the words they name, in ASCII
constexpr uint32_t SHORT_DRAWS = 8; // the draws of a building's shorted points: its id times this, plus the point's
constexpr float SPHERE_VOLUME = 4.18879f; // 4/3 of pi

// Where a blow stands from a building: the point of its blocks' walls nearest it, that wall's way along and outward (unit), how far it
// is (0 inside a block) and the depth of the block it reaches, which takes the load. `any` false for a body of no block.
struct WallSpot {
	V2 at, along, out;
	float near = 0, depth = 0;
	bool any = false;
};
WallSpot nearest_wall(const Building &bd, const b40::Body &body, float x, float z) {
	const b40::LotFrame f = b40::lot_frame(bd);
	const V2 p(x, z), local((p - f.o).dot(f.e0), (p - f.o).dot(f.e1)); // the frame's axes are unit
	WallSpot w;
	float best = std::numeric_limits<float>::max(), inside_depth = -1.f;
	for (const b40::Volume &v : body.volumes) {
		const float depth = std::min(v.w, v.d) * STANDOFF_DEPTH;
		if (local.x >= v.x && local.x <= v.x + v.w && local.y >= v.y && local.y <= v.y + v.d) inside_depth = std::max(inside_depth, depth);
		const V2 c[4] = { f.at(v.x, v.y), f.at(v.x + v.w, v.y), f.at(v.x + v.w, v.y + v.d), f.at(v.x, v.y + v.d) };
		const V2 mid = f.at(v.x + v.w * 0.5f, v.y + v.d * 0.5f);
		for (int k = 0; k < 4; k++) {
			const V2 a = c[k], e = c[(k + 1) % 4] - a;
			const float len2 = e.dot(e);
			if (len2 <= 0.f) continue;
			const V2 q = a + e * std::clamp((p - a).dot(e) / len2, 0.f, 1.f);
			const float d = (q - p).dot(q - p);
			if (d >= best) continue;
			best = d;
			const V2 along = e * (1.f / std::sqrt(len2));
			const V2 side(along.y, -along.x);
			w.at = q;
			w.along = along;
			w.out = side.dot(q - mid) >= 0.f ? side : V2(-side.x, -side.y);
			w.depth = depth;
		}
	}
	w.any = best < std::numeric_limits<float>::max();
	w.near = inside_depth >= 0.f ? 0.f : std::sqrt(best);
	if (inside_depth >= 0.f) w.depth = inside_depth;
	return w;
}

// What the building feels: the blast at its walls taken to the middle of the block's depth.
float felt_by(const Blow &b, const WallSpot &wall) {
	return blast::felt_pressure(b.p, blast::airblast(b.p, b.kg, std::hypot(wall.near, wall.depth)));
}

// Where and how hard a blow breaks a building: at full force the one it landed on, else less with the
// distance, at the wall nearest the burst.
struct Aim {
	float x = 0, y = 0, z = 0, force = 0;
	bool breaks = false;
};

Aim aim_at(const Blow &b, const WallSpot &wall) {
	const Tuning &t = b.s.catalog.tune;
	const float radius = t.strike_scar_radius_m + b.strength * t.strike_scar_radius_per_force, near = wall.near;
	const bool landed = b.on_building && near <= 0.f;
	Aim a;
	a.x = landed ? b.x : wall.at.x;
	a.z = landed ? b.z : wall.at.y;
	a.y = landed ? b.height : std::max(b.height, b.ground + t.strike_scar_lift_m);
	a.force = landed ? b.strength : std::max(1.f, b.strength * (1.f - near / radius) * t.strike_scar_force_gain);
	a.breaks = near <= radius && (landed || near < radius * t.strike_scar_near_share || a.force >= t.strike_scar_force_least);
	return a;
}

// The share of a building its scars would leave: a cut across all of it takes everything above, a hole only what it reaches. Its
// height, width and room are its blocks'.
float standing_share(const Blow &b, const Building &bd, const b40::Body &body) {
	float height = 0.f, room = 0.f, lo_x = std::numeric_limits<float>::max(), lo_y = lo_x, hi_x = -lo_x, hi_y = -lo_x;
	for (const b40::Volume &v : body.volumes) {
		const float top = body.level_z(v.top());
		height = std::max(height, top);
		room += v.w * v.d * (top - body.level_z(v.base));
		lo_x = std::min(lo_x, v.x);
		lo_y = std::min(lo_y, v.y);
		hi_x = std::max(hi_x, v.x + v.w);
		hi_y = std::max(hi_y, v.y + v.d);
	}
	if (height <= 0.f || room <= 0.f) return 1.f;
	const float half = std::min(hi_x - lo_x, hi_y - lo_y) * 0.5f, ground = b.s.terrain.height_at(bd.x, bd.z);
	float left = 1.f;
	b.s.damage.each_scar_of(bd.seed, bd.x, bd.z, [&](const Scar &c) {
		const float r = scar_reach_m(c.force);
		const float share = r >= half ? (c.y - r - ground) / height : 1.f - r * r * r * SPHERE_VOLUME / room;
		left = std::min(left, std::max(share, 0.f));
	});
	return left;
}

int scars_on(const CityState &s, const Building &bd) {
	return s.damage.scar_count_of(bd.seed, bd.x, bd.z);
}

void bring_down(Blow &b, int id, const Aim &aim) {
	const Building rec = b.s.buildings.items[size_t(id)];
	Ruin ruin;
	ruin.x = rec.x; ruin.z = rec.z; ruin.w = rec.w; ruin.d = rec.d; ruin.rot = rec.rot; ruin.seed = rec.seed;
	ruin.record = rec;
	ruin.blow_x = aim.x;
	ruin.blow_y = aim.y;
	ruin.blow_z = aim.z;
	ruin.blow_force = aim.force;
	j_add_ruin(b.s, ruin);
	const uint32_t ruin_index = uint32_t(b.s.damage.ruins.size() - 1); // the event names the ruin it brought (damage_event.h)
	j_remove_building(b.s, id);
	j_set_hurt(b.s, rec.seed, rec.x, rec.z, 0.f);
	auto &fires = b.s.damage.fires;
	fires.erase(std::remove_if(fires.begin(), fires.end(), [&](const Fire &f) { return f.building == id; }), fires.end());
	b.result.buildings_removed++;
	DamageEvent e;
	e.kind = DamageEvent::BUILDING_DOWN;
	e.id = id;
	e.x = rec.x; e.z = rec.z; e.y = b.ground;
	e.a = b.strength;
	e.u = ruin_index;
	b.emit(e);
}

void ignite(Blow &b, int id, const Building &rec) {
	auto &fires = b.s.damage.fires;
	if (int(fires.size()) >= b.p.fire_max) return;
	for (const Fire &f : fires)
		if (f.building == id) return;
	fires.push_back({ id, rec.x, rec.z, b.s.damage.now + double(b.p.fire_min_s + b.p.fire_span_s * b.roll(SALT_FIRE, uint32_t(id))) });
	DamageEvent e;
	e.kind = DamageEvent::IGNITE;
	e.id = id;
	e.x = rec.x; e.z = rec.z;
	b.emit(e);
}

// Its wiring shorting and its pipes bursting: the first blow a building takes and stands leaves a few
// points of the wall nearest the burst, at storeys drawn at random, sparking or spurting water a while (SPARK, LEAK).
void short_out(Blow &b, int id, const Building &bd, const b40::Body &body, const Aim &aim) {
	const Tuning &t = b.s.catalog.tune;
	const WallSpot wall = nearest_wall(bd, body, aim.x, aim.z);
	if (!wall.any) return;
	const float ground = b.s.terrain.height_at(wall.at.x, wall.at.y);
	const int spots = std::min(int(t.strike_short_spots_most), 1 + int(std::lround(aim.force / t.strike_short_force_step)));
	for (int k = 0; k < spots; k++) {
		const uint32_t who = uint32_t(id) * SHORT_DRAWS + uint32_t(k);
		const float along = (b.roll(SALT_SHORT_ALONG, who) - 0.5f) * t.strike_short_spread_m;
		const int storey = std::min(bd.floors - 1, int(b.roll(SALT_SHORT_STOREY, who) * float(bd.floors)));
		const bool spark = b.roll(SALT_SHORT_KIND, who) < t.strike_short_spark_share;
		DamageEvent e;
		e.kind = spark ? DamageEvent::SPARK : DamageEvent::LEAK;
		e.x = wall.at.x + wall.along.x * along + wall.out.x * t.strike_short_out_m;
		e.z = wall.at.y + wall.along.y * along + wall.out.y * t.strike_short_out_m;
		e.y = ground + body.level_z(storey) + t.strike_short_lift_m + b.roll(SALT_SHORT_RISE, who) * t.strike_short_lift_span_m;
		const float last = spark ? t.strike_short_spark_s : t.strike_short_leak_s, more = spark ? t.strike_short_spark_span_s : t.strike_short_leak_span_s;
		e.a = last + b.roll(SALT_SHORT_TIME, who) * more;
		b.emit(e);
	}
}

// Soot and wear: the building looks older by as much as the blow aged it.
void wear(Blow &b, int id, float pressure) {
	const BlastParams &p = b.p;
	const float share = std::min(1.f, (pressure - p.light_kpa) / (p.structural_kpa - p.light_kpa));
	Building after = b.s.buildings.items[size_t(id)];
	after.built -= double(p.wear_gain * share) * double(b.s.catalog.growth.decay_span_min);
	j_edit_building(b.s, id, after);
}
} // namespace

void strike_buildings(Blow &b) {
	const BlastParams &p = b.p;
	const float reach = blast::range_at_pressure(p, b.kg, p.light_kpa) + LOT_REACH_M;
	std::vector<int> ids;
	b.s.buildings.query(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	for (int id : ids) {
		const Building bd = b.s.buildings.items[size_t(id)];
		if (!bd.alive || bd.floors == 0) continue; // an empty lot has nothing to break
		const std::shared_ptr<const b40::Body> body = b40::record_mass(b.s.buildings.designs, bd);
		const WallSpot wall = nearest_wall(bd, *body, b.x, b.z);
		if (!wall.any) continue;
		const Tuning &t = b.s.catalog.tune;
		const float felt = felt_by(b, wall);
		const Aim aim = aim_at(b, wall);
		const bool scarred = aim.breaks && float(scars_on(b.s, bd)) < t.strike_scars_most;
		const bool felt_all = felt >= p.light_kpa; // a big building may feel little of a blow that still breaks the wall it lands on
		if (!felt_all && !scarred) continue;
		const int at = b.s.damage.hurt_of(bd.seed, bd.x, bd.z);
		const float limit = bd.floors >= t.strike_frame_floors ? t.strike_frame_collapse_kpa : p.collapse_kpa;
		const float taken = (at >= 0 ? b.s.damage.hurt[size_t(at)].amount : 0.f) + (felt_all ? felt / limit : 0.f);
		if (scarred) j_add_scar(b.s, Scar{ bd.seed, bd.x, bd.z, aim.x, aim.y, aim.z, aim.force });
		if (taken >= 1.f || standing_share(b, bd, *body) < t.strike_standing_least) {
			bring_down(b, id, aim);
			continue;
		}
		if (scarred) {
			DamageEvent cut;
			cut.kind = DamageEvent::BUILDING_SCAR;
			cut.id = id;
			b.emit(cut);
		}
		if (!felt_all) continue;
		j_set_hurt(b.s, bd.seed, bd.x, bd.z, taken);
		DamageEvent e;
		e.kind = DamageEvent::BUILDING_HIT;
		e.id = id;
		e.x = std::clamp(b.x, bd.x - bd.w, bd.x + bd.w); e.z = std::clamp(b.z, bd.z - bd.d, bd.z + bd.d); e.y = b.ground;
		e.a = felt;
		e.b = taken;
		b.emit(e);
		if (at < 0) short_out(b, id, bd, *body, aim); // its first blow: once shorted, a building is not shorted again
		wear(b, id, felt);
		if (felt >= p.structural_kpa) ignite(b, id, b.s.buildings.items[size_t(id)]);
	}
}

} // namespace city
