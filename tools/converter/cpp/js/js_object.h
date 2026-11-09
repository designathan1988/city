// The ECMAScript Object built-ins over a dictionary (a TypeScript Record, a js::Map of text keys): Object.keys, values,
// entries and fromEntries, with the order an object lists its keys (ECMA-262, 15th edition, sections cited at each one).
// Each is checked against Node by the converter (test/js-runtime.test.ts).
#pragma once

#include "js_map.h"
#include "js_number.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace js {

/** The largest array index, 2^32 - 2, and its digits (ECMA-262 6.1.7: an array index is an integer index up to 2^32 - 2). */
constexpr uint64_t MAX_ARRAY_INDEX = 4294967294ULL;
constexpr size_t ARRAY_INDEX_DIGITS = 10;

/** The array index a key is (a canonical numeric string of an integer 0 to 2^32 - 2: no sign, no leading zero), or none. */
inline std::optional<uint64_t> array_index(const std::string &k) {
	if (k.empty() || k.size() > ARRAY_INDEX_DIGITS || (k.size() > 1 && k[0] == '0')) return std::nullopt;
	uint64_t v = 0;
	for (const char c : k) {
		if (c < '0' || c > '9') return std::nullopt;
		v = v * 10 + uint64_t(c - '0');
	}
	if (v > MAX_ARRAY_INDEX) return std::nullopt;
	return v;
}

/**
 * A dictionary's entries in the order an object lists its keys (ECMA-262 10.1.11.1 OrdinaryOwnPropertyKeys): the array
 * indices in ascending numeric order first, then every other key in the order it was made.
 */
template <class V>
std::vector<const std::pair<std::string, V> *> property_order(const Map<std::string, V> &m) {
	std::vector<std::pair<uint64_t, const std::pair<std::string, V> *>> indices;
	std::vector<const std::pair<std::string, V> *> named;
	for (const auto &e : m) {
		if (const auto at = array_index(e.first)) indices.emplace_back(*at, &e);
		else named.push_back(&e);
	}
	std::sort(indices.begin(), indices.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
	std::vector<const std::pair<std::string, V> *> ordered;
	ordered.reserve(indices.size() + named.size());
	for (const auto &i : indices) ordered.push_back(i.second);
	ordered.insert(ordered.end(), named.begin(), named.end());
	return ordered;
}

/** Object.keys, Object.values and Object.entries of a dictionary, in the order an object lists its keys. */
template <class V>
std::vector<std::string> object_keys(const Map<std::string, V> &m) {
	std::vector<std::string> out;
	for (const auto *e : property_order(m)) out.push_back(e->first);
	return out;
}
template <class V>
std::vector<V> object_values(const Map<std::string, V> &m) {
	std::vector<V> out;
	for (const auto *e : property_order(m)) out.push_back(e->second);
	return out;
}
template <class V>
std::vector<std::pair<std::string, V>> object_entries(const Map<std::string, V> &m) {
	std::vector<std::pair<std::string, V>> out;
	for (const auto *e : property_order(m)) out.push_back(*e);
	return out;
}

/** A property key as JavaScript makes it: a text is itself, a number its text (ECMA-262 7.1.19 ToPropertyKey). */
template <class K>
std::string property_key(const K &k) {
	if constexpr (std::is_arithmetic_v<K>) return number_to_string(double(k));
	else return std::string(k);
}

/** Object.fromEntries(pairs) (ECMA-262 20.1.2.7): each [key, value] set in order; a key set again keeps its first place. */
template <class Range>
auto object_from_entries(const Range &pairs) {
	using V = std::decay_t<decltype(std::declval<const Range &>().begin()->second)>;
	Map<std::string, V> out;
	for (const auto &p : pairs) out.insert_or_assign(property_key(p.first), p.second);
	return out;
}

} // namespace js
