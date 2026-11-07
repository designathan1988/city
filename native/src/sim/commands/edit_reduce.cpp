// An edit reduced to its net effect when it closes: neighbouring operations that are exact inverses are the identity, so both go
// (Qt's QUndoCommand::isObsolete: a command with no effect leaves the stack). A piece taken away and put back the same, a price
// paid and refunded: whatever command wrote them, now or later, an edit that comes to nothing is no step of undo, and one that
// comes to something keeps only that. Only neighbours cancel: anything between two operations may depend on the first.
// What the entry holds is measured again after (EditEntry::measure), so its bytes are those of what it kept.
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

template <typename P>
bool same_payload(const EditOp &a, const EditOp &b) { return same(a.get<P>(), b.get<P>()); }

bool inverse(const EditOp &a, const EditOp &b) {
	if (a.kind == OpKind::MONEY && b.kind == OpKind::MONEY) return a.get<double>() + b.get<double>() == 0;
	if (a.id != b.id) return false;
	if (a.kind == OpKind::PROFILE && b.kind == OpKind::PROFILE) {
		const ProfileChange &p = a.get<ProfileChange>(), &q = b.get<ProfileChange>();
		return p.from == q.to && p.to == q.from;
	}
	if (a.kind == OpKind::ZONE && b.kind == OpKind::ZONE) {
		const Change &p = a.get<Change>(), &q = b.get<Change>();
		return p.side == q.side && p.from == q.to && p.to == q.from && p.player == q.player;
	}
	if (either(a.kind, b.kind, OpKind::ADD_ITEM, OpKind::REM_ITEM)) return same_payload<StreetItem>(a, b);
	if (either(a.kind, b.kind, OpKind::ADD_POLE, OpKind::REM_POLE)) return same_payload<UtilityPole>(a, b);
	if (either(a.kind, b.kind, OpKind::ADD_SPAN, OpKind::REM_SPAN)) return same_payload<UtilitySpan>(a, b);
	if (either(a.kind, b.kind, OpKind::ADD_BARRIER, OpKind::REM_BARRIER)) return same_payload<Barrier>(a, b);
	if (either(a.kind, b.kind, OpKind::ADD_CUT, OpKind::REM_CUT)) return same_payload<StreetCut>(a, b);
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
	kept.shrink_to_fit(); // what it holds is what it keeps
	ops = std::move(kept);
}

} // namespace city
