// A document into a body (body_doc.h): paints, blocks with what each storey shows, reliefs, lot parts and cores, all in the document's
// lot frame until the fit at the end. Open blocks are the lot's own ground: a surface, none for plain grass.
#include "api/body_doc.h"

#include "sim/buildings/b40/signature_kinds.h"

#include "api/function_names.h"

#include "sim/commands/body_ops.h"
#include "sim/commands/model_ops.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cstring>
#include <string>

using namespace godot;

namespace city {

using namespace b40;

namespace {
constexpr float OPEN_SURFACE_H = 0.02f; // what an open block stands over the ground

constexpr Named DOC_BAYS[] = { { "wall", BAY_WALL }, { "window", BAY_WINDOW }, { "sashWindow", BAY_SASH }, { "frenchWindow", BAY_FRENCH },
	{ "bayWindow", BAY_BAY_WINDOW }, { "ribbon", BAY_RIBBON }, { "wideWindow", BAY_WIDE }, { "balcony", BAY_BALCONY }, { "door", BAY_DOOR },
	{ "doubleDoor", BAY_DOUBLE_DOOR }, { "garageDoor", BAY_GARAGE_DOOR }, { "shopfront", BAY_SHOPFRONT }, { "loadingDoor", BAY_LOADING_DOOR },
	{ "pillar", BAY_PILLAR }, { "loggia", BAY_LOGGIA }, { "brise", BAY_BRISE }, { "shutteredWindow", BAY_SHUTTERED }, { "gourmet", BAY_GOURMET } };
constexpr Named DOC_FINISHES[] = { { "plaster", FIN_PLASTER }, { "stucco", FIN_STUCCO }, { "ceramic", FIN_CERAMIC }, { "brick", FIN_BRICK },
	{ "stone", FIN_STONE }, { "concrete", FIN_CONCRETE }, { "wood", FIN_WOOD }, { "metal", FIN_METAL }, { "glass", FIN_GLASS }, { "tile", FIN_TILE },
	{ "slate", FIN_SLATE }, { "panel", FIN_PANEL }, { "roofing", FIN_ROOFING } };
constexpr Named DOC_ROOFS[] = { { "flat", ROOF_FLAT }, { "terrace", ROOF_TERRACE }, { "gable", ROOF_GABLE }, { "hip", ROOF_HIP }, { "shed", ROOF_SHED },
	{ "sawtooth", ROOF_SAWTOOTH } };
constexpr Named DOC_LINES[] = { { "every", LINES_EVERY }, { "base", LINES_BASE }, { "slab", LINES_SLAB }, { "none", LINES_NONE } };
constexpr Named DOC_CROWNS[] = { { "cornice", CROWN_CORNICE }, { "deco", CROWN_DECO }, { "slab", CROWN_SLAB }, { "attic", CROWN_ATTIC }, { "none", CROWN_NONE } };
// mixed: homes over shops; what a storey is used for it says itself, and what is left is homes
constexpr Named DOC_USES[] = { { "residential", USE_RESIDENTIAL }, { "mixed", USE_RESIDENTIAL }, { "commercial", USE_COMMERCIAL }, { "industrial", USE_INDUSTRIAL } };
constexpr Named DOC_ELEMENTS[] = { { "pavement", EL_PAVEMENT }, { "slab", EL_SLAB }, { "pillar", EL_PILLAR }, { "canopy", EL_CANOPY }, { "awning", EL_AWNING },
	{ "wall", EL_WALL }, { "fence", EL_FENCE }, { "railing", EL_RAILING }, { "hedge", EL_HEDGE }, { "gate", EL_GATE }, { "carGate", EL_CAR_GATE },
	{ "planter", EL_PLANTER }, { "bench", EL_BENCH }, { "bollard", EL_BOLLARD }, { "lamp", EL_LAMP }, { "bin", EL_BIN }, { "ac", EL_AC },
	{ "stall", EL_STALL }, { "dock", EL_DOCK }, { "drain", EL_DRAIN }, { "tree", EL_TREE }, { "shrub", EL_SHRUB }, { "flowers", EL_FLOWERS },
	{ "pool", EL_POOL }, { "lounger", EL_LOUNGER }, { "table", EL_TABLE }, { "swing", EL_SWING }, { "shed", EL_SHED }, { "line", EL_LINE },
	{ "pallet", EL_PALLET }, { "cage", EL_CAGE }, { "stair", EL_STAIR }, { "ramp", EL_RAMP }, { "rocks", EL_ROCKS }, { "parking", EL_PARKING },
	{ "clock", EL_CLOCK } };
constexpr Named DOC_PATTERNS[] = { { "residential", PATTERN_RESIDENTIAL }, { "storefront", PATTERN_STOREFRONT }, { "office", PATTERN_OFFICE },
	{ "industrial", PATTERN_INDUSTRIAL }, { "arcade", PATTERN_ARCADE }, { "gallery", PATTERN_GALLERY }, { "artDeco", PATTERN_ART_DECO },
	{ "artDecoCrown", PATTERN_ART_DECO_CROWN }, { "observation", PATTERN_OBSERVATION } };
// What each open surface shows, by the colour table's name.
constexpr struct {
	const char *open, *colour;
} DOC_SURFACES[] = { { "paving", "paving" }, { "gravel", "concrete" }, { "sand", "sand" }, { "asphalt", "asphalt" }, { "concrete", "concrete" },
	{ "pavers", "paving" }, { "tiles", "paving" }, { "water", "fountain_water" } };

std::string text(const Variant &v) { return std::string(String(v).utf8().get_data()); }

class Reader {
public:
	Reader(const Data &data, Body &body, PackedStringArray &errors) : data_(data), b_(body), errors_(errors) {}

	template <size_t N>
	int named(const Named (&table)[N], const Variant &v, const char *what) {
		const std::string s = text(v);
		for (const Named &n : table)
			if (s == n.name) return n.value;
		errors_.push_back(String("documento: ") + what + " desconhecido: " + s.c_str());
		return table[0].value;
	}
	Paint paint(const Dictionary &m) {
		return Paint{ int8_t(named(DOC_FINISHES, m.get("finish", "plaster"), "acabamento")), uint32_t(int64_t(m.get("colour", 0))) };
	}
	void building(const Dictionary &doc);

private:
	void storey(Volume &v, int k, const Dictionary &row);
	void faces(Volume &v, const Dictionary &geometry);
	void reliefs(Volume &v, const Array &rows);
	void block(const Dictionary &d);
	void open_block(const Dictionary &d, const std::string &open);
	void element(const Dictionary &d);
	Kind kind_of(const std::string &function) const;

	const Data &data_;
	Body &b_;
	PackedStringArray &errors_;
};

// "side:index" of a bay override.
bool bay_key_of(const std::string &key, int &side, int &bay) {
	const size_t colon = key.find(':');
	if (colon == std::string::npos) return false;
	side = std::stoi(key.substr(0, colon));
	bay = std::stoi(key.substr(colon + 1));
	return true;
}

void Reader::storey(Volume &v, int k, const Dictionary &row) {
	const Dictionary facade = row.get("facade", Dictionary());
	v.fill.push_back(uint8_t(named(DOC_BAYS, facade.get("fill", "wall"), "componente")));
	const Dictionary sides = facade.get("sides", Dictionary());
	for (const Variant &key : sides.keys()) v.side_fill[side_key(k, int(String(key).to_int()))] = uint8_t(named(DOC_BAYS, sides[key], "componente"));
	const Dictionary bays = facade.get("bays", Dictionary());
	for (const Variant &key : bays.keys()) {
		int side = 0, bay = 0;
		if (bay_key_of(text(key), side, bay)) v.bays[bay_key(k, side, bay)] = uint8_t(named(DOC_BAYS, bays[key], "componente"));
	}
	const Dictionary paints = row.get("materials", Dictionary());
	for (const Variant &key : paints.keys()) v.storey_paint[side_key(k, int(String(key).to_int()))] = b_.add_paint(paint(paints[key]));
}

void Reader::faces(Volume &v, const Dictionary &geometry) {
	for (const Variant &key : geometry.keys()) {
		const int side = int(String(key).to_int());
		if (side < 0 || side >= MAX_FACES) continue;
		const Dictionary g = geometry[key];
		FaceGeometry &f = v.faces[side];
		f.bays = int(g.get("bays", -1));
		f.window_w = float(g.get("windowWidth", 0));
		f.window_h = float(g.get("windowHeight", 0));
		f.sill = float(g.get("sill", 0));
		f.pier_w = float(g.get("pierWidth", 0));
		f.pier_d = float(g.get("pierDepth", 0));
		f.pier_every = int(g.get("pierEvery", -1));
	}
}

void Reader::reliefs(Volume &v, const Array &rows) {
	for (const Variant &row : rows) {
		const Dictionary r = row;
		v.reliefs.push_back(Relief{ int(r.get("side", 0)), int(r.get("bay0", 0)), int(r.get("bay1", 0)), int(r.get("storey0", 0)), int(r.get("storey1", 0)), float(r.get("depth", 0)) });
	}
}

void Reader::open_block(const Dictionary &d, const std::string &open) {
	for (const auto &s : DOC_SURFACES) {
		if (open != s.open) continue;
		LotElement e;
		e.kind = EL_SURFACE;
		const float w = float(d.get("w", 0)), depth = float(d.get("d", 0));
		e.x = float(d.get("x", 0)) + w / 2;
		e.y = float(d.get("y", 0)) + depth / 2;
		e.w = w;
		e.d = depth;
		e.h = OPEN_SURFACE_H;
		e.paint = b_.add_paint(data_.color(s.colour));
		b_.elements.push_back(e);
	}
}

void Reader::block(const Dictionary &d) {
	const std::string open = text(d.get("open", ""));
	if (!open.empty()) {
		open_block(d, open);
		return;
	}
	Volume v;
	v.x = float(d.get("x", 0));
	v.y = float(d.get("y", 0));
	v.w = float(d.get("w", 0));
	v.d = float(d.get("d", 0));
	v.base = int(d.get("base", 0));
	v.roof = Roof(named(DOC_ROOFS, d.get("roof", "flat"), "telhado"));
	v.pitch = float(d.get("pitch", -1));
	v.keep = true;
	const Array storeys = d.get("storeys", Array());
	v.storeys = int(storeys.size());
	for (int k = 0; k < v.storeys; k++) storey(v, k, storeys[k]);
	for (const Variant &p : Array(d.get("outline", Array()))) {
		const Dictionary point = p;
		v.outline.push_back(Point2{ float(point.get("x", 0)), float(point.get("y", 0)) });
	}
	const Dictionary dress = d.get("dress", Dictionary());
	v.lines = Lines(named(DOC_LINES, dress.get("lines", "none"), "linha"));
	v.crown = Crown(named(DOC_CROWNS, dress.get("crown", "none"), "coroa"));
	const Dictionary paints = d.get("materials", Dictionary());
	if (paints.has("wall")) v.wall = b_.add_paint(paint(paints["wall"]));
	if (paints.has("roof")) v.roof_paint = b_.add_paint(paint(paints["roof"]));
	faces(v, d.get("facadeGeometry", Dictionary()));
	reliefs(v, d.get("reliefs", Array()));
	b_.volumes.push_back(std::move(v));
	if (d.has("facadePattern")) op_set_pattern(b_, int(b_.volumes.size()) - 1, -1, -1, named(DOC_PATTERNS, d["facadePattern"], "fachada"));
}

void Reader::element(const Dictionary &d) {
	LotElement e;
	e.kind = Element(named(DOC_ELEMENTS, d.get("kind", ""), "peça"));
	e.x = float(d.get("x", 0));
	e.y = float(d.get("y", 0));
	e.w = float(d.get("w", 1));
	e.d = float(d.get("d", 1));
	e.z = float(d.get("z", 0));
	e.h = float(d.get("h", 1));
	e.facing = int(d.get("facing", 0));
	if (d.has("material")) e.paint = b_.add_paint(paint(d["material"]));
	b_.elements.push_back(e);
}

Kind Reader::kind_of(const std::string &function) const {
	if (function == "residentialTower") return KIND_TOWER;
	if (function == "apartments") return KIND_FLATS;
	if (function == "house" || function == "townhouse") return KIND_HOUSE;
	if (b_.use == USE_INDUSTRIAL) return KIND_WORKS;
	if (b_.use == USE_RESIDENTIAL) return KIND_FLATS;
	return function == "shop" ? KIND_SHOP : KIND_OFFICE;
}

void Reader::building(const Dictionary &doc) {
	b_.use = Use(named(DOC_USES, doc.get("use", "residential"), "uso"));
	b_.kind = b_.plan.kind = kind_of(text(doc.get("function", "")));
	b_.ground = b_.kind == KIND_HOUSE ? GROUND_HOUSE : b_.kind == KIND_SHOP ? GROUND_SHOP : b_.kind == KIND_WORKS ? GROUND_WORKS : GROUND_LOBBY;
	b_.palette = int(doc.get("palette", 0));
	b_.ground_h = float(doc.get("groundHeight", b_.ground_h));
	b_.storey_h = float(doc.get("storeyHeight", b_.storey_h));
	const Dictionary paints = doc.get("materials", Dictionary());
	b_.wall = b_.add_paint(paint(paints.get("wall", Dictionary())));
	b_.roof = b_.add_paint(paint(paints.get("roof", Dictionary())));
	b_.trim = b_.add_paint(paint(paints.get("trim", Dictionary())));
	for (const Variant &v : Array(doc.get("volumes", Array()))) block(v);
	for (const Variant &e : Array(doc.get("elements", Array()))) element(e);
	for (const Variant &row : Array(doc.get("cores", Array()))) {
		const Dictionary c = row;
		const String kind = c.get("kind", "lift");
		b_.cores.push_back(Core{ float(c.get("x", 0)), float(c.get("y", 0)), int(c.get("to", 0)), kind == "stair" ? CORE_STAIR : kind == "stairLift" ? CORE_STAIR_LIFT : CORE_LIFT });
	}
}
} // namespace

bool body_from_doc(const Dictionary &doc, const Data &data, Body &out, float &front_x, float &front_y, int &function, PackedStringArray &errors) {
	out = Body{};
	const int before = int(errors.size());
	Reader reader(data, out, errors);
	reader.building(doc);
	function = doc.has("function") ? reader.named(FUNCTION_NAMES, doc["function"], "função") : int(FN_NONE);
	if (int(errors.size()) != before || out.volumes.empty()) return false;
	const V2 front = fit_plan(out);
	front_x = front.x;
	front_y = front.y;
	out.signature = uint8_t(signature_of(String(doc.get("blueprint", "")).utf8().get_data()));
	out.signature_x = front.x;
	out.signature_y = front.y;
	return true;
}

} // namespace city
