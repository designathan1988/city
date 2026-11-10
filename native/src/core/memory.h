// The core's allocator as its threads use it (core/memory.cpp: new and delete on mimalloc, a heap a thread): a thread about to
// sleep gives back to the system what its work freed. A heap's segment is committed as it is used, never whole (CMakeLists.txt,
// MI_DEFAULT_EAGER_COMMIT): what the process holds follows what its threads hold.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace city {

// What the allocator holds of the system now: the bytes it has committed, and the threads that have a heap in it (a thread that
// ended has none: its heap went with it).
struct AllocatorHold {
	double committed = 0;
	int64_t threads = 0;
};
AllocatorHold allocator_hold();

// A place of the code that asked the allocator for blocks still alive: its address from the library's base (what a symbolizer
// takes), their bytes and how many they are.
struct HeapSite {
	uint64_t at = 0;
	double live = 0;
	int64_t blocks = 0;
};
// The `most` places that hold the most live bytes, heaviest first: only in a measuring build (CITY_HEAP_SITES, core/memory.cpp),
// empty in the game's.
std::vector<HeapSite> heap_sites(size_t most);

// What the calling thread's heap holds, in bytes. `live`: the blocks handed out and not yet taken back by this heap (a block another
// thread freed counts until this heap collects it); `held`: the pages they stand in, committed; `by_size`: the live bytes of each
// block size. A walk of the heap's pages: for the memory census, never per frame.
struct HeapUse {
	double live = 0, held = 0;
	std::map<size_t, double> by_size;
};
HeapUse heap_use();

// How long freed memory waits before it may be given back (the allocator's own purge delay), in milliseconds: a thread idle for
// longer than this has nothing more to gain from keeping it.
long freed_memory_wait_ms();
// Gives the calling thread's freed memory back to the system. The allocator does this only when the thread allocates or frees
// again (its purge runs on the heap's own activity, never from another thread), so a worker that built meshes and went to sleep
// kept every page its jobs had used: the blocks are freed by the frame's thread, after the upload.
void release_freed_memory();

} // namespace city
