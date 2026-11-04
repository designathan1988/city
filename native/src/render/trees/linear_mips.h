// Mipmaps of an sRGB picture on a transparent ground, averaged in linear light. Image::generate_mipmaps averages the stored
// numbers: where half a texel's area is bare, sRGB 0, that halves the number and leaves a quarter of the light, so leaves and
// fronds went dark with distance.
#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace city {

// An RGBA8 image of `w` x `h` from `base`, with its whole mip chain.
godot::Ref<godot::Image> image_with_linear_mips(int w, int h, const godot::PackedByteArray &base);

} // namespace city
