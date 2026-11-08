// Journalled public transport plan (see journal_ops.cpp): j_transit replaces the plan and, while a command records, keeps
// what differs as the ops that turn each list (stops, tracks, lines) into the next, record by record (by id).
#include "sim/commands/edit_journal.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace city {

namespace {
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
		if (!stays[i]) j_record(s, rem, before[i].id, Listed<R>{ int(i), before[i] });
	int index = 0; // the kept, in order, after the removals
	for (size_t i = 0; i < before.size(); i++) {
		if (!stays[i]) continue;
		const R &now = after[size_t(after_of[i])];
		if (!same(before[i], now)) j_record(s, edit, now.id, Edited<R>{ index, before[i], now });
		index++;
	}
	for (size_t j = 0; j < after.size(); j++) {
		const auto it = at_before.find(after[j].id);
		if (it == at_before.end() || !stays[size_t(it->second)]) j_record(s, add, after[j].id, Listed<R>{ int(j), after[j] });
	}
}

} // namespace

void j_transit(CityState &s, TransitData next) {
	if (s.history.recording) {
		const TransitData &t = s.transit;
		record_list(s, t.stops, next.stops, OpKind::ADD_STOP, OpKind::REM_STOP, OpKind::EDIT_STOP);
		record_list(s, t.tracks, next.tracks, OpKind::ADD_TRACK, OpKind::REM_TRACK, OpKind::EDIT_TRACK);
		record_list(s, t.lines, next.lines, OpKind::ADD_LINE, OpKind::REM_LINE, OpKind::EDIT_LINE);
		if (t.next_id != next.next_id) j_record(s, OpKind::TRANSIT_NEXT, -1, Change{ 0, t.next_id, next.next_id });
	}
	s.transit = std::move(next);
	s.transit_revision++;
	s.touch();
}

} // namespace city
