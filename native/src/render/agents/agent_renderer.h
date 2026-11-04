// Vehicles and pedestrians near the camera: per vehicle type two MultiMeshes, near (the cabin behind see-through glass)
// and far (no cabin, the glass opaque), refilled every frame from CityState::traffic and interpolated between the last
// two simulation ticks (wheels roll by a render-side odometer); pedestrians through PeopleRenderer. Materials "agent"
// and, on the near glass, "agent_glass" (data/render/materials.json); meshes, paints, palettes and how full each type
// runs from data/render/agents.json.
#pragma once

#include "render/agents/people_renderer.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

class AgentRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	int drawn_vehicles() const { return drawn_vehicles_; }
	int drawn_people() const { return drawn_people_; }
	const PeopleAsset &people_asset() const { return people_.asset(); } // the cooked people the bodies that fall are made of
	// Per vehicle type and level: the triangles of its body and glass surfaces, the instances drawn last frame and the
	// draws (surfaces of the shown MultiMeshes): counts, no clock (city_core_probe agent_cost).
	godot::Dictionary cost() const;

private:
	static constexpr int CABIN_LEVEL = 0, BODY_LEVEL = 1, LEVELS = 2;
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
	bool push_palettes(const godot::Dictionary &t, godot::PackedStringArray &errors);
	// How the people aboard are drawn (agents.json occupants): skin, hair colour and cut, clothes (the pedestrians').
	void push_looks(const godot::Dictionary &occupants, const godot::Array &clothes, int hair_n, godot::PackedStringArray &errors);
	bool read_loads(const godot::Dictionary &loads, godot::PackedStringArray &errors);
	// A vehicle's passengers and the order its seats fill in (stride * seats + first), from its id.
	void riders(int type, uint32_t id, float &passengers, float &order) const;
	godot::RID scenario_, material_, glass_;
	OwnedMesh meshes_[VEHICLE_TYPES][LEVELS]; // the body surface, and the near level's glass surface
	MultiMeshDraw draws_[VEHICLE_TYPES][LEVELS];
	int body_tris_[VEHICLE_TYPES][LEVELS] = {}, glass_tris_[VEHICLE_TYPES][LEVELS] = {}, shown_[VEHICLE_TYPES][LEVELS] = {};
	Load load_[VEHICLE_TYPES];
	int paints_ = 0;
	float wheel_r_[VEHICLE_TYPES] = {};
	struct Odometer {
		float x = 0, z = 0, angle = 0;
		bool seen = false;
	};
	std::vector<Odometer> odo_;
	PeopleRenderer people_;
	float vehicle_r_ = 0, cabin_r_ = 0, people_r_ = 0, jump_m_ = 0;
	int drawn_vehicles_ = 0, drawn_people_ = 0;
};

} // namespace city
