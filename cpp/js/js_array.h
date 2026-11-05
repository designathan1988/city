// The ECMAScript built-ins a translated program calls: arrays (std::vector), with ECMAScript's semantics (ECMA-262, 15th edition,
// sections cited at each one). Each is checked against Node by the converter (test/js-runtime.test.ts).
#pragma once

#include "js_string.h"
#include <algorithm>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace js {

namespace detail {
template <class T> struct is_optional : std::false_type {};
template <class T> struct is_optional<std::optional<T>> : std::true_type {};
} // namespace detail

/** ECMA-262 23.1.3.15 Array.prototype.join: each element by String, null and undefined as empty. */
template <class T>
std::string join(const std::vector<T> &v, const std::string &sep = ",") {
	std::string out;
	for (size_t i = 0; i < v.size(); i++) {
		if (i) out += sep;
		if constexpr (detail::is_optional<T>::value) { if (v[i]) out += to_string(*v[i]); }
		else out += to_string(v[i]);
	}
	return out;
}

/** ECMA-262 23.1.3.31 splice: removes count from start (relative), inserts items there; the removed. */
template <class T, class... U>
std::vector<T> splice(std::vector<T> &v, double start, std::optional<double> count = std::nullopt, U &&...more) {
	std::vector<T> items{T(std::forward<U>(more))...};
	const int len = int(v.size());
	const int a = detail::relative(start, len);
	const int n = count ? std::min(std::max(int(std::trunc(std::isnan(*count) ? 0 : *count)), 0), len - a) : len - a;
	std::vector<T> removed(v.begin() + a, v.begin() + a + n);
	v.erase(v.begin() + a, v.begin() + a + n);
	v.insert(v.begin() + a, std::make_move_iterator(items.begin()), std::make_move_iterator(items.end()));
	return removed;
}

template <class T>
std::optional<T> pop(std::vector<T> &v) {
	if (v.empty()) return std::nullopt;
	T last = std::move(v.back());
	v.pop_back();
	return last;
}
template <class T>
std::optional<T> shift(std::vector<T> &v) {
	if (v.empty()) return std::nullopt;
	T first = std::move(v.front());
	v.erase(v.begin());
	return first;
}
template <class T, class... U>
int unshift(std::vector<T> &v, U &&...more) {
	std::vector<T> items{T(std::forward<U>(more))...};
	v.insert(v.begin(), std::make_move_iterator(items.begin()), std::make_move_iterator(items.end()));
	return int(v.size());
}

/** indexOf: strict equality (NaN is never found); includes: SameValueZero (NaN is found). */
template <class T, class U>
int index_of(const std::vector<T> &v, const U &x, double from = 0) {
	for (int i = detail::relative(from, int(v.size())); i < int(v.size()); i++) if (v[size_t(i)] == x) return i;
	return -1;
}
template <class T, class U>
int last_index_of(const std::vector<T> &v, const U &x) {
	for (int i = int(v.size()) - 1; i >= 0; i--) if (v[size_t(i)] == x) return i;
	return -1;
}
template <class T, class U>
bool includes(const std::vector<T> &v, const U &x) {
	for (const auto &e : v) {
		if (e == x) return true;
		if constexpr (std::is_floating_point_v<T> && std::is_floating_point_v<U>) if (std::isnan(e) && std::isnan(x)) return true;
	}
	return false;
}

template <class T, class F>
int find_index(const std::vector<T> &v, F &&test) {
	for (size_t i = 0; i < v.size(); i++) if (test(v[i])) return int(i);
	return -1;
}
template <class T, class F>
int find_last_index(const std::vector<T> &v, F &&test) {
	for (size_t i = v.size(); i-- > 0;) if (test(v[i])) return int(i);
	return -1;
}

template <class T, class... More>
std::vector<T> concat(std::vector<T> a, const More &...more) {
	(a.insert(a.end(), more.begin(), more.end()), ...);
	return a;
}

/** flat() of depth 1. */
template <class T>
std::vector<T> flat(const std::vector<std::vector<T>> &v) {
	std::vector<T> out;
	for (const auto &x : v) out.insert(out.end(), x.begin(), x.end());
	return out;
}

/** sort with a comparator: stable (ECMA-262 23.1.3.30), a before b when compare(a, b) < 0. */
template <class T, class F>
std::vector<T> &sort(std::vector<T> &v, F &&compare) {
	std::stable_sort(v.begin(), v.end(), [&](const T &a, const T &b) { return compare(a, b) < 0; });
	return v;
}
/** sort with no comparator: by the elements' strings (ECMA-262 23.1.3.30.2 SortCompare), UTF-16 order. */
template <class T>
std::vector<T> &sort(std::vector<T> &v) {
	std::stable_sort(v.begin(), v.end(), [](const T &a, const T &b) { return detail::utf16(to_string(a)) < detail::utf16(to_string(b)); });
	return v;
}

} // namespace js
