// Pedestrians near the camera drawn with the cooked MakeHuman people (people_asset.h): one MultiMesh per variation and
// level, refilled every frame. A body of the pedestrian's age, tints and phase come from its look hash (Roadcraft looks.ts);
// the walk phase follows the metres walked over the variation's stride, and one standing plays the idle. Past blob_from_m
// a person casts no real shadow: a soft disc lies under it (material "people_blob"). Material "people"
// (data/render/materials.json), settings in data/render/agents.json "people".
#pragma once

#include "render/agents/people_asset.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

// The pose between two simulation ticks (t = share of the tick elapsed), heading renormalised.
inline Pose lerp_pose(const Pose &a, const Pose &b, float t) {
	Pose p;
	p.x = lerpf(a.x, b.x, t);
	p.y = lerpf(a.y, b.y, t);
	p.z = lerpf(a.z, b.z, t);
	V2 f = V2(lerpf(a.fx, b.fx, t), lerpf(a.fz, b.fz, t)).norm();
	p.fx = f.x;
	p.fz = f.y;
	return p;
}

// A record's pose at the moment the render draws (pose_buffer.h PoseSample).
inline Pose record_pose(const AgentRecord &rec, const PoseSample &at) {
	return lerp_pose(rec.at[at.first], rec.at[at.first + 1], at.t);
}

// One agent instance: model +Z along the heading, +X to its left, uniform scale.
// `seed` goes in the instance colour's alpha (a vehicle's occupants, agents.gdshader); the colour stays white.
inline void push_agent(godot::PackedFloat32Array &buf, const Pose &p, float scale, float c0, float c1, float c2, float c3,
		float seed = 1.f) {
	godot::Vector3 x(p.fz * scale, 0, -p.fx * scale), y(0, scale, 0), z(p.fx * scale, 0, p.fz * scale);
	mm_push(buf, x, y, z, godot::Vector3(p.x, p.y, p.z), godot::Color(1, 1, 1, seed), c0, c1, c2, c3);
}

class PeopleRenderer {
public:
	bool init(const RenderContext &ctx, const godot::Dictionary &people, godot::PackedStringArray &errors);
	void reset();
	// Returns how many people were drawn.
	int update(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at, float radius);
	// For the tests and the captures: the file's clips, the first body's fit in each; and everybody at rest in clip `name`.
	godot::Array clips() const;
	bool show_clip(const godot::String &name);

private:
	PeopleAsset asset_;
	godot::RID scenario_, material_;
	std::vector<MultiMeshDraw> draws_; // variation * (lods + 1) + level; the last, the far mesh casting no shadow
	std::vector<godot::PackedFloat32Array> bufs_;
	std::vector<float> lod_m_; // camera distance where each level ends
	std::vector<int> by_age_[PED_AGES]; // the variations of each age
	OwnedMesh blob_mesh_;
	MultiMeshDraw blob_;
	godot::PackedFloat32Array blob_buf_;
	godot::RID blob_material_;
	float blob_from_m_ = 0, blob_radius_m_ = 0, blob_lift_m_ = 0;
};

} // namespace city
