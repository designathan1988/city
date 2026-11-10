// See far_casts.h.
#include "render/buildings/far_casts.h"

#include "render/buildings/cast_hull.h"

using namespace godot;

namespace city {

int64_t FarCastsBuilt::emitted() const {
	int64_t n = 0;
	for (const b40::Shell &s : shells) n += int64_t(s.triangles());
	return n;
}

int64_t FarCastsBuilt::bytes() const {
	int64_t n = 0;
	for (const b40::Shell &s : shells) n += int64_t(s.pos.size() * sizeof(Vector3) + s.idx.size() * sizeof(int32_t));
	return n;
}

void FarCastsBuilt::bake() {
	const b40::Shell none;
	for (size_t i = 0; i < SUPER_CHUNKS; i++) {
		if (shells[i].triangles() == 0) continue;
		Vector3 lo, hi;
		shells[i].bounds(lo, hi);
		boxes[i] = AABB(lo, hi - lo);
		meshes[i] = cast_hull(shells[i], none, error_m, material, &triangles);
		shells[i] = b40::Shell(); // moved from a new shell: its arrays' memory goes now
	}
}

void FarCasts::commit(FarCastsBuilt &b, const RID &scenario, RenderLayer layer) {
	triangles = b.triangles;
	for (size_t i = 0; i < SUPER_CHUNKS; i++) {
		OwnedMesh next = std::move(b.meshes[i]);
		twins[i].bind(scenario, next.rid, RID(), b.boxes[i], layer); // made in the shadow material: no override
		meshes[i] = std::move(next); // the old one goes after the instance left it
	}
}

void FarCasts::show(int detailed) const {
	for (size_t i = 0; i < SUPER_CHUNKS; i++) twins[i].visible(((detailed >> i) & 1) == 0);
}

void FarCasts::free() {
	for (ShadowTwin &t : twins) t.free();
	for (OwnedMesh &m : meshes) m.reset();
	triangles = 0;
}

} // namespace city
