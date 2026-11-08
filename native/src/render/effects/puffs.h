// The puff pool: smoke, dust, flame, sparks, grit, blood spray, water and paper. A ring buffer of `pool` billboards, one
// slot written at birth and nothing after: render/shaders/puffs.gdshader moves, grows and fades each from its age.
#pragma once

#include "render/effects/effect_params.h"

#include "core/random.h"
#include "render/effects/mm_layer.h"

namespace city {

constexpr uint64_t PUFF_SEED = 0x9ef0d51ULL; // the puffs' own random stream: any fixed seed

// Puff kinds (PuffKind) are core/shader_contracts/effects.h's, which puffs.gdshader reads.

class Puffs {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const EffectParams &params);
	void reset(); // every puff gone
	void release();
	// `count` puffs of `kind` over `spread` round (x, y, z), a size and a life each varied (as the reference does).
	void burst(float x, float y, float z, int count, int kind, float spread, float size, float life);
	void set_wind(float x, float z) { wind_ = godot::Vector2(x, z); }
	// The frame's clock, `now` seconds: what is born from here on is stamped with it, and the shader ages every puff by it.
	void stamp(double now);
	// What this frame wrote, sent once; the layer drawn only while a puff lives (a dead one is drawn nowhere by the shader).
	void flush();
	bool active(double now) const { return now < last_death_; }

private:
	EffectParams params_; // effects.json: the pool and the puffs' spread
	MmLayer layer_;
	godot::RID material_;
	godot::Vector2 wind_;
	Rng rng_{ PUFF_SEED };
	int pool_ = 0, next_ = 0;
	bool dirty_ = false, shown_ = true;
	double now_ = 0, last_death_ = 0;
};

} // namespace city
