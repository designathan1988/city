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
	layers_.resize(asset.variations.size());
	for (size_t v = 0; v < layers_.size(); v++) {
		layers_[v].init_owned(scenario_, asset.variations[v].lods.front(), material_->get_rid(), MOST_BODIES, true); // the bones place the skin
		layers_[v].count(0);
	}
}

void BodyRenderer::reset() {
	for (const Body &b : bodies_) free_slots_.push_back(b.slot);
	bodies_.clear();
	gore_.reset();
	layers_dirty_ = !layers_.empty();
}

void BodyRenderer::release() {
	reset();
	gore_.release();
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
		gore_.forget(bodies_.front().slot);
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
	std::vector<float> &rows = clip_scratch_;
	rows.resize(size_t(bones_) * FLOATS_A_BONE);
	asset_->clip_rows(spec.variation, idle_clip_, 0, rows.data());
	b.base.resize(size_t(bones_));
	for (int i = 0; i < bones_; i++) b.base[size_t(i)] = stands * rows_to_transform(rows.data() + size_t(i) * FLOATS_A_BONE);
	Vector3 joints[RAGDOLL_JOINTS];
	const float *rest = asset_->variations[size_t(spec.variation)].joints;
	for (int j = 0; j < RAGDOLL_JOINTS; j++) joints[j] = b.base[size_t(asset_->joint_bone[size_t(j)])].xform(Vector3(rest[j * 3], rest[j * 3 + 1], rest[j * 3 + 2]));
	b.doll.begin(joints, spec.velocity, spec.tip, spec.scatter, rng_);
	if (spec.piece != 0) {
		b.doll.cut_to(spec.piece);
		b.lost = RAGDOLL_ALL_PARTS & ~spec.piece;
	}
	b.doll.set_walls(spec.walls);
	for (const BodySpec::StumpBone &s : spec.bones) gore_.bone(b.slot, s.stump, s.length);
	if (spec.guts > 0) gore_.spill(b.slot, Ragdoll::BELLY, b.doll.belly(), spec.guts_kick);
	if (spec.guts > 1) gore_.spill(b.slot, Ragdoll::PELVIS, b.doll.pelvis(), spec.second_kick);
	b.stands = stands;
	if (spec.rises) b.rise_s = std::min(GET_UP_S, spec.life_s * RISE_SHARE_MOST);
	b.doll.frames(b.stood);
	for (Transform3D &t : b.stood) t = t.affine_inverse();
	pose(b);
	layers_dirty_ = true;
}

// The rows of its bones, and its box: a vertex is its bones' bind -> world rows applied to it, weighted to one (people_look.gdshaderinc
// BODY), so it lies within the boxes of its bones, each the bone's place out by the reach along the rows of its basis.
void BodyRenderer::write_bones(Body &b, const Transform3D *bones) {
	float *out = reinterpret_cast<float *>(rows_.ptrw()) + size_t(b.slot) * size_t(bones_) * FLOATS_A_BONE;
	const float reach = asset_->variations[size_t(b.variation)].reach;
	for (int i = 0; i < bones_; i++, out += FLOATS_A_BONE) {
		const Transform3D &t = bones[i];
		const Vector3 half = Vector3(t.basis.rows[0].length(), t.basis.rows[1].length(), t.basis.rows[2].length()) * reach;
		const AABB bone(t.origin - half, half * 2.f);
		b.box = i == 0 ? bone : b.box.merge(bone);
		for (int r = 0; r < TEXELS_A_BONE; r++) {
			out[r * FLOATS_A_TEXEL] = t.basis.rows[r].x;
			out[r * FLOATS_A_TEXEL + 1] = t.basis.rows[r].y;
			out[r * FLOATS_A_TEXEL + 2] = t.basis.rows[r].z;
			out[r * FLOATS_A_TEXEL + 3] = t.origin[r];
		}
	}
	rows_dirty_ = true;
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
	std::vector<float> &rows = clip_scratch_;
	rows.resize(size_t(bones_) * FLOATS_A_BONE);
	const bool up = t >= RISE_TO_CROUCH;
	const PersonClip &clip = asset_->clips[size_t(up ? up_clip_ : crouch_clip_)];
	asset_->clip_rows(b.variation, up ? up_clip_ : crouch_clip_, up ? std::min(int(played * float(clip.frames)), clip.frames - 1) : 0, rows.data());
	std::vector<Transform3D> &bones = bone_scratch_;
	bones.resize(size_t(bones_));
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

void BodyRenderer::lay_out() {
	std::vector<int> shown(layers_.size(), 0);
	for (const Body &b : bodies_) {
		MmLayer &l = layers_[size_t(b.variation)];
		l.put(shown[size_t(b.variation)]++, AXIS_X, AXIS_Y, AXIS_Z, Vector3(), float(b.variation), float(b.slot), 0.f, b.charred ? CHARRED_MARK : 0.f);
	}
	bound_layers();
	for (size_t v = 0; v < layers_.size(); v++) {
		layers_[v].count(shown[v]);
		layers_[v].shown(shown[v] > 0);
		if (shown[v] > 0) layers_[v].upload();
	}
	layers_dirty_ = false;
}

void BodyRenderer::bound_layers() {
	for (MmLayer &l : layers_) l.clear_bounds();
	for (const Body &b : bodies_) layers_[size_t(b.variation)].grow(b.box);
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
		free_slots_.push_back(b.slot);
		gore_.forget(b.slot);
		return true;
	});
	if (gone != bodies_.end()) {
		bodies_.erase(gone, bodies_.end());
		layers_dirty_ = true;
	}
	if (layers_dirty_) lay_out();
	gore_.update(dt, [this](int slot, int particle, Vector3 &out) {
		for (const Body &b : bodies_)
			if (b.slot == slot) {
				out = b.doll.point(particle);
				return true;
			}
		return false;
	}, ground, rows_dirty_);
	if (!rows_dirty_) return;
	bound_layers(); // the bones moved: the boxes follow
	for (MmLayer &l : layers_) l.send_bounds();
	RenderingServer::get_singleton()->texture_2d_update(texture_, Image::create_from_data(bones_ * TEXELS_A_BONE, MOST_BODIES, false, Image::FORMAT_RGBAF, rows_), 0);
	rows_dirty_ = false;
}

PackedVector3Array BodyRenderer::places() const {
	PackedVector3Array out;
	for (const Body &b : bodies_) out.push_back(b.doll.pelvis());
	return out;
}

int BodyRenderer::moving() const {
	return int(std::count_if(bodies_.begin(), bodies_.end(), [](const Body &b) { return !b.doll.asleep(); }));
}

} // namespace city
