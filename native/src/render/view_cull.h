// What the camera sees this frame, for whoever fills a MultiMesh by hand. A MultiMesh has one box and the engine culls it whole, so
// an instance out of the view still pays every vertex of its mesh: the fleet behind the camera cost as much as the one before it.
#pragma once

#include "render/projected_error.h"

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace city {

struct ViewCull {
	static constexpr int MAX_PLANES = 6;
	godot::Plane planes[MAX_PLANES]; // normals pointing out of the view (Projection::get_projection_planes)
	ProjectedErrorView error_view;
	int count = 0; // none (no camera): everything passes

	void read(const godot::Camera3D &camera) {
		error_view.read(camera);
		const godot::TypedArray<godot::Plane> list = camera.get_frustum();
		count = 0;
		for (int i = 0; i < list.size() && i < MAX_PLANES; i++) planes[count++] = list[i];
	}
	bool same_as(const ViewCull &other) const {
		if (count != other.count) return false;
		for (int i = 0; i < count; i++)
			if (planes[i] != other.planes[i]) return false;
		return true;
	}
	bool sees(const godot::Vector3 &p, float radius) const {
		for (int i = 0; i < count; i++)
			if (planes[i].distance_to(p) > radius) return false;
		return true;
	}
};

} // namespace city
