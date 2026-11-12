// The puff pool: smoke, dust, flame, sparks, grit, blood spray, water and paper. A ring buffer of `pool` billboards, one
// logical slot written at birth; only live slots are packed for drawing. The shader moves, grows and fades each from its age.
#pragma once

#include "core/random.h"
#include "render/effects/mm_layer.h"

#include <set>
#include <vector>

namespace city {

// Puff kinds, the shader's.
enum PuffKind : int { PUFF_EXHAUST = 0, PUFF_DUST, PUFF_SMOKE, PUFF_CONCRETE, PUFF_BLOOD, PUFF_FLAME, PUFF_SPARK, PUFF_GRIT, PUFF_WATER, PUFF_PAPER };

class Puffs {
public:
	void init(const godot::RID &scenario, const godot::RID &material, int pool, float sector_m = 0);
	void reset(); // every puff gone
	void release();
	// `count` puffs of `kind` over `spread` round (x, y, z), a size and a life each varied (as the reference does).
	void burst(float x, float y, float z, int count, int kind, float spread, float size, float life);
	void set_wind(float x, float z) { const godot::Vector2 next(x, z); if (next != wind_) { wind_ = next; dirty_ = true; } }
	// Clock/retirement before births; one upload after every producer has finished.
	void begin_frame(double now);
	void flush();
	bool active(double now) const { return now < last_death_; }

private:
	struct Puff {
		godot::Vector3 at;
		float size = 0, born = 0, kind = 0, seed = 0, life = 0;
		double expiry = 0;
	};
	std::vector<Puff> slots_;
	std::set<int> live_; // original ring-slot draw order, independent of dense GPU row
	std::set<std::pair<double, int>> deaths_; // one entry per live slot, replaced on ring overwrite
	MmLayer layer_;
	godot::AABB lifetime_bounds(const Puff &p) const;
	godot::RID material_;
	godot::Vector2 wind_;
	Rng rng_{ 0x9ef0d51ULL };
	int pool_ = 0, next_ = 0;
	bool dirty_ = false;
	double now_ = 0, last_death_ = 0;
};

} // namespace city
