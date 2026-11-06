// Journalled damage (see journal_ops.cpp): a crater dug, a ruin left or cleared, street furniture knocked out. Each
// appends to DamageState and, while a command records, the op its inverse needs. The undo is last-in first-out, so a
// crater or a piece of furniture comes off the end, and a ruin goes back where it stood in the list.
#include "sim/commands/edit_journal.h"

#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

namespace {
EditOp &record(CityState &s, OpKind k) { return j_record(s, k, -1); }

// The lots a ruin sits on are drawn and grown again: its building chunk is marked.
void mark_ruin(CityState &s, const Ruin &r) {
	Building b = r.record;
	{
		const float reach = std::max(b.w, b.d) * 0.5f;
		for (int j = int(std::floor((b.z - reach) / CHUNK)); j <= int(std::floor((b.z + reach) / CHUNK)); j++)
			for (int i = int(std::floor((b.x - reach) / CHUNK)); i <= int(std::floor((b.x + reach) / CHUNK)); i++)
				s.log.building_chunks.push_back(cell_key(i, j));
	}
}
} // namespace

void j_add_crater(CityState &s, const Crater &c) {
	s.damage.craters.push_back(c);
	if (s.history.recording) record(s, OpKind::DMG_CRATER).crater = c;
}

void j_add_ruin(CityState &s, const Ruin &r) {
	s.damage.ruins.push_back(r);
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::DMG_RUIN);
		op.ruin = r;
		op.side = 0;
		op.id = int(s.damage.ruins.size()) - 1;
	}
	mark_ruin(s, r);
}

void j_remove_ruin(CityState &s, int index) {
	if (index < 0 || index >= int(s.damage.ruins.size())) return;
	const Ruin r = s.damage.ruins[size_t(index)];
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::DMG_RUIN);
		op.ruin = r;
		op.side = 1;
		op.id = index;
	}
	s.damage.ruins.erase(s.damage.ruins.begin() + index);
	mark_ruin(s, r);
}

void j_add_gone(CityState &s, const Gone &g) {
	s.damage.gone.push_back(g);
	if (s.history.recording) record(s, OpKind::DMG_GONE).gone = g;
}

namespace {
void put_hurt(DamageState &d, const Hurt &h, float amount) {
	const int i = d.hurt_of(h.seed, h.x, h.z);
	if (amount <= 0) {
		if (i >= 0) d.hurt.erase(d.hurt.begin() + i);
		return;
	}
	Hurt next = h;
	next.amount = amount;
	if (i >= 0) d.hurt[size_t(i)] = next;
	else d.hurt.push_back(next);
}
} // namespace

void j_set_hurt(CityState &s, uint32_t seed, float x, float z, float amount) {
	const int i = s.damage.hurt_of(seed, x, z);
	const float before = i >= 0 ? s.damage.hurt[size_t(i)].amount : 0.f;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::DMG_HURT);
		op.hurt = { seed, x, z, amount };
		op.hurt_before = before;
	}
	put_hurt(s.damage, { seed, x, z, amount }, amount);
}

void j_add_scar(CityState &s, const Scar &c) {
	s.damage.scars.push_back(c);
	j_nature(s, sizeof(c), [c](CityState &st, bool fwd) {
		if (fwd) st.damage.scars.push_back(c);
		else if (!st.damage.scars.empty()) st.damage.scars.pop_back();
	});
}

void apply_damage_op(CityState &s, const EditOp &op, bool fwd) {
	DamageState &d = s.damage;
	switch (op.kind) {
		case OpKind::DMG_CRATER:
			if (fwd) d.craters.push_back(op.crater);
			else if (!d.craters.empty()) d.craters.pop_back();
			break;
		case OpKind::DMG_GONE:
			if (fwd) d.gone.push_back(op.gone);
			else if (!d.gone.empty()) d.gone.pop_back();
			break;
		case OpKind::DMG_HURT: put_hurt(d, op.hurt, fwd ? op.hurt.amount : op.hurt_before); break;
		case OpKind::DMG_RUIN: {
			const bool add = (op.side == 0) == fwd;
			if (add) d.ruins.insert(d.ruins.begin() + std::min<size_t>(size_t(op.id), d.ruins.size()), op.ruin);
			else if (op.id >= 0 && op.id < int(d.ruins.size())) d.ruins.erase(d.ruins.begin() + op.id);
			mark_ruin(s, op.ruin);
			break;
		}
		default: break;
	}
}

} // namespace city
