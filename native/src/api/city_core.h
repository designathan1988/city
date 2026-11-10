// CityCore: the GDExtension facade. Only project/game/store/city_store.gd talks to it (gate UM_DONO_DO_CORE).
// It owns the CityState (single source of truth), the simulation thread and the render world.
#pragma once

#include <godot_cpp/variant/packed_vector3_array.hpp>

#include "render/detail.h"
#include "render/render_world.h"
#include "api/city_core_parts.h"
#include "sim/demolition/blast_model.h"
#include "sim/simulation/simulation.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <memory>
#include <mutex>

namespace city {

class CityBuilder;
class CityLandscape;
class CityProbes;
class CityStreet;
class CityTransit;
class CityLab;

class CityCore : public godot::Node3D {
	GDCLASS(CityCore, godot::Node3D)

public:
	CityCore();
	~CityCore() override;

	// --- lifecycle (city_core.cpp)
	godot::PackedStringArray load_catalog(const godot::Dictionary &tables);
	godot::PackedStringArray setup_render(const godot::Dictionary &render_tables, const godot::Dictionary &materials);
	// A new map: map = entry of data/catalog/maps.json; with_city = false makes an empty map. Returns the report.
	godot::Dictionary new_map(int map, int seed, bool with_city, bool paused = false);
	void mark_saved();
	void set_speed(int speed);
	void set_levels(float traffic, float people, float demand); // the Simulation panel: shares of the city's own density
	void set_sim_switch(const godot::String &name, bool on); // the Effects panel: "traffic", "pedestrians", "growth", "crowd"
	void set_detail_scale(float scale); // the graphics tier's reach of the buildings' near level
	void set_lamp_lights(int count, int casting, int heads); // the graphics tier's street lamps with a light of their own, those with a shadow, and the vehicles' headlamps
	void set_lamp_night(float night); // how much of the night it is, as daylight.gd reckons it
	void set_vegetation(float share); // the tier's share of the wild trees
	void set_agent_detail(float cabin_m, int cabin_cap, int people_floor); // the tier's reach and cap of the cabins, the people's nearest level
	void set_population(float share); // the Optimisations tab: the share of the fleet and the walkers allowed
	void set_upload_every(int frames); // the Optimisations tab: mesh uploads only every N frames
	void set_lots_drawn(bool on); // the Effects panel: buildings without their lots (a full redraw)
	godot::Dictionary traffic_metrics(); // the Simulation panel: mean speed, vehicles standing
	void set_day_hour(double hour);
	int get_speed() const;
	void set_terrain_path(const godot::NodePath &p) { terrain_path_ = p; }
	godot::NodePath get_terrain_path() const { return terrain_path_; }
	void _process(double delta) override;
	bool step_ticks(int ticks); // tests: deterministic steps, refused (false) unless the clock is paused or stopped (Simulation::hold_still)

	// --- the areas of the API (city_core_areas.cpp): the Build drawer, the Landscape drawer, public transport and the test probes, each
	// an object of its own that game/store/city_store.gd calls by the names of game/store/store_api.gd ("builder", "landscape", ...).
	godot::Object *area(const godot::StringName &name) const;

	// --- player commands and previews (city_core_commands.cpp)
	godot::Dictionary road_start(const godot::Vector3 &p, double px, bool on, bool grid, double cell, double offset);
	godot::Dictionary road_end(const godot::Vector3 &start, const godot::Vector3 &p, double px, bool on, bool angles, bool grid, double cell, double offset);
	godot::PackedVector3Array road_pieces(const godot::Vector3 &a, const godot::Vector3 &b, int alignment, bool has_control, const godot::Vector3 &c,
			const godot::PackedVector3Array &samples, int profile, double px);
	// from_lift: the height the road was begun at, over the grade line; HEIGHT_IN_HAND for the one the tool draws at now.
	godot::Dictionary preview_road_path(const godot::PackedVector3Array &pieces, int profile, double px, float from_lift), build_road_path(const godot::PackedVector3Array &pieces, int profile, double px, float from_lift);
	godot::Dictionary preview_road(const godot::Vector3 &a, const godot::Vector3 &b, bool has_control, const godot::Vector3 &c, int profile, double px, float from_lift);
	godot::Dictionary build_road(const godot::Vector3 &a, const godot::Vector3 &b, bool has_control, const godot::Vector3 &c, int profile, double px, float from_lift);
	godot::Dictionary demolish(const godot::Vector3 &p);
	godot::Dictionary demolish_box(const godot::Vector3 &a, const godot::Vector3 &b); // everything in the box, one undo step
	godot::Dictionary paint_zone(const godot::Vector3 &p, float radius, int zone);
	godot::Dictionary paint_zone_path(const godot::PackedVector3Array &points, float radius, int zone);
	godot::Array road_profiles() const; // the catalogue as the drawer shows it (derived width and price per metre)
	int road_variant(int base, int lanes, float width, int parking);
	godot::Array node_legs(int node);
	godot::Dictionary set_approach_rule(int node, int seg, int rule);
	godot::Dictionary node_lanes(int node), toggle_lane_link(int node, int from, int from_lane, int to, int to_lane);
	godot::Dictionary road_spec(int profile) const, road_spec_limits() const;
	godot::Array road_spec_issues(int base, const godot::Dictionary &spec) const;
	int road_spec_make(int base, const godot::Dictionary &spec);

	// --- the bomb (city_core_strike.cpp)
	godot::Dictionary strike(const godot::Vector3 &p, int strength); // p.y: the roof it fell on, or the ground
	float effects_shake() const { return render_.shake(); } // metres the blasts move the camera now
	float effects_since_blast() const { return render_.since_blast(); } // effect seconds since the last blast (-1 none)
	godot::PackedInt64Array ruin_seeds() const;
	int effects_ruin_pieces() const { return render_.ruin_pieces(); } // of the buildings drawn in pieces, and of those in the air
	int effects_ruin_flying() const { return render_.ruin_pieces_flying(); }
	int64_t effects_ruin_digest(int64_t seed) const { return int64_t(render_.ruin_digest(uint32_t(seed))); } // where a ruin's pieces lie
	int effects_bodies() const { return render_.bodies(); } // of people off their feet, and of those still falling
	int effects_bodies_moving() const { return render_.bodies_moving(); }
	godot::Array effects_bodies_falling() const { return render_.bodies_falling(); } // where, how fast, how long left: what a test says of a body that never lands
	// The pistol along a line of sight: {ok, hit 0 none / 1 walker / 2 vehicle, killed, burst}; a car that took enough rounds bursts as a blow.
	godot::Dictionary shoot(const godot::Vector3 &origin, const godot::Vector3 &direction);
	godot::Dictionary damage_probe() const; // what blows left on the map (counts)
	// The first `limit` walkers and vehicles as the pistol sees them (tests, probes): id, position, health, fallen, ...
	godot::Dictionary shot_targets(int limit) const;
	godot::Dictionary blast_probe(float strength, float distance) const; // the blast model's numbers at a distance

	// --- undo / redo and the drawer edits (city_core_edit.cpp)
	bool undo(), redo();
	void begin_stroke(const godot::String &label); // a held brush: every dab until end_stroke is one undo step
	void end_stroke();
	godot::Dictionary split_road(const godot::Vector3 &p);
	godot::Dictionary body_census(int kind);
	godot::Dictionary road_census();
	godot::Dictionary lot_audit(); // the lots' defects read from the state (city_core_audit.cpp)
	godot::Dictionary set_node_height(int node, float metres);
	godot::Dictionary bend_road(int seg, float height, float at);
	godot::Dictionary join_roads(int node);
	godot::Dictionary add_crossing(const godot::Vector3 &p, int kind);
	godot::Dictionary remove_crossing(int node);
	godot::Dictionary add_uturn(const godot::Vector3 &p, bool toward_b);
	godot::Dictionary set_movement_banned(int node, int from, int to, bool banned);
	godot::Dictionary remove_node(int node);
	godot::Dictionary set_road_end(int node, bool bulb);
	godot::Dictionary move_node(int node, const godot::Vector3 &p);
	godot::Dictionary set_node_control(int node, int ctl);
	godot::Array junctions();
	godot::Dictionary node_at(const godot::Vector3 &p, float px);
	godot::Array moved_roads(int node, const godot::Vector3 &p);
	godot::Array recent_edits(int count); // the F9 World tab and the F8 marks: the latest undo steps, newest first
	godot::Dictionary lot_add_rect(const godot::Vector3 &a, const godot::Vector3 &b, double angle), lot_add_polygon(const godot::PackedVector3Array &points);
	godot::Dictionary lot_move_corner(const godot::Vector3 &from, const godot::Vector3 &to), lot_curve_side(const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &through);
	godot::Dictionary lot_set_front(int id, int side), lot_split(int id, int kind, int parts, const godot::Vector3 &a, const godot::Vector3 &b);
	godot::Dictionary lot_join(int a, int b), lot_delete(int id), lot_zone(int id, int zone);
	godot::Dictionary lot_erase(const godot::PackedVector3Array &stroke); // the delete stroke: its lots and the buildings under it
	godot::PackedVector3Array lot_land(const godot::Vector3 &a, const godot::Vector3 &b, double angle);
	godot::Vector3 lot_snap_point(const godot::Vector3 &p, double reach, const godot::Vector3 &apart);
	godot::Array lot_proposals(const godot::Vector3 &p);
	int lot_at(const godot::Vector3 &p);
	double lot_angle_at(const godot::Vector3 &p);
	godot::Array lots();
	godot::PackedVector3Array lot_cut_lines(int id, int kind, int parts, const godot::Vector3 &a, const godot::Vector3 &b);
	godot::Dictionary build_blocks(const godot::Vector3 &at, int cols, int rows, double block_m, double angle, int profile);
	godot::Array block_lines(const godot::Vector3 &at, int cols, int rows, double block_m, double angle, double step_m);
	godot::Dictionary reverse_road(int seg), duplicate_road(int seg); // the inspector: the one-way runs the other way; a copy alongside (Ctrl+D)
	godot::Dictionary set_marking_style(int style, bool undoable); // Menu > Paint; false: a city coming back from its session
	godot::Dictionary upgrade_road(const godot::Vector3 &p, int profile, bool whole_street);
	godot::Dictionary roundabout(const godot::Vector3 &p, float radius), preview_roundabout(const godot::Vector3 &p, float radius);
	godot::Dictionary segment_at(const godot::Vector3 &p); // the segment a click there edits (preview)
	godot::Dictionary terrain_brush(const godot::Vector3 &p, float radius, float strength, float dt, int mode, float hardness);
	godot::Dictionary draw_building(const godot::Vector3 &a, const godot::Vector3 &b, int fn, int zone, int shape);
	godot::Dictionary preview_lot(const godot::Vector3 &a, const godot::Vector3 &b);
	godot::Dictionary set_building(int id, int fn, int floors);

	// --- save / open (city_core_persist.cpp)
	godot::Dictionary save_city(const godot::String &path);
	godot::Dictionary save_status(); // { busy, ok, error, bytes } of the background save
	// Opening runs aside: load_city starts it ({ pending } or a refusal), load_poll answers { pending } until the city is read, then
	// takes it in and answers as the opening did ({ ok, error, ms }).
	godot::Dictionary load_city(const godot::String &path, bool paused = false);
	godot::Dictionary load_poll();

	// --- queries (city_core_query.cpp)
	godot::Dictionary snapshot();
	int64_t map_revision(); // the unsaved-changes revision: a camera at rest re-asks nothing while it holds
	godot::Dictionary constants() const;
	godot::Dictionary raycast(const godot::Vector3 &origin, const godot::Vector3 &dir);
	float height_at(float x, float z); // terrain height without a ray march (camera, ground queries)
	godot::PackedFloat32Array heights_at(const godot::PackedVector2Array &points); // the same at each point (x, z), under one lock
	godot::Dictionary inspect(const godot::Vector3 &p);
	godot::PackedVector3Array segment_path(int seg); // the road's line in the world, for what the Information tool draws
	godot::Dictionary segment_traffic(int seg), node_traffic(int node); // the Inspect tool's live numbers (city_core_inspect.cpp)
	godot::Array impossible_nodes(); // the top bar's warning: nodes whose legs meet too sharply
	float clear_reach(const godot::Vector3 &from, const godot::Vector3 &to, int samples); // the camera's line of sight (city_core_sight.cpp)
	float sight_pull(const godot::Vector3 &focus, const godot::Vector3 &eye, int samples, float soft);
	godot::PackedVector3Array eye_free(const godot::Vector3 &focus, const godot::Vector3 &eye, float lift);
	godot::Dictionary probe_building(int id);
	godot::Array list_buildings();
	godot::Dictionary traffic_probe(int sample); // counts, instances drawn last frame and `sample` vehicle poses
	godot::Dictionary set_signal(int node, bool fixed, const godot::PackedFloat32Array &greens, float offset);
	godot::Dictionary green_wave(int node);
	godot::Dictionary city_bounds() const; // the built area (for cameras and tests)
	godot::TypedArray<godot::Image> film_grade(const godot::Dictionary &grade) const; // slices of the screen's lookup table
	godot::Color film_graded(const godot::Dictionary &grade, const godot::Color &colour) const; // a display colour through it
	void set_overlay(const godot::String &name, bool on); // Layers popover: "zones", "congestion"
	// The Interior layer (city_core_interior.cpp): buildings cut open at a floor, for a camera looking along step `dir`.
	godot::Dictionary set_interior(bool on, int target, int level, int dir);
	godot::Dictionary interior() const; // on, target, level, dir, revision, floors, lamps of the floor seen (not for every frame)
	int interior_revision() const; // cheap: changes with every command and resets with the city (the script reads `interior` when it does)
	godot::Dictionary interior_cost(int id, int level, int dir) const; // triangles, boxes and bytes of a building's inside (tests, render_cost)
	void show_every_zone(bool on);
	godot::Dictionary set_style(const godot::String &id); // the regional style of this city
	godot::Dictionary overlays() const;
	int render_pending() const { return render_.pending(); } // chunks still waiting to be drawn
	godot::Dictionary building_chunk(float x, float z) const { return render_.building_chunk(x, z); } // the tests: BuildingRenderer::chunk_view
	godot::Dictionary render_cost() const; // where the last frame's _process went, in ms (tests tell a hitch's cause); constant time
	godot::Dictionary wear_probe() const; // the wear field's worn cells, scanned whole (captures; never per frame)
	godot::Dictionary road_mesh_counts() const; // what the road and ground seam chunks hold and the wear field's bytes
	godot::Dictionary road_surface_probe() const; // the roads' stations over the map and how far their mesh stands off the true road (tests)
	godot::Dictionary road_seam_probe(); // the ground seams of every road chunk, built anew: cells, cuts, refusals, cover (tests)
	godot::Dictionary prop_buffer_probe() const; // each furniture chunk's poles and wires as the GPU holds them, by digest (two runs compared)
	void draw_seams(bool on) { render_.draw_seams(on); } // tests/perf/camera_pan: a frame without them
	void wear_paint(float x0, float z0, float x1, float z1, float value) { render_.wear_paint(x0, z0, x1, z1, value); } // tests/visual: a set use
	godot::Dictionary building_cost() const { return render_.building_cost(); } // the building levels by surface: triangles, bytes
	godot::Dictionary agent_cost() const; // vehicles by type and level: triangles, instances, draws (counts)
	godot::Dictionary surface_probe(const godot::Array &arrays, const godot::Dictionary &lods, int64_t format) const; // tests: render/surface_data.h's surface of these arrays
	godot::Dictionary memory_census() const; // where the process's memory is: its heaps and the core's largest owners, MiB (log/owner-review.md §8)
	int memory_release(); // every thread of the core gives back what its heap holds freed: how many did (tests/perf/memory_probe)
	godot::Dictionary memory_churn(int mib, int keep_one_in); // a thread takes, frees and ends: what the allocator held before, at most, thinned and after (tests/core/memory_test)
	godot::Dictionary pose_stats(); // since the last call: pose publications, their gaps, frames shown past the newest pose

protected:
	static void _bind_methods();
	// What _bind_methods binds of the core itself: damage, edits and queries (the areas bind their own).
	static void bind_damage(), bind_edits(), bind_queries();

private:
	InspectCache inspect_;
	SaveJob save_;
	godot::Object *terrain3d() const;
	void join_save(); // waits for a save in flight before the state changes or is destroyed
	LoadJob load_;
	void join_load(); // waits for an opening in flight; what it read is dropped (another city or the end comes first)
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
	DetailView detail_view(godot::Camera3D *cam);
	double process_ms_ = 0, agents_ms_ = 0, overlays_ms_ = 0; // the last _process (render_cost)
	PoseStats pose_stats_;
	void note_pose(const PoseFrame &f, bool fresh, const PoseSample &at);
	// The areas, made with the core on its state and render, freed before them (city_core_areas.cpp).
	CityBuilder *builder_ = nullptr;
	CityLandscape *landscape_ = nullptr;
	CityTransit *transit_ = nullptr;
	CityProbes *probes_ = nullptr;
	CityStreet *street_ = nullptr;
	CityLab *lab_ = nullptr;
	void make_areas(), free_areas();
};

} // namespace city
