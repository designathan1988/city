// What a blow looks like: smoke, fire, flung pieces, ground marks, the flash, the ring and the camera's shake. The owner of
// the effects (render/effects/*): it takes the events of the simulation's blows (damage_event.h) from the ChangeLog, plays
// each once, draws the craters and the fires the damage state holds, and steps everything with its own clock. It drains
// those events from the ChangeLog (the one dirty channel) and never changes the map.
#pragma once

#include "render/agents/body_walls.h"
#include "render/agents/people_renderer.h"
#include "render/effects/debris.h"
#include "render/effects/effect_params.h"
#include "render/effects/ground_cells.h"
#include "render/effects/heave.h"
#include "render/effects/marks.h"
#include "render/effects/mounds.h"
#include "render/effects/puffs.h"
#include "render/effects/ropes.h"
#include "render/effects/ruin_pieces.h"
#include "render/props/prop_kit.h"
#include "render/render_context.h"
#include "sim/state/city_state.h"

#include <chrono>
#include <unordered_map>
#include <vector>

namespace city {

class EffectWorld {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	// Whose bodies fall; what hangs off a body opened or cut is drawn with the debris' material, in the gore's look.
	void set_people(PeopleRenderer *people) {
		people_ = people;
		if (people_) people_->bodies().set_gore(debris_material_, params_.gore, params_.gravity);
	}
	int gore_chains() const { return people_ ? people_->bodies().gore_chains() : 0; }
	godot::PackedVector3Array body_places() const { return people_ ? people_->bodies().places() : godot::PackedVector3Array(); }
	int gore_bones() const { return people_ ? people_->bodies().gore_bones() : 0; }
	void set_ruin_maker(RuinPieces::Maker maker) { ruins_.set_maker(std::move(maker)); }
	void set_wrecks(const std::vector<godot::Ref<godot::Mesh>> &bodies, const std::vector<godot::Vector3> &sizes); // the burnt bodies, by vehicle type
	void reset(); // a new city or a load: every effect gone
	void release();
	// The height-sample rectangles (x0, z0, x1, z1) the terrain composed again since the last frame (ChangeLog::terrain_rects): the
	// craters whose decal stands on them are laid again at the next change of the crater list. Before sync, each frame.
	void touch_terrain(const CityState &s, const std::vector<int> &rects);
	// One frame, the caller holding the map shared: plays what happened since the last, then steps the effects; the ruins' pieces
	// it hands the server come within the frame's admission (its deadline and byte ledger).
	void sync(CityState &s, FrameAdmission &admission);
	float shake() const { return shake_; } // metres the camera moves now
	float since_blast() const { return blast_at_ < 0 ? -1.f : float(now_ - blast_at_); } // effect seconds since the last blast (-1 none)
	bool busy() const {
		return puffs_.active(now_) || !sources_.empty() || debris_.moving() || ruins_.moving() || ropes_.moving() || !arcs_.empty() || flash_left_ > 0;
	}
	int heave_pieces() const { return heave_.pieces(); }
	int wires_down() const { return ropes_.count(); }
	int pieces() const { return debris_.count(); }
	int ruin_pieces() const { return ruins_.pieces(); }
	const std::unordered_set<uint32_t> &broken() const { return ruins_.broken(); }
	std::vector<int64_t> take_repaint() { return ruins_.take_repaint(); }
	int bodies() const { return people_ ? people_->bodies().count() : 0; }
	int bodies_moving() const { return people_ ? people_->bodies().moving() : 0; }
	int ruin_pieces_flying() const { return ruins_.flying(); }
	uint32_t ruin_digest(uint32_t seed) const { return ruins_.digest(seed); }
	int blood_marks() const { return marks_.blood(); }
	size_t events_played() const { return played_; }
	const std::vector<DamageEvent> &played() const { return events_played_; } // this frame's

private:
	struct Geyser {
		float x = 0, y = 0, z = 0;
		double until = 0;
		float carry = 0;
		bool weak = false; // a burst pipe's spurt (LEAK), not a hydrant's jet
	};
	struct Arc { // an electric discharge, repeated a while
		godot::Vector3 at;
		double until = 0, next = 0;
	};
	void apply(const DamageEvent &e, const CityState &s);
	void blast(const DamageEvent &e, const CityState &s);
	void building_hit(const DamageEvent &e);
	void building_scar(const DamageEvent &e, const CityState &s); // effect_ruins.cpp
	void building_down(const DamageEvent &e, const CityState &s);
	void pile(const Ruin &r, bool falling, bool mound, bool loose); // the heap a building left (effect_ruins.cpp): its mound and the plain pieces on it
	void rebuild_ruins(const CityState &s); // every ruin of the state as a heap lying (open, undo, redo)
	static float heap_of(const Ruin &r); // how high its mound stands
	void furniture(const DamageEvent &e, const CityState &s);
	void pole(const DamageEvent &e); // a pole or a signal post knocked out (effect_things.cpp)
	void body(const DamageEvent &e, const CityState &s); // the body of someone struck, the pieces cut off it, what came out
	void organs(const godot::Vector3 &at, int count, const godot::Vector3 &kick); // what was inside, thrown (effect_bodies.cpp)
	void tracer(const DamageEvent &e); // a bullet's line and where it ends
	void wound(const DamageEvent &e);
	void drip(const DamageEvent &e);
	void vehicle_hit(const DamageEvent &e);
	void vehicle(const DamageEvent &e, const CityState &s);
	void casualty(const DamageEvent &e, const CityState &s);
	void burn_fires(const CityState &s, float dt);
	void step_geysers(float dt);
	void step_arcs();
	void wire(const DamageEvent &e); // a wire torn loose, hanging from the pole left standing (ropes.h)
	void arc(const godot::Vector3 &at, float seconds) { arcs_.push_back({ at, now_ + double(seconds), now_ }); }
	// The one light flashes `colour` at `at`: a flash of another colour brighter than FLASH_YIELD of this one keeps it.
	void flash_at(const godot::Vector3 &at, const godot::Color &colour, float energy, float range, float life);
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
	Heave heave_;
	GroundCells ground_; // what the pieces in the air come down on
	Ropes ropes_;
	WireSeats seats_; // where a pole's wires sit (streets.json wires): a torn wire hangs from them
	RuinPieces ruins_;
	PeopleRenderer *people_ = nullptr; // whose bodies fall (render_world.cpp)
	static constexpr uint64_t EFFECTS_STREAM = 0xb1a57ULL; // the effects' own stream, the same every run
	Rng rng_{ EFFECTS_STREAM };
	godot::RID scenario_, light_, light_inst_;
	std::vector<DamageEvent> events_, events_played_; // to play; played this frame
	struct Source {
		float x = 0, y = 0, z = 0, size = 0, rate = 0, carry = 0;
		double until = 0;
	};
	std::vector<Source> sources_;
	std::vector<Geyser> geysers_;
	std::vector<Arc> arcs_;
	std::unordered_map<int, float> fire_carry_;
	std::chrono::steady_clock::time_point last_;
	bool started_ = false;
	double now_ = 0, blast_at_ = -1;
	float shake_ = 0, flash_left_ = 0, flash_life_ = 0, flash_energy_ = 0, flash_range_ = 0;
	godot::Vector3 flash_at_;
	godot::Color flash_colour_, flash_sent_;
	static constexpr uint64_t UNSEEN = ~uint64_t(0);
	uint64_t craters_seen_ = UNSEEN, crater_layout_seen_ = UNSEEN, ruins_seen_ = UNSEEN, scars_seen_ = UNSEEN; // DamageState's revisions drawn
	size_t played_ = 0;
	float ground_here_ = 0; // the ground under the heap being laid
};

} // namespace city
