// Vehicles and pedestrians near the camera: one MultiMesh per vehicle type, refilled every frame from
// CityState::traffic and interpolated between the last two simulation ticks (wheels roll by a render-side odometer);
// pedestrians through PeopleRenderer. Material "agent" (data/render/materials.json); meshes and paints from
// data/render/agents.json.
#pragma once

#include "render/agents/people_renderer.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/array_mesh.hpp>

#include <vector>

namespace city {

class AgentRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at);
	int drawn_vehicles() const { return drawn_vehicles_; }
	int drawn_people() const { return drawn_people_; }

private:
	// Wheel roll angle of a vehicle slot: metres moved since the last frame over the wheel radius (render-side odometer).
	float roll(int id, const Pose &p, float wheel_r);
	godot::RID scenario_, material_;
	std::vector<godot::Ref<godot::ArrayMesh>> meshes_;
	MultiMeshDraw draws_[VEHICLE_TYPES];
	std::vector<godot::Color> paints_;
	float wheel_r_[VEHICLE_TYPES] = {};
	struct Odometer {
		float x = 0, z = 0, angle = 0;
		bool seen = false;
	};
	std::vector<Odometer> odo_;
	PeopleRenderer people_;
	float vehicle_r_ = 0, people_r_ = 0, jump_m_ = 0;
	int drawn_vehicles_ = 0, drawn_people_ = 0;
};

} // namespace city
