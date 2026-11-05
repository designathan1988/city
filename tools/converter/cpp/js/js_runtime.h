// The ECMAScript built-ins a translated program calls, with ECMAScript's semantics: what Number.prototype.toFixed,
// String.prototype.slice or a Map's iteration order give in JavaScript, these give in C++ (ECMA-262, 15th edition,
// sections cited at each one). The translator writes every built-in the same way, as a call here; each is checked
// against Node by the converter's runtime differential (test/js-runtime.test.ts). Header only, C++17.
#pragma once

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace js {

// ---------------------------------------------------------------- numbers

/** ECMA-262 21.1.3.3: toFixed prints with ToString at or above 10^21. */
constexpr double FIXED_LIMIT = 1e21;
/** ECMA-262 21.1.3.3: fractionDigits is 0..100. */
constexpr int FIXED_MAX_DIGITS = 100;
/** ECMA-262 6.1.6.1.20: decimal notation for exponents n with -6 < n <= 21, scientific outside. */
constexpr int PLAIN_MAX_EXPONENT = 21;
constexpr int PLAIN_MIN_EXPONENT = -6;
/** The decimal base, and the base of one limb of the exact integers below. */
constexpr uint32_t DECIMAL = 10;
constexpr uint64_t LIMB = uint64_t(1) << 32;
constexpr uint32_t LIMB_DECIMAL = 1000000000; // the largest power of ten in a limb, for printing
constexpr int LIMB_DECIMAL_DIGITS = 9;
constexpr int MANTISSA_BITS = 53;
constexpr int LIMB_BITS = 32;

namespace detail {

/** An exact non-negative integer, little-endian limbs: what toFixed needs to round a double exactly. */
struct Big {
	std::vector<uint32_t> limb;
	explicit Big(uint64_t v) {
		while (v) {
			limb.push_back(uint32_t(v % LIMB));
			v /= LIMB;
		}
	}
	void mul(uint32_t m) {
		uint64_t carry = 0;
		for (auto &l : limb) {
			const uint64_t p = uint64_t(l) * m + carry;
			l = uint32_t(p % LIMB);
			carry = p / LIMB;
		}
		if (carry) limb.push_back(uint32_t(carry));
	}
	void shl(int k) {
		const int words = k / LIMB_BITS, bits = k % LIMB_BITS;
		if (bits) {
			uint32_t carry = 0;
			for (auto &l : limb) {
				const uint32_t next = l >> (LIMB_BITS - bits);
				l = (l << bits) | carry;
				carry = next;
			}
			if (carry) limb.push_back(carry);
		}
		limb.insert(limb.begin(), size_t(words), 0u);
	}
	bool bit(int k) const {
		const size_t w = size_t(k / LIMB_BITS);
		return w < limb.size() && ((limb[w] >> (k % LIMB_BITS)) & 1u);
	}
	void shr(int k) {
		const size_t words = size_t(k / LIMB_BITS);
		const int bits = k % LIMB_BITS;
		if (words >= limb.size()) {
			limb.clear();
			return;
		}
		limb.erase(limb.begin(), limb.begin() + std::ptrdiff_t(words));
		if (bits) {
			for (size_t i = 0; i < limb.size(); i++) {
				const uint32_t hi = i + 1 < limb.size() ? limb[i + 1] << (LIMB_BITS - bits) : 0u;
				limb[i] = (limb[i] >> bits) | hi;
			}
		}
		while (!limb.empty() && limb.back() == 0) limb.pop_back();
	}
	void add_one() {
		for (auto &l : limb) {
			if (++l != 0) return;
		}
		limb.push_back(1);
	}
	std::string decimal() const {
		std::vector<uint32_t> n = limb;
		std::string out;
		while (!n.empty()) {
			uint64_t rem = 0;
			for (size_t i = n.size(); i-- > 0;) {
				const uint64_t cur = rem * LIMB + n[i];
				n[i] = uint32_t(cur / LIMB_DECIMAL);
				rem = cur % LIMB_DECIMAL;
			}
			while (!n.empty() && n.back() == 0) n.pop_back();
			for (int d = 0; d < LIMB_DECIMAL_DIGITS; d++) {
				out.push_back(char('0' + rem % DECIMAL));
				rem /= DECIMAL;
				if (n.empty() && rem == 0) break;
			}
		}
		if (out.empty()) out = "0";
		std::reverse(out.begin(), out.end());
		return out;
	}
};

/** The shortest digits that read back as v, and the decimal exponent n (v = 0.d1d2... x 10^n), for a finite v > 0. */
template <class T>
void shortest(T v, std::string &digits, int &n) {
	char buf[64];
	const auto r = std::to_chars(buf, buf + sizeof(buf), v, std::chars_format::scientific);
	const std::string s(buf, r.ptr);
	const size_t e = s.find('e');
	for (size_t i = 0; i < e; i++) if (s[i] != '.') digits.push_back(s[i]);
	while (digits.size() > 1 && digits.back() == '0') digits.pop_back();
	n = std::atoi(s.c_str() + e + 1) + 1;
}

} // namespace detail

/**
 * ECMA-262 6.1.6.1.20 Number::toString(x) in base 10: the shortest digits that read back as x, in decimal notation
 * for -6 < n <= 21, else d.ddde+n. A float (the target's world number) is printed by its own shortest digits.
 */
template <class T>
std::string number_to_string(T x) {
	if (std::isnan(x)) return "NaN";
	if (x == 0) return "0";
	if (std::isinf(x)) return x < 0 ? "-Infinity" : "Infinity";
	std::string sign = x < 0 ? "-" : "";
	std::string d;
	int n = 0;
	detail::shortest(x < 0 ? -x : x, d, n);
	const int k = int(d.size());
	if (k <= n && n <= PLAIN_MAX_EXPONENT) return sign + d + std::string(size_t(n - k), '0');
	if (0 < n && n <= PLAIN_MAX_EXPONENT) return sign + d.substr(0, size_t(n)) + "." + d.substr(size_t(n));
	if (PLAIN_MIN_EXPONENT < n && n <= 0) return sign + "0." + std::string(size_t(-n), '0') + d;
	const int e = n - 1;
	const std::string exp = std::string("e") + (e < 0 ? "-" : "+") + std::to_string(e < 0 ? -e : e);
	return sign + d.substr(0, 1) + (k > 1 ? "." + d.substr(1) : "") + exp;
}

/**
 * ECMA-262 21.1.3.3 Number.prototype.toFixed(f): the integer n with n / 10^f - x closest to zero, the larger of two
 * (halves away from zero), computed exactly on the double's binary value; ToString at or above 10^21.
 */
inline std::string to_fixed(double x, double fraction_digits = 0) {
	const int f = int(std::min(std::max(std::trunc(std::isnan(fraction_digits) ? 0 : fraction_digits), 0.0), double(FIXED_MAX_DIGITS)));
	if (std::isnan(x)) return "NaN";
	if (std::fabs(x) >= FIXED_LIMIT || std::isinf(x)) return number_to_string(x);
	const bool negative = x < 0;
	const double a = negative ? -x : x;
	int e = 0;
	const double m = std::frexp(a, &e); // a = m * 2^e, 0.5 <= m < 1
	const uint64_t mant = uint64_t(std::ldexp(m, MANTISSA_BITS));
	e -= MANTISSA_BITS; // a = mant * 2^e exactly
	detail::Big big(mant);
	for (int i = 0; i < f; i++) big.mul(DECIMAL);
	if (e >= 0) big.shl(e);
	else {
		const bool up = big.bit(-e - 1);
		big.shr(-e);
		if (up) big.add_one();
	}
	std::string digits = big.decimal();
	if (f > 0) {
		if (int(digits.size()) <= f) digits = std::string(size_t(f + 1 - int(digits.size())), '0') + digits;
		digits.insert(digits.size() - size_t(f), ".");
	}
	// -0.00 is printed "-0.00" by JavaScript only when x < 0 (x = -0 gives "0.00")
	return (negative ? "-" : "") + digits;
}

inline bool number_is_integer(double x) { return std::isfinite(x) && std::trunc(x) == x; }
inline bool number_is_finite(double x) { return std::isfinite(x); }
inline bool number_is_nan(double x) { return std::isnan(x); }
inline bool number_is_safe_integer(double x) { return number_is_integer(x) && std::fabs(x) <= 9007199254740991.0; }

// ---------------------------------------------------------------- Math

/** ECMA-262 21.3.2.28 Math.round: the integer closest, halves toward +Infinity (x - floor(x) is exact). */
inline double round(double x) {
	if (!std::isfinite(x)) return x;
	// [-0.5, 0) rounds to -0 (the sign is kept, as for every x)
	if (x < 0 && x >= -0.5) return -0.0;
	const double down = std::floor(x);
	return x - down >= 0.5 ? down + 1 : down;
}
inline double sign(double x) { return std::isnan(x) ? x : x > 0 ? 1 : x < 0 ? -1 : x; }
inline double fround(double x) { return double(float(x)); }
/** ECMA-262 7.1.6 ToInt32: truncated, modulo 2^32, as a signed 32-bit integer. */
inline int32_t to_int32(double x) {
	if (!std::isfinite(x)) return 0;
	const double t = std::fmod(std::trunc(x), 4294967296.0);
	return int32_t(uint32_t(int64_t(t < 0 ? t + 4294967296.0 : t)));
}
inline uint32_t to_uint32(double x) { return uint32_t(to_int32(x)); }
inline int32_t imul(double a, double b) { return int32_t(to_uint32(a) * to_uint32(b)); }
inline int clz32(double x) {
	uint32_t v = to_uint32(x);
	int n = 0;
	for (uint32_t bit = 1u << 31; bit && !(v & bit); bit >>= 1) n++;
	return n;
}

// ---------------------------------------------------------------- strings (UTF-16 code units, as JavaScript counts)

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

// ---------------------------------------------------------------- arrays (std::vector)

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

// ---------------------------------------------------------------- Map and Set: insertion order (ECMA-262 24.1, 24.2)

/**
 * A Map as JavaScript's: entries in insertion order (a key set again keeps its place; deleted and set again, it goes
 * last), found by hash. A delete leaves a hole that iteration skips, so deleting while iterating is safe as in
 * JavaScript; the holes are compacted by an insertion once they outnumber the entries.
 */
template <class K, class V>
class Map {
public:
	using value_type = std::pair<K, V>;

	class iterator {
	public:
		iterator(Map *m, size_t i) : m_(m), i_(i) { skip(); }
		value_type &operator*() const { return *m_->slots_[i_]; }
		value_type *operator->() const { return &*m_->slots_[i_]; }
		iterator &operator++() { i_++; skip(); return *this; }
		bool operator==(const iterator &o) const { return i_ == o.i_; }
		bool operator!=(const iterator &o) const { return i_ != o.i_; }
	private:
		// the end is read each step: an entry set while iterating is visited, as JavaScript visits it
		void skip() { while (i_ < m_->slots_.size() && !m_->slots_[i_]) i_++; if (i_ >= m_->slots_.size()) i_ = END; }
		Map *m_;
		size_t i_;
		friend class Map;
	};
	using const_iterator = iterator;
	static constexpr size_t END = size_t(-1);

	iterator begin() const { return iterator(const_cast<Map *>(this), 0); }
	iterator end() const { return iterator(const_cast<Map *>(this), END); }
	int size() const { return int(index_.size()); }
	bool empty() const { return index_.empty(); }
	size_t count(const K &k) const { return index_.count(k); }
	iterator find(const K &k) const {
		const auto it = index_.find(k);
		return it == index_.end() ? end() : iterator(const_cast<Map *>(this), it->second);
	}
	V &operator[](const K &k) {
		const auto it = index_.find(k);
		if (it != index_.end()) return slots_[it->second]->second;
		return insert_new(k, V{});
	}
	void insert_or_assign(const K &k, V v) {
		const auto it = index_.find(k);
		if (it != index_.end()) slots_[it->second]->second = std::move(v);
		else insert_new(k, std::move(v));
	}
	size_t erase(const K &k) {
		const auto it = index_.find(k);
		if (it == index_.end()) return 0;
		slots_[it->second].reset();
		index_.erase(it);
		return 1;
	}
	void clear() { slots_.clear(); index_.clear(); }
	void reserve(size_t n) { slots_.reserve(n); index_.reserve(n); }

private:
	V &insert_new(const K &k, V v) {
		if (slots_.size() > 2 * index_.size() + COMPACT_SLACK) compact();
		index_.emplace(k, slots_.size());
		slots_.emplace_back(std::in_place, k, std::move(v));
		return slots_.back()->second;
	}
	void compact() {
		size_t w = 0;
		for (size_t r = 0; r < slots_.size(); r++) {
			if (!slots_[r]) continue;
			if (w != r) slots_[w] = std::move(slots_[r]);
			index_[slots_[w]->first] = w;
			w++;
		}
		slots_.resize(w);
	}
	static constexpr size_t COMPACT_SLACK = 32;
	std::vector<std::optional<value_type>> slots_;
	std::unordered_map<K, size_t> index_;
};

/** A Set as JavaScript's: members in insertion order, found by hash (a Map of members to nothing). */
template <class T>
class Set {
	struct None {};
public:
	class iterator {
	public:
		explicit iterator(typename Map<T, None>::iterator it) : it_(it) {}
		const T &operator*() const { return it_->first; }
		const T *operator->() const { return &it_->first; }
		iterator &operator++() { ++it_; return *this; }
		bool operator!=(const iterator &o) const { return it_ != o.it_; }
		bool operator==(const iterator &o) const { return it_ == o.it_; }
	private:
		typename Map<T, None>::iterator it_;
	};
	iterator begin() const { return iterator(m_.begin()); }
	iterator end() const { return iterator(m_.end()); }
	int size() const { return m_.size(); }
	bool empty() const { return m_.empty(); }
	size_t count(const T &x) const { return m_.count(x); }
	/** add: true when it was not there (the member keeps its place otherwise). */
	bool insert(const T &x) {
		if (m_.count(x)) return false;
		m_[x];
		return true;
	}
	size_t erase(const T &x) { return m_.erase(x); }
	void clear() { m_.clear(); }
	void reserve(size_t n) { m_.reserve(n); }
private:
	Map<T, None> m_;
};

} // namespace js
