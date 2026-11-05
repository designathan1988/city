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
	if (either(a.kind, b.kind, OpKind::ADD_ITEM, OpKind::REM_ITEM)) return same(a.item, b.item);
	if (either(a.kind, b.kind, OpKind::ADD_POLE, OpKind::REM_POLE)) return same(a.pole, b.pole);
	if (either(a.kind, b.kind, OpKind::ADD_SPAN, OpKind::REM_SPAN)) return same(a.span, b.span);
	if (either(a.kind, b.kind, OpKind::ADD_BARRIER, OpKind::REM_BARRIER)) return same(a.barrier, b.barrier);
	if (either(a.kind, b.kind, OpKind::ADD_CUT, OpKind::REM_CUT)) return same(a.cut, b.cut);
	return false;
}
} // namespace

void EditEntry::reduce() {
	std::vector<EditOp> kept;
	kept.reserve(ops.size());
	for (EditOp &op : ops) {
		if (!kept.empty() && inverse(kept.back(), op)) kept.pop_back();
		else kept.push_back(std::move(op));
	}
	ops = std::move(kept);
}

} // namespace city
