// The pieces of a building as it comes down (the 4.0's destruction.ts): the walls of the building itself, cut in a job (ruin_fracture.h), one mesh a
// ruin. What the blast reaches is thrown; the break runs on through the bonds between the pieces, and whatever no longer hangs from the ground by them
// falls (Blast's support graph, the world as a node). What rests on the ground stays. The pieces are matrices in a small texture a ruin
// (building_ruin.gdshader), written only while something moves: a ruin is one draw however many pieces fly.
#pragma once

#include "render/buildings/ruin_fracture.h"
#include "render/effects/puffs.h"
#include "render/effects/rigid_step.h"
#include "sim/state/city_state.h"

#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <functional>
#include <map>
#include <memory>
#include <mutex>

namespace city {

class RuinPieces {
public:
	// What cuts a building: run on a job's thread.
	using Maker = std::function<std::shared_ptr<RuinMesh>(const b40::Data &, const Building &, const HeightPatch &)>;
	struct Blow {
		godot::Vector3 at;
		float force = 0, heap = 0; // the bomb's, 1..50; how high its mound stands
	};

	void init(const godot::RID &scenario, const godot::Ref<godot::ShaderMaterial> &material, const EffectParams &params);
	void set_maker(Maker maker) { maker_ = std::move(maker); }
	void reset();
	void release() { reset(); }
	// The building of `r` comes down. False with no room for more pieces: it falls as a plain heap.
	bool fall(const CityState &s, const Ruin &r, const Blow &blow);
	void keep_only(const std::vector<Ruin> &ruins); // an undo took some ruins back
	bool holds(uint32_t seed) const { return falls_.count(seed) > 0; }
	void update(float dt, Puffs &puffs);
	std::vector<Ruin> take_uncut(); // what had no walls to cut (a design of its own): it falls as a plain heap after all
	bool moving() const;
	int pieces() const;
	int flying() const;

private:
	struct Piece : RigidPiece {
		godot::Vector3 home;
		float fall_at = 0, radius = 0, power = 0, aloft = 0;
		int first_link = 0, links = 0;
		bool loose = false, grounded = false, scheduled = false;
	};
	struct Fall {
		Ruin ruin;
		Blow blow;
		float ground = 0, clock = 0, support_in = 0;
		std::shared_ptr<RuinMesh> cut;
		std::vector<Piece> pieces;
		godot::RID inst, texture;
		godot::Ref<godot::ShaderMaterial> material;
		godot::PackedByteArray rows;
		int left = 0, rows_of_pieces = 0; // pieces yet to come to rest
		bool ready = false, dirty = false;
	};
	struct Inbox {
		std::mutex mutex;
		std::vector<std::pair<uint32_t, std::shared_ptr<RuinMesh>>> done;
	};
	void adopt(Fall &f, const std::shared_ptr<RuinMesh> &cut);
	void make_room(int64_t vertices);
	void schedule(Fall &f);
	void step(Fall &f, Puffs &puffs);
	void let_go(Fall &f, Piece &p, const godot::Vector3 &from, float power, Puffs &puffs);
	void drop_unsupported(Fall &f, Puffs &puffs);
	void write(Fall &f, size_t i);
	void upload(Fall &f);
	void free_fall(Fall &f);
	float top(const Fall &f, float x, float z) const;

	godot::RID scenario_;
	godot::Ref<godot::ShaderMaterial> material_;
	EffectParams law_;
	Maker maker_;
	std::shared_ptr<Inbox> inbox_ = std::make_shared<Inbox>();
	std::map<uint32_t, Fall> falls_; // by the ruin's seed
	std::vector<Ruin> uncut_;
	std::vector<int> stack_;
	std::vector<uint8_t> held_;
	Rng rng_{ 0x2a1f3ULL };
	float clock_ = 0;
	int dust_left_ = 0; // bursts this frame may still make
};

} // namespace city
