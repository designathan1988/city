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

EditOp &j_record(CityState &s, OpKind k, int id) { return record(s, k, id); }

void EditEntry::grow(double x0, double z0, double x1, double z1) {
	if (!has_box) {
		minx = x0; minz = z0; maxx = x1; maxz = z1;
		has_box = true;
		return;
	}
	minx = std::min(minx, x0); minz = std::min(minz, z0);
	maxx = std::max(maxx, x1); maxz = std::max(maxz, z1);
}

int j_add_node(CityState &s, double x, double z, double y, float lift) {
	int id = s.roads.add_node(x, z, y);
	s.roads.nodes[size_t(id)].lift = lift; // in the record the journal keeps: a redo lays it at the same height
	s.roads.nodes[id].plain = true; // a road the player lays ends in a straight cut: the bulb is chosen (set_road_end)
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

void j_set_control(CityState &s, int n, uint8_t ctl) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.ctl == ctl) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_CTL, n);
		op.from = nd.ctl;
		op.to = ctl;
		grow_point(s, nd.x, nd.z, 0);
	}
	nd.ctl = ctl;
	s.log.net_node(n);
}

void j_set_lift(CityState &s, int n, float lift) {
	RoadNode &nd = s.roads.nodes[n];
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_LIFT, n);
		op.lift_from = nd.lift;
		op.lift_to = lift;
		grow_point(s, nd.x, nd.z, 0);
	}
	s.roads.set_lift(n, lift);
	s.log.net_node(n);
}

void j_set_crossing(CityState &s, int n, uint8_t kind) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.crossing == kind) return; // the same crossing again is nothing to undo
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_CROSSING, n);
		op.from = nd.crossing;
		op.to = kind;
		grow_point(s, nd.x, nd.z, 0);
	}
	nd.crossing = kind;
	s.log.net_node(n);
}

void j_set_uturn(CityState &s, int n, uint8_t kind) {
	RoadNode &nd = s.roads.nodes[n];
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_UTURN, n);
		op.from = nd.uturn;
		op.to = kind;
		grow_point(s, nd.x, nd.z, 0);
	}
	nd.uturn = kind;
	s.log.net_node(n);
}

void j_set_plain(CityState &s, int n, bool plain) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.plain == plain) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_PLAIN, n);
		op.from = nd.plain ? 1 : 0;
		op.to = plain ? 1 : 0;
		grow_point(s, nd.x, nd.z, 0);
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

void j_set_marking(CityState &s, int style) {
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::MARKING, 0);
		op.from = s.marking_style.load();
		op.to = style;
	}
	s.marking_style.store(style);
}

void j_set_profile(CityState &s, int seg, int profile) {
	RoadSegment &g = s.roads.segs[seg];
	if (g.profile == profile) return;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::PROFILE, seg);
		op.from = g.profile;
		op.to = profile;
		op.seg = g; // what the change is worth: its length, its structure and the heights at its ends
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
		op.player = (g.flags & (side == 0 ? SEG_ZONED_LEFT : SEG_ZONED_RIGHT)) != 0;
		grow_seg(s, g);
	}
	s.roads.set_zone(seg, side, zone);
	s.roads.set_player_zone(seg, side, zone != 0);
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

void j_lots(CityState &s, LotBook next) {
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::LOTS, -1);
		op.lots_before = std::make_shared<const LotBook>(s.lot_book);
		op.lots_after = std::make_shared<const LotBook>(next);
	}
	s.lot_book = std::move(next);
	s.touch();
}

void j_links(CityState &s, int node, std::vector<LaneLink> next) {
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::NODE_LINKS, node);
		op.links_before = s.roads.nodes[size_t(node)].links;
		op.links_after = next;
		const RoadNode &n = s.roads.nodes[size_t(node)];
		s.history.open.grow(float(n.x), float(n.z), float(n.x), float(n.z));
	}
	s.roads.nodes[size_t(node)].links = std::move(next);
	s.log.net_node(node);
	s.touch();
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
			if (!e.terrain_open.emplace(i, t.base_q[i]).second) continue;
			const int tile = (z / t.region_size) * t.regions + x / t.region_size;
			if (!t.tile_dirty[size_t(tile)] && std::find(e.clean_tiles.begin(), e.clean_tiles.end(), tile) == e.clean_tiles.end())
				e.clean_tiles.push_back(tile);
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
