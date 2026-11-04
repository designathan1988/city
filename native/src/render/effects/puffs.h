// The puff pool: smoke, dust, flame, sparks, grit, blood spray, water and paper. A ring buffer of `pool` billboards, one
// slot written at birth and nothing after: render/shaders/puffs.gdshader moves, grows and fades each from its age.
#pragma once

#include "core/random.h"
#include "render/effects/mm_layer.h"

namespace city {

// Puff kinds, the shader's.
enum PuffKind : int { PUFF_EXHAUST = 0, PUFF_DUST, PUFF_SMOKE, PUFF_CONCRETE, PUFF_BLOOD, PUFF_FLAME, PUFF_SPARK, PUFF_GRIT, PUFF_WATER, PUFF_PAPER };

class Puffs {
public:
	void init(const godot::RID &scenario, const godot::RID &material, int pool);
	void reset(); // every puff gone
	void release();
	// `count` puffs of `kind` over `spread` round (x, y, z), a size and a life each varied (as the reference does).
	void burst(float x, float y, float z, int count, int kind, float spread, float size, float life);
	void set_wind(float x, float z) { wind_ = godot::Vector2(x, z); }
	// Advances the clock to `now` (seconds) and sends what was written.
	void update(double now);
	bool active(double now) const { return now < last_death_; }

private:
	MmLayer layer_;
	godot::RID material_;
	godot::Vector2 wind_;
	Rng rng_{ 0x9ef0d51ULL };
	int pool_ = 0, next_ = 0;
	bool dirty_ = false;
	double now_ = 0, last_death_ = 0;
};

} // namespace city
