// Vehicles and pedestrians near the camera: per vehicle type three MultiMeshes, near (the cabin behind see-through glass),
// the body (no cabin, the glass opaque) and far (the body's outline alone), each vehicle at the level its distance asks
// (cabin_radius_m, far_radius_m: the detail a camera-sized MultiMesh cannot choose per instance, nor the engine's mesh LOD,
// which picks one level per node, and which every shadow cascade the box touches draws again), refilled every frame from
// CityState::traffic and interpolated between the last two simulation ticks (wheels roll by a render-side odometer);
// pedestrians through PeopleRenderer. Materials "agent" and, on the near glass, "agent_glass" (data/render/materials.json);
// meshes, paints, palettes and how full each type runs from data/render/agents.json.
#pragma once

#include "render/agents/agent_meshes.h"
#include "render/agents/people_renderer.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

constexpr int WAGON_MODEL = VEHICLE_TYPES, AGENT_MODELS = VEHICLE_TYPES + 1; // a model per type and the sedans' estate

class AgentRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	int drawn_vehicles() const { return drawn_vehicles_; }
	int drawn_people() const { return drawn_people_; }
	// Per vehicle type and level: the triangles of its body and glass surfaces, the instances drawn last frame and the
	// draws (surfaces of the shown MultiMeshes): counts, no clock (city_core_probe agent_cost).
	godot::Dictionary cost() const;

private:
	static constexpr int CABIN_LEVEL = AGENT_CABIN_LEVEL, BODY_LEVEL = AGENT_BODY_LEVEL, FAR_LEVEL = AGENT_FAR_LEVEL, LEVELS = AGENT_LEVELS;
	// The level a vehicle `dist2` (square metres) from the camera is drawn at.
	int level_of(float dist2) const { return dist2 > far_r_ * far_r_ ? FAR_LEVEL : (dist2 > cabin_r_ * cabin_r_ ? BODY_LEVEL : CABIN_LEVEL); }
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
	// How the people aboard are drawn (agents.json occupants): skin, hair colour and cut, clothes (the pedestrians').
	void push_looks(const godot::Dictionary &occupants, const godot::Array &clothes, int hair_n, godot::PackedStringArray &errors);
	bool read_loads(const godot::Dictionary &loads, godot::PackedStringArray &errors);
	// A vehicle's passengers and the order its seats fill in (stride * seats + first), from its id.
	void riders(int type, uint32_t id, float &passengers, float &order) const;
	godot::RID scenario_, material_, glass_;
	OwnedMesh meshes_[AGENT_MODELS][LEVELS]; // the body surface, and the near level's glass surface
	MultiMeshDraw draws_[AGENT_MODELS][LEVELS];
	int body_tris_[AGENT_MODELS][LEVELS] = {}, glass_tris_[AGENT_MODELS][LEVELS] = {}, shown_[AGENT_MODELS][LEVELS] = {};
	Load load_[AGENT_MODELS];
	int paints_ = 0;
	float wheel_r_[AGENT_MODELS] = {};
	struct Odometer {
		float x = 0, z = 0, angle = 0;
		bool seen = false;
	};
	std::vector<Odometer> odo_;
	PeopleRenderer people_;
	float vehicle_r_ = 0, cabin_r_ = 0, far_r_ = 0, people_r_ = 0, jump_m_ = 0;
	int drawn_vehicles_ = 0, drawn_people_ = 0;
};

} // namespace city
