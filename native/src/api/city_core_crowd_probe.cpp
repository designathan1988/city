// `probes` area (`walkers_apart`): the crowd's rule for the room between two walkers (sim/traffic/ped_apart.h) on bodies a test
// lays out: each one's place, the step it would take this tick (in the world) and how far either side of that step's line it may
// give way; back come two bodies' width and the step each takes. A scene of its own: no city, no clock, the same bodies give the
// same steps.
#include "api/city_probes.h"

#include "sim/state/city_state.h"
#include "sim/traffic/ped_apart.h"
#include "sim/traffic/ped_stripes.h"

#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

Dictionary CityProbes::walkers_apart(const PackedVector2Array &at, const PackedVector2Array &steps, const PackedFloat32Array &aside) const {
	const float touch = 2 * state_->catalog.traffic.ped_body_m;
	std::vector<traffic::Stride> strides(size_t(std::min({ at.size(), steps.size(), aside.size() })));
	for (size_t i = 0; i < strides.size(); i++) {
		traffic::Stride &s = strides[i];
		const V2 step(steps[int64_t(i)].x, steps[int64_t(i)].y);
		s.at = V2(at[int64_t(i)].x, at[int64_t(i)].y);
		s.far = step.len();
		if (s.far > 0) s.ahead = step / s.far;
		s.left = traffic::left_of(s.ahead);
		s.lo = -aside[int64_t(i)];
		s.hi = aside[int64_t(i)];
	}
	traffic::ApartRoom room;
	traffic::keep_apart(strides, touch, room);
	PackedVector2Array taken;
	for (const traffic::Stride &s : strides) taken.push_back(Vector2(s.taken.x, s.taken.y));
	Dictionary d;
	d["touch"] = touch;
	d["taken"] = taken;
	return d;
}

} // namespace city
