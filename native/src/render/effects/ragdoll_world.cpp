// The falling people (ragdoll_world.h): their rigs from the walkers' meshes, the throw, the step, the bones' texture and the instances.
#include "render/effects/ragdoll_world.h"

#include "render/agents/people_renderer.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <chrono>
#include <limits>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int TEXELS_PER_ROW = 2, ROWS_PER_BONE = 3, FIXED_BITS = 8, FIXED_LOW = 255, CHANNELS = 4;
constexpr float FIXED_TOP = 65535.f, FRAMES_BASE = 64.f, HAIR_BASE = 17.f, SKIN_BASE = 256.f, BOX_PAD_M = 4.f, HALF = 0.5f;
constexpr double MICRO = 1e6;
constexpr float NO_LIMIT = std::numeric_limits<float>::max();

class CityGround : public phys::Ground {
public:
	explicit CityGround(const CityState &c) : city_(c) {}
	float height(float x, float z) const override { return city_.terrain.height_at(x, z); }

private:
	const CityState &city_;
};

phys::V3 v3(const Vector3 &v) { return phys::V3(v.x, v.y, v.z); }

// One value as 16-bit fixed point over [-span, span]: the high byte in texel 2x, the low byte in texel 2x + 1 (people_skin.gdshaderinc).
void put_fixed(PackedByteArray &texels, int64_t high_at, float value, float span) {
	const float unit = std::clamp((value + span) / (2.f * span), 0.f, 1.f);
	const uint32_t q = uint32_t(std::lround(unit * FIXED_TOP));
	uint8_t *w = texels.ptrw();
	w[high_at] = uint8_t(q >> FIXED_BITS);
	w[high_at + CHANNELS] = uint8_t(q & FIXED_LOW);
}
} // namespace

bool RagdollWorld::init(const RenderContext &ctx, const PeopleAsset &people, const Dictionary &people_cfg, float gravity, PackedStringArray &errors) {
	reset();
	asset_ = &people;
	scenario_ = ctx.scenario;
	gravity_ = gravity;
	params_ = RagdollParams();
	if (!params_.load(ctx.table("ragdoll", errors), errors)) return false;
	params_.rig.bone_count = people.bones;
	params_.solver.gravity = gravity;
	material_ = ctx.material("ragdoll", errors);
	if (!errors.is_empty()) return false;
	set_people_palette(material_, people_cfg);
	width_ = people.bones * ROWS_PER_BONE * TEXELS_PER_ROW;
	image_ = Image::create(width_, params_.max_people, false, Image::FORMAT_RGBA8);
	bones_ = ImageTexture::create_from_image(image_);
	texels_.resize(int64_t(width_) * params_.max_people * CHANNELS);
	texels_.fill(0);
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "bones", bones_->get_rid());
	rs->material_set_param(material_, "span_m", params_.span_m);
	const size_t nv = people.variations.size();
	rigs_.resize(nv);
	rig_tried_.assign(nv, 0);
	draws_.assign(nv, MultiMeshDraw());
	bufs_.assign(nv, PackedFloat32Array());
	return true;
}

void RagdollWorld::reset() {
	people_.clear();
	for (PackedFloat32Array &b : bufs_) b.resize(0);
	cost_ = phys::StepCost();
}

void RagdollWorld::release() {
	reset();
	for (MultiMeshDraw &d : draws_) d.free();
}

phys::PersonRig *RagdollWorld::rig_for(int variation) {
	if (!asset_ || variation < 0 || size_t(variation) >= rigs_.size()) return nullptr;
	if (rig_tried_[size_t(variation)]) return rigs_[size_t(variation)].get();
	rig_tried_[size_t(variation)] = 1;
	const Ref<ArrayMesh> mesh = asset_->variations[size_t(variation)].lods[0];
	const Array arrays = mesh->surface_get_arrays(0);
	const PackedVector3Array pos = arrays[Mesh::ARRAY_VERTEX];
	const PackedInt32Array bones = arrays[Mesh::ARRAY_BONES];
	const PackedFloat32Array weights = arrays[Mesh::ARRAY_WEIGHTS];
	const int n = int(pos.size());
	if (n == 0 || bones.size() != int64_t(n) * CHANNELS || weights.size() != int64_t(n) * CHANNELS) {
		UtilityFunctions::push_error("ragdoll: a malha da variação ", variation, " não traz pele");
		return nullptr;
	}
	phys::SkinView skin;
	skin.positions = reinterpret_cast<const float *>(pos.ptr());
	skin.bones = bones.ptr();
	skin.weights = weights.ptr();
	skin.vertices = n;
	auto rig = std::make_unique<phys::PersonRig>();
	std::string error;
	if (!rig->build(skin, params_.rig, error)) {
		UtilityFunctions::push_error("ragdoll: variação ", variation, ": ", String(error.c_str()));
		return nullptr;
	}
	rigs_[size_t(variation)] = std::move(rig);
	return rigs_[size_t(variation)].get();
}

void RagdollWorld::spawn(const RagdollSpawn &s, Rng &rng) {
	const PersonLook look = asset_->look(s.look);
	const phys::PersonRig *rig = rig_for(look.variation);
	if (!rig) return;
	if (int(people_.size()) >= params_.max_people) {
		size_t oldest = 0;
		for (size_t i = 1; i < people_.size(); i++)
			if (people_[i].born < people_[oldest].born) oldest = i;
		people_.erase(people_.begin() + int64_t(oldest));
	}
	Person p;
	p.art = rig->proto;
	p.art.params = params_.solver;
	p.rig = rig;
	p.variation = look.variation;
	p.top = look.top;
	p.bottom = look.bottom;
	p.hair = look.hair;
	p.skin = look.skin;
	p.born = clock_;
	p.life = s.life_s;
	p.charred = s.charred;
	const phys::Q4 turn = phys::axis_angle(phys::V3(0, 1, 0), std::atan2(std::cos(s.heading), std::sin(s.heading))); // model +Z along the heading
	const phys::V3 feet = v3(s.feet);
	for (phys::Body &b : p.art.bodies) {
		b.q = b.q_prev = turn;
		b.x = b.x_prev = feet + turn.rotate(b.x);
		b.v = v3(s.velocity) + phys::V3((rng.f() - HALF) * s.scatter, 0, (rng.f() - HALF) * s.scatter);
	}
	for (int k = 0; k < RagdollParams::LIMBS; k++) {
		const uint32_t bit = 1u << k; // CAS_LIMB_HEAD, ARM_L, ARM_R, LEG_L, LEG_R
		const int root = params_.limb_root[k];
		const bool torn = (s.severed & bit) != 0, gone = (s.lost & bit) != 0 && !torn;
		for (const int below : rig->descendants[size_t(root)]) {
			if (gone) p.art.bodies[size_t(below)].alive = false;
			else if (torn) p.art.bodies[size_t(below)].v = v3(s.limb_velocity) + phys::V3((rng.f() - HALF) * s.scatter, 0, (rng.f() - HALF) * s.scatter);
		}
		if (torn)
			for (phys::Joint &j : p.art.joints)
				if (j.child == root) j.attached = false;
	}
	p.art.refresh();
	people_.push_back(std::move(p));
}

void RagdollWorld::step(float dt, const CityState &city) {
	const auto t0 = std::chrono::steady_clock::now();
	cost_ = phys::StepCost();
	clock_ += double(dt);
	const CityGround ground(city);
	for (size_t i = 0; i < people_.size();) {
		Person &p = people_[i];
		p.age += dt;
		if (p.age > p.life) {
			people_.erase(people_.begin() + int64_t(i));
			continue;
		}
		if (!p.at_rest) {
			p.art.step(dt, ground, cost_);
			p.at_rest = p.art.quiet_s >= params_.rest_after_s;
		}
		i++;
	}
	last_us_ = int(std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count() * MICRO);
}

bool RagdollWorld::moving() const {
	for (const Person &p : people_)
		if (!p.at_rest) return true;
	return false;
}

// Row `row` of the texture: for each cooked bone the 3x4 matrix taking a bind-pose point to its place relative to the pelvis, which
// the instance's origin is (zero for a bone of a lost part).
void RagdollWorld::write_bones(const Person &p, int row) {
	const phys::V3 origin = p.art.bodies[0].x;
	for (int bone = 0; bone < asset_->bones; bone++) {
		const int body = p.rig->bone_body[size_t(bone)];
		phys::M3 r;
		phys::V3 t;
		bool zero = body < 0 || !p.art.bodies[size_t(body)].alive;
		if (!zero) {
			const phys::Body &b = p.art.bodies[size_t(body)];
			r = b.q.matrix();
			t = b.x - r * p.rig->centre[size_t(body)] - origin;
		}
		for (int k = 0; k < ROWS_PER_BONE; k++) {
			const int64_t at = (int64_t(row) * width_ + (bone * ROWS_PER_BONE + k) * TEXELS_PER_ROW) * CHANNELS;
			const float v[CHANNELS] = { r.at(k, 0), r.at(k, 1), r.at(k, 2), t[k] };
			for (int c = 0; c < CHANNELS; c++) put_fixed(texels_, at + c, zero ? 0.f : v[c], params_.span_m);
		}
	}
}

void RagdollWorld::draw() {
	if (draws_.empty()) return;
	for (PackedFloat32Array &b : bufs_) b.resize(0);
	std::vector<Vector3> lo(draws_.size(), Vector3(NO_LIMIT, NO_LIMIT, NO_LIMIT)), hi(draws_.size(), Vector3(-NO_LIMIT, -NO_LIMIT, -NO_LIMIT));
	for (size_t i = 0; i < people_.size(); i++) {
		const Person &p = people_[i];
		write_bones(p, int(i));
		const phys::V3 o = p.art.bodies[0].x;
		const Vector3 origin(o.x, o.y, o.z);
		lo[size_t(p.variation)] = lo[size_t(p.variation)].min(origin);
		hi[size_t(p.variation)] = hi[size_t(p.variation)].max(origin);
		mm_push(bufs_[size_t(p.variation)], Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), origin, Color(1, 1, 1, 1), float(i), FRAMES_BASE * p.top,
				p.bottom + HAIR_BASE * p.hair + SKIN_BASE * p.skin, p.charred ? 1.f : 0.f);
	}
	for (size_t v = 0; v < draws_.size(); v++) {
		const bool any = bufs_[v].size() > 0;
		const AABB box = any ? AABB(lo[v] - Vector3(BOX_PAD_M, BOX_PAD_M, BOX_PAD_M), (hi[v] - lo[v]) + Vector3(BOX_PAD_M, BOX_PAD_M, BOX_PAD_M) * 2.f) : AABB();
		draws_[v].set(scenario_, asset_->variations[v].lods[0]->get_rid(), material_, bufs_[v], box, true);
	}
	if (!people_.empty()) {
		image_->set_data(width_, params_.max_people, false, Image::FORMAT_RGBA8, texels_);
		bones_->update(image_);
	}
}

Dictionary RagdollWorld::cost() const {
	Dictionary d;
	d["people"] = int(people_.size());
	d["bodies"] = cost_.bodies;
	d["joints"] = cost_.joints;
	d["points_tested"] = cost_.points_tested;
	d["contacts"] = cost_.contacts;
	d["substeps"] = cost_.substeps;
	d["us"] = last_us_;
	return d;
}

} // namespace city
