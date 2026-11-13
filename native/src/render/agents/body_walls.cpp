// The walls round a body (body_walls.h).
#include "render/agents/body_walls.h"

#include "sim/buildings/b40/record.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
// The lot's elements a body strikes as it would a wall: walls, fences, railings, hedges, sheds and stalls (their boxes, b40/types.h).
bool stands_in_the_way(b40::Element kind) {
	return kind == b40::EL_WALL || kind == b40::EL_FENCE || kind == b40::EL_RAILING || kind == b40::EL_HEDGE || kind == b40::EL_SHED || kind == b40::EL_STALL;
}
} // namespace

std::vector<BodyWall> walls_near(const CityState &s, float x, float z, float reach) {
	std::vector<BodyWall> out;
	s.buildings.query(x - reach, z - reach, x + reach, z + reach, [&](int id) {
		const Building &rec = s.buildings.items[size_t(id)];
		const std::shared_ptr<const b40::Body> held = b40::record_mass(s.buildings.designs, rec);
		const b40::Body &b = *held;
		const b40::LotFrame f = b40::lot_frame(rec);
		const float ground = s.terrain.height_at(rec.x, rec.z);
		const Vector2 o(f.o.x, f.o.y), e0(f.e0.x, f.e0.y), e1(f.e1.x, f.e1.y);
		if (rec.floors != 0)
			for (const b40::Volume &v : b.volumes)
				out.push_back(BodyWall{ o, e0, e1, v.x, v.y, v.x + v.w, v.y + v.d, ground + b.block_z(v, v.base + v.storeys) });
		for (const b40::LotElement &el : b.elements) { // a box about its middle, turned by its angle in the lot's frame
			if (!stands_in_the_way(el.kind)) continue;
			const float c = std::cos(el.angle), n = std::sin(el.angle);
			const Vector2 along = e0 * c + e1 * n, deep = e1 * c - e0 * n;
			out.push_back(BodyWall{ o + e0 * el.x + e1 * el.y, along, deep, -el.w * 0.5f, -el.d * 0.5f, el.w * 0.5f, el.d * 0.5f, ground + el.z + el.h });
		}
	});
	return out;
}

} // namespace city
