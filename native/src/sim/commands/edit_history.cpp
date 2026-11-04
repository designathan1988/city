// The undo / redo stacks (Roadcraft core/commands.ts CommandStack: limit 500 steps / 512 MiB, groups for strokes,
// redo cleared by a new step) and the inverse of each recorded op (journal.ts exec).
#include "sim/commands/edit_journal.h"

#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <map>

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

void exec(CityState &s, const EditOp &op, bool fwd) {
	RoadNetwork &net = s.roads;
	switch (op.kind) {
		case OpKind::ADD_NODE:
			if (fwd) net.put_node(op.id, op.node); else net.remove_node_if_orphan(op.id);
			s.log.net_node(op.id);
			break;
		case OpKind::REM_NODE:
			if (fwd) net.remove_node_if_orphan(op.id); else net.put_node(op.id, op.node);
			s.log.net_node(op.id);
			break;
		case OpKind::NODE_PLAIN:
			net.nodes[op.id].plain = (fwd ? op.to : op.from) != 0;
			s.log.net_node(op.id);
			break;
		case OpKind::ADD_SEG:
			if (fwd) { net.put_segment(op.id, op.seg); mark_seg(s, op.id, false); }
			else { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			break;
		case OpKind::REM_SEG:
			if (fwd) { mark_seg(s, op.id, true); net.remove_segment(op.id); }
			else { net.put_segment(op.id, op.seg); mark_seg(s, op.id, false); }
			break;
		case OpKind::PROFILE:
			net.set_profile(op.id, fwd ? op.to : op.from);
			mark_seg(s, op.id, false);
			break;
		case OpKind::ZONE:
			net.set_zone(op.id, op.side, fwd ? op.to : op.from);
			mark_furniture(s, op.id);
			break;
		case OpKind::ADD_BLD: if (fwd) s.buildings.put(op.id, op.bld, true); else s.buildings.remove(op.id); break;
		case OpKind::REM_BLD: if (fwd) s.buildings.remove(op.id); else s.buildings.put(op.id, op.bld, true); break;
		case OpKind::EDIT_BLD:
			s.buildings.remove(op.id);
			s.buildings.put(op.id, fwd ? op.bld_after : op.bld, false);
			break;
		case OpKind::ADD_MOD: if (fwd) s.terrain.put_modifier(op.id, op.mod); else s.terrain.remove_modifier(op.id); break;
		case OpKind::MONEY: s.money += fwd ? op.money : -op.money; break;
		default: exec_street(s, op, fwd); break;
	}
}

} // namespace

void apply_entry(CityState &s, const EditEntry &e, bool forward) {
	if (forward) {
		for (const EditOp &op : e.ops) exec(s, op, true);
	} else {
		for (size_t i = e.ops.size(); i-- > 0;) exec(s, e.ops[i], false);
	}
	if (!e.terrain_idx.empty()) {
		s.terrain.write_samples(e.terrain_idx, forward ? e.terrain_after : e.terrain_before);
		mark_ground(s, e.minx, e.minz, e.maxx, e.maxz);
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
	undo_stack.clear();
	redo_stack.clear();
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
	recording = false;
	if (!ok && open.ops.size() > begin_ops) {
		EditEntry tail;
		tail.ops.assign(open.ops.begin() + long(begin_ops), open.ops.end());
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

void EditHistory::end_group(CityState &s) {
	if (!group) return;
	group = false;
	push(s);
}

void EditHistory::close_terrain(const CityState &s) {
	if (open.terrain_open.empty()) return;
	std::map<uint32_t, uint16_t> sorted(open.terrain_open.begin(), open.terrain_open.end());
	for (const auto &kv : sorted) {
		uint16_t now = s.terrain.base_q[kv.first];
		if (now == kv.second) continue;
		open.terrain_idx.push_back(kv.first);
		open.terrain_before.push_back(kv.second);
		open.terrain_after.push_back(now);
	}
	open.terrain_open.clear();
	open.bytes += open.terrain_idx.size() * (sizeof(uint32_t) + 2 * sizeof(uint16_t));
}

void EditHistory::push(CityState &s) {
	close_terrain(s);
	if (!open.empty()) {
		undo_stack.push_back(std::move(open));
		redo_stack.clear();
		trim();
		s.edited = true; // a recorded edit is the player's work
	}
	open = EditEntry();
}

void EditHistory::trim() {
	size_t total = 0;
	for (const EditEntry &e : undo_stack) total += e.bytes;
	for (const EditEntry &e : redo_stack) total += e.bytes;
	while (undo_stack.size() > 1 && (undo_stack.size() + redo_stack.size() > max_steps || total > max_bytes)) {
		total -= undo_stack.front().bytes;
		undo_stack.erase(undo_stack.begin());
	}
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
