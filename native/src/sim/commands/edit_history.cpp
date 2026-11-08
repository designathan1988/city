// The undo / redo stacks (Roadcraft core/commands.ts CommandStack: limit 500 steps / 512 MiB, groups for strokes,
// redo cleared by a new step) and the inverse of each recorded op.
#include "sim/commands/edit_journal.h"

#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr size_t BYTES_PER_MB = size_t(1) << 20;
constexpr double SETTLE_PAD_M = 4; // the box an undo settles reaches one height sample past what it touched

// The lane graph follows an undo/redo the same way it follows a journal op: mark what the traffic must rebuild.
void mark_seg(CityState &s, int id, bool removed) {
	const RoadSegment &g = s.roads.segs[id];
	s.log.net_node(g.a);
	s.log.net_node(g.b);
	(removed ? s.log.net_seg_removed(id) : s.log.net_seg(id));
}

// A record of an ordered list put back at its place, or taken from it.
template <typename R>
void put_listed(std::vector<R> &list, const Listed<R> &l, bool in) {
	if (in) list.insert(list.begin() + std::min<std::ptrdiff_t>(l.index, std::ptrdiff_t(list.size())), l.record);
	else if (l.index >= 0 && l.index < int(list.size())) list.erase(list.begin() + l.index);
}
template <typename R>
void put_edited(std::vector<R> &list, const Edited<R> &e, bool fwd) {
	if (e.index >= 0 && e.index < int(list.size())) list[size_t(e.index)] = fwd ? e.after : e.before;
}

void exec_lot(CityState &s, const EditOp &op, bool fwd) {
	LotBook &book = s.lot_book;
	if (op.kind == OpKind::EDIT_LOT) {
		const Edited<PlayerLot> &e = op.get<Edited<PlayerLot>>();
		book.set(e.index, fwd ? e.after : e.before);
		mark_lot_cells(s, e.before.corners);
		mark_lot_cells(s, e.after.corners);
		return;
	}
	const Listed<PlayerLot> &l = op.get<Listed<PlayerLot>>();
	const bool in = (op.kind == OpKind::ADD_LOT) == fwd;
	if (in) book.insert(l.index, l.record);
	else book.erase(l.index);
	if (op.kind == OpKind::ADD_LOT) book.next_id = fwd ? l.record.id + 1 : l.record.id; // j_add_lot gave it the book's next id
	mark_lot_cells(s, l.record.corners);
}

void exec_transit(CityState &s, const EditOp &op, bool fwd) {
	TransitData &t = s.transit;
	switch (op.kind) {
		case OpKind::ADD_STOP: case OpKind::REM_STOP: put_listed(t.stops, op.get<Listed<TransitStop>>(), (op.kind == OpKind::ADD_STOP) == fwd); break;
		case OpKind::EDIT_STOP: put_edited(t.stops, op.get<Edited<TransitStop>>(), fwd); break;
		case OpKind::ADD_TRACK: case OpKind::REM_TRACK: put_listed(t.tracks, op.get<Listed<RailTrack>>(), (op.kind == OpKind::ADD_TRACK) == fwd); break;
		case OpKind::EDIT_TRACK: put_edited(t.tracks, op.get<Edited<RailTrack>>(), fwd); break;
		case OpKind::ADD_LINE: case OpKind::REM_LINE: put_listed(t.lines, op.get<Listed<TransitLine>>(), (op.kind == OpKind::ADD_LINE) == fwd); break;
		case OpKind::EDIT_LINE: put_edited(t.lines, op.get<Edited<TransitLine>>(), fwd); break;
		case OpKind::TRANSIT_NEXT: t.next_id = fwd ? op.get<Change>().to : op.get<Change>().from; break;
		default: return;
	}
	s.transit_revision++;
}

void exec(CityState &s, const EditOp &op, bool fwd) {
	RoadNetwork &net = s.roads;
	switch (op.kind) {
		case OpKind::ADD_NODE:
			if (fwd) net.put_node(op.id, op.get<RoadNode>()); else net.remove_node_if_orphan(op.id);
			s.log.net_node(op.id);
			break;
		case OpKind::REM_NODE:
			if (fwd) net.remove_node_if_orphan(op.id); else net.put_node(op.id, op.get<RoadNode>());
			s.log.net_node(op.id);
			break;
		case OpKind::NODE_PLAIN: {
			const Change &c = op.get<Change>();
			net.set_plain(op.id, (fwd ? c.to : c.from) != 0);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_CROSSING: {
			const Change &c = op.get<Change>();
			net.nodes[op.id].crossing = uint8_t(fwd ? c.to : c.from);
			for (int seg : net.nodes[op.id].segs) net.mark_segment(seg);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_UTURN: {
			const Change &c = op.get<Change>();
			net.nodes[op.id].uturn = uint8_t(fwd ? c.to : c.from);
			for (int seg : net.nodes[op.id].segs) net.mark_segment(seg);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_LIFT: {
			const LiftChange &c = op.get<LiftChange>();
			net.set_lift(op.id, fwd ? c.to : c.from);
			s.log.net_node(op.id);
			break;
		}
		case OpKind::NODE_CTL: {
			const Change &c = op.get<Change>();
			net.nodes[op.id].ctl = uint8_t(fwd ? c.to : c.from);
			mark_control(s, op.id);
			break;
		}
		case OpKind::NODE_SIGNAL: {
			const SignalChange &c = op.get<SignalChange>();
			net.nodes[op.id].signal = fwd ? c.to : c.from;
			s.log.signal_nodes.push_back(op.id);
			break;
		}
		case OpKind::ADD_SEG:
			if (fwd) { net.put_segment(op.id, op.get<RoadSegment>()); mark_seg(s, op.id, false); }
			else { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			break;
		case OpKind::REM_SEG:
			if (fwd) { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			else { net.put_segment(op.id, op.get<RoadSegment>()); mark_seg(s, op.id, false); }
			break;
		case OpKind::MARKING: {
			const Change &c = op.get<Change>();
			s.marking_style.store(fwd ? c.to : c.from);
			for (int seg = 0; seg < int(net.segs.size()); seg++)
				if (net.segs[size_t(seg)].alive) net.mark_segment(seg);
			break;
		}
		case OpKind::PROFILE: {
			const ProfileChange &c = op.get<ProfileChange>();
			net.set_profile(op.id, fwd ? c.to : c.from);
			mark_seg(s, op.id, false);
			break;
		}
		case OpKind::ADD_BLD: if (fwd) s.buildings.put(op.id, op.get<Building>(), true); else s.buildings.remove(op.id); break;
		case OpKind::REM_BLD: if (fwd) s.buildings.remove(op.id); else s.buildings.put(op.id, op.get<Building>(), true); break;
		case OpKind::EDIT_BLD: {
			const BuildingEdit &e = op.get<BuildingEdit>();
			s.buildings.remove(op.id);
			s.buildings.put(op.id, fwd ? e.after : e.before, false);
			break;
		}
		case OpKind::ADD_MOD: if (fwd) s.terrain.put_modifier(op.id, op.get<TerrainModifier>()); else s.terrain.remove_modifier(op.id); break;
		case OpKind::MONEY: s.money += fwd ? op.get<double>() : -op.get<double>(); break;
		case OpKind::NODE_LINKS: {
			const LinksChange &c = op.get<LinksChange>();
			s.roads.nodes[size_t(op.id)].links = fwd ? c.after : c.before;
			s.log.net_node(op.id);
			break;
		}
		case OpKind::ADD_LOT: case OpKind::REM_LOT: case OpKind::EDIT_LOT: exec_lot(s, op, fwd); break;
		case OpKind::ADD_STOP: case OpKind::REM_STOP: case OpKind::EDIT_STOP: case OpKind::ADD_TRACK: case OpKind::REM_TRACK: case OpKind::EDIT_TRACK:
		case OpKind::ADD_LINE: case OpKind::REM_LINE: case OpKind::EDIT_LINE: case OpKind::TRANSIT_NEXT: exec_transit(s, op, fwd); break;
		case OpKind::DMG_CRATER:
		case OpKind::DMG_RUIN:
		case OpKind::DMG_GONE:
		case OpKind::DMG_HURT: apply_damage_op(s, op, fwd); break;
		case OpKind::NATURE: (*op.get<NatureChange>().edit)(s, fwd); break;
		case OpKind::ADD_ITEM: case OpKind::REM_ITEM: case OpKind::ADD_POLE: case OpKind::REM_POLE: case OpKind::ADD_SPAN:
		case OpKind::REM_SPAN: case OpKind::ADD_BARRIER: case OpKind::REM_BARRIER: case OpKind::ADD_CUT: case OpKind::REM_CUT:
		case OpKind::SEG_RULE: exec_street(s, op, fwd); break;
	}
}

} // namespace

// Whether the ops since `from` laid a road or widened one.
static bool moved_paving(const EditEntry &e, size_t from) {
	return std::any_of(e.ops.begin() + std::ptrdiff_t(from), e.ops.end(), [](const EditOp &op) { return op.kind == OpKind::ADD_SEG || op.kind == OpKind::PROFILE; });
}

void apply_entry(CityState &s, const EditEntry &e, bool forward) {
	if (forward) {
		for (const EditOp &op : e.ops) exec(s, op, true);
	} else {
		for (size_t i = e.ops.size(); i-- > 0;) exec(s, e.ops[i], false);
	}
	if (!e.terrain_idx.empty()) {
		s.terrain.write_samples(e.terrain_idx, forward ? e.terrain_after : e.terrain_before);
		if (!forward)
			for (int tile : e.clean_tiles) s.terrain.tile_dirty[size_t(tile)] = 0;
		mark_ground(s, e.minx, e.minz, e.maxx, e.maxz);
	}
	for (const PaintEdit &p : e.paint) {
		s.nature.ground.put_tile(p.key, (forward ? p.after : p.before).get());
		int r[4];
		s.nature.ground.tile_rect(p.key, r);
		mark_paint(s, r);
	}
	finish_command(s, true); // an entry applied is a change: settled, derived, the revisions moved
}

void EditHistory::reset(const Tuning &t) {
	clear();
	max_steps = size_t(t.history_max_entries);
	max_bytes = size_t(t.history_max_mb) * BYTES_PER_MB;
}

void EditHistory::clear() {
	undo_stack.clear();
	redo_stack.clear();
	open = EditEntry();
	recording = group = stroke_has_h0 = touched_ = false;
	begin_ops = 0;
}

void EditHistory::begin(const std::string &label) {
	recording = true;
	touched_ = false;
	if (!group) {
		open = EditEntry();
		open.label = label;
	}
	begin_ops = open.ops.size();
}

void EditHistory::end(CityState &s, bool ok) {
	if (ok && open.has_box && !s.lot_book.empty() && moved_paving(open, begin_ops)) {
		settle_network(s); // the lots are cut by the paving as it now stands (its junctions' shapes)
		cut_lots_by_roads(s, open.minx - SETTLE_PAD_M, open.minz - SETTLE_PAD_M, open.maxx + SETTLE_PAD_M, open.maxz + SETTLE_PAD_M);
	}
	recording = false;
	if (!ok && open.ops.size() > begin_ops) {
		EditEntry tail;
		tail.ops.assign(std::make_move_iterator(open.ops.begin() + std::ptrdiff_t(begin_ops)), std::make_move_iterator(open.ops.end()));
		tail.has_box = open.has_box;
		tail.minx = open.minx; tail.minz = open.minz; tail.maxx = open.maxx; tail.maxz = open.maxz;
		open.ops.resize(begin_ops);
		apply_entry(s, tail, false); // the rollback finishes the transaction
	} else {
		finish_command(s, ok && touched_);
	}
	if (!group) push(s);
}

void EditHistory::begin_group(const std::string &label) {
	if (group) return;
	group = true;
	stroke_has_h0 = false;
	open = EditEntry();
	open.label = label;
}

void EditHistory::end_group(CityState &s) {
	if (!group) return;
	group = false;
	push(s);
}

void EditHistory::close_terrain(const CityState &s) {
	const Heightfield &t = s.terrain;
	const int rs = t.region_size;
	for (auto it = open.terrain_open.begin(); it != open.terrain_open.end(); it = open.terrain_open.erase(it)) { // each copy freed once read
		const int tile = it->first, x0 = (tile % t.regions) * rs, z0 = (tile / t.regions) * rs;
		const std::vector<uint16_t> &before = it->second.before;
		for (int z = 0; z < rs; z++)
			for (int x = 0; x < rs; x++) {
				const uint32_t i = uint32_t(z0 + z) * uint32_t(t.N) + uint32_t(x0 + x);
				const uint16_t was = before[size_t(z) * size_t(rs) + size_t(x)], now = t.base_q[i];
				if (now == was) continue;
				open.terrain_idx.push_back(i);
				open.terrain_before.push_back(was);
				open.terrain_after.push_back(now);
			}
		if (it->second.clean) open.clean_tiles.push_back(tile);
	}
}

void EditHistory::close_paint(const CityState &s) {
	for (const auto &kv : open.paint_open) {
		const PaintTile *now = s.nature.ground.tile(kv.first);
		PaintEdit e;
		e.key = kv.first;
		e.before = kv.second;
		if (now) e.after = std::make_shared<const PaintTile>(*now);
		open.paint.push_back(std::move(e));
	}
	open.paint_open.clear();
}

void EditEntry::measure() {
	bytes = sizeof(EditEntry) + label.capacity() + ops.capacity() * sizeof(EditOp);
	for (const EditOp &op : ops) bytes += op_bytes(op) - sizeof(EditOp);
	for (const auto &kv : terrain_open) bytes += sizeof(kv) + kv.second.before.capacity() * sizeof(uint16_t);
	bytes += terrain_idx.capacity() * sizeof(uint32_t) + (terrain_before.capacity() + terrain_after.capacity()) * sizeof(uint16_t);
	bytes += clean_tiles.capacity() * sizeof(int) + paint.capacity() * sizeof(PaintEdit);
	for (const PaintEdit &p : paint) bytes += sizeof(PaintTile) * ((p.before ? 1 : 0) + (p.after ? 1 : 0));
	for (const auto &kv : paint_open) bytes += sizeof(kv) + (kv.second ? sizeof(PaintTile) : 0);
}

void EditHistory::push(CityState &s) {
	close_terrain(s);
	close_paint(s);
	open.reduce();
	open.measure();
	if (open.empty()) {
		open = EditEntry();
		return;
	}
	undo_stack.push_back(std::move(open));
	open = EditEntry();
	redo_stack.clear();
	trim();
	s.edited = true; // a recorded edit is the player's work
}

void EditHistory::hold(size_t bytes) {
	open.bytes += bytes;
	trim();
}

// The oldest steps go while the history holds more than its limits allow, the open entry (a stroke held down, a command
// under way) counted with the rest. The newest step stays: the open one, or the one just closed when none is open.
void EditHistory::trim() {
	const size_t open_steps = open.empty() ? 0 : 1, keep = 1 - open_steps;
	size_t total = open.bytes;
	for (const EditEntry &e : undo_stack) total += e.bytes;
	for (const EditEntry &e : redo_stack) total += e.bytes;
	size_t drop = 0;
	while (undo_stack.size() - drop > keep && (undo_stack.size() - drop + redo_stack.size() + open_steps > max_steps || total > max_bytes))
		total -= undo_stack[drop++].bytes;
	undo_stack.erase(undo_stack.begin(), undo_stack.begin() + std::ptrdiff_t(drop));
}

bool EditHistory::undo(CityState &s) {
	end_group(s);
	if (undo_stack.empty()) return false;
	EditEntry e = std::move(undo_stack.back());
	undo_stack.pop_back();
	apply_entry(s, e, false);
	redo_stack.push_back(std::move(e));
	s.edited = true;
	return true;
}

bool EditHistory::redo(CityState &s) {
	end_group(s);
	if (redo_stack.empty()) return false;
	EditEntry e = std::move(redo_stack.back());
	redo_stack.pop_back();
	apply_entry(s, e, true);
	undo_stack.push_back(std::move(e));
	s.edited = true;
	return true;
}

const std::string &EditHistory::undo_label() const {
	static const std::string none;
	return undo_stack.empty() ? none : undo_stack.back().label;
}

const std::string &EditHistory::redo_label() const {
	static const std::string none;
	return redo_stack.empty() ? none : redo_stack.back().label;
}

} // namespace city
