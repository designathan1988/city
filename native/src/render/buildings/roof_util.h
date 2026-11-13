// Roof predicates shared by the near and far building emitters: one answer for every level of detail.
#pragma once

#include "sim/buildings/b40/types.h"

namespace city::b40 {

inline bool roof_flat(Roof r) { return r == ROOF_FLAT || r == ROOF_TERRACE; }

// Another block stands on this volume's top, anywhere: a terrace under one is left plain.
inline bool built_on(const Body &b, const Volume &v) {
	for (const Volume &o : b.volumes)
		if (&o != &v && o.base == v.top()) return true;
	return false;
}

// A volume standing on this volume's top covers it (another storey sits there).
inline bool covered_by_above(const Body &b, const Volume &v) {
	for (const Volume &o : b.volumes)
		if (&o != &v && o.base == v.top() && o.x < v.x + v.w && o.x + o.w > v.x && o.y < v.y + v.d && o.y + o.d > v.y) return true;
	return false;
}

// The volume whose roof carries a core's machine room: the highest flat roof over the middle of the core's square (nullptr:
// none). The near and the far level both ask here, so a machine room is on the same roof at every distance.
inline const Volume *machine_room_roof(const Body &b, const Core &c) {
	const float cx = c.x + CORE_SIDE_M / 2, cy = c.y + CORE_SIDE_M / 2;
	const Volume *best = nullptr;
	for (const Volume &o : b.volumes)
		if (roof_flat(o.roof) && cx > o.x && cx < o.x + o.w && cy > o.y && cy < o.y + o.d && (!best || o.top() > best->top())) best = &o;
	return best;
}

} // namespace city::b40
