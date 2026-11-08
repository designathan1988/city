// Builds the film grade's lookup table (see film_grade.h).
#include "render/film_grade.h"

#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <algorithm>
#include <cstdint>

using namespace godot;

namespace city {

namespace {
constexpr int CHANNELS = 3, TEXEL_FLOATS = 4;
constexpr float BYTE_TOP = 255.f; // a byte's largest value
constexpr float ROUND_UP = 0.5f;
constexpr float REC709_R = 0.2126f, REC709_G = 0.7152f, REC709_B = 0.0722f; // luma weights, ITU-R BT.709

float smooth01(float x) {
	x = std::clamp(x, 0.f, 1.f);
	return x * x * (3.f - 2.f * x);
}

struct Grade {
	float contrast, vibrance, tone_from, tone_to;
	float warm[CHANNELS];

	void apply(float c[CHANNELS]) const {
		float luma = REC709_R * c[0] + REC709_G * c[1] + REC709_B * c[2];
		for (int i = 0; i < CHANNELS; i++) c[i] += (smooth01(c[i]) - c[i]) * contrast;
		float most = std::max({ c[0], c[1], c[2] }), least = std::min({ c[0], c[1], c[2] });
		float rich = 1.f + vibrance * (1.f - (most - least));
		float tone = smooth01((luma - tone_from) / (tone_to - tone_from));
		for (int i = 0; i < CHANNELS; i++) c[i] = std::clamp(luma + (c[i] - luma) * rich + warm[i] * tone, 0.f, 1.f);
	}
};

// The grade the script hands in (data/render's film block), through the strict reader (render/data_reader.h): a missing or
// mistyped key is pushed as an error and the grade refused, never read as 0.
bool read_grade(const Dictionary &d, Grade &g, int *side) {
	PackedStringArray errors;
	DataReader r{ errors, "film grade" };
	const Array warm = r.list(d, "warm_levels", CHANNELS); // in levels of the 8-bit picture
	g = Grade{ float(r.num(d, "contrast")), float(r.num(d, "vibrance")), float(r.num(d, "tone_from")), float(r.num(d, "tone_to")),
		{ float(r.in("warm_levels").num_at(warm, 0)) / BYTE_TOP, float(r.in("warm_levels").num_at(warm, 1)) / BYTE_TOP,
				float(r.in("warm_levels").num_at(warm, 2)) / BYTE_TOP } };
	if (side) *side = r.integer(d, "lut_side");
	for (const String &e : errors) UtilityFunctions::push_error(e);
	return errors.is_empty();
}
} // namespace

TypedArray<Image> film_grade_slices(const Dictionary &d) {
	Grade g;
	int side = 0;
	if (!read_grade(d, g, &side)) return TypedArray<Image>();
	TypedArray<Image> slices;
	PackedByteArray bytes;
	bytes.resize(int64_t(side) * side * TEXEL_FLOATS * int64_t(sizeof(float)));
	for (int b = 0; b < side; b++) {
		float *out = reinterpret_cast<float *>(bytes.ptrw());
		for (int y = 0; y < side; y++) {
			for (int x = 0; x < side; x++) {
				float c[CHANNELS] = { (x + ROUND_UP) / side, (y + ROUND_UP) / side, (b + ROUND_UP) / side };
				g.apply(c);
				for (int i = 0; i < CHANNELS; i++) *out++ = c[i];
				*out++ = 1.f;
			}
		}
		// In half floats: at a byte a channel the table's own rounding moved what it graded by a step of the picture, and steps show in a sky's gradient.
		Ref<Image> slice = Image::create_from_data(side, side, false, Image::FORMAT_RGBAF, bytes);
		slice->convert(Image::FORMAT_RGBAH);
		slices.push_back(slice);
	}
	return slices;
}

Color film_graded(const Dictionary &d, const Color &colour) {
	float c[CHANNELS] = { colour.r, colour.g, colour.b };
	Grade g;
	if (!read_grade(d, g, nullptr)) return colour;
	g.apply(c);
	return Color(c[0], c[1], c[2], colour.a);
}

} // namespace city
