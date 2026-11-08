// The undo / redo stacks (Roadcraft core/commands.ts CommandStack: limit 500 steps / 512 MiB, groups for strokes,
// redo cleared by a new step); the inverse of each recorded op and an entry applied are edit_apply.cpp.
#include "sim/commands/edit_journal.h"
#include "sim/commands/lot_commands.h"

#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr size_t BYTES_PER_MB = size_t(1) << 20; // a mebibyte
constexpr double SETTLE_PAD_M = 4; // the box an undo settles reaches one height sample past what it touched
} // namespace

// Whether the ops since `from` laid a road or widened one.
static bool moved_paving(const EditEntry &e, size_t from) {
	return std::any_of(e.ops.begin() + std::ptrdiff_t(from), e.ops.end(), [](const EditOp &op) { return op.kind == OpKind::ADD_SEG || op.kind == OpKind::PROFILE; });
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
