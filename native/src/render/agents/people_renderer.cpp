// Fills one instance buffer per (body, level) every frame, and each figure's row of the plays texture (people_plays.h). Look hash bits: a
// seated body 24..31 (a walker's is cast, people_casting.h); the low ten the figure's seed. INSTANCE_CUSTOM = (body, its plays' row, the
// seed, what is held's share); on the far card (body, its height, the seed, 0). What it reads once is people_boot.cpp.
#include "render/agents/people_renderer.h"
#include "core/shader_contracts/people.h"

#include "core/random.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t VARIATION_SHIFT = 24; // a look's top byte picks a seated body
constexpr float FRIGHT_MARK = 0.25f; // people_face.gdshaderinc: a seed below zero is a frightened person's, this short of its number
constexpr double NS_PER_S = 1e9, CLOCK_WRAP_S = 86400.0; // the clock wraps once a day: a float keeps its milliseconds
constexpr uint32_t GLANCE_PERIOD_SHIFT = 8, BYTE = 255, VARIANT_SHIFT = 4; // occupants.ts: the hash's fields for a person's glances
constexpr float BYTE_SPAN = 256.f; // a byte's values: its share of a cycle (occupants.ts ((h >>> 8) & 0xff) / 256)

float smooth(float x) {
	const float t = clampf(x, 0.f, 1.f);
	return t * t * (3 - 2 * t);
}
} // namespace

void PeopleRenderer::reset() {
	bodies_.release();
	held_.reset();
	helmets_.reset();
	plays_.reset();
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
		d["stride"] = fit.stride;
		out.push_back(d);
	}
	return out;
}

Dictionary PeopleRenderer::card() const {
	Dictionary d;
	d["file"] = asset_.card_strip;
	d["material"] = far_material_.is_valid() ? RenderingServer::get_singleton()->material_get_param(far_material_, "strip") : Variant();
	return d;
}

bool PeopleRenderer::show_clip(const String &name) {
	if (name.is_empty()) { // everybody back to what the gait and the seats play
		forced_clip_ = -1;
		return true;
	}
	const int c = asset_.clip(name.utf8().get_data());
	if (c < 0) return false;
	forced_clip_ = c;
	return true;
}

void PeopleRenderer::begin(const PoseFrame &f) {
	for (InstanceRows &b : bufs_) b.reopen();
	blob_buf_.restart();
	keeper_.begin();
	held_.begin();
	helmets_.begin();
	plays_.begin();
	clock_ = std::fmod(double(f.stamp_ns[POSE_TRAIL - 1]) / NS_PER_S, CLOCK_WRAP_S);
	gait_.set_tick(float(double(f.period_ns) / NS_PER_S));
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

void PeopleRenderer::place(uint64_t key, int v, int level, const Pose &p, float scale, uint32_t h, int row, bool frightened) {
	const PersonVariation &pv = asset_.variations[size_t(v)];
	const size_t k = slot(size_t(v), level);
	const float seed = float(h & PEOPLE_SEED_MASK);
	agent_row(keeper_.place(key, int(k), bufs_[k]), p, scale, float(v), level == asset_.lods ? pv.height : float(row), frightened ? -(seed + FRIGHT_MARK) : seed, 0.f);
}

// occupants.ts occupantPlays: the seat's own pose, and once a cycle (but one in quiet_one_in) a glance aside eased in and out.
int PeopleRenderer::seated_clip(SeatPose pose, uint32_t look, bool right_side) const {
	const int base = seat_clips_[int(pose)], glance = glance_.clips[int(pose)][0];
	if (glance < 0) return base;
	const uint32_t h = hash_u32(look);
	const double period = double(glance_.period_s) + double(h % uint32_t(glance_.period_span));
	const double t = clock_ / period + double((h >> GLANCE_PERIOD_SHIFT) & BYTE) / double(BYTE_SPAN);
	const double cycle = std::floor(t);
	const float u = float(t - cycle);
	const uint32_t c = hash_u32(h ^ uint32_t(int64_t(cycle)));
	const float shown = c % uint32_t(glance_.quiet_one_in) == 0 ? 0.f
			: smooth((u - glance_.from) / glance_.edge) * (1.f - smooth((u - glance_.to) / glance_.edge));
	if (shown <= glance_.shown_over) return base;
	const int variant = int((c >> VARIANT_SHIFT) & 1u);
	if (pose == SeatPose::CHAIR && variant == 1) return right_side ? glance_.window_right : glance_.window_left; // out of their own side's window
	return glance_.clips[int(pose)][variant];
}

void PeopleRenderer::seat(uint64_t key, const Pose &vehicle, const SeatSpot &spot, uint32_t look, float dist, float phase, bool stopped) {
	if (draws_.empty()) return;
	const int pose = int(spot.pose);
	int clip = stopped && stop_clips_[pose] >= 0 ? stop_clips_[pose] : seated_clip(spot.pose, look, spot.hip.x < 0.f);
	if (forced_clip_ >= 0) clip = forced_clip_;
	if (clip < 0) return;
	const std::vector<int> &bodies = by_age_[int(PedAge::ADULT)];
	const int v = bodies[size_t((look >> VARIATION_SHIFT) % uint32_t(bodies.size()))];
	// the pelvis on the hip point, in the vehicle's frame: its left, its up on a grade, its heading
	const Vector3 off = spot.hip - asset_.variations[size_t(v)].fits[size_t(seat_clips_[pose])].pelvis;
	const float flat = 1.f / std::sqrt(1.f + vehicle.rise * vehicle.rise), up = flat * vehicle.rise;
	Pose at = vehicle;
	at.x += vehicle.fz * off.x - vehicle.fx * up * off.y + vehicle.fx * flat * off.z;
	at.y += flat * off.y + up * off.z;
	at.z += -vehicle.fx * off.x - vehicle.fz * up * off.y + vehicle.fz * flat * off.z;
	const int level = dist > blob_from_m_ ? asset_.lods : level_at(dist);
	int row = 0;
	if (level < asset_.lods) {
		// a two-wheeler's cranks turn with its wheels; a seated idle runs from each person's own point of it (occupants.ts)
		const bool crank = spot.pose == SeatPose::BICYCLE;
		const Play play{ clip, crank ? phase : gait_.clips().clock_phase(clip, v, clock_, look & PEOPLE_SEED_MASK), 1.f };
		row = plays_.put(&play, 1);
	}
	place(PEOPLE_DENSE_KEYS + key, v, level, at, 1.f, look, row);
	if (spot.pose == SeatPose::MOTO && level < asset_.lods) agent_row(helmets_.row(key), at, 1.f, float(v), float(row), float(look & PEOPLE_SEED_MASK), 0.f);
}

int PeopleRenderer::update(const PoseFrame &f, const Vector3 &cam, const ViewCull &view, const PoseSample &at, float radius) {
	if (draws_.empty()) return 0;
	const float r2 = radius * radius;
	const int nv = int(asset_.variations.size()), lods = asset_.lods, levels = lods + 1;
	RenderingServer::get_singleton()->material_set_param(material_, "idle_clock", float(clock_));
	casting_.cast(f.people);
	leading_.assign(asset_.clips.size(), 0);
	int drawn = 0;
	Play plays[PEOPLE_PLAYS];
	for (const AgentRecord &rec : f.people) {
		Pose p = record_pose(rec, at);
		float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z, d2 = dx * dx + dz * dz;
		if (d2 > r2) continue;
		float dist = std::sqrt(d2 + dy * dy);
		if (!view.sees(Vector3(p.x, p.y, p.z), view_reach_m_)) continue; // their shadow included: this far out of the view it reaches nothing seen
		const int v = casting_.body(rec.id);
		const int level = held_level(rec.id, dist);
		if (level == lods) {
			Pose under = p;
			under.y += blob_lift_m_;
			agent_row(blob_buf_.add(), under, blob_radius_m_, 0, 0, 0, 0);
		}
		int row = 0;
		if (level < lods) {
			int n = gait_.step(rec, p, v, rec.scale, clock_, plays);
			if (forced_clip_ >= 0) {
				plays[0] = Play{ forced_clip_, gait_.clips().clock_phase(forced_clip_, v, clock_, rec.look & PEOPLE_SEED_MASK), 1.f };
				n = 1;
			}
			if (n > 0) leading_[size_t(plays[0].clip)]++;
			row = plays_.put(plays, n);
		}
		place(rec.id, v, level, p, rec.scale, rec.look, row, rec.frightened);
		const int act = rec.act < int(PedAct::COUNT) ? rec.act : 0;
		if (level == 0 && act != 0 && rec.act_share > 0) {
			if (float *held = held_.row(rec.id, act)) agent_row(held, p, rec.scale, float(v), float(row), float(rec.look & PEOPLE_SEED_MASK), rec.act_share);
		}
		drawn++;
	}
	const RID posing[] = { material_, held_.material(), helmets_.material() };
	plays_.upload(posing, int(std::size(posing)));
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

Dictionary PeopleRenderer::plays_census() const {
	Dictionary out;
	for (size_t c = 0; c < leading_.size(); c++)
		if (leading_[c] > 0) out[String(asset_.clips[c].name.c_str())] = leading_[c];
	return out;
}

int PeopleRenderer::shown(int level) const {
	int n = 0;
	for (size_t v = 0; v < asset_.variations.size(); v++) n += bufs_[slot(v, level)].count();
	return n;
}

} // namespace city
