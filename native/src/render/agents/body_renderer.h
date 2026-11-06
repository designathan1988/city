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
	int variation = 0;
	uint32_t lost = 0; // parts gone, a bit each (ragdoll.h order)
	bool charred = false;
};

class BodyRenderer {
public:
	bool init(const RenderContext &ctx, const PeopleAsset &asset, godot::PackedStringArray &errors);
	void reset();
	void release();
	void add(const BodySpec &spec);
	void update(float dt, const Ragdoll::Ground &ground);
	int count() const { return int(bodies_.size()); }
	int moving() const;

private:
	struct Body {
		Ragdoll doll;
		std::vector<godot::Transform3D> base; // each bone, bind to the world as it stood
		godot::Transform3D stood[RAGDOLL_PARTS]; // the parts' frames, inverted, as it stood
		float left = 0, floor = 0; // the pavement it stood on: the ground under a street lies lower
		int variation = 0, slot = 0;
		uint32_t lost = 0;
		bool charred = false;
	};
	void make_ready();
	void pose(Body &b);
	void lay_out();
	const PeopleAsset *asset_ = nullptr;
	godot::RID scenario_, texture_;
	godot::Ref<godot::ShaderMaterial> material_;
	std::vector<MmLayer> layers_; // a variation's
	std::deque<Body> bodies_;
	std::vector<int> free_slots_;
	godot::PackedByteArray rows_;
	Rng rng_{ 0xb0d1e5ULL };
	float clock_ = 0;
	int bones_ = 0;
	bool rows_dirty_ = false, layers_dirty_ = false;
};

} // namespace city
