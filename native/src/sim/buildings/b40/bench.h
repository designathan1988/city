// A bench as a part of a lot, its one owner: the way it looks is its element's facing (types.h: the way its open end looks), its seat
// runs across that way, and its back stands on the side away from it. The layouts lay it so (the lots' zones, the open lots), the render
// draws its back where this says (render/buildings/lot_emit.cpp) and the tests hold both (api/city_core_lot_parts_probe.cpp).
#pragma once

#include "sim/buildings/b40/element_shape.h"

namespace city::b40 {

// Whether the bench looks along the lot's y (the street or away from it) or along its x.
inline bool looks_along_y(const LotElement &e) { return e.facing == FACE_STREET || e.facing == FACE_BACK; }
// The way it looks, a unit step of the lot's frame.
inline Point2 bench_look(const LotElement &e) {
	switch (e.facing) {
		case FACE_BACK: return { 0, 1 };
		case FACE_RIGHT: return { 1, 0 };
		case FACE_LEFT: return { -1, 0 };
		default: return { 0, -1 };
	}
}
// A bench laid with its long side along the way it looks is laid wrong.
inline bool bench_across(const LotElement &e) { return looks_along_y(e) ? e.w >= e.d : e.d >= e.w; }

// The back's footprint, `thick` deep, along the side away from where the bench looks.
inline Footprint bench_back(const LotElement &e, float thick) {
	const float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	switch (e.facing) {
		case FACE_BACK: return { x0, y0, x1, y0 + thick };
		case FACE_RIGHT: return { x0, y0, x0 + thick, y1 };
		case FACE_LEFT: return { x1 - thick, y0, x1, y1 };
		default: return { x0, y1 - thick, x1, y1 };
	}
}

} // namespace city::b40
