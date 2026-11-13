// Lifecycle: catalog, render setup, new map, speed, per-frame render sync (and, in the tests' DLL, the broken records said).
#include "api/city_core.h"

#include "api/buildings_loader.h"
#include "api/interiors_loader.h"
#include "api/models_loader.h"
#include "api/catalog_loader.h"
#include "api/broken_records.h"
#include "core/checks.h"
#include "core/units.h"
#include "sim/citygen/city_gen.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>

using namespace godot;

namespace city {

namespace {
const char *const OVERLAY_NAMES[int(Overlay::COUNT)] = { "zones", "congestion" }; // Layers rows (data/ui/drawers.json)
} // namespace

CityCore::CityCore() : state_(std::make_unique<CityState>()) { make_areas(); }

namespace {

// Below this the camera looks almost level: the line of sight reaches the ground too far off to mean anything.
constexpr float LEVEL_VIEW = 0.05f;

// Where the player is looking: the line of sight down to the ground. The windows of traffic and walkers stand there (the
// 4.0's focus, the camera's target), not under an eye that from high up stands past the edge of the city it shows.
Vector3 ground_seen(const Camera3D &cam) {
	const Transform3D t = cam.get_global_transform();
	const Vector3 ahead = -t.basis.get_column(2);
	return ahead.y < -LEVEL_VIEW ? t.origin + ahead * (t.origin.y / -ahead.y) : t.origin;
}

} // namespace

CityCore::~CityCore() {
	free_areas(); // first: they point at the state and the render
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
	if (catalog_ok_) state_->broken_sink.reserve(size_t(state_->catalog.tune.checks_records_kept)); // the room, once a session
	return errors;
}

PackedStringArray CityCore::setup_render(const Dictionary &tables, const Dictionary &materials) {
	PackedStringArray errors;
	if (!is_inside_tree()) {
		errors.push_back("CityCore outside the tree");
		return errors;
	}
	if (!catalog_ok_) { // the render draws the simulation's street furniture and crosswalks from the catalog
		errors.push_back("render before the catalog");
		return errors;
	}
	// The building generator's tables belong to the simulation (lots are world data): read before any body is made.
	state_->buildings.designs = b40::Data{};
	state_->buildings.designs.made->set_budget(size_t(state_->catalog.tune.sim_body_cache_mb * BYTES_PER_MIB));
	if (!load_buildings(tables.get("buildings", Dictionary()), state_->buildings.designs, errors)) return errors;
	if (!load_models(tables.get("models", Dictionary()), state_->buildings.designs, errors)) return errors;
	if (!load_interiors(tables.get("interiors", Dictionary()), state_->buildings.designs.interior, errors)) return errors;
	RenderContext ctx;
	ctx.scenario = get_world_3d()->get_scenario();
	materials_ = materials;
	ctx.material_objects = materials;
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

Dictionary CityCore::new_map(int map, int seed, bool with_city, bool paused) {
	Dictionary d;
	d["ok"] = false;
	if (!catalog_ok_) return d;
	join_save();
	sim_.stop();
	CityGenReport r;
	{
		map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::GENERATION);
		state_->log.drop_damage(); // the old city's blows go with it
		state_->growth.clear();
		state_->interior = InteriorView(); // the layer closes with the city it showed
		r = generate_map(*state_, map, uint32_t(seed), with_city);
		traffic::reset_traffic(*state_); // a new map: full lane graph and an empty fleet
	}
	need_full_rebuild_ = true;
	sim_.start(*state_, paused);
	d["ok"] = true;
	d["nodes"] = r.nodes;
	d["segments"] = r.segments;
	d["lots"] = r.lots;
	d["buildings"] = r.buildings;
	d["squares"] = r.squares;
	d["seconds"] = r.seconds;
	return d;
}

void CityCore::mark_saved() {
	map_hold::WriterHold lk(state_->map_mutex, map_hold::Writer::SAVE);
	state_->saved_revision = state_->revision;
}

void CityCore::set_speed(int speed) {
	sim_.set_speed(SimSpeed(clampi(speed, int(SimSpeed::PAUSED), int(SimSpeed::FASTEST))));
}

int CityCore::get_speed() const { return int(sim_.speed()); }

void CityCore::set_day_hour(double hour) { state_->clock.set_hour(hour); }

// The frame's level-of-detail view (render/detail.h): what a metre spans on this camera and viewport, and the allowed error;
// the engine's own mesh LOD takes the same pixels, so meshes with engine levels and the levels chosen here follow one rule.
DetailView CityCore::detail_view(Camera3D *cam) {
	DetailView d;
	Viewport *vp = get_viewport();
	if (!cam || !vp) return d;
	const Tuning &t = state_->catalog.tune;
	const Vector2 size = vp->get_visible_rect().size;
	const double span = cam->get_keep_aspect_mode() == Camera3D::KEEP_WIDTH ? double(size.x) : double(size.y);
	d.eye = cam_;
	if (cam->get_projection() == Camera3D::PROJECTION_ORTHOGONAL) d.ortho_px_per_m = span / double(cam->get_size());
	else d.px_per_m_at_1m = span / (2.0 * std::tan(Math::deg_to_rad(double(cam->get_fov())) / 2.0));
	d.error_px = t.render_detail_error_px;
	vp->set_mesh_lod_threshold(float(d.error_px));
	return d;
}

// Drained on the main thread, so the lines come in order and in the test's own output; an engine ERROR: line fails the test
// (tools/tasks/verdicts.py) at the frame after the command or the ticks that broke a record, not only at its end.
void CityCore::report_broken() {
	Array found;
	{
		checks::BrokenSink &sink = state_->broken_sink;
		std::lock_guard<std::mutex> lk(sink.mutex);
		if (sink.broken.records().empty() && sink.broken.dropped() == 0) return;
		append_broken(found, sink.broken, &sink.causes);
		sink.clear();
	}
	for (int i = 0; i < found.size(); i++) {
		const Dictionary d = found[i];
		UtilityFunctions::push_error("records broken: ", d["feature"], ".", d["check"], " ", d["records"], " (after ", d["after"], "; ",
				d["dropped"], " more past the room)");
	}
}

void CityCore::_process(double delta) {
	if (Engine::get_singleton()->is_editor_hint()) return;
	if constexpr (CHECKS) report_broken();
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
	const Vector3 look = cam ? ground_seen(*cam) : cam_;
	state_->camera_x.store(look.x);
	state_->camera_z.store(look.z);
	bool fresh = false;
	const PoseFrame &poses = state_->poses.acquire(fresh);
	const int64_t now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	// a moment render.pose_delay_ticks behind the clock, inside the published trail even when a tick lands late
	const PoseSample at = poses.sample(now - int64_t(state_->catalog.tune.render_pose_delay_ticks * double(poses.period_ns)));
	note_pose(poses, fresh, at);
	ViewCull view;
	if (cam) view.read(*cam);
	render_.set_view(detail_view(cam));
	render_.update_agents(poses, cam_, view, at); // no map lock: the frame is the published snapshot
	const int64_t published = poses.stamp_ns[POSE_TRAIL - 1];
	const double ticks = fresh && poses.period_ns > 0 && wear_stamp_ns_ > 0 ? double(published - wear_stamp_ns_) / double(poses.period_ns) : 0;
	if (fresh) wear_stamp_ns_ = published;
	render_.update_wear(poses, fresh, float(ticks / state_->catalog.tune.sim_tick_hz), cam_, float(delta));
	render_.update_signals(poses, fresh);
	render_.update_lamps(cam_, view, float(delta));
	agents_ms_ = laps.lap();
	if (need_full_rebuild_) {
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex); // reads the map; the log has its own mutex
		render_.full_rebuild(*state_, state_->log, terrain3d());
		need_full_rebuild_ = false;
	}
	render_.sync(*state_, state_->log, terrain3d(), cam_, delta); // takes the map shared (never exclusive: render_world.h)
	laps.lap();
	{
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
		render_.update_overlays(*state_, poses, delta);
	}
	overlays_ms_ = laps.lap();
	process_ms_ = total.lap();
}

void CityCore::set_levels(float traffic, float people, float demand) { // atomics the traffic reads: no wait for its step
	TrafficLevels &l = state_->traffic_levels;
	l.traffic = traffic;
	l.people = people;
	l.demand = demand;
}

void CityCore::set_sim_switch(const String &name, bool on) {
	TrafficLevels &l = state_->traffic_levels;
	if (name == "traffic") l.vehicles_on = on;
	else if (name == "pedestrians") l.people_on = on;
	else if (name == "growth") l.growth_on = on;
	else if (name == "crowd") l.crowd_on = on;
}

void CityCore::set_detail_scale(float scale) { render_.set_detail_scale(scale); }
void CityCore::set_lamp_lights(int count, int casting, int heads) { render_.set_lamp_budget(count, casting, heads); }
void CityCore::set_lamp_night(float night) { render_.set_lamp_night(night); }
void CityCore::set_tree_share(float share) { render_.set_tree_share(share); }
void CityCore::set_agent_detail(float cabin_m, int cabin_cap, int people_floor) { render_.set_agent_detail(cabin_m, cabin_cap, people_floor); }
void CityCore::set_population(float share) {
	state_->traffic_levels.population = share;
}

void CityCore::set_upload_every(int frames) { render_.set_upload_every(frames); }

void CityCore::set_lots_drawn(bool on) { render_.set_lots_drawn(on); } // the buildings' levels that draw lots, nothing else

void CityCore::set_overlay(const String &name, bool on) {
	for (int i = 0; i < int(Overlay::COUNT); i++)
		if (name == OVERLAY_NAMES[i]) render_.set_overlay(Overlay(i), on);
}

void CityCore::show_every_zone(bool on) {
	render_.show_every_zone(on);
}

Dictionary CityCore::overlays() const {
	Dictionary d;
	for (int i = 0; i < int(Overlay::COUNT); i++) d[OVERLAY_NAMES[i]] = render_.overlay_on(Overlay(i));
	return d;
}

bool CityCore::step_ticks(int ticks) {
	return sim_.step(*state_, ticks);
}

} // namespace city
