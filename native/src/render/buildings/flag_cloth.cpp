// The flags' cloth and instances (flag_cloth.h). The cloth has the Roadcraft's 24 x 12 cells (flagLayer.ts PlaneGeometry), so
// the wave the shader lays on it bends the same; UV x runs from the hoist, UV y from the bottom; UV2.x tags it as a cloth
// for building_parts.gdshader, which draws the unit boxes of the facade parts with the same material. Each colour of a design
// travels as one float holding 0xRRGGBB (exact: under 2^24), the pattern as the fourth.
#include "render/buildings/flag_cloth.h"

#include "render/mesh_builder.h"

using namespace godot;

namespace city::b40 {

namespace {
constexpr int CLOTH_COLS = 24, CLOTH_ROWS = 12; // the cloth's grid, fine enough for its wave
constexpr float CLOTH_TAG = 1.f; // UV2.x of every cloth vertex (a box's is 0)

void grid(MeshBuilder &m, const Vector3 &normal) {
	const Color white(1, 1, 1, 1);
	const Vector2 tag(CLOTH_TAG, 0);
	const int first = int(m.v.size());
	for (int j = 0; j <= CLOTH_ROWS; j++)
		for (int i = 0; i <= CLOTH_COLS; i++) {
			const float u = float(i) / float(CLOTH_COLS), v = float(j) / float(CLOTH_ROWS);
			m.add(Vector3(u, v - 1.f, 0), normal, white, Vector2(u, v), tag);
		}
	for (int j = 0; j < CLOTH_ROWS; j++)
		for (int i = 0; i < CLOTH_COLS; i++) {
			const int a = first + j * (CLOTH_COLS + 1) + i, b = a + 1, c = a + CLOTH_COLS + 1, d = c + 1;
			m.tri(a, b, d, normal);
			m.tri(a, d, c, normal);
		}
}
} // namespace

MeshBuilder cloth_mesh() {
	MeshBuilder m;
	grid(m, Vector3(0, 0, 1));
	grid(m, Vector3(0, 0, -1));
	return m;
}

PackedFloat32Array bake_flags(const std::vector<FlagInstance> &flags) {
	PackedFloat32Array buf;
	for (const FlagInstance &f : flags)
		mm_push_plain(buf, Vector3(f.width, 0, 0), Vector3(0, f.height, 0), Vector3(0, 0, 1), f.top, float(f.design.rgb[0]),
				float(f.design.rgb[1]), float(f.design.rgb[2]), float(f.design.pattern));
	return buf;
}

} // namespace city::b40
