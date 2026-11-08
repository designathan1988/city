// The people a blow or a bullet took off their feet, drawn as themselves: the cooked body of the walker, near level, its bones carried by a ragdoll
// (ragdoll.h). A row of a float texture per body holds its bones as bind -> world rows (people_look.gdshaderinc BODY), written only while it moves.
// A limb or the head cut off is a body of its own, drawing only that piece (the person's own skin, not a stand-in); what hangs off a body opened
// or cut (guts, a bone out of a stump) is body_gore.h's, stepped here with the bodies and gone with them.
#pragma once

#include "render/agents/body_gore.h"
#include "render/agents/people_asset.h"
#include "render/agents/ragdoll.h"
#include "render/effects/mm_layer.h"
#include "render/render_context.h"

#include <godot_cpp/classes/shader_material.hpp>

#include <deque>
#include <vector>

namespace city {

struct BodySpec {
	godot::Vector3 at, velocity, tip; // at: the feet; tip: Ragdoll::begin
	float heading = 0, scatter = 0, life_s = 0; // radians; m/s between its joints; seconds it stays (one that rises: it lies)
	double up_s = -1; // one that rises: the simulated second its walker stands again (PoseFrame::sim_s), where it got up to; none below 0
	std::vector<BodyWall> walls; // of the buildings around it
	int variation = 0;
	uint32_t lost = 0; // parts gone, a bit each (ragdoll.h order)
	uint32_t piece = 0; // a piece cut off (Ragdoll::cut_to): only these parts are drawn, and only their particles hold together
	bool charred = false;
	struct StumpBone {
		RagdollStump stump;
		float length = 0;
	};
	std::vector<StumpBone> bones; // bones that show out of its stumps
	int guts = 0; // chains of guts out of it: from the belly, and a second from the pelvis
	godot::Vector3 guts_kick, second_kick;
};

class BodyRenderer {
public:
	bool init(const RenderContext &ctx, const PeopleAsset &asset, const RagdollShape &law, godot::PackedStringArray &errors);
	void reset();
	void release();
	void add(const BodySpec &spec);
	void set_gore(const godot::RID &material, const GoreLook &look, float gravity) { gore_.init(scenario_, material, look, gravity, law_->step_s); }
	int gore_chains() const { return gore_.chains(); }
	godot::PackedVector3Array places() const; // where each body's pelvis lies now (a probe)
	int gore_bones() const { return gore_.bones(); }
	void update(float dt, const Ragdoll::Ground &ground);
	void sim_now(double sim_s) { sim_s_ = sim_s; } // the newest pose publication's simulated second (PoseFrame::sim_s)
	int count() const { return int(bodies_.size()); }
	float rising_left() const; // the most seconds left to a body knocked down before its walker stands again; -1 with none
	int moving() const;

private:
	struct Body {
		Ragdoll doll;
		std::vector<godot::Transform3D> base; // each bone, bind to the world as it stood
		godot::Transform3D stood[RAGDOLL_PARTS]; // the parts' frames, inverted, as it stood
		float left = 0, floor = 0; // the pavement it stood on: the ground under a street lies lower
		float rise_s = 0; // how long it takes to get up, 0 when it does not
		double up_s = -1; // BodySpec::up_s: its seconds left are the simulation's, never the effects' clock
		godot::Transform3D stands; // where it gets up to
		std::vector<godot::Transform3D> lying; // each bone when it began to
		int variation = 0, slot = 0;
		uint32_t lost = 0;
		bool charred = false;
		godot::AABB box; // where its skin lies now: each bone's place, out by the variation's reach along the bone's axes
	};
	void make_ready();
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
	BodyGore gore_;
	static constexpr uint64_t BODIES_SEED = 0xb0d1e5ULL; // any fixed seed: the same shot scatters the same bodies
	Rng rng_{ BODIES_SEED };
	const RagdollShape *law_ = nullptr; // the ragdolls' (agents.json shapes.ragdoll), from init on
	float clock_ = 0;
	double sim_s_ = 0; // the newest publication's simulated second (sim_now)
	int bones_ = 0, idle_clip_ = 0, crouch_clip_ = 0, up_clip_ = 0;
	bool rows_dirty_ = false, layers_dirty_ = false;
};

} // namespace city
