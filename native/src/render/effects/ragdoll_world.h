// The people a blow struck, falling as bodies (render/effects: a visual effect, not game state, like the debris): each is the
// cooked person of its look as the 16 rigid bodies of sim/physics/person_rig.h, thrown by the blow and stepped by XPBD against the
// ground of the city, drawn with the person's own mesh skinned by a texture of bone matrices (ragdoll.gdshader). A part lost
// earlier is not there; a part torn off at this blow leaves its joint and flies. A body at rest is frozen; the oldest goes
// when max_people is reached. Reads the city, never writes it.
#pragma once

#include "core/random.h"
#include "render/agents/people_asset.h"
#include "render/effects/ragdoll_params.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>

#include <memory>
#include <vector>

namespace city {

struct RagdollSpawn {
	uint32_t look = 0; // the pedestrian's look hash
	godot::Vector3 feet; // where it stood, on the ground
	float heading = 0; // radians, forward = (cos, sin) in x, z
	godot::Vector3 velocity; // what the blow gives every body
	godot::Vector3 limb_velocity; // what a part torn off at this blow leaves with
	float scatter = 0; // m/s each body's own difference
	uint32_t lost = 0, severed = 0; // CasLimb masks: parts gone earlier / taken at this blow
	bool charred = false; // drawn black
	float life_s = 0;
};

class RagdollWorld {
public:
	bool init(const RenderContext &ctx, const PeopleAsset &people, const godot::Dictionary &people_cfg, float gravity, godot::PackedStringArray &errors);
	void reset(); // every body gone
	void release();
	void spawn(const RagdollSpawn &s, Rng &rng);
	void step(float dt, const CityState &city); // one frame of every body that is not at rest
	void draw(); // the bones' texture and the MultiMesh of each variation
	int count() const { return int(people_.size()); }
	bool moving() const; // some body is not at rest yet
	// What the last step counted: bodies, joints, skin points tested, contacts solved, substeps; people alive; microseconds.
	godot::Dictionary cost() const;
	// One person of `variation` thrown (m/s forward and up) from `drop_m` over flat ground for `seconds`: the solver's own measures.
	godot::Dictionary probe(int variation, float speed_ms, float up_ms, float drop_m, float seconds, int substeps = 0);
	const RagdollParams &params() const { return params_; }
	// For tests: the live bodies as seen by the solver.
	const phys::Articulation *body_of(int index) const { return index >= 0 && index < count() ? &people_[size_t(index)].art : nullptr; }
	const phys::PersonRig *rig_of(int variation) { return rig_for(variation); }

private:
	struct Person {
		phys::Articulation art;
		int variation = 0;
		float top = 0, bottom = 0, hair = 0, skin = 0;
		double born = 0;
		float life = 0, age = 0;
		bool at_rest = false, charred = false;
		const phys::PersonRig *rig = nullptr;
	};
	phys::PersonRig *rig_for(int variation); // built on the first need from the variation's own mesh
	void write_bones(const Person &p, int row);

	RagdollParams params_;
	const PeopleAsset *asset_ = nullptr;
	godot::RID scenario_, material_;
	std::vector<std::unique_ptr<phys::PersonRig>> rigs_;
	std::vector<char> rig_tried_;
	std::vector<Person> people_;
	std::vector<MultiMeshDraw> draws_; // one per variation
	std::vector<godot::PackedFloat32Array> bufs_;
	godot::Ref<godot::Image> image_;
	godot::Ref<godot::ImageTexture> bones_;
	godot::PackedByteArray texels_;
	int width_ = 0;
	float gravity_ = 0;
	double clock_ = 0;
	phys::StepCost cost_;
	int last_us_ = 0;
};

} // namespace city
