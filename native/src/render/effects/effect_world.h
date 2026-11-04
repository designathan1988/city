// What a blow looks like: smoke, fire, flung pieces, ground marks, the flash, the ring and the camera's shake. The owner of
// the effects (render/effects/*): it takes the events of the simulation's blows (damage_event.h) from the ChangeLog, plays
// each once, draws the craters and the fires the damage state holds, and steps everything with its own clock. Reads the
// city, never writes it (render is read only).
#pragma once

#include "render/effects/debris.h"
#include "render/effects/effect_params.h"
#include "render/effects/marks.h"
#include "render/effects/puffs.h"
#include "render/render_context.h"
#include "sim/state/city_state.h"

#include <chrono>
#include <unordered_map>
#include <vector>

namespace city {

class EffectWorld {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset(); // a new city or a load: every effect gone
	void release();
	// One frame, the caller holding the map shared: plays what happened since the last, then steps the effects.
	void sync(CityState &s);
	float shake() const { return shake_; } // metres the camera moves now
	bool busy() const { return puffs_.active(now_) || debris_.moving() || flash_left_ > 0; }
	int pieces() const { return debris_.count(); }
	size_t events_played() const { return played_; }

private:
	struct Geyser {
		float x = 0, y = 0, z = 0;
		double until = 0;
		float carry = 0;
	};
	void apply(const DamageEvent &e, const CityState &s);
	void blast(const DamageEvent &e, const CityState &s);
	void building_hit(const DamageEvent &e);
	void building_down(const DamageEvent &e);
	void furniture(const DamageEvent &e, const CityState &s);
	void vehicle(const DamageEvent &e, const CityState &s);
	void casualty(const DamageEvent &e, const CityState &s);
	void burn_fires(const CityState &s, float dt);
	void step_geysers(float dt);
	void flash(float radius);
	void light_update(float dt);
	void piece(const godot::Vector3 &at, const godot::Vector3 &size, const godot::Vector3 &v, const godot::Color &tint, bool cylinder = false, float burn = 0.f);

	EffectParams params_;
	Puffs puffs_;
	Marks marks_;
	Debris debris_;
	Rng rng_{ 0xb1a57ULL };
	godot::RID scenario_, light_, light_inst_;
	std::vector<DamageEvent> events_;
	std::vector<Geyser> geysers_;
	std::unordered_map<int, float> fire_carry_;
	std::chrono::steady_clock::time_point last_;
	bool started_ = false;
	double now_ = 0;
	float shake_ = 0, flash_left_ = 0, flash_energy_ = 0, flash_range_ = 0;
	godot::Vector3 flash_at_;
	size_t craters_seen_ = ~size_t(0), played_ = 0;
	int city_serial_ = -1;
};

} // namespace city
