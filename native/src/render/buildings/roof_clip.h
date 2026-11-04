// The part of a pitched roof that a taller volume of the same body encloses. Intact, that volume's walls hide it; a body cut
// open shows the floor inside those walls, so the roof's geometry there would stand in the rooms: it is left out, clipped
// exactly on the walls' plane (convex pieces of each polygon, no shader discard).
#pragma once

#include "render/buildings/facade.h"

#include <utility>
#include <vector>

namespace city::b40 {

class RoofHide {
public:
	// The volumes of `pl`'s body that stand over `v`'s eaves (base below them, top above them); none unless the body is cut open.
	RoofHide(const Placed &pl, const Volume &v);
	bool any() const { return !rects_.empty(); }

	// The same primitives as Shell, minus what is enclosed.
	void tri(Shell &s, const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &out, const Lin &lin,
			int8_t fin) const;
	void quad(Shell &s, const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &d,
			const godot::Vector3 &out, const Lin &lin, int8_t fin) const;
	// The spans of [u0, u1] along `axis` (a plan axis of the lot) from `from` that lie outside every enclosing volume.
	std::vector<std::pair<float, float>> spans(const godot::Vector3 &from, const godot::Vector3 &axis, float u0, float u1) const;

private:
	using Poly = std::vector<godot::Vector3>;
	struct Rect {
		float x0, x1, y0, y1;
	};
	float plan_x(const godot::Vector3 &p) const { return (p - o_).dot(ex_); }
	float plan_y(const godot::Vector3 &p) const { return (p - o_).dot(ey_); }
	void emit(Shell &s, const Poly &poly, const godot::Vector3 &out, const Lin &lin, int8_t fin) const;
	std::vector<Poly> outside(const Poly &poly) const;

	std::vector<Rect> rects_;
	godot::Vector3 o_, ex_, ey_;
};

} // namespace city::b40
