// Seeded greedy street walks and their local continuation inputs. Cached values never hold a RoadNetwork reference.
#pragma once

#include <cstdint>
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace city {
class RoadNetwork;
class StreetChains {
public:
	std::vector<int> get(const RoadNetwork &roads, int seed) const;
	void clear();
private:
	struct Continuation { int segment = -1, end = 0; double dot = 0; };
	struct Value {
		std::vector<int> chain;
		std::vector<Continuation> choices;
		std::vector<std::pair<int, uint64_t>> dependencies;
		float cosine = 0;
		size_t bytes = 0;
		bool current(const RoadNetwork &roads) const;
	};
	struct Entry { std::shared_ptr<const Value> value; std::list<uint64_t>::iterator age; };
	using Entries = std::map<uint64_t, Entry>;
	using Roots = std::vector<std::shared_ptr<const Value>>;
	std::shared_ptr<const Value> lookup(const RoadNetwork &roads, uint64_t key) const;
	std::shared_ptr<const Value> choices(const RoadNetwork &roads, int segment, int end) const;
	void retain(const RoadNetwork &roads, uint64_t key, std::shared_ptr<const Value> value) const;
	void retire(Entries::iterator entry, Roots &retired) const;
	static size_t footprint(const Value &value);
	mutable std::mutex mutex_;
	mutable Entries entries_;
	mutable std::list<uint64_t> recent_;
	mutable size_t bytes_ = 0;
};
} // namespace city
