// An edit reduced to its net effect when it closes: neighbouring operations that are exact inverses are the identity, so both go
// (Qt's QUndoCommand::isObsolete: a command with no effect leaves the stack). A piece taken away and put back the same, a price
// paid and refunded: whatever command wrote them, now or later, an edit that comes to nothing is no step of undo, and one that
// comes to something keeps only that. Only neighbours cancel: anything between two operations may depend on the first.
#include "sim/commands/edit_journal.h"

namespace city {

namespace {
bool same(const StreetItem &a, const StreetItem &b) {
	return a.kind == b.kind && a.sign == b.sign && a.x == b.x && a.z == b.z && a.text == b.text;
}
bool same(const UtilityPole &a, const UtilityPole &b) {
	return a.x == b.x && a.z == b.z && a.lamp == b.lamp && a.kind == b.kind && a.spans == b.spans;
}
bool same(const UtilitySpan &a, const UtilitySpan &b) { return a.a == b.a && a.b == b.b; }
bool same(const Barrier &a, const Barrier &b) { return a.kind == b.kind && a.pts == b.pts; }
bool same(const StreetCut &a, const StreetCut &b) { return a.x == b.x && a.z == b.z; }

bool either(OpKind a, OpKind b, OpKind add, OpKind rem) { return (a == add && b == rem) || (a == rem && b == add); }

bool inverse(const EditOp &a, const EditOp &b) {
	if (a.kind == OpKind::MONEY && b.kind == OpKind::MONEY) return a.money + b.money == 0;
	if (a.id != b.id) return false;
	if (a.kind == OpKind::PROFILE && b.kind == OpKind::PROFILE) return a.from == b.to && a.to == b.from;
	if (a.kind == OpKind::ZONE && b.kind == OpKind::ZONE) return a.side == b.side && a.from == b.to && a.to == b.from && a.player == b.player;
	if (either(a.kind, b.kind, OpKind::ADD_ITEM, OpKind::REM_ITEM)) return same(a.get<StreetItem>(), b.get<StreetItem>());
	if (either(a.kind, b.kind, OpKind::ADD_POLE, OpKind::REM_POLE)) return same(a.get<UtilityPole>(), b.get<UtilityPole>());
	if (either(a.kind, b.kind, OpKind::ADD_SPAN, OpKind::REM_SPAN)) return same(a.get<UtilitySpan>(), b.get<UtilitySpan>());
	if (either(a.kind, b.kind, OpKind::ADD_BARRIER, OpKind::REM_BARRIER)) return same(a.get<Barrier>(), b.get<Barrier>());
	if (either(a.kind, b.kind, OpKind::ADD_CUT, OpKind::REM_CUT)) return same(a.get<StreetCut>(), b.get<StreetCut>());
	return false;
}
} // namespace

void EditEntry::reduce() {
	size_t kept = 0;
	for (size_t i = 0; i < ops.size(); i++) {
		if (kept != 0 && inverse(ops[kept - 1], ops[i])) {
			ops[--kept] = EditOp();
			ops[i] = EditOp();
		} else {
			if (kept != i) ops[kept] = std::move(ops[i]);
			kept++;
		}
	}
	ops.resize(kept);
}

} // namespace city
