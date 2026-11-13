// The job pool and the core's allocator for the tests: the pool's rule, seen on a pool of its own, and what the allocator holds
// of the system while a thread takes memory, frees it and ends. And the probes a renderer answers, handed on, with every probe's name.
#include "api/city_probes.h"

#include "render/render_world.h"
#include "render/surface_data.h"

#include "core/job_pool.h"
#include "core/memory.h"
#include "core/units.h"
#include "sim/state/city_state.h"

#include <godot_cpp/core/class_db.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr int SERIAL_WORKERS = 4, SERIAL_JOBS = 8; // the probe's pool and the jobs it runs one after the other
constexpr int SERIAL_JOB_MIB = 32; // what each builds: a segment of the allocator
constexpr std::chrono::milliseconds SERIAL_LINGER{ 50 }; // the probe's workers look at their heaps soon after their work, so the test is short
constexpr int LATE_LINGERS = 2; // rests the last job's memory outlives before its caller frees it: the worker has looked once by then
constexpr double SEGMENT_BYTES = 32 * BYTES_PER_MIB; // the allocator's unit of the system's memory (mimalloc MI_SEGMENT_SIZE)
constexpr auto GIVE_BACK_LIMIT = std::chrono::seconds(3); // how long the probe waits for the pool's memory to come back
constexpr auto GIVE_BACK_LOOK = std::chrono::milliseconds(5); // between two looks at it
} // namespace

Dictionary CityProbes::bay_census() { return render_->bay_census(); }
Dictionary CityProbes::far_form() const { return render_->far_form(); }
Dictionary CityProbes::ground_drawn(const Vector3 &at) const { return render_->ground_drawn(*state_, at.x, at.z); }
Dictionary CityProbes::lamp_pools(float x, float z, float radius) const { return render_->lamp_pools(*state_, x, z, radius); }
Array CityProbes::wire_draws() const { return render_->wire_draws(); }
Dictionary CityProbes::garden_rooms() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return render_->garden_rooms(*state_);
}
// What stands on a record's lot, one row an element: its kind (b40::Element), its middle in the world, its width, depth, height,
// base and angle: where a capture looks, and what a photo is read against.
Array CityProbes::lot_elements(int id) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Array rows;
	if (id < 0 || id >= int(state_->buildings.items.size()) || !state_->buildings.items[size_t(id)].alive) return rows;
	const Building &rec = state_->buildings.items[size_t(id)];
	const b40::LotFrame f = b40::lot_frame(rec);
	for (const b40::LotElement &e : *b40::record_elements(state_->buildings.designs, rec)) {
		const V2 at = f.at(e.x, e.y);
		rows.push_back(Array::make(int(e.kind), at.x, at.y, e.w, e.d, e.h, e.base, e.angle));
	}
	return rows;
}

Dictionary CityProbes::garden_fit(const String &kind, float w, float d, float h, float turn, int seed) const {
	const b40::Element k = kind == "tree" ? b40::EL_TREE : kind == "shrub" ? b40::EL_SHRUB : kind == "hedge" ? b40::EL_HEDGE : b40::EL_FLOWERS;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return render_->garden_fit(*state_, int(k), w, d, h, turn, seed);
}
// The surface render/surface_data.h makes of `arrays`: a test lays it beside the engine's own (mesh_get_surface) byte for byte.
Dictionary CityProbes::surface_probe(const Array &arrays, const Dictionary &lods, int64_t format) const { return surface_data(arrays, lods, uint64_t(format), RID()); }

// The pool's rule on a pool of one worker, held busy while the jobs queue: a key's newest job replaces a queued one, and only that.
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
	// Jobs that come one after the other, each building memory its caller frees once the job ended (as a mesh is freed after its
	// upload): they stay on one worker, and once the pool rests the allocator holds what it held before them.
	const AllocatorHold before = allocator_hold();
	std::mutex seen_lock;
	std::vector<std::thread::id> seen;
	using Built = std::vector<std::unique_ptr<char[]>>;
	auto build = [&seen, &seen_lock](Built &built) {
		for (int b = 0; b < SERIAL_JOB_MIB; b++) {
			built.emplace_back(new char[size_t(BYTES_PER_MIB)]);
			std::memset(built.back().get(), 1, size_t(BYTES_PER_MIB)); // written: the system has to back it
		}
		std::lock_guard<std::mutex> hold(seen_lock);
		if (std::find(seen.begin(), seen.end(), std::this_thread::get_id()) == seen.end()) seen.push_back(std::this_thread::get_id());
	};
	// Waits for a state, with a limit: MiB the allocator still holds past what it held before, once `rested` says so or the limit came.
	auto left = [&before](const std::function<bool()> &rested) {
		const auto limit = std::chrono::steady_clock::now() + GIVE_BACK_LIMIT;
		while (!rested() && std::chrono::steady_clock::now() < limit) std::this_thread::sleep_for(GIVE_BACK_LOOK);
		return (allocator_hold().committed - before.committed) / BYTES_PER_MIB;
	};
	auto back = [&before]() { return allocator_hold().committed - before.committed <= SEGMENT_BYTES; };
	JobPool serial(SERIAL_WORKERS, SERIAL_LINGER);
	for (int i = 0; i < SERIAL_JOBS; i++) {
		auto built = std::make_shared<Built>();
		serial.submit(JobKey{ JobLane::WATER, i }, 0.f, [built, &build]() { build(*built); });
		serial.wait_idle();
		built->clear(); // freed here, on the caller's thread, after the worker went back to rest
	}
	d["serial_threads"] = int64_t(seen.size());
	d["serial_left_mib"] = left(back);
	// The same for memory freed only after its worker rested and looked at its heap once, as the frame frees a mesh long after
	// the job that built it.
	auto late = std::make_shared<Built>();
	serial.submit(JobKey{ JobLane::WATER, SERIAL_JOBS }, 0.f, [late, &build]() { build(*late); });
	serial.wait_idle();
	std::this_thread::sleep_for(SERIAL_LINGER * LATE_LINGERS);
	late->clear();
	serial.freed(); // as the frame tells the pool once it freed what jobs built
	d["late_left_mib"] = left(back);
	// A paused pool hands out nothing: pause returns with no job running, what is submitted meanwhile runs only after resume.
	std::atomic<int> ran{ 0 }, running{ 0 };
	serial.submit(JobKey{ JobLane::WATER, SERIAL_JOBS + 1 }, 0.f, [&ran, &running]() {
		running++;
		std::this_thread::sleep_for(SERIAL_LINGER);
		running--;
		ran++;
	});
	serial.pause();
	d["pause_left_running"] = running.load();
	const int ran_by_pause = ran.load();
	serial.submit(JobKey{ JobLane::WATER, SERIAL_JOBS + 2 }, 0.f, [&ran]() { ran++; });
	std::this_thread::sleep_for(SERIAL_LINGER * LATE_LINGERS);
	d["ran_while_paused"] = ran.load() - ran_by_pause;
	serial.resume();
	serial.wait_idle();
	d["ran_after_resume"] = ran.load();
	return d;
}

// What was built under the map, by the lane of the job that built it, and the map's exclusive holds, by the writer each named
// (sim/state/map_hold.h): both counted by the tests' DLL since the last reset.
Dictionary CityProbes::map_hold(bool reset) {
	Dictionary built, writers;
	for (const auto &[who, count] : map_hold::tally()) built[String(who.c_str())] = count;
	for (const auto &[who, count] : map_hold::writers()) writers[String(who.c_str())] = count;
	if (reset) map_hold::clear();
	Dictionary d;
	d["counted"] = CHECKS;
	d["built"] = built;
	d["writers"] = writers;
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
	ClassDB::bind_method(D_METHOD("map_hold", "reset"), &CityProbes::map_hold);
	ClassDB::bind_method(D_METHOD("park_sample"), &CityProbes::park_sample);
	ClassDB::bind_method(D_METHOD("plan_all_yards"), &CityProbes::plan_all_yards);
	ClassDB::bind_method(D_METHOD("camera_clear", "at", "radius", "vehicles"), &CityProbes::camera_clear, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("kerb_busy"), &CityProbes::kerb_busy);
	ClassDB::bind_method(D_METHOD("dock_count"), &CityProbes::dock_count);
	ClassDB::bind_method(D_METHOD("look_twins", "reach_m", "alike"), &CityProbes::look_twins);
	ClassDB::bind_method(D_METHOD("lot_clearance"), &CityProbes::lot_clearance);
	ClassDB::bind_method(D_METHOD("vehicles_near", "at", "radius"), &CityProbes::vehicles_near);
	ClassDB::bind_method(D_METHOD("vehicle_bodies", "at", "radius"), &CityProbes::vehicle_bodies);
	ClassDB::bind_method(D_METHOD("footway_under", "at"), &CityProbes::footway_under);
	ClassDB::bind_method(D_METHOD("people_census", "at", "radius"), &CityProbes::people_census);
	ClassDB::bind_method(D_METHOD("walker_bodies", "at", "radius"), &CityProbes::walker_bodies);
	ClassDB::bind_method(D_METHOD("walkers_apart", "at", "steps", "aside"), &CityProbes::walkers_apart);
	ClassDB::bind_method(D_METHOD("bodies_audit", "clearance_m"), &CityProbes::bodies_audit);
	ClassDB::bind_method(D_METHOD("corner_ways"), &CityProbes::corner_ways);
	ClassDB::bind_method(D_METHOD("lot_gates"), &CityProbes::lot_gates);
	ClassDB::bind_method(D_METHOD("drive_kerbs"), &CityProbes::drive_kerbs);
	ClassDB::bind_method(D_METHOD("lot_fronts"), &CityProbes::lot_fronts);
	ClassDB::bind_method(D_METHOD("lot_bounds"), &CityProbes::lot_bounds);
	ClassDB::bind_method(D_METHOD("lot_paving"), &CityProbes::lot_paving);
	ClassDB::bind_method(D_METHOD("lot_covers"), &CityProbes::lot_covers);
	ClassDB::bind_method(D_METHOD("road_overlaps"), &CityProbes::road_overlaps);
	ClassDB::bind_method(D_METHOD("onto_sidewalk_moves"), &CityProbes::onto_sidewalk_moves);
	ClassDB::bind_method(D_METHOD("set_city_hour", "hour"), &CityProbes::set_city_hour);
	ClassDB::bind_method(D_METHOD("people_clips"), &CityProbes::people_clips);
	ClassDB::bind_method(D_METHOD("show_people_clip", "name"), &CityProbes::show_people_clip);
	ClassDB::bind_method(D_METHOD("light_holds", "v", "line_m", "top", "left", "type"), &CityProbes::light_holds);
	ClassDB::bind_method(D_METHOD("amber_wanted", "kmh"), &CityProbes::amber_wanted);
	ClassDB::bind_method(D_METHOD("job_pool_probe"), &CityProbes::job_pool_probe);
	ClassDB::bind_method(D_METHOD("memory_churn", "mib", "keep_one_in"), &CityProbes::memory_churn);
	ClassDB::bind_method(D_METHOD("allocator_probe"), &CityProbes::allocator_probe);
	ClassDB::bind_method(D_METHOD("flow_replay"), &CityProbes::flow_replay);
	ClassDB::bind_method(D_METHOD("flow_push", "n", "seconds"), &CityProbes::flow_push);
	ClassDB::bind_method(D_METHOD("flow_state", "n"), &CityProbes::flow_state);
	ClassDB::bind_method(D_METHOD("street_watch", "near_m"), &CityProbes::street_watch);
	ClassDB::bind_method(D_METHOD("scene_walker", "at"), &CityProbes::scene_walker);
	ClassDB::bind_method(D_METHOD("scene_car", "walker", "metres", "speed"), &CityProbes::scene_car);
	ClassDB::bind_method(D_METHOD("scene_watch", "walker", "look", "car"), &CityProbes::scene_watch);
	ClassDB::bind_method(D_METHOD("probe_set_money", "money"), &CityProbes::probe_set_money);
	ClassDB::bind_method(D_METHOD("probe_road_build", "profile", "structure", "height_a", "height_b", "len"), &CityProbes::probe_road_build);
	ClassDB::bind_method(D_METHOD("grade_probe", "scene"), &CityProbes::grade_probe);
	ClassDB::bind_method(D_METHOD("state_hash"), &CityProbes::state_hash);
	ClassDB::bind_method(D_METHOD("bay_census"), &CityProbes::bay_census);
	ClassDB::bind_method(D_METHOD("far_form"), &CityProbes::far_form);
	ClassDB::bind_method(D_METHOD("garden_rooms"), &CityProbes::garden_rooms);
	ClassDB::bind_method(D_METHOD("lot_elements", "id"), &CityProbes::lot_elements);
	ClassDB::bind_method(D_METHOD("garden_fit", "kind", "w", "d", "h", "turn", "seed"), &CityProbes::garden_fit);
	ClassDB::bind_method(D_METHOD("lot_parts", "zone", "cap", "w", "d", "seed", "count"), &CityProbes::lot_parts);
	ClassDB::bind_method(D_METHOD("open_lot_parts", "fn", "w", "d", "seed", "count", "streets"), &CityProbes::open_lot_parts);
	ClassDB::bind_method(D_METHOD("basin_rims", "fn", "zone", "w", "d", "seed", "count"), &CityProbes::basin_rims);
	ClassDB::bind_method(D_METHOD("covers_on_slope", "rows", "w", "d"), &CityProbes::covers_on_slope);
	ClassDB::bind_method(D_METHOD("square_fit", "lots", "seed"), &CityProbes::square_fit);
	ClassDB::bind_method(D_METHOD("square_rooms"), &CityProbes::square_rooms);
	ClassDB::bind_method(D_METHOD("ground_drawn", "at"), &CityProbes::ground_drawn);
	ClassDB::bind_method(D_METHOD("lamp_pools", "x", "z", "radius"), &CityProbes::lamp_pools);
	ClassDB::bind_method(D_METHOD("wire_draws"), &CityProbes::wire_draws);
	ClassDB::bind_method(D_METHOD("surface_probe", "arrays", "lods", "format"), &CityProbes::surface_probe);
	ClassDB::bind_method(D_METHOD("traffic_hashes", "ticks"), &CityProbes::traffic_hashes);
	ClassDB::bind_method(D_METHOD("graph_fault"), &CityProbes::graph_fault);
	ClassDB::bind_method(D_METHOD("broken_records"), &CityProbes::broken_records);
	ClassDB::bind_method(D_METHOD("undo_steps"), &CityProbes::undo_steps);
}

} // namespace city
