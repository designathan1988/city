// data/render/furniture.json -> InteriorLook (the models of the furniture and the paints of the cut-open view), through the
// strict reader's values (render/data_reader.h).
#include "render/buildings/furniture_look.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace city::b40 {

namespace {
// The finishes in Finish order (types.h).
const char *const FINISHES[FIN_COUNT] = { "plaster", "stucco", "ceramic", "brick", "stone", "concrete", "wood", "metal", "glass", "tile", "slate",
	"panel", "roofing" };
// The paints the drawing of the floor, walls and cores asks for by name.
const char *const NEEDED[] = { "floor_wood", "floor_stone", "floor_porcelain", "partition", "inner_wall", "shaft", "cab", "step" };
constexpr int HEX_SKIP = 1; // the '#'
constexpr int SHORT_PART = 4, WITH_OPTIONS = 5; // a part has a shape, a size, a place and a colour, and may carry options
constexpr int VEC = 3;

// [x, y, z], short ones taken as zeros at the end (a ball's [r], a disc's [r, h]): the format writes what is not zero.
bool vec3(const DataValue &v, float out[VEC]) {
	const Array a = v.list();
	if (a.size() < 1 || a.size() > VEC) {
		v.fail("expected 1 to 3 numbers");
		return false;
	}
	for (int i = 0; i < VEC; i++) out[i] = i < a.size() ? v[i].f() : 0.f;
	return true;
}

int finish_of(const String &name) {
	for (int i = 0; i < FIN_COUNT; i++)
		if (name == FINISHES[i]) return i;
	return -1;
}

int colour_index(const InteriorLook &look, const DataValue &name) {
	const auto found = look.index.find(name.utf8());
	if (found != look.index.end()) return found->second;
	name.fail(String("unknown colour '") + name.text() + String("'"));
	return -1;
}

// [shape, size, at, colour, {n, spin, step, grow, colours}?]
bool part_of(const DataValue &a, const InteriorLook &look, FurniturePart &p) {
	const int n = a.size();
	if (n < SHORT_PART || n > WITH_OPTIONS) {
		a.fail("expected [shape, size, at, colour] and its options");
		return false;
	}
	const String shape = a[0].text();
	p.shape = shape == "box" ? PART_BOX : shape == "cyl" ? PART_CYL : PART_BALL;
	if (shape != "box" && shape != "cyl" && shape != "ball") {
		a[0].fail(String("unknown shape '") + shape + String("'"));
		return false;
	}
	if (!vec3(a[1], p.size) || !vec3(a[2], p.at)) return false;
	const int colour = colour_index(look, a[3]);
	if (colour < 0) return false;
	p.colours.push_back(uint16_t(colour));
	if (n < WITH_OPTIONS) return true;
	const DataValue o = a[4];
	p.copies = o.has("n") ? o["n"].whole() : 1;
	p.spin = o.has("spin") ? o["spin"].whole() : 1;
	if (o.has("step") && !vec3(o["step"], p.step)) return false;
	if (o.has("grow") && !vec3(o["grow"], p.grow)) return false;
	if (!o.has("colours")) return true;
	p.colours.clear();
	const DataValue names = o["colours"];
	for (int i = 0; i < names.size(); i++) {
		const int named = colour_index(look, names[i]);
		if (named < 0) return false;
		p.colours.push_back(uint16_t(named));
	}
	return true;
}
} // namespace

const Paint &InteriorLook::paint(const char *name) const {
	static const Paint MISSING{ FIN_PLASTER, 0xff00ff }; // magenta: a colour the table lacks shows
	auto it = index.find(name);
	return it == index.end() ? MISSING : palette[size_t(it->second)];
}

bool InteriorLook::load(const DataValue &table, PackedStringArray &errors) {
	const int64_t before = errors.size();
	palette.clear(); // a second read is the table again
	index.clear();
	for (auto &list : models) list.clear();
	const DataValue colours = table["palette"], models_t = table["models"];
	const Array names = colours.keys();
	for (int i = 0; i < names.size(); i++) {
		const String name = names[i];
		const DataValue def = colours[name.utf8().get_data()];
		const int fin = def.list(2).size() == 2 ? finish_of(def[0].text()) : -1;
		if (fin < 0) {
			def.fail("expected [finish, '#rrggbb']");
			continue;
		}
		def[1].colour(); // '#rrggbb', or an error
		index[std::string(name.utf8().get_data())] = int(palette.size());
		palette.push_back(Paint{ int8_t(fin), uint32_t(def[1].text().substr(HEX_SKIP).hex_to_int()) });
	}
	for (const char *need : NEEDED)
		if (!index.count(need)) colours.fail(String("no colour ") + need);
	for (int k = 0; k < FK_COUNT; k++) {
		const DataValue parts = models_t[FURNITURE_NAMES[k]];
		for (int i = 0; i < parts.size(); i++) {
			FurniturePart p;
			if (part_of(parts[i], *this, p)) models[k].push_back(std::move(p));
		}
	}
	return errors.size() == before;
}

} // namespace city::b40
