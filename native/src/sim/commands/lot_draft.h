// A lot command's private draft: copy only selected records, then prepare ordered deltas without cloning the whole book.
#pragma once

#include "sim/lots/lot_book.h"

#include <map>
#include <set>

namespace city {

class LotDraft {
public:
	explicit LotDraft(const LotBook &source) : next_id(source.next_id), source_(source) {}
	int next_id;
	PlayerLot *find(int id) {
		const PlayerLot *original = source_.find(id);
		if (!original) return nullptr;
		const size_t at = size_t(original - source_.records().data());
		if (removed_.count(at)) return nullptr;
		return &edited_.try_emplace(at, *original).first->second;
	}
	void remove(int id) {
		const PlayerLot *original = source_.find(id);
		if (!original) return;
		const size_t at = size_t(original - source_.records().data());
		removed_.insert(at);
		edited_.erase(at);
	}
	void add(PlayerLot lot) { added_.push_back(std::move(lot)); }
	LotsChange take() {
		LotsChange change(source_.next_id);
		change.next_after = next_id;
		for (auto &entry : edited_) {
			const PlayerLot &before = source_.records()[entry.first];
			if (!(before == entry.second)) change.lots.replace(entry.first, before, std::move(entry.second));
		}
		// Original indices remain valid while removing from the back. Append positions follow the surviving records.
		for (auto it = removed_.rbegin(); it != removed_.rend(); ++it) change.lots.remove(*it, source_.records()[*it]);
		size_t at = source_.records().size() - removed_.size();
		for (PlayerLot &lot : added_) change.lots.add(at++, std::move(lot));
		return change;
	}

private:
	const LotBook &source_;
	std::map<size_t, PlayerLot> edited_;
	std::set<size_t> removed_;
	std::vector<PlayerLot> added_;
};

} // namespace city
