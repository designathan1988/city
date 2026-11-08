// Vehicles and pedestrians near the camera: per vehicle type two MultiMeshes, near (the cabin behind see-through glass)
// and far (no cabin, the glass opaque), refilled every frame from the poses the simulation published (PoseFrame) and
// interpolated between the last two simulation ticks (wheels roll by a render-side odometer); pedestrians through PeopleRenderer. Materials "agent"
// and, on the near glass, "agent_glass" (data/render/materials.json); meshes, paints, palettes and how full each type
// runs from data/render/agents.json.
#pragma once

#include "render/agents/agent_meshes.h"
#include "render/agents/agent_shapes.h"
#include "render/agents/head_lights.h"
#include "render/agents/fleet_shadow.h"
#include "render/view_cull.h"
#include "render/agents/people_renderer.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace city {

constexpr int CAR_MODELS = int(VehicleType::MOTO); // sedan .. pickup come from agents.json "cars", then the wagon
constexpr int WAGON_MODEL = VEHICLE_TYPES, AGENT_MODELS = VEHICLE_TYPES + 1; // a model per type and the sedans' estate

class AgentRenderer {
public:
	const std::array<AgentWreck, AGENT_MODELS> &wrecks() const { return wrecks_; }
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	// `taken`: the walkers and vehicles a blow took since the newest publication, which still holds them (taken_agents.h).
	void update(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const PoseSample &at, const TakenAgents &taken);
	int drawn_vehicles() const { return drawn_vehicles_; }
	int drawn_people() const { return drawn_people_; }
	godot::Array people_clips() const { return people_.clips(); }
	PeopleRenderer &people() { return people_; }
	bool show_people_clip(const godot::String &name) { return people_.show_clip(name); }
	// Per vehicle type and level: the triangles of its body and glass surfaces, the instances drawn last frame and the
	// draws (surfaces of the shown MultiMeshes): counts, no clock (city_core_probe agent_cost).
	godot::Dictionary cost() const;

	// The graphics tier: the reach and the cap of the cabin level, and the nearest level people may use.
	void set_detail(float cabin_m, int cabin_cap, int people_floor) {
		cabin_reach_ = cabin_m;
		cabin_cap_ = cabin_cap;
		cabin_limit2_ = 0;
		people_.set_floor(people_floor);
	}
	int cabins_drawn() const;
	void set_head_lights(int count) { heads_.set_budget(count); }
	void set_night(float night) { heads_.set_night(night); }
	void shine_heads(float frame_s) { heads_.shine(frame_s); }
	int lit_heads() const { return heads_.lit(); }

private:
	HeadLights heads_;
	AgentShapes shapes_; // every size the part builders draw with (agents.json "shapes"): the meshes are built from it at init
	std::array<AgentWreck, AGENT_MODELS> wrecks_;
	float level_hold_ = 0; // of a level's edge: how far past it a vehicle or a walker changes level
	static constexpr int CABIN_LEVEL = AGENT_CABIN_LEVEL, BODY_LEVEL = AGENT_BODY_LEVEL, FAR_LEVEL = AGENT_FAR_LEVEL, LEVELS = AGENT_LEVELS;
	// One more than the built levels: the cabin simplified, glass and people still seen, past cabin_close_m (agent_lod.h).
	static constexpr int MID_DRAW = LEVELS, DRAWS = LEVELS + 1;
	static constexpr uint64_t VEHICLE_DENSE_KEYS = 1u << 20;
	static constexpr uint64_t ROW_KINDS = 2, PARKED_ROW = 1; // a key says whose id it is: the fleet's and the bays' both count from 0
	// Who rides in one type: its passenger seats, the cumulative shares of its vehicles in each band of how full they
	// run, and the strides its seats may fill in (1 alone: in their own order, front first).
	struct Load {
		int seats = 0;
		bool shuffle = false;
		std::vector<float> bands;
		std::vector<int> strides;
	};
	// Wheel roll angle of a vehicle slot: metres moved since the last frame over the wheel radius (render-side odometer).
	float roll(int id, const Pose &p, float wheel_r);
	bool read_shapes(const godot::Dictionary &table, godot::PackedStringArray &errors); // agent_looks.cpp
	bool build_meshes(const DataValue &t, godot::PackedStringArray &errors);
	bool push_palettes(const DataValue &t, const std::vector<uint32_t> &lines, godot::PackedStringArray &errors);
	bool read_loads(const DataValue &loads, godot::PackedStringArray &errors);
	// A vehicle's passengers and the order its seats fill in (stride * seats + first), from its id.
	void riders(int type, uint32_t id, float &passengers, float &order) const;
	// The frame's level edges, distances squared (level_rule, from the tier's reach and the table's radii), and the hold past them.
	struct LevelRule {
		float reach, r2, near2, far2, people_r2, close2, hold2;
		int plain_level(float d2) const; // the level at d2, with no hold
		static int rank(int level); // the levels from the nearest: cabin, mid, body, far
	};
	LevelRule level_rule() const;
	int level_of(const LevelRule &rule, uint64_t key, float d2) const; // the plain level, or the one the vehicle had while within the hold
	void count_for_cap(const LevelRule &rule, int level, float d2); // a seen vehicle in the race for the cabins (near_d2_)
	bool in_view(const ViewCull &view, int model, const Pose &p, float d2) const; // goes to the camera's lists
	// The frame's rows of the fleet and of the bays' cars: the camera's by key (keeper_), the sun's in order; `placed` counts the seen.
	void place_vehicles(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const PoseSample &at, const TakenAgents &taken, const LevelRule &rule, int &placed);
	void place_parked(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const LevelRule &rule, int &placed);
	void limit_cabins(); // the next frame's limit of who has a cabin
	// The bays' cars by cell of vehicle_r_ (their indices in the published list, ascending), made again only when the simulation
	// publishes a new list; and this frame's, those of the cells round the camera, ascending: the list's own order.
	void index_parked(const std::shared_ptr<const std::vector<ParkedRecord>> &parked);
	const std::vector<uint32_t> &parked_near(const godot::Vector3 &cam);
	void draw_rows(const godot::Vector3 &cam); // the rows of this frame to the server
	void commit_mesh(int model, int draw, const MeshBuilder &mb);
	// Whoever is aboard, to PeopleRenderer: the driver, and the passengers the order seats.
	void seat_people(uint32_t id, int type, int model, const Pose &p, float dist, float passengers, float order, float spin);
	godot::RID scenario_, material_, glass_, shadow_;
	OwnedMesh meshes_[AGENT_MODELS][DRAWS]; // the body surface, and the cabin's glass surface
	MultiMeshDraw draws_[AGENT_MODELS][DRAWS];
	std::vector<SeatSpot> seats_[AGENT_MODELS]; // where people sit in each model
	bool astride_[AGENT_MODELS] = {}; // its rider shows at any distance: nothing hides them
	InstanceRows seen_rows_[AGENT_MODELS][DRAWS], cast_rows_[AGENT_MODELS][DRAWS]; // the rows of the frame, kept at their size
	RowKeeper keeper_{ VEHICLE_DENSE_KEYS }; // each vehicle's row among what the camera sees
	int rows_lost_ = 0; // seen on the last frame and with no row: never, or a vehicle goes undrawn with its driver at the wheel
	MultiMeshDraw shadow_draws_[AGENT_MODELS][DRAWS]; // a level's instances with a lighter mesh; none for the far level, FleetShadow's
	FleetShadow fleet_shadow_; // what the far level casts
	float reach_m_[AGENT_MODELS] = {}; // from a model's origin to its farthest corner
	float view_slack_ = 0; // agents.json view_slack
	float cabin_close_m_ = 0; // agents.json lod.cabin_close_m
	int body_tris_[AGENT_MODELS][DRAWS] = {}, glass_tris_[AGENT_MODELS][DRAWS] = {}, shown_[AGENT_MODELS][DRAWS] = {};
	Load load_[AGENT_MODELS];
	int paints_ = 0;
	float wheel_r_[AGENT_MODELS] = {};
	struct Odometer {
		float x = 0, z = 0, angle = 0;
		bool seen = false;
		int still = 0; // frames it has not moved
	};
	std::vector<Odometer> odo_;
	PeopleRenderer people_;
	float vehicle_r_ = 0, cabin_r_ = 0, far_r_ = 0, people_r_ = 0, jump_m_ = 0;
	float cabin_reach_ = 0; // the tier's reach of the cabin level; 0: the table's
	int cabin_cap_ = 0; // the most cabins drawn, the nearest; 0: no cap
	float cabin_limit2_ = 0; // the cap's distance, squared, found on the last frame (0: none)
	std::vector<float> near_d2_;
	std::shared_ptr<const std::vector<ParkedRecord>> parked_indexed_; // held: the list the cells index
	std::unordered_map<int64_t, std::vector<uint32_t>> parked_cells_;
	std::vector<uint32_t> parked_near_;
	int drawn_vehicles_ = 0, drawn_people_ = 0;
};

} // namespace city
