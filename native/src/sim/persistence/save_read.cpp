// Reading a city (save_format.h): the land is regenerated from the saved seed, then the sculpted tiles, modifiers,
// roads, buildings and growth go back under their ids; lots and node shapes are derived again.
#include "sim/persistence/save_format.h"
#include "sim/roads/road_variants.h"
#include "sim/persistence/body_codec.h"

#include "core/bytes.h"
#include "sim/persistence/save_demolition.h"
#include "sim/citygen/city_gen.h"
#include "sim/state/city_state.h"

#include <cmath>
#include <memory>

namespace city {

namespace {
constexpr uint32_t MAX_RECORDS = 50000000; // sanity bound on any saved table

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
	t.recompose(0, 0, n - 1, n - 1);
	uint32_t mods = r.get<uint32_t>();
	if (!r.ok || mods > MAX_RECORDS) return false;
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
	if (!r.ok || nodes > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < nodes && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		RoadNode n;
		n.x = r.get<double>(); n.z = r.get<double>(); n.y = r.get<double>();
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
	if (!r.ok || segs > MAX_RECORDS) return false;
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
	if (!r.ok || count > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		Building b;
		if (get_building(r, b)) bs.put(int(i), b, false);
	}
	return r.ok;
}

bool read_transit(ByteReader &r, TransitData &t) {
	t = TransitData();
	t.next_id = r.get<int32_t>();
	const uint32_t stops = r.get<uint32_t>();
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
		t.stops.push_back(q);
	}
	const uint32_t tracks = r.get<uint32_t>();
	for (uint32_t i = 0; i < tracks && r.ok; i++) {
		RailTrack k;
		k.id = r.get<int32_t>();
		k.mode = TransitMode(r.get<uint8_t>());
		r.get_vec(k.points);
		t.tracks.push_back(std::move(k));
	}
	const uint32_t lines = r.get<uint32_t>();
	for (uint32_t i = 0; i < lines && r.ok; i++) {
		TransitLine l;
		l.id = r.get<int32_t>();
		l.mode = TransitMode(r.get<uint8_t>());
		r.get_vec(l.stops);
		l.colour = r.get<uint32_t>();
		l.vehicles = r.get<int32_t>();
		l.name.resize(r.get<uint32_t>());
		r.get_bytes(l.name.data(), l.name.size());
		t.lines.push_back(std::move(l));
	}
	return r.ok;
}
} // namespace

namespace {
constexpr uint32_t NATURE_VERSION = 2, VARIANTS_VERSION = 3;

// Made again in the saved order, so each keeps the index the roads hold.
bool read_variants(ByteReader &r, Catalog &c) {
	const uint32_t n = r.get<uint32_t>();
	for (uint32_t i = 0; i < n && r.ok; i++) {
		RoadVariant v;
		v.base = r.get<int32_t>();
		v.lanes = r.get<int32_t>();
		v.parking = r.get<int32_t>();
		v.width = r.get<float>();
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
// Everything the file holds, read into `s` (the land founded again from its seed first): false at the first thing that is
// not a city, with `s` left half replaced.
bool read_city(CityState &s, const uint8_t *data, size_t size, std::string &error) {
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
	found_land(s, map, seed);
	s.money = r.get<double>();
	if (!std::isfinite(s.money)) return false;
	s.unlimited_money = r.get<uint8_t>() != 0;
	s.clock.minutes.store(r.get<double>());
	s.clock.day_shift = r.get<double>();
	if (!r.ok || !read_terrain(r, s.terrain)) return false;
	clear_variants(s.catalog);
	if (version >= VARIANTS_VERSION && !read_variants(r, s.catalog)) return false;
	if (!read_roads(r, s.roads, version) || !read_buildings(r, s.buildings)) return false;
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
	return r.ok && read_extra_blocks(r, s); // the demolition block, then the end
}
} // namespace

bool check_city(Catalog catalog, const uint8_t *data, size_t size, std::string &error) {
	auto scratch = std::make_unique<CityState>();
	scratch->catalog = std::move(catalog);
	if (!read_city(*scratch, data, size, error)) return false;
	error.clear();
	return true;
}

bool decode_city(CityState &s, const uint8_t *data, size_t size, std::string &error) {
	if (!read_city(s, data, size, error)) return false;
	mark_all_lot_cells(s);
	s.lots.process_dirty(s);
	s.log.network_changed = true;
	s.ready = true;
	s.touch();
	s.saved_revision = s.revision;
	error.clear();
	return true;
}

} // namespace city
