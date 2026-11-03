// Fills one instance buffer per (variation, level) every frame. Look hash bits (Roadcraft looks.ts): variation 24..31,
// top tinted unless bits 0..1 are 0 (colour 4..7), bottom unless bits 2..3 are 0 (colour 9..12), grey hair for seniors
// unless bits 14..15 are 0 (grey 16..17), hair 18..20, skin 21..23. The Compatibility renderer keeps instance custom
// data as half floats (exact integers to 2048), so the codes are packed small: INSTANCE_CUSTOM = (palette row,
// frames + 64 * top, bottom + 17 * hair + 256 * skin, walk phase); top / bottom 0 = the texture colour, k + 1 = clothes
// k; hair 0..7 = hairs, 8..11 = greys.
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
	if (!errors.is_empty() || !asset_.load(String(people["asset"]), errors)) return false;
	lod_m_.clear();
	for (const Variant &m : Array(people["lod_m"])) lod_m_.push_back(float(m));
	if (int(lod_m_.size()) != asset_.lods) {
		errors.push_back("agents.json people.lod_m: um limite por nível do arquivo de pessoas");
		return false;
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "palette", asset_.palette->get_rid());
	rs->material_set_param(material_, "clothes", hex_list(people["clothes"]));
	rs->material_set_param(material_, "hairs", hex_list(people["hair"]));
	rs->material_set_param(material_, "greys", hex_list(people["grey"]));
	size_t n = asset_.variations.size() * size_t(asset_.lods);
	draws_.assign(n, MultiMeshDraw());
	bufs_.assign(n, PackedFloat32Array());
	return true;
}

void PeopleRenderer::reset() {
	for (MultiMeshDraw &d : draws_) d.free();
}

int PeopleRenderer::update(const PoseFrame &f, const Vector3 &cam, float alpha, float radius) {
	if (draws_.empty()) return 0;
	for (PackedFloat32Array &b : bufs_) b.resize(0);
	const float r2 = radius * radius;
	const int nv = int(asset_.variations.size()), lods = asset_.lods;
	int drawn = 0;
	for (const AgentRecord &rec : f.people) {
		Pose prev, cur;
		prev.x = rec.px; prev.y = rec.py; prev.z = rec.pz; prev.fx = rec.pfx; prev.fz = rec.pfz;
		cur.x = rec.x; cur.y = rec.y; cur.z = rec.z; cur.fx = rec.fx; cur.fz = rec.fz;
		Pose p = lerp_pose(prev, cur, alpha);
		float dx = p.x - cam.x, dy = p.y - cam.y, dz = p.z - cam.z, d2 = dx * dx + dz * dz;
		if (d2 > r2) continue;
		float dist = std::sqrt(d2 + dy * dy);
		int lod = 0;
		while (lod < lods - 1 && dist > lod_m_[size_t(lod)]) lod++;
		int v = int((rec.look >> VARIATION_SHIFT) % uint32_t(nv));
		const PersonVariation &pv = asset_.variations[size_t(v)];
		uint32_t h = rec.look;
		float top = (h & TWO_BITS) ? float((h >> TOP_SHIFT) % CLOTHES + 1) : 0.f;
		float bottom = ((h >> BOTTOM_TEST) & TWO_BITS) ? float((h >> BOTTOM_SHIFT) % CLOTHES + 1) : 0.f;
		bool grey = pv.senior && ((h >> GREY_TEST) & TWO_BITS);
		float hair = grey ? float(HAIRS + (h >> GREY_SHIFT) % GREYS) : float((h >> HAIR_SHIFT) % HAIRS);
		float skin = float((h >> SKIN_SHIFT) % SKINS);
		float phase = rec.walked / pv.walk_advance;
		push_agent(bufs_[size_t(v * lods + lod)], p, 1.f, float(pv.palette_row), float(pv.frames) + FRAMES_BASE * top, bottom + HAIR_BASE * hair + SKIN_BASE * skin,
				phase - std::floor(phase));
		drawn++;
	}
	AABB box(Vector3(cam.x - radius, cam.y - AABB_HALF_H, cam.z - radius), Vector3(radius * 2, AABB_HALF_H * 2, radius * 2));
	for (int v = 0; v < nv; v++)
		for (int l = 0; l < lods; l++) {
			size_t k = size_t(v * lods + l);
			draws_[k].set(scenario_, asset_.variations[size_t(v)].lods[size_t(l)]->get_rid(), material_, bufs_[k], box, true);
		}
	return drawn;
}

} // namespace city
