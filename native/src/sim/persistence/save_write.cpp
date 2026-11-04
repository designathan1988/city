// Writing a city (save_format.h): header, sculpted height tiles, terrain modifiers, road nodes and segments with
// their ids, building records and the growth queue.
#include "sim/persistence/save_format.h"
#include "sim/persistence/body_codec.h"

#include "core/bytes.h"
#include "sim/state/city_state.h"

namespace city {

namespace {
void put_quad(ByteWriter &w, const Quad &q) {
	for (double v : { q.x0, q.z0, q.cx, q.cz, q.x1, q.z1 }) w.put<double>(v);
}

void write_terrain(ByteWriter &w, const Heightfield &t) {
	w.put<int32_t>(t.N);
	w.put<int32_t>(t.regions);
	w.put<int32_t>(t.region_size);
	w.put_vec(t.tile_dirty);
	for (int r = 0; r < int(t.tile_dirty.size()); r++) {
		if (!t.tile_dirty[r]) continue;
		int x0 = (r % t.regions) * t.region_size, z0 = (r / t.regions) * t.region_size;
		for (int z = z0; z < z0 + t.region_size; z++) w.put_bytes(&t.base_q[size_t(z) * t.N + x0], size_t(t.region_size) * sizeof(uint16_t));
	}
	w.put<uint32_t>(uint32_t(t.mods.size()));
	for (const TerrainModifier &m : t.mods) {
		w.put<uint8_t>(m.alive ? 1 : 0);
		if (!m.alive) continue;
		for (float v : { m.flat, m.max_run, m.minx, m.minz, m.maxx, m.maxz }) w.put<float>(v);
		w.put_vec(m.pts);
	}
}

void write_roads(ByteWriter &w, const RoadNetwork &net) {
	w.put<uint32_t>(uint32_t(net.nodes.size()));
	for (const RoadNode &n : net.nodes) {
		w.put<uint8_t>(n.alive ? 1 : 0);
		if (!n.alive) continue;
		for (double v : { n.x, n.z, n.y }) w.put<double>(v);
		w.put<uint8_t>(n.ctl);
		w.put<uint8_t>(n.signal.fixed ? 1 : 0);
		w.put<float>(n.signal.offset);
		w.put_vec(n.signal.greens);
		w.put<uint8_t>(n.plain ? 1 : 0);
	}
	w.put<uint32_t>(uint32_t(net.segs.size()));
	for (const RoadSegment &g : net.segs) {
		w.put<uint8_t>(g.alive ? 1 : 0);
		if (!g.alive) continue;
		w.put<int32_t>(g.a);
		w.put<int32_t>(g.b);
		put_quad(w, g.q);
		w.put<int32_t>(g.profile);
		w.put<uint8_t>(g.zl);
		w.put<uint8_t>(g.zr);
		w.put<uint16_t>(g.flags);
	}
}

void write_buildings(ByteWriter &w, const BuildingStore &bs) {
	w.put<uint32_t>(uint32_t(bs.items.size()));
	for (const Building &b : bs.items) {
		w.put<uint8_t>(b.alive ? 1 : 0);
		if (!b.alive) continue;
		for (float v : { b.x, b.z, b.w, b.d, b.rot }) w.put<float>(v);
		w.put<uint8_t>(b.zone);
		w.put<uint8_t>(b.fn);
		w.put<uint16_t>(b.floors);
		w.put<uint32_t>(b.seed);
		w.put<uint16_t>(b.style);
		w.put<uint16_t>(b.profile);
		w.put<int16_t>(b.landmark);
		w.put<double>(b.built);
	}
}
} // namespace

std::vector<uint8_t> encode_city(const CityState &s) {
	ByteWriter w;
	w.put<uint32_t>(SAVE_MAGIC);
	w.put<uint32_t>(SAVE_VERSION);
	w.put<int32_t>(s.map_index);
	w.put<uint32_t>(s.seed);
	w.put<double>(s.money);
	w.put<uint8_t>(s.unlimited_money ? 1 : 0);
	w.put<double>(s.clock.minutes.load());
	w.put<double>(s.clock.day_shift.load());
	write_terrain(w, s.terrain);
	write_roads(w, s.roads);
	write_buildings(w, s.buildings);
	write_nature(w, s);
	w.put_vec(s.growth.cells);
	w.put<int32_t>(s.growth.grown);
	w.put<uint32_t>(SAVE_BODIES);
	write_edited_bodies(w, s.buildings);
	w.put<uint32_t>(SAVE_END);
	return std::move(w.buf);
}

} // namespace city
