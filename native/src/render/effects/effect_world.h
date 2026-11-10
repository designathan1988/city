// What a blow looks like: smoke, fire, flung pieces, ground marks, the flash, the ring and the camera's shake. The owner of
// the effects (render/effects/*): it takes the events of the simulation's blows (damage_event.h) from the ChangeLog, plays
// each once, draws the craters and the fires the damage state holds, and steps everything with its own clock. It drains
// those events from the ChangeLog (the one dirty channel) and never changes the map.
#pragma once

#include "render/agents/body_walls.h"
#include "render/mesh_builder.h"
#include "render/agents/people_renderer.h"
#include "render/effects/debris.h"
#include "render/effects/effect_params.h"
#include "render/effects/gore.h"
#include "render/effects/marks.h"
#include "render/effects/mounds.h"
#include "render/effects/puffs.h"
#include "render/effects/ruin_pieces.h"
#include "render/effects/wires.h"
#include "render/render_context.h"
#include "render/trees/tree_place.h"
#include "sim/state/city_state.h"

#include <unordered_map>
#include <vector>

namespace city {

constexpr uint64_t EFFECT_SEED = 0xb1a57ULL; // the effects' own random stream: any fixed seed

class EffectWorld {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void set_people(PeopleRenderer *people) { people_ = people; }
	void set_ruin_maker(RuinPieces::Maker maker) { ruins_.set_maker(std::move(maker)); }
	// The woods' trees a blow's clearing took (TreeRenderer::felled): (state, x, z, radius, clearing, out).
	using TreeFinder = std::function<void(const CityState &, float, float, float, int, std::vector<TreeInstance> &)>;
	void set_tree_finder(TreeFinder finder) { tree_finder_ = std::move(finder); }
	void set_wrecks(const std::vector<const MeshBuilder *> &bodies, const std::vector<godot::Vector3> &sizes); // the burnt bodies, by vehicle type
	void reset(); // a new city or a load: every effect gone
	void release();
	// The height-sample rectangles (x0, z0, x1, z1) the terrain composed again since the last frame (ChangeLog::terrain_rects): the
	// craters whose decal stands on them are laid again at the next change of the crater list. Before sync, each frame.
	void touch_terrain(const CityState &s, const std::vector<int> &rects);
	// One frame, the caller holding the map shared: plays what happened since the last, then steps the effects; the ruins' pieces
	// it hands the server come within the frame's admission (its deadline and byte ledger).
	void sync(CityState &s, FrameAdmission &admission, double frame_s);
	float shake() const { return shake_; } // metres the camera moves now
	float since_blast() const { return blast_at_ < 0 ? -1.f : float(now_ - blast_at_); } // effect seconds since the last blast (-1 none)
	bool busy() const {
		return puffs_.active(now_) || !sources_.empty() || debris_.moving() || ruins_.moving() || wires_.moving() || gore_.moving() || flash_left_ > 0;
	}
	int wires() const { return wires_.count(); }
	int pieces() const { return debris_.count(); }
	int ruin_pieces() const { return ruins_.pieces(); }
	const std::unordered_set<uint32_t> &broken() const { return ruins_.broken(); }
	std::vector<int64_t> take_repaint() { return ruins_.take_repaint(); }
	int bodies() const { return people_ ? people_->bodies().count() : 0; }
	int bodies_moving() const { return people_ ? people_->bodies().moving() : 0; }
	godot::Array bodies_falling() const { return people_ ? people_->bodies().falling() : godot::Array(); }
	int ruin_pieces_flying() const { return ruins_.flying(); }
	uint32_t ruin_digest(uint32_t seed) const { return ruins_.digest(seed); }
	int blood_marks() const { return marks_.blood(); }
	size_t events_played() const { return played_; }

private:
	struct Geyser {
		float x = 0, y = 0, z = 0;
		double until = 0;
		float carry = 0;
	};
	void apply(const DamageEvent &e, const CityState &s);
	void blast(const DamageEvent &e, const CityState &s);
	// The broken ground round the crater a blast on the ground dug (effect_blast.cpp): the rim of slabs heaved round the hole,
	// the plates of the surface tipped up round it and the clods, laid for good (effects.json shapes.crater_slabs).
	void crater_slabs(const DamageEvent &e, const CityState &s);
	void building_hit(const DamageEvent &e);
	void building_scar(const DamageEvent &e, const CityState &s); // effect_ruins.cpp
	void building_down(const DamageEvent &e, const CityState &s);
	void pile(const Ruin &r, bool falling, bool mound, bool loose); // the heap a building left (effect_ruins.cpp): its mound and the plain pieces on it
	void rebuild_ruins(const CityState &s); // every ruin of the state as a heap lying (open, undo, redo)
	float heap_of(const Ruin &r) const; // how high its mound stands
	void furniture(const DamageEvent &e, const CityState &s);
	void wire(const DamageEvent &e); // a wire torn off a felled pole, swinging (wires.h)
	void trees(const DamageEvent &e, const CityState &s); // the woods' trees a blow felled (effect_things.cpp)
	// A tree thrown down at its foot `at`, `height` tall: a charred stump, its trunk thrown burning, splinters and its crown in clumps.
	void fell_tree(const godot::Vector3 &at, const godot::Vector3 &push, float height);
	godot::Vector3 away(float x, float z, float speed) const; // the push of the last blast at (x, z): out from it, less far off
	void open_body(uint32_t body, const godot::Vector3 &kick, bool pelvis_too); // the guts out of a body's belly (gore.h)
	void gore(const DamageEvent &e, uint32_t body, const godot::Vector3 &way, uint32_t severed); // what a casualty's body shows opened (effect_bodies.cpp)
	bool load_looks(const RenderContext &ctx, godot::PackedStringArray &errors); // skin and clothes tints of the bodies (effect_bodies.cpp)
	void body(const DamageEvent &e, const CityState &s); // the pieces of someone struck
	void tracer(const DamageEvent &e); // a bullet's line and where it ends
	void wound(const DamageEvent &e);
	void drip(const DamageEvent &e);
	void vehicle_hit(const DamageEvent &e);
	void vehicle(const DamageEvent &e, const CityState &s);
	void casualty(const DamageEvent &e, const CityState &s);
	void burn_fires(const CityState &s, float dt);
	void step_geysers(float dt);
	void step_sources(float dt); // the flame sources a blast leaves (effect_blast.cpp)
	void flash(float radius);
	void light_update(float dt);
	void piece(const godot::Vector3 &at, const godot::Vector3 &size, const godot::Vector3 &v, const godot::Color &tint, bool cylinder = false, float burn = 0.f);

	godot::RID debris_material_;
	std::vector<godot::Vector3> wreck_sizes_; // by vehicle type: the box of its burnt body
	EffectParams params_;
	Puffs puffs_;
	Marks marks_;
	Mounds mounds_;
	Debris debris_;
	Wires wires_;
	Gore gore_;
	TreeFinder tree_finder_;
	std::vector<TreeInstance> felled_; // the trees a TREES event fells, found again each time
	float blast_x_ = 0, blast_z_ = 0, blast_r_ = 0; // the last blast played: what its furniture, wires and trees are pushed from
	RuinPieces ruins_;
	PeopleRenderer *people_ = nullptr; // whose bodies fall (render_world.cpp)
	Rng rng_{ EFFECT_SEED };
	godot::RID scenario_, light_, light_inst_;
	std::vector<DamageEvent> events_, events_played_; // to play; played this frame
	struct Source {
		float x = 0, y = 0, z = 0, size = 0, rate = 0, carry = 0;
		double until = 0;
	};
	std::vector<Source> sources_;
	std::vector<Geyser> geysers_;
	std::vector<godot::Color> skins_, clothes_;
	std::unordered_map<int, float> fire_carry_;
	double now_ = 0, blast_at_ = -1;
	float shake_ = 0, flash_left_ = 0, flash_energy_ = 0, flash_range_ = 0;
	godot::Vector3 flash_at_;
	static constexpr uint64_t UNSEEN = ~uint64_t(0);
	uint64_t craters_seen_ = UNSEEN, crater_layout_seen_ = UNSEEN, ruins_seen_ = UNSEEN, scars_seen_ = UNSEEN; // DamageState's revisions drawn
	size_t played_ = 0;
	float ground_here_ = 0; // the ground under the heap being laid
};

} // namespace city
