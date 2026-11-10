// Fills one instance buffer per (body, level) every frame. Look hash bits: body 24..31; the low ones spread the idles. INSTANCE_CUSTOM =
// (body, walk frames, a seed, walk phase); on the far card (body, its height, the seed, 0). The instance colour's alpha: the share of the
// walk in the pose, or from CLIP_CODE_BASE a clip held (somebody in a vehicle).
#include "render/agents/people_renderer.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t VARIATION_SHIFT = 24, SEED_MASK = 1023;
constexpr float FRIGHT_MARK = 0.25f; // people_face.gdshaderinc: a seed below zero is a frightened person's, this short of its number
// What the face does at each clip: people_face.gdshaderinc ACT_*
struct FaceAct {
	const char *clip;
	int act;
};
constexpr FaceAct FACE_ACTS[] = { { "talk", 1 }, { "listen", 2 }, { "phone", 3 }, { "crouchDown", 4 }, { "crouchIdle", 4 }, { "crouchUp", 4 } };
constexpr int BODY_TABLE = 32; // people_pose.gdshaderinc walk_frames, body_female; people_held.gdshader palm_rows
constexpr int BLOB_SIDES = 10;
constexpr int CLIP_TABLE = 24; // people_pose.gdshaderinc clip_row, clip_frames, clip_seconds
constexpr float ACT_SPAN = .96f; // people_pose.gdshaderinc: the whole of an act, short of the next clip's number
constexpr int PALM_ROWS = 3, PALM_ROW_FLOATS = 4;
constexpr const char *ACT_CLIPS[int(PedAct::COUNT)] = { nullptr, "talk", "listen", "look", "phone", "headphones", "bag", "crouchIdle", "photo" };
constexpr float CLIP_CODE_BASE = 2.f; // an instance colour's alpha from here on: the clip of that number, held (people.gdshader)
// The clip of each SeatPose and, for who puts a foot down, the one stopped.
constexpr const char *SEAT_CLIPS[int(SeatPose::COUNT)] = { "carDrive", "carRide", "carRearRide", "cabDrive", "cabRide", "chairSit", "motoRide", "bikePedal" };
constexpr const char *STOP_CLIPS[int(SeatPose::COUNT)] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "motoStop", "bikeStop" };
constexpr double NS_PER_S = 1e9, CLOCK_WRAP_S = 86400.0; // the idles' clock wraps once a day: a float keeps its milliseconds

// The unit disc, its centre in the mesh's vertex 0: people_blob.gdshader fades it from the middle out.
void disc_mesh(MeshBuilder &b) {
	const Vector3 up(0, 1, 0);
	for (int i = 0; i < BLOB_SIDES; i++) {
		const float a0 = float(i) * TAU_F / BLOB_SIDES, a1 = float(i + 1) * TAU_F / BLOB_SIDES;
		const int first = int(b.v.size());
		for (const Vector3 &p : { Vector3(0, 0, 0), Vector3(std::cos(a0), 0, std::sin(a0)), Vector3(std::cos(a1), 0, std::sin(a1)) }) {
			b.v.push_back(p);
			b.n.push_back(up);
			b.c.push_back(Color(1, 1, 1, 1));
			b.uv.push_back(Vector2());
			b.uv2.push_back(Vector2());
		}
		b.tri(first, first + 1, first + 2, up);
	}
}

} // namespace

void PeopleRenderer::set_faces(const RID &material) const {
	PackedInt32Array acts;
	PackedFloat32Array moods;
	acts.resize(CLIP_TABLE);
	acts.fill(0);
	for (const FaceAct &f : FACE_ACTS) acts.set(asset_.clip(f.clip), f.act);
	moods.resize(BODY_TABLE);
	moods.fill(0);
	for (size_t v = 0; v < asset_.variations.size(); v++) moods.set(int64_t(v), asset_.variations[v].mood);
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material, "face_table", asset_.faces->get_rid());
	rs->material_set_param(material, "act_face", acts);
	rs->material_set_param(material, "body_mood", moods);
}

void PeopleRenderer::set_tables(const RID &material) const {
	PackedInt32Array clip_rows, clip_frames, walk_frames, female;
	PackedFloat32Array seconds;
	clip_rows.resize(CLIP_TABLE);
	clip_frames.resize(CLIP_TABLE);
	seconds.resize(CLIP_TABLE * 2);
	for (size_t i = 0; i < asset_.clips.size(); i++) {
		clip_rows.set(int64_t(i), asset_.clips[i].row);
		clip_frames.set(int64_t(i), asset_.clips[i].frames);
		for (int sex = 0; sex < 2; sex++) seconds.set(int64_t(i) * 2 + sex, asset_.clips[i].seconds[sex]);
	}
	walk_frames.resize(BODY_TABLE);
	female.resize(BODY_TABLE);
	for (size_t v = 0; v < asset_.variations.size(); v++) {
		walk_frames.set(int64_t(v), asset_.variations[v].walk_frames);
		female.set(int64_t(v), asset_.variations[v].female ? 1 : 0);
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material, "palette", asset_.palette->get_rid());
	rs->material_set_param(material, "block_rows", asset_.block_rows);
	rs->material_set_param(material, "idle_clip", asset_.clip("idle"));
	rs->material_set_param(material, "clip_row", clip_rows);
	rs->material_set_param(material, "clip_frames", clip_frames);
	rs->material_set_param(material, "clip_seconds", seconds);
	rs->material_set_param(material, "walk_frames", walk_frames);
	rs->material_set_param(material, "body_female", female);
}

bool PeopleRenderer::init(const RenderContext &ctx, const Dictionary &people, const Dictionary &held, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("people", errors);
	blob_material_ = ctx.material("people_blob", errors);
	far_material_ = ctx.material("people_far", errors);
	if (!errors.is_empty() || !asset_.load(String(people["asset"]), String(people["atlas"]), material_, far_material_, errors)) return false;
	lod_m_.clear();
	for (const Variant &m : Array(people["lod_m"])) lod_m_.push_back(float(m));
	if (int(lod_m_.size()) != asset_.lods) {
		errors.push_back("agents.json people.lod_m: um limite por nível do arquivo de pessoas");
		return false;
	}
	if (asset_.variations.size() > size_t(BODY_TABLE)) {
		errors.push_back("pessoas: variações demais para as tabelas dos corpos");
		return false;
	}
	PackedVector4Array palms, shells;
	palms.resize(BODY_TABLE * PALM_ROWS);
	shells.resize(BODY_TABLE * PALM_ROWS);
	for (std::vector<int> &list : by_age_) list.clear();
	for (size_t v = 0; v < asset_.variations.size(); v++) {
		const PersonVariation &pv = asset_.variations[v];
		for (int r = 0; r < PALM_ROWS; r++) {
			const float *row = pv.palm + r * PALM_ROW_FLOATS;
			palms.set(int64_t(v) * PALM_ROWS + r, Vector4(row[0], row[1], row[2], row[3]));
			const float *shell = pv.helmet + r * PALM_ROW_FLOATS;
			shells.set(int64_t(v) * PALM_ROWS + r, Vector4(shell[0], shell[1], shell[2], shell[3]));
		}
		by_age_[int(pv.child ? PedAge::CHILD : pv.senior ? PedAge::ELDER : PedAge::ADULT)].push_back(int(v));
	}
	for (const std::vector<int> &list : by_age_)
		if (list.empty()) {
			errors.push_back("pessoas: falta corpo de alguma idade");
			return false;
		}
	if (asset_.clips.size() > size_t(CLIP_TABLE)) {
		errors.push_back("pessoas: clipes demais para a tabela do shader");
		return false;
	}
	for (int i = 0; i < int(PedAct::COUNT); i++) {
		act_clips_[i] = ACT_CLIPS[i] ? asset_.clip(ACT_CLIPS[i]) : 0;
		if (act_clips_[i] < 0) errors.push_back(String("pessoas: falta o clipe ") + ACT_CLIPS[i]);
	}
	crouch_down_ = asset_.clip("crouchDown");
	crouch_up_ = asset_.clip("crouchUp");
	if (crouch_down_ < 0 || crouch_up_ < 0) errors.push_back("pessoas: falta o clipe de agachar");
	for (int i = 0; i < int(SeatPose::COUNT); i++) {
		seat_clips_[i] = asset_.clip(SEAT_CLIPS[i]);
		stop_clips_[i] = STOP_CLIPS[i] ? asset_.clip(STOP_CLIPS[i]) : -1;
		if (seat_clips_[i] < 0) errors.push_back(String("pessoas: falta o clipe ") + SEAT_CLIPS[i]);
	}
	if (!errors.is_empty() || !held_.init(ctx, held, ACT_CLIPS, errors) || !helmets_.init(ctx, held["helmet"], errors)) return false;
	RenderingServer *rs = RenderingServer::get_singleton();
	set_tables(material_);
	set_faces(material_);
	set_tables(held_.material());
	set_tables(helmets_.material());
	rs->material_set_param(helmets_.material(), "head_bone", asset_.head_bone);
	rs->material_set_param(helmets_.material(), "helmet_rows", shells);
	rs->material_set_param(held_.material(), "hand_bone", asset_.hand_bone);
	rs->material_set_param(held_.material(), "palm_rows", palms);
	const Array fist = held["fist"];
	rs->material_set_param(held_.material(), "fist", Vector3(float(fist[0]), float(fist[1]), float(fist[2])));
	rs->material_set_param(material_, "atlas", asset_.atlas->get_rid());
	rs->material_set_param(material_, "alpha_test", asset_.variations.front().alpha_test);
	if (!bodies_.init(ctx, asset_, errors)) return false;
	rs->material_set_param(far_material_, "atlas", asset_.atlas->get_rid());
	rs->material_set_param(far_material_, "strip", asset_.card_strip);
	rs->material_set_param(far_material_, "frames", float(asset_.card_frames));
	const Dictionary blob = people["blob"];
	blob_from_m_ = float(blob["from_m"]);
	view_reach_m_ = float(people["view_reach_m"]);
	blob_radius_m_ = float(blob["radius_m"]);
	blob_lift_m_ = float(blob["lift_m"]);
	MeshBuilder disc;
	disc_mesh(disc);
	blob_mesh_ = disc.bake(blob_material_).shared();
	size_t n = asset_.variations.size() * size_t(asset_.lods + 1);
	draws_.assign(n, MultiMeshDraw());
	bufs_.assign(n, InstanceRows());
	keeper_.clear();
	keeper_.draws(n);
	return true;
}

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
	place(PEOPLE_DENSE_KEYS + key, v, level, at, look, 0.f, phase, CLIP_CODE_BASE + float(clip));
	if (spot.pose == SeatPose::MOTO && level < asset_.lods) agent_row(helmets_.row(key), at, 1.f, float(v), 0.f, float(look & SEED_MASK), phase, CLIP_CODE_BASE + float(clip));
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
		float doing = rec.act_share > 0 ? float(act_clips_[act]) + rec.act_share * ACT_SPAN : 0.f;
		float walk = phase - std::floor(phase);
		if (act == int(PedAct::CROUCH) && doing > 0) { // down, a while there and up again (citizenGait.ts): played from its start, not by the clock
			const Played at_it = crouch_at(pv.female ? 1 : 0, rec.act_since, rec.act_left);
			doing = -(float(at_it.clip) + rec.act_share * ACT_SPAN);
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
			draws_[k].set(scenario_, l < lods ? asset_.variations[size_t(v)].lods[size_t(l)].rid : asset_.card.rid, bufs_[k], box, l < lods, LAYER_PEOPLE);
		}
	blob_.set(scenario_, blob_mesh_.rid, blob_buf_, box, false, LAYER_PEOPLE);
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
