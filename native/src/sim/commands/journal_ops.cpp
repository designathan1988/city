// Journalled mutations (Roadcraft RoadJournal live operations addNode/removeSeg/setProfile/setZone/addBuilding/…):
// each changes the state and, while a command records, appends the op with the record its inverse needs.
#include "sim/commands/edit_journal.h"

#include "sim/commands/building_refs.h"
#include "sim/commands/road_edit.h"
#include "sim/economy/road_economy.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
EditOp &record(CityState &s, OpKind k, int id, OpPayload payload) {
	EditEntry &e = s.history.open;
	e.ops.push_back(EditOp{ k, id, std::move(payload) });
	s.history.changed();
	s.history.hold(op_bytes(e.ops.back()));
	return e.ops.back();
}

void grow_seg(CityState &s, const RoadSegment &g) { s.history.open.grow(g.minx, g.minz, g.maxx, g.maxz); }
void grow_point(CityState &s, double x, double z, double r) { s.history.open.grow(x - r, z - r, x + r, z + r); }

// The memory a record holds besides itself.
size_t heap_of(const SignalSettings &v) { return v.greens.capacity() * sizeof(float); }
size_t heap_of(const RoadNode &n) { return n.segs.capacity() * sizeof(int) + n.links.capacity() * sizeof(LaneLink) + heap_of(n.signal); }
size_t heap_of(const RoadSegment &g) { return g.table.capacity() * sizeof(double); }
size_t heap_of(const Building &b) { return b.body ? sizeof(b40::BodyVersion) + b40::heap_bytes(*b.body) : 0; } // the version it holds
size_t heap_of(const PlayerLot &l) { return l.corners.capacity() * sizeof(LotPoint); }
size_t heap_of(const TransitStop &) { return 0; }
size_t heap_of(const RailTrack &k) { return k.points.capacity() * sizeof(V2); }
size_t heap_of(const TransitLine &l) { return l.stops.capacity() * sizeof(int) + l.name.capacity(); }
template <typename R>
size_t heap_of(const Listed<R> &l) { return heap_of(l.record); }
template <typename R>
size_t heap_of(const Edited<R> &e) { return heap_of(e.before) + heap_of(e.after); }

struct Heap {
	size_t operator()(const std::monostate &) const { return 0; }
	size_t operator()(double) const { return 0; }
	size_t operator()(const Change &) const { return 0; }
	size_t operator()(const ProfileChange &p) const { return heap_of(p.seg); }
	size_t operator()(const LiftChange &) const { return 0; }
	size_t operator()(const SignalChange &c) const { return heap_of(c.from) + heap_of(c.to); }
	size_t operator()(const RoadNode &n) const { return heap_of(n); }
	size_t operator()(const RoadSegment &g) const { return heap_of(g); }
	size_t operator()(const Building &b) const { return heap_of(b); }
	size_t operator()(const BuildingEdit &e) const { return heap_of(e.before) + heap_of(e.after); }
	size_t operator()(const TerrainModifier &m) const { return m.pts.capacity() * sizeof(float); }
	size_t operator()(const LinksChange &c) const { return (c.before.capacity() + c.after.capacity()) * sizeof(LaneLink); }
	size_t operator()(const Crater &) const { return 0; }
	size_t operator()(const RuinChange &c) const { return heap_of(c.ruin.record); }
	size_t operator()(const Gone &) const { return 0; }
	size_t operator()(const HurtChange &) const { return 0; }
	size_t operator()(const NatureChange &c) const { return sizeof(NatureEdit) + c.bytes; }
	size_t operator()(const StreetItem &i) const { return i.text.capacity() + i.given.form.capacity() + i.given.name.capacity(); }
	size_t operator()(const UtilityPole &p) const { return p.spans.capacity() * sizeof(int); }
	size_t operator()(const UtilitySpan &) const { return 0; }
	size_t operator()(const Barrier &b) const { return b.pts.capacity() * sizeof(float); }
	size_t operator()(const StreetCut &) const { return 0; }
	template <typename R>
	size_t operator()(const Listed<R> &l) const { return heap_of(l); }
	template <typename R>
	size_t operator()(const Edited<R> &e) const { return heap_of(e); }
};

// Two records of the transport plan alike in all they keep.
bool same(const V2 &a, const V2 &b) { return a.x == b.x && a.y == b.y; }
bool same(const TransitStop &a, const TransitStop &b) {
	return a.id == b.id && a.mode == b.mode && same(a.at, b.at) && a.seg == b.seg && a.track == b.track && a.terminal == b.terminal &&
			a.has_entrance == b.has_entrance && same(a.entrance, b.entrance);
}
bool same(const RailTrack &a, const RailTrack &b) {
	return a.id == b.id && a.mode == b.mode && a.points.size() == b.points.size() &&
			std::equal(a.points.begin(), a.points.end(), b.points.begin(), [](const V2 &p, const V2 &q) { return same(p, q); });
}
bool same(const TransitLine &a, const TransitLine &b) {
	return a.id == b.id && a.mode == b.mode && a.stops == b.stops && a.colour == b.colour && a.vehicles == b.vehicles && a.name == b.name;
}

// The ops that turn one list of the plan into the next, record by record (by id), each index the list's at the moment it runs:
// the records gone, from the last; those changed in place; those new, from the first. A record kept whose place among the kept
// ones moved is taken out and put back (the kept in order are the longest run of them in the old order).
template <typename R>
void record_list(CityState &s, const std::vector<R> &before, const std::vector<R> &after, OpKind add, OpKind rem, OpKind edit) {
	std::unordered_map<int, int> at_before;
	for (size_t i = 0; i < before.size(); i++) at_before.emplace(before[i].id, int(i));
	std::vector<int> common; // before-indices of the records kept, in the order of `after`
	std::vector<int> after_of(before.size(), -1);
	for (size_t j = 0; j < after.size(); j++) {
		const auto it = at_before.find(after[j].id);
		if (it == at_before.end()) continue;
		common.push_back(it->second);
		after_of[size_t(it->second)] = int(j);
	}
	// the longest increasing run of before-indices among the kept (patience sorting): they keep their places
	std::vector<int> tails, tail_at, prev(common.size(), -1);
	for (size_t k = 0; k < common.size(); k++) {
		const size_t len = size_t(std::lower_bound(tails.begin(), tails.end(), common[k]) - tails.begin());
		if (len == tails.size()) {
			tails.push_back(common[k]);
			tail_at.push_back(int(k));
		} else {
			tails[len] = common[k];
			tail_at[len] = int(k);
		}
		prev[k] = len > 0 ? tail_at[len - 1] : -1;
	}
	std::vector<char> stays(before.size(), 0);
	for (int k = tail_at.empty() ? -1 : tail_at.back(); k >= 0; k = prev[size_t(k)]) stays[size_t(common[size_t(k)])] = 1;
	for (size_t i = before.size(); i-- > 0;)
		if (!stays[i]) record(s, rem, before[i].id, Listed<R>{ int(i), before[i] });
	int index = 0; // the kept, in order, after the removals
	for (size_t i = 0; i < before.size(); i++) {
		if (!stays[i]) continue;
		const R &now = after[size_t(after_of[i])];
		if (!same(before[i], now)) record(s, edit, now.id, Edited<R>{ index, before[i], now });
		index++;
	}
	for (size_t j = 0; j < after.size(); j++) {
		const auto it = at_before.find(after[j].id);
		if (it == at_before.end() || !stays[size_t(it->second)]) record(s, add, after[j].id, Listed<R>{ int(j), after[j] });
	}
}

int lot_index(const LotBook &book, int id) {
	for (size_t i = 0; i < book.lots.size(); i++)
		if (book.lots[i].id == id) return int(i);
	return -1;
}
} // namespace

size_t op_bytes(const EditOp &op) { return sizeof(EditOp) + std::visit(Heap{}, op.payload); }

EditOp &j_record(CityState &s, OpKind k, int id, OpPayload payload) { return record(s, k, id, std::move(payload)); }

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
		record(s, OpKind::ADD_NODE, id, s.roads.nodes[id]);
		grow_point(s, x, z, 0);
	}
	return id;
}

void j_remove_node_if_orphan(CityState &s, int n) {
	const RoadNode &nd = s.roads.nodes[n];
	if (!nd.alive || !nd.segs.empty()) return;
	if (s.history.recording) {
		record(s, OpKind::REM_NODE, n, nd);
		grow_point(s, nd.x, nd.z, 0);
	}
	s.roads.remove_node_if_orphan(n);
	s.log.net_node(n);
	s.log.net_nodes_removed.push_back(n); // what the traffic keyed by this node goes with it (lane_graph_edit.cpp)
}

void j_set_control(CityState &s, int n, uint8_t ctl) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.ctl == ctl) return;
	if (s.history.recording) {
		record(s, OpKind::NODE_CTL, n, Change{ 0, nd.ctl, ctl });
		grow_point(s, nd.x, nd.z, 0);
	}
	nd.ctl = ctl;
	s.log.net_node(n);
}

void j_set_lift(CityState &s, int n, float lift) {
	RoadNode &nd = s.roads.nodes[n];
	if (s.history.recording) {
		record(s, OpKind::NODE_LIFT, n, LiftChange{ nd.lift, lift });
		grow_point(s, nd.x, nd.z, 0);
	}
	s.roads.set_lift(n, lift);
	s.log.net_node(n);
}

void j_set_crossing(CityState &s, int n, uint8_t kind) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.crossing == kind) return; // the same crossing again is nothing to undo
	if (s.history.recording) {
		record(s, OpKind::NODE_CROSSING, n, Change{ 0, nd.crossing, kind });
		grow_point(s, nd.x, nd.z, 0);
	}
	nd.crossing = kind;
	s.log.net_node(n);
}

void j_set_uturn(CityState &s, int n, uint8_t kind) {
	RoadNode &nd = s.roads.nodes[n];
	if (s.history.recording) {
		record(s, OpKind::NODE_UTURN, n, Change{ 0, nd.uturn, kind });
		grow_point(s, nd.x, nd.z, 0);
	}
	nd.uturn = kind;
	s.log.net_node(n);
}

void j_set_plain(CityState &s, int n, bool plain) {
	RoadNode &nd = s.roads.nodes[n];
	if (nd.plain == plain) return;
	if (s.history.recording) {
		record(s, OpKind::NODE_PLAIN, n, Change{ 0, nd.plain ? 1 : 0, plain ? 1 : 0 });
		grow_point(s, nd.x, nd.z, 0);
	}
	s.roads.set_plain(n, plain);
	s.log.net_node(n);
}

void j_set_signal(CityState &s, int n, const SignalSettings &to) {
	RoadNode &nd = s.roads.nodes[size_t(n)];
	if (nd.signal == to) return;
	if (s.history.recording) record(s, OpKind::NODE_SIGNAL, n, SignalChange{ nd.signal, to });
	nd.signal = to;
	s.log.signal_nodes.push_back(n);
}

int j_add_segment(CityState &s, int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags, float worth) {
	int id = s.roads.add_segment(a, b, q, profile, zl, zr, flags);
	{
		// what it is built as, recorded with it: handed in (a road that only changes hands: split, joined, turned), else as it is
		// laid by itself between the heights its two nodes stand at: the function the draft was priced by (road_economy.h)
		RoadSegment &g = s.roads.segs[id];
		const RoadNode &na = s.roads.nodes[size_t(a)], &nb = s.roads.nodes[size_t(b)];
		g.worth = worth >= 0 ? worth : float(built_alone(s, g.q, g.table, profile, flags, float(na.y), float(nb.y), na.lift != 0 || nb.lift != 0).worth);
	}
	s.log.net_seg(id);
	s.log.net_node(a);
	s.log.net_node(b);
	if (s.history.recording) {
		record(s, OpKind::ADD_SEG, id, s.roads.segs[id]);
		grow_seg(s, s.roads.segs[id]);
	}
	return id;
}

void j_remove_segment(CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	if (!g.alive) return;
	// What names the road goes with it, journalled in the same command: the hand links and bans of its two nodes, and the bus
	// stops on it, which stay inert (seg -1) until a road laid again carries them (road_refs.h) or the player moves them. An id
	// freed here is the next road's, and must bring it nothing.
	for (int n : { g.a, g.b }) {
		const std::vector<LaneLink> &held = s.roads.nodes[size_t(n)].links;
		const auto names = [seg](const LaneLink &l) { return l.from == seg || l.to == seg; };
		if (std::none_of(held.begin(), held.end(), names)) continue;
		std::vector<LaneLink> rest;
		rest.reserve(held.size());
		for (const LaneLink &l : held)
			if (!names(l)) rest.push_back(l);
		j_links(s, n, std::move(rest));
	}
	for (size_t i = 0; i < s.transit.stops.size(); i++)
		if (s.transit.stops[i].seg == seg) j_stop_road(s, int(i), -1);
	if (s.history.recording) {
		record(s, OpKind::REM_SEG, seg, g);
		grow_seg(s, g);
	}
	s.log.net_seg_removed(seg);
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	s.roads.remove_segment(seg);
}

void j_set_worth(CityState &s, int seg, float worth) {
	RoadSegment &g = s.roads.segs[size_t(seg)];
	if (g.worth == worth) return;
	if (s.history.recording) record(s, OpKind::SEG_WORTH, seg, LiftChange{ g.worth, worth });
	g.worth = worth;
}

void j_set_marking(CityState &s, int style) {
	if (s.history.recording) record(s, OpKind::MARKING, 0, Change{ 0, s.marking_style.load(), style });
	s.marking_style.store(style);
}

void j_set_profile(CityState &s, int seg, int profile) {
	RoadSegment &g = s.roads.segs[seg];
	if (g.profile == profile) return;
	if (s.history.recording) {
		record(s, OpKind::PROFILE, seg, ProfileChange{ g.profile, profile, g });
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
		record(s, OpKind::ZONE, seg, Change{ side, cur, zone, (g.flags & (side == 0 ? SEG_ZONED_LEFT : SEG_ZONED_RIGHT)) != 0 });
		grow_seg(s, g);
	}
	s.roads.set_zone(seg, side, zone);
	s.roads.set_player_zone(seg, side, zone != 0);
	mark_furniture(s, seg);
}

int j_add_building(CityState &s, const Building &b) {
	int id = s.buildings.add(b);
	if (s.history.recording) {
		record(s, OpKind::ADD_BLD, id, s.buildings.items[id]);
		grow_point(s, b.x, b.z, std::max(b.w, b.d));
	}
	return id;
}

void j_remove_building(CityState &s, int id) {
	const Building &b = s.buildings.items[id];
	if (!b.alive) return;
	if (s.history.recording) {
		record(s, OpKind::REM_BLD, id, b);
		grow_point(s, b.x, b.z, std::max(b.w, b.d));
	}
	const int64_t cell = chunk_of(b.x, b.z);
	s.buildings.remove(id);
	building_gone(s, id); // what named the building goes with it (building_refs.h)
	if (s.lots.lots_of(cell)) s.growth.push(cell); // its lot is open again: growth weighs the cell's lots anew (city_growth.h)
}

void j_edit_building(CityState &s, int id, const Building &after) {
	Building before = s.buildings.items[id];
	if (s.history.recording) {
		grow_point(s, before.x, before.z, std::max(before.w, before.d));
		record(s, OpKind::EDIT_BLD, id, BuildingEdit{ before, after });
	}
	s.buildings.remove(id);
	s.buildings.put(id, after, false);
}

int j_add_modifier(CityState &s, const std::vector<float> &pts, float flat, float max_run) {
	int id = s.terrain.add_modifier(pts, flat, max_run);
	if (s.history.recording) record(s, OpKind::ADD_MOD, id, s.terrain.mods[id]);
	return id;
}

int j_add_lot(CityState &s, PlayerLot lot) {
	LotBook &book = s.lot_book;
	lot.id = book.next_id++;
	book.lots.push_back(lot);
	if (s.history.recording) record(s, OpKind::ADD_LOT, lot.id, Listed<PlayerLot>{ int(book.lots.size()) - 1, std::move(lot) });
	s.touch();
	return book.lots.back().id;
}

void j_remove_lot(CityState &s, int id) {
	LotBook &book = s.lot_book;
	const int i = lot_index(book, id);
	if (i < 0) return;
	if (s.history.recording) record(s, OpKind::REM_LOT, id, Listed<PlayerLot>{ i, book.lots[size_t(i)] });
	book.lots.erase(book.lots.begin() + i);
	s.touch();
}

void j_edit_lot(CityState &s, int id, const PlayerLot &after) {
	LotBook &book = s.lot_book;
	const int i = lot_index(book, id);
	if (i < 0) return;
	if (s.history.recording) record(s, OpKind::EDIT_LOT, id, Edited<PlayerLot>{ i, book.lots[size_t(i)], after });
	book.lots[size_t(i)] = after;
	s.touch();
}

void j_links(CityState &s, int node, std::vector<LaneLink> next) {
	if (s.history.recording) {
		record(s, OpKind::NODE_LINKS, node, LinksChange{ s.roads.nodes[size_t(node)].links, next });
		const RoadNode &n = s.roads.nodes[size_t(node)];
		s.history.open.grow(float(n.x), float(n.z), float(n.x), float(n.z));
	}
	s.roads.nodes[size_t(node)].links = std::move(next);
	s.log.net_node(node);
	s.touch();
}

void j_transit(CityState &s, TransitData next) {
	if (s.history.recording) {
		const TransitData &t = s.transit;
		record_list(s, t.stops, next.stops, OpKind::ADD_STOP, OpKind::REM_STOP, OpKind::EDIT_STOP);
		record_list(s, t.tracks, next.tracks, OpKind::ADD_TRACK, OpKind::REM_TRACK, OpKind::EDIT_TRACK);
		record_list(s, t.lines, next.lines, OpKind::ADD_LINE, OpKind::REM_LINE, OpKind::EDIT_LINE);
		if (t.next_id != next.next_id) record(s, OpKind::TRANSIT_NEXT, -1, Change{ 0, t.next_id, next.next_id });
	}
	s.transit = std::move(next);
	s.transit_revision++;
	s.touch();
}

void j_stop_road(CityState &s, int index, int seg) {
	TransitStop &q = s.transit.stops[size_t(index)];
	if (q.seg == seg) return;
	if (s.history.recording) {
		TransitStop after = q;
		after.seg = seg;
		record(s, OpKind::EDIT_STOP, q.id, Edited<TransitStop>{ index, q, after });
		grow_point(s, q.at.x, q.at.y, 0);
	}
	q.seg = seg;
	s.transit_revision++;
	s.touch();
}

void j_money(CityState &s, double delta) {
	if (s.unlimited_money || delta == 0) return;
	s.money += delta;
	if (s.history.recording) record(s, OpKind::MONEY, -1, delta);
}

void j_terrain_touch(CityState &s, const SampleRect &r) {
	if (!s.history.recording || r.empty()) return;
	EditEntry &e = s.history.open;
	const Heightfield &t = s.terrain;
	const int rs = t.region_size;
	for (int tz = r.z0 / rs; tz <= r.z1 / rs; tz++)
		for (int tx = r.x0 / rs; tx <= r.x1 / rs; tx++) {
			const int tile = tz * t.regions + tx;
			auto [it, fresh] = e.terrain_open.try_emplace(tile);
			if (!fresh) continue;
			TerrainTouch &touch = it->second;
			touch.clean = !t.tile_dirty[size_t(tile)];
			touch.before.resize(size_t(rs) * size_t(rs));
			for (int z = 0; z < rs; z++) {
				const uint16_t *row = &t.base_q[size_t(tz * rs + z) * size_t(t.N) + size_t(tx * rs)];
				std::copy(row, row + rs, touch.before.begin() + std::ptrdiff_t(z) * rs);
			}
			s.history.changed();
			s.history.hold(touch.before.capacity() * sizeof(uint16_t) + sizeof(std::pair<const int, TerrainTouch>));
		}
	e.grow(t.x_of(r.x0), t.x_of(r.z0), t.x_of(r.x1), t.x_of(r.z1));
}

void j_nature(CityState &s, size_t bytes, NatureEdit edit) {
	if (!s.history.recording) return;
	record(s, OpKind::NATURE, -1, NatureChange{ std::make_shared<const NatureEdit>(std::move(edit)), bytes });
}

void j_paint_touch(CityState &s, const std::vector<int> &tiles) {
	if (!s.history.recording) return;
	for (int key : tiles) {
		auto [it, fresh] = s.history.open.paint_open.try_emplace(key);
		if (!fresh) continue;
		const PaintTile *t = s.nature.ground.tile(key);
		if (t) it->second = std::make_shared<const PaintTile>(*t);
		s.history.changed();
		s.history.hold(t ? sizeof(PaintTile) : 0);
	}
}

} // namespace city
