// The strict reader's bodies (data_reader.h): presence, type, length, index, whole numbers, colours; every failure an error
// that names the table and the path.
#include "render/data_reader.h"

#include "core/color.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr size_t RGB_TEXT = 7, RGBA_TEXT = 9; // '#rrggbb', '#rrggbbaa'

String step_of(int index) { return String("[") + String::num_int64(index) + String("]"); }
double number(const Variant &v) { return v.get_type() == Variant::FLOAT || v.get_type() == Variant::INT ? double(v) : 0.0; }
String string_of(const Variant &v) { return v.get_type() == Variant::STRING ? String(v) : String(); }
Array array_of(const Variant &v) { return v.get_type() == Variant::ARRAY ? Array(v) : Array(); }
Dictionary dict_of(const Variant &v) { return v.get_type() == Variant::DICTIONARY ? Dictionary(v) : Dictionary(); }
} // namespace

DataReader DataReader::in(const String &step) const { return DataReader{ errors, where + String(".") + step }; }
DataReader DataReader::in(int index) const { return DataReader{ errors, where + step_of(index) }; }
void DataReader::fail(const String &what) { errors.push_back(where + String(": ") + what); }

bool DataReader::has(const Dictionary &d, const char *key) {
	if (d.has(key)) return true;
	fail(String("missing '") + key + String("'"));
	return false;
}

Variant DataReader::typed(const Dictionary &d, const char *key, Variant::Type type) {
	if (!has(d, key)) return Variant();
	return checked(d[key], type, String(key));
}

Variant DataReader::typed_at(const Array &a, int i, Variant::Type type) {
	if (i < 0 || i >= a.size()) {
		fail(String("no item ") + String::num_int64(i) + String(" (the list has ") + String::num_int64(a.size()) + String(")"));
		return Variant();
	}
	return checked(a[i], type, step_of(i));
}

Variant DataReader::checked(const Variant &v, Variant::Type type, const String &key) {
	const Variant::Type got = v.get_type();
	if (got == type || (type == Variant::FLOAT && got == Variant::INT)) return v;
	fail(key + String(": expected ") + Variant::get_type_name(type) + String(", found ") + Variant::get_type_name(got));
	return Variant();
}

double DataReader::num(const Dictionary &d, const char *key) { return number(typed(d, key, Variant::FLOAT)); }
int DataReader::integer(const Dictionary &d, const char *key) { return whole(num(d, key), String(key)); }
bool DataReader::flag(const Dictionary &d, const char *key) {
	const Variant v = typed(d, key, Variant::BOOL);
	return v.get_type() == Variant::BOOL && bool(v);
}
String DataReader::text(const Dictionary &d, const char *key) { return string_of(typed(d, key, Variant::STRING)); }
std::string DataReader::utf8(const Dictionary &d, const char *key) { return std::string(text(d, key).utf8().get_data()); }
Array DataReader::list(const Dictionary &d, const char *key, int size) { return sized(array_of(typed(d, key, Variant::ARRAY)), size, String(key)); }
Dictionary DataReader::table(const Dictionary &d, const char *key) { return dict_of(typed(d, key, Variant::DICTIONARY)); }
std::vector<float> DataReader::floats(const Dictionary &d, const char *key) {
	const Array a = list(d, key);
	std::vector<float> out;
	DataReader r = in(key);
	for (int i = 0; i < a.size(); i++) out.push_back(float(r.num_at(a, i)));
	return out;
}
std::vector<int> DataReader::counts(const Dictionary &d, const char *key) {
	const Array a = list(d, key);
	std::vector<int> out;
	DataReader r = in(key);
	for (int i = 0; i < a.size(); i++) out.push_back(r.integer_at(a, i));
	return out;
}
uint32_t DataReader::colour(const Dictionary &d, const char *key) { return hex(typed(d, key, Variant::STRING), String(key)); }

double DataReader::num_or(const Dictionary &d, const char *key, double fallback) { return d.has(key) ? num(d, key) : fallback; }
int DataReader::integer_or(const Dictionary &d, const char *key, int fallback) { return d.has(key) ? integer(d, key) : fallback; }
bool DataReader::flag_or(const Dictionary &d, const char *key, bool fallback) { return d.has(key) ? flag(d, key) : fallback; }
Array DataReader::list_or(const Dictionary &d, const char *key) { return d.has(key) ? list(d, key) : Array(); }
Dictionary DataReader::table_or(const Dictionary &d, const char *key) { return d.has(key) ? table(d, key) : Dictionary(); }
void DataReader::expect(const Array &a, int size) { sized(a, size, String()); }

double DataReader::num_at(const Array &a, int i) { return number(typed_at(a, i, Variant::FLOAT)); }
int DataReader::integer_at(const Array &a, int i) { return whole(num_at(a, i), step_of(i)); }
String DataReader::text_at(const Array &a, int i) { return string_of(typed_at(a, i, Variant::STRING)); }
std::string DataReader::utf8_at(const Array &a, int i) { return std::string(text_at(a, i).utf8().get_data()); }
Array DataReader::list_at(const Array &a, int i, int size) { return sized(array_of(typed_at(a, i, Variant::ARRAY)), size, step_of(i)); }
Dictionary DataReader::table_at(const Array &a, int i) { return dict_of(typed_at(a, i, Variant::DICTIONARY)); }
uint32_t DataReader::colour_at(const Array &a, int i) { return hex(typed_at(a, i, Variant::STRING), step_of(i)); }

void DataReader::unknown(const std::string &s, const char *what) { fail(String("unknown ") + what + String(" '") + s.c_str() + String("'")); }

int DataReader::whole(double v, const String &key) {
	if (v == std::floor(v)) return int(v);
	fail(key + String(": expected a whole number, found ") + String::num(v));
	return int(v);
}

Array DataReader::sized(const Array &a, int size, const String &key) {
	if (size >= 0 && a.size() != size)
		fail((key.is_empty() ? String() : key + String(": ")) + String("expected ") + String::num_int64(size) + String(" items, found ") +
				String::num_int64(a.size()));
	return a; // the positional reads after it check their own index
}

uint32_t DataReader::hex(const Variant &v, const String &key) {
	if (v.get_type() != Variant::STRING) return 0; // its type error is reported already
	const std::string t(String(v).utf8().get_data());
	const bool digits = t.find_first_not_of("0123456789abcdefABCDEF", 1) == std::string::npos;
	if (!t.empty() && t[0] == '#' && (t.size() == RGB_TEXT || t.size() == RGBA_TEXT) && digits) return parse_hex_color(t);
	fail((key.is_empty() ? String() : key + String(": ")) + String("expected '#rrggbb', found '") + String(v) + String("'"));
	return 0;
}

DataValue DataReader::at(const Variant &table) const { return DataValue{ *this, table }; }

void DataValue::fail(const String &what) const {
	DataReader r = reader;
	r.fail(what);
}

Variant DataValue::typed(Variant::Type type) const {
	if (failed) return Variant();
	const Variant::Type got = value.get_type();
	if (got == type || (type == Variant::FLOAT && got == Variant::INT)) return value;
	fail(String("expected ") + Variant::get_type_name(type) + String(", found ") + Variant::get_type_name(got));
	return Variant();
}

DataValue DataValue::operator[](const char *key) const {
	if (failed) return DataValue{ reader.in(String(key)), Variant(), true };
	const Variant t = typed(Variant::DICTIONARY);
	if (t.get_type() != Variant::DICTIONARY) return DataValue{ reader.in(String(key)), Variant(), true };
	const Dictionary d = t;
	if (!d.has(key)) {
		fail(String("missing '") + key + String("'"));
		return DataValue{ reader.in(String(key)), Variant(), true };
	}
	return DataValue{ reader.in(String(key)), d[key] };
}

DataValue DataValue::operator[](int index) const {
	if (failed) return DataValue{ reader.in(index), Variant(), true };
	const Variant t = typed(Variant::ARRAY);
	if (t.get_type() != Variant::ARRAY) return DataValue{ reader.in(index), Variant(), true };
	const Array a = t;
	if (index < 0 || index >= a.size()) {
		fail(String("no item ") + String::num_int64(index) + String(" (the list has ") + String::num_int64(a.size()) + String(")"));
		return DataValue{ reader.in(index), Variant(), true };
	}
	return DataValue{ reader.in(index), a[index] };
}

bool DataValue::has(const char *key) const {
	return !failed && value.get_type() == Variant::DICTIONARY && Dictionary(value).has(key);
}

int DataValue::size() const { return int(array_of(typed(Variant::ARRAY)).size()); }
Array DataValue::keys() const { return dict_of(typed(Variant::DICTIONARY)).keys(); }
double DataValue::num() const { return number(typed(Variant::FLOAT)); }

int DataValue::whole() const {
	const double v = num();
	if (v == std::floor(v)) return int(v);
	fail(String("expected a whole number, found ") + String::num(v));
	return int(v);
}

bool DataValue::flag() const {
	const Variant v = typed(Variant::BOOL);
	return v.get_type() == Variant::BOOL && bool(v);
}

String DataValue::text() const { return string_of(typed(Variant::STRING)); }
std::string DataValue::utf8() const { return std::string(text().utf8().get_data()); }

uint32_t DataValue::colour() const {
	DataReader r = reader;
	return failed ? 0 : r.hex(typed(Variant::STRING), String());
}

Array DataValue::list(int size) const {
	const Array a = array_of(typed(Variant::ARRAY));
	if (!failed && value.get_type() == Variant::ARRAY && size >= 0 && a.size() != size)
		fail(String("expected ") + String::num_int64(size) + String(" items, found ") + String::num_int64(a.size()));
	return a;
}

Dictionary DataValue::table() const { return dict_of(typed(Variant::DICTIONARY)); }

Vector2 DataValue::v2() const {
	const Array a = list(2);
	if (a.size() != 2) return Vector2();
	return Vector2((*this)[0].f(), (*this)[1].f());
}

Vector3 DataValue::v3() const {
	const Array a = list(3);
	if (a.size() != 3) return Vector3();
	return Vector3((*this)[0].f(), (*this)[1].f(), (*this)[2].f());
}

} // namespace city
