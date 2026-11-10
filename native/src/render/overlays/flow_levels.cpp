// The congestion levels' texture: fitted to the segments, refreshed from the published speeds (see flow_levels.h).
#include "render/overlays/flow_levels.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr int LEVELS_WIDE = 256; // texels a row of the level texture: a segment's texel is (id % wide, id / wide)
constexpr uint8_t LEVEL_TOP = 255; // a level's texel at free flow (an 8-bit share of the ramp)
constexpr float RANGE_MIN = 1e-3f; // free_share == jam_share still divides
constexpr float HALF = 0.5f;

PackedByteArray bytes_of(const std::vector<uint8_t> &level) {
	PackedByteArray bytes;
	bytes.resize(int64_t(level.size()));
	std::memcpy(bytes.ptrw(), level.data(), level.size());
	return bytes;
}
} // namespace

void FlowLevels::init(const RID &material, float free_share, float jam_share) {
	material_ = material;
	free_share_ = free_share;
	jam_share_ = jam_share;
	level_.clear();
	rows_ = 0;
	texture_.unref();
	fit(1);
}

void FlowLevels::reset() {
	segments_ = 0;
	if (std::all_of(level_.begin(), level_.end(), [](uint8_t l) { return l == LEVEL_TOP; })) return;
	std::fill(level_.begin(), level_.end(), LEVEL_TOP);
	send();
}

uint8_t FlowLevels::level_of(float share) const {
	const float k = std::clamp((share - jam_share_) / std::max(RANGE_MIN, free_share_ - jam_share_), 0.f, 1.f);
	return uint8_t(std::lround(k * float(LEVEL_TOP)));
}

// Made again, its rows doubled, when it does not hold `slots`: the levels it had are kept and the new texels are free.
void FlowLevels::fit(int slots) {
	int rows = std::max(1, rows_);
	while (rows * LEVELS_WIDE < slots) rows *= 2;
	if (rows == rows_ && texture_.is_valid()) return;
	rows_ = rows;
	level_.resize(size_t(rows) * LEVELS_WIDE, LEVEL_TOP);
	texture_.unref();
	send();
}

// The levels sent whole: into the texture it has (ImageTexture::update: same size and format, nothing allocated), or a new one
// the first time and when it grew, which the material is told.
void FlowLevels::send() {
	const Ref<Image> image = Image::create_from_data(LEVELS_WIDE, rows_, false, Image::FORMAT_R8, bytes_of(level_));
	if (texture_.is_valid()) {
		texture_->update(image);
	} else {
		texture_ = ImageTexture::create_from_image(image);
		RenderingServer *rs = RenderingServer::get_singleton();
		rs->material_set_param(material_, "levels", texture_->get_rid());
		rs->material_set_param(material_, "levels_wide", LEVELS_WIDE);
	}
	sends_++;
}

int64_t FlowLevels::refresh(const PoseFrame &f) {
	refreshes_++;
	segments_ = int(f.seg_speed_share.size());
	fit(segments_);
	bool moved = false;
	for (size_t i = 0; i < f.seg_speed_share.size(); i++) {
		const uint8_t l = level_of(f.seg_speed_share[i]);
		if (level_[i] == l) continue;
		level_[i] = l;
		moved = true;
	}
	if (!moved) return 0;
	send();
	return int64_t(level_.size());
}

void FlowLevels::probe(Dictionary &d) const {
	d["refreshes"] = refreshes_;
	d["level_sends"] = sends_;
	d["segments"] = segments_;
	d["levels"] = bytes_of(level_);
	d["texture"] = texture_;
	PackedByteArray ramp;
	for (float share : { 0.f, jam_share_, (jam_share_ + free_share_) * HALF, free_share_, 1.f }) ramp.push_back(level_of(share));
	d["ramp"] = ramp;
}

} // namespace city
