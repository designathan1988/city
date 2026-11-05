// The shape of a block's roof over its eaves: read by what draws it, what stands on it and what clicks it. Lot frame, metres.
#pragma once

#include "core/units.h"
#include "sim/buildings/b40/types.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

constexpr float DEFAULT_GABLE_DEG = 30.f, DEFAULT_SHED_DEG = 12.f, SAW_STEP_M = 6.f;

inline float roof_slope(const Volume &v) {
	float pitch = v.pitch > 0 ? v.pitch : v.roof == ROOF_SHED ? DEFAULT_SHED_DEG : DEFAULT_GABLE_DEG;
	return std::tan(pitch * float(DEG_TO_RAD));
}

inline bool ridge_along_x(const Volume &v) { return v.ridge >= 0 ? v.ridge == 0 : v.w >= v.d; }

// The depth of one tooth of a sawtooth roof: the roof shared evenly in teeth of about SAW_STEP_M.
inline float saw_step(const Volume &v) {
	float step = std::min(SAW_STEP_M, v.d);
	return v.d / float(std::max(1, int(std::round(v.d / step))));
}

// How far the roof rises over the eaves at its highest.
inline float roof_rise(const Volume &v) {
	float slope = roof_slope(v);
	switch (v.roof) {
		case ROOF_GABLE: case ROOF_HIP: return (ridge_along_x(v) ? v.d : v.w) / 2 * slope;
		case ROOF_SHED: return (v.fall == 1 || v.fall == 3 ? v.w : v.d) * slope;
		case ROOF_SAWTOOTH: return saw_step(v) / 2 * slope;
		default: return 0;
	}
}

// Its height over the eaves at a point of the plan.
inline float roof_height_at(const Volume &v, float x, float y) {
	float slope = roof_slope(v), x0 = x - v.x, x1 = v.x + v.w - x, y0 = y - v.y, y1 = v.y + v.d - y;
	switch (v.roof) {
		case ROOF_GABLE: return (ridge_along_x(v) ? std::min(y0, y1) : std::min(x0, x1)) * slope;
		case ROOF_HIP: return std::min({ x0, x1, y0, y1 }) * slope;
		case ROOF_SHED: return (v.fall == 2 ? y1 : v.fall == 1 ? x1 : v.fall == 3 ? x0 : y0) * slope;
		case ROOF_SAWTOOTH: {
			float step = saw_step(v);
			return std::fmod(std::max(y0, 0.f), step) / 2 * slope;
		}
		default: return 0;
	}
}

} // namespace city::b40
