// Fills one instance buffer per (body, level) every frame. Look hash bits: body 24..31; the low ones spread the idles. INSTANCE_CUSTOM =
// (body, walk frames, a seed, walk phase); on the far card (body, its height, the seed, 0). The instance colour's alpha: the share of the
// walk in the pose, or from PEOPLE_CLIP_CODE_BASE a clip held (somebody in a vehicle). What it reads once is people_boot.cpp.
#include "render/agents/people_renderer.h"
#include "core/shader_contracts/people.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t VARIATION_SHIFT = 24, SEED_MASK = 1023;
constexpr float FRIGHT_MARK = 0.25f; // people_face.gdshaderinc: a seed below zero is a frightened person's, this short of its number
constexpr double NS_PER_S = 1e9, CLOCK_WRAP_S = 86400.0; // the idles' clock wraps once a day: a float keeps its milliseconds

} // namespace

void PeopleRenderer::reset() {
	bodies_.release();
	held_.reset();
	helmets_.reset();
	for (MultiMeshDraw &d : draws_) d.free();
	blob_.free();
}

Array PeopleRenderer::clips() const {
	Array out;
	if (asset_.variations.empty()) return out; // no people.bin read: no body to fit a clip on
	for (size_t c = 0; c < asset_.clips.size(); c++) {
		const PersonClip &clip = asset_.clips[c];
		const PersonFit &fit = asset_.variations.front().fits[c];
		Dictionary d;
		d["name"] = String(clip.name.c_str());
		d["row"] = clip.row;
		d["frames"] = clip.frames;
		d["pelvis"] = fit.pelvis;
		d["above"] = fit.above;
		d["below"] = fit.below;
		d["ahead"] = fit.ahead;
		d["side"] = fit.side;
		out.push_back(d);
	}
	return out;
}

bool PeopleRenderer::show_clip(const String &name) {
	const int c = asset_.clip(name.utf8().get_data());
	if (c < 0) return false;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "idle_clip", c);
	rs->material_set_param(held_.material(), "idle_clip", c);
	rs->material_set_param(helmets_.material(), "idle_clip", c);
	return true;
}

void PeopleRenderer::begin() {
	for (InstanceRows &b : bufs_) b.reopen();
	blob_buf_.restart();
	keeper_.begin();
	held_.begin();
	helmets_.begin();
}

int PeopleRenderer::level_at(float dist) const {
	int lod = std::min(floor_, asset_.lods - 1);
	while (lod < asset_.lods - 1 && dist > lod_m_[size_t(lod)]) lod++;
	return lod;
}

// A walker keeps its level until it is past the edge by the hold, or it flickers there.
int PeopleRenderer::held_level(uint64_t key, float dist) {
	const auto at = [&](float d) { return d > blob_from_m_ ? asset_.lods : level_at(d); };
	const int now = at(dist), place = keeper_.draw_of(key), was = place < 0 ? now : place % (asset_.lods + 1);
	if (was == now) return now;
	return now > was ? (at(dist / (1 + level_hold_)) > was ? now : was) : (at(dist * (1 + level_hold_)) < was ? now : was);
}

int PeopleRenderer::variation_of(uint32_t look, int age) const {
	const std::vector<int> &bodies = by_age_[age >= 0 && age < PED_AGES ? age : int(PedAge::ADULT)];
	return bodies.empty() ? 0 : bodies[size_t((look >> VARIATION_SHIFT) % uint32_t(bodies.size()))];
}

void PeopleRenderer::place(uint64_t key, int v, int level, const Pose &p, uint32_t h, float doing, float phase, float pose_code, bool frightened) {
	const PersonVariation &pv = asset_.variations[size_t(v)];
	const size_t k = slot(size_t(v), level);
	agent_row(keeper_.place(key, int(k), bufs_[k]), p, 1.f, float(v), level == asset_.lods ? pv.height : doing, frightened ? -(float(h & SEED_MASK) + FRIGHT_MARK) : float(h & SEED_MASK), phase, pose_code);
}

void PeopleRenderer::seat(uint64_t key, const Pose &vehicle, const SeatSpot &spot, uint32_t look, float dist, float phase, bool stopped) {
	if (draws_.empty()) return;
	const int pose = int(spot.pose), clip = stopped && stop_clips_[pose] >= 0 ? stop_clips_[pose] : seat_clips_[pose];
	if (clip < 0) return;
	const std::vector<int> &bodies = by_age_[int(PedAge::ADULT)];
	const int v = bodies[size_t((look >> VARIATION_SHIFT) % uint32_t(bodies.size()))];
	// the pelvis on the hip point, in the vehicle's frame: its left, its up on a grade, its heading
	const Vector3 off = spot.hip - asset_.variations[size_t(v)].fits[size_t(clip)].pelvis;
	const float flat = 1.f / std::sqrt(1.f + vehicle.rise * vehicle.rise), up = flat * vehicle.rise;
	Pose at = vehicle;
	at.x += vehicle.fz * off.x - vehicle.fx * up * off.y + vehicle.fx * flat * off.z;
	at.y += flat * off.y + up * off.z;
	at.z += -vehicle.fx * off.x - vehicle.fz * up * off.y + vehicle.fz * flat * off.z;
	const int level = dist > blob_from_m_ ? asset_.lods : level_at(dist);
	place(PEOPLE_DENSE_KEYS + key, v, level, at, look, 0.f, phase, PEOPLE_CLIP_CODE_BASE + float(clip));
	if (spot.pose == SeatPose::MOTO && level < asset_.lods) agent_row(helmets_.row(key), at, 1.f, float(v), 0.f, float(look & SEED_MASK), phase, PEOPLE_CLIP_CODE_BASE + float(clip));
}

int PeopleRenderer::update(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const PoseSample &at, float radius) {
	if (draws_.empty()) return 0;
	const float r2 = radius * radius;
	const int nv = int(asset_.variations.size()), lods = asset_.lods, levels = lods + 1;
	const float clock = float(std::fmod(double(f.stamp_ns[POSE_TRAIL - 1]) / NS_PER_S, CLOCK_WRAP_S));
	RenderingServer::get_singleton()->material_set_param(material_, "idle_clock", clock);
	RenderingServer::get_singleton()->material_set_param(held_.material(), "idle_clock", clock);
	int drawn = 0;
	for (const AgentRecord &rec : f.people) {
		Pose p = record_pose(rec, at);
		float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z, d2 = dx * dx + dz * dz;
		if (d2 > r2) continue;
		float dist = std::sqrt(d2 + dy * dy);
		if (!view.sees(Vector3(p.x, p.y, p.z), view_reach_m_)) continue; // their shadow included: this far out of the view it reaches nothing seen
		const std::vector<int> &bodies = by_age_[rec.age < PED_AGES ? rec.age : int(PedAge::ADULT)];
		int v = bodies[size_t((rec.look >> VARIATION_SHIFT) % uint32_t(bodies.size()))];
		const PersonVariation &pv = asset_.variations[size_t(v)];
		float phase = rec.walked / pv.walk_advance;
		const int level = held_level(rec.id, dist);
		const bool blob = level == lods;
		if (blob) {
			Pose under = p;
			under.y += blob_lift_m_;
			agent_row(blob_buf_.add(), under, blob_radius_m_, 0, 0, 0, 0);
		}
		const int act = rec.act < int(PedAct::COUNT) ? rec.act : 0;
		float doing = rec.act_share > 0 ? float(act_clips_[act]) + rec.act_share * PEOPLE_ACT_SPAN : 0.f;
		float walk = phase - std::floor(phase);
		if (act == int(PedAct::CROUCH) && doing > 0) { // down, a while there and up again (citizenGait.ts): played from its start, not by the clock
			const Played at_it = crouch_at(pv.female ? 1 : 0, rec.act_since, rec.act_left);
			doing = -(float(at_it.clip) + rec.act_share * PEOPLE_ACT_SPAN);
			walk = at_it.phase;
		}
		place(rec.id, v, level, p, rec.look, doing, walk, 1.f - rec.rest, rec.frightened);
		if (level == 0 && doing > 0) {
			if (float *row = held_.row(rec.id, act)) agent_row(row, p, 1.f, float(v), doing, float(rec.look & SEED_MASK), walk, 1.f - rec.rest);
		}
		drawn++;
	}
	keeper_.sweep([&](int k) -> InstanceRows & { return bufs_[size_t(k)]; });
	AABB box(Vector3(cam.x - radius, cam.y - AABB_HALF_H, cam.z - radius), Vector3(radius * 2, AABB_HALF_H * 2, radius * 2));
	for (int v = 0; v < nv; v++)
		for (int l = 0; l < levels; l++) {
			size_t k = slot(size_t(v), l);
			draws_[k].set(scenario_, l < lods ? asset_.variations[size_t(v)].lods[size_t(l)]->get_rid() : asset_.card->get_rid(), l < lods ? material_ : far_material_,
					bufs_[k], box, l < lods, LAYER_PEOPLE);
		}
	blob_.set(scenario_, blob_mesh_.rid, blob_material_, blob_buf_, box, false, LAYER_PEOPLE);
	held_.draw(scenario_, box, LAYER_PEOPLE);
	helmets_.draw(scenario_, box, LAYER_PEOPLE);
	return drawn;
}

PeopleRenderer::Played PeopleRenderer::crouch_at(int female, float since, float left) const {
	const PersonClip &down = asset_.clips[size_t(crouch_down_)], &up = asset_.clips[size_t(crouch_up_)];
	const PersonClip &stay = asset_.clips[size_t(act_clips_[int(PedAct::CROUCH)])];
	// the last frame is the end of a clip played once: a clip's phase 1 is its first frame again
	auto once = [](const PersonClip &c, float share) { return clampf(share, 0.f, 1.f) * float(c.frames - 1) / float(c.frames); };
	if (left < up.seconds[female]) return { crouch_up_, once(up, 1.f - left / up.seconds[female]) };
	if (since < down.seconds[female]) return { crouch_down_, once(down, since / down.seconds[female]) };
	const float turns = (since - down.seconds[female]) / stay.seconds[female];
	return { act_clips_[int(PedAct::CROUCH)], turns - std::floor(turns) };
}

int PeopleRenderer::shown(int level) const {
	int n = 0;
	for (size_t v = 0; v < asset_.variations.size(); v++) n += bufs_[slot(v, level)].count();
	return n;
}

} // namespace city
