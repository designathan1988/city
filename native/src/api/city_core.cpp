// Lifecycle: catalog, render setup, new map, speed, per-frame render sync.
#include "api/city_core.h"

#include "api/buildings_loader.h"
#include "api/interiors_loader.h"
#include "api/catalog_loader.h"
#include "sim/citygen/city_gen.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <chrono>

using namespace godot;

namespace city {

namespace {
const char *const OVERLAY_NAMES[int(Overlay::COUNT)] = { "zones", "congestion" }; // Layers rows (data/ui/drawers.json)
} // namespace

CityCore::CityCore() : state_(std::make_unique<CityState>()) {}

CityCore::~CityCore() {
	join_save();
	sim_.stop();
	if (state_ && state_->jobs) state_->jobs->wait_idle();
	render_.reset();
	render_.shutdown();
}

Object *CityCore::terrain3d() const {
	if (terrain_path_.is_empty() || !is_inside_tree()) return nullptr;
	return get_node_or_null(terrain_path_);
}

PackedStringArray CityCore::load_catalog(const Dictionary &tables) {
	PackedStringArray errors;
	sim_.stop();
	catalog_ok_ = CatalogLoader::load(tables, state_->catalog, errors);
	return errors;
}

PackedStringArray CityCore::setup_render(const Dictionary &tables, const Dictionary &materials) {
	PackedStringArray errors;
	if (!is_inside_tree()) {
		errors.push_back("CityCore fora da arvore");
		return errors;
	}
	if (!catalog_ok_) { // the render draws the simulation's street furniture and crosswalks from the catalog
		errors.push_back("render antes do catalogo");
		return errors;
	}
	// The building generator's tables belong to the simulation (lots are world data): read before any body is made.
	state_->buildings.designs = b40::Data{};
	if (!load_buildings(tables.get("buildings", Dictionary()), state_->buildings.designs, errors)) return errors;
	if (!load_interiors(tables.get("interiors", Dictionary()), state_->buildings.designs.interior, errors)) return errors;
	RenderContext ctx;
	ctx.scenario = get_world_3d()->get_scenario();
	materials_ = materials;
	ctx.tables = tables;
	ctx.catalog = &state_->catalog;
	Array names = materials.keys();
	for (int i = 0; i < names.size(); i++) {
		Ref<Material> m = materials[names[i]];
		if (m.is_valid()) ctx.materials[std::string(String(names[i]).utf8().get_data())] = m->get_rid();
	}
	render_ok_ = render_.init(ctx, errors);
	return errors;
}

Dictionary CityCore::new_map(int map, int seed, bool with_city) {
	Dictionary d;
	d["ok"] = false;
	if (!catalog_ok_) return d;
	join_save();
	sim_.stop();
	CityGenReport r;
	{
		std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
		state_->growth.clear();
		r = generate_map(*state_, map, uint32_t(seed), with_city);
		traffic::reset_traffic(*state_); // a new map: full lane graph and an empty fleet
	}
	need_full_rebuild_ = true;
	sim_.start(*state_);
	d["ok"] = true;
	d["nodes"] = r.nodes;
	d["segments"] = r.segments;
	d["lots"] = r.lots;
	d["buildings"] = r.buildings;
	d["seconds"] = r.seconds;
	return d;
}

void CityCore::mark_saved() {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	state_->saved_revision = state_->revision;
}

void CityCore::set_speed(int speed) {
	sim_.set_speed(SimSpeed(clampi(speed, int(SimSpeed::PAUSED), int(SimSpeed::FASTEST))));
}

int CityCore::get_speed() const { return int(sim_.speed()); }

void CityCore::set_day_hour(double hour) { state_->clock.set_hour(hour); }

void CityCore::_process(double delta) {
	if (Engine::get_singleton()->is_editor_hint()) return;
	if (!render_ok_ || !state_->ready) {
		// No consumer in this run (tests without a screen): drain the change log so it cannot grow without bound
		// (shared map lock + the log's own mutex, as every consumer: change_log.h).
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
		std::lock_guard<std::mutex> log(state_->log.mutex);
		state_->log.clear_render();
		state_->log.lot_cells.clear();
		return;
	}
	Laps total, laps;
	Camera3D *cam = get_viewport() ? get_viewport()->get_camera_3d() : nullptr;
	if (cam) cam_ = cam->get_global_position();
	state_->camera_x.store(cam_.x);
	state_->camera_z.store(cam_.z);
	bool fresh = false;
	const PoseFrame &poses = state_->poses.acquire(fresh);
	const int64_t now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	// a moment render.pose_delay_ticks behind the clock, inside the published trail even when a tick lands late
	const PoseSample at = poses.sample(now - int64_t(state_->catalog.tune.render_pose_delay_ticks * double(poses.period_ns)));
	note_pose(poses, fresh, at);
	render_.update_agents(poses, cam_, at); // no map lock: the frame is the published snapshot
	const int64_t published = poses.stamp_ns[POSE_TRAIL - 1];
	const double ticks = fresh && poses.period_ns > 0 && wear_stamp_ns_ > 0 ? double(published - wear_stamp_ns_) / double(poses.period_ns) : 0;
	if (fresh) wear_stamp_ns_ = published;
	render_.update_wear(poses, fresh, float(ticks / state_->catalog.tune.sim_tick_hz), cam_, float(delta));
	render_.update_signals(poses);
	agents_ms_ = laps.lap();
	if (need_full_rebuild_) {
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex); // reads the map; the log has its own mutex
		render_.full_rebuild(*state_, terrain3d());
		need_full_rebuild_ = false;
	}
	render_.sync(*state_, terrain3d(), cam_); // takes the map shared (never exclusive: render_world.h)
	laps.lap();
	{
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
		render_.update_overlays(*state_, poses, delta);
	}
	overlays_ms_ = laps.lap();
	process_ms_ = total.lap();
}

void CityCore::set_overlay(const String &name, bool on) {
	for (int i = 0; i < int(Overlay::COUNT); i++)
		if (name == OVERLAY_NAMES[i]) render_.set_overlay(Overlay(i), on);
}

Dictionary CityCore::overlays() const {
	Dictionary d;
	for (int i = 0; i < int(Overlay::COUNT); i++) d[OVERLAY_NAMES[i]] = render_.overlay_on(Overlay(i));
	return d;
}

void CityCore::step_ticks(int ticks) {
	for (int i = 0; i < ticks; i++) sim_.step(*state_);
}

} // namespace city
