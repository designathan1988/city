// The undo / redo stacks (Roadcraft core/commands.ts CommandStack: limit 500 steps / 512 MiB, groups for strokes,
// redo cleared by a new step) and the inverse of each recorded op (journal.ts exec).
#include "sim/commands/edit_journal.h"

#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/commands/edit_memory.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <iterator>

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

void exec_lots(CityState &s, const EditOp &op, bool fwd) {
	const LotsChange &change = op.get<LotsChange>();
	for (const auto &edit : change.lots.edits)
		for (const auto *record : { &edit.before, &edit.after }) {
			if (!*record || (*record)->corners.empty()) continue;
			double minx = BIG, minz = BIG, maxx = -BIG, maxz = -BIG;
			for (const LotPoint &p : (*record)->corners) {
				minx = std::min(minx, p.x); minz = std::min(minz, p.z);
				maxx = std::max(maxx, p.x); maxz = std::max(maxz, p.z);
			}
			for_chunks(minx, minz, maxx, maxz, [&](int64_t key) { s.log.lot_cells.push_back(key); });
		}
	change.apply(s.lot_book, fwd);
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
		case OpKind::NODE_PLAIN:
			net.nodes[op.id].plain = (fwd ? op.to : op.from) != 0;
			s.log.net_node(op.id);
			s.log.shape_nodes.push_back(op.id);
			break;
		case OpKind::NODE_CROSSING:
			net.nodes[op.id].crossing = uint8_t(fwd ? op.to : op.from);
			for (int seg : net.nodes[op.id].segs) net.mark_segment(seg);
			s.log.net_node(op.id);
			break;
		case OpKind::NODE_UTURN:
			net.nodes[op.id].uturn = uint8_t(fwd ? op.to : op.from);
			for (int seg : net.nodes[op.id].segs) net.mark_segment(seg);
			s.log.net_node(op.id);
			break;
		case OpKind::NODE_LIFT:
			net.set_lift(op.id, fwd ? op.lift_to : op.lift_from);
			s.log.net_node(op.id);
			break;
		case OpKind::NODE_CTL:
			net.nodes[op.id].ctl = uint8_t(fwd ? op.to : op.from);
			s.log.net_node(op.id);
			s.log.street_nodes.push_back(op.id);
			break;
		case OpKind::NODE_SIGNAL:
			net.nodes[op.id].signal = fwd ? op.get<SignalChange>().after : op.get<SignalChange>().before;
			s.log.signal_nodes.push_back(op.id);
			break;
		case OpKind::ADD_SEG:
			if (fwd) { net.put_segment(op.id, op.get<RoadSegment>()); mark_seg(s, op.id, false); }
			else { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			break;
		case OpKind::REM_SEG:
			if (fwd) { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			else { net.put_segment(op.id, op.get<RoadSegment>()); mark_seg(s, op.id, false); }
			break;
		case OpKind::MARKING:
			s.marking_style.store(fwd ? op.to : op.from);
			for (int seg = 0; seg < int(net.segs.size()); seg++)
				if (net.segs[size_t(seg)].alive) net.mark_segment(seg);
			break;
		case OpKind::PROFILE:
			net.set_profile(op.id, fwd ? op.to : op.from);
			mark_seg(s, op.id, false);
			break;
		case OpKind::ZONE:
			net.set_zone(op.id, op.side, fwd ? op.to : op.from);
			net.set_player_zone(op.id, op.side, fwd ? op.to != 0 : op.player);
			mark_furniture(s, op.id);
			break;
		case OpKind::ADD_BLD: if (fwd) s.buildings.put(op.id, op.get<BuildingChange>().before, true); else s.buildings.remove(op.id); break;
		case OpKind::REM_BLD: if (fwd) s.buildings.remove(op.id); else s.buildings.put(op.id, op.get<BuildingChange>().before, true); break;
		case OpKind::EDIT_BLD:
			s.buildings.remove(op.id);
			s.buildings.put(op.id, fwd ? op.get<BuildingChange>().after : op.get<BuildingChange>().before, false);
			break;
		case OpKind::ADD_MOD: if (fwd) s.terrain.put_modifier(op.id, op.get<TerrainModifier>()); else s.terrain.remove_modifier(op.id); break;
		case OpKind::MONEY: s.money += fwd ? op.money : -op.money; break;
		case OpKind::TRANSIT:
			op.get<TransitChange>().apply(s.transit, fwd);
			s.transit_revision++;
			break;
		case OpKind::NODE_LINKS:
			s.roads.nodes[size_t(op.id)].links = fwd ? op.get<LinksChange>().after : op.get<LinksChange>().before;
			s.log.net_node(op.id);
			break;
		case OpKind::LOTS: exec_lots(s, op, fwd); break;
		case OpKind::DMG_CRATER:
		case OpKind::DMG_RUIN:
		case OpKind::DMG_GONE:
		case OpKind::DMG_HURT: apply_damage_op(s, op, fwd); break;
		case OpKind::NATURE: apply_nature_edit(s, op.get<NatureChange>().edit, fwd); break;
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
	if (e.has_box) finish_edit(s, e.minx - SETTLE_PAD_M, e.minz - SETTLE_PAD_M, e.maxx + SETTLE_PAD_M, e.maxz + SETTLE_PAD_M);
	else s.touch();
	s.edit_revision++;
}

void EditHistory::reset(const Tuning &t) {
	clear();
	max_steps = size_t(t.history_max_entries);
	max_bytes = size_t(t.history_max_mb) * BYTES_PER_MB;
}

void EditHistory::clear() {
	revision = next_generation();
	decltype(undo_stack)().swap(undo_stack);
	decltype(redo_stack)().swap(redo_stack);
	open = EditEntry();
	recording = group = stroke_has_h0 = false;
	begin_ops = 0;
}

void EditHistory::begin(const std::string &label) {
	recording = true;
	if (!group) {
		open = EditEntry();
		open.label = label;
	}
	begin_ops = open.ops.size();
}

void EditHistory::end(CityState &s, bool ok) {
	if (ok && open.has_box && !s.lot_book.records().empty() && moved_paving(open, begin_ops))
		cut_lots_by_roads(s, open.minx - SETTLE_PAD_M, open.minz - SETTLE_PAD_M, open.maxx + SETTLE_PAD_M, open.maxz + SETTLE_PAD_M);
	recording = false;
	if (!ok && open.ops.size() > begin_ops) {
		EditEntry tail;
		tail.ops.assign(std::make_move_iterator(open.ops.begin() + std::ptrdiff_t(begin_ops)), std::make_move_iterator(open.ops.end()));
		tail.has_box = open.has_box;
		tail.minx = open.minx; tail.minz = open.minz; tail.maxx = open.maxx; tail.maxz = open.maxz;
		open.ops.resize(begin_ops);
		apply_entry(s, tail, false);
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

bool EditHistory::end_group(CityState &s) {
	if (!group) return false;
	group = false;
	return push(s);
}

void EditHistory::close_terrain(const CityState &s) {
	if (open.terrain_open.empty()) return;
	// Sort just changed indices; no second node-per-sample tree coexists with the open hash and final arrays.
	for (const auto &kv : open.terrain_open)
		if (s.terrain.base_q[kv.first] != kv.second) open.terrain_idx.push_back(kv.first);
	std::sort(open.terrain_idx.begin(), open.terrain_idx.end());
	open.terrain_before.reserve(open.terrain_idx.size());
	open.terrain_after.reserve(open.terrain_idx.size());
	for (uint32_t index : open.terrain_idx) {
		open.terrain_before.push_back(open.terrain_open.at(index));
		open.terrain_after.push_back(s.terrain.base_q[index]);
		open.terrain_open.erase(index); // release each node as its compact representation becomes available
	}
	decltype(open.terrain_open)().swap(open.terrain_open); // clear alone would retain buckets
}

void EditHistory::close_paint(const CityState &s) {
	for (const auto &kv : open.paint_open) {
		const PaintTile *now = s.nature.ground.tile(kv.first);
		if ((!now && !kv.second) || (now && kv.second && *now == *kv.second)) continue;
		PaintEdit e;
		e.key = kv.first;
		e.before = kv.second;
		if (now) e.after = std::make_shared<const PaintTile>(*now);
		open.paint.push_back(std::move(e));
	}
	decltype(open.paint_open)().swap(open.paint_open);
}

bool EditHistory::push(CityState &s) {
	close_terrain(s);
	close_paint(s);
	open.reduce();
	const bool kept = !open.empty();
	if (kept) {
		undo_stack.push_back(std::move(open));
		redo_stack.clear();
		trim();
		revision = next_generation();
		s.edited = true; // a recorded edit is the player's work
	}
	open = EditEntry();
	return kept;
}

static RetainedMemory account_history(const EditHistory &h) {
	RetainedMemory memory;
	memory.storage(sizeof(h));
	memory.vector(h.undo_stack); memory.vector(h.redo_stack);
	account_entry(memory, h.open);
	for (const EditEntry &e : h.undo_stack) account_entry(memory, e);
	for (const EditEntry &e : h.redo_stack) account_entry(memory, e);
	return memory;
}

size_t EditHistory::retained_bytes() const { return account_history(*this).bytes(); }

void EditHistory::trim() {
	RetainedMemory memory = account_history(*this);
	memory.removing(true);
	// Remove in one prefix, without shifting the entire stack after every retired entry. Shared roots leave the
	// budget only when the final history reference leaves; the stacks' retained capacity is charged throughout.
	size_t retired = 0;
	while (undo_stack.size() - retired > 1 && (undo_stack.size() - retired + redo_stack.size() > max_steps || memory.bytes() > max_bytes)) {
		account_entry(memory, undo_stack[retired]);
		retired++;
	}
	undo_stack.erase(undo_stack.begin(), undo_stack.begin() + std::ptrdiff_t(retired));
}

bool EditHistory::undo(CityState &s) {
	end_group(s);
	if (undo_stack.empty()) return false;
	EditEntry e = std::move(undo_stack.back());
	undo_stack.pop_back();
	apply_entry(s, e, false);
	redo_stack.push_back(std::move(e));
	revision = next_generation();
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
	revision = next_generation();
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
