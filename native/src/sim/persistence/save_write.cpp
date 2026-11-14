// Writing a city (save_format.h): header, sculpted height tiles, terrain modifiers, road nodes and segments with
// their ids, building records and the growth queue.
#include "sim/persistence/save_format.h"
#include "sim/persistence/body_codec.h"

#include "core/bytes.h"
#include "sim/persistence/save_blocks.h"
#include "sim/persistence/save_demolition.h"
#include "sim/state/city_state.h"

#include <algorithm>

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
	w.put_slots(t.mods.size());
	for (const TerrainModifier &m : t.mods) {
		if (!w.put_alive(m.alive)) continue;
		for (float v : { m.flat, m.max_run, m.minx, m.minz, m.maxx, m.maxz }) w.put<float>(v);
		w.put_vec(m.pts);
	}
}

void write_variants(ByteWriter &w, const std::vector<RoadVariant> &list) {
	w.put<uint32_t>(uint32_t(list.size()));
	for (const RoadVariant &v : list) {
		w.put<int32_t>(v.base);
		w.put<int32_t>(v.lanes);
		w.put<int32_t>(v.parking);
		w.put<float>(v.width);
		w.put<uint32_t>(uint32_t(v.spec.elements.size()));
		for (const SpecElement &e : v.spec.elements) {
			w.put<uint8_t>(uint8_t(e.kind));
			w.put<float>(e.width);
			w.put<int32_t>(e.dir);
			w.put<uint8_t>(uint8_t((e.bus ? 1 : 0) | (e.solid ? 2 : 0) | (e.flush ? 4 : 0)));
			w.put<uint8_t>(uint8_t(e.pave));
		}
		if (v.spec.elements.empty()) continue;
		w.put<float>(v.spec.speed);
		w.put<int32_t>(v.spec.priority);
		w.put<uint8_t>(uint8_t(std::find(CENTRE_LINES, CENTRE_LINES + CENTRE_LINE_COUNT, v.spec.centre) - CENTRE_LINES));
		w.put<int8_t>(v.spec.surface);
	}
}

void write_roads(ByteWriter &w, const RoadNetwork &net) {
	w.put_slots(net.nodes.size());
	for (const RoadNode &n : net.nodes) {
		if (!w.put_alive(n.alive)) continue;
		for (double v : { n.x, n.z }) w.put<double>(v);
		w.put<float>(n.lift); // its height is solved again from the roads and the ground on load
		w.put<uint8_t>(n.ctl);
		w.put<uint8_t>(n.signal.fixed ? 1 : 0);
		w.put<float>(n.signal.offset);
		w.put_vec(n.signal.greens);
		w.put<uint8_t>(n.signal.buses_first ? 1 : 0);
		w.put<uint8_t>(n.plain ? 1 : 0);
	}
	w.put_slots(net.segs.size());
	for (const RoadSegment &g : net.segs) {
		if (!w.put_alive(g.alive)) continue;
		w.put<int32_t>(g.a);
		w.put<int32_t>(g.b);
		put_quad(w, g.q);
		w.put<int32_t>(g.profile);
		w.put<uint8_t>(g.zl);
		w.put<uint8_t>(g.zr);
		w.put<uint16_t>(g.flags);
		w.put<float>(g.worth); // what it was built as: what it cost and gives back (road_network.h)
	}
}

void write_buildings(ByteWriter &w, const BuildingStore &bs) {
	w.put_slots(bs.items.size());
	for (const Building &b : bs.items) {
		if (w.put_alive(b.alive)) put_building(w, b);
	}
}
void write_transit(ByteWriter &w, const TransitData &t) {
	w.put<int32_t>(t.next_id);
	w.put<uint32_t>(uint32_t(t.stops.size()));
	for (const TransitStop &q : t.stops) {
		w.put<int32_t>(q.id);
		w.put<uint8_t>(uint8_t(q.mode));
		w.put<float>(q.at.x);
		w.put<float>(q.at.y);
		w.put<int32_t>(q.seg);
		w.put<int32_t>(q.track);
		w.put<uint8_t>(uint8_t(q.terminal) | uint8_t(q.has_entrance ? 2 : 0));
		w.put<float>(q.entrance.x);
		w.put<float>(q.entrance.y);
	}
	w.put<uint32_t>(uint32_t(t.tracks.size()));
	for (const RailTrack &k : t.tracks) {
		w.put<int32_t>(k.id);
		w.put<uint8_t>(uint8_t(k.mode));
		w.put_vec(k.points);
	}
	w.put<uint32_t>(uint32_t(t.lines.size()));
	for (const TransitLine &l : t.lines) {
		w.put<int32_t>(l.id);
		w.put<uint8_t>(uint8_t(l.mode));
		w.put_vec(l.stops);
		w.put<uint32_t>(l.colour);
		w.put<int32_t>(l.vehicles);
		w.put_string(l.name);
	}
}
} // namespace

void put_building(ByteWriter &w, const Building &b) {
	for (float v : { b.x, b.z, b.w, b.d, b.rot }) w.put<float>(v);
	w.put<uint8_t>(b.zone);
	w.put<uint8_t>(b.fn);
	w.put<uint16_t>(b.floors);
	w.put<uint16_t>(b.cap);
	w.put<uint32_t>(b.seed);
	w.put<uint16_t>(b.style);
	w.put<uint16_t>(b.profile);
	w.put<int16_t>(b.landmark);
	w.put<double>(b.built);
}

std::vector<uint8_t> encode_city(const CityState &s, std::vector<SavePart> *parts, size_t expect) {
	ByteWriter w;
	w.canonical = parts != nullptr;
	w.buf.reserve(expect);
	const auto mark = [&](const char *name) {
		if (parts) parts->push_back({ name, w.buf.size() });
	};
	w.put<uint32_t>(SAVE_MAGIC);
	w.put<uint32_t>(SAVE_VERSION);
	w.put<int32_t>(s.map_index);
	w.put<uint32_t>(s.seed);
	w.put<int32_t>(s.map_tiles);
	mark("header");
	w.put<double>(s.money);
	w.put<uint8_t>(s.unlimited_money ? 1 : 0);
	mark("money");
	w.put<double>(s.clock.minutes.load());
	w.put<double>(s.clock.day_shift.load());
	mark("clock");
	write_terrain(w, s.terrain);
	mark("terrain");
	write_variants(w, s.catalog.variants);
	mark("variants");
	write_roads(w, s.roads);
	mark("roads");
	write_buildings(w, s.buildings);
	mark("buildings");
	write_nature(w, s);
	mark("nature");
	w.put_vec(s.growth.cells);
	w.put<int32_t>(s.growth.grown);
	mark("growth");
	write_transit(w, s.transit);
	mark("transit");
	w.put<uint32_t>(SAVE_BODIES);
	write_edited_bodies(w, s.buildings);
	mark("bodies");
	write_extra_blocks(w, s, mark);
	w.put<uint32_t>(SAVE_END);
	mark("end");
	return std::move(w.buf);
}

} // namespace city
