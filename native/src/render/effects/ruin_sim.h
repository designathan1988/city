// How a building's pieces come down, with nothing drawn: the blow throws what it reaches, the break runs on through the bonds, and whatever no longer hangs from the
// ground by them falls (Blast's support graph, the world as a node). A piece at rest becomes ground for the next (`Pile`), so the heap stands as high as what fell.
// All in fixed steps from the ruin's seed and its blow: the same ruin comes to rest the same way as it falls before the player and in a job, opened from a save.
#pragma once

#include "render/buildings/ruin_fracture.h"
#include "render/effects/rigid_step.h"
#include "sim/demolition/damage_state.h"

#include "core/random.h"

#include <memory>
#include <vector>

namespace city {

class RuinSim {
public:
	struct Note { // where dust belongs
		godot::Vector3 at;
		float radius = 0;
		bool landing = false;
	};
	static constexpr int FLOATS_A_PIECE = 12; // three rows of four

	void begin(const std::shared_ptr<RuinMesh> &cut, const Ruin &ruin, float ground, float heap, const EffectParams &law);
	void step(std::vector<Note> *notes);
	void settle(); // to the end, at once
	bool resting() const { return left_ == 0; }
	int pieces() const { return int(pieces_.size()); }
	int flying() const;
	bool moved(size_t i) const { return pieces_[i].moved; }
	void clear_moved();
	void write(size_t i, float *rows) const;
	uint32_t digest() const; // where everything lies

private:
	struct Piece : RigidPiece {
		godot::Vector3 home;
		float fall_at = 0, radius = 0, power = 0, aloft = 0, flat = 0;
		int first_link = 0, links = 0;
		bool loose = false, grounded = false, scheduled = false, moved = false;
	};
	// The heap as it builds: the top of what lies at each cell.
	struct Pile {
		godot::Vector2 from;
		float cell = 1;
		int side = 0;
		std::vector<float> top;
		float at(float x, float z) const;
		void lay(const godot::Vector3 &lo, const godot::Vector3 &hi, float height);
	};
	void schedule();
	void let_go(Piece &p, const godot::Vector3 &from, float power, std::vector<Note> *notes);
	void drop_unsupported(std::vector<Note> *notes);
	void rest_on_pile(const Piece &p);
	float top(float x, float z) const;

	std::shared_ptr<RuinMesh> cut_;
	std::vector<Piece> pieces_;
	std::vector<int> stack_;
	std::vector<uint8_t> held_;
	Pile pile_;
	EffectParams law_;
	Ruin ruin_;
	Rng rng_{ 1 };
	float ground_ = 0, heap_ = 0, clock_ = 0, support_in_ = 0, cos_ = 1, sin_ = 0;
	int left_ = 0;
};

} // namespace city
