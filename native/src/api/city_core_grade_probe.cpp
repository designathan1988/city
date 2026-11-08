// grade_probe: the road heights solver run on a scene of its own (project/tests/core/road_grade_test.gd), the cases of the
// Roadcraft 4.0's tests/world/elevation.spec.ts: a scratch network with the game's road classes over a ground given by
// name (flat, a hill, a valley, rolling, broken, sunk landings), its stations and node heights handed back. The open city
// is not touched.
#include "api/city_probes.h"

#include "render/data_reader.h"

#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
// The 4.0 test's grounds (elevation.spec.ts), in metres.
constexpr float ROLL_A = 3.f * 0.4f, ROLL_B = 2.f * 0.4f, ROLL_X = 90.f * 0.4f, ROLL_XZ = 37.f * 0.4f; // rolling
constexpr float HILLY_A = 9.f * 0.4f, HILLY_B = 6.f * 0.4f, HILLY_C = 4.f * 0.4f; // hilly: three waves
constexpr float HILLY_X = 140.f * 0.4f, HILLY_XZ = 70.f * 0.4f, HILLY_Z = 55.f * 0.4f;

enum { SEG_A, SEG_B, SEG_PROFILE, SEG_STRUCTURE, SEG_ROW }; // a segment of the scene: [a, b, profile id, structure]
constexpr int NODE_ROW = 2; // a node: [x, z], and a lift after it when it has one

float arg(const Dictionary &d, const char *key, float fallback) { return d.has(key) ? float(d[key]) : fallback; }

GroundAt ground_named(const Dictionary &g) {
	const String kind = g.get("kind", "flat");
	const float cx = arg(g, "cx", 0), cz = arg(g, "cz", 0), radius = arg(g, "radius", 1), height = arg(g, "height", 0);
	if (kind == "hill") // a smoothstep dome (hill())
		return [=](float x, float z) {
			const float d = std::hypot(x - cx, z - cz);
			if (d >= radius) return 0.f;
			const float u = 1.f - d / radius;
			return height * u * u * (3.f - 2.f * u);
		};
	if (kind == "valley") // a V across x, `radius` either side, `height` deep
		return [=](float x, float) { return std::fabs(x) < radius ? -height * (1.f - std::fabs(x) / radius) : 0.f; };
	if (kind == "rolling") return [](float x, float z) { return ROLL_A * std::sin(x / ROLL_X) + ROLL_B * std::sin((x + z) / ROLL_XZ); };
	if (kind == "hilly")
		return [](float x, float z) { return HILLY_A * std::sin(x / HILLY_X) + HILLY_B * std::sin((x - z) / HILLY_XZ) + HILLY_C * std::cos(z / HILLY_Z); };
	if (kind == "sunk") // landings sunk `height` under the plain around |x| = cx, a bell `radius` wide
		return [=](float x, float) { return -height * std::exp(-std::pow((std::fabs(x) - cx) / radius, 2.f)); };
	return [](float, float) { return 0.f; };
}
} // namespace

// scene: {ground: {kind, ...}, nodes: [[x, z, lift?], ...], segs: [[a, b, profile id, structure], ...]}.
// Gives {nodes: [height...], segs: [{structure, len, step, plate_a, plate_b, h, ceil}], ground: [[x, z, y]...] at each station}.
Dictionary CityProbes::grade_probe(const Dictionary &scene) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const Catalog &cat = state_->catalog;
	if (cat.profiles.empty()) return Dictionary(); // a refused catalog has no road to lay the scene with
	RoadNetwork net;
	net.init(&cat, nullptr);
	// The scene through the strict reader (render/data_reader.h): a malformed one is pushed as errors and refused, never read past
	// a list's end or with a node or a profile that is not there.
	PackedStringArray errors;
	DataReader in{ errors, "grade_probe scene" };
	const Array nodes = in.list_or(scene, "nodes"), segs = in.list_or(scene, "segs");
	for (int i = 0; i < nodes.size(); i++) {
		DataReader rn = in.in("nodes").in(i);
		const Array n = in.in("nodes").list_at(nodes, i);
		if (n.size() != NODE_ROW && n.size() != NODE_ROW + 1) rn.fail("expected [x, z] or [x, z, lift]");
		const int id = net.add_node(rn.num_at(n, 0), rn.num_at(n, 1), 0);
		if (n.size() > NODE_ROW) net.nodes[size_t(id)].lift = float(rn.num_at(n, NODE_ROW));
	}
	for (int i = 0; i < segs.size(); i++) {
		DataReader rs = in.in("segs").in(i);
		const Array g = in.in("segs").list_at(segs, i, SEG_ROW);
		const int a = rs.integer_at(g, SEG_A), b = rs.integer_at(g, SEG_B);
		const int profile = cat.profile_id(rs.utf8_at(g, SEG_PROFILE));
		if (profile < 0) rs.fail(String("unknown profile '") + rs.text_at(g, SEG_PROFILE) + String("'"));
		const int count = int(net.nodes.size());
		if (a < 0 || a >= count || b < 0 || b >= count) rs.fail("a node outside the scene's");
		if (!errors.is_empty()) break;
		const RoadNode &na = net.nodes[size_t(a)], &nb = net.nodes[size_t(b)];
		const int s = net.add_segment(a, b, Quad::straight(na.x, na.z, nb.x, nb.z), profile, 0, 0, 0);
		net.set_structure(s, Structure(rs.integer_at(g, SEG_STRUCTURE)));
	}
	if (!errors.is_empty()) {
		for (const String &e : errors) UtilityFunctions::push_error(e);
		return Dictionary();
	}
	net.refresh_all();
	const GroundAt ground = ground_named(scene.get("ground", Dictionary()));
	std::vector<int> moved, gone;
	net.grade.everything();
	net.grade.solve(net, ground, moved, gone);
	Dictionary out;
	Array heights, list;
	for (const RoadNode &n : net.nodes) heights.push_back(n.y);
	for (int s = 0; s < int(net.segs.size()); s++) {
		const GradeProfile &p = net.grade.prof[size_t(s)];
		Dictionary d;
		PackedFloat32Array h, ceil, under;
		for (size_t i = 0; i < p.h.size(); i++) {
			h.push_back(p.h[i]);
			ceil.push_back(p.ceil[i]);
			double x, z;
			net.segs[size_t(s)].q.at(t_at_length(net.segs[size_t(s)].table, double(p.step) * double(i)), x, z);
			under.push_back(ground(float(x), float(z)));
		}
		d["structure"] = int(p.structure);
		d["len"] = p.len;
		d["step"] = p.step;
		d["plate_a"] = p.plate_a;
		d["plate_b"] = p.plate_b;
		d["h"] = h;
		d["ceil"] = ceil;
		d["ground"] = under;
		list.push_back(d);
	}
	out["nodes"] = heights;
	out["segs"] = list;
	return out;
}

} // namespace city
