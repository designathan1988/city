// The memory probes: where the process's memory is, by owner (memory_census), and every thread of the core giving back what its
// heap holds freed (memory_release), so a measurement tells what the allocator holds from what the city needs. For a measurement,
// once: the census walks every heap's pages and the city's containers.
#include "api/city_core.h"

#include "core/memory.h"
#include "core/units.h"

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

#include <mimalloc.h>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

using namespace godot;

namespace city {

namespace {
#ifdef _WIN32
constexpr DWORD MOST_HEAPS = 256; // a process has a handful (7 measured): room for far more than it holds
#endif
constexpr auto MEET_LIMIT = std::chrono::seconds(2); // how long the threads of a census wait for one another: a worker in a long job is left out
constexpr double SIZE_ROW_MIB = 8.0; // a block size is a row of the census from this many MiB live on: the sizes that hold the memory
constexpr size_t SITE_ROWS = 120; // the places of the code the census names, heaviest first (a measuring build: core/memory.h heap_sites)
template <typename V>
double vec_mib(const V &v) {
	return double(v.capacity() * sizeof(typename V::value_type)) / BYTES_PER_MIB;
}
// The bytes a vector holds on the heap.
template <typename T>
double heap_bytes(const std::vector<T> &v) {
	return double(v.capacity() * sizeof(T));
}
// A vector of vectors: the outer array and what each inner one holds, MiB.
template <typename T>
double nested_mib(const std::vector<std::vector<T>> &v) {
	double bytes = heap_bytes(v);
	for (const std::vector<T> &inner : v) bytes += heap_bytes(inner);
	return bytes / BYTES_PER_MIB;
}

// `fn` once on each worker of the pool and on the calling thread. Each thread takes one piece of a parallel_for and holds it until
// all came, so no thread takes two; a worker in a job longer than MEET_LIMIT is left out, and a thread that then takes a second
// piece does not run `fn` again.
size_t on_every_thread(JobPool &pool, const std::function<void()> &fn) {
	const size_t n = pool.workers() + 1;
	std::mutex m;
	std::condition_variable cv;
	std::vector<std::thread::id> came;
	pool.parallel_for(n, [&](size_t) {
		const std::thread::id me = std::this_thread::get_id();
		{
			std::lock_guard<std::mutex> lk(m);
			for (const std::thread::id &id : came)
				if (id == me) return;
		}
		fn();
		std::unique_lock<std::mutex> lk(m);
		came.push_back(me);
		cv.notify_all();
		cv.wait_for(lk, MEET_LIMIT, [&]() { return came.size() == n; });
	});
	return came.size();
}
} // namespace

// The process's memory against its owners, MiB: the working set and private bytes, the C runtime's heaps (the engine's allocator), the
// core's allocator as its statistics print it and as every thread's heap holds it (live in blocks, held in pages, the block sizes
// that hold the most), and the core's largest owners by their containers' capacities.
Dictionary CityCore::memory_census() const {
	Dictionary d;
#ifdef _WIN32
	PROCESS_MEMORY_COUNTERS_EX pm{};
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pm), sizeof(pm))) {
		d["working_set"] = double(pm.WorkingSetSize) / BYTES_PER_MIB;
		d["private"] = double(pm.PrivateUsage) / BYTES_PER_MIB;
	}
	HANDLE heaps[MOST_HEAPS];
	const DWORD n = GetProcessHeaps(DWORD(std::size(heaps)), heaps);
	double allocated = 0, committed = 0;
	for (DWORD i = 0; i < n && i < DWORD(std::size(heaps)); i++) {
		HEAP_SUMMARY hs{};
		hs.cb = sizeof(hs);
		if (HeapSummary(heaps[i], 0, &hs)) {
			allocated += double(hs.cbAllocated);
			committed += double(hs.cbCommitted);
		}
	}
	d["heaps"] = int(n);
	d["heap_allocated"] = allocated / BYTES_PER_MIB;
	d["heap_committed"] = committed / BYTES_PER_MIB;
#endif
	// the core's own allocator (core/memory.cpp): what it holds of the system, as its statistics print it (the rows a thread counts
	// for itself are the calling thread's only), and what each thread's heap holds
	std::string stats;
	mi_stats_merge();
	mi_stats_print_out([](const char *msg, void *arg) { static_cast<std::string *>(arg)->append(msg); }, &stats);
	d["mimalloc"] = String(stats.c_str());
	std::mutex uses_m;
	std::vector<HeapUse> uses;
	d["core_threads"] = int64_t(on_every_thread(*state_->jobs, [&]() {
		HeapUse use = heap_use();
		std::lock_guard<std::mutex> lk(uses_m);
		uses.push_back(std::move(use));
	}));
	double live = 0, held = 0;
	std::map<size_t, double> by_size;
	Array threads;
	for (const HeapUse &u : uses) {
		live += u.live;
		held += u.held;
		for (const auto &row : u.by_size) by_size[row.first] += row.second;
		threads.push_back(Vector2(float(u.live / BYTES_PER_MIB), float(u.held / BYTES_PER_MIB)));
	}
	Dictionary sizes;
	for (const auto &row : by_size)
		if (row.second / BYTES_PER_MIB >= SIZE_ROW_MIB) sizes[int64_t(row.first)] = row.second / BYTES_PER_MIB;
	d["core_live"] = live / BYTES_PER_MIB;
	d["core_held"] = held / BYTES_PER_MIB;
	d["core_by_thread"] = threads; // (live, held) of each thread's heap
	d["core_by_size"] = sizes; // block size in bytes -> MiB live in blocks of it
	PackedStringArray sites; // a measuring build only: "address MiB blocks" of the places that asked for the most live bytes
	for (const HeapSite &site : heap_sites(SITE_ROWS)) sites.push_back(vformat("%x %.1f %d", int64_t(site.at), site.live / BYTES_PER_MIB, site.blocks));
	d["core_sites"] = sites;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Heightfield &h = s.terrain;
	d["terrain"] = vec_mib(h.base_q) + double(h.final_h.bytes()) / BYTES_PER_MIB + vec_mib(h.ground_moved) + vec_mib(h.water) + vec_mib(h.tile_dirty);
	d["terrain_samples"] = int64_t(h.N) * h.N;
	d["body_cache"] = double(s.buildings.designs.made->used()) / BYTES_PER_MIB;
	d["buildings"] = vec_mib(s.buildings.items);
	d["vehicles"] = vec_mib(s.traffic.vehicles) + vec_mib(s.traffic.route_pool);
	d["walkers"] = vec_mib(s.traffic.peds);
	// What grows with the map's roads: the records with the vectors each holds, the lane graph and the traffic's lists by lane,
	// by connector and by node; and the lots.
	std::lock_guard<std::mutex> traffic(state_->traffic_mutex);
	double node_heap = 0, seg_heap = 0, conn_heap = 0, junction_heap = 0, lots = 0;
	for (const RoadNode &node : s.roads.nodes) node_heap += heap_bytes(node.segs) + heap_bytes(node.links) + heap_bytes(node.signal.greens);
	for (const RoadSegment &g : s.roads.segs) seg_heap += heap_bytes(g.table);
	d["road_records"] = vec_mib(s.roads.nodes) + vec_mib(s.roads.segs) + (node_heap + seg_heap) / BYTES_PER_MIB;
	const TrafficState &t = s.traffic;
	const LaneGraph &lg = t.g;
	for (const Conn &c : lg.conns) conn_heap += heap_bytes(c.conflicts) + heap_bytes(c.conflict_at);
	for (const Junction &j : lg.junctions) {
		junction_heap += heap_bytes(j.green) + heap_bytes(j.green_max) + heap_bytes(j.amber) + heap_bytes(j.all_red) + heap_bytes(j.least) + heap_bytes(j.served) +
				heap_bytes(j.walk_call) + heap_bytes(j.walk_from) + heap_bytes(j.approaches);
		for (const std::vector<int> &a : j.approaches) junction_heap += heap_bytes(a);
	}
	d["lane_graph"] = vec_mib(lg.ds) + vec_mib(lg.conns) + vec_mib(lg.junctions) + vec_mib(lg.node_conn) + vec_mib(lg.lane_conn) + (conn_heap + junction_heap) / BYTES_PER_MIB;
	d["lane_lists"] = nested_mib(t.lane_vehicles) + nested_mib(t.lane_shadows) + nested_mib(t.conn_vehicles) + nested_mib(t.node_crossers) + nested_mib(t.crowd) +
			nested_mib(t.node_walkers) + nested_mib(t.doors) + nested_mib(t.route_next);
	for (const auto &cell : s.lots.by_cell) lots += heap_bytes(cell.second) + double(sizeof(cell));
	d["lots"] = lots / BYTES_PER_MIB;
	d["segments"] = s.roads.alive_segments();
	d["nodes"] = s.roads.alive_nodes();
	d["connectors"] = int64_t(lg.conns.size());
	return d;
}

// Every worker of the pool and the calling thread give back what their heaps hold freed (core/memory.h): how many did.
int CityCore::memory_release() {
	return int(on_every_thread(*state_->jobs, []() { release_freed_memory(); }));
}

// A thread of its own takes `mib` MiB of the core's allocator in blocks of a MiB and writes them, frees all but one in `keep_one_in`
// and gives back, then frees the rest and ends: what the allocator held of the system before, at the most, with the few blocks
// left among the freed ones (thin) and after (MiB), and its threads before and after. Memory freed is the system's again, also
// where live blocks stand beside it, and a thread that ended leaves no heap behind (tests/core/memory_test).
Dictionary CityCore::memory_churn(int mib, int keep_one_in) {
	const AllocatorHold before = allocator_hold();
	AllocatorHold most, thin;
	std::thread worker([&]() {
		std::vector<std::unique_ptr<char[]>> blocks;
		for (int i = 0; i < mib; i++) {
			blocks.emplace_back(new char[size_t(BYTES_PER_MIB)]);
			std::memset(blocks.back().get(), 1, size_t(BYTES_PER_MIB)); // written: the system has to back it
		}
		most = allocator_hold();
		for (int i = 0; i < mib; i++)
			if (i % keep_one_in != 0) blocks[size_t(i)].reset();
		release_freed_memory();
		thin = allocator_hold();
		blocks.clear();
		release_freed_memory();
	});
	worker.join();
	const AllocatorHold after = allocator_hold();
	Dictionary d;
	d["before"] = before.committed / BYTES_PER_MIB;
	d["most"] = most.committed / BYTES_PER_MIB;
	d["thin"] = thin.committed / BYTES_PER_MIB;
	d["after"] = after.committed / BYTES_PER_MIB;
	d["threads_before"] = before.threads;
	d["threads_after"] = after.threads;
	return d;
}

} // namespace city
