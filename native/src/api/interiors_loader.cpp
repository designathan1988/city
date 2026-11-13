// Parses data/render/interiors.json into the interior tables (sim/buildings/interior/tables.h). Every name is checked: an
// unknown piece, edge, purpose or step is an error at startup, never a silent default.
#include "api/interiors_loader.h"
#include "render/region_table.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cmath>
#include <string>

using namespace godot;

namespace city {

using namespace b40;

namespace {
const auto &KINDS = FURNITURE_NAMES;
const char *const PURPOSES[PU_COUNT] = { "house", "flats", "hotel", "office", "cityHall", "police", "courthouse", "postOffice", "council",
	"hospital", "school", "library", "museum", "prison", "church", "busStation", "shop", "supermarket", "mall", "bank", "restaurant", "bar",
	"nightclub", "cinema", "gym", "club", "fireStation", "factory", "warehouse" };
const char *const EDGES[EDGE_COUNT] = { "x0", "x1", "y0", "y1", "cx", "cy", "ys" };
const char *const OPS[STEP_COUNT] = { "put", "grid", "back", "left", "right", "wallAcross", "lobby" };
const char *const LEVELS[] = { "any", "ground", "upper" };
const char *const ANGLES[] = { "front", "right", "back", "left" };
constexpr float RADIANS_PER_STEP = 1.57079633f;
constexpr int SIZE_AXES = 3;

std::string str(const Variant &v) { return std::string(String(v).utf8().get_data()); }

template <size_t N>
int index_of(const char *const (&names)[N], const std::string &s, const char *what, PackedStringArray &errors) {
	for (size_t i = 0; i < N; i++)
		if (s == names[i]) return int(i);
	errors.push_back(String("interiors.json: unknown ") + what + ": " + s.c_str());
	return 0;
}

Ref ref_of(const Dictionary &d, const char *key, PackedStringArray &errors) {
	Ref r;
	if (!d.has(key)) return r;
	Array a = d[key];
	r.edge = Edge(index_of(EDGES, str(a[0]), "edge", errors));
	r.off = float(a[1]);
	return r;
}

Step step_of(const Dictionary &d, PackedStringArray &errors) {
	Step s;
	s.op = StepOp(index_of(OPS, str(d["op"]), "step", errors));
	if (d.has("kind")) s.kind = FurnitureKind(index_of(KINDS, str(d["kind"]), "piece", errors));
	if (d.has("level")) s.level = StepLevel(index_of(LEVELS, str(d["level"]), "level", errors));
	s.a = ref_of(d, d.has("x") ? "x" : "x0", errors);
	s.b = ref_of(d, d.has("y") ? "y" : "y0", errors);
	s.c = ref_of(d, "x1", errors);
	s.d = ref_of(d, "y1", errors);
	if (s.op == STEP_SIDE_LEFT || s.op == STEP_SIDE_RIGHT) s.b = s.c;
	s.gx = float(d.get("gx", 0));
	s.gy = float(d.get("gy", 0));
	s.wide = bool(d.get("wide", false));
	if (d.has("angle")) {
		const int turn = index_of(ANGLES, str(d["angle"]), "angle", errors);
		s.angle = float(turn > 2 ? turn - 4 : turn) * RADIANS_PER_STEP;
	}
	return s;
}
} // namespace

bool load_interiors(const Dictionary &table, InteriorTables &out, PackedStringArray &errors) {
	const Dictionary t = region_table(table);
	const int64_t before = errors.size();
	Array sizes = t.get("sizes", Array());
	bool sized[FK_COUNT] = {};
	for (int64_t r = 0; r < sizes.size(); r++) {
		Array row = sizes[r];
		const int k = index_of(KINDS, str(row[0]), "piece", errors);
		sized[k] = true;
		for (int i = 0; i < SIZE_AXES; i++) out.size[k][i] = float(row[i + 1]);
	}
	for (int k = 0; k < FK_COUNT; k++)
		if (!sized[k]) errors.push_back(String("interiors.json: missing the size of ") + KINDS[k]);
	Array seen = t.get("seen", Array());
	for (int64_t i = 0; i < seen.size(); i++) out.seen[index_of(KINDS, str(seen[i]), "piece", errors)] = true;
	Dictionary recipes = t.get("recipes", Dictionary());
	Array keys = recipes.keys();
	for (int64_t i = 0; i < keys.size(); i++) {
		const int p = index_of(PURPOSES, str(keys[i]), "purpose", errors);
		Array steps = recipes[keys[i]];
		for (int64_t k = 0; k < steps.size(); k++) out.recipes[p].push_back(step_of(steps[k], errors));
	}
	return errors.size() == before;
}

} // namespace city
