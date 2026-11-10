// The job pool and the core's allocator for the tests: the pool's rule, seen on a pool of its own, and what the allocator holds
// of the system while a thread takes memory, frees it and ends.
#include "api/city_probes.h"

#include "render/render_world.h"

#include "core/job_pool.h"
#include "core/memory.h"
#include "core/units.h"
#include "sim/state/city_state.h"

#include <godot_cpp/core/class_db.hpp>

#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

using namespace godot;

namespace city {

// The pool's rule on a pool of one worker, held busy while the jobs queue: a key's newest job replaces a queued one, and only that.
Dictionary CityProbes::bay_census() { return render_->bay_census(); }

Dictionary CityProbes::job_pool_probe() const {
	constexpr int64_t CHUNK_ID = 7;
	std::mutex gate;
	std::atomic<int> older{ 0 }, newer{ 0 }, other_lane{ 0 };
	JobPool pool(1);
	gate.lock();
	pool.submit(JobKey{ JobLane::FOG, 0 }, 0.f, [&gate]() { std::lock_guard<std::mutex> hold(gate); });
	pool.submit(JobKey{ JobLane::BUILDING, CHUNK_ID }, 1.f, [&older]() { older++; });
	pool.submit(JobKey{ JobLane::COVER, CHUNK_ID }, 1.f, [&other_lane]() { other_lane++; });
	pool.submit(JobKey{ JobLane::BUILDING, CHUNK_ID }, 1.f, [&newer]() { newer++; });
	gate.unlock();
	pool.wait_idle();
	Dictionary d;
	d["older"] = older.load();
	d["newer"] = newer.load();
	d["other_lane"] = other_lane.load();
	d["replaced"] = int64_t(pool.replaced());
	d["keys_left"] = int64_t(pool.tracked()); // forgotten once run: the bookkeeping does not grow with the map
	d["game_keys"] = int64_t(state_->jobs->tracked()); // the game's pool now: keys with a job queued
	return d;
}

// Memory a thread used and freed is the system's again, also where live blocks stand beside it, and a thread that ended leaves no
// heap behind: a thread takes blocks of a MiB and writes them, frees all but one in `keep_one_in` and gives back, then frees the
// rest and ends.
Dictionary CityProbes::memory_churn(int mib, int keep_one_in) const {
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

Dictionary CityProbes::allocator_probe() const {
	const AllocatorHold hold = allocator_hold();
	Dictionary d;
	d["committed"] = hold.committed / BYTES_PER_MIB;
	d["threads"] = hold.threads;
	return d;
}

void CityProbes::_bind_methods() {
	ClassDB::bind_method(D_METHOD("park_sample"), &CityProbes::park_sample);
	ClassDB::bind_method(D_METHOD("plan_all_yards"), &CityProbes::plan_all_yards);
	ClassDB::bind_method(D_METHOD("camera_clear", "at", "radius", "vehicles"), &CityProbes::camera_clear, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("kerb_busy"), &CityProbes::kerb_busy);
	ClassDB::bind_method(D_METHOD("dock_count"), &CityProbes::dock_count);
	ClassDB::bind_method(D_METHOD("look_twins", "reach_m", "alike"), &CityProbes::look_twins);
	ClassDB::bind_method(D_METHOD("lot_clearance"), &CityProbes::lot_clearance);
	ClassDB::bind_method(D_METHOD("vehicles_near", "at", "radius"), &CityProbes::vehicles_near);
	ClassDB::bind_method(D_METHOD("people_census", "at", "radius"), &CityProbes::people_census);
	ClassDB::bind_method(D_METHOD("set_city_hour", "hour"), &CityProbes::set_city_hour);
	ClassDB::bind_method(D_METHOD("people_clips"), &CityProbes::people_clips);
	ClassDB::bind_method(D_METHOD("show_people_clip", "name"), &CityProbes::show_people_clip);
	ClassDB::bind_method(D_METHOD("amber_stops", "v", "stop_m", "top", "left", "type"), &CityProbes::amber_stops);
	ClassDB::bind_method(D_METHOD("amber_wanted", "kmh"), &CityProbes::amber_wanted);
	ClassDB::bind_method(D_METHOD("job_pool_probe"), &CityProbes::job_pool_probe);
	ClassDB::bind_method(D_METHOD("memory_churn", "mib", "keep_one_in"), &CityProbes::memory_churn);
	ClassDB::bind_method(D_METHOD("allocator_probe"), &CityProbes::allocator_probe);
	ClassDB::bind_method(D_METHOD("flow_replay"), &CityProbes::flow_replay);
	ClassDB::bind_method(D_METHOD("flow_junction"), &CityProbes::flow_junction);
	ClassDB::bind_method(D_METHOD("flow_push", "n", "seconds"), &CityProbes::flow_push);
	ClassDB::bind_method(D_METHOD("flow_state", "n"), &CityProbes::flow_state);
	ClassDB::bind_method(D_METHOD("street_watch", "near_m"), &CityProbes::street_watch);
	ClassDB::bind_method(D_METHOD("probe_set_money", "money"), &CityProbes::probe_set_money);
	ClassDB::bind_method(D_METHOD("probe_road_build", "profile", "structure", "height_a", "height_b", "len"), &CityProbes::probe_road_build);
	ClassDB::bind_method(D_METHOD("grade_probe", "scene"), &CityProbes::grade_probe);
	ClassDB::bind_method(D_METHOD("state_hash"), &CityProbes::state_hash);
	ClassDB::bind_method(D_METHOD("bay_census"), &CityProbes::bay_census);
	ClassDB::bind_method(D_METHOD("traffic_hashes", "ticks"), &CityProbes::traffic_hashes);
	ClassDB::bind_method(D_METHOD("graph_fault"), &CityProbes::graph_fault);
	ClassDB::bind_method(D_METHOD("undo_steps"), &CityProbes::undo_steps);
}

} // namespace city
