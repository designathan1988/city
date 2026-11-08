// Landscape commands (the Landscape drawer's Ground, Nature and Sky tabs): the weather, the clouds, painted fog and
// ground, planted trees and clearings, the map's biome, and taking the relief back to the land the map started with.
// Each records how to undo itself (j_nature, j_paint_touch); a held brush is one undo step.
#include "sim/commands/commands.h"
#include "sim/commands/nature_commands.h"
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
constexpr uint32_t DAB_SALT = 0x74726565u; // a stream's salt: the word it names, in ASCII

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

void changed_sky(CityState &s) {
	s.nature.sky_revision++;
	s.touch();
}
void changed_fog(CityState &s) {
	s.nature.fog_revision++;
	changed_sky(s);
}

// Runs `change` on the cloud list and records the list before and after (a map holds a few dozen at most).
template <typename F>
EditResult edit_clouds(CityState &s, F change) {
	std::vector<PlacedCloud> before = s.nature.clouds;
	int32_t next_before = s.nature.next_cloud;
	change(s.nature.clouds);
	std::vector<PlacedCloud> after = s.nature.clouds;
	int32_t next_after = s.nature.next_cloud;
	j_nature(s, (before.size() + after.size()) * sizeof(PlacedCloud), [before, after, next_before, next_after](CityState &st, bool fwd) {
		st.nature.clouds = fwd ? after : before;
		st.nature.next_cloud = fwd ? next_after : next_before;
		changed_sky(st);
	});
	changed_sky(s);
	return done();
}

float tree_distance(const PlantedTree &t, float x, float z) { return std::hypot(t.x - x, t.z - z); }

void add_trees(CityState &s, const std::vector<PlantedTree> &list, std::vector<int> &ids) {
	for (const PlantedTree &t : list) ids.push_back(s.nature.planted.add(t));
}

void mark_trees(CityState &s, float x, float z, float r) { mark_nature(s, x - r, z - r, x + r, z + r); }

// Trees and clearings of a dab: put back or taken away together.
void record_trees(CityState &s, const std::vector<int> &ids, const std::vector<PlantedTree> &added, float x, float z, float r) {
	j_nature(s, added.size() * sizeof(PlantedTree), [ids, added, x, z, r](CityState &st, bool fwd) {
		for (size_t i = 0; i < ids.size(); i++) {
			if (fwd) st.nature.planted.put(ids[i], added[i]);
			else st.nature.planted.remove(ids[i]);
		}
		mark_trees(st, x, z, r);
	});
}
} // namespace

int cut_trees_at(CityState &s, float x, float z, float radius) {
	PlantedTrees &p = s.nature.planted;
	const TreeBrushDef &d = s.catalog.nature.trees;
	if (p.live_clearings() >= d.max_clearings) return -1;
	std::vector<int> gone;
	p.for_trees(x - radius, z - radius, x + radius, z + radius, [&](int id) {
		if (tree_distance(p.trees[size_t(id)], x, z) < radius) gone.push_back(id);
	});
	std::vector<PlantedTree> kept;
	for (int id : gone) {
		kept.push_back(p.trees[size_t(id)]);
		p.remove(id);
	}
	TreeClearing c;
	c.x = x; c.z = z; c.radius = radius;
	int cid = p.add_clearing(c);
	j_nature(s, kept.size() * sizeof(PlantedTree) + sizeof(TreeClearing), [gone, kept, c, cid, x, z, radius](CityState &st, bool fwd) {
		for (size_t i = 0; i < gone.size(); i++) {
			if (fwd) st.nature.planted.remove(gone[i]);
			else st.nature.planted.put(gone[i], kept[i]);
		}
		if (fwd) st.nature.planted.put_clearing(cid, c);
		else st.nature.planted.remove_clearing(cid);
		mark_trees(st, x, z, radius);
	});
	mark_trees(s, x, z, radius);
	return cid;
}

EditResult set_weather(CityState &s, const Weather &next) {
	Weather before = s.nature.weather, after = clamped_weather(next, s.catalog.nature.weather);
	s.nature.weather = after;
	j_nature(s, sizeof(Weather) * 2, [before, after](CityState &st, bool fwd) {
		st.nature.weather = fwd ? after : before;
		changed_sky(st);
	});
	changed_sky(s);
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
	j_nature(s, sizeof(FogDab), [dab](CityState &st, bool fwd) {
		if (fwd) st.nature.fog.push_back(dab);
		else st.nature.fog.pop_back();
		changed_fog(st);
	});
	changed_fog(s);
	return done();
}

EditResult clear_fog(CityState &s) {
	std::vector<FogDab> before;
	before.swap(s.nature.fog);
	size_t bytes = before.size() * sizeof(FogDab);
	j_nature(s, bytes, [before](CityState &st, bool fwd) {
		st.nature.fog = fwd ? std::vector<FogDab>() : before;
		changed_fog(st);
	});
	changed_fog(s);
	return done();
}

EditResult paint_dab(CityState &s, const PaintDab &d) {
	GroundPaint &g = s.nature.ground;
	const PaintDef &def = s.catalog.nature.paint;
	if (d.kind >= int(def.kinds.size()) || d.kind < GULLY_WIPE) return refused("tool.failed");
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
	std::vector<int> keys; // the tiles with a gully cut or wiped: only they change
	for (const auto &kv : g.all())
		if (std::any_of(kv.second->begin(), kv.second->end(), [](const PaintCell &c) { return c.gully != 0 || c.gully_wipe != 0; })) keys.push_back(kv.first);
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
	if (mode == TREE_CUT) return cut_trees_at(s, fx, fz, r) >= 0 ? done() : refused(FULL);
	PlantedTrees &p = s.nature.planted;
	float reach = r + b.spacing;
	std::vector<PlantedTree> nearby;
	p.for_trees(fx - reach, fz - reach, fx + reach, fz + reach, [&](int id) { nearby.push_back(p.trees[size_t(id)]); });
	uint32_t seed = hash_combine(hash_combine(s.seed ^ DAB_SALT, uint32_t(p.trees.size())), uint32_t(int32_t(std::floor(x))) ^ uint32_t(int32_t(std::floor(z))));
	std::vector<PlantedTree> stand;
	if (mode == TREE_ONE) {
		bool room = std::all_of(nearby.begin(), nearby.end(), [&](const PlantedTree &t) { return tree_distance(t, fx, fz) >= b.spacing; });
		if (room) stand.push_back(one_tree(TreeKind(kind), b, fx, fz, seed, d));
	} else {
		stand = plant_stand(TreeKind(kind), b, fx, fz, r, seed, nearby, d);
	}
	if (p.live_trees() + int(stand.size()) > d.max_planted) return refused(FULL);
	std::vector<int> ids;
	add_trees(s, stand, ids);
	record_trees(s, ids, stand, fx, fz, reach);
	mark_trees(s, fx, fz, reach);
	s.touch();
	return done();
}

EditResult clear_trees(CityState &s) {
	PlantedTrees &p = s.nature.planted;
	PlantedTrees before = std::move(p);
	size_t bytes = before.trees.size() * sizeof(PlantedTree) + before.clearings.size() * sizeof(TreeClearing);
	p = PlantedTrees();
	p.clear();
	auto kept = std::make_shared<const PlantedTrees>(std::move(before));
	float h = s.map_half;
	j_nature(s, bytes, [kept, h](CityState &st, bool fwd) {
		if (fwd) {
			st.nature.planted = PlantedTrees();
			st.nature.planted.clear();
		} else {
			st.nature.planted = *kept;
		}
		mark_nature(st, -h, -h, h, h);
	});
	mark_nature(s, -h, -h, h, h);
	s.touch();
	return done();
}

EditResult set_biome(CityState &s, int region) {
	if (region < int(Biome::NONE) || region >= BIOMES) return refused("tool.failed");
	int before = s.nature.nature.region;
	s.nature.nature.region = region;
	float h = s.map_half;
	j_nature(s, sizeof(int) * 2, [before, region, h](CityState &st, bool fwd) {
		st.nature.nature.region = fwd ? region : before;
		mark_nature(st, -h, -h, h, h);
		changed_sky(st);
	});
	mark_nature(s, -h, -h, h, h);
	changed_sky(s);
	return done();
}

EditResult clear_relief(CityState &s) {
	Heightfield &t = s.terrain;
	const Tuning &tn = s.catalog.tune;
	const ScenicCity *scenic = s.catalog.scenic_of(s.map_index); // a scenic map's relief is its own laid-out land
	TerrainGen gen(s.seed, s.map_half, tn, scenic ? &scenic->land : nullptr);
	for (int r = 0; r < int(t.tile_dirty.size()); r++) {
		if (!t.tile_dirty[size_t(r)]) continue;
		int x0 = (r % t.regions) * t.region_size, z0 = (r / t.regions) * t.region_size;
		SampleRect rect{ x0, z0, x0 + t.region_size - 1, z0 + t.region_size - 1 };
		j_terrain_touch(s, rect);
		for (int z = rect.z0; z <= rect.z1; z++)
			for (int x = rect.x0; x <= rect.x1; x++) t.base_q[size_t(z) * t.N + x] = Heightfield::quant(float(gen.base_height(t.x_of(x), t.x_of(z))));
		t.recompose(rect.x0, rect.z0, rect.x1, rect.z1);
		double ax = t.x_of(rect.x0), az = t.x_of(rect.z0), bx = t.x_of(rect.x1), bz = t.x_of(rect.z1);
		mark_ground(s, ax, az, bx, bz);
		settle_network(s); // the roads over this region read its ground before the next one is taken back
	}
	unflood(s);
	s.touch();
	return done();
}

} // namespace city
