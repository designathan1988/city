// See save_links.h: each node with links, by index, and its links.
#include "sim/persistence/save_links.h"

#include "core/bytes.h"
#include "sim/roads/road_network.h"

namespace city {

bool write_links_block(ByteWriter &w, const RoadNetwork &net) {
	uint32_t nodes = 0;
	for (const RoadNode &n : net.nodes) nodes += n.alive && !n.links.empty() ? 1 : 0;
	if (nodes == 0) return false;
	w.put<uint32_t>(nodes);
	for (size_t i = 0; i < net.nodes.size(); i++) {
		const RoadNode &n = net.nodes[i];
		if (!n.alive || n.links.empty()) continue;
		w.put<uint32_t>(uint32_t(i));
		w.put<uint32_t>(uint32_t(n.links.size()));
		for (const LaneLink &l : n.links)
			for (int v : { l.from, l.from_lane, l.to, l.to_lane }) w.put<int32_t>(v);
	}
	return true;
}

bool read_links_block(ByteReader &r, RoadNetwork &net) {
	const uint32_t nodes = r.get<uint32_t>();
	if (!r.require_items(nodes, 2 * sizeof(uint32_t), net.nodes.size())) return false;
	for (uint32_t k = 0; k < nodes && r.ok; k++) {
		const uint32_t i = r.get<uint32_t>(), n = r.get<uint32_t>();
		if (!r.ok || i >= net.nodes.size()) return false;
		if (!r.require_items(n, 4 * sizeof(int32_t))) return false;
		for (uint32_t j = 0; j < n && r.ok; j++) {
			LaneLink l;
			l.from = r.get<int32_t>();
			l.from_lane = r.get<int32_t>();
			l.to = r.get<int32_t>();
			l.to_lane = r.get<int32_t>();
			net.nodes[i].links.push_back(l);
		}
	}
	return r.ok;
}

bool write_crossings_block(ByteWriter &w, const RoadNetwork &net) {
	uint32_t nodes = 0;
	for (const RoadNode &n : net.nodes) nodes += n.alive && (n.crossing != CROSSING_NONE || n.uturn != UTURN_NONE) ? 1 : 0;
	if (nodes == 0) return false;
	w.put<uint32_t>(nodes);
	for (size_t i = 0; i < net.nodes.size(); i++) {
		if (!net.nodes[i].alive || (net.nodes[i].crossing == CROSSING_NONE && net.nodes[i].uturn == UTURN_NONE)) continue;
		w.put<uint32_t>(uint32_t(i));
		w.put<uint8_t>(net.nodes[i].crossing);
		w.put<uint8_t>(net.nodes[i].uturn);
	}
	return true;
}

bool read_crossings_block(ByteReader &r, RoadNetwork &net) {
	const uint32_t nodes = r.get<uint32_t>();
	if (!r.require_items(nodes, sizeof(uint32_t) + 2 * sizeof(uint8_t), net.nodes.size())) return false;
	for (uint32_t k = 0; k < nodes && r.ok; k++) {
		const uint32_t i = r.get<uint32_t>();
		const uint8_t kind = r.get<uint8_t>(), uturn = r.get<uint8_t>();
		if (!r.ok || i >= net.nodes.size()) return false;
		net.nodes[i].crossing = kind;
		net.nodes[i].uturn = uturn;
	}
	return r.ok;
}

} // namespace city
