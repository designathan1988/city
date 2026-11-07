// A record's walls as a job draws them, handed to the fracture (ruin_fracture.h).
#include "render/buildings/building_bodies.h"
#include "render/buildings/ruin_fracture.h"

namespace city {

std::shared_ptr<RuinMesh> ruin_of(BuildingBodies &bodies, const b40::Data &data, const b40::BuildingLook &look, const Building &rec, const HeightPatch &ground) {
	b40::Placed pl;
	bodies.place(data, look, ground, rec, pl);
	b40::Shell shell, glass, aside; // railings, lanterns and lawns do not come down as walls
	glass.rooms = true;
	shell.glass = &glass;
	shell.sheer = &aside;
	shell.cards = &aside;
	shell.washes = &aside;
	shell.lawns = &aside;
	b40::emit_facades(shell, shell, pl);
	b40::emit_roofs(shell, shell, pl);
	std::vector<RuinSlab> slabs;
	for (const b40::Volume &v : pl.b->volumes)
		for (int level = v.base + 1; level < v.top(); level++) {
			RuinSlab ring;
			for (const b40::Side &sd : b40::sides_of(pl, v, pl.b->block_z(v, level))) ring.push_back(sd.f.o);
			slabs.push_back(std::move(ring));
		}
	return fracture_shell(shell, glass, slabs, rec.seed);
}

} // namespace city
