// The fallen, drawn (body_renderer.h).
#include "render/agents/body_renderer.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr int MOST_BODIES = 64; // past it the oldest goes: a row of the texture each
constexpr int TEXELS_A_BONE = 3, FLOATS_A_TEXEL = 4, FLOATS_A_BONE = TEXELS_A_BONE * FLOATS_A_TEXEL;
constexpr float STEP_S = 1.f / 60.f;
constexpr int STEPS_MOST = 4;
constexpr float CHARRED_MARK = 1.f;
const Vector3 AXIS_X(1, 0, 0), AXIS_Y(0, 1, 0), AXIS_Z(0, 0, 1);

Transform3D rows_to_transform(const float *r) {
	constexpr int Y = FLOATS_A_TEXEL, Z = 2 * FLOATS_A_TEXEL, O = 3; // a row of four: the basis, then the origin
	return Transform3D(Basis(r[0], r[1], r[2], r[Y], r[Y + 1], r[Y + 2], r[Z], r[Z + 1], r[Z + 2]), Vector3(r[O], r[Y + O], r[Z + O]));
}
} // namespace

bool BodyRenderer::init(const RenderContext &ctx, const PeopleAsset &asset, PackedStringArray &errors) {
	asset_ = &asset;
	scenario_ = ctx.scenario;
	material_ = ctx.material_objects.get("people_body", Variant());
	if (material_.is_null()) {
		errors.push_back("material ausente: people_body");
		return false;
	}
	bones_ = int(asset.bone_part.size());
	return true;
}

// Made for the first body of a city: a new city frees what the agents draw with (RenderWorld::reset).
void BodyRenderer::make_ready() {
	if (!layers_.empty()) return;
	const PeopleAsset &asset = *asset_;
	free_slots_.clear();
	for (int i = MOST_BODIES - 1; i >= 0; i--) free_slots_.push_back(i);
	rows_.resize(int64_t(MOST_BODIES) * bones_ * FLOATS_A_BONE * int64_t(sizeof(float)));
	rows_.fill(0);
	RenderingServer *rs = RenderingServer::get_singleton();
	const Ref<Image> image = Image::create_from_data(bones_ * TEXELS_A_BONE, MOST_BODIES, false, Image::FORMAT_RGBAF, rows_);
	texture_ = rs->texture_2d_create(image);
	rs->material_set_param(material_->get_rid(), "bodies", texture_);
	rs->material_set_param(material_->get_rid(), "atlas", asset.atlas->get_rid());
	rs->material_set_param(material_->get_rid(), "alpha_test", asset.variations.front().alpha_test);
	const AABB world(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M));
	layers_.resize(asset.variations.size());
	for (size_t v = 0; v < layers_.size(); v++) {
		layers_[v].init(scenario_, asset.variations[v].lods.front(), material_->get_rid(), MOST_BODIES, true, world);
		layers_[v].count(0);
	}
}

void BodyRenderer::reset() {
	for (const Body &b : bodies_) free_slots_.push_back(b.slot);
	bodies_.clear();
	layers_dirty_ = !layers_.empty();
}

void BodyRenderer::release() {
	reset();
	for (MmLayer &l : layers_) l.release();
	layers_.clear();
	if (texture_.is_valid()) RenderingServer::get_singleton()->free_rid(texture_);
	texture_ = RID();
}

void BodyRenderer::add(const BodySpec &spec) {
	if (!asset_ || spec.variation >= int(asset_->variations.size())) return;
	make_ready();
	if (free_slots_.empty()) {
		free_slots_.push_back(bodies_.front().slot);
		bodies_.pop_front();
	}
	Body &b = bodies_.emplace_back();
	b.slot = free_slots_.back();
	free_slots_.pop_back();
	b.variation = spec.variation;
	b.left = spec.life_s;
	b.floor = spec.at.y;
	b.lost = spec.lost;
	b.charred = spec.charred;
	const Vector3 front(std::cos(spec.heading), 0, std::sin(spec.heading));
	Basis stands_b;
	stands_b.set_columns(AXIS_Y.cross(front), AXIS_Y, front);
	const Transform3D stands(stands_b, spec.at);
	std::vector<float> rows(size_t(bones_) * FLOATS_A_BONE);
	asset_->standing_rows(spec.variation, rows.data());
	b.base.resize(size_t(bones_));
	for (int i = 0; i < bones_; i++) b.base[size_t(i)] = stands * rows_to_transform(rows.data() + size_t(i) * FLOATS_A_BONE);
	Vector3 joints[RAGDOLL_JOINTS];
	const float *rest = asset_->variations[size_t(spec.variation)].joints;
	for (int j = 0; j < RAGDOLL_JOINTS; j++) joints[j] = b.base[size_t(asset_->joint_bone[size_t(j)])].xform(Vector3(rest[j * 3], rest[j * 3 + 1], rest[j * 3 + 2]));
	b.doll.begin(joints, spec.velocity, spec.tip, spec.scatter, rng_);
	b.doll.frames(b.stood);
	for (Transform3D &t : b.stood) t = t.affine_inverse();
	pose(b);
	layers_dirty_ = true;
}

// Every bone follows its part from how the body stood; a part gone closes on its joint.
void BodyRenderer::pose(Body &b) {
	Transform3D now[RAGDOLL_PARTS], moved[RAGDOLL_PARTS];
	b.doll.frames(now);
	for (int p = 0; p < RAGDOLL_PARTS; p++) moved[p] = now[p] * b.stood[p];
	float *out = reinterpret_cast<float *>(rows_.ptrw()) + size_t(b.slot) * size_t(bones_) * FLOATS_A_BONE;
	for (int i = 0; i < bones_; i++, out += FLOATS_A_BONE) {
		const int part = asset_->bone_part[size_t(i)];
		const bool gone = (b.lost >> part & 1u) != 0;
		const Transform3D t = moved[part] * b.base[size_t(i)];
		for (int r = 0; r < TEXELS_A_BONE; r++) {
			const Vector3 row = gone ? Vector3() : t.basis.rows[r];
			out[r * FLOATS_A_TEXEL] = row.x;
			out[r * FLOATS_A_TEXEL + 1] = row.y;
			out[r * FLOATS_A_TEXEL + 2] = row.z;
			out[r * FLOATS_A_TEXEL + 3] = gone ? now[part].origin[r] : t.origin[r];
		}
	}
	rows_dirty_ = true;
}

void BodyRenderer::lay_out() {
	std::vector<int> shown(layers_.size(), 0);
	for (const Body &b : bodies_) {
		MmLayer &l = layers_[size_t(b.variation)];
		l.put(shown[size_t(b.variation)]++, AXIS_X, AXIS_Y, AXIS_Z, Vector3(), float(b.variation), float(b.slot), 0.f, b.charred ? CHARRED_MARK : 0.f);
	}
	for (size_t v = 0; v < layers_.size(); v++) {
		layers_[v].count(shown[v]);
		layers_[v].shown(shown[v] > 0);
		if (shown[v] > 0) layers_[v].upload();
	}
	layers_dirty_ = false;
}

void BodyRenderer::update(float dt, const Ragdoll::Ground &ground) {
	clock_ = std::min(clock_ + dt, STEP_S * float(STEPS_MOST));
	const int steps = int(clock_ / STEP_S);
	clock_ -= float(steps) * STEP_S;
	for (Body &b : bodies_) {
		b.left -= dt;
		if (b.doll.asleep() || steps == 0) continue;
		const float floor = b.floor;
		for (int k = 0; k < steps; k++) b.doll.step(STEP_S, [&](float x, float z) { return std::max(ground(x, z), floor); });
		pose(b);
	}
	const auto gone = std::remove_if(bodies_.begin(), bodies_.end(), [this](const Body &b) {
		if (b.left > 0.f) return false;
		free_slots_.push_back(b.slot);
		return true;
	});
	if (gone != bodies_.end()) {
		bodies_.erase(gone, bodies_.end());
		layers_dirty_ = true;
	}
	if (layers_dirty_) lay_out();
	if (!rows_dirty_) return;
	RenderingServer::get_singleton()->texture_2d_update(texture_, Image::create_from_data(bones_ * TEXELS_A_BONE, MOST_BODIES, false, Image::FORMAT_RGBAF, rows_), 0);
	rows_dirty_ = false;
}

int BodyRenderer::moving() const {
	return int(std::count_if(bodies_.begin(), bodies_.end(), [](const Body &b) { return !b.doll.asleep(); }));
}

} // namespace city
