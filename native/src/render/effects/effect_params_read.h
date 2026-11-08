// What the effects' tables are read with (effect_params.cpp, effect_params_aftermath.cpp): a table of a JSON file through the strict
// reader, a list of three numbers as a vector, the least counts a table may give.
#pragma once

#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace city::effect_read {

constexpr size_t FEWEST_CORNERS = 3;
constexpr int MIN_ROPE_POINTS = 2;
constexpr int VEC3 = 3;

// A list of three numbers (already checked for its length by the reader) as a vector; a zero one when the reader failed it.
inline godot::Vector3 vec3_of(const godot::Array &a) {
	return a.size() == VEC3 ? godot::Vector3(float(double(a[0])), float(double(a[1])), float(double(a[2]))) : godot::Vector3();
}

// One table of a JSON file, read through the strict reader.
struct Group {
	DataReader r;
	godot::Dictionary d;
	Group(DataReader &top, const godot::Dictionary &root, const char *key) : r(top.in(key)), d(top.table(root, key)) {}
	float num(const char *key) { return float(r.num(d, key)); }
	int integer(const char *key) { return r.integer(d, key); }
};

} // namespace city::effect_read
