// What the people renderer reads once (people_renderer.h): the cooked people file and its clip and face tables for the shaders, the
// clips the gait, the acts and the seats play, the held things and helmets, the blob under each walker, and agents.json's people block
// through the strict reader's values (render/data_reader.h). The frame's work is people_renderer.cpp.
#include "render/agents/people_renderer.h"
#include "core/shader_contracts/people.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// What the face does at each clip: people_face.gdshaderinc ACT_*
struct FaceAct {
	const char *clip;
	int act;
};
// A party's moods as the lab's face shows them while they talk (faceExpression.ts: the talk gesture's face): who argues or laughs speaks,
// the angry one listens.
constexpr FaceAct FACE_ACTS[] = { { "talk", FACE_ACT_TALK }, { "listen", FACE_ACT_LISTEN }, { "phone", FACE_ACT_PHONE }, { "crouchDown", FACE_ACT_CROUCH },
	{ "crouchIdle", FACE_ACT_CROUCH }, { "crouchUp", FACE_ACT_CROUCH }, { "argue", FACE_ACT_TALK }, { "laugh", FACE_ACT_TALK }, { "angry", FACE_ACT_LISTEN } };
constexpr int PALM_ROW_FLOATS = 4;
constexpr const char *ACT_CLIPS[int(PedAct::COUNT)] = { nullptr, "talk", "listen", "look", "phone", "headphones", "bag", "crouchIdle", "photo", "laugh", "angry", "argue" };
// The clip of each SeatPose and, for who puts a foot down, the one stopped.
constexpr const char *SEAT_CLIPS[int(SeatPose::COUNT)] = { "carDrive", "carRide", "carRearRide", "cabDrive", "cabRide", "chairSit", "motoRide", "bikePedal" };
constexpr const char *STOP_CLIPS[int(SeatPose::COUNT)] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "motoStop", "bikeStop" };

// The unit disc, its centre in the mesh's vertex 0: people_blob.gdshader fades it from the middle out.
void disc_mesh(MeshBuilder &b, int sides) {
	const Vector3 up(0, 1, 0);
	for (int i = 0; i < sides; i++) {
		const float a0 = float(i) * TAU_F / float(sides), a1 = float(i + 1) * TAU_F / float(sides);
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
	acts.resize(PEOPLE_CLIPS);
	acts.fill(0);
	for (const FaceAct &f : FACE_ACTS) acts.set(asset_.clip(f.clip), f.act);
	moods.resize(PEOPLE_BODIES);
	moods.fill(0);
	for (size_t v = 0; v < asset_.variations.size(); v++) moods.set(int64_t(v), asset_.variations[v].mood);
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material, "face_table", asset_.faces->get_rid());
	rs->material_set_param(material, "act_face", acts);
	rs->material_set_param(material, "body_mood", moods);
}

void PeopleRenderer::set_tables(const RID &material) const {
	PackedInt32Array clip_rows, clip_frames, clip_loop;
	clip_rows.resize(PEOPLE_CLIPS);
	clip_frames.resize(PEOPLE_CLIPS);
	clip_loop.resize(PEOPLE_CLIPS);
	for (size_t i = 0; i < asset_.clips.size(); i++) {
		clip_rows.set(int64_t(i), asset_.clips[i].row);
		clip_frames.set(int64_t(i), asset_.clips[i].frames);
		clip_loop.set(int64_t(i), asset_.clips[i].loop ? 1 : 0);
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material, "palette", asset_.palette->get_rid());
	rs->material_set_param(material, "block_rows", asset_.block_rows);
	rs->material_set_param(material, "palette_columns", asset_.columns);
	rs->material_set_param(material, "clip_row", clip_rows);
	rs->material_set_param(material, "clip_frames", clip_frames);
	rs->material_set_param(material, "clip_loop", clip_loop);
}

bool PeopleRenderer::read_glance(const DataValue &glance) {
	const DataValue seats = glance["seats"];
	for (int i = 0; i < int(SeatPose::COUNT); i++) {
		glance_.clips[i][0] = glance_.clips[i][1] = -1;
		if (!seats.has(SEAT_CLIPS[i])) continue; // a two-wheeler's rider looks ahead
		const DataValue pair = seats[SEAT_CLIPS[i]];
		for (int k = 0; k < 2; k++) {
			glance_.clips[i][k] = asset_.clip(pair[k].utf8().c_str());
			if (glance_.clips[i][k] < 0) pair[k].fail("no such clip in people.bin");
		}
	}
	glance_.window_left = asset_.clip(glance["window_left"].utf8().c_str());
	glance_.window_right = asset_.clip(glance["window_right"].utf8().c_str());
	if (glance_.window_left < 0 || glance_.window_right < 0) glance.fail("the side windows' glances are clips of people.bin");
	glance_.period_s = glance["period_s"].f();
	glance_.period_span = float(std::max(1, glance["period_span"].whole()));
	glance_.from = glance["glance_from"].f();
	glance_.to = glance["glance_to"].f();
	glance_.edge = glance["glance_edge"].f();
	if (!(glance_.edge > 0)) glance["glance_edge"].fail("an edge above zero");
	glance_.shown_over = glance["shown_over"].f();
	glance_.quiet_one_in = std::max(1, glance["quiet_one_in"].whole());
	return glance.reader.errors.is_empty();
}

// The clips the acts, the gait and the seats play: each one present in people.bin, else an error naming it.
bool PeopleRenderer::read_clips(const DataValue &people, PackedStringArray &errors) {
	const auto need = [&](const char *name) {
		const int c = asset_.clip(name);
		if (c < 0) errors.push_back(String("people: no clip ") + name);
		return std::max(c, 0);
	};
	for (int i = 0; i < int(PedAct::COUNT); i++) standing_.act[i] = ACT_CLIPS[i] ? need(ACT_CLIPS[i]) : 0;
	standing_.idle = need("idle");
	standing_.crouch_down = need("crouchDown");
	standing_.crouch_idle = need("crouchIdle");
	standing_.crouch_up = need("crouchUp");
	for (const FaceAct &f : FACE_ACTS) need(f.clip); // set_faces writes each act at its clip's row
	for (int i = 0; i < int(SeatPose::COUNT); i++) {
		seat_clips_[i] = need(SEAT_CLIPS[i]);
		stop_clips_[i] = STOP_CLIPS[i] ? need(STOP_CLIPS[i]) : -1;
	}
	return errors.is_empty() && read_glance(people["glance"]) && gait_.init(people["gait"], asset_, standing_);
}

bool PeopleRenderer::init(const RenderContext &ctx, const DataValue &people, const DataValue &held, const AgentShapes &shapes, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("people", errors);
	blob_material_ = ctx.material("people_blob", errors);
	far_material_ = ctx.material("people_far", errors);
	const String asset = people["asset"].text(), atlas = people["atlas"].text();
	if (!errors.is_empty() || !asset_.load(asset, atlas, errors)) return false;
	lod_m_.clear();
	const DataValue lods = people["lod_m"];
	for (int i = 0; i < lods.size(); i++) lod_m_.push_back(lods[i].f());
	if (int(lod_m_.size()) != asset_.lods) {
		lods.fail("one limit for each level of the people file");
		return false;
	}
	if (asset_.variations.size() > size_t(PEOPLE_BODIES)) {
		errors.push_back("pessoas: variações demais para as tabelas dos corpos");
		return false;
	}
	PackedVector4Array palms, shells;
	palms.resize(PEOPLE_BODIES * PEOPLE_HAND_ROWS);
	shells.resize(PEOPLE_BODIES * PEOPLE_HAND_ROWS);
	for (std::vector<int> &list : by_age_) list.clear();
	for (size_t v = 0; v < asset_.variations.size(); v++) {
		const PersonVariation &pv = asset_.variations[v];
		for (int r = 0; r < PEOPLE_HAND_ROWS; r++) {
			const float *row = pv.palm + r * PALM_ROW_FLOATS;
			palms.set(int64_t(v) * PEOPLE_HAND_ROWS + r, Vector4(row[0], row[1], row[2], row[3]));
			const float *shell = pv.helmet + r * PALM_ROW_FLOATS;
			shells.set(int64_t(v) * PEOPLE_HAND_ROWS + r, Vector4(shell[0], shell[1], shell[2], shell[3]));
		}
		by_age_[int(pv.child ? PedAge::CHILD : pv.senior ? PedAge::ELDER : PedAge::ADULT)].push_back(int(v));
	}
	for (const std::vector<int> &list : by_age_)
		if (list.empty()) {
			errors.push_back("pessoas: falta corpo de alguma idade");
			return false;
		}
	if (asset_.clips.size() > size_t(PEOPLE_CLIPS)) {
		errors.push_back("pessoas: clipes demais para a tabela do shader");
		return false;
	}
	if (!read_clips(people, errors) || !held_.init(ctx, held, ACT_CLIPS, errors) || !helmets_.init(ctx, held["helmet"], errors) || !casting_.init(people["cast"], asset_.variations)) return false;
	RenderingServer *rs = RenderingServer::get_singleton();
	set_tables(material_);
	set_faces(material_);
	set_tables(held_.material());
	set_tables(helmets_.material());
	rs->material_set_param(helmets_.material(), "head_bone", asset_.head_bone);
	rs->material_set_param(helmets_.material(), "helmet_rows", shells);
	rs->material_set_param(held_.material(), "hand_bone", asset_.hand_bone);
	rs->material_set_param(held_.material(), "palm_rows", palms);
	rs->material_set_param(held_.material(), "fist", held["fist"].v3());
	rs->material_set_param(material_, "atlas", asset_.atlas->get_rid());
	rs->material_set_param(material_, "alpha_test", asset_.variations.front().alpha_test);
	if (!bodies_.init(ctx, asset_, shapes.ragdoll, errors)) return false;
	rs->material_set_param(far_material_, "atlas", asset_.atlas->get_rid());
	rs->material_set_param(far_material_, "strip", asset_.card_strip);
	rs->material_set_param(far_material_, "frames", float(asset_.card_frames));
	const DataValue blob = people["blob"];
	blob_from_m_ = blob["from_m"].f();
	view_reach_m_ = people["view_reach_m"].f();
	blob_radius_m_ = blob["radius_m"].f();
	blob_lift_m_ = blob["lift_m"].f();
	const int blob_sides = blob["sides"].whole();
	if (!errors.is_empty()) return false;
	MeshBuilder disc;
	disc_mesh(disc, blob_sides);
	blob_mesh_ = disc.bake();
	size_t n = asset_.variations.size() * size_t(asset_.lods + 1);
	draws_.assign(n, MultiMeshDraw());
	bufs_.assign(n, InstanceRows());
	keeper_.clear();
	keeper_.draws(n);
	return true;
}

} // namespace city
