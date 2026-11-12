// Process-wide identities for replaceable sources. Unlike a table-local id, a generation never names a later source.
#pragma once

#include <atomic>
#include <cstdint>
#include <limits>
#include <cstdlib>

namespace city {
inline uint64_t next_generation() {
	static std::atomic<uint64_t> next{ 1 };
	uint64_t value = next.load(std::memory_order_relaxed);
	for (;;) {
		if (value == std::numeric_limits<uint64_t>::max()) std::abort(); // no source/epoch may wrap into an earlier identity
		if (next.compare_exchange_weak(value, value + 1, std::memory_order_relaxed)) return value;
	}
}
} // namespace city
