// A value whose type only the running program knows: what `unknown`, `any` and a union of different types are in the source. It holds
// what ECMAScript holds (ECMA-262 6.1) with its conversions (7.1) and strict equality (7.2.15); an array and an object are shared by
// reference, as in JavaScript. The translation uses a static C++ type wherever the program's types give one, this one where they do not.
#pragma once

#include "js_map.h"

#include <cmath>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace js {

class Value;
using Object = Map<std::string, Value>;
void to_value(); // a struct's own to_value is found by its namespace; this only names the function

class Value {
public:
	enum class Kind { Undefined, Null, Boolean, Number, String, Array, Object };

	Value() = default;
	Value(std::nullptr_t) : v_(nullptr) {}
	Value(bool b) : v_(b) {}
	template <class N, std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>, int> = 0>
	Value(N n) : v_(double(n)) {}
	Value(const char *s) : v_(std::string(s)) {}
	Value(std::string s) : v_(std::move(s)) {}
	template <class T> Value(const std::optional<T> &o) { if (o) *this = Value(*o); }
	template <class T> Value(const T *p) { if (p) *this = Value(*p); else v_ = nullptr; }
	template <class T> Value(const std::vector<T> &items);
	template <class T> Value(const Map<std::string, T> &entries);
	template <class T, class = decltype(to_value(std::declval<const T &>()))>
	Value(const T &v) : Value(to_value(v)) {}

	static Value object();
	Kind kind() const { return Kind(v_.index()); }
	bool nullish() const { return kind() == Kind::Undefined || kind() == Kind::Null; }
	bool is_array() const { return kind() == Kind::Array; }
	std::string type_of() const;
	/** ToBoolean. */
	bool truthy() const;
	explicit operator bool() const { return truthy(); }
	/** ToNumber. */
	double number() const;
	/** ToString of what the program reads as text. */
	std::string text() const;
	/** A member: undefined when it is not there; an array's and a string's length. */
	Value get(const std::string &key) const;
	Value at(double index) const;
	void set(const std::string &key, Value value);
	bool has(const std::string &key) const;
	int length() const;
	/** An array's elements, or an object's values. */
	std::vector<Value> items() const;
	std::vector<std::string> keys() const;
	/** Strict equality: an array or an object by identity. */
	bool same(const Value &o) const;

private:
	std::variant<std::monostate, std::nullptr_t, bool, double, std::string, std::shared_ptr<std::vector<Value>>, std::shared_ptr<Object>> v_;
};

template <class T> Value::Value(const std::vector<T> &items) {
	auto list = std::make_shared<std::vector<Value>>();
	for (const T &x : items) list->push_back(Value(x));
	v_ = std::move(list);
}

template <class T> Value::Value(const Map<std::string, T> &entries) {
	auto made = std::make_shared<Object>();
	for (const auto &e : entries) made->insert_or_assign(e.first, Value(e.second));
	v_ = std::move(made);
}

inline Value Value::object() {
	Value out;
	out.v_ = std::make_shared<Object>();
	return out;
}

inline std::string Value::type_of() const {
	switch (kind()) {
		case Kind::Undefined: return "undefined";
		case Kind::Boolean: return "boolean";
		case Kind::Number: return "number";
		case Kind::String: return "string";
		default: return "object";
	}
}

inline bool Value::truthy() const {
	switch (kind()) {
		case Kind::Undefined: case Kind::Null: return false;
		case Kind::Boolean: return std::get<bool>(v_);
		case Kind::Number: { const double n = std::get<double>(v_); return n != 0 && !std::isnan(n); }
		case Kind::String: return !std::get<std::string>(v_).empty();
		default: return true;
	}
}

inline double Value::number() const {
	switch (kind()) {
		case Kind::Null: return 0;
		case Kind::Boolean: return std::get<bool>(v_) ? 1 : 0;
		case Kind::Number: return std::get<double>(v_);
		case Kind::String: {
			const std::string &s = std::get<std::string>(v_);
			if (s.empty()) return 0;
			char *end = nullptr;
			const double n = std::strtod(s.c_str(), &end);
			return *end == 0 ? n : std::nan("");
		}
		default: return std::nan("");
	}
}

inline std::string Value::text() const {
	switch (kind()) {
		case Kind::Undefined: return "undefined";
		case Kind::Null: return "null";
		case Kind::Boolean: return std::get<bool>(v_) ? "true" : "false";
		case Kind::String: return std::get<std::string>(v_);
		case Kind::Number: {
			const double n = std::get<double>(v_);
			return n == std::floor(n) && std::abs(n) < 1e15 ? std::to_string((long long)n) : std::to_string(n);
		}
		default: return "[object Object]";
	}
}

inline Value Value::get(const std::string &key) const {
	if (key == "length" && (kind() == Kind::Array || kind() == Kind::String)) return length();
	if (kind() != Kind::Object) return Value();
	const auto &o = *std::get<std::shared_ptr<Object>>(v_);
	const auto it = o.find(key);
	return it == o.end() ? Value() : it->second;
}

inline Value Value::at(double index) const {
	if (kind() != Kind::Array) return Value();
	const auto &a = *std::get<std::shared_ptr<std::vector<Value>>>(v_);
	return index >= 0 && index < double(a.size()) ? a[size_t(index)] : Value();
}

inline void Value::set(const std::string &key, Value value) {
	if (kind() == Kind::Object) std::get<std::shared_ptr<Object>>(v_)->insert_or_assign(key, std::move(value));
}

inline bool Value::has(const std::string &key) const {
	return kind() == Kind::Object && std::get<std::shared_ptr<Object>>(v_)->count(key) != 0;
}

inline int Value::length() const {
	if (kind() == Kind::Array) return int(std::get<std::shared_ptr<std::vector<Value>>>(v_)->size());
	return kind() == Kind::String ? int(std::get<std::string>(v_).size()) : 0;
}

inline std::vector<Value> Value::items() const {
	if (kind() == Kind::Array) return *std::get<std::shared_ptr<std::vector<Value>>>(v_);
	std::vector<Value> out;
	if (kind() == Kind::Object) for (const auto &e : *std::get<std::shared_ptr<Object>>(v_)) out.push_back(e.second);
	return out;
}

inline std::vector<std::string> Value::keys() const {
	std::vector<std::string> out;
	if (kind() == Kind::Object) for (const auto &e : *std::get<std::shared_ptr<Object>>(v_)) out.push_back(e.first);
	return out;
}

inline bool Value::same(const Value &o) const {
	if (kind() != o.kind()) return false;
	switch (kind()) {
		case Kind::Undefined: case Kind::Null: return true;
		case Kind::Number: return std::get<double>(v_) == std::get<double>(o.v_);
		default: return v_ == o.v_;
	}
}

/** Array.isArray, Object.keys and Object.values of a value only the running program knows. */
inline bool array_is_array(const Value &v) { return v.is_array(); }
inline std::vector<std::string> object_keys(const Value &v) { return v.keys(); }
inline std::vector<Value> object_values(const Value &v) { return v.items(); }

/** A value read into a static type: what `value as T` promises the source is true. */
template <class N, std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>, int> = 0>
void read(const Value &x, N &out) { out = N(x.number()); }
inline void read(const Value &x, bool &out) { out = x.truthy(); }
inline void read(const Value &x, std::string &out) { out = x.text(); }
inline void read(const Value &x, Value &out) { out = x; }
template <class T> void read(const Value &x, std::optional<T> &out);
template <class T> void read(const Value &x, std::vector<T> &out);
template <class T> void read(const Value &x, Map<std::string, T> &out);

template <class T> void read(const Value &x, std::optional<T> &out) {
	if (x.nullish()) { out.reset(); return; }
	T held{};
	read(x, held);
	out = std::move(held);
}

template <class T> void read(const Value &x, std::vector<T> &out) {
	out.clear();
	for (const Value &item : x.items()) {
		T held{};
		read(item, held);
		out.push_back(std::move(held));
	}
}

template <class T> void read(const Value &x, Map<std::string, T> &out) {
	out.clear();
	for (const std::string &key : x.keys()) {
		T held{};
		read(x.get(key), held);
		out.insert_or_assign(key, std::move(held));
	}
}

template <class T> T as(const Value &x) {
	T out{};
	read(x, out);
	return out;
}

inline bool operator==(const Value &a, const Value &b) { return a.same(b); }
inline bool operator!=(const Value &a, const Value &b) { return !a.same(b); }

} // namespace js

/** A running value as a key of a Map. */
template <> struct std::hash<js::Value> {
	size_t operator()(const js::Value &v) const { return std::hash<std::string>()(v.text()); }
};
