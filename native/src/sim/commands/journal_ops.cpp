// Journalled mutations (Roadcraft RoadJournal live operations addNode/removeSeg/setProfile/setZone/addBuilding/…):
// each changes the state and, while a command records, appends the op with the record its inverse needs.
#include "sim/commands/edit_journal.h"

#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
EditOp &record(CityState &s, OpKind k, int id) {
	EditEntry &e = s.history.open;
	e.ops.emplace_back();
	EditOp &op = e.ops.back();
	op.kind = k;
	op.id = id;
	e.bytes += sizeof(EditOp); // the op carries full records; the old fixed 64 B undercounted it several times
	return op;
}

void grow_seg(CityState &s, const RoadSegment &g) { s.history.open.grow(g.minx, g.minz, g.maxx, g.maxz); }
void grow_point(CityState &s, double x, double z, double r) { s.history.open.grow(x - r, z - r, x + r, z + r); }
} // namespace

void EditEntry::grow(double x0, double z0, double x1, double z1) {
	if (!has_box) {
		minx = x0; minz = z0; maxx = x1; maxz = z1;
		has_box = true;
		return;
	}
	minx = std::min(minx, x0); minz = std::min(minz, z0);
	maxx = std::max(maxx, x1); maxz = std::max(maxz, z1);
}

int j_add_node(CityState &s, double x, double z, double y) {
	int id = s.roads.add_node(x, z, y);
	s.log.net_node(id);
	if (s.history.recording) {
		record(s, OpKind::ADD_NODE, id).node = s.roads.nodes[id];
		grow_point(s, x, z, 0);
	}
	return id;
}

void j_remove_node_if_orphan(CityState &s, int n) {
	const RoadNode &nd = s.roads.nodes[n];
	if (!nd.alive || !nd.segs.empty()) return;
	if (s.history.recording) {
		record(s, OpKind::REM_NODE, n).node = nd;
		grow_point(s, nd.x, nd.z, 0);
	}
	s.roads.remove_node_if_orphan(n);
	s.log.net_node(n);
}

void j_set_plain(CityState &s, int n, bool plain) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.plain == plain) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_PLAIN, n);
		op.from = nd.plain ? 1 : 0;
		op.to = plain ? 1 : 0;
	}
	nd.plain = plain;
	s.log.net_node(n);
}

void j_set_signal(CityState &s, int n, const SignalSettings &to) {
	RoadNode &nd = s.roads.nodes[size_t(n)];
	if (nd.signal == to) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_SIGNAL, n);
		op.signal_from = nd.signal;
		op.signal_to = to;
	}
	nd.signal = to;
	s.log.signal_nodes.push_back(n);
}

int j_add_segment(CityState &s, int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags) {
	int id = s.roads.add_segment(a, b, q, profile, zl, zr, flags);
	s.log.net_seg(id);
	s.log.net_node(a);
	s.log.net_node(b);
	if (s.history.recording) {
		record(s, OpKind::ADD_SEG, id).seg = s.roads.segs[id];
		grow_seg(s, s.roads.segs[id]);
	}
	return id;
}

void j_remove_segment(CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	if (!g.alive) return;
	if (s.history.recording) {
		record(s, OpKind::REM_SEG, seg).seg = g;
		grow_seg(s, g);
	}
	s.log.net_seg_removed(seg);
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	s.roads.remove_segment(seg);
}

void j_set_profile(CityState &s, int seg, int profile) {
	RoadSegment &g = s.roads.segs[seg];
	if (g.profile == profile) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::PROFILE, seg);
		op.from = g.profile;
		op.to = profile;
		grow_seg(s, g);
	}
	s.roads.set_profile(seg, profile);
	s.log.net_seg(seg);
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	if (s.history.recording) grow_seg(s, s.roads.segs[seg]);
}

void j_set_zone(CityState &s, int seg, int side, int zone) {
	RoadSegment &g = s.roads.segs[seg];
	int cur = side == 0 ? g.zl : g.zr;
	if (cur == zone) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::ZONE, seg);
		op.side = side;
		op.from = cur;
		op.to = zone;
		grow_seg(s, g);
	}
	s.roads.set_zone(seg, side, zone);
	mark_furniture(s, seg);
}

int j_add_building(CityState &s, const Building &b) {
	int id = s.buildings.add(b);
	if (s.history.recording) {
		record(s, OpKind::ADD_BLD, id).bld = s.buildings.items[id];
		grow_point(s, b.x, b.z, std::max(b.w, b.d));
	}
	return id;
}

void j_remove_building(CityState &s, int id) {
	const Building &b = s.buildings.items[id];
	if (!b.alive) return;
	if (s.history.recording) {
		record(s, OpKind::REM_BLD, id).bld = b;
		grow_point(s, b.x, b.z, std::max(b.w, b.d));
	}
	s.buildings.remove(id);
}

void j_edit_building(CityState &s, int id, const Building &after) {
	Building before = s.buildings.items[id];
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::EDIT_BLD, id);
		op.bld = before;
		op.bld_after = after;
		grow_point(s, before.x, before.z, std::max(before.w, before.d));
	}
	s.buildings.remove(id);
	s.buildings.put(id, after, false);
}

int j_add_modifier(CityState &s, const std::vector<float> &pts, float flat, float max_run) {
	int id = s.terrain.add_modifier(pts, flat, max_run);
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::ADD_MOD, id);
		op.mod = s.terrain.mods[id];
		s.history.open.bytes += pts.size() * sizeof(float);
	}
	return id;
}

void j_transit(CityState &s, TransitData next) {
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::TRANSIT, -1);
		op.transit_before = std::make_shared<const TransitData>(s.transit);
		op.transit_after = std::make_shared<const TransitData>(next);
	}
	s.transit = std::move(next);
	s.transit_revision++;
	s.touch();
}

void j_money(CityState &s, double delta) {
	if (s.unlimited_money || delta == 0) return;
	s.money += delta;
	if (s.history.recording) record(s, OpKind::MONEY, -1).money = delta;
}

void j_terrain_touch(CityState &s, const SampleRect &r) {
	if (!s.history.recording || r.empty()) return;
	EditEntry &e = s.history.open;
	const Heightfield &t = s.terrain;
	for (int z = r.z0; z <= r.z1; z++)
		for (int x = r.x0; x <= r.x1; x++) {
			uint32_t i = uint32_t(z) * uint32_t(t.N) + uint32_t(x);
			e.terrain_open.emplace(i, t.base_q[i]);
		}
	e.grow(t.x_of(r.x0), t.x_of(r.z0), t.x_of(r.x1), t.x_of(r.z1));
}

void j_nature(CityState &s, size_t bytes, NatureEdit edit) {
	if (!s.history.recording) return;
	record(s, OpKind::NATURE, -1).nature = std::make_shared<const NatureEdit>(std::move(edit));
	s.history.open.bytes += bytes;
}

void j_paint_touch(CityState &s, const std::vector<int> &tiles) {
	if (!s.history.recording) return;
	for (int key : tiles) {
		if (s.history.open.paint_open.count(key)) continue;
		const PaintTile *t = s.nature.ground.tile(key);
		s.history.open.paint_open[key] = t ? std::make_shared<const PaintTile>(*t) : nullptr;
	}
}

} // namespace city
