// The walls round a body (body_walls.h).
#include "render/agents/body_walls.h"

#include "sim/buildings/b40/record.h"

using namespace godot;

namespace city {

std::vector<BodyWall> walls_near(const CityState &s, float x, float z, float reach) {
	std::vector<BodyWall> out;
	s.buildings.query(x - reach, z - reach, x + reach, z + reach, [&](int id) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (rec.floors == 0) return;
		const std::shared_ptr<const b40::Body> held = b40::record_mass(s.buildings.designs, rec);
		const b40::Body &b = *held;
		const b40::LotFrame f = b40::lot_frame(rec);
		const float ground = s.terrain.height_at(rec.x, rec.z);
		for (const b40::Volume &v : b.volumes)
			out.push_back(BodyWall{ Vector2(f.o.x, f.o.y), Vector2(f.e0.x, f.e0.y), Vector2(f.e1.x, f.e1.y), v.x, v.y, v.x + v.w, v.y + v.d, ground + b.block_z(v, v.base + v.storeys) });
	});
	return out;
}

} // namespace city
