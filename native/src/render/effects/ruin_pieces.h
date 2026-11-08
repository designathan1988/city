// The pieces of a building as it comes down (the 4.0's destruction.ts), drawn: the walls of the building itself, cut in a job (ruin_fracture.h), one mesh a ruin, moved by
// RuinSim. The pieces are matrices in a small texture a ruin (building_ruin.gdshader), written only while something moves: a ruin is one draw however many pieces fly.
// A ruin read from the state (open, undo, redo) comes to rest in its job and is laid at once: the same heap the player saw fall.
#pragma once

#include "render/effects/puffs.h"
#include "render/effects/ruin_sim.h"
#include "render/frame_admission.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_set>

namespace city {

constexpr uint64_t PIECES_SEED = 0x2a1f3ULL; // the falling pieces' own random stream: any fixed seed

class RuinPieces {
public:
	// What cuts a building: run on a job's thread.
	using Maker = std::function<std::shared_ptr<RuinMesh>(const b40::Data &, const Building &, const HeightPatch &)>;
	struct Uncut { // what had no walls to cut (a design of its own): a plain heap after all
		Ruin ruin;
		bool falling = false;
	};

	void init(const godot::RID &scenario, const godot::Ref<godot::ShaderMaterial> &material, const EffectParams &params);
	void set_maker(Maker maker) { maker_ = std::move(maker); }
	void reset();
	void release() { reset(); }
	// The building of `r`, its mound `heap` high, in pieces: `falling` before the player, else lying as it came to rest. False with no room: a plain heap.
	bool add(const CityState &s, const Ruin &r, float heap, bool falling);
	// The building of `r`, standing, its `scars` taken out of it: `falling` before the player, else lying as they came to rest.
	void scar(const CityState &s, const Ruin &r, const std::vector<Scar> &scars, bool falling);
	void keep_only(const std::vector<Ruin> &ruins, const std::map<uint32_t, size_t> &scarred); // an undo took some back: by seed, how many scars
	const std::unordered_set<uint32_t> &broken() const { return broken_; } // standing, drawn here and no longer by the buildings
	std::vector<int64_t> take_repaint(); // the chunks whose buildings changed hands
	bool holds(uint32_t seed) const { return falls_.count(seed) > 0; }
	// One frame: the finished cuts adopted while the frame's admission does (its deadline and byte ledger; the rest wait, in the order
	// they came), then the pieces stepped.
	void update(float dt, Puffs &puffs, FrameAdmission &admission);
	std::vector<Uncut> take_uncut();
	bool moving() const;
	int pieces() const;
	int flying() const;
	uint32_t digest(uint32_t seed) const; // where the pieces of that ruin lie, 0 before they come to rest

private:
	struct Fall {
		std::shared_ptr<RuinSim> sim;
		std::shared_ptr<RuinMesh> cut;
		Ruin ruin;
		godot::RID inst, texture;
		godot::Ref<godot::ShaderMaterial> material;
		godot::PackedByteArray rows;
		int rows_of_pieces = 0;
		bool falling = false, ready = false, dirty = false, standing = false;
		std::vector<Scar> scars; // of a standing building
		size_t scars_done = 0;
		float heap_after = -1; // it came down while its pieces were being cut: this high
	};
	struct Done {
		uint32_t seed = 0;
		std::shared_ptr<RuinMesh> cut;
		std::shared_ptr<RuinSim> sim;
		size_t scars_done = 0;
	};
	struct Inbox {
		std::mutex mutex;
		std::vector<Done> done;
	};
	void start(const CityState &s, Fall &f, float heap); // its pieces cut in a job
	bool room();
	void take_done(FrameAdmission &admission);
	void adopt(Fall &f, const Done &done);
	void make_room(int64_t vertices);
	void dust(const std::vector<RuinSim::Note> &notes, Puffs &puffs);
	void upload(Fall &f);
	void free_fall(Fall &f);

	godot::RID scenario_;
	godot::Ref<godot::ShaderMaterial> material_;
	EffectParams law_;
	Maker maker_;
	std::shared_ptr<Inbox> inbox_ = std::make_shared<Inbox>();
	std::map<uint32_t, Fall> falls_; // by the ruin's seed
	std::vector<Uncut> uncut_;
	std::unordered_set<uint32_t> broken_;
	std::vector<int64_t> repaint_;
	std::vector<RuinSim::Note> notes_;
	Rng rng_{ PIECES_SEED };
	float clock_ = 0;
	int dust_left_ = 0; // bursts this frame may still make
};

} // namespace city
