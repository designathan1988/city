// The core's allocator as its threads use it (core/memory.cpp: new and delete on mimalloc, a heap a thread): what it holds of the
// system, and a thread giving back what its work freed. What the process holds follows what its threads hold: memory freed is the
// system's again, and a thread that ends leaves no heap behind (native/CMakeLists.txt, MI_WIN_USE_FLS; tests/core/memory_test).
#pragma once

#include <cstdint>

namespace city {

// What the allocator holds of the system now: the bytes it has committed, and the threads that have a heap in it.
struct AllocatorHold {
	double committed = 0;
	int64_t threads = 0;
};
AllocatorHold allocator_hold();

// Gives the calling thread's freed memory back to the system now (the allocator does it by itself only as the thread allocates or
// frees again).
void release_freed_memory();

} // namespace city
