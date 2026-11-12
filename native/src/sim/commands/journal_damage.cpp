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
	s.damage.craters_revision = next_generation();
	if (s.history.recording) record(s, OpKind::DMG_CRATER).get<Crater>() = c;
}

void j_add_ruin(CityState &s, const Ruin &r) {
	const b40::BodyRef body = s.buildings.designs.edited->get(r.record.body);
	s.damage.insert_ruin(s.damage.ruins().size(), r, body);
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::DMG_RUIN);
		op.get<RuinChange>().record = r;
		op.get<RuinChange>().body = body;
		op.side = 0;
		op.id = int(s.damage.ruins().size()) - 1;
	}
	mark_ruin(s, r);
}

void j_remove_ruin(CityState &s, int index) {
	if (index < 0 || index >= int(s.damage.ruins().size())) return;
	const Ruin r = s.damage.ruins()[size_t(index)];
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::DMG_RUIN);
		op.get<RuinChange>().record = r;
		op.get<RuinChange>().body = s.damage.ruin_bodies()[size_t(index)];
		op.side = 1;
		op.id = index;
	}
	s.damage.remove_ruin(size_t(index));
	mark_ruin(s, r);
}

void j_add_gone(CityState &s, const Gone &g) {
	s.damage.add_gone(g);
	if (s.history.recording) record(s, OpKind::DMG_GONE).get<Gone>() = g;
}

void j_set_hurt(CityState &s, uint32_t seed, float x, float z, float amount) {
	const int i = s.damage.hurt_of(seed, x, z);
	const float before = i >= 0 ? s.damage.hurt()[size_t(i)].amount : 0.f;
	if (s.history.recording) {
		EditOp &op = record(s, OpKind::DMG_HURT);
		op.get<HurtChange>().after = { seed, x, z, amount };
		op.get<HurtChange>().before = before;
	}
	s.damage.set_hurt({ seed, x, z, amount }, amount);
}

void j_add_scar(CityState &s, const Scar &c) {
	s.damage.add_scar(c);
	j_nature(s, ScarAdded{ c });
}

void apply_damage_op(CityState &s, const EditOp &op, bool fwd) {
	DamageState &d = s.damage;
	switch (op.kind) {
		case OpKind::DMG_CRATER:
			if (fwd) d.craters.push_back(op.get<Crater>());
			else if (!d.craters.empty()) d.craters.pop_back();
			d.craters_revision = next_generation();
			break;
		case OpKind::DMG_GONE:
			if (fwd) d.add_gone(op.get<Gone>());
			else d.pop_gone();
			break;
		case OpKind::DMG_HURT: d.set_hurt(op.get<HurtChange>().after, fwd ? op.get<HurtChange>().after.amount : op.get<HurtChange>().before); break;
		case OpKind::DMG_RUIN: {
			const bool add = (op.side == 0) == fwd;
			if (add) {
				const size_t at = std::min<size_t>(size_t(op.id), d.ruins().size());
				d.insert_ruin(at, op.get<RuinChange>().record, op.get<RuinChange>().body);
			} else if (op.id >= 0 && op.id < int(d.ruins().size())) {
				d.remove_ruin(size_t(op.id));
			}
			mark_ruin(s, op.get<RuinChange>().record);
			break;
		}
		default: break;
	}
}

} // namespace city
