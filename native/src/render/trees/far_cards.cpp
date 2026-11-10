// Bakes a superchunk's far cards into one mesh (see far_cards.h).
#include "render/trees/far_cards.h"

#include "render/surface_data.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr int CORNERS = 4, CARD_INDICES = 6, CUSTOM_HALFS = 4;
constexpr float FRAME_SIDES = 2.f; // a frame reaches its half side either way
constexpr float TINT_SHARE = 0.5f; // a vertex colour stops at 1 and the tint goes either side of it (vegetation.gdshaderinc FAR_TINT_SPAN)
constexpr int CARD_TRIS[CARD_INDICES] = { 0, 1, 2, 0, 2, 3 };
} // namespace

int64_t far_cards_bytes(size_t cards) {
	const int64_t n = int64_t(cards);
	return n * CORNERS * int64_t(sizeof(Vector3) + sizeof(Color) + CUSTOM_HALFS * sizeof(uint16_t)) + n * CARD_INDICES * int64_t(sizeof(int32_t));
}

OwnedMesh bake_far_cards(const std::vector<FarCard> &cards, float centre, float half_tall, const RID &material) {
	if (cards.empty()) return OwnedMesh();
	const int64_t n = int64_t(cards.size());
	PackedVector3Array pos;
	PackedColorArray col;
	PackedByteArray custom;
	PackedInt32Array idx;
	pos.resize(n * CORNERS);
	col.resize(n * CORNERS);
	custom.resize(n * CORNERS * CUSTOM_HALFS * int64_t(sizeof(uint16_t)));
	idx.resize(n * CARD_INDICES);
	Vector3 *p = pos.ptrw();
	Color *c = col.ptrw();
	uint8_t *u = custom.ptrw();
	int32_t *ix = idx.ptrw();
	for (int64_t k = 0; k < n; k++) {
		const FarCard &f = cards[size_t(k)];
		Color tone = f.tone * TINT_SHARE;
		tone.a = centre;
		const uint16_t half[CUSTOM_HALFS] = { Math::make_half_float(f.width * half_tall * FRAME_SIDES), Math::make_half_float(f.height), Math::make_half_float(float(f.species)),
			Math::make_half_float(half_tall) };
		for (int v = 0; v < CORNERS; v++) {
			int64_t at = k * CORNERS + v;
			p[at] = f.p;
			c[at] = tone;
			std::memcpy(u + at * CUSTOM_HALFS * int64_t(sizeof(uint16_t)), half, sizeof(half));
		}
		for (int i = 0; i < CARD_INDICES; i++) ix[k * CARD_INDICES + i] = int32_t(k * CORNERS + CARD_TRIS[i]);
	}
	Array arrays;
	arrays.resize(RenderingServer::ARRAY_MAX);
	arrays[RenderingServer::ARRAY_VERTEX] = pos;
	arrays[RenderingServer::ARRAY_COLOR] = col;
	arrays[RenderingServer::ARRAY_CUSTOM0] = custom;
	arrays[RenderingServer::ARRAY_INDEX] = idx;
	const uint64_t format = uint64_t(RenderingServer::ARRAY_CUSTOM_RGBA_HALF) << RenderingServer::ARRAY_FORMAT_CUSTOM0_SHIFT;
	return OwnedMesh::of(mesh_of_arrays(arrays, material, format));
}

} // namespace city
