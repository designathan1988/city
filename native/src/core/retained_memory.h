// Logical retained allocations: vector capacity, container storage and shared roots counted once by identity.
// Charges include named node/control metadata, not allocator arenas, fragmentation or a process RSS measurement.
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace city {

class RetainedMemory {
public:
	static constexpr size_t shared_control_bytes() { return SHARED_CONTROL_WORDS * sizeof(void *); }
	void removing(bool value) { removing_ = value; }
	size_t bytes() const { return bytes_; }
	void storage(size_t bytes) {
		if (removing_) { assert(bytes_ >= bytes); bytes_ -= bytes; }
		else bytes_ += bytes;
	}
	template <typename T> void vector(const std::vector<T> &v) { storage(v.capacity() * sizeof(T)); }
	template <typename K, typename V, typename H, typename E, typename A>
	void hash(const std::unordered_map<K, V, H, E, A> &m) {
		// Nodes have a link and may cache the hash; the bucket array retains capacity even after clear().
		storage(m.bucket_count() * sizeof(void *) + m.size() * (sizeof(typename std::decay_t<decltype(m)>::value_type) + HASH_NODE_WORDS * sizeof(void *)));
	}
	template <typename K, typename V, typename C, typename A>
	void tree(const std::map<K, V, C, A> &m) {
		storage(m.size() * (sizeof(typename std::decay_t<decltype(m)>::value_type) + TREE_NODE_WORDS * sizeof(void *)));
	}
	void string(const std::string &s) {
		const auto address = reinterpret_cast<uintptr_t>(s.data()), begin = reinterpret_cast<uintptr_t>(&s);
		if (address < begin || address - begin >= sizeof(s)) storage(s.capacity() + 1); // exclude inline storage
	}
	template <typename T, typename Footprint> void shared(const std::shared_ptr<const T> &p, Footprint footprint) {
		if (!p) return;
		if (removing_) {
			auto it = roots_.find(p.get());
			assert(it != roots_.end() && it->second.count);
			if (--it->second.count == 0) { storage(it->second.bytes); roots_.erase(it); }
		} else {
			auto [it, fresh] = roots_.try_emplace(p.get());
			if (fresh) { it->second.bytes = footprint(*p) + shared_control_bytes(); storage(it->second.bytes); }
			it->second.count++;
		}
	}

private:
	static constexpr size_t HASH_NODE_WORDS = 2, TREE_NODE_WORDS = 4, SHARED_CONTROL_WORDS = 4;
	struct Root { size_t count = 0, bytes = 0; };
	std::unordered_map<const void *, Root> roots_;
	size_t bytes_ = 0;
	bool removing_ = false;
};

} // namespace city
