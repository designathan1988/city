// The paints pushed to the "agent" material and how full each type runs
// (agents.json). Once, at boot; the frame's work is in agent_renderer.cpp.
#include "render/agents/agent_renderer.h"

#include "render/agents/agent_meshes.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>

using namespace godot;

namespace city {

namespace {

// agents.gdshader palette array sizes
constexpr int PAINT_MAX = 16, LINE_PAINTS = 8;
constexpr int HALF_EXACT = 2048; // whole numbers a half float holds exactly: the order (stride * seats + first) below it
constexpr int MIN_STRIDE = 2;    // a shuffled order skips at least one seat (stride 1 fills them front to back)

PackedColorArray palette(const DataValue &list, int max) {
	PackedColorArray out;
	for (int i = 0; i < list.size(); i++) out.push_back(agent_hex(list[i]));
	if (out.is_empty() || out.size() > max) list.fail(String("between 1 and ") + String::num_int64(max) + String(" colours"));
	while (out.size() < max) out.push_back(out.is_empty() ? Color() : out[0]);
	return out;
}

// agents.json occupants.load key of a vehicle type (the motorbike carries its rider alone).
const char *load_key(int type) {
	if (type < CAR_MODELS || type == WAGON_MODEL) return "car";
	if (type == int(VehicleType::BUS)) return "bus";
	return type == int(VehicleType::TRUCK) ? "truck" : nullptr;
}

} // namespace

bool AgentRenderer::push_palettes(const DataValue &t, const std::vector<uint32_t> &lines, PackedStringArray &errors) {
	const int64_t before = errors.size();
	RenderingServer *rs = RenderingServer::get_singleton();
	const DataValue paints = t["styles"][REGIONAL_STYLE]["paints"];
	// as numbers: a Color array would be turned linear on the way (the parts' vertex colours are not)
	PackedVector3Array raw;
	for (const Color &c : palette(paints, PAINT_MAX)) raw.push_back(Vector3(c.r, c.g, c.b));
	for (size_t i = 0; i < lines.size() && i < size_t(LINE_PAINTS); i++) { // after them: a bus's line (traffic_state.h LINE_PAINT)
		const Color c = rgb8_to_color(lines[i]);
		raw.push_back(Vector3(c.r, c.g, c.b));
	}
	rs->material_set_param(material_, "paints", raw);
	const DataValue roof = t["car_body"]["black_roof"];
	rs->material_set_param(material_, "black_roof", agent_hex(roof["colour"]));
	rs->material_set_param(material_, "black_roof_share", roof["share"].f());
	paints_ = paints.size();
	return errors.size() == before;
}

bool AgentRenderer::read_loads(const DataValue &loads, PackedStringArray &errors) {
	const int64_t before = errors.size();
	for (int i = 0; i < AGENT_MODELS; i++) {
		Load &l = load_[i];
		const char *key = load_key(i);
		if (!key || l.seats == 0) continue;
		const DataValue d = loads[key];
		l.shuffle = d["shuffle"].flag();
		float sum = 0;
		const DataValue bands = d["bands"];
		l.bands.clear(); // a second read is the table again
		l.strides.clear();
		for (int b = 0; b < bands.size(); b++) l.bands.push_back(sum += bands[b].f());
		for (float &b : l.bands) b /= sum;
		if (l.bands.empty() || sum <= 0) d.fail("no bands");
		const int widest = (HALF_EXACT - l.seats) / l.seats; // the largest stride whose order a half float still holds
		if (l.shuffle && widest < MIN_STRIDE) d.fail("too many seats for the order a half float holds");
		for (int s = MIN_STRIDE; l.shuffle && s <= std::min(l.seats - MIN_STRIDE, widest); s++)
			if (std::gcd(s, l.seats) == 1) l.strides.push_back(s);
		if (l.strides.empty()) l.strides.push_back(1);
	}
	return errors.size() == before;
}

} // namespace city
