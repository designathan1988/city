// The people a blow or a bullet took off their feet, drawn as themselves: the cooked body of the walker, near level, its bones carried by a ragdoll
// (ragdoll.h). A row of a float texture per body holds its bones as bind -> root rows (people_look.gdshaderinc BODY), written only while it moves;
// its instance's transform is that root, its root bone's frame, so the engine's motion vectors carry the body's fall (TAA, FSR2 read them).
// One body a casualty: struck again while down, the body that fell takes the blow (strike).
#pragma once

#include "render/agents/people_asset.h"
#include "render/agents/ragdoll.h"
#include "render/effects/mm_layer.h"
#include "render/render_context.h"

#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <deque>
#include <limits>
#include <vector>

namespace city {

struct BodySpec {
	godot::Vector3 at, velocity, tip, brace; // at: the feet; tip and brace: Ragdoll::begin
	float heading = 0, scatter = 0, life_s = 0; // radians; m/s between its joints; seconds it stays, 0 for good (the dead)
	bool rises = false; // gets up at the end of them, to where it stood: the walker goes on from there
	int who = -1; // the casualty's id (DamageEvent::id): a walker down struck again or dying there is found by it (strike, die)
	std::vector<BodyWall> walls; // of the buildings around it
	int variation = 0;
	uint32_t lost = 0; // parts gone, a bit each (ragdoll.h order)
	bool charred = false;
	float pool = 0; // the blood it leaves where it comes to rest, a share of a pool's size; 0 none (the living)
	uint32_t bleeds = 0; // the particles its wounds bleed from (ragdoll_parts.h Joint, a bit each): where it splashes and pools
	uint32_t piece = 0; // a limb torn off: its parts (ragdoll.h order), the body of its own; `lost` then every other part. 0: a whole body
	int clip = -1; // the clip it was drawn in at the blow (PeopleRenderer::drawn_pose), -1 the idle
	float phase = 0; // and how far into it
};

// What a body leaves on the ground: a pool where it came to rest (pool > 0, its share of a pool's size), a splash where it landed hard (0).
struct BodyMark {
	godot::Vector3 at;
	float pool = 0;
};

class BodyRenderer {
public:
	bool init(const RenderContext &ctx, const PeopleAsset &asset, godot::PackedStringArray &errors);
	void reset();
	void release();
	void add(const BodySpec &spec);
	// body_blows.cpp: a body down struck again (`spec.who` a walker's or a dead body's): that body takes the blow: its wounds join, a death
	// makes it a dead one, a knock sets when it gets up, the push shoves it, and each of `pieces` (BodySpec::piece) comes off it as it lies.
	// False when nobody's body of that id is down: the caller adds one.
	bool strike(const BodySpec &spec, const std::vector<BodySpec> &pieces);
	// Somebody lying hurt has died where they lie (a wound bled out, wounds.cpp CAS_BLED): their body gets up no more and stays.
	void die(int who);
	// The walker `from` died: their body lies on under the dead body's id `to` (ped_dead.cpp BODY_DEAD), the walker's id free for another.
	void rename(int from, int to);
	// The simulation lets dead body `who` go (BODY_GONE, past its cap): the body and the limbs torn off it go too.
	void let_go(int who);
	// One frame; a bleeding body that strikes the ground faster than `hard_ms` splashes there, `splashes_most` times at most.
	void update(float dt, const Ragdoll::Ground &ground, float hard_ms, int splashes_most);
	void take_marks(std::vector<BodyMark> &out); // what the bodies left since the last call
	// What the tests and look plans read of the bodies (body_census.cpp): {bodies; moving, those still falling; kept, those who stay
	// where they fell (the dead); from_drawn, those fallen from the pose their walker was drawn in; pieces, limbs torn off, each a body of
	// its own; deepest, the metres the deepest body come to rest has lain under the ground since the city opened; overlap, the metres the
	// deepest joint of a body at rest lies within another's (its own torn limbs aside), nearer than they touch; in_walls, the metres the
	// deepest joint of a body at rest lies inside a wall it meets; street_walls, how many of the bodies' walls are the street's pieces
	// (those with a bottom); who, the ids (DamageEvent::id) of the whole bodies lying there: a walker's while they live, the dead body's
	// once dead (DEAD_BODY_ID_BASE + n, ped_dead.cpp)}.
	godot::Dictionary census() const;
	godot::PackedVector3Array places() const; // each whole body's pelvis, where it lies, its torn limbs by it (a look plan aims at one)

private:
	static constexpr float FOR_GOOD_S = std::numeric_limits<float>::infinity(); // the dead stay where they fell, the aftermath kept, till the cap
	static constexpr float NEAR_BODY_M = 2.5f; // a body's joints reach this far from its pelvis, and as far again for another's: farther, they cannot touch
	static constexpr float GET_UP_S = 1.7f, RISE_SHARE_MOST = 0.7f; // how long getting up takes, at most of the time down
	struct Body {
		Ragdoll doll;
		std::vector<godot::Transform3D> base; // each bone, bind to the world as it stood
		godot::Transform3D stood[RAGDOLL_PARTS]; // the parts' frames, inverted, as it stood
		float left = 0, floor = 0; // the pavement it stood on: the ground under a street lies lower
		float rise_s = 0; // how long it takes to get up, 0 when it does not
		godot::Transform3D stands; // where it gets up to
		std::vector<godot::Transform3D> lying; // each bone when it began to
		int variation = 0, slot = 0, splashes = 0;
		int who = -1; // the walker's id while they live, the dead body's once dead (rename): the later blows find it (strike, die, let_go)
		float pool = 0;
		bool pooled = false; // its pool laid
		bool from_drawn = false; // fell from its walker's drawn pose, not the idle
		bool piece = false; // a limb torn off
		uint32_t family = 0; // a body and the limbs torn off it
		uint32_t lost = 0, bleeds = 0;
		bool charred = false;
		godot::AABB box; // where its skin lies now: each bone's place, out by the variation's reach along the bone's axes
		int root_bone = 0; // the bone its root follows: the torso's, a limb torn off's first part's (root_bone_of)
		godot::Transform3D root; // that bone's frame, orthonormal: its instance's transform, its rows relative to it (write_bones)
	};
	void make_ready();
	int root_bone_of(uint32_t piece) const; // the first bone of the torso, or of the first part of a limb torn off (`piece`)
	// A row of the texture for a new body; full, the oldest limb torn off goes, else the oldest of those who will not get up (none of
	// `keep`, the family it joins): the dead the simulation keeps (blast.shot dead_kept, under the rows) keep their bodies.
	int take_slot(uint32_t keep);
	void pose(Body &b);
	void rise(Body &b);
	void write_bones(Body &b, const godot::Transform3D *bones);
	void lay_out();
	void bound_layers(); // each variation's layer boxed round its bodies: what the camera and each light cull it by
	const PeopleAsset *asset_ = nullptr;
	godot::RID scenario_, texture_;
	godot::Ref<godot::ShaderMaterial> material_;
	std::vector<MmLayer> layers_; // a variation's
	std::deque<Body> bodies_;
	std::vector<int> free_slots_;
	godot::PackedByteArray rows_;
	std::vector<float> clip_scratch_; // a clip's rows of one body, for a pose being worked out (add, rise)
	std::vector<godot::Transform3D> bone_scratch_;
	std::vector<BodyMark> marks_;
	float deepest_ = 0;
	uint32_t families_ = 0; // the last family given; 0 is none
	static constexpr uint64_t FALL_SEED = 0xb0d1e5ULL; // the bodies' own draws: the same falls on every run
	Rng rng_{ FALL_SEED };
	float clock_ = 0;
	int bones_ = 0, idle_clip_ = 0, walk_clip_ = -1, crouch_clip_ = 0, up_clip_ = 0;
	bool rows_dirty_ = false, layers_dirty_ = false;
};

} // namespace city
