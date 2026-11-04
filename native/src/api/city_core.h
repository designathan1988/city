// CityCore: the GDExtension facade. Only project/game/store/city_store.gd talks to it (gate UM_DONO_DO_CORE).
// It owns the CityState (single source of truth), the simulation thread and the render world.
#pragma once

#include "render/render_world.h"
#include "sim/simulation/simulation.h"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/node_path.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace city {

class CityCore : public godot::Node3D {
	GDCLASS(CityCore, godot::Node3D)

public:
	CityCore();
	~CityCore() override;

	// --- lifecycle (city_core.cpp)
	godot::PackedStringArray load_catalog(const godot::Dictionary &tables);
	godot::PackedStringArray setup_render(const godot::Dictionary &render_tables, const godot::Dictionary &materials);
	// A new map: map = entry of data/catalog/maps.json; with_city = false makes an empty map. Returns the report.
	godot::Dictionary new_map(int map, int seed, bool with_city);
	void mark_saved();
	void set_speed(int speed);
	int get_speed() const;
	void set_terrain_path(const godot::NodePath &p) { terrain_path_ = p; }
	godot::NodePath get_terrain_path() const { return terrain_path_; }
	void _process(double delta) override;
	void step_ticks(int ticks); // tests: deterministic steps with the clock paused

	// --- player commands and previews (city_core_commands.cpp)
	godot::Dictionary snap_road(const godot::Vector3 &p, int profile);
	godot::Dictionary preview_road(const godot::Vector3 &a, const godot::Vector3 &b, bool has_control, const godot::Vector3 &c, int profile);
	godot::Dictionary build_road(const godot::Vector3 &a, const godot::Vector3 &b, bool has_control, const godot::Vector3 &c, int profile);
	godot::Dictionary demolish(const godot::Vector3 &p);
	godot::Dictionary paint_zone(const godot::Vector3 &p, float radius, int zone);
	godot::Array road_profiles() const;

	// --- the Street tab (city_core_street.cpp)
	godot::Dictionary preview_item(const godot::Vector3 &p, int kind, float reach);
	godot::Dictionary place_item(const godot::Vector3 &p, int kind, int sign, const godot::String &text, float reach);
	godot::Dictionary place_item_row(const godot::Vector3 &a, const godot::Vector3 &b, int kind, float every_m);
	godot::Dictionary preview_barrier(const godot::PackedVector3Array &pts, const godot::Vector3 &cursor, int kind);
	godot::Dictionary build_barrier(const godot::PackedVector3Array &pts, int kind);
	godot::Dictionary remove_street(const godot::Vector3 &p, float reach);
	godot::Dictionary preview_pole_run(const godot::Vector3 &a, const godot::Vector3 &b, float reach, int lamps);
	godot::Dictionary build_pole_run(const godot::Vector3 &a, const godot::Vector3 &b, float reach, int lamps);
	void show_pole_preview(const godot::Array &poles);
	void set_road_furniture(int level); // what new roads are laid with: 0 complete, 1 basic, 2 none
	int road_furniture();
	godot::Array sign_cells(); // the plates to paint (project/render/sign_sheet.gd)
	godot::Dictionary street_counts();

	// --- undo / redo and the drawer edits (city_core_edit.cpp)
	bool undo();
	bool redo();
	void begin_stroke(const godot::String &label); // a held brush: every dab until end_stroke is one undo step
	void end_stroke();
	godot::Dictionary split_road(const godot::Vector3 &p);
	godot::Dictionary upgrade_road(const godot::Vector3 &p, int profile, bool whole_street);
	godot::Dictionary roundabout(const godot::Vector3 &p, float radius, bool cut);
	godot::Dictionary preview_roundabout(const godot::Vector3 &p, float radius, bool cut);
	godot::Dictionary segment_at(const godot::Vector3 &p); // the segment a click there edits (preview)
	godot::Dictionary terrain_brush(const godot::Vector3 &p, float radius, float strength, float dt, int mode);
	godot::Dictionary draw_building(const godot::Vector3 &a, const godot::Vector3 &b, int fn, int zone);
	godot::Dictionary preview_lot(const godot::Vector3 &a, const godot::Vector3 &b);
	godot::Dictionary set_building(int id, int fn, int floors);

	// --- save / open (city_core_persist.cpp)
	godot::Dictionary save_city(const godot::String &path);
	godot::Dictionary save_status(); // { busy, ok, error, bytes } of the background save
	godot::Dictionary load_city(const godot::String &path); // the catalogue as the drawer shows it (derived width and price per metre)

	// --- queries (city_core_query.cpp)
	godot::Dictionary snapshot();
	godot::Dictionary constants() const;
	godot::Dictionary raycast(const godot::Vector3 &origin, const godot::Vector3 &dir);
	float height_at(float x, float z); // terrain height without a ray march (camera, ground queries)
	godot::Dictionary inspect(const godot::Vector3 &p);
	godot::Dictionary probe_building(int id);
	godot::Array list_buildings();
	godot::Dictionary traffic_probe(int sample); // counts, instances drawn last frame and `sample` vehicle poses
	// Tests: the amber's decision for a vehicle class in numbers (traffic::amber_stops), the amber a speed in km/h asks.
	bool amber_stops(float v, float stop_m, float top, float left, int type) const;
	float amber_wanted(float kmh) const;
	godot::Dictionary city_bounds() const; // the built area (for cameras and tests)
	void set_overlay(const godot::String &name, bool on); // Layers popover: "zones", "congestion"
	godot::Dictionary overlays() const;
	int render_pending() const { return render_.pending(); } // chunks still waiting to be drawn
	godot::Dictionary render_cost() const; // where the last frame's _process went, in ms (tests tell a hitch's cause)
	godot::Dictionary agent_cost() const; // vehicles by type and level: triangles, instances, draws (counts)
	godot::Dictionary pose_stats(); // since the last call: pose publications, their gaps, frames shown past the newest pose

protected:
	static void _bind_methods();

private:
	godot::Object *terrain3d() const;
	void join_save(); // waits for a save in flight before the state changes or is destroyed
	std::thread save_thread_;
	std::atomic<int> save_state_{ 0 }; // 0 idle, 1 writing, 2 ok, 3 error
	std::atomic<int64_t> save_bytes_{ 0 };
	std::string save_error_; // set by the save thread before save_state_ turns 3, read only then
	// The snapshot's expensive counts follow the player's edits, not the simulation tick.
	uint64_t snap_edit_revision_ = ~uint64_t(0);
	int snap_nodes_ = 0, snap_zoned_ = 0, snap_residents_ = 0, snap_jobs_ = 0;
	std::unique_ptr<CityState> state_;
	Simulation sim_;
	// The materials the render draws with (render/city_world.gd makes them): held while its meshes reference them, so
	// they are freed after render_ (declared first), never under a mesh still drawing.
	godot::Dictionary materials_;
	RenderWorld render_;
	godot::NodePath terrain_path_;
	bool catalog_ok_ = false, render_ok_ = false, need_full_rebuild_ = false;
	godot::Vector3 cam_;
	double process_ms_ = 0, agents_ms_ = 0, overlays_ms_ = 0; // the last _process (render_cost)
	struct PoseStats {
		int frames = 0, held = 0, behind = 0, publications = 0, early = 0, late = 0;
		int64_t last_tick_ns = 0, gap_min_ns = 0, gap_max_ns = 0;
	} pose_stats_;
	void note_pose(const PoseFrame &f, bool fresh, const PoseSample &at);
};

} // namespace city
