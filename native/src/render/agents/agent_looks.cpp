// What an agent wears and who rides in it: the palettes and looks pushed to the "agent" material, and how full each type runs
// (agents.json). Once, at boot; the frame's work is in agent_renderer.cpp.
#include "render/agents/agent_renderer.h"

#include "render/agents/agent_meshes.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>

using namespace godot;

namespace city {

namespace {

// agents.gdshader palette array sizes
constexpr int HAIR_MAX = 8, SKIN_MAX = 8, PAINT_MAX = 16, LINE_PAINTS = 8, CUTS = 3, OUTFITS_MAX = 8, OUTFIT_COLOURS = 8;
constexpr int HALF_EXACT = 2048; // whole numbers a half float holds exactly: the order (stride * seats + first) below it
constexpr int MIN_STRIDE = 2;    // a shuffled order skips at least one seat (stride 1 fills them front to back)

PackedColorArray palette(const Variant &list, int max, const char *name, PackedStringArray &errors) {
	PackedColorArray out;
	for (const Variant &c : Array(list)) out.push_back(agent_hex(c));
	if (out.is_empty() || out.size() > max) errors.push_back(String("agents.json: paleta ") + name + " vazia ou longa demais");
	while (out.size() < max) out.push_back(out.is_empty() ? Color() : out[0]);
	return out;
}

// The cumulative shares of `weights`, `count` of them, padded to `max` with 1 (what the shader's draws walk).
PackedFloat32Array cumulative(const Variant &weights, int count, int max, const char *name, PackedStringArray &errors) {
	PackedFloat32Array out;
	float sum = 0;
	for (const Variant &w : Array(weights)) out.push_back(sum += float(w));
	if (out.size() != count || count > max || sum <= 0) errors.push_back(String("agents.json: occupants.") + name + " sem os pesos certos");
	for (int64_t i = 0; i < out.size(); i++) out.set(i, sum > 0 ? out[i] / sum : 1.f);
	while (out.size() < max) out.push_back(1.f);
	return out;
}

// agents.json occupants.load key of a vehicle type (the motorbike carries its rider alone).
const char *load_key(int type) {
	if (type < CAR_MODELS || type == WAGON_MODEL) return "car";
	if (type == int(VehicleType::BUS)) return "bus";
	return type == int(VehicleType::TRUCK) ? "truck" : nullptr;
}

} // namespace

bool AgentRenderer::push_palettes(const Dictionary &t, const std::vector<uint32_t> &lines, PackedStringArray &errors) {
	const Dictionary people = t["people"], occupants = t["occupants"];
	RenderingServer *rs = RenderingServer::get_singleton();
	const Dictionary styles = t["styles"], style = styles[REGIONAL_STYLE];
	const Array hair = people["hair"], skin = occupants["skin"], paints = style["paints"];
	rs->material_set_param(material_, "hair", palette(hair, HAIR_MAX, "hair", errors));
	rs->material_set_param(material_, "skin", palette(skin, SKIN_MAX, "skin", errors));
	// as numbers: a Color array would be turned linear on the way (the parts' vertex colours are not)
	PackedVector3Array raw;
	for (const Color &c : palette(paints, PAINT_MAX, "paints", errors)) raw.push_back(Vector3(c.r, c.g, c.b));
	for (size_t i = 0; i < lines.size() && i < size_t(LINE_PAINTS); i++) { // after them: a bus's line (traffic_state.h LINE_PAINT)
		const Color c = rgb8_to_color(lines[i]);
		raw.push_back(Vector3(c.r, c.g, c.b));
	}
	rs->material_set_param(material_, "paints", raw);
	rs->material_set_param(material_, "paint_n", int(paints.size())); // the riders' helmets come in the paints too
	const Dictionary roof = Dictionary(t["car_body"])["black_roof"];
	rs->material_set_param(material_, "black_roof", agent_hex(roof["colour"]));
	rs->material_set_param(material_, "black_roof_share", float(roof["share"]));
	rs->material_set_param(material_, "skin_cdf", cumulative(occupants["skin_weights"], int(skin.size()), SKIN_MAX, "skin_weights", errors));
	push_looks(occupants, people["clothes"], int(hair.size()), errors);
	paints_ = int(paints.size());
	return errors.is_empty();
}

void AgentRenderer::push_looks(const Dictionary &o, const Array &clothes, int hair_n, PackedStringArray &errors) {
	RenderingServer *rs = RenderingServer::get_singleton();
	const Dictionary hw = o["hair_weights"];
	PackedFloat32Array hair = cumulative(hw["light_skinned"], hair_n, HAIR_MAX, "hair_weights.light_skinned", errors);
	hair.append_array(cumulative(hw["dark_skinned"], hair_n, HAIR_MAX, "hair_weights.dark_skinned", errors));
	rs->material_set_param(material_, "hair_cdf", hair);
	rs->material_set_param(material_, "light_skins", int(o["light_skins"]));
	rs->material_set_param(material_, "cut_cdf", cumulative(o["cuts"], CUTS, CUTS, "cuts", errors));
	// one outfit: the shirt and the trousers each drawn from the pedestrians' clothes (agents.json people.clothes)
	if (clothes.is_empty() || clothes.size() > OUTFITS_MAX * OUTFIT_COLOURS) errors.push_back("agents.json: people.clothes com cores demais ou nenhuma");
	PackedColorArray shirts;
	for (const Variant &c : clothes) shirts.push_back(agent_hex(c));
	while (shirts.size() < OUTFITS_MAX * OUTFIT_COLOURS) shirts.push_back(Color());
	const PackedColorArray &trousers = shirts;
	PackedInt32Array from, count;
	for (int i = 0; i < OUTFITS_MAX; i++) {
		from.push_back(0);
		count.push_back(std::max(int(clothes.size()), 1));
	}
	Array one;
	one.push_back(1);
	rs->material_set_param(material_, "outfit_cdf", cumulative(one, 1, OUTFITS_MAX, "outfits", errors));
	rs->material_set_param(material_, "shirts", shirts);
	rs->material_set_param(material_, "trousers", trousers);
	rs->material_set_param(material_, "shirt_from", from);
	rs->material_set_param(material_, "shirt_n", count);
	rs->material_set_param(material_, "trousers_from", from);
	rs->material_set_param(material_, "trousers_n", count);
}

bool AgentRenderer::read_loads(const Dictionary &loads, PackedStringArray &errors) {
	for (int i = 0; i < AGENT_MODELS; i++) {
		Load &l = load_[i];
		const char *key = load_key(i);
		if (!key || l.seats == 0) continue;
		const Dictionary d = loads[key];
		l.shuffle = bool(d["shuffle"]);
		float sum = 0;
		for (const Variant &w : Array(d["bands"])) l.bands.push_back(sum += float(w));
		for (float &b : l.bands) b /= sum;
		if (l.bands.empty() || sum <= 0) errors.push_back(String("agents.json: occupants.load.") + key + " sem faixas");
		const int widest = (HALF_EXACT - l.seats) / l.seats; // the largest stride whose order a half float still holds
		if (l.shuffle && widest < MIN_STRIDE) errors.push_back(String("agents.json: ") + key + " tem lugares demais para a ordem em meia precisão");
		for (int s = MIN_STRIDE; l.shuffle && s <= std::min(l.seats - MIN_STRIDE, widest); s++)
			if (std::gcd(s, l.seats) == 1) l.strides.push_back(s);
		if (l.strides.empty()) l.strides.push_back(1);
	}
	return errors.is_empty();
}

} // namespace city
