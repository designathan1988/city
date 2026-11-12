// Shared placed street furniture, before consumer-specific kind/damage projections. Full precedence resolves once.
#pragma once

#include "sim/roads/street_furniture.h"

#include <memory>
#include <list>
#include <mutex>
#include <map>

namespace city {

struct CityState;

class StreetPlacement {
public:
	std::shared_ptr<const std::vector<FurnitureSpot>> get(const CityState &s, int seg, const bool signalled[2]) const;
	void invalidate(int seg);
	void clear();

private:
	struct Entry {
		int network = 0;
		uint8_t signals = 0;
		std::shared_ptr<const std::vector<FurnitureSpot>> spots;
		size_t bytes = 0;
		std::list<int>::iterator age;
	};
	using Entries = std::map<int, Entry>; // no bucket high-water retained after evicting entries
	using Roots = std::vector<std::shared_ptr<const std::vector<FurnitureSpot>>>;
	void retire(Entries::iterator entry, Roots &retired) const;
	mutable std::mutex mutex_;
	mutable Entries entries_;
	mutable std::list<int> recent_;
	mutable size_t bytes_ = 0;
};

} // namespace city
