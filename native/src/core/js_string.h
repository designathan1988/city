// The ECMAScript built-ins a translated program calls: strings, in UTF-16 code units as JavaScript counts, with ECMAScript's semantics (ECMA-262, 15th edition,
// sections cited at each one). Each is checked against Node by the converter (test/js-runtime.test.ts).
#pragma once

#include "core/js_number.h"
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace js {


namespace detail {

inline bool ascii(const std::string &s) {
	for (const unsigned char c : s) if (c >= 0x80) return false;
	return true;
}

inline std::u16string utf16(const std::string &s) {
	std::u16string out;
	for (size_t i = 0; i < s.size();) {
		const unsigned char c = (unsigned char)s[i];
		uint32_t cp;
		size_t len;
		if (c < 0x80) { cp = c; len = 1; }
		else if ((c >> 5) == 0x6) { cp = c & 0x1Fu; len = 2; }
		else if ((c >> 4) == 0xE) { cp = c & 0x0Fu; len = 3; }
		else { cp = c & 0x07u; len = 4; }
		for (size_t k = 1; k < len && i + k < s.size(); k++) cp = (cp << 6) | ((unsigned char)s[i + k] & 0x3Fu);
		i += len;
		if (cp >= 0x10000) {
			cp -= 0x10000;
			out.push_back(char16_t(0xD800 + (cp >> 10)));
			out.push_back(char16_t(0xDC00 + (cp & 0x3FF)));
		} else out.push_back(char16_t(cp));
	}
	return out;
}

inline std::string utf8(const std::u16string &s) {
	std::string out;
	for (size_t i = 0; i < s.size(); i++) {
		uint32_t cp = s[i];
		if (cp >= 0xD800 && cp < 0xDC00 && i + 1 < s.size() && s[i + 1] >= 0xDC00 && s[i + 1] < 0xE000) {
			cp = 0x10000 + ((cp - 0xD800) << 10) + (s[i + 1] - 0xDC00);
			i++;
		} else if (cp >= 0xD800 && cp < 0xE000) cp = 0xFFFD; // a lone surrogate: U+FFFD, as every UTF-8 encoder of JavaScript writes it
		if (cp < 0x80) out.push_back(char(cp));
		else if (cp < 0x800) { out.push_back(char(0xC0 | (cp >> 6))); out.push_back(char(0x80 | (cp & 0x3F))); }
		else if (cp < 0x10000) { out.push_back(char(0xE0 | (cp >> 12))); out.push_back(char(0x80 | ((cp >> 6) & 0x3F))); out.push_back(char(0x80 | (cp & 0x3F))); }
		else { out.push_back(char(0xF0 | (cp >> 18))); out.push_back(char(0x80 | ((cp >> 12) & 0x3F))); out.push_back(char(0x80 | ((cp >> 6) & 0x3F))); out.push_back(char(0x80 | (cp & 0x3F))); }
	}
	return out;
}

/** ECMA-262 relative index: below zero counts from the end, clamped to [0, len]. */
inline int relative(double i, int len) {
	if (std::isnan(i)) return 0;
	const double t = std::trunc(i);
	if (t < 0) return int(std::max(0.0, len + t));
	return int(std::min(t, double(len)));
}

} // namespace detail

/** String length in UTF-16 code units. */
inline int length(const std::string &s) { return detail::ascii(s) ? int(s.size()) : int(detail::utf16(s).size()); }

/** ECMA-262 22.1.3.22 String.prototype.slice. */
inline std::string slice(const std::string &s, double start, std::optional<double> end = std::nullopt) {
	if (detail::ascii(s)) {
		const int len = int(s.size());
		const int a = detail::relative(start, len), b = end ? detail::relative(*end, len) : len;
		return b > a ? s.substr(size_t(a), size_t(b - a)) : std::string();
	}
	const std::u16string u = detail::utf16(s);
	const int len = int(u.size());
	const int a = detail::relative(start, len), b = end ? detail::relative(*end, len) : len;
	return b > a ? detail::utf8(u.substr(size_t(a), size_t(b - a))) : std::string();
}

/** ECMA-262 22.1.3.24 String.prototype.substring: negative or NaN is 0, the two swapped if out of order. */
inline std::string substring(const std::string &s, double start, std::optional<double> end = std::nullopt) {
	const std::u16string u = detail::utf16(s);
	const int len = int(u.size());
	const auto clamp = [len](double v) { return std::isnan(v) ? 0 : int(std::min(std::max(std::trunc(v), 0.0), double(len))); };
	int a = clamp(start), b = end ? clamp(*end) : len;
	if (a > b) std::swap(a, b);
	return detail::utf8(u.substr(size_t(a), size_t(b - a)));
}

inline int index_of(const std::string &s, const std::string &p, double from = 0) {
	const std::u16string u = detail::utf16(s), q = detail::utf16(p);
	const size_t at = u.find(q, size_t(std::min(std::max(std::trunc(std::isnan(from) ? 0 : from), 0.0), double(u.size()))));
	return at == std::u16string::npos ? -1 : int(at);
}
inline int last_index_of(const std::string &s, const std::string &p) {
	const std::u16string u = detail::utf16(s), q = detail::utf16(p);
	const size_t at = u.rfind(q);
	return at == std::u16string::npos ? -1 : int(at);
}
inline bool starts_with(const std::string &s, const std::string &p) { return s.compare(0, p.size(), p) == 0; }
inline bool ends_with(const std::string &s, const std::string &p) { return s.size() >= p.size() && s.compare(s.size() - p.size(), p.size(), p) == 0; }
inline bool includes(const std::string &s, const std::string &p) { return s.find(p) != std::string::npos; }

/** ECMA-262 22.1.3.32 trim: the white space and line terminators of 12.2 and 12.3 (the ASCII ones and NBSP, BOM). */
inline std::string trim(const std::string &s) {
	const auto space = [&](size_t i, size_t &len) {
		const unsigned char c = (unsigned char)s[i];
		len = 1;
		if (c == ' ' || (c >= '\t' && c <= '\r')) return true;
		if (c == 0xC2 && i + 1 < s.size() && (unsigned char)s[i + 1] == 0xA0) { len = 2; return true; }
		if (c == 0xEF && i + 2 < s.size() && (unsigned char)s[i + 1] == 0xBB && (unsigned char)s[i + 2] == 0xBF) { len = 3; return true; }
		return false;
	};
	size_t a = 0, len = 0;
	while (a < s.size() && space(a, len)) a += len;
	size_t b = s.size();
	while (b > a) {
		size_t back = b - 1;
		while (back > a && ((unsigned char)s[back] & 0xC0) == 0x80) back--;
		if (!space(back, len) || back + len != b) break;
		b = back;
	}
	return s.substr(a, b - a);
}

/** toLowerCase and toUpperCase of the ASCII letters (the program's keys and labels; other letters are kept). */
inline std::string to_lower_case(std::string s) { for (auto &c : s) if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a'); return s; }
inline std::string to_upper_case(std::string s) { for (auto &c : s) if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A'); return s; }

/** ECMA-262 22.1.3.17 padStart / 22.1.3.16 padEnd: to maxLength UTF-16 units with the filler repeated, cut at the end. */
inline std::string pad(const std::string &s, double max_length, const std::string &filler, bool at_start) {
	const int len = length(s);
	const int want = int(std::trunc(max_length));
	if (want <= len || filler.empty()) return s;
	std::u16string f = detail::utf16(filler), add;
	while (int(add.size()) < want - len) add += f;
	add.resize(size_t(want - len));
	return at_start ? detail::utf8(add) + s : s + detail::utf8(add);
}
inline std::string pad_start(const std::string &s, double n, const std::string &f = " ") { return pad(s, n, f, true); }
inline std::string pad_end(const std::string &s, double n, const std::string &f = " ") { return pad(s, n, f, false); }

inline double char_code_at(const std::string &s, double i) {
	const std::u16string u = detail::utf16(s);
	const double t = std::trunc(std::isnan(i) ? 0 : i);
	return t < 0 || t >= double(u.size()) ? std::numeric_limits<double>::quiet_NaN() : double(u[size_t(t)]);
}

inline std::string repeat(const std::string &s, double n) {
	std::string out;
	for (int i = 0; i < int(n); i++) out += s;
	return out;
}

/** ECMA-262 22.1.3.23 split by a string separator (an empty one: each UTF-16 unit), up to limit pieces. */
inline std::vector<std::string> split(const std::string &s, const std::string &sep, std::optional<double> limit = std::nullopt) {
	std::vector<std::string> out;
	const size_t lim = limit ? size_t(to_uint32(*limit)) : size_t(-1);
	if (lim == 0) return out;
	if (sep.empty()) {
		const std::u16string u = detail::utf16(s);
		for (const char16_t c : u) {
			if (out.size() >= lim) break;
			out.push_back(detail::utf8(std::u16string(1, c)));
		}
		return out;
	}
	size_t at = 0;
	for (size_t next; (next = s.find(sep, at)) != std::string::npos;) {
		out.push_back(s.substr(at, next - at));
		if (out.size() >= lim) return out;
		at = next + sep.size();
	}
	out.push_back(s.substr(at));
	return out;
}

/** ECMA-262 22.1.2.1 String.fromCharCode: the UTF-16 units given. */
template <class... U>
std::string string_from_char_code(U... units) {
	std::u16string u{char16_t(to_uint32(double(units)) & 0xFFFFu)...};
	return detail::utf8(u);
}

/** String(x) of a value: a number by Number::toString, a string as it is, a flag as true or false. */
inline std::string to_string(const std::string &s) { return s; }
inline std::string to_string(const char *s) { return s; }
inline std::string to_string(bool b) { return b ? "true" : "false"; }
template <class T, std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, int> = 0>
std::string to_string(T x) {
	if constexpr (std::is_integral_v<T>) return std::to_string(x);
	else return number_to_string(x);
}

} // namespace js
