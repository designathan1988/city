// The film grade of the picture on screen as a lookup table: a slight S of contrast, vibrance and warm highlights
// (the 4.0 tints nothing below a luma of 0.15), all of them a function of one pixel's colour, so Environment.adjustment_color_correction draws them in
// the tonemap pass itself, at no extra pass. The numbers come from data/game/daylight.json "grade".
#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace city {

// The slices of the 3D table for ImageTexture3D.create. Godot samples it with the display colour as the coordinate and
// no half-texel remap (tonemap.glsl apply_color_correction): texel i holds the grade of colour (i + 0.5) / side.
godot::TypedArray<godot::Image> film_grade_slices(const godot::Dictionary &grade);

// One display colour through the grade, as the table gives it (tests read what the screen must show).
godot::Color film_graded(const godot::Dictionary &grade, const godot::Color &colour);

} // namespace city
