// DataReader: the one strict reader of the data tables (project/data/** as Godot hands them to the core). Every key is present
// and of its type, every list of its length, every index inside its list: godot-cpp's Array::operator[] dereferences what the
// engine returns, and the engine returns nullptr past the end, so a positional read is checked here first. A problem is an
// error that names the table and the path (`where`), and reading goes on with a neutral value, so one boot reports them all.
// The build has no exceptions: nothing here throws or aborts. The bodies are in data_reader.cpp, compiled once: tables are read
// at boot, and an inline copy at every read site only made the DLL bigger.
#pragma once

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace city {

struct DataValue;

struct DataReader {
	godot::PackedStringArray &errors;
	godot::String where; // "buildings.json.lots.pool"

	// The reader of a part of this one: its errors go to the same list, its path a step longer.
	DataReader in(const godot::String &step) const;
	DataReader in(int index) const;
	void fail(const godot::String &what);

	bool has(const godot::Dictionary &d, const char *key);
	double num(const godot::Dictionary &d, const char *key);
	int integer(const godot::Dictionary &d, const char *key);
	bool flag(const godot::Dictionary &d, const char *key);
	godot::String text(const godot::Dictionary &d, const char *key);
	std::string utf8(const godot::Dictionary &d, const char *key);
	godot::Array list(const godot::Dictionary &d, const char *key, int size = -1);
	godot::Dictionary table(const godot::Dictionary &d, const char *key);
	// A list of numbers, each a float or each a whole number.
	std::vector<float> floats(const godot::Dictionary &d, const char *key);
	std::vector<int> counts(const godot::Dictionary &d, const char *key);
	// '#rrggbb' or '#rrggbbaa' as 0xRRGGBBAA (core/color.h).
	uint32_t colour(const godot::Dictionary &d, const char *key);
	// A key the table may leave out: absent, the value the format gives it; present, checked as any other.
	double num_or(const godot::Dictionary &d, const char *key, double fallback);
	int integer_or(const godot::Dictionary &d, const char *key, int fallback);
	bool flag_or(const godot::Dictionary &d, const char *key, bool fallback);
	godot::Array list_or(const godot::Dictionary &d, const char *key);
	godot::Dictionary table_or(const godot::Dictionary &d, const char *key);
	// A list that came some other way (a value handed in, a list whose length depends on its content) checked for its length.
	void expect(const godot::Array &a, int size);
	// A whole table as a value with its path, for code that reads as it builds (the kit builders): DataValue below.
	DataValue at(const godot::Variant &table) const;

	double num_at(const godot::Array &a, int i);
	int integer_at(const godot::Array &a, int i);
	godot::String text_at(const godot::Array &a, int i);
	std::string utf8_at(const godot::Array &a, int i);
	godot::Array list_at(const godot::Array &a, int i, int size = -1);
	godot::Dictionary table_at(const godot::Array &a, int i);
	uint32_t colour_at(const godot::Array &a, int i);

	// The index of `s` in a table of names: an unknown name is an error saying what it was, and 0 stands in.
	template <size_t N>
	int index_of(const char *const (&names)[N], const std::string &s, const char *what) {
		for (size_t i = 0; i < N; i++)
			if (s == names[i]) return int(i);
		unknown(s, what);
		return 0;
	}
	// The value of `s` in a table of {name, value} rows (api/function_names.h Named): unknown, the first row's.
	template <typename Row, size_t N>
	int value_of(const Row (&rows)[N], const std::string &s, const char *what) {
		for (const Row &row : rows)
			if (s == row.name) return row.value;
		unknown(s, what);
		return rows[0].value;
	}

private:
	friend struct DataValue;
	// The value at `key` (or index `i`) when it is of `type` (a number accepts INT and FLOAT); Nil, with an error, otherwise.
	godot::Variant typed(const godot::Dictionary &d, const char *key, godot::Variant::Type type);
	godot::Variant typed_at(const godot::Array &a, int i, godot::Variant::Type type);
	godot::Variant checked(const godot::Variant &v, godot::Variant::Type type, const godot::String &key);
	void unknown(const std::string &s, const char *what);
	// A count or an index: a number with a fraction is an error, not a truncation.
	int whole(double v, const godot::String &key);
	godot::Array sized(const godot::Array &a, int size, const godot::String &key);
	// Every character after '#' a hex digit: parse_hex_color reads what strtoul can and stops at the rest.
	uint32_t hex(const godot::Variant &v, const godot::String &key);
};

// A value inside a table with its path, read one step at a time: `d["box_m"][2].f()`. Every step checks what DataReader checks
// (a key present, an index inside its list, the value of its type, a list of its length); the first failure is said once, and
// what is read through it is a neutral value, said nothing more of.
struct DataValue {
	DataReader reader; // its path, and the list its errors go to
	godot::Variant value;
	bool failed = false; // a step before it failed, and said so

	DataValue operator[](const char *key) const; // a key of a table
	DataValue operator[](int index) const; // an item of a list
	bool has(const char *key) const; // a key the format may leave out (a table, or nothing said)
	int size() const; // the items of a list
	godot::Array keys() const; // the keys of a table
	double num() const;
	float f() const { return float(num()); }
	int whole() const; // a count or an index: a fraction is an error
	bool flag() const;
	godot::String text() const;
	std::string utf8() const;
	uint32_t colour() const; // '#rrggbb[aa]' as 0xRRGGBBAA
	godot::Vector2 v2() const; // [x, y]
	godot::Vector3 v3() const; // [x, y, z]
	godot::Array list(int size = -1) const;
	godot::Dictionary table() const;
	void fail(const godot::String &what) const; // a rule of the table's own, on this value

private:
	godot::Variant typed(godot::Variant::Type type) const;
};

} // namespace city
