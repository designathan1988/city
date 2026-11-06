// The ECMAScript built-ins a translated program calls: numbers and Math, with ECMAScript's semantics (ECMA-262, 15th edition,
// sections cited at each one). Each is checked against Node by the converter (test/js-runtime.test.ts).
#pragma once

#include <algorithm>
#include <charconv>
#include <cmath>
#include <random>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

namespace js {


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

// ECMA-262 21.3.1 and 21.1.2: the constants of Math and Number.
constexpr double PI = 3.141592653589793;
constexpr double E = 2.718281828459045;
constexpr double LN2 = 0.6931471805599453;
constexpr double LN10 = 2.302585092994046;
constexpr double LOG2E = 1.4426950408889634;
constexpr double LOG10E = 0.4342944819032518;
constexpr double SQRT2 = 1.4142135623730951;
constexpr double SQRT1_2 = 0.7071067811865476;
constexpr double NUMBER_EPSILON = 2.220446049250313e-16;
constexpr double NUMBER_MAX_VALUE = 1.7976931348623157e308;
constexpr double NUMBER_MIN_VALUE = 5e-324;
constexpr double NUMBER_MAX_SAFE_INTEGER = 9007199254740991.0;
constexpr double NUMBER_MIN_SAFE_INTEGER = -9007199254740991.0;
constexpr double NUMBER_POSITIVE_INFINITY = std::numeric_limits<double>::infinity();
constexpr double NUMBER_NEGATIVE_INFINITY = -std::numeric_limits<double>::infinity();
constexpr double NUMBER_NaN = std::numeric_limits<double>::quiet_NaN();

/** ECMA-262 21.3.2.28 Math.round: the integer closest, halves toward +Infinity (x - floor(x) is exact). */
inline double round(double x) {
	if (!std::isfinite(x)) return x;
	// [-0.5, 0) rounds to -0 (the sign is kept, as for every x)
	if (x < 0 && x >= -0.5) return -0.0;
	const double down = std::floor(x);
	return x - down >= 0.5 ? down + 1 : down;
}
/** ECMA-262 21.3.2.27 Math.random: uniform in [0, 1), no seed the program can ask for. */
inline double random() {
	static thread_local std::mt19937_64 engine{std::random_device{}()};
	return std::uniform_real_distribution<double>(0.0, 1.0)(engine);
}
/** ECMA-262 21.3.2.24 and 25 Math.max, Math.min of two reals: a NaN gives NaN, and +0 counts as above -0. */
template <class T> T math_max(T a, T b) {
	if (a != a) return a;
	if (b != b) return b;
	if (a == b) return std::signbit(a) ? b : a;
	return a < b ? b : a;
}
template <class T> T math_min(T a, T b) {
	if (a != a) return a;
	if (b != b) return b;
	if (a == b) return std::signbit(a) ? a : b;
	return b < a ? b : a;
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

} // namespace js
