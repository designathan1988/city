// The one owner of level of detail: how many pixels a geometric error spans on the screen at a distance, for the live camera
// (field of view, viewport height, orthographic size). Every family chooses its levels here, by each level's own error in
// metres against one allowed error in pixels: no switch distance is tuned by hand anywhere else. The allowed error is half a
// pixel (data: render.detail_error_px): an error under half a pixel cannot change which samples a surface covers, so a level
// that coarse looks the same, and a switch between two levels whose difference is under it cannot be seen (no hysteresis).
#pragma once

#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <limits>

namespace city {

// What one frame sees, set once per frame by CityCore from the camera (api/city_core.cpp) and read by every renderer.
struct DetailView {
	godot::Vector3 eye;
	double px_per_m_at_1m = 0; // perspective: viewport span / (2 tan(fov / 2)); what a metre spans one metre away
	double ortho_px_per_m = 0; // orthographic: what a metre spans anywhere (0 in perspective)
	double error_px = 0; // the most a level may be off on the screen (data: render.detail_error_px)

	bool ready() const { return error_px > 0 && (px_per_m_at_1m > 0 || ortho_px_per_m > 0); }
	// Pixels an error of `error_m` spans at `distance_m` from the eye.
	double px_of(double error_m, double distance_m) const {
		if (ortho_px_per_m > 0) return error_m * ortho_px_per_m;
		return error_m * px_per_m_at_1m / (distance_m > NEAREST_M ? distance_m : NEAREST_M);
	}
	// The distance from which a level that is `error_m` off may be drawn: nearer than this its error would show, and the finer
	// level is drawn. Infinite in an orthographic view whose scale shows the error, 0 where it does not.
	double reach_of(double error_m) const {
		if (ortho_px_per_m > 0) return error_m * ortho_px_per_m <= error_px ? 0 : std::numeric_limits<double>::infinity();
		return error_m * px_per_m_at_1m / error_px;
	}
	// Whether a level that is `error_m` off is fine at `distance_m`.
	bool fits(double error_m, double distance_m) const { return distance_m >= reach_of(error_m); }
	// The distance from the eye to the nearest point of a box (0 inside it): a level is judged where its geometry is nearest.
	double distance_to(const godot::AABB &box) const;
	double distance_to(double minx, double miny, double minz, double maxx, double maxy, double maxz) const;

	static constexpr double NEAREST_M = 0.01; // a distance below this is taken as this: the projection is singular at the eye
};

} // namespace city
