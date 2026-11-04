// CityCore: the GDExtension facade. Only project/game/store/city_store.gd talks to it (gate UM_DONO_DO_CORE).
// It owns the CityState (single source of truth), the simulation thread and the render world.
#pragma once

#include "render/render_world.h"
#include "sim/buildings/b40/record.h"
#include "sim/roads/junction.h"
#include "sim/simulation/simulation.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace city {

// A building's volume on its lot: the lot's frame, the rectangle there and the height of its top over the ground (clear_reach).
struct Solid {
	b40::LotFrame f;
	float x0, y0, x1, y1, top;
};

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
	void set_levels(float traffic, float people, float demand); // the Simulation panel: shares of the city's own density
	void set_sim_switch(const godot::String &name, bool on); // the Effects panel: "traffic", "pedestrians", "growth"
	void set_detail_scale(float scale); // the graphics tier's reach of the buildings' near level
	void set_vegetation(float share); // the tier's share of the wild trees
	void set_agent_detail(float cabin_m, int cabin_cap, int people_floor); // the tier's reach and cap of the cabins, the people's nearest level
	void set_population(float share); // the Optimisations tab: the share of the fleet and the walkers allowed
	void set_upload_every(int frames); // the Optimisations tab: mesh uploads only every N frames
	void set_lots_drawn(bool on); // the Effects panel: buildings without their lots (a full redraw)
	godot::Dictionary traffic_metrics(); // the Simulation panel: mean speed, vehicles standing
	void set_day_hour(double hour);
	godot::Dictionary body_info(int id) const;
	godot::Dictionary body_pick(const godot::Vector3 &origin, const godot::Vector3 &dir) const;
	// Build > Models (city_core_models.cpp): a body in hand travels as bytes.
	godot::Array model_list() const;
	godot::PackedByteArray model_bytes(const godot::String &id) const;
	godot::Dictionary doc_body(const godot::Dictionary &doc) const;
	godot::Dictionary building_bytes(int id) const;
	godot::Vector3 body_size(const godot::PackedByteArray &bytes) const; // width, depth, height
	godot::Dictionary place_fit(const godot::Vector3 &at, const godot::Vector2 &size, double rot) const;
	godot::String place_check(const godot::PackedByteArray &bytes, double x, double z, double rot) const;
	godot::Dictionary place_model(const godot::PackedByteArray &bytes, double x, double z, double rot, int fn, int zone);
	int stage_show(const godot::PackedByteArray &bytes, const godot::RID &scenario, bool ghost);
	void stage_move(int handle, const godot::Transform3D &at);
	void stage_free(int handle);
	int stage_triangles(int handle) const;
	godot::PackedVector3Array body_outline(int id, int vol) const;
	godot::Dictionary body_storeys(int id, int vol, int count);
	godot::Dictionary body_insert_storey(int id, int vol, int at, bool above);
	godot::Dictionary body_resize(int id, int vol, int side, double delta, bool snap);
	godot::Dictionary body_add_wing(int id, int vol, int side, double depth, double length);
	godot::Dictionary body_add_setback(int id, int vol, double inset, int storeys);
	godot::Dictionary body_split(int id, int vol, int level);
	godot::Dictionary body_remove_volume(int id, int vol);
	godot::Dictionary body_heights(int id, double ground_h, double storey_h);
	int body_bay(int id, int vol, int storey, int side, int bay) const;
	godot::Dictionary body_set_bay(int id, int vol, int storey, int side, int bay, int what, int scope);
	godot::Dictionary body_set_zone(int id, int vol, int side, int storey0, int storey1, int bay0, int bay1, int what);
	godot::Dictionary body_set_pattern(int id, int vol, int storey, int side, int pattern);
	godot::Dictionary body_set_face(int id, int vol, int side, const godot::Dictionary &patch);
	godot::Dictionary body_set_relief(int id, int vol, const godot::Dictionary &region, bool snap);
	godot::Dictionary body_set_roof(int id, int vol, int roof);
	godot::Dictionary body_roof_shape(int id, int vol, double pitch, int ridge, int fall);
	godot::Dictionary body_paint(int id, int scope, int vol, int side, int storey, int finish, const godot::String &colour);
	godot::Dictionary body_add_gear(int id, int vol, const godot::Vector3 &at, const godot::Dictionary &spec);
	godot::Dictionary body_remove_gear(int id, int vol, int gear);
	godot::Dictionary body_turn_gear(int id, int vol, int gear);
	godot::Dictionary body_add_part(int id, int vol, int storey, int side, int bay, const godot::Dictionary &spec);
	godot::Dictionary body_add_part_at(int id, const godot::Vector3 &at, const godot::Dictionary &spec);
	godot::Dictionary body_remove_part(int id, int part);
	godot::Dictionary body_turn_part(int id, int part);
	godot::Dictionary body_repeat_part(int id, int part, double spacing);
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

	// --- the Landscape drawer (city_core_nature.cpp)
	godot::Dictionary set_weather(const godot::Dictionary &change);
	godot::Dictionary add_cloud(const godot::Dictionary &c);
	godot::Dictionary edit_cloud(int id, const godot::Dictionary &c);
	godot::Dictionary remove_cloud(int id);
	godot::Dictionary scatter_clouds(int count, const godot::Dictionary &like, float variation);
	godot::Dictionary clear_clouds();
	int cloud_at(const godot::Vector3 &origin, const godot::Vector3 &direction, float drift_x, float drift_z) const;
	godot::Dictionary fog_brush(const godot::Vector3 &p, float radius, float strength, float height, float speed, bool erase);
	godot::Dictionary clear_fog();
	godot::Dictionary paint_brush(const godot::Vector3 &p, float radius, float strength, float hardness, int kind);
	godot::Dictionary clear_gullies();
	godot::Dictionary tree_brush(const godot::Vector3 &p, float radius, int mode, int kind, const godot::Dictionary &brush);
	godot::Dictionary clear_trees();
	godot::Dictionary element_brush(const godot::Vector3 &p, float radius, int kind, const godot::Dictionary &brush, bool erase);
	godot::Dictionary clear_elements();
	godot::Array effects_near(const godot::Vector3 &p, float reach, int first_effect) const;
	godot::Dictionary set_biome(int region);
	godot::Dictionary clear_relief();
	godot::Dictionary landscape() const;
	int64_t sky_revision() const;
	godot::Dictionary fog_field(const godot::Vector3 &camera);

	// --- undo / redo and the drawer edits (city_core_edit.cpp)
	bool undo();
	bool redo();
	void begin_stroke(const godot::String &label); // a held brush: every dab until end_stroke is one undo step
	void end_stroke();
	godot::Dictionary split_road(const godot::Vector3 &p);
	godot::Array recent_edits(int count); // the F9 World tab and the F8 marks: the latest undo steps, newest first
	godot::Dictionary reverse_road(int seg); // the inspector: the one-way runs the other way
	godot::Dictionary duplicate_road(int seg); // a copy alongside (Ctrl+D)
	godot::Dictionary set_marking_style(int style, bool undoable); // Menu > Paint; false: a city coming back from its session
	godot::Dictionary upgrade_road(const godot::Vector3 &p, int profile, bool whole_street);
	godot::Dictionary roundabout(const godot::Vector3 &p, float radius, bool cut);
	godot::Dictionary preview_roundabout(const godot::Vector3 &p, float radius, bool cut);
	godot::Dictionary segment_at(const godot::Vector3 &p); // the segment a click there edits (preview)
	godot::Dictionary terrain_brush(const godot::Vector3 &p, float radius, float strength, float dt, int mode, float hardness);
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
	godot::PackedVector3Array segment_path(int seg); // the road's line in the world, for what the Information tool draws
	godot::Dictionary segment_traffic(int seg); // the Inspect tool's live numbers (city_core_inspect.cpp)
	godot::Dictionary node_traffic(int node);
	godot::Array impossible_nodes(); // the top bar's warning: nodes whose legs meet too sharply
	float clear_reach(const godot::Vector3 &from, const godot::Vector3 &to, int samples); // the camera's line of sight
	godot::Dictionary probe_building(int id);
	godot::Array list_buildings();
	godot::Dictionary traffic_probe(int sample); // counts, instances drawn last frame and `sample` vehicle poses
	// Tests: the amber's decision for a vehicle class in numbers (traffic::amber_stops), the amber a speed in km/h asks.
	bool amber_stops(float v, float stop_m, float top, float left, int type) const;
	float amber_wanted(float kmh) const;
	godot::Dictionary city_bounds() const; // the built area (for cameras and tests)
	godot::TypedArray<godot::Image> film_grade(const godot::Dictionary &grade) const; // slices of the screen's lookup table
	godot::Color film_graded(const godot::Dictionary &grade, const godot::Color &colour) const; // a display colour through it
	void set_overlay(const godot::String &name, bool on); // Layers popover: "zones", "congestion"
	void show_every_zone(bool on);
	godot::Dictionary set_style(const godot::String &id); // the regional style of this city
	godot::Dictionary overlays() const;
	int render_pending() const { return render_.pending(); } // chunks still waiting to be drawn
	int building_chunk(float x, float z) const { return render_.building_chunk(x, z); } // the tests: BuildingRenderer::chunk_flags
	godot::Dictionary render_cost() const; // where the last frame's _process went, in ms (tests tell a hitch's cause)
	godot::Dictionary job_pool_probe() const;
	godot::Dictionary agent_cost() const; // vehicles by type and level: triangles, instances, draws (counts)
	godot::Dictionary pose_stats(); // since the last call: pose publications, their gaps, frames shown past the newest pose

protected:
	static void _bind_methods();
	static void bind_models();
	static void bind_bodies();
	static void bind_nature();

private:
	std::unordered_map<int, std::vector<Solid>> solids_; // clear_reach: each building asked, until the city changes
	uint64_t solids_revision_ = 0;
	std::vector<SharpNode> impossible_; // impossible_nodes, kept for this version of the network
	int impossible_version_ = -1;
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
	int64_t wear_stamp_ns_ = 0; // the last publication the wear field took
	godot::Vector3 cam_;
	double process_ms_ = 0, agents_ms_ = 0, overlays_ms_ = 0; // the last _process (render_cost)
	struct PoseStats {
		int frames = 0, held = 0, behind = 0, publications = 0, early = 0, late = 0;
		int64_t last_tick_ns = 0, gap_min_ns = 0, gap_max_ns = 0;
	} pose_stats_;
	void note_pose(const PoseFrame &f, bool fresh, const PoseSample &at);
};

} // namespace city
