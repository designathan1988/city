// A vertex's four bytes (an RGBA8 word) as a custom attribute both of Godot 4.7.2's renderers read whole.
//
// Compatibility binds a custom format of four bytes as one component: drivers/gles3/storage/mesh_storage.cpp
// (_mesh_surface_generate_version_for_input_mask) sets the attribute's size to fmtsize / sizeof(float), so ARRAY_CUSTOM_RGBA8_UNORM
// reaches the shader as (r, 0, 0, 1) and RG_HALF and RGBA_HALF lose half their channels; upstream fixed it in 323598b42b ("Fix all
// data formats being assumed to be the same size"), after 4.7.2. The float formats are read whole by both, so a word goes as two
// floats, its low and its high 16 bits as whole numbers (exact: a float holds every integer to 2^24), ARRAY_CUSTOM_RG_FLOAT.
// render/shaders/bytes.gdshaderinc custom_bytes gives the four bytes back as an RGBA8 attribute reads them.
#pragma once

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include <cstddef>
#include <cstdint>

namespace city {

constexpr godot::RenderingServer::ArrayCustomFormat CUSTOM_WORD_FORMAT = godot::RenderingServer::ARRAY_CUSTOM_RG_FLOAT;
constexpr size_t CUSTOM_WORD_FLOATS = 2; // the floats of a word: its low half, its high half
constexpr uint32_t HALF_WORD_BITS = 16, HALF_WORD_MASK = 0xffff; // a 32-bit word's halves

// The word `w` into dst[0], dst[1].
inline void put_custom_word(float *dst, uint32_t w) {
	dst[0] = float(w & HALF_WORD_MASK);
	dst[1] = float(w >> HALF_WORD_BITS);
}

// `n` words as the array a custom attribute of CUSTOM_WORD_FORMAT takes.
inline godot::PackedFloat32Array custom_words(const uint32_t *w, size_t n) {
	godot::PackedFloat32Array out;
	out.resize(int64_t(n * CUSTOM_WORD_FLOATS));
	float *dst = out.ptrw();
	for (size_t i = 0; i < n; i++) put_custom_word(dst + i * CUSTOM_WORD_FLOATS, w[i]);
	return out;
}

} // namespace city
