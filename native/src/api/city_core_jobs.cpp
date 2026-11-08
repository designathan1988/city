// The job pool for the tests: its rule, seen on a pool of its own.
#include "api/city_probes.h"

#include "render/render_world.h"

#include "core/job_pool.h"
#include "sim/state/city_state.h"

#include <godot_cpp/core/class_db.hpp>

#include <atomic>
#include <mutex>

using namespace godot;

namespace city {

Dictionary CityProbes::bay_census() { return render_->bay_census(); }

// The pool's rule on a pool of one worker, held busy while the jobs queue: a key's newest job replaces a queued one, and only that.
Dictionary CityProbes::job_pool_probe() const {
	constexpr int64_t CHUNK_ID = 7; // any chunk's id: the probe's two jobs share it
	std::mutex gate;
	std::atomic<int> older{ 0 }, newer{ 0 }, other_lane{ 0 };
	JobPool pool(1);
	gate.lock();
	pool.submit(JobKey{ JobLane::PROBE_HOLD, 0 }, 0.f, [&gate]() { std::lock_guard<std::mutex> hold(gate); });
	pool.submit(JobKey{ JobLane::PROBE_KEY, CHUNK_ID }, 1.f, [&older]() { older++; });
	pool.submit(JobKey{ JobLane::PROBE_OTHER, CHUNK_ID }, 1.f, [&other_lane]() { other_lane++; });
	pool.submit(JobKey{ JobLane::PROBE_KEY, CHUNK_ID }, 1.f, [&newer]() { newer++; });
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
