// Roof predicates shared by the near and far building emitters.
#pragma once

#include "render/buildings/b40_types.h"

namespace city::b40 {

inline bool roof_flat(Roof r) { return r == ROOF_FLAT || r == ROOF_TERRACE; }

// A volume standing on this volume's top covers it (another storey sits there).
inline bool covered_by_above(const Body &b, const Volume &v) {
	for (const Volume &o : b.volumes)
		if (&o != &v && o.base == v.top() && o.x < v.x + v.w && o.x + o.w > v.x && o.y < v.y + v.d && o.y + o.d > v.y) return true;
	return false;
}

} // namespace city::b40
