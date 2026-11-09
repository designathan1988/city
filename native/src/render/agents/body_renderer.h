// The people a blow or a bullet took off their feet, drawn as themselves: the cooked body of the walker, near level, its bones carried by a ragdoll
// (ragdoll.h). A row of a float texture per body holds its bones as bind -> world rows (people_look.gdshaderinc BODY), written only while it moves.
#pragma once

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
	float heading = 0, scatter = 0, life_s = 0; // radians; m/s between its joints; seconds it stays
	bool rises = false; // gets up at the end of them, to where it stood: the walker goes on from there
	std::vector<BodyWall> walls; // of the buildings around it
	int variation = 0;
	uint32_t lost = 0; // parts gone, a bit each (ragdoll.h order)
	bool charred = false;
	bool writhes = false; // down for good and hurt (damage_event.h CAS_CRAWL): it writhes once it lies still (Ragdoll::writhe)
};

class BodyRenderer {
public:
	bool init(const RenderContext &ctx, const PeopleAsset &asset, const RagdollShape &law, godot::PackedStringArray &errors);
	void reset();
	void release();
	uint32_t add(const BodySpec &spec); // its serial, 0 when it could not be laid
	bool joint(uint32_t serial, int particle, godot::Vector3 &at) const; // a particle of that body now (rag::Joint); false once it is gone
	void update(float dt, const Ragdoll::Ground &ground);
	int count() const { return int(bodies_.size()); }
	int moving() const;

private:
	struct Body {
		Ragdoll doll;
		std::vector<godot::Transform3D> base; // each bone, bind to the world as it stood
		godot::Transform3D stood[RAGDOLL_PARTS]; // the parts' frames, inverted, as it stood
		float left = 0, floor = 0; // the pavement it stood on: the ground under a street lies lower
		float rise_s = 0; // how long it takes to get up, 0 when it does not
		godot::Transform3D stands; // where it gets up to
		std::vector<godot::Transform3D> lying; // each bone when it began to
		int variation = 0, slot = 0;
		uint32_t serial = 0; // its name while it lasts (Gore holds bodies by it)
		uint32_t lost = 0;
		bool charred = false, writhes = false;
		float writhe_t = -1.f; // seconds it has writhed, below 0 until it came to rest
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
	static constexpr uint64_t BODIES_SEED = 0xb0d1e5ULL; // any fixed seed: the same shot scatters the same bodies
	Rng rng_{ BODIES_SEED };
	const RagdollShape *law_ = nullptr; // the ragdolls' (agents.json shapes.ragdoll), from init on
	float clock_ = 0;
	uint32_t next_serial_ = 0;
	int bones_ = 0, idle_clip_ = 0, crouch_clip_ = 0, up_clip_ = 0;
	bool rows_dirty_ = false, layers_dirty_ = false;
};

} // namespace city
