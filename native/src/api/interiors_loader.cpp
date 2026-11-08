// Parses data/render/interiors.json into the interior tables (sim/buildings/interior/tables.h) through the strict reader
// (api/data_reader.h): every key, type, length and name is checked (an unknown piece, edge, purpose or step is an error at
// startup, never a silent default); the keys a step may leave out take the value the format gives them.
#include "api/interiors_loader.h"

#include "render/data_reader.h"
#include "render/region_table.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

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
const char *const DOC_KEY = "_doc";
constexpr float RADIANS_PER_STEP = 1.57079633f;
enum { SIZE_NAME, SIZE_X, SIZE_Y, SIZE_Z, SIZE_ROW };
enum { REF_EDGE, REF_OFF, REF_SIZE };

// [edge, offset], or none when the step leaves the key out.
Ref ref_of(DataReader &r, const Dictionary &d, const char *key) {
	Ref out;
	if (!d.has(key)) return out;
	const Array a = r.list(d, key, REF_SIZE);
	DataReader ra = r.in(key);
	out.edge = Edge(ra.index_of(EDGES, ra.utf8_at(a, REF_EDGE), "edge"));
	out.off = float(ra.num_at(a, REF_OFF));
	return out;
}

Step step_of(DataReader r, const Dictionary &d) {
	Step s;
	s.op = StepOp(r.index_of(OPS, r.utf8(d, "op"), "step"));
	if (d.has("kind")) s.kind = FurnitureKind(r.index_of(KINDS, r.utf8(d, "kind"), "piece"));
	if (d.has("level")) s.level = StepLevel(r.index_of(LEVELS, r.utf8(d, "level"), "level"));
	s.a = ref_of(r, d, d.has("x") ? "x" : "x0");
	s.b = ref_of(r, d, d.has("y") ? "y" : "y0");
	s.c = ref_of(r, d, "x1");
	s.d = ref_of(r, d, "y1");
	if (s.op == STEP_SIDE_LEFT || s.op == STEP_SIDE_RIGHT) s.b = s.c;
	s.gx = float(r.num_or(d, "gx", 0.0));
	s.gy = float(r.num_or(d, "gy", 0.0));
	s.wide = r.flag_or(d, "wide", false);
	if (d.has("angle")) {
		const int turn = r.index_of(ANGLES, r.utf8(d, "angle"), "angle");
		s.angle = float(turn > 2 ? turn - 4 : turn) * RADIANS_PER_STEP;
	}
	return s;
}
} // namespace

bool load_interiors(const Dictionary &table, InteriorTables &out, PackedStringArray &errors) {
	const Dictionary t = region_table(table);
	const int64_t before = errors.size();
	DataReader r{ errors, "interiors.json" };
	const Array sizes = r.list(t, "sizes");
	bool sized[FK_COUNT] = {};
	for (int i = 0; i < sizes.size(); i++) {
		const Array row = r.in("sizes").list_at(sizes, i, SIZE_ROW);
		DataReader rs = r.in("sizes").in(i);
		const int k = rs.index_of(KINDS, rs.utf8_at(row, SIZE_NAME), "piece");
		sized[k] = true;
		out.size[k][0] = float(rs.num_at(row, SIZE_X));
		out.size[k][1] = float(rs.num_at(row, SIZE_Y));
		out.size[k][2] = float(rs.num_at(row, SIZE_Z));
	}
	for (int k = 0; k < FK_COUNT; k++)
		if (!sized[k]) r.in("sizes").fail(String("no size for ") + KINDS[k]);
	const Array seen = r.list(t, "seen");
	for (int i = 0; i < seen.size(); i++) {
		DataReader rs = r.in("seen");
		out.seen[rs.index_of(KINDS, rs.utf8_at(seen, i), "piece")] = true;
	}
	const Dictionary recipes = r.table(t, "recipes");
	const Array keys = recipes.keys();
	for (int i = 0; i < keys.size(); i++) {
		const String key = keys[i];
		if (key == DOC_KEY) continue;
		DataReader rp = r.in("recipes").in(key);
		const int p = rp.index_of(PURPOSES, key.utf8().get_data(), "purpose");
		const Array steps = r.in("recipes").list(recipes, key.utf8().get_data());
		for (int k = 0; k < steps.size(); k++) out.recipes[p].push_back(step_of(rp.in(k), rp.table_at(steps, k)));
	}
	return errors.size() == before;
}

} // namespace city
