// data/render/furniture.json -> InteriorLook (the models of the furniture and the paints of the cut-open view).
#include "render/buildings/furniture_look.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

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

Variant at(const Dictionary &d, const Variant &k) { return d.get(k, Variant()); }

bool vec3(const Variant &v, float out[VEC]) {
	if (v.get_type() != Variant::ARRAY) return false;
	Array a = v;
	if (a.size() < 1) return false;
	for (int i = 0; i < VEC; i++) out[i] = i < a.size() ? float(a[i]) : 0.f;
	return true;
}

int finish_of(const String &name) {
	for (int i = 0; i < FIN_COUNT; i++)
		if (name == FINISHES[i]) return i;
	return -1;
}

bool part_of(const Array &a, const InteriorLook &look, FurniturePart &p, String &why) {
	if (a.size() < SHORT_PART) return why = "short part", false;
	const String shape = a[0];
	p.shape = shape == "box" ? PART_BOX : shape == "cyl" ? PART_CYL : PART_BALL;
	if (shape != "box" && shape != "cyl" && shape != "ball") return why = "shape " + shape, false;
	if (!vec3(a[1], p.size) || !vec3(a[2], p.at)) return why = "sizes", false;
	auto colour = look.index.find(std::string(String(a[3]).utf8().get_data()));
	if (colour == look.index.end()) return why = "colour " + String(a[3]), false;
	p.colours.push_back(uint16_t(colour->second));
	if (a.size() < WITH_OPTIONS) return true;
	Dictionary o = a[4];
	p.copies = int(o.get("n", 1));
	p.spin = int(o.get("spin", 1));
	if (o.has("step")) vec3(o["step"], p.step);
	if (o.has("grow")) vec3(o["grow"], p.grow);
	if (!o.has("colours")) return true;
	p.colours.clear();
	Array names = o["colours"];
	for (int i = 0; i < names.size(); i++) {
		auto named = look.index.find(std::string(String(names[i]).utf8().get_data()));
		if (named == look.index.end()) return why = "colour " + String(names[i]), false;
		p.colours.push_back(uint16_t(named->second));
	}
	return true;
}
} // namespace

const Paint &InteriorLook::paint(const char *name) const {
	static const Paint MISSING{ FIN_PLASTER, 0xff00ff };
	auto it = index.find(name);
	return it == index.end() ? MISSING : palette[size_t(it->second)];
}

bool InteriorLook::load(const Dictionary &table, PackedStringArray &errors) {
	Dictionary colours = at(table, "palette"), models_t = at(table, "models");
	Array names = colours.keys();
	for (int i = 0; i < names.size(); i++) {
		Array def = colours[names[i]];
		const int fin = def.size() == 2 ? finish_of(def[0]) : -1;
		if (fin < 0) {
			errors.push_back("furniture.json: invalid palette entry " + String(names[i]));
			continue;
		}
		index[std::string(String(names[i]).utf8().get_data())] = int(palette.size());
		palette.push_back(Paint{ int8_t(fin), uint32_t(String(def[1]).substr(HEX_SKIP).hex_to_int()) });
	}
	for (const char *need : NEEDED)
		if (!index.count(need)) errors.push_back(String("furniture.json: missing the colour ") + need);
	for (int k = 0; k < FK_COUNT; k++) {
		if (!models_t.has(FURNITURE_NAMES[k])) {
			errors.push_back(String("furniture.json: missing the model ") + FURNITURE_NAMES[k]);
			continue;
		}
		Array parts = models_t[FURNITURE_NAMES[k]];
		for (int i = 0; i < parts.size(); i++) {
			FurniturePart p;
			String why;
			if (!part_of(parts[i], *this, p, why)) errors.push_back(String("furniture.json: ") + FURNITURE_NAMES[k] + ": " + why);
			else models[k].push_back(std::move(p));
		}
	}
	return errors.is_empty();
}

} // namespace city::b40
