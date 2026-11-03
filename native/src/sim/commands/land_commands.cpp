// Landscape and Build drawer commands: one dab of the Relief brush (Roadcraft world/brush.ts BrushStroke, a held
// stroke is one undo step) and a building drawn as a lot rectangle (Roadcraft lotEdit.ts lot "rect", the Build
// drawer's Draw tab) with the function chosen in the strip: civic buildings, parks and squares.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "core/random.h"
#include "sim/buildings/growth.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int LOT_SAMPLES = 5; // lot rectangle samples per side tested against the road surfaces
constexpr double FACE_REACH_M = 80; // a drawn lot faces the nearest road this close
constexpr uint32_t LOT_SALT = 0x6c6f7421U;
constexpr double SEED_GRID_M = 4;
constexpr double HALF = 0.5; // the drawn lot's seed comes from its corner rounded to this grid
} // namespace

EditResult terrain_dab(CityState &s, double x, double z, double radius, double strength, double dt, int mode) {
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
	j_terrain_touch(s, s.terrain.brush_rect(d.x, d.z, d.radius));
	if (!s.terrain.brush(d)) {
		r.ok = true;
		return r;
	}
	mark_ground(s, x - radius, z - radius, x + radius, z + radius);
	finish_edit(s, x - radius, z - radius, x + radius, z + radius);
	r.ok = true;
	return r;
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
	const GrowthDef &g = s.catalog.growth;
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
	bool open_space = fn == FN_PARK || fn == FN_SQUARE;
	b.floors = uint16_t(open_space ? 0 : floors_for(g, zone, s.catalog.profile(b.profile), b.seed));
	b.built = s.clock.minutes.load();
	j_add_building(s, b);
	finish_edit(s, b.x - reach, b.z - reach, b.x + reach, b.z + reach);
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
	b.fn = uint8_t(fn);
	b.floors = uint16_t(floors);
	j_edit_building(s, id, b);
	float reach = std::max(b.w, b.d);
	finish_edit(s, b.x - reach, b.z - reach, b.x + reach, b.z + reach);
	r.ok = true;
	return r;
}

} // namespace city
