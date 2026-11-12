// Landscape commands (the Landscape drawer's Ground, Nature and Sky tabs): the weather, the clouds, painted fog and
// ground, planted trees and clearings, the map's biome, and taking the relief back to the land the map started with.
// Each records how to undo itself (j_nature, j_paint_touch); a held brush is one undo step.
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"

#include "core/random.h"
#include "sim/state/city_state.h"
#include "sim/terrain/terrain_gen.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr const char *FULL = "hint.tileFull";
constexpr uint32_t DAB_SALT = 0x74726565u;

EditResult done() {
	EditResult r;
	r.ok = true;
	return r;
}
EditResult refused(const char *key) {
	EditResult r;
	r.error = key;
	return r;
}

// Runs `change` on the cloud list and records the list before and after (a map holds a few dozen at most).
template <typename F>
EditResult edit_clouds(CityState &s, F change) {
	std::vector<PlacedCloud> before = s.nature.clouds;
	int32_t next_before = s.nature.next_cloud;
	change(s.nature.clouds);
	std::vector<PlacedCloud> after = s.nature.clouds;
	int32_t next_after = s.nature.next_cloud;
	if (before == after && next_before == next_after) return done();
	j_nature(s, CloudsEdit{ std::move(before), std::move(after), next_before, next_after });
	nature_sky_changed(s);
	return done();
}

float dab_chunk(const PlantedTree &t, float x, float z) { return std::hypot(t.x - x, t.z - z); }

void add_trees(CityState &s, const std::vector<PlantedTree> &list, std::vector<int> &ids) {
	for (const PlantedTree &t : list) ids.push_back(s.nature.planted.add(t));
}

// Trees and clearings of a dab: put back or taken away together.
void record_trees(CityState &s, std::vector<int> ids, std::vector<PlantedTree> added, float x, float z, float r) {
	j_nature(s, TreesEdit{ std::move(ids), std::move(added), true, {}, x, z, r });
}
} // namespace

bool cut_trees_at(CityState &s, float x, float z, float radius) {
	PlantedTrees &p = s.nature.planted;
	const TreeBrushDef &d = s.catalog.nature.trees;
	if (p.live_clearings() >= d.max_clearings) return false;
	std::vector<int> gone;
	p.for_trees(x - radius, z - radius, x + radius, z + radius, [&](int id) {
		if (dab_chunk(p.trees[size_t(id)], x, z) < radius) gone.push_back(id);
	});
	std::vector<PlantedTree> kept;
	for (int id : gone) {
		kept.push_back(p.trees[size_t(id)]);
		p.remove(id);
	}
	TreeClearing c;
	c.x = x; c.z = z; c.radius = radius;
	int cid = p.add_clearing(c);
	j_nature(s, TreesEdit{ std::move(gone), std::move(kept), false, ClearingEdit{ cid, c }, x, z, radius });
	nature_trees_changed(s, x, z, radius);
	s.touch(); // a clearing changes procedural woods even when no authored tree was present
	return true;
}

EditResult set_weather(CityState &s, const Weather &next) {
	Weather before = s.nature.weather, after = clamped_weather(next, s.catalog.nature.weather);
	if (before == after) return done();
	s.nature.weather = after;
	j_nature(s, WeatherEdit{ before, after });
	nature_sky_changed(s);
	return done();
}

EditResult add_cloud(CityState &s, const PlacedCloud &c) {
	const CloudDef &d = s.catalog.nature.clouds;
	if (int(s.nature.clouds.size()) >= d.max_clouds) return refused("hint.cloud.full");
	return edit_clouds(s, [&](std::vector<PlacedCloud> &list) {
		PlacedCloud n = clamped_cloud(c, d);
		n.id = s.nature.next_cloud++;
		list.push_back(n);
	});
}

EditResult edit_cloud(CityState &s, int id, const PlacedCloud &c) {
	const CloudDef &d = s.catalog.nature.clouds;
	return edit_clouds(s, [&](std::vector<PlacedCloud> &list) {
		for (PlacedCloud &o : list)
			if (o.id == id) {
				o = clamped_cloud(c, d);
				o.id = id;
			}
	});
}

EditResult remove_cloud(CityState &s, int id) {
	return edit_clouds(s, [&](std::vector<PlacedCloud> &list) {
		list.erase(std::remove_if(list.begin(), list.end(), [&](const PlacedCloud &c) { return c.id == id; }), list.end());
	});
}

EditResult scatter_sky(CityState &s, int count, const PlacedCloud &like, float variation) {
	const CloudDef &d = s.catalog.nature.clouds;
	int room = d.max_clouds - int(s.nature.clouds.size());
	std::vector<PlacedCloud> laid = scatter_clouds(std::min(count, room), like, variation, s.nature.clouds,
			hash_combine(s.seed, uint32_t(s.nature.next_cloud)), d);
	EditResult r = edit_clouds(s, [&](std::vector<PlacedCloud> &list) {
		for (PlacedCloud c : laid) {
			c.id = s.nature.next_cloud++;
			list.push_back(c);
		}
	});
	if (int(laid.size()) < count) r.error = "hint.cloud.full";
	return r;
}

EditResult clear_clouds(CityState &s) {
	return edit_clouds(s, [](std::vector<PlacedCloud> &list) { list.clear(); });
}

EditResult fog_dab(CityState &s, const FogDab &in) {
	const FogDef &d = s.catalog.nature.fog;
	if (int(s.nature.fog.size()) >= d.max_dabs) return refused(FULL);
	FogDab dab = in;
	dab.height = d.height_m.clamp(dab.height);
	dab.speed = d.speed_ms.clamp(dab.speed);
	s.nature.fog.push_back(dab);
	j_nature(s, FogAdded{ dab });
	nature_fog_changed(s);
	return done();
}

EditResult clear_fog(CityState &s) {
	std::vector<FogDab> before = s.nature.fog.take();
	if (before.empty()) return done();
	j_nature(s, FogCleared{ std::move(before) });
	nature_fog_changed(s);
	return done();
}

EditResult paint_dab(CityState &s, const PaintDab &d) {
	GroundPaint &g = s.nature.ground;
	const PaintDef &def = s.catalog.nature.paint;
	if (d.kind >= int(def.kinds.size()) || d.kind < GULLY_WIPE) return refused("tool.failed");
	if (d.radius <= 0 || d.strength <= 0) return done();
	std::vector<int> tiles;
	g.tiles_under(d.x, d.z, d.radius, tiles);
	int fresh = 0;
	for (int key : tiles) fresh += g.tile(key) ? 0 : 1;
	if (int(g.all().size()) + fresh > def.max_tiles) return refused(FULL);
	j_paint_touch(s, tiles);
	int rect[4];
	if (g.dab(d, rect)) {
		mark_paint(s, rect);
		s.touch();
	}
	return done();
}

EditResult clear_gullies(CityState &s) {
	GroundPaint &g = s.nature.ground;
	std::vector<int> keys;
	for (const auto &kv : g.all())
		if (std::any_of(kv.second->begin(), kv.second->end(), [](const PaintCell &c) { return c.gully != 0 || c.gully_wipe != 0; })) keys.push_back(kv.first);
	if (keys.empty()) return done();
	j_paint_touch(s, keys);
	for (int key : keys) {
		PaintTile t = *g.tile(key);
		for (PaintCell &c : t) c.gully = c.gully_wipe = 0;
		g.put_tile(key, &t);
		int rect[4];
		g.tile_rect(key, rect);
		mark_paint(s, rect);
	}
	s.touch();
	return done();
}

EditResult tree_dab(CityState &s, double x, double z, double radius, int mode, int kind, const TreeBrush &b) {
	const TreeBrushDef &d = s.catalog.nature.trees;
	float fx = float(x), fz = float(z), r = float(radius);
	if (!s.terrain.in_bounds(fx, fz) || kind < 0 || kind >= int(TreeKind::COUNT)) return refused("tool.failed");
	if (mode == TREE_CUT) return cut_trees_at(s, fx, fz, r) ? done() : refused(FULL);
	PlantedTrees &p = s.nature.planted;
	float reach = r + b.spacing;
	std::vector<PlantedTree> nearby;
	p.for_trees(fx - reach, fz - reach, fx + reach, fz + reach, [&](int id) { nearby.push_back(p.trees[size_t(id)]); });
	uint32_t seed = hash_combine(hash_combine(s.seed ^ DAB_SALT, uint32_t(p.trees.size())), uint32_t(int32_t(std::floor(x))) ^ uint32_t(int32_t(std::floor(z))));
	std::vector<PlantedTree> stand;
	if (mode == TREE_ONE) {
		bool room = std::all_of(nearby.begin(), nearby.end(), [&](const PlantedTree &t) { return dab_chunk(t, fx, fz) >= b.spacing; });
		if (room) stand.push_back(one_tree(TreeKind(kind), b, fx, fz, seed, d));
	} else {
		stand = plant_stand(TreeKind(kind), b, fx, fz, r, seed, nearby, d);
	}
	if (p.live_trees() + int(stand.size()) > d.max_planted) return refused(FULL);
	if (stand.empty()) return done();
	std::vector<int> ids;
	add_trees(s, stand, ids);
	record_trees(s, std::move(ids), std::move(stand), fx, fz, reach);
	nature_trees_changed(s, fx, fz, reach);
	s.touch();
	return done();
}

EditResult clear_trees(CityState &s) {
	PlantedTrees &p = s.nature.planted;
	if (p.trees.empty() && p.clearings.empty()) return done();
	PlantedTrees before = std::move(p);
	p = PlantedTrees();
	p.clear();
	auto kept = std::make_shared<const PlantedTrees>(std::move(before));
	float h = s.map_half;
	j_nature(s, TreesCleared{ std::move(kept), h });
	mark_nature(s, -h, -h, h, h);
	s.touch();
	return done();
}

EditResult set_biome(CityState &s, int region) {
	if (region < int(Biome::NONE) || region >= BIOMES) return refused("tool.failed");
	int before = s.nature.nature.region;
	if (before == region) return done();
	s.nature.nature.region = region;
	float h = s.map_half;
	j_nature(s, BiomeEdit{ before, region, h });
	mark_nature(s, -h, -h, h, h);
	nature_sky_changed(s);
	return done();
}

EditResult clear_relief(CityState &s) {
	Heightfield &t = s.terrain;
	const Tuning &tn = s.catalog.tune;
	TerrainGen gen(s.seed, s.map_half, tn.world_water_spacing_m, s.natural_river);
	for (int r = 0; r < int(t.tile_dirty.size()); r++) {
		if (!t.tile_dirty[size_t(r)]) continue;
		int x0 = (r % t.regions) * t.region_size, z0 = (r / t.regions) * t.region_size;
		SampleRect rect{ x0, z0, x0 + t.region_size - 1, z0 + t.region_size - 1 };
		j_terrain_touch(s, rect);
		for (int z = rect.z0; z <= rect.z1; z++)
			for (int x = rect.x0; x <= rect.x1; x++) t.base_q[size_t(z) * t.N + x] = Heightfield::quant(float(gen.base_height(t.x_of(x), t.x_of(z))));
		t.base_changed(rect);
		t.recompose(rect.x0, rect.z0, rect.x1, rect.z1);
		double ax = t.x_of(rect.x0), az = t.x_of(rect.z0), bx = t.x_of(rect.x1), bz = t.x_of(rect.z1);
		mark_ground(s, ax, az, bx, bz);
		finish_edit(s, ax, az, bx, bz);
	}
	unflood(s);
	s.touch();
	return done();
}

} // namespace city
