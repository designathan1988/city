// A building's wear share from its age (wear.h): new for the grace, then a step at a time over the span, each step taken at the
// half past the one before (the share is the age's share rounded to its step, Roadcraft decayOf).
#include "render/buildings/wear.h"

#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr double HALF_STEP = 0.5; // a share rounds to its nearest step: the next one is taken half a step on
}

Wear wear_of(const WearLook &K, double built, double now, double span_min) {
	Wear w;
	if (span_min <= 0 || K.steps <= 0 || !std::isfinite(now) || !std::isfinite(built)) {
		w.next_min = WEAR_NEVER_MIN;
		return w;
	}
	const double steps = double(K.steps);
	const double age = std::clamp((now - built - K.grace_min) / span_min, 0.0, 1.0);
	const double step = std::round(age * steps);
	w.share = float(step / steps);
	w.next_min = step >= steps ? WEAR_NEVER_MIN : built + K.grace_min + span_min * (step + HALF_STEP) / steps;
	return w;
}

float wear_tone(const WearLook &K, float share, float stain_floor) {
	return (1 - K.far_k * std::max(share, stain_floor)) / (1 - K.far_k * stain_floor);
}

bool WearRule::load(const WearLook &K, const godot::Dictionary &materials, godot::PackedStringArray &errors) {
	const int64_t before = errors.size();
	look = &K;
	DataReader rm{ errors, "materials.json" };
	const godot::Dictionary shared = rm.table(materials, "_shared");
	DataReader rs = rm.in("_shared");
	const godot::Dictionary finishes = rs.table(shared, "finishes");
	DataReader rf = rs.in("finishes");
	const godot::Array stain = rf.list(finishes, "weather_stain");
	if (errors.size() != before) return false;
	stain_floor = float(rf.in("weather_stain").num_at(stain, 0));
	return errors.size() == before;
}

float WearRule::share(double built, double now, double span_min, double &due) const {
	const Wear w = wear_of(*look, built, now, span_min);
	due = std::min(due, w.next_min);
	return w.share;
}

} // namespace city::b40
