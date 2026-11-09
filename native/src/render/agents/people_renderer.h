// Pedestrians near the camera drawn with the cooked MakeHuman people (people_asset.h): one MultiMesh per variation and
// level, refilled every frame. A walker's body is cast once (people_casting.h), its phase comes from its look hash;
// the walk phase follows the metres walked over the variation's stride, and one standing plays the idle or what the simulation says
// they are doing (PedAct), with what that holds in the hand (held_things.h). Past blob_from_m
// a person casts no real shadow: a soft disc lies under it (material "people_blob"). Material "people"
// (data/render/materials.json), settings in data/render/agents.json "people".
#pragma once

#include "render/agents/agent_shapes.h"
#include "render/agents/agent_meshes.h"
#include "render/agents/held_things.h"
#include "render/agents/helmets.h"
#include "render/agents/body_renderer.h"
#include "render/agents/people_asset.h"
#include "render/agents/people_casting.h"
#include "render/row_keeper.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/view_cull.h"
#include "sim/state/city_state.h"

#include <cmath>
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
	p.rise = lerpf(a.rise, b.rise, t);
	return p;
}

// A record's pose at the moment the render draws (pose_buffer.h PoseSample).
inline Pose record_pose(const AgentRecord &rec, const PoseSample &at) {
	return lerp_pose(rec.at[at.first], rec.at[at.first + 1], at.t);
}

// One agent instance: model +Z along the heading, +X to its left, uniform scale.
// `seed` goes in the instance colour's alpha (a vehicle's occupants, agents.gdshader); the colour stays white.
inline void agent_row(float *row, const Pose &p, float scale, float c0, float c1, float c2, float c3, float seed = 1.f) {
	// on a grade the body turns about its own left: along the heading it rises, its up leans back
	const float flat = scale / std::sqrt(1.f + p.rise * p.rise), up = flat * p.rise;
	godot::Vector3 x(p.fz * scale, 0, -p.fx * scale), y(-p.fx * up, flat, -p.fz * up), z(p.fx * flat, up, p.fz * flat);
	mm_row(row, x, y, z, godot::Vector3(p.x, p.y, p.z), godot::Color(1, 1, 1, seed), c0, c1, c2, c3);
}

class PeopleRenderer {
public:
	bool init(const RenderContext &ctx, const DataValue &people, const DataValue &held, const AgentShapes &shapes, godot::PackedStringArray &errors);
	void reset();
	void set_level_hold(float share) { level_hold_ = share; }
	// Who rides is keyed from here on, past the walkers' ids.
	static constexpr uint64_t PEOPLE_DENSE_KEYS = 1u << 20;
	// The frame's rows open: riders first (seat), then the walkers and the draws (update).
	void begin();
	// Somebody in `spot` of a vehicle at `vehicle`, `dist` from the camera: their looks from `look`, the bicycle's cranks at `phase` (0..1),
	// a two-wheeler's rider with a foot down when `stopped`.
	void seat(uint64_t key, const Pose &vehicle, const SeatSpot &spot, uint32_t look, float dist, float phase, bool stopped);
	// Returns how many walkers were drawn.
	int update(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const PoseSample &at, float radius);
	void set_floor(int level) { floor_ = level; } // the nearest level the graphics tier allows
	BodyRenderer &bodies() { return bodies_; }
	int variation_of(uint32_t look, int age) const { return casting_.worn(look, age); } // the body a walker wears, or wore last
	int shown(int level) const; // people drawn at this level on the last frame
	godot::Dictionary cast_census() const { return casting_.census(); } // who walks as whom (people_casting.h census)
	// For the tests and the captures: the file's clips, the first body's fit in each; and everybody at rest in clip `name`.
	godot::Array clips() const;
	bool show_clip(const godot::String &name);

private:
	PeopleAsset asset_;
	BodyRenderer bodies_;
	godot::RID scenario_, material_, far_material_;
	std::vector<MultiMeshDraw> draws_; // body * (lods + 1) + level; the last, the far card casting no shadow
	std::vector<InstanceRows> bufs_;
	int held_level(uint64_t key, float dist);
	float level_hold_ = 0;
	// The slot of a variation's level in draws_ and bufs_: its lods, then the far mesh casting no shadow.
	size_t slot(size_t variation, int level) const { return variation * size_t(asset_.lods + 1) + size_t(level); }
	std::vector<float> lod_m_; // camera distance where each level ends
	std::vector<int> by_age_[PED_AGES]; // the variations of each age (who rides in a vehicle)
	PeopleCasting casting_; // who walks as whom
	OwnedMesh blob_mesh_;
	MultiMeshDraw blob_;
	InstanceRows blob_buf_;
	godot::RID blob_material_;
	float blob_from_m_ = 0, blob_radius_m_ = 0, blob_lift_m_ = 0;
	float view_reach_m_ = 0; // agents.json people.view_reach_m
	int seat_clips_[int(SeatPose::COUNT)] = {}, stop_clips_[int(SeatPose::COUNT)] = {}; // each pose's clip, in PeopleAsset::clips' order
	void set_faces(const godot::RID &material) const;
	int act_clips_[int(PedAct::COUNT)] = {}; // what each act plays
	int crouch_down_ = 0, crouch_up_ = 0; // either side of the crouch's own clip
	struct Played {
		int clip = 0;
		float phase = 0;
	};
	Played crouch_at(int female, float since, float left) const; // `since` it began and `left` of it
	HeldThings held_;
	Helmets helmets_;
	void set_tables(const godot::RID &material) const; // what any material that poses a person reads
	// A body's level by the distance; `blob`: past blob_from_m_, where the far mesh casts no shadow.
	int level_at(float dist) const;
	// The instance of a body at `p`.
	void place(uint64_t key, int variation, int slot_level, const Pose &p, uint32_t look, float doing, float phase, float pose_code, bool frightened = false);
	RowKeeper keeper_{ PEOPLE_DENSE_KEYS };
	int floor_ = 0;
};

} // namespace city
