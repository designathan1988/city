// A bounded cache of shared values made from their keys, safe from any thread. A value is made once outside the lock (two threads
// asking for the same key at once may both make it: the first kept is the one both get, the same key making the same value) and
// kept until the others asked since push it out by their cost, the least recently asked first. Whoever holds a value keeps it alive
// past its eviction. A budget of 0 keeps nothing: every value is made for its asker alone.
#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace city {

template <typename K, typename T, typename H>
class LruCache {
public:
	using Ptr = std::shared_ptr<T>;

	// The most the kept values may cost together; a smaller budget pushes the oldest out now.
	void set_budget(size_t budget) {
		std::lock_guard<std::mutex> lk(mutex_);
		budget_ = budget;
		trim();
	}

	// The value of `key`: the kept one, or the one `make()` returns (a Ptr), kept at the cost `cost(value)`.
	template <typename Make, typename Cost>
	Ptr get(const K &key, Make &&make, Cost &&cost) {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			if (Ptr kept = touch(key)) return kept;
		}
		Ptr made = make();
		const size_t c = cost(*made);
		std::lock_guard<std::mutex> lk(mutex_);
		if (Ptr kept = touch(key)) return kept; // another thread kept it while this one made it
		order_.push_front(Entry{ key, made, c });
		index_.emplace(key, order_.begin());
		used_ += c;
		trim();
		return made;
	}

	// The kept value of `key` (asked now: it moves to the front), or none: nothing is made.
	Ptr find(const K &key) {
		std::lock_guard<std::mutex> lk(mutex_);
		return touch(key);
	}

	void clear() {
		std::lock_guard<std::mutex> lk(mutex_);
		index_.clear();
		order_.clear();
		used_ = 0;
	}

	size_t used() const {
		std::lock_guard<std::mutex> lk(mutex_);
		return used_;
	}

private:
	struct Entry {
		K key;
		Ptr value;
		size_t cost = 0;
	};

	// The kept value of `key`, moved to the front, or none. Under the lock.
	Ptr touch(const K &key) {
		auto it = index_.find(key);
		if (it == index_.end()) return nullptr;
		order_.splice(order_.begin(), order_, it->second);
		return it->second->value;
	}

	// The least recently asked pushed out until the rest fits the budget. Under the lock.
	void trim() {
		while (used_ > budget_ && !order_.empty()) {
			const Entry &last = order_.back();
			used_ -= last.cost;
			index_.erase(last.key);
			order_.pop_back();
		}
	}

	mutable std::mutex mutex_;
	std::list<Entry> order_; // most recently asked first
	std::unordered_map<K, typename std::list<Entry>::iterator, H> index_;
	size_t budget_ = 0, used_ = 0;
};

} // namespace city
