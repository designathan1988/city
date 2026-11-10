// Parses data/render/models.json. Rows of parts and fences all round a rectangle are laid out here, once, into plain elements; an
// unknown name is an error at startup, never a silent default.
#include "api/models_loader.h"

#include "api/function_names.h"

#include "api/buildings_loader.h"
#include "render/region_table.h"
#include "sim/buildings/building_store.h"

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
constexpr int ZONE_RESIDENTIAL = 1, ZONE_COMMERCIAL = 2, ZONE_INDUSTRIAL = 3, ZONE_MIXED = 4;
constexpr int SPEC_W = 1, SPEC_D = 2, SPEC_H = 3, RECT_WIDTH = 2, RECT_DEPTH = 3;

constexpr Named ROOF_NAMES[] = { { "flat", ROOF_FLAT }, { "terrace", ROOF_TERRACE }, { "gable", ROOF_GABLE }, { "hip", ROOF_HIP }, { "shed", ROOF_SHED },
	{ "sawtooth", ROOF_SAWTOOTH } };
constexpr Named PATTERN_NAMES[] = { { "office", PATTERN_OFFICE }, { "gallery", PATTERN_GALLERY }, { "artDeco", PATTERN_ART_DECO }, { "artDecoCrown", PATTERN_ART_DECO_CROWN } };
constexpr Named ELEMENT_NAMES[] = { { "tree", EL_TREE }, { "bench", EL_BENCH }, { "planter", EL_PLANTER }, { "flowers", EL_FLOWERS }, { "rocks", EL_ROCKS }, { "fence", EL_FENCE },
	{ "wall", EL_WALL }, { "pavement", EL_PAVEMENT }, { "slab", EL_SLAB }, { "pillar", EL_PILLAR }, { "canopy", EL_CANOPY }, { "parking", EL_PARKING },
	{ "railing", EL_RAILING }, { "clock", EL_CLOCK }, { "awning", EL_AWNING } };
constexpr Named USE_NAMES[] = { { "residential", ZONE_RESIDENTIAL }, { "commercial", ZONE_COMMERCIAL }, { "industrial", ZONE_INDUSTRIAL }, { "mixed", ZONE_MIXED } };

Variant at(const Dictionary &d, const Variant &k) { return d.get(k, Variant()); }
std::string str(const Variant &v) { return std::string(String(v).utf8().get_data()); }

template <size_t N>
int named(const Named (&list)[N], const Variant &v, const char *what, PackedStringArray &errors) {
	std::string s = str(v);
	for (const Named &n : list)
		if (s == n.name) return n.value;
	errors.push_back(String("models.json: ") + what + " desconhecido: " + s.c_str());
	return list[0].value;
}

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
LotElement part(const Layout &l, int kind, float x, float y, int facing) {
	LotElement e;
	const Sizes &s = l.sizes.at(kind);
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

void parts(const Layout &l, const Array &list, PackedStringArray &errors) {
	for (int i = 0; i < list.size(); i++) {
		Dictionary p = list[i];
		Array where = at(p, "at");
		LotElement e = part(l, named(ELEMENT_NAMES, at(p, "kind"), "peça", errors), float(where[0]), float(where[1]), int(p.get("facing", 0)));
		Dictionary size = p.get("size", Dictionary());
		e.w = float(size.get("w", e.w));
		e.d = float(size.get("d", e.d));
		e.h = float(size.get("h", e.h));
		e.z = float(p.get("rise_m", 0.0));
		put(e, *l.model);
	}
}

void rows(const Layout &l, const Array &list, PackedStringArray &errors) {
	for (int i = 0; i < list.size(); i++) {
		Dictionary r = list[i];
		Array from = at(r, "from"), to = at(r, "to");
		int n = int(at(r, "count")), kind = named(ELEMENT_NAMES, at(r, "kind"), "peça", errors), facing = int(r.get("facing", 0));
		for (int k = 0; k < n; k++) {
			float share = n == 1 ? 0.5f : float(k) / float(n - 1);
			float x = float(from[0]) + (float(to[0]) - float(from[0])) * share, y = float(from[1]) + (float(to[1]) - float(from[1])) * share;
			put(part(l, kind, x, y, facing), *l.model);
		}
	}
}

// A fence or wall all round a rectangle, in pieces; the gate is a gap in the middle of the front.
void rings(const Layout &l, const Array &list, PackedStringArray &errors) {
	for (int i = 0; i < list.size(); i++) {
		Dictionary r = list[i];
		Array rect = at(r, "at");
		int kind = named(ELEMENT_NAMES, at(r, "kind"), "peça", errors);
		float x = float(rect[0]), y = float(rect[1]), w = float(rect[RECT_WIDTH]), d = float(rect[RECT_DEPTH]), gate = float(at(r, "opening_m")), piece = l.ring_piece;
		auto lay = [&](float px, float py, int facing) {
			LotElement e = part(l, kind, px, py, facing);
			e.w = piece;
			put(e, *l.model);
		};
		for (float a = 0; a + piece <= w + RING_TOLERANCE_M; a += piece) {
			if (std::fabs(a + piece / 2 - w / 2) > gate / 2) lay(x + a + piece / 2, y, FACE_STREET);
			lay(x + a + piece / 2, y + d, FACE_BACK);
		}
		for (float a = 0; a + piece <= d + RING_TOLERANCE_M; a += piece) {
			lay(x, y + a + piece / 2, FACE_LEFT);
			lay(x + w, y + a + piece / 2, FACE_RIGHT);
		}
	}
}

void open_ground(const Dictionary &surfaces, float height, const Array &list, ModelSpec &m, PackedStringArray &errors) {
	for (int i = 0; i < list.size(); i++) {
		Dictionary o = list[i];
		Array rect = at(o, "at");
		LotElement e;
		float w = float(rect[RECT_WIDTH]), d = float(rect[RECT_DEPTH]);
		e.kind = EL_SURFACE;
		e.x = float(rect[0]) + w / 2;
		e.y = float(rect[1]) + d / 2;
		e.w = w;
		e.d = d;
		e.h = height;
		e.paint = int(m.paints.size());
		m.paints.push_back(paint_named(at(surfaces, at(o, "surface")), errors));
		m.elements.push_back(e);
	}
}

void dress_of(const Dictionary &d, ModelBlock &b, int every, int start, PackedStringArray &errors) {
	ModelDress out;
	out.side = int(at(d, "side"));
	out.from = int(at(d, "from"));
	out.to = int(at(d, "to"));
	out.every = every;
	out.start = start;
	out.fill = bay_named(str(at(d, "fill")), errors);
	b.dress.push_back(out);
}

ModelBlock block_of(const Dictionary &s, PackedStringArray &errors) {
	ModelBlock b;
	Array rect = at(s, "at");
	b.x = float(rect[0]);
	b.y = float(rect[1]);
	b.w = float(rect[RECT_WIDTH]);
	b.d = float(rect[RECT_DEPTH]);
	b.base = int(s.get("base", 0));
	b.storeys = int(at(s, "storeys"));
	b.pitch = float(s.get("pitch_deg", -1.0));
	if (s.has("roof")) b.roof = Roof(named(ROOF_NAMES, at(s, "roof"), "telhado", errors));
	b.fill = bay_named(str(at(s, "fill")), errors);
	if (s.has("ground_fill")) b.ground = bay_named(str(at(s, "ground_fill")), errors);
	if (s.has("door_kind")) b.door_kind = bay_named(str(at(s, "door_kind")), errors);
	if (s.has("door_bay")) {
		Variant door = at(s, "door_bay");
		b.door_bay = door.get_type() == Variant::STRING ? MIDDLE_BAY : int(door);
	}
	if (s.has("pattern")) b.pattern = named(PATTERN_NAMES, at(s, "pattern"), "padrão", errors);
	Array outline = s.get("outline", Array());
	for (int i = 0; i < outline.size(); i++) {
		Array p = outline[i];
		b.outline.push_back(Point2{ float(p[0]), float(p[1]) });
	}
	Array sides = s.get("sides", Array()), bays = s.get("bays", Array());
	for (int i = 0; i < sides.size(); i++) dress_of(sides[i], b, 0, 0, errors);
	for (int i = 0; i < bays.size(); i++) {
		Dictionary run = bays[i];
		dress_of(run, b, int(at(run, "alternate")), int(at(run, "first")), errors);
	}
	return b;
}
} // namespace

bool load_models(const Dictionary &table, Data &d, PackedStringArray &errors) {
	const int64_t before = errors.size();
	const Dictionary t = region_table(table);
	d.models.module_m = float(at(t, "module_m"));
	const float ground_h = float(at(t, "ground_m")), storey_h = float(at(t, "storey_m")), surface_h = float(at(t, "surface_m"));
	const Array schemes = at(t, "schemes"), list = at(t, "models");
	const Dictionary surfaces = at(t, "surfaces");
	const Array sizes = at(t, "part_sizes");
	Layout l;
	l.ring_piece = float(at(t, "ring_piece_m"));
	for (int i = 0; i < sizes.size(); i++) {
		Array s = sizes[i];
		l.sizes[named(ELEMENT_NAMES, s[0], "peça", errors)] = Sizes{ float(s[SPEC_W]), float(s[SPEC_D]), float(s[SPEC_H]) };
	}
	d.models.items.resize(size_t(list.size()));
	for (int i = 0; i < list.size(); i++) {
		Dictionary s = list[i];
		ModelSpec &m = d.models.items[size_t(i)];
		l.model = &m;
		m.id = str(at(s, "id"));
		m.group = str(at(s, "group"));
		m.zone = named(USE_NAMES, at(s, "use"), "uso", errors);
		m.use = m.zone == ZONE_RESIDENTIAL ? USE_RESIDENTIAL : m.zone == ZONE_INDUSTRIAL ? USE_INDUSTRIAL : USE_COMMERCIAL;
		m.kind = kind_named(str(at(s, "kind")), errors);
		if (s.has("function")) m.function = named(FUNCTION_NAMES, at(s, "function"), "função", errors);
		m.palette = int(at(s, "scheme"));
		m.ground_h = float(s.get("ground_m", ground_h));
		m.storey_h = float(s.get("storey_m", storey_h));
		Dictionary scheme = schemes[m.palette % schemes.size()];
		m.wall = paint_named(s.get("wall", at(scheme, "wall")), errors);
		m.roof = paint_named(s.get("roof", at(scheme, "roof")), errors);
		m.trim = paint_named(s.get("trim", at(scheme, "trim")), errors);
		Array blocks = s.get("blocks", Array()), cores = s.get("cores", Array());
		for (int k = 0; k < blocks.size(); k++) m.blocks.push_back(block_of(blocks[k], errors));
		open_ground(surfaces, surface_h, s.get("open", Array()), m, errors);
		parts(l, s.get("parts", Array()), errors);
		rows(l, s.get("rows", Array()), errors);
		rings(l, s.get("rings", Array()), errors);
		for (int k = 0; k < cores.size(); k++) {
			Dictionary c = cores[k];
			Array where = at(c, "at");
			m.cores.push_back(Core{ float(where[0]), float(where[1]), int(at(c, "top")) });
		}
	}
	return errors.size() == before;
}

} // namespace city
