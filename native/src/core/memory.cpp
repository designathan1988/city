// new and delete of the whole core on mimalloc: every job, the simulation and the render thread have their own heap, where the C
// runtime's allocator has one lock for them all. Only new and delete: what Godot allocates stays in Godot, and nothing allocated
// here is freed by anybody outside this library (the C++ runtime is linked into it).
#include "core/memory.h"

#include <mimalloc-new-delete.h>
#include <mimalloc-stats.h>

namespace city {

AllocatorHold allocator_hold() {
	mi_stats_t stats;
	mi_stats_get(sizeof(stats), &stats); // what it takes of the system and its threads are counted for the process, not by thread
	return AllocatorHold{ double(stats.committed.current), stats.threads.current };
}

void release_freed_memory() { mi_collect(true); }

double heap_held() {
	double held = 0;
	mi_heap_visit_blocks(
			mi_heap_get_default(), false,
			[](const mi_heap_t *, const mi_heap_area_t *area, void *, size_t, void *sum) {
				*static_cast<double *>(sum) += double(area->committed);
				return true;
			},
			&held);
	return held;
}

} // namespace city
