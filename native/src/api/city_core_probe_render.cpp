// Probes for the tests: the agents' and the wear's cost (agent_cost, wear_probe), where the last _process went (render_cost), the road
// meshes' size (road_mesh_counts), how far they stand off the roads (road_surface_probe), the ground seams (road_seam_probe), the
// furniture's poles and wires on the GPU (prop_buffer_probe), and how the agents' poses arrive (pose_stats: publications, their gaps,
// frames shown past the newest pose). The traffic's own probe is city_core_probe.cpp.
#include "api/city_core.h"

#include "core/units.h"
#include "render/roads/road_mesh.h"
#include "render/surface_data.h"

#include <godot_cpp/classes/hashing_context.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <mutex>
#include <shared_mutex>
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
template <typename V>
double vec_mib(const V &v) {
	return double(v.capacity() * sizeof(typename V::value_type)) / BYTES_PER_MIB;
}
} // namespace

// The surface render/surface_data.h makes of `arrays`: a test lays it beside the engine's own (mesh_get_surface) byte for byte.
Dictionary CityCore::surface_probe(const Array &arrays, const Dictionary &lods, int64_t format) const {
	return surface_data(arrays, lods, uint64_t(format), RID());
}

// The process's memory against its owners, MiB: the working set and private bytes, every heap's allocated bytes (the engine's allocator and
// the core's containers share the C runtime's heap), and the core's largest owners by their containers' capacities. For a measurement, once.
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
	// the core's own allocator (core/memory.cpp): what it holds, as its statistics print them
	std::string stats;
	mi_stats_merge();
	mi_stats_print_out([](const char *msg, void *arg) { static_cast<std::string *>(arg)->append(msg); }, &stats);
	d["mimalloc"] = String(stats.c_str());
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Heightfield &h = s.terrain;
	d["terrain"] = vec_mib(h.base_q) + vec_mib(h.final_h) + vec_mib(h.ground_moved) + vec_mib(h.water) + vec_mib(h.tile_dirty);
	d["terrain_samples"] = int64_t(h.N) * h.N;
	d["body_cache"] = double(s.buildings.designs.made->used()) / BYTES_PER_MIB;
	d["buildings"] = vec_mib(s.buildings.items);
	d["vehicles"] = vec_mib(s.traffic.vehicles) + vec_mib(s.traffic.route_pool);
	d["walkers"] = vec_mib(s.traffic.peds);
	return d;
}

Dictionary CityCore::agent_cost() const { return render_.agent_cost(); }

// The wear field counted cell by cell: a scan of the whole field, for the captures only (never per frame).
Dictionary CityCore::wear_probe() const {
	Dictionary d;
	d["worn_cells"] = render_.worn_cells();
	d["wear_publications"] = render_.wear_publications();
	return d;
}

Dictionary CityCore::render_cost() const {
	const RenderWorld::Cost &c = render_.cost();
	Dictionary d;
	d["lit_street_lamps"] = render_.lit_lamps();
	d["lit_head_lamps"] = render_.lit_heads();
	d["lamps_hand_over"] = render_.lamps_hand_over();
	d["process"] = process_ms_; // constant time: the game reads it on every long frame (health log), so it must not make one
	d["agents"] = agents_ms_;
	d["lock"] = c.lock;
	d["stream"] = c.stream;
	d["levels"] = c.levels;
	d["flush"] = c.flush;
	d["chunk_max"] = c.chunk_max;
	d["super_max"] = c.super_max;
	d["super_mb_max"] = double(render_.super_bytes_max()) / BYTES_PER_MIB; // largest far mesh built
	d["b_scan"] = c.building.scan;
	d["b_freed"] = c.building.freed;
	d["b_sort"] = c.building.sort;
	d["b_admit"] = c.building.admit;
	d["b_hand"] = c.building.hand;
	d["b_tail"] = c.building.tail;
	d["roads"] = c.roads;
	d["props"] = c.props;
	d["buildings"] = c.buildings;
	d["trees"] = c.trees;
	d["overlays"] = overlays_ms_;
	d["uploads"] = c.uploads; // MiB
	const BuildingViewNeed need = render_.building_need();
	d["backlog"] = c.backlog;
	d["budget_left"] = c.budget_left;
	d["b_want"] = need.want;
	d["b_unmet"] = need.unmet;
	d["b_requested"] = need.requested;
	d["b_done"] = need.done;
	d["jobs_replaced"] = int64_t(state_->jobs ? state_->jobs->replaced() : 0);
	return d;
}

namespace {
template <typename Count>
Dictionary count_of(const Count &n) {
	Dictionary d;
	d["chunks"] = n.chunks; // one instance and one draw call each
	d["triangles"] = n.all.triangles;
	d["vertices"] = n.all.vertices;
	d["bytes"] = n.all.bytes;
	d["largest_bytes"] = n.largest;
	return d;
}
} // namespace

Dictionary CityCore::road_mesh_counts() const {
	Dictionary d;
	d["roads"] = count_of(render_.road_count());
	d["seams"] = count_of(render_.seam_count());
	d["wear_bytes"] = render_.wear_bytes();
	return d;
}

// The ground seams of every chunk a road or a node stands in (render/roads/ground_seam.h), built anew from the state: the cells drawn
// and cut, the triangles, the cells no triangulation took (also of the standing chunks), the outline points no paving gave a height,
// where the last of them is (its y: its polygon's ring count), and the worst cell where seam and paving do not make the cell's area.
Dictionary CityCore::road_seam_probe() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::vector<int64_t> keys;
	for (size_t sid = 0; sid < state_->roads.segs.size(); sid++) // every chunk a road's drawing reaches (RoadNetwork::for_drawn_chunks)
		if (state_->roads.segs[sid].alive) state_->roads.for_drawn_chunks(int(sid), [&](int64_t k) { keys.push_back(k); });
	std::sort(keys.begin(), keys.end());
	keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
	SeamProbe all;
	for (const int64_t k : keys) {
		const SeamProbe p = render_.probe_seam(*state_, k);
		all.cells += p.cells;
		all.cut += p.cut;
		all.triangles += p.triangles;
		all.refused += p.refused;
		all.unheighted += p.unheighted;
		if (p.unheighted > 0) all.lost = p.lost;
		all.worst_cover_m2 = std::max(all.worst_cover_m2, p.worst_cover_m2);
	}
	Dictionary d;
	d["chunks"] = int64_t(keys.size());
	d["cells"] = all.cells;
	d["cut"] = all.cut;
	d["triangles"] = all.triangles;
	d["refused"] = all.refused;
	d["refused_drawn"] = render_.seam_refused();
	d["unheighted"] = all.unheighted;
	d["lost"] = all.lost;
	d["worst_cover_m2"] = all.worst_cover_m2;
	return d;
}

// Each furniture chunk's pole and wire instances as the GPU holds them (PropRenderer::instance_buffers), as "chunk key" -> "poles
// count:md5 wires count:md5": two runs of one plan compare by it, a chunk whose instances or their order differ shows by its key.
Dictionary CityCore::prop_buffer_probe() const {
	const Dictionary buffers = render_.prop_buffers();
	auto digest = [](const PackedFloat32Array &a) {
		Ref<HashingContext> h;
		h.instantiate();
		h->start(HashingContext::HASH_MD5);
		h->update(a.to_byte_array());
		return String::num_int64(a.size()) + ":" + h->finish().hex_encode();
	};
	Dictionary d;
	const Array keys = buffers.keys();
	for (int64_t i = 0; i < keys.size(); i++) {
		const Array pair = buffers[keys[i]];
		d[keys[i]] = digest(pair[0]) + " " + digest(pair[1]);
	}
	return d;
}

// The road surfaces' stations over the whole map (render/roads/road_emit.cpp segment_stations), the near level's: how many
// stretches the segments are drawn in, how far the mesh stands off the true surface at worst (read densely between the stations:
// stations_error), the tolerance it is held to, and how many segments are drawn in one stretch (straight, on one grade).
Dictionary CityCore::road_surface_probe() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const StreetLook &L = render_.street_look();
	int64_t stretches = 0, segments = 0, whole = 0;
	double worst = 0;
	for (size_t sid = 0; sid < state_->roads.segs.size(); sid++) {
		const RoadSegment &g = state_->roads.segs[sid];
		const double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
		if (!g.alive || v1 <= v0) continue;
		const std::vector<double> at = segment_stations(*state_, L, false, int(sid), v0, v1);
		segments++;
		stretches += int64_t(at.size()) - 1;
		whole += at.size() == 2 ? 1 : 0;
		worst = std::max(worst, stations_error(*state_, int(sid), at));
	}
	Dictionary d;
	d["segments"] = segments;
	d["stretches"] = stretches;
	d["whole"] = whole;
	d["error_m"] = worst;
	d["tolerance_m"] = L.surface_error;
	return d;
}

namespace {
constexpr float EARLY_GAP = 0.5f, LATE_GAP = 1.5f; // pose_stats: a publication this share of a tick after the last
} // namespace

void CityCore::note_pose(const PoseFrame &f, bool fresh, const PoseSample &at) {
	PoseStats &st = pose_stats_;
	st.frames++;
	if (at.beyond) st.held++; // the moment drawn is past the newest pose: the agents stand still this frame
	if (at.before) st.behind++; // older than the trail: they jump to its oldest pose
	if (!fresh || f.period_ns <= 0) return;
	const int64_t newest = f.stamp_ns[POSE_TRAIL - 1];
	if (st.last_tick_ns > 0) {
		const int64_t gap = newest - st.last_tick_ns;
		st.gap_min_ns = st.publications > 0 ? std::min(st.gap_min_ns, gap) : gap;
		st.gap_max_ns = std::max(st.gap_max_ns, gap);
		if (float(gap) < EARLY_GAP * float(f.period_ns)) st.early++;
		if (float(gap) > LATE_GAP * float(f.period_ns)) st.late++;
		st.publications++;
	}
	st.last_tick_ns = newest;
}

Dictionary CityCore::pose_stats() {
	PoseStats &st = pose_stats_;
	Dictionary d;
	d["frames"] = st.frames;
	d["held"] = st.held;
	d["behind"] = st.behind;
	d["publications"] = st.publications;
	d["early"] = st.early;
	d["late"] = st.late;
	d["gap_min_ms"] = double(st.gap_min_ns) / NS_PER_MS;
	d["gap_max_ms"] = double(st.gap_max_ns) / NS_PER_MS;
	d["sim_wakeups"] = int64_t(sim_.wakeups()); // since the clock started (not reset by this call)
	const int64_t last = st.last_tick_ns;
	st = PoseStats();
	st.last_tick_ns = last;
	return d;
}

} // namespace city
