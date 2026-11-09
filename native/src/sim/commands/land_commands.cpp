// Landscape and Build drawer commands: one dab of the Relief brush (Roadcraft world/brush.ts BrushStroke, a held
// stroke is one undo step) and a building drawn as a lot rectangle (Roadcraft lotEdit.ts lot "rect", the Build
// drawer's Draw tab) with the function chosen in the strip: civic buildings, parks and squares.
#include "sim/commands/commands.h"
#include "sim/commands/nature_commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"

#include "core/random.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/growth.h"
#include "sim/commands/body_ops.h"
#include "sim/state/city_state.h"
#include "sim/terrain/terrain_gen.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int LOT_SAMPLES = 5; // lot rectangle samples per side tested against the road surfaces
constexpr double FACE_REACH_M = 80; // a drawn lot faces the nearest road this close
constexpr uint32_t LOT_SALT = 0x6c6f7421U; // a stream's salt: the word it names, in ASCII
constexpr double SEED_GRID_M = 4;
constexpr double HALF = 0.5; // the drawn lot's seed comes from its corner rounded to this grid
} // namespace

// The rock a landform stands on, under the whole dab and a little past it, so it reaches the foot of its cliff.
void landform_rock(CityState &s, double x, double z, double radius, BrushMode mode) {
	const PaintDef &def = s.catalog.nature.paint;
	size_t i = size_t(int(mode) - int(BrushMode::MESA));
	if (mode < BrushMode::MESA || mode > BrushMode::SUGARLOAF || i >= def.landform_rock.size()) return;
	PaintDab rock;
	rock.x = float(x); rock.z = float(z); rock.radius = float(radius) * def.rock_reach; rock.strength = 1.f; rock.kind = def.landform_rock[i];
	rock.hardness = 1.f - def.rock_rim; // GroundPaint::dab's rim is 1 - hardness: the rock whole to its cliff's foot
	paint_dab(s, rock);
}

// The water of a river dab, and what undoes it.
void lay_river(CityState &s, const BrushDab &d) {
	std::vector<uint32_t> index;
	std::vector<float> before;
	float level = s.terrain.river_level(d.h0);
	s.terrain.flood(d.x, d.z, d.radius, level, index, before);
	cut_trees_at(s, d.x, d.z, d.radius);
	if (index.empty()) return;
	double x0 = d.x - d.radius, z0 = d.z - d.radius, x1 = d.x + d.radius, z1 = d.z + d.radius;
	auto mark = [x0, z0, x1, z1](CityState &st) { for_chunks(x0, z0, x1, z1, [&](int64_t k) { st.log.water_chunks.push_back(k); }); };
	mark(s);
	j_nature(s, index.size() * (sizeof(uint32_t) + sizeof(float)), [index, before, level, mark](CityState &st, bool fwd) {
		for (size_t i = 0; i < index.size(); i++) st.terrain.water[index[i]] = fwd ? level : before[i];
		mark(st);
	});
}

EditResult terrain_dab(CityState &s, double x, double z, double radius, double strength, double dt, int mode, double hardness) {
	EditResult r;
	if (mode < 0 || mode >= int(BrushMode::COUNT) || radius <= 0 || !s.terrain.in_bounds(float(x), float(z))) {
		r.error = "tool.failed";
		return r;
	}
	EditHistory &h = s.history;
	if (!h.stroke_has_h0) {
		h.stroke_h0 = s.terrain.base_height_at(float(x), float(z));
		h.stroke_has_h0 = true;
	}
	BrushDab d;
	d.x = float(x); d.z = float(z); d.radius = float(radius); d.strength = float(strength); d.dt = float(dt);
	d.h0 = h.stroke_h0;
	d.mode = BrushMode(mode);
	d.hardness = float(hardness);
	j_terrain_touch(s, s.terrain.brush_rect(d.x, d.z, d.radius));
	if (!s.terrain.brush(d)) {
		r.ok = true;
		return r;
	}
	landform_rock(s, x, z, radius, d.mode);
	if (d.mode == BrushMode::RIVER) lay_river(s, d);
	mark_ground(s, x - radius, z - radius, x + radius, z + radius);
	r.ok = true;
	return r;
}

void unflood(CityState &s) {
	Heightfield &t = s.terrain;
	if (t.water.empty()) return;
	std::vector<uint32_t> changed, index;
	std::vector<float> before, after;
	for (const auto &kv : t.water_original) changed.push_back(kv.first); // only what changed can differ from the land
	std::sort(changed.begin(), changed.end());
	for (uint32_t i : changed) {
		const float was = t.water_original.at(i);
		if (t.water[i] == TerrainGen::NO_WATER || was == t.water[i]) continue;
		index.push_back(i);
		before.push_back(t.water[i]);
		after.push_back(was);
		t.water[i] = was;
	}
	if (index.empty()) return;
	float h = s.map_half;
	auto mark = [h](CityState &st) { for_chunks(-h, -h, h, h, [&](int64_t k) { st.log.water_chunks.push_back(k); }); };
	mark(s);
	j_nature(s, index.size() * (sizeof(uint32_t) + 2 * sizeof(float)), [index, before, after, mark](CityState &st, bool fwd) {
		for (size_t i = 0; i < index.size(); i++) st.terrain.water[index[i]] = fwd ? after[i] : before[i];
		mark(st);
	});
}

std::string lot_frame(const CityState &s, double ax, double az, double bx, double bz, Building &b) {
	double mx = (ax + bx) / 2, mz = (az + bz) / 2, t, dist;
	int street = s.roads.find_segment_near(mx, mz, FACE_REACH_M, t, dist);
	double rot = 0;
	if (street >= 0) {
		double px, pz;
		s.roads.segs[street].q.at(t, px, pz);
		rot = std::atan2(px - mx, pz - mz); // local +z faces the street
	}
	double c = std::cos(rot), sn = std::sin(rot);
	double du = (bx - ax) * c - (bz - az) * sn, dv = (bx - ax) * sn + (bz - az) * c;
	b.x = float(mx); b.z = float(mz); b.rot = float(rot);
	b.w = float(std::fabs(du)); b.d = float(std::fabs(dv));
	b.profile = uint16_t(street >= 0 ? s.roads.segs[street].profile : 0);
	if (std::min(b.w, b.d) < s.catalog.tune.road_min_len_m) return "refuse.lot.small";
	for (int i = 0; i < LOT_SAMPLES; i++)
		for (int k = 0; k < LOT_SAMPLES; k++) {
			double lu = (double(i) / (LOT_SAMPLES - 1) - HALF) * b.w, lv = (double(k) / (LOT_SAMPLES - 1) - HALF) * b.d;
			double px = mx + lu * c + lv * sn, pz = mz - lu * sn + lv * c;
			bool on_road = false;
			s.roads.for_segments_near(px, pz, px, pz, [&](int sg) {
				double tt, dd;
				closest_t(s.roads.segs[sg].q, px, pz, tt, dd);
				on_road = on_road || dd < s.roads.profile_of(sg).half();
			});
			if (on_road) return "refuse.lot.road";
		}
	return "";
}

EditResult place_building(CityState &s, double ax, double az, double bx, double bz, int fn, int zone) {
	EditResult r;
	if (fn < 0 || fn >= FN_COUNT || zone < 1 || zone >= ZONE_CODES) {
		r.error = "tool.failed";
		return r;
	}
	Building b;
	r.error = lot_frame(s, ax, az, bx, bz, b);
	if (!r.error.empty()) return r;
	std::vector<int> doomed;
	float reach = std::max(b.w, b.d);
	s.buildings.query(b.x - reach, b.z - reach, b.x + reach, b.z + reach, [&](int id) {
		const Building &o = s.buildings.items[id];
		if (BuildingStore::covers(b, o.x, o.z, 0.f) || BuildingStore::covers(o, b.x, b.z, 0.f)) doomed.push_back(id);
	});
	for (int id : doomed) j_remove_building(s, id);
	b.zone = uint8_t(zone);
	b.fn = uint8_t(fn);
	b.seed = hash_combine(uint32_t(int32_t(std::lround(ax / SEED_GRID_M))), uint32_t(int32_t(std::lround(az / SEED_GRID_M))) ^ LOT_SALT);
	b.style = uint16_t(b.seed & BUILDING_AUTO_STYLE_MASK);
	b.floors = uint16_t(body_floors(s.buildings.designs, b));
	b.built = s.clock.minutes.load();
	r.placed = j_add_building(s, b);
	r.ok = true;
	r.buildings_removed = int(doomed.size());
	return r;
}

EditResult edit_building(CityState &s, int id, int fn, int floors) {
	EditResult r;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive || fn < 0 || fn >= FN_COUNT || floors < 0 ||
			floors > s.catalog.tune.edit_max_floors) {
		r.error = "tool.failed";
		return r;
	}
	Building b = s.buildings.items[id];
	if (b.fn == fn && b.floors == floors) {
		r.ok = true;
		return r;
	}
	const bool other_use = b.fn != fn;
	b.fn = uint8_t(fn);
	if (other_use) {
		b.body = 0; // another use is another building, made again from the seed; undo brings the edited one back
		b.floors = 0;
		b.floors = uint16_t(body_floors(s.buildings.designs, b));
	}
	if (floors != b.floors && floors > 0) {
		// Another height is the body's: its tallest block grows or shrinks by the difference.
		b40::Body body = *b40::record_body(s.buildings.designs, b); // a copy: the new height becomes a version of its own
		int tallest = -1;
		for (size_t i = 0; i < body.volumes.size(); i++)
			if (tallest < 0 || body.volumes[i].top() > body.volumes[size_t(tallest)].top()) tallest = int(i);
		if (tallest < 0) {
			r.error = "tool.failed";
			return r;
		}
		const b40::Volume &v = body.volumes[size_t(tallest)];
		r.error = op_set_storeys(body, s.catalog.tune, tallest, std::max(1, floors - v.base));
		if (!r.error.empty()) return r;
		b.body = s.buildings.designs.edited->add(std::move(body));
		b.floors = uint16_t(body_floors(s.buildings.designs, b));
	}
	j_edit_building(s, id, b);
	r.ok = true;
	return r;
}

} // namespace city
