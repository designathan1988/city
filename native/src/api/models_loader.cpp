// Parses data/render/models.json through the strict reader (api/data_reader.h). Rows of parts and fences all round a rectangle
// are laid out here, once, into plain elements; a missing key, a wrong type or length and an unknown name are errors at startup,
// never a silent default (the keys the format leaves out take the value it gives them).
#include "api/models_loader.h"
#include "sim/catalog/catalog.h"

#include "api/function_names.h"

#include "api/buildings_loader.h"
#include "render/region_table.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cmath>
#include <string>
#include <unordered_map>
#include <utility>

using namespace godot;

namespace city {

using namespace b40;

namespace {
constexpr float RING_TOLERANCE_M = 0.01f;
enum { SPEC_NAME, SPEC_W, SPEC_D, SPEC_H, SPEC_SIZE };     // part_sizes rows
enum { RECT_X, RECT_Y, RECT_WIDTH, RECT_DEPTH, RECT_SIZE }; // "at" of a block, a ring, an open ground
constexpr int POINT = 2;                                    // [x, y]
constexpr double NO_PITCH = -1.0;                           // the roof's own pitch
constexpr const char *MIDDLE = "middle";                    // door_bay: the middle bay of the front

constexpr Named ROOF_NAMES[] = { { "flat", ROOF_FLAT }, { "terrace", ROOF_TERRACE }, { "gable", ROOF_GABLE }, { "hip", ROOF_HIP }, { "shed", ROOF_SHED },
	{ "sawtooth", ROOF_SAWTOOTH } };
constexpr Named PATTERN_NAMES[] = { { "office", PATTERN_OFFICE }, { "gallery", PATTERN_GALLERY }, { "artDeco", PATTERN_ART_DECO }, { "artDecoCrown", PATTERN_ART_DECO_CROWN } };
constexpr Named ELEMENT_NAMES[] = { { "tree", EL_TREE }, { "bench", EL_BENCH }, { "planter", EL_PLANTER }, { "flowers", EL_FLOWERS }, { "rocks", EL_ROCKS }, { "fence", EL_FENCE },
	{ "wall", EL_WALL }, { "pavement", EL_PAVEMENT }, { "slab", EL_SLAB }, { "pillar", EL_PILLAR }, { "canopy", EL_CANOPY }, { "parking", EL_PARKING },
	{ "railing", EL_RAILING }, { "clock", EL_CLOCK }, { "awning", EL_AWNING } };
constexpr Named USE_NAMES[] = { { "residential", BASE_RESIDENTIAL }, { "commercial", BASE_COMMERCIAL }, { "industrial", BASE_INDUSTRIAL }, { "mixed", BASE_MIXED } };

struct Sizes {
	float x = 0, y = 0, z = 0;
};

// What a table lays parts out with.
struct Layout {
	std::unordered_map<int, Sizes> sizes;
	float ring_piece = 0;
	ModelSpec *model = nullptr;
};

// A part at (x, y): `facing` 1 or 3 turns it a quarter, so its width runs along the lot's y.
LotElement part(const Layout &l, int kind, float x, float y, int facing, DataReader &r) {
	LotElement e;
	const auto size = l.sizes.find(kind);
	if (size == l.sizes.end()) r.fail("a part with no part_sizes row");
	const Sizes s = size == l.sizes.end() ? Sizes{} : size->second;
	e.kind = Element(kind);
	e.x = x;
	e.y = y;
	e.w = s.x;
	e.d = s.y;
	e.h = s.z;
	e.facing = facing;
	return e;
}

void put(LotElement e, ModelSpec &m) {
	if (e.facing % 2 == 1) std::swap(e.w, e.d);
	m.elements.push_back(e);
}

int element_of(DataReader &r, const Dictionary &d) { return r.value_of(ELEMENT_NAMES, r.utf8(d, "kind"), "part"); }

void parts(const Layout &l, DataReader r, const Array &list) {
	for (int i = 0; i < list.size(); i++) {
		DataReader rp = r.in(i);
		const Dictionary p = r.table_at(list, i);
		const Array where = rp.list(p, "at", POINT);
		LotElement e = part(l, element_of(rp, p), float(rp.in("at").num_at(where, 0)), float(rp.in("at").num_at(where, 1)),
				rp.integer_or(p, "facing", FACE_STREET), rp);
		const Dictionary size = rp.table_or(p, "size"); // each side the part's own unless given
		DataReader rs = rp.in("size");
		e.w = float(rs.num_or(size, "w", e.w));
		e.d = float(rs.num_or(size, "d", e.d));
		e.h = float(rs.num_or(size, "h", e.h));
		e.z = float(rp.num_or(p, "rise_m", 0.0));
		put(e, *l.model);
	}
}

void rows(const Layout &l, DataReader r, const Array &list) {
	for (int i = 0; i < list.size(); i++) {
		DataReader rr = r.in(i);
		const Dictionary row = r.table_at(list, i);
		const Array from = rr.list(row, "from", POINT), to = rr.list(row, "to", POINT);
		const float x0 = float(rr.in("from").num_at(from, 0)), y0 = float(rr.in("from").num_at(from, 1));
		const float x1 = float(rr.in("to").num_at(to, 0)), y1 = float(rr.in("to").num_at(to, 1));
		const int n = rr.integer(row, "count"), kind = element_of(rr, row), facing = rr.integer_or(row, "facing", FACE_STREET);
		for (int k = 0; k < n; k++) {
			const float share = n == 1 ? 0.5f : float(k) / float(n - 1);
			put(part(l, kind, x0 + (x1 - x0) * share, y0 + (y1 - y0) * share, facing, rr), *l.model);
		}
	}
}

struct Rect {
	float x = 0, y = 0, w = 0, d = 0;
};

Rect rect_of(DataReader &r, const Dictionary &d) {
	const Array a = r.list(d, "at", RECT_SIZE);
	DataReader ra = r.in("at");
	return Rect{ float(ra.num_at(a, RECT_X)), float(ra.num_at(a, RECT_Y)), float(ra.num_at(a, RECT_WIDTH)), float(ra.num_at(a, RECT_DEPTH)) };
}

// A fence or wall all round a rectangle, in pieces; the gate is a gap in the middle of the front.
void rings(const Layout &l, DataReader r, const Array &list) {
	for (int i = 0; i < list.size(); i++) {
		DataReader rr = r.in(i);
		const Dictionary ring = r.table_at(list, i);
		const Rect at = rect_of(rr, ring);
		const int kind = element_of(rr, ring);
		const float gate = float(rr.num(ring, "opening_m")), piece = l.ring_piece;
		auto lay = [&](float px, float py, int facing) {
			LotElement e = part(l, kind, px, py, facing, rr);
			e.w = piece;
			put(e, *l.model);
		};
		for (float a = 0; a + piece <= at.w + RING_TOLERANCE_M; a += piece) {
			if (std::fabs(a + piece / 2 - at.w / 2) > gate / 2) lay(at.x + a + piece / 2, at.y, FACE_STREET);
			lay(at.x + a + piece / 2, at.y + at.d, FACE_BACK);
		}
		for (float a = 0; a + piece <= at.d + RING_TOLERANCE_M; a += piece) {
			lay(at.x, at.y + a + piece / 2, FACE_LEFT);
			lay(at.x + at.w, at.y + a + piece / 2, FACE_RIGHT);
		}
	}
}

void open_ground(const Dictionary &surfaces, float height, DataReader r, const Array &list, ModelSpec &m) {
	for (int i = 0; i < list.size(); i++) {
		DataReader ro = r.in(i);
		const Dictionary o = r.table_at(list, i);
		const Rect at = rect_of(ro, o);
		LotElement e;
		e.kind = EL_SURFACE;
		e.x = at.x + at.w / 2;
		e.y = at.y + at.d / 2;
		e.w = at.w;
		e.d = at.d;
		e.h = height;
		e.paint = int(m.paints.size());
		const String surface = ro.text(o, "surface");
		if (!surfaces.has(surface)) ro.fail(String("unknown surface '") + surface + String("'"));
		m.paints.push_back(paint_named(surfaces.get(surface, Array()), ro.in("surface")));
		m.elements.push_back(e);
	}
}

void dress_of(DataReader r, const Dictionary &d, ModelBlock &b, int every, int start) {
	ModelDress out;
	out.side = r.integer(d, "side");
	out.from = r.integer(d, "from");
	out.to = r.integer(d, "to");
	out.every = every;
	out.start = start;
	out.fill = bay_named(r.utf8(d, "fill"), r);
	b.dress.push_back(out);
}

void door_of(DataReader r, const Dictionary &s, ModelBlock &b) { // a bay's index, or "middle"
	const Variant door = s["door_bay"];
	if (door.get_type() == Variant::STRING) {
		if (String(door) != MIDDLE) r.fail(String("expected a bay or '") + MIDDLE + String("', found '") + String(door) + String("'"));
		b.door_bay = MIDDLE_BAY;
	} else {
		b.door_bay = r.integer(s, "door_bay");
	}
}

ModelBlock block_of(DataReader r, const Dictionary &s) {
	ModelBlock b;
	const Rect at = rect_of(r, s);
	b.x = at.x;
	b.y = at.y;
	b.w = at.w;
	b.d = at.d;
	b.base = r.integer_or(s, "base", 0);
	b.storeys = r.integer(s, "storeys");
	b.pitch = float(r.num_or(s, "pitch_deg", NO_PITCH));
	if (s.has("roof")) b.roof = Roof(r.value_of(ROOF_NAMES, r.utf8(s, "roof"), "roof"));
	b.fill = bay_named(r.utf8(s, "fill"), r);
	if (s.has("ground_fill")) b.ground = bay_named(r.utf8(s, "ground_fill"), r);
	if (s.has("door_kind")) b.door_kind = bay_named(r.utf8(s, "door_kind"), r);
	if (s.has("door_bay")) door_of(r.in("door_bay"), s, b);
	if (s.has("pattern")) b.pattern = r.value_of(PATTERN_NAMES, r.utf8(s, "pattern"), "pattern");
	const Array outline = r.list_or(s, "outline");
	for (int i = 0; i < outline.size(); i++) {
		const Array p = r.in("outline").list_at(outline, i, POINT);
		DataReader rp = r.in("outline").in(i);
		b.outline.push_back(V2{ float(rp.num_at(p, 0)), float(rp.num_at(p, 1)) });
	}
	const Array sides = r.list_or(s, "sides"), bays = r.list_or(s, "bays");
	for (int i = 0; i < sides.size(); i++) dress_of(r.in("sides").in(i), r.in("sides").table_at(sides, i), b, 0, 0);
	for (int i = 0; i < bays.size(); i++) {
		DataReader rb = r.in("bays").in(i);
		const Dictionary run = r.in("bays").table_at(bays, i);
		dress_of(rb, run, b, rb.integer(run, "alternate"), rb.integer(run, "first"));
	}
	return b;
}

// The model's own colour, or its scheme's.
Paint scheme_paint(DataReader &r, const Dictionary &s, const Dictionary &scheme, const char *key) {
	return s.has(key) ? paint_named(s[key], r.in(key)) : paint_named(scheme.get(key, Variant()), r.in("scheme").in(key));
}

void model_of(const Layout &l, DataReader r, const Dictionary &s, const Array &schemes, const Dictionary &surfaces, float surface_h,
		ModelSpec &m) {
	m.id = r.utf8(s, "id");
	m.group = r.utf8(s, "group");
	m.zone = r.value_of(USE_NAMES, r.utf8(s, "use"), "use");
	m.use = m.zone == BASE_RESIDENTIAL ? USE_RESIDENTIAL : m.zone == BASE_INDUSTRIAL ? USE_INDUSTRIAL : USE_COMMERCIAL;
	m.kind = kind_named(r.utf8(s, "kind"), r);
	if (s.has("function")) m.function = r.value_of(FUNCTION_NAMES, r.utf8(s, "function"), "function");
	m.palette = r.integer(s, "scheme");
	if (m.palette < 0 || m.palette >= schemes.size()) {
		r.fail(String("scheme ") + String::num_int64(m.palette) + String(" outside the ") + String::num_int64(schemes.size()) + String(" schemes"));
		m.palette = 0;
	}
	const Dictionary scheme = schemes.is_empty() ? Dictionary() : r.in("schemes").table_at(schemes, m.palette);
	m.wall = scheme_paint(r, s, scheme, "wall");
	m.roof = scheme_paint(r, s, scheme, "roof");
	m.trim = scheme_paint(r, s, scheme, "trim");
	const Array blocks = r.list_or(s, "blocks"), cores = r.list_or(s, "cores");
	for (int k = 0; k < blocks.size(); k++) m.blocks.push_back(block_of(r.in("blocks").in(k), r.in("blocks").table_at(blocks, k)));
	open_ground(surfaces, surface_h, r.in("open"), r.list_or(s, "open"), m);
	parts(l, r.in("parts"), r.list_or(s, "parts"));
	rows(l, r.in("rows"), r.list_or(s, "rows"));
	rings(l, r.in("rings"), r.list_or(s, "rings"));
	for (int k = 0; k < cores.size(); k++) {
		DataReader rc = r.in("cores").in(k);
		const Dictionary c = r.in("cores").table_at(cores, k);
		const Array where = rc.list(c, "at", POINT);
		m.cores.push_back(Core{ float(rc.in("at").num_at(where, 0)), float(rc.in("at").num_at(where, 1)), rc.integer(c, "top") });
	}
}
} // namespace

bool load_models(const Dictionary &table, Data &d, PackedStringArray &errors) {
	const int64_t before = errors.size();
	const Dictionary t = region_table(table);
	DataReader r{ errors, "models.json" };
	d.models.module_m = float(r.num(t, "module_m"));
	const float ground_h = float(r.num(t, "ground_m")), storey_h = float(r.num(t, "storey_m")), surface_h = float(r.num(t, "surface_m"));
	const Array schemes = r.list(t, "schemes"), list = r.list(t, "models");
	const Dictionary surfaces = r.table(t, "surfaces");
	const Array sizes = r.list(t, "part_sizes");
	Layout l;
	l.ring_piece = float(r.num(t, "ring_piece_m"));
	for (int i = 0; i < sizes.size(); i++) {
		const Array s = r.in("part_sizes").list_at(sizes, i, SPEC_SIZE);
		DataReader rs = r.in("part_sizes").in(i);
		l.sizes[rs.value_of(ELEMENT_NAMES, rs.utf8_at(s, SPEC_NAME), "part")] =
				Sizes{ float(rs.num_at(s, SPEC_W)), float(rs.num_at(s, SPEC_D)), float(rs.num_at(s, SPEC_H)) };
	}
	d.models.items.resize(size_t(list.size()));
	for (int i = 0; i < list.size(); i++) {
		ModelSpec &m = d.models.items[size_t(i)];
		l.model = &m;
		const Dictionary s = r.in("models").table_at(list, i);
		m.ground_h = float(r.in("models").in(i).num_or(s, "ground_m", ground_h));
		m.storey_h = float(r.in("models").in(i).num_or(s, "storey_m", storey_h));
		model_of(l, r.in("models").in(i), s, schemes, surfaces, surface_h, m);
	}
	return errors.size() == before;
}

} // namespace city
