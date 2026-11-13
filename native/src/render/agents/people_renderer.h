// Pedestrians near the camera drawn with the cooked MakeHuman people (people_asset.h): one MultiMesh per variation and
// level, refilled every frame. A body of the pedestrian's age, tints and phase come from its look hash (Roadcraft looks.ts);
// the walk phase follows the metres walked over the variation's stride, and one standing plays the idle or what the simulation says
// they are doing (PedAct), with what that holds in the hand (held_things.h). Past blob_from_m
// a person is its coarsest mesh casting no real shadow, a soft disc under it (material "people_blob"); where the whole figure
// stands under card_below_px on the screen (render/detail.h, the frame's own lens) it is the card of its pictures
// (material "people_far"). Material "people" (data/render/materials.json), settings in data/render/agents.json "people".
#pragma once

#include "render/agents/agent_meshes.h"
#include "render/agents/held_things.h"
#include "render/agents/helmets.h"
#include "render/agents/body_renderer.h"
#include "render/agents/people_asset.h"
#include "render/detail.h"
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
	bool init(const RenderContext &ctx, const godot::Dictionary &people, const godot::Dictionary &held, godot::PackedStringArray &errors);
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
	void set_view(const DetailView &view) { view_ = view; } // the frame's lens, before seat and update
	BodyRenderer &bodies() { return bodies_; }
	int variation_of(uint32_t look, int age) const; // which body a walker is drawn with
	// The clip and phase walker `id` was drawn in a frame or two ago (a body struck down falls from it, body_renderer.cpp); false when
	// it was not drawn lately (out of the view, or gone longer).
	bool drawn_pose(int id, int &clip, float &phase) const;
	int shown(int level) const; // people drawn at this slot on the last frame
	// The probes: how many slots a body has (its lods, the shadowless mesh, the card), and from how far a body `height` tall is a card
	// on this frame's lens (0: at any distance or at none, an orthographic view).
	int levels() const { return slots(); }
	float card_from_m(float height) const { return view_.ready() && view_.ortho_px_per_m <= 0 ? float(height * view_.px_per_m_at_1m / card_below_px_) : 0.f; }
	float blob_from_m() const { return blob_from_m_; }
	float tallest_m() const;
	// The probes, of the walkers of the last frame: the pixels the tallest-standing card takes and the fewest a mesh takes
	// (0: none drawn), against the card's pixels and the hold of a slot's edge.
	float card_px_most() const { return card_px_most_; }
	float mesh_px_least() const { return mesh_px_least_; }
	float card_below_px() const { return card_below_px_; }
	float level_hold() const { return level_hold_; }
	// For the tests and the captures: the file's clips, the first body's fit in each; and everybody at rest in clip `name`.
	godot::Array clips() const;
	bool show_clip(const godot::String &name);

private:
	PeopleAsset asset_;
	BodyRenderer bodies_;
	godot::RID scenario_, material_, far_material_;
	std::vector<MultiMeshDraw> draws_; // body * slots() + level
	std::vector<InstanceRows> bufs_;
	// A walker's slot at `dist`, kept until it is past the edge by the hold; `height` the body's, for the card's pixels.
	int held_level(uint64_t key, float dist, float height);
	float level_hold_ = 0;
	// A body's slots: its lods (casting), the coarsest mesh again casting nothing (past blob_from_m_), the card.
	int shadowless() const { return asset_.lods; }
	int card_slot() const { return asset_.lods + 1; }
	int slots() const { return asset_.lods + 2; }
	size_t slot(size_t variation, int level) const { return variation * size_t(slots()) + size_t(level); }
	std::vector<float> lod_m_; // camera distance where each level ends
	std::vector<int> by_age_[PED_AGES]; // the variations of each age
	OwnedMesh blob_mesh_;
	MultiMeshDraw blob_;
	InstanceRows blob_buf_;
	godot::RID blob_material_;
	float blob_from_m_ = 0, blob_radius_m_ = 0, blob_lift_m_ = 0;
	float card_below_px_ = 0; // agents.json people.card.below_px
	float card_px_most_ = 0, mesh_px_least_ = 0; // the last frame's walkers (probes)
	DetailView view_;
	// The slot a body `height` tall takes at `dist`: a mesh level, the shadowless mesh past blob_from_m_, or the card under card_below_px_.
	int slot_at(float dist, float height) const;
	float view_reach_m_ = 0; // agents.json people.view_reach_m
	int seat_clips_[int(SeatPose::COUNT)] = {}, stop_clips_[int(SeatPose::COUNT)] = {}; // each pose's clip, in PeopleAsset::clips' order
	void set_faces(const godot::RID &material) const;
	int act_clips_[int(PedAct::COUNT)] = {}; // what each act plays
	int crouch_down_ = 0, crouch_up_ = 0; // either side of the crouch's own clip
	struct DrawnPose {
		int clip = -1;
		float phase = 0;
		uint32_t frame = 0;
	};
	std::vector<DrawnPose> drawn_; // by walker id: what each was drawn in (drawn_pose)
	uint32_t frame_ = 0; // update's count
	int walk_clip_ = -1, idle_clip_ = -1;
	void remember_pose(uint32_t id, float rest, float doing, float walk);
	static constexpr float WALK_WEIGHS_MORE = 0.5f; // a standing share under this: the walk weighs most in the pose
	static constexpr uint32_t DRAWN_LATELY = 2; // frames: the casualty comes the frame after its walker was last drawn, or the same
	struct Played {
		int clip = 0;
		float phase = 0;
	};
	Played crouch_at(int female, float since, float left) const; // `since` it began and `left` of it
	HeldThings held_;
	Helmets helmets_;
	void set_tables(const godot::RID &material) const; // what any material that poses a person reads
	// A body's mesh level by the distance (lod_m_), from the tier's floor.
	int level_at(float dist) const;
	// The instance of a body at `p`.
	void place(uint64_t key, int variation, int slot_level, const Pose &p, uint32_t look, float doing, float phase, float pose_code, bool frightened = false);
	RowKeeper keeper_{ PEOPLE_DENSE_KEYS };
	int floor_ = 0;
};

} // namespace city
