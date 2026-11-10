// new and delete of the whole core on mimalloc: every job, the simulation and the render thread have their own heap, where the C
// runtime's allocator has one lock for them all. Only new and delete: what Godot allocates stays in Godot, and nothing allocated
// here is freed by anybody outside this library (the C++ runtime is linked into it).
// A measuring build (CITY_HEAP_SITES, native/CMakeLists.txt) puts before every block the place that asked for it and its size, and
// counts the live bytes of each place: heap_sites names the owners of what the core holds. The game's builds carry none of it.
#include "core/memory.h"

#if CITY_HEAP_SITES
#include "core/random.h"

#include <mimalloc.h>

#include <algorithm>
#include <atomic>
#include <new>
#else
#include <mimalloc-new-delete.h>
#endif
#include <mimalloc-stats.h>

#if CITY_HEAP_SITES
extern "C" char __ImageBase; // the library's first byte in memory (the linker's own symbol): an asker is named from it

namespace {
constexpr size_t SITE_SLOTS = size_t(1) << 16; // the table's slots: the places that allocate are a few thousand
constexpr int SITE_SHIFT = 48; // the top 16 bits of the address's hash pick the slot
// What stands before a block: its size, its asker's slot and how far back its allocation begins (16 bytes: the block keeps new's alignment).
struct Head {
	uint64_t size;
	uint32_t site;
	uint32_t back;
};
struct Site {
	std::atomic<uintptr_t> at;
	std::atomic<int64_t> bytes, blocks;
};
Site sites[SITE_SLOTS]; // zero from the start: new runs before any constructor of this library

uint32_t site_of(uintptr_t at) {
	for (uint32_t i = uint32_t((at * city::GOLDEN_GAMMA_64) >> SITE_SHIFT);; i = (i + 1) & uint32_t(SITE_SLOTS - 1)) {
		uintptr_t have = sites[i].at.load(std::memory_order_relaxed);
		if (have == at) return i;
		if (have == 0 && (sites[i].at.compare_exchange_strong(have, at) || have == at)) return i;
	}
}

void *take(size_t n, size_t align, void *from, bool may_fail) {
	const size_t pad = std::max(align, sizeof(Head));
	char *base;
	if (align > sizeof(Head)) base = static_cast<char *>(may_fail ? mi_malloc_aligned(n + pad, align) : mi_new_aligned(n + pad, align));
	else base = static_cast<char *>(may_fail ? mi_malloc(n + pad) : mi_new(n + pad));
	if (!base) return nullptr;
	Head *h = reinterpret_cast<Head *>(base + pad - sizeof(Head));
	h->size = n;
	h->site = site_of(reinterpret_cast<uintptr_t>(from));
	h->back = uint32_t(pad);
	sites[h->site].bytes.fetch_add(int64_t(n), std::memory_order_relaxed);
	sites[h->site].blocks.fetch_add(1, std::memory_order_relaxed);
	return base + pad;
}

void give(void *p) {
	if (!p) return;
	const Head *h = reinterpret_cast<const Head *>(static_cast<char *>(p) - sizeof(Head));
	sites[h->site].bytes.fetch_sub(int64_t(h->size), std::memory_order_relaxed);
	sites[h->site].blocks.fetch_sub(1, std::memory_order_relaxed);
	mi_free(static_cast<char *>(p) - h->back);
}
} // namespace

void *operator new(size_t n) { return take(n, 0, __builtin_return_address(0), false); }
void *operator new[](size_t n) { return take(n, 0, __builtin_return_address(0), false); }
void *operator new(size_t n, const std::nothrow_t &) noexcept { return take(n, 0, __builtin_return_address(0), true); }
void *operator new[](size_t n, const std::nothrow_t &) noexcept { return take(n, 0, __builtin_return_address(0), true); }
void *operator new(size_t n, std::align_val_t al) { return take(n, size_t(al), __builtin_return_address(0), false); }
void *operator new[](size_t n, std::align_val_t al) { return take(n, size_t(al), __builtin_return_address(0), false); }
void *operator new(size_t n, std::align_val_t al, const std::nothrow_t &) noexcept { return take(n, size_t(al), __builtin_return_address(0), true); }
void *operator new[](size_t n, std::align_val_t al, const std::nothrow_t &) noexcept { return take(n, size_t(al), __builtin_return_address(0), true); }
void operator delete(void *p) noexcept { give(p); }
void operator delete[](void *p) noexcept { give(p); }
void operator delete(void *p, size_t) noexcept { give(p); }
void operator delete[](void *p, size_t) noexcept { give(p); }
void operator delete(void *p, const std::nothrow_t &) noexcept { give(p); }
void operator delete[](void *p, const std::nothrow_t &) noexcept { give(p); }
void operator delete(void *p, std::align_val_t) noexcept { give(p); }
void operator delete[](void *p, std::align_val_t) noexcept { give(p); }
void operator delete(void *p, size_t, std::align_val_t) noexcept { give(p); }
void operator delete[](void *p, size_t, std::align_val_t) noexcept { give(p); }
void operator delete(void *p, std::align_val_t, const std::nothrow_t &) noexcept { give(p); }
void operator delete[](void *p, std::align_val_t, const std::nothrow_t &) noexcept { give(p); }
#endif

namespace city {

long freed_memory_wait_ms() { return mi_option_get(mi_option_purge_delay); }

void release_freed_memory() { mi_collect(true); }

AllocatorHold allocator_hold() {
	mi_stats_t stats;
	mi_stats_get(sizeof(stats), &stats); // what it takes of the system and its threads are counted for the process, not by thread
	return AllocatorHold{ double(stats.committed.current), stats.threads.current };
}

HeapUse heap_use() {
	HeapUse use;
	mi_heap_visit_blocks(
			mi_heap_get_default(), false,
			[](const mi_heap_t *, const mi_heap_area_t *area, void *, size_t, void *arg) {
				HeapUse &u = *static_cast<HeapUse *>(arg);
				const double live = double(area->used) * double(area->block_size);
				u.live += live;
				u.held += double(area->committed);
				u.by_size[area->block_size] += live;
				return true;
			},
			&use);
	return use;
}

std::vector<HeapSite> heap_sites(size_t most) {
	std::vector<HeapSite> out;
#if CITY_HEAP_SITES
	const uintptr_t base = reinterpret_cast<uintptr_t>(&__ImageBase);
	for (const Site &s : sites) {
		const uintptr_t at = s.at.load(std::memory_order_relaxed);
		const int64_t bytes = s.bytes.load(std::memory_order_relaxed);
		if (at && bytes > 0) out.push_back(HeapSite{ uint64_t(at - base), double(bytes), s.blocks.load(std::memory_order_relaxed) });
	}
	std::sort(out.begin(), out.end(), [](const HeapSite &a, const HeapSite &b) { return a.live > b.live; });
	if (out.size() > most) out.resize(most);
#else
	(void)most;
#endif
	return out;
}

} // namespace city
