// The fallen, drawn (body_renderer.h).
#include "render/agents/body_renderer.h"
#include "render/detail_hold.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cstring>
#include <cmath>
#include <limits>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr double BOUNDS_ROUNDING_ULPS = 32, WEIGHT_ROUNDING_ULPS = 8;
constexpr int TEXELS_A_BONE = 3, FLOATS_A_TEXEL = 4, FLOATS_A_BONE = TEXELS_A_BONE * FLOATS_A_TEXEL;
constexpr float STEP_S = 1.f / 60.f;
constexpr int STEPS_MOST = 4;
constexpr float CHARRED_MARK = 1.f;
constexpr float GET_UP_S = 1.7f, RISE_SHARE_MOST = 0.7f, RISE_TO_CROUCH = 0.45f; // how long getting up takes, at most of the time down; and the blend's part of it
const Vector3 AXIS_X(1, 0, 0), AXIS_Y(0, 1, 0), AXIS_Z(0, 0, 1);
constexpr int PART_JOINT[RAGDOLL_PARTS] = { 0, 2, 3, 4, 5, 7, 8, 10, 11, 12, 14, 15, 16 }; // the joint each part starts at: ragdoll.cpp PART_FRAMES

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
	idle_clip_ = asset.clip("idle");
	crouch_clip_ = asset.clip("crouchIdle");
	up_clip_ = asset.clip("crouchUp");
	return true;
}

// Made for the first body of a city: a new city frees what the agents draw with (RenderWorld::reset).
void BodyRenderer::make_ready() {
	if (texture_.is_valid()) return;
	const PeopleAsset &asset = *asset_;
	free_slots_.clear();
	for (int i = MOST_BODIES - 1; i >= 0; i--) free_slots_.push_back(i);
	rows_.resize(int64_t(MOST_BODIES) * bones_ * FLOATS_A_BONE * int64_t(sizeof(float)));
	rows_.fill(0);
	RenderingServer *rs = RenderingServer::get_singleton();
	const int64_t row_bytes = int64_t(bones_) * FLOATS_A_BONE * int64_t(sizeof(float));
	PackedByteArray blank; blank.resize(row_bytes); blank.fill(0);
	TypedArray<Ref<Image>> pages;
	for (int slot = 0; slot < MOST_BODIES; slot++) pages.push_back(Image::create_from_data(bones_ * TEXELS_A_BONE, 1, false, Image::FORMAT_RGBAF, blank));
	texture_ = rs->texture_2d_layered_create(pages, RenderingServer::TEXTURE_LAYERED_2D_ARRAY);
	palette_rows_.resize(size_t(bones_) * FLOATS_A_BONE);
	rise_bones_.resize(size_t(bones_));
	rs->material_set_param(material_->get_rid(), "bodies", texture_);
	rs->material_set_param(material_->get_rid(), "atlas", asset.atlas->get_rid());
	rs->material_set_param(material_->get_rid(), "alpha_test", asset.variations.front().alpha_test);
}

void BodyRenderer::reset() {
	for (const Body &b : bodies_) { drop_draw(b.slot); free_slots_.push_back(b.slot); }
	bodies_.clear();
	rows_dirty_.reset();
}

void BodyRenderer::release() {
	reset();
	for (int slot = 0; slot < MOST_BODIES; slot++) drop_draw(slot);
	palette_rows_.clear(); rise_bones_.clear(); rows_ = PackedByteArray();
	if (texture_.is_valid()) RenderingServer::get_singleton()->free_rid(texture_);
	texture_ = RID();
}

void BodyRenderer::add(const BodySpec &spec) {
	if (!asset_ || spec.variation < 0 || spec.variation >= int(asset_->variations.size())) return;
	make_ready();
	if (free_slots_.empty()) {
		drop_draw(bodies_.front().slot);
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
	auto &rows = palette_rows_;
	asset_->clip_rows(spec.variation, idle_clip_, 0, rows.data());
	b.base.resize(size_t(bones_));
	for (int i = 0; i < bones_; i++) b.base[size_t(i)] = stands * rows_to_transform(rows.data() + size_t(i) * FLOATS_A_BONE);
	Vector3 joints[RAGDOLL_JOINTS];
	const float *rest = asset_->variations[size_t(spec.variation)].joints;
	for (int j = 0; j < RAGDOLL_JOINTS; j++) joints[j] = b.base[size_t(asset_->joint_bone[size_t(j)])].xform(Vector3(rest[j * 3], rest[j * 3 + 1], rest[j * 3 + 2]));
	b.doll.begin(joints, spec.velocity, spec.tip, spec.scatter, rng_);
	b.doll.set_walls(spec.walls);
	b.stands = stands;
	if (spec.rises) b.rise_s = std::min(GET_UP_S, spec.life_s * RISE_SHARE_MOST);
	b.doll.frames(b.stood);
	for (Transform3D &t : b.stood) t = t.affine_inverse();
	pose(b);
}

void BodyRenderer::write_bones(Body &b, const Transform3D *bones) {
	float *out = reinterpret_cast<float *>(rows_.ptrw()) + size_t(b.slot) * size_t(bones_) * FLOATS_A_BONE;
	for (int i = 0; i < bones_; i++, out += FLOATS_A_BONE) {
		const Transform3D &t = bones[i];
		for (int r = 0; r < TEXELS_A_BONE; r++) {
			out[r * FLOATS_A_TEXEL] = t.basis.rows[r].x;
			out[r * FLOATS_A_TEXEL + 1] = t.basis.rows[r].y;
			out[r * FLOATS_A_TEXEL + 2] = t.basis.rows[r].z;
			out[r * FLOATS_A_TEXEL + 3] = t.origin[r];
		}
	}
	const PersonVariation &variation = asset_->variations[size_t(b.variation)];
	Vector3 low, high;
	double arithmetic_scale[3] = {};
	bool any = false, finite = variation.bone_bounds.size() == size_t(bones_);
	for (int i = 0; finite && i < bones_; i++) {
		const PersonBoneBounds &box = variation.bone_bounds[size_t(i)];
		if (!box.used) continue;
		const Vector3 center = (box.low + box.high) * 0.5f, extent = (box.high - box.low) * 0.5f;
		const Transform3D &t = bones[i];
		const Vector3 at = t.xform(center);
		Vector3 span;
		for (int axis = 0; axis < 3; axis++) {
			const Vector3 &r = t.basis.rows[axis];
			span[axis] = std::abs(r.x) * extent.x + std::abs(r.y) * extent.y + std::abs(r.z) * extent.z;
			const double magnitude = std::abs(double(r.x)) * std::max(std::abs(double(box.low.x)), std::abs(double(box.high.x))) +
				std::abs(double(r.y)) * std::max(std::abs(double(box.low.y)), std::abs(double(box.high.y))) +
				std::abs(double(r.z)) * std::max(std::abs(double(box.low.z)), std::abs(double(box.high.z))) + std::abs(double(t.origin[axis]));
			arithmetic_scale[axis] = std::max(arithmetic_scale[axis], magnitude);
			finite = finite && std::isfinite(at[axis]) && std::isfinite(span[axis]) && std::isfinite(magnitude);
		}
		const Vector3 a = at - span, c = at + span;
		if (!any) { low = a; high = c; any = true; }
		else { low = low.min(a); high = high.max(c); }
	}
	b.bounds_prepared = finite && any;
	if (b.bounds_prepared) {
		// Nonnegative skin weights form a convex combination scaled by their sum. Include GPU UNORM16 sum error,
		// including large world translations: simply treating the packed weights as exactly unit sum is unsafe.
		const double weight_error = WEIGHT_ROUNDING_ULPS * std::numeric_limits<float>::epsilon();
		const double sum_low = std::max(0.0, variation.weight_sum_low - weight_error), sum_high = variation.weight_sum_high + weight_error;
		for (int axis = 0; axis < 3; axis++) {
			const double a = std::min(double(low[axis]) * sum_low, double(low[axis]) * sum_high);
			const double c = std::max(double(high[axis]) * sum_low, double(high[axis]) * sum_high);
			const double error = BOUNDS_ROUNDING_ULPS * std::numeric_limits<float>::epsilon() * (1 + std::max({std::abs(a), std::abs(c), arithmetic_scale[axis] * std::max(1.0, sum_high)}));
			low[axis] = float(a - error); high[axis] = float(c + error);
		}
		b.bounds = AABB(low, high - low);
	} else {
		// Standalone assets without prepared bounds (or invalid physics) remain conservatively drawable.
		b.bounds = AABB(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M));
	}
	rows_dirty_.set(size_t(b.slot));
}

// Every bone follows its part from how the body stood; a part gone closes on its joint.
void BodyRenderer::pose(Body &b) {
	Transform3D now[RAGDOLL_PARTS], moved[RAGDOLL_PARTS];
	b.doll.frames(now);
	for (int p = 0; p < RAGDOLL_PARTS; p++) moved[p] = now[p] * b.stood[p];
	b.lying.resize(size_t(bones_));
	for (int i = 0; i < bones_; i++) {
		const int part = asset_->bone_part[size_t(i)];
		const bool gone = (b.lost >> part & 1u) != 0;
		b.lying[size_t(i)] = gone ? Transform3D(Basis(Vector3(), Vector3(), Vector3()), now[part].origin) : moved[part] * b.base[size_t(i)];
	}
	write_bones(b, b.lying.data());
}

// Getting up (the 4.0's getUp: to a crouch, then the crouch's own rise), from however it lies, to where it stood: the first part of the time blends every bone to the crouch,
// the rest plays the clip, and at its last frame the walker stands there.
void BodyRenderer::rise(Body &b) {
	const float t = 1.f - b.left / b.rise_s, reach = std::min(t / RISE_TO_CROUCH, 1.f);
	const float blend = reach * reach * (3.f - 2.f * reach), played = std::max(t - RISE_TO_CROUCH, 0.f) / (1.f - RISE_TO_CROUCH);
	auto &rows = palette_rows_;
	const bool up = t >= RISE_TO_CROUCH;
	const PersonClip &clip = asset_->clips[size_t(up ? up_clip_ : crouch_clip_)];
	asset_->clip_rows(b.variation, up ? up_clip_ : crouch_clip_, up ? std::min(int(played * float(clip.frames)), clip.frames - 1) : 0, rows.data());
	auto &bones = rise_bones_;
	for (int i = 0; i < bones_; i++) {
		const int part = asset_->bone_part[size_t(i)];
		if ((b.lost >> part & 1u) != 0) { // closed on its joint, where that joint stands in this pose
			const int joint = PART_JOINT[part], bone = asset_->joint_bone[size_t(joint)];
			const float *rest = asset_->variations[size_t(b.variation)].joints + joint * 3;
			const Vector3 at = (b.stands * rows_to_transform(rows.data() + size_t(bone) * FLOATS_A_BONE)).xform(Vector3(rest[0], rest[1], rest[2]));
			bones[size_t(i)] = Transform3D(Basis(Vector3(), Vector3(), Vector3()), at);
			continue;
		}
		const Transform3D aim = b.stands * rows_to_transform(rows.data() + size_t(i) * FLOATS_A_BONE);
		bones[size_t(i)] = up ? aim : b.lying[size_t(i)].interpolate_with(aim, blend);
	}
	write_bones(b, bones.data());
}

void BodyRenderer::drop_draw(int slot) {
	Draw &draw = draws_[size_t(slot)];
	if (draw.ready) draw.layer.release();
	draw.ready = false; draw.level = -1;
	rows_dirty_.reset(size_t(slot));
}

void BodyRenderer::present(const Vector3 &camera, float hold, const std::function<int(float)> &level_at) {
	RenderingServer *rs = RenderingServer::get_singleton();
	for (const Body &body : bodies_) {
		Draw &draw = draws_[size_t(body.slot)];
		const float distance = (body.bounds.get_center() - camera).length();
		const int level = body.bounds_prepared ? held_detail_level(distance, draw.level, hold, level_at) : 0;
		const Ref<ArrayMesh> &mesh = asset_->variations[size_t(body.variation)].lods[size_t(level)];
		if (!draw.ready) {
			draw.layer.init(scenario_, mesh, material_->get_rid(), 1, true, body.bounds);
			draw.layer.put(0, AXIS_X, AXIS_Y, AXIS_Z, Vector3(), float(body.variation), float(body.slot), 0.f, body.charred ? CHARRED_MARK : 0.f);
			draw.layer.count(1); draw.layer.upload(); draw.ready = true; draw.bounds = body.bounds;
		} else {
			if (draw.level != level) {
				rs->multimesh_set_mesh(draw.layer.mm, mesh->get_rid());
				draw.layer.mesh = mesh;
			}
			if (draw.bounds != body.bounds) {
				rs->multimesh_set_custom_aabb(draw.layer.mm, body.bounds);
				rs->instance_set_custom_aabb(draw.layer.inst, body.bounds);
				draw.bounds = body.bounds;
			}
		}
		draw.level = level;
	}
}

void BodyRenderer::update(float dt, const Ragdoll::Ground &ground) {
	clock_ = std::min(clock_ + dt, STEP_S * float(STEPS_MOST));
	const int steps = int(clock_ / STEP_S);
	clock_ -= float(steps) * STEP_S;
	for (Body &b : bodies_) {
		b.left -= dt;
		if (b.left <= b.rise_s && b.rise_s > 0.f) {
			rise(b);
			continue;
		}
		if (b.doll.asleep() || steps == 0) continue;
		const float floor = b.floor;
		for (int k = 0; k < steps; k++) b.doll.step(STEP_S, [&](float x, float z) { return std::max(ground(x, z), floor); });
		pose(b);
	}
	const auto gone = std::remove_if(bodies_.begin(), bodies_.end(), [this](const Body &b) {
		if (b.left > 0.f) return false;
		drop_draw(b.slot);
		free_slots_.push_back(b.slot);
		return true;
	});
	if (gone != bodies_.end()) {
		bodies_.erase(gone, bodies_.end());
	}
	if (rows_dirty_.none()) return;
	const int64_t row_bytes = int64_t(bones_) * FLOATS_A_BONE * int64_t(sizeof(float));
	RenderingServer *rs = RenderingServer::get_singleton();
	for (int slot = 0; slot < MOST_BODIES; slot++) if (rows_dirty_[size_t(slot)]) {
		PackedByteArray page; page.resize(row_bytes);
		std::memcpy(page.ptrw(), rows_.ptr() + int64_t(slot) * row_bytes, size_t(row_bytes));
		rs->texture_2d_update(texture_, Image::create_from_data(bones_ * TEXELS_A_BONE, 1, false, Image::FORMAT_RGBAF, page), slot);
	}
	rows_dirty_.reset();
}

int BodyRenderer::moving() const {
	return int(std::count_if(bodies_.begin(), bodies_.end(), [](const Body &b) { return !b.doll.asleep(); }));
}

} // namespace city
