// Outward screen-space error for a vertical displacement anywhere in a world box, including perspective division.
// No distance/FOV approximation: orthographic and asymmetric camera projections use the same clip-coordinate bound.
#pragma once

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/projection.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

struct ProjectedErrorView {
	godot::Projection projection;
	godot::Transform3D view;
	godot::Vector2 pixels;
	bool ready = false;

	void read(const godot::Camera3D &camera) {
		projection = camera.get_camera_projection();
		view = camera.get_camera_transform().affine_inverse();
		pixels = camera.get_viewport() ? camera.get_viewport()->get_visible_rect().size : godot::Vector2();
		ready = pixels.x > 0 && pixels.y > 0;
	}
	bool same_as(const ProjectedErrorView &other) const {
		return ready == other.ready && (!ready || (projection == other.projection && view == other.view && pixels == other.pixels));
	}
	double vertical_error(const godot::AABB &box, double metres) const {
		if (metres == 0) return 0;
		if (!ready || !std::isfinite(metres) || metres < 0) return std::numeric_limits<double>::infinity();
		double clip[4][4] = {};
		for (int a = 0; a < 4; a++) {
			for (int k = 0; k < 3; k++) {
				const godot::Vector3 column = view.basis.get_column(k);
				for (int j = 0; j < 3; j++) clip[a][k] += double(projection[j][a]) * column[j];
			}
			clip[a][3] = projection[3][a];
			for (int j = 0; j < 3; j++) clip[a][3] += double(projection[j][a]) * view.origin[j];
		}
		double wmin = std::numeric_limits<double>::infinity(), xmax = 0, ymax = 0, terms = 1;
		for (int corner = 0; corner < 8; corner++) {
			const godot::Vector3 p = box.position + godot::Vector3(corner & 1 ? box.size.x : 0, corner & 2 ? box.size.y : 0, corner & 4 ? box.size.z : 0);
			double value[4] = {};
			for (int a : { 0, 1, 3 }) {
				value[a] = clip[a][3];
				double magnitude = std::fabs(double(projection[3][a]));
				// Include cancellation in the separate world->view->clip arithmetic, not just the final clip value.
				for (int j = 0; j < 3; j++) {
					double component = std::fabs(double(view.origin[j]));
					for (int k = 0; k < 3; k++) component += std::fabs(double(view.basis.get_column(k)[j]) * p[k]);
					magnitude += std::fabs(double(projection[j][a])) * component;
				}
				for (int k = 0; k < 3; k++) value[a] += clip[a][k] * p[k];
				terms = std::max(terms, magnitude);
			}
			wmin = std::min(wmin, value[3]);
			xmax = std::max(xmax, std::fabs(value[0])); ymax = std::max(ymax, std::fabs(value[1]));
		}
		const double padding = terms * 64 * std::numeric_limits<float>::epsilon();
		wmin -= padding;
		if (!(wmin > 0) || !std::isfinite(wmin + xmax + ymax)) return std::numeric_limits<double>::infinity();
		const double wx = std::fabs(clip[3][1]);
		const double dx = (metres * std::fabs(clip[0][1]) + 2 * padding) / wmin + (xmax + padding) * (metres * wx + 2 * padding) / (wmin * wmin);
		const double dy = (metres * std::fabs(clip[1][1]) + 2 * padding) / wmin + (ymax + padding) * (metres * wx + 2 * padding) / (wmin * wmin);
		const double error = std::hypot(dx * double(pixels.x) * .5, dy * double(pixels.y) * .5);
		return std::isfinite(error) ? error : std::numeric_limits<double>::infinity();
	}
};

} // namespace city
