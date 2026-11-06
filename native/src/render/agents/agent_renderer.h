// Vehicles and pedestrians near the camera: per vehicle type two MultiMeshes, near (the cabin behind see-through glass)
// and far (no cabin, the glass opaque), refilled every frame from CityState::traffic and interpolated between the last
// two simulation ticks (wheels roll by a render-side odometer); pedestrians through PeopleRenderer. Materials "agent"
// and, on the near glass, "agent_glass" (data/render/materials.json); meshes, paints, palettes and how full each type
// runs from data/render/agents.json.
#pragma once

#include "render/agents/agent_meshes.h"
#include "render/agents/fleet_shadow.h"
#include "render/view_cull.h"
#include "render/agents/people_renderer.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

constexpr int CAR_MODELS = int(VehicleType::MOTO); // sedan .. pickup come from agents.json "cars", then the wagon
constexpr int WAGON_MODEL = VEHICLE_TYPES, AGENT_MODELS = VEHICLE_TYPES + 1; // a model per type and the sedans' estate

class AgentRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void update(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const PoseSample &at);
	int drawn_vehicles() const { return drawn_vehicles_; }
	int drawn_people() const { return drawn_people_; }
	godot::Array people_clips() const { return people_.clips(); }
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

private:
	static constexpr int CABIN_LEVEL = AGENT_CABIN_LEVEL, BODY_LEVEL = AGENT_BODY_LEVEL, FAR_LEVEL = AGENT_FAR_LEVEL, LEVELS = AGENT_LEVELS;
	// One more than the built levels: the cabin simplified, glass and people still seen, past cabin_close_m (agent_lod.h).
	static constexpr int MID_DRAW = LEVELS, DRAWS = LEVELS + 1;
	static constexpr uint64_t VEHICLE_DENSE_KEYS = 1u << 20;
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
	bool build_meshes(const godot::Dictionary &t, godot::PackedStringArray &errors);
	bool push_palettes(const godot::Dictionary &t, const std::vector<uint32_t> &lines, godot::PackedStringArray &errors);
	bool read_loads(const godot::Dictionary &loads, godot::PackedStringArray &errors);
	// A vehicle's passengers and the order its seats fill in (stride * seats + first), from its id.
	void riders(int type, uint32_t id, float &passengers, float &order) const;
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
	int drawn_vehicles_ = 0, drawn_people_ = 0;
};

} // namespace city
