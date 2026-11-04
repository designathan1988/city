// Fills one instance buffer per (variation, level) every frame. Look hash bits (Roadcraft looks.ts): variation 24..31,
// top tinted unless bits 0..1 are 0 (colour 4..7), bottom unless bits 2..3 are 0 (colour 9..12), grey hair for seniors
// unless bits 14..15 are 0 (grey 16..17), hair 18..20, skin 21..23. The Compatibility renderer keeps instance custom
// data as half floats (exact integers to 2048), so the codes are packed small: INSTANCE_CUSTOM = (variation,
// walk frames + 64 * top, bottom + 17 * hair + 256 * skin, walk phase); top / bottom 0 = the texture colour, k + 1 =
// clothes k; hair 0..7 = hairs, 8..11 = greys. The instance colour's alpha: the share of the walk in the pose.
#include "render/agents/people_renderer.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_HALF_H = 500.f;
constexpr uint32_t VARIATION_SHIFT = 24, TWO_BITS = 3, TOP_SHIFT = 4, BOTTOM_TEST = 2, BOTTOM_SHIFT = 9, GREY_TEST = 14, GREY_SHIFT = 16;
constexpr uint32_t HAIR_SHIFT = 18, SKIN_SHIFT = 21, CLOTHES = 16, HAIRS = 8, GREYS = 4, SKINS = 8;
constexpr float FRAMES_BASE = 64.f, HAIR_BASE = 17.f, SKIN_BASE = 256.f;
constexpr int IDLE_TABLE = 32; // people.gdshader idle_seconds
constexpr int BLOB_SIDES = 10;
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

PackedColorArray hex_list(const Array &a) {
	PackedColorArray out;
	for (int i = 0; i < a.size(); i++) out.push_back(rgba8_to_color(parse_hex_color(String(a[i]).utf8().get_data())));
	return out;
}
} // namespace

bool PeopleRenderer::init(const RenderContext &ctx, const Dictionary &people, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("people", errors);
	blob_material_ = ctx.material("people_blob", errors);
	if (!errors.is_empty() || !asset_.load(String(people["asset"]), errors)) return false;
	lod_m_.clear();
	for (const Variant &m : Array(people["lod_m"])) lod_m_.push_back(float(m));
	if (int(lod_m_.size()) != asset_.lods) {
		errors.push_back("agents.json people.lod_m: um limite por nível do arquivo de pessoas");
		return false;
	}
	if (asset_.variations.size() > size_t(IDLE_TABLE)) {
		errors.push_back("pessoas: variações demais para a tabela do idle");
		return false;
	}
	PackedFloat32Array idle_s;
	idle_s.resize(IDLE_TABLE);
	for (std::vector<int> &list : by_age_) list.clear();
	for (size_t v = 0; v < asset_.variations.size(); v++) {
		const PersonVariation &pv = asset_.variations[v];
		idle_s.set(int64_t(v), pv.idle_s);
		by_age_[int(pv.child ? PedAge::CHILD : pv.senior ? PedAge::ELDER : PedAge::ADULT)].push_back(int(v));
	}
	for (const std::vector<int> &list : by_age_)
		if (list.empty()) {
			errors.push_back("pessoas: falta corpo de alguma idade");
			return false;
		}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "palette", asset_.palette->get_rid());
	rs->material_set_param(material_, "block_rows", asset_.block_rows);
	rs->material_set_param(material_, "idle_row", asset_.idle_row);
	rs->material_set_param(material_, "idle_frames", asset_.idle_frames);
	rs->material_set_param(material_, "idle_seconds", idle_s);
	rs->material_set_param(material_, "clothes", hex_list(people["clothes"]));
	rs->material_set_param(material_, "hairs", hex_list(people["hair"]));
	rs->material_set_param(material_, "greys", hex_list(people["grey"]));
	const Dictionary blob = people["blob"];
	blob_from_m_ = float(blob["from_m"]);
	blob_radius_m_ = float(blob["radius_m"]);
	blob_lift_m_ = float(blob["lift_m"]);
	MeshBuilder disc;
	disc_mesh(disc);
	blob_mesh_ = disc.bake();
	size_t n = asset_.variations.size() * size_t(asset_.lods + 1);
	draws_.assign(n, MultiMeshDraw());
	bufs_.assign(n, PackedFloat32Array());
	return true;
}

void PeopleRenderer::reset() {
	for (MultiMeshDraw &d : draws_) d.free();
	blob_.free();
}

int PeopleRenderer::update(const PoseFrame &f, const Vector3 &cam, const PoseSample &at, float radius) {
	if (draws_.empty()) return 0;
	for (PackedFloat32Array &b : bufs_) b.resize(0);
	blob_buf_.resize(0);
	const float r2 = radius * radius;
	const int nv = int(asset_.variations.size()), lods = asset_.lods, levels = lods + 1;
	RenderingServer::get_singleton()->material_set_param(material_, "idle_clock",
			float(std::fmod(double(f.stamp_ns[POSE_TRAIL - 1]) / NS_PER_S, CLOCK_WRAP_S)));
	int drawn = 0;
	for (const AgentRecord &rec : f.people) {
		Pose p = record_pose(rec, at);
		float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z, d2 = dx * dx + dz * dz;
		if (d2 > r2) continue;
		float dist = std::sqrt(d2 + dy * dy);
		int lod = 0;
		while (lod < lods - 1 && dist > lod_m_[size_t(lod)]) lod++;
		const std::vector<int> &bodies = by_age_[rec.age < PED_AGES ? rec.age : int(PedAge::ADULT)];
		int v = bodies[size_t((rec.look >> VARIATION_SHIFT) % uint32_t(bodies.size()))];
		const PersonVariation &pv = asset_.variations[size_t(v)];
		uint32_t h = rec.look;
		float top = (h & TWO_BITS) ? float((h >> TOP_SHIFT) % CLOTHES + 1) : 0.f;
		float bottom = ((h >> BOTTOM_TEST) & TWO_BITS) ? float((h >> BOTTOM_SHIFT) % CLOTHES + 1) : 0.f;
		bool grey = pv.senior && ((h >> GREY_TEST) & TWO_BITS);
		float hair = grey ? float(HAIRS + (h >> GREY_SHIFT) % GREYS) : float((h >> HAIR_SHIFT) % HAIRS);
		float skin = float((h >> SKIN_SHIFT) % SKINS);
		float phase = rec.walked / pv.walk_advance;
		const bool blob = dist > blob_from_m_;
		if (blob) {
			Pose under = p;
			under.y += blob_lift_m_;
			push_agent(blob_buf_, under, blob_radius_m_, 0, 0, 0, 0);
		}
		push_agent(bufs_[size_t(v * levels + (blob ? lods : lod))], p, 1.f, float(v), float(pv.walk_frames) + FRAMES_BASE * top, bottom + HAIR_BASE * hair + SKIN_BASE * skin,
				phase - std::floor(phase), 1.f - rec.rest);
		drawn++;
	}
	AABB box(Vector3(cam.x - radius, cam.y - AABB_HALF_H, cam.z - radius), Vector3(radius * 2, AABB_HALF_H * 2, radius * 2));
	for (int v = 0; v < nv; v++)
		for (int l = 0; l < levels; l++) {
			size_t k = size_t(v * levels + l);
			draws_[k].set(scenario_, asset_.variations[size_t(v)].lods[size_t(std::min(l, lods - 1))]->get_rid(), material_, bufs_[k], box, l < lods);
		}
	blob_.set(scenario_, blob_mesh_.rid, blob_material_, blob_buf_, box, false);
	return drawn;
}

} // namespace city
