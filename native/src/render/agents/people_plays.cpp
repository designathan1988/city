// The figures' plays texture: rows written as the frame draws, sent once at its end (people_plays.h).
#include "render/agents/people_plays.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr int FIRST_ROWS = 256; // the texture's first height: a street's figures near the eye
constexpr int TEXEL_FLOATS = 4;
constexpr size_t ROW_BYTES = size_t(PEOPLE_PLAYS) * TEXEL_FLOATS * sizeof(float);
} // namespace

int FigurePlays::put(const Play *plays, int n) {
	if (used_ == rows_) {
		rows_ = std::max(FIRST_ROWS, rows_ * 2);
		texels_.resize(int64_t(rows_) * int64_t(ROW_BYTES));
	}
	float row[PEOPLE_PLAYS * TEXEL_FLOATS] = {};
	for (int i = 0; i < std::min(n, int(PEOPLE_PLAYS)); i++) {
		row[i * TEXEL_FLOATS] = float(plays[i].clip);
		row[i * TEXEL_FLOATS + 1] = plays[i].phase;
		row[i * TEXEL_FLOATS + 2] = plays[i].weight;
	}
	std::memcpy(texels_.ptrw() + size_t(used_) * ROW_BYTES, row, ROW_BYTES);
	return used_++;
}

void FigurePlays::upload(const RID *materials, int count) {
	if (rows_ == 0) put(nullptr, 0); // an empty row: the materials always have a texture to read
	const Ref<Image> image = Image::create_from_data(PEOPLE_PLAYS, rows_, false, Image::FORMAT_RGBAF, texels_);
	if (texture_.is_valid() && texture_->get_height() == rows_) {
		texture_->update(image);
		return;
	}
	texture_ = ImageTexture::create_from_image(image);
	RenderingServer *rs = RenderingServer::get_singleton();
	for (int i = 0; i < count; i++) rs->material_set_param(materials[i], "plays", texture_->get_rid());
}

} // namespace city
