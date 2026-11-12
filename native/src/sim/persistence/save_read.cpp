// Reading a city (save_format.h): the land is regenerated from the saved seed, then the sculpted tiles, modifiers,
// roads, buildings and growth go back under their ids; lots and node shapes are derived again.
#include "sim/persistence/save_format.h"
#include "sim/roads/road_variants.h"
#include "sim/persistence/body_codec.h"

#include "core/bytes.h"
#include "core/laps.h"
#include "sim/persistence/save_blocks.h"
#include "sim/persistence/save_demolition.h"
#include "sim/citygen/city_gen.h"
#include "sim/commands/road_grade_sync.h"
#include "sim/state/city_state.h"

#include <cmath>
#include <array>
#include <limits>
#include <memory>

namespace city {

namespace {

Quad get_quad(ByteReader &r) {
	Quad q;
	q.x0 = r.get<double>(); q.z0 = r.get<double>();
	q.cx = r.get<double>(); q.cz = r.get<double>();
	q.x1 = r.get<double>(); q.z1 = r.get<double>();
	return q;
}

bool read_terrain(ByteReader &r, Heightfield &t) {
	int n = r.get<int32_t>(), regions = r.get<int32_t>(), rs = r.get<int32_t>();
	if (!r.ok || n != t.N || regions != t.regions || rs != t.region_size) return false;
	std::vector<uint8_t> dirty;
	r.get_vec(dirty, t.tile_dirty.size());
	if (!r.ok || dirty.size() != t.tile_dirty.size()) return false;
	for (int k = 0; k < int(dirty.size()) && r.ok; k++) {
		if (!dirty[k]) continue;
		t.tile_dirty[k] = 1;
		int x0 = (k % regions) * rs, z0 = (k / regions) * rs;
		for (int z = z0; z < z0 + rs; z++) r.get_bytes(&t.base_q[size_t(z) * n + x0], size_t(rs) * sizeof(uint16_t));
	}
	t.base_changed({ 0, 0, n - 1, n - 1 });
	t.recompose(0, 0, n - 1, n - 1);
	uint32_t mods = r.get<uint32_t>();
	if (!r.require_items(mods, sizeof(uint8_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < mods && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		TerrainModifier m;
		m.flat = r.get<float>(); m.max_run = r.get<float>();
		m.minx = r.get<float>(); m.minz = r.get<float>(); m.maxx = r.get<float>(); m.maxz = r.get<float>();
		r.get_vec(m.pts);
		if (r.ok) t.put_modifier(int(i), m);
	}
	return r.ok;
}

bool read_roads(ByteReader &r, RoadNetwork &net, uint32_t version) {
	uint32_t nodes = r.get<uint32_t>();
	if (!r.require_items(nodes, sizeof(uint8_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < nodes && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		RoadNode n;
		n.x = r.get<double>(); n.z = r.get<double>();
		if (version >= GRADE_SAVE_VERSION) n.lift = r.get<float>();
		else n.y = r.get<double>(); // an older file kept the height: solved again now, with no lift
		n.ctl = r.get<uint8_t>();
		if (version >= SIGNAL_SAVE_VERSION) {
			n.signal.fixed = r.get<uint8_t>() != 0;
			n.signal.offset = r.get<float>();
			r.get_vec(n.signal.greens);
		}
		n.plain = r.get<uint8_t>() != 0;
		if (r.ok) net.put_node(int(i), n);
	}
	uint32_t segs = r.get<uint32_t>();
	if (!r.require_items(segs, sizeof(uint8_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < segs && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		RoadSegment g;
		g.a = r.get<int32_t>(); g.b = r.get<int32_t>();
		g.q = get_quad(r);
		g.profile = r.get<int32_t>();
		g.zl = r.get<uint8_t>(); g.zr = r.get<uint8_t>();
		g.flags = r.get<uint16_t>();
		bool ends_ok = g.a >= 0 && g.b >= 0 && g.a < int(net.nodes.size()) && g.b < int(net.nodes.size()) &&
				net.nodes[g.a].alive && net.nodes[g.b].alive && size_t(g.profile) < net.cat->profiles.size();
		if (!ends_ok) return false;
		if (r.ok) net.put_segment(int(i), g);
	}
	for (int n = 0; n < int(net.nodes.size()); n++) net.refresh_node(n);
	return r.ok;
}

bool read_buildings(ByteReader &r, BuildingStore &bs) {
	uint32_t count = r.get<uint32_t>();
	if (!r.require_items(count, sizeof(uint8_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		Building b;
		if (get_building(r, b)) bs.put(int(i), b, false);
	}
	return r.ok;
}

bool read_transit(ByteReader &r, TransitData &t) {
	t = TransitData();
	const int next = r.get<int32_t>();
	std::vector<TransitStop> saved_stops;
	std::vector<RailTrack> saved_tracks;
	std::vector<TransitLine> saved_lines;
	const uint32_t stops = r.get<uint32_t>();
	constexpr size_t STOP_BYTES = 3 * sizeof(int32_t) + 4 * sizeof(float) + 2 * sizeof(uint8_t);
	if (!r.require_items(stops, STOP_BYTES, MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < stops && r.ok; i++) {
		TransitStop q;
		q.id = r.get<int32_t>();
		q.mode = TransitMode(r.get<uint8_t>());
		q.at.x = r.get<float>();
		q.at.y = r.get<float>();
		q.seg = r.get<int32_t>();
		q.track = r.get<int32_t>();
		const uint8_t flags = r.get<uint8_t>();
		q.terminal = (flags & 1) != 0;
		q.has_entrance = (flags & 2) != 0;
		q.entrance.x = r.get<float>();
		q.entrance.y = r.get<float>();
		saved_stops.push_back(q);
	}
	const uint32_t tracks = r.get<uint32_t>();
	if (!r.require_items(tracks, sizeof(int32_t) + sizeof(uint8_t) + sizeof(uint32_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < tracks && r.ok; i++) {
		RailTrack k;
		k.id = r.get<int32_t>();
		k.mode = TransitMode(r.get<uint8_t>());
		r.get_vec(k.points);
		saved_tracks.push_back(std::move(k));
	}
	const uint32_t lines = r.get<uint32_t>();
	constexpr size_t LINE_HEADER_BYTES = 2 * sizeof(int32_t) + sizeof(uint8_t) + 3 * sizeof(uint32_t);
	if (!r.require_items(lines, LINE_HEADER_BYTES, MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < lines && r.ok; i++) {
		TransitLine l;
		l.id = r.get<int32_t>();
		l.mode = TransitMode(r.get<uint8_t>());
		r.get_vec(l.stops);
		l.colour = r.get<uint32_t>();
		l.vehicles = r.get<int32_t>();
		r.get_string(l.name);
		saved_lines.push_back(std::move(l));
	}
	return r.ok && t.restore(std::move(saved_stops), std::move(saved_tracks), std::move(saved_lines), next);
}
} // namespace

namespace {
constexpr uint32_t NATURE_VERSION = 2, VARIANTS_VERSION = 3;

// Made again in the saved order, so each keeps the index the roads hold.
void read_spec(ByteReader &r, RoadSpec &spec) {
	const uint32_t n = r.get<uint32_t>();
	constexpr size_t SPEC_ELEMENT_BYTES = 3 * sizeof(uint8_t) + sizeof(float) + sizeof(int32_t);
	if (!r.require_items(n, SPEC_ELEMENT_BYTES, MAX_RECORDS)) return;
	for (uint32_t i = 0; i < n && r.ok; i++) {
		SpecElement e;
		e.kind = SpecKind(r.get<uint8_t>());
		e.width = r.get<float>();
		e.dir = r.get<int32_t>();
		const uint8_t flags = r.get<uint8_t>();
		e.bus = flags & 1;
		e.solid = flags & 2;
		e.flush = flags & 4;
		e.pave = Paving(r.get<uint8_t>());
		spec.elements.push_back(e);
	}
	if (n == 0) return;
	spec.speed = r.get<float>();
	spec.priority = r.get<int32_t>();
	const uint8_t centre = r.get<uint8_t>();
	spec.centre = CENTRE_LINES[centre < CENTRE_LINE_COUNT ? centre : 0];
}

bool read_variants(ByteReader &r, Catalog &c, uint32_t version) {
	const uint32_t n = r.get<uint32_t>();
	const size_t variant_header = 3 * sizeof(int32_t) + sizeof(float) + (version >= SPEC_SAVE_VERSION ? sizeof(uint32_t) : 0);
	if (!r.require_items(n, variant_header, MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < n && r.ok; i++) {
		RoadVariant v;
		v.base = r.get<int32_t>();
		v.lanes = r.get<int32_t>();
		v.parking = r.get<int32_t>();
		v.width = r.get<float>();
		if (version >= SPEC_SAVE_VERSION) read_spec(r, v.spec);
		if (road_variant(c, v) < 0) return false;
	}
	return r.ok;
}
} // namespace

bool get_building(ByteReader &r, Building &b) {
	b.x = r.get<float>(); b.z = r.get<float>(); b.w = r.get<float>(); b.d = r.get<float>(); b.rot = r.get<float>();
	b.zone = r.get<uint8_t>();
	b.fn = r.get<uint8_t>();
	b.floors = r.get<uint16_t>();
	b.seed = r.get<uint32_t>();
	b.style = r.get<uint16_t>();
	b.profile = r.get<uint16_t>();
	b.landmark = r.get<int16_t>();
	b.built = r.get<double>();
	return r.ok;
}

namespace {
constexpr double PAD_ON_NODE_M = 0.5; // a round pad this near a node is that junction's (city_gen's), not a roundabout's disc

// A file older than the solved road heights kept, as terrain modifiers, the ground the old linear heights were laid on:
// a corridor along every road and a round pad under every junction. The ground is shaped to the roads from their solved
// heights now (sim/commands/road_grade_sync.cpp): those are dropped; what the player made of the ground otherwise (a
// roundabout's disc, centred where no node stands) stays.
void drop_baked_road_ground(CityState &s) {
	Heightfield &t = s.terrain;
	for (int id = 0; id < int(t.mods.size()); id++) {
		const TerrainModifier &m = t.mods[size_t(id)];
		if (!m.alive) continue;
		const bool corridor = m.pts.size() > 3;
		const bool pad = m.pts.size() == 3 && s.roads.find_node_near(m.pts[0], m.pts[2], PAD_ON_NODE_M) >= 0;
		if (corridor || pad) t.remove_modifier(id);
	}
}

// The planned cities stood on this much land before the file said so: the land comes from the seed and the side, and another side moves its water.
constexpr int PLANNED_TILES_BEFORE = 10;

// Inspect dimensions and backed sculpted tiles before found_land allocates the generated height grids. This reads
// only the fixed header and at most TERRAIN_MAX_REGIONS squared mask bytes; it does not decode the city a second time.
bool valid_land_header(ByteReader header, const Tuning &tune, int tiles) {
	header.get<double>(); header.get<uint8_t>(); header.get<double>(); header.get<double>(); // money, unlimited, clock
	const int n = header.get<int32_t>(), regions = header.get<int32_t>(), rs = header.get<int32_t>();
	const float station_count = std::round(tune.world_tile_m / tune.world_height_spacing_m); // same float ratio as Heightfield::init
	const int expected_regions = clampi(tiles, 2, TERRAIN_MAX_REGIONS);
	if (!header.ok || !std::isfinite(station_count) || station_count < 1 || double(station_count) > double(std::numeric_limits<int>::max() / expected_regions)) return false;
	if (regions != expected_regions || rs != int(station_count) || n != expected_regions * int(station_count)) return false;
	const uint32_t count = header.get<uint32_t>();
	if (count != uint32_t(regions * regions) || !header.require_items(count, sizeof(uint8_t))) return false;
	std::array<uint8_t, TERRAIN_MAX_REGIONS * TERRAIN_MAX_REGIONS> dirty{};
	header.get_bytes(dirty.data(), count);
	const size_t side = size_t(rs);
	if (side > std::numeric_limits<size_t>::max() / side / sizeof(uint16_t)) return false;
	size_t sculpted = 0;
	for (uint32_t i = 0; i < count; i++) {
		if (dirty[i] > 1) return false;
		sculpted += dirty[i];
	}
	return header.require_items(sculpted, side * side * sizeof(uint16_t));
}

// Everything the file holds, read into `s` (the land founded again from its seed first, or only sized when `checking`):
// false at the first thing that is not a city, with `s` left half replaced.
bool read_city(CityState &s, const uint8_t *data, size_t size, std::string &error, bool checking) {
	ByteReader r{ data, size };
	error = "save.file.damaged";
	if (r.get<uint32_t>() != SAVE_MAGIC || !r.ok) return false;
	const uint32_t version = r.get<uint32_t>();
	if (version > SAVE_VERSION) {
		error = "save.file.newer";
		return false;
	}
	int map = r.get<int32_t>();
	uint32_t seed = r.get<uint32_t>();
	if (!r.ok || map < 0 || size_t(map) >= s.catalog.maps.size()) return false;
	const MapDef &def = s.catalog.maps[size_t(map)];
	const int tiles = version >= LAND_SAVE_VERSION ? r.get<int32_t>() : def.medium ? PLANNED_TILES_BEFORE : def.tiles;
	if (!r.ok || tiles <= 0 || tiles > TERRAIN_MAX_REGIONS || !valid_land_header(r, s.catalog.tune, tiles)) return false;
	found_land(s, map, seed, !checking, tiles);
	s.money = r.get<double>();
	if (!std::isfinite(s.money)) return false;
	s.unlimited_money = r.get<uint8_t>() != 0;
	s.clock.minutes.store(r.get<double>());
	s.clock.day_shift = r.get<double>();
	if (!r.ok || !read_terrain(r, s.terrain)) return false;
	clear_variants(s.catalog);
	if (version >= VARIANTS_VERSION && !read_variants(r, s.catalog, version)) return false;
	if (!read_roads(r, s.roads, version) || !read_buildings(r, s.buildings)) return false;
	if (version < GRADE_SAVE_VERSION) drop_baked_road_ground(s);
	if (version >= NATURE_VERSION && !read_nature(r, s)) return false;
	std::vector<int64_t> growth_cells;
	r.get_vec(growth_cells);
	s.growth.assign(std::move(growth_cells)); // members follow the cells (GrowthQueue owns that)
	s.growth.grown = r.get<int32_t>();
	s.transit = TransitData();
	if (version >= TRANSIT_SAVE_VERSION && !read_transit(r, s.transit)) return false;
	transit_normalize(s.transit);
	s.transit_revision++;
	uint32_t tag = r.get<uint32_t>();
	if (tag == SAVE_BODIES) {
		if (!read_edited_bodies(r, s.buildings)) return false;
	} else {
		r.o -= sizeof(uint32_t); // not the edited bodies: the tag is the first of the extra blocks
	}
	return r.ok && read_extra_blocks(r, s); // the tagged blocks (demolition, street), then the end
}
} // namespace

bool check_city(Catalog catalog, const uint8_t *data, size_t size, std::string &error) {
	auto scratch = std::make_unique<CityState>();
	scratch->catalog = std::move(catalog);
	ComposeHold hold(scratch->terrain, false); // only checked: its final heights are never read
	if (!read_city(*scratch, data, size, error, true)) return false;
	error.clear();
	return true;
}

bool decode_city(CityState &s, const uint8_t *data, size_t size, std::string &error, OpenTimes *times) {
	OpenTimes spent;
	Laps laps;
	{
		ComposeHold hold(s.terrain); // the modifiers read (and the old ones dropped) and every road shaped: composed once
		if (!read_city(s, data, size, error, false)) return false;
		spent.read = laps.lap();
		s.roads.grade.everything(); // every road's height solved on the ground read, and the ground shaped to them
		sync_grade(s);
		spent.grade = laps.lap();
	}
	spent.compose = laps.lap();
	mark_all_lot_cells(s);
	s.lots.process_dirty(s);
	spent.lots = laps.lap();
	if (times) *times = spent;
	s.log.network_changed = true;
	s.ready = true;
	s.touch();
	s.saved_revision = s.revision;
	error.clear();
	return true;
}

} // namespace city
