// 2D vector math in the XZ plane (V2.y is world Z). Pure C++, usable from any thread.
#pragma once

#include "core/plane_geometry.h"

#include <algorithm>
#include <cmath>

namespace city {

// The circle's constants, to more digits than a double holds, and a unit square's diagonal.
constexpr double PI_D = 3.14159265358979323846;
constexpr double TAU_D = 2 * PI_D;
constexpr float PI_F = 3.14159265358979323846f;
constexpr float TAU_F = 6.28318530717958647692f;
constexpr float SQRT2_F = 1.41421356237309504880f;
// A vector this short (a length or a squared length, metres) is a point: it has no direction.
constexpr float POINT_EPS = 1e-9f;
// Below this cross product of their directions two lines are parallel (line_line), or two segments (segments_cross).
constexpr float PARALLEL_CROSS = 1e-7f, SHARE_PARALLEL = 1e-6f;
// A meeting this near either end of a segment (a share of its length) touches the end: segments_cross is not a crossing.
constexpr float END_MARGIN = 0.02f;
// std::floor as an int, inline: on the x86-64 baseline std::floor is a library call, paid per cell of a raster.
inline int ifloor(double v) {
	const int i = int(v);
	return i - (v < double(i) ? 1 : 0);
}

constexpr double NO_ROOM = 1e300; // "nothing yet" for the double searches of the generators
constexpr float BIG = 1e9f; // "no value yet" for min searches and unbounded gaps

struct V2 {
	float x = 0, y = 0;
	V2() = default;
	V2(float px, float py) : x(px), y(py) {}
	V2 operator+(const V2 &o) const { return V2(x + o.x, y + o.y); }
	V2 operator-(const V2 &o) const { return V2(x - o.x, y - o.y); }
	V2 operator*(float s) const { return V2(x * s, y * s); }
	V2 operator/(float s) const { return V2(x / s, y / s); }
	V2 operator-() const { return V2(-x, -y); }
	V2 &operator+=(const V2 &o) { x += o.x; y += o.y; return *this; }
	V2 &operator-=(const V2 &o) { x -= o.x; y -= o.y; return *this; }
	float dot(const V2 &o) const { return x * o.x + y * o.y; }
	float cross(const V2 &o) const { return x * o.y - y * o.x; }
	float len() const { return std::sqrt(x * x + y * y); }
	float len2() const { return x * x + y * y; }
	V2 norm() const { float l = len(); return l > POINT_EPS ? V2(x / l, y / l) : V2(1, 0); }
	// Right-hand normal: for a heading in the XZ plane this points to the driver's right side.
	V2 perp() const { return V2(-y, x); }
	float angle() const { return std::atan2(y, x); }
};

inline V2 v2_from_angle(float a) { return V2(std::cos(a), std::sin(a)); }

// A point or step of the ground plane in doubles, as the generators, the lots and the road gestures place them (world x and z).
// An aggregate: XZ{ x, z } builds one.
struct XZ {
	double x = 0, z = 0;
	XZ operator+(XZ o) const { return { x + o.x, z + o.z }; }
	XZ operator-(XZ o) const { return { x - o.x, z - o.z }; }
	XZ operator*(double k) const { return { x * k, z * k }; }
	double dot(XZ o) const { return x * o.x + z * o.z; }
};
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }
inline V2 lerp_v2(const V2 &a, const V2 &b, float t) { return a + (b - a) * t; }
inline float smooth01(float t) { t = clampf(t, 0.f, 1.f); return t * t * (3.f - 2.f * t); }
inline float wrap_angle(float a) {
	while (a > PI_F) a -= TAU_F;
	while (a < -PI_F) a += TAU_F;
	return a;
}

inline float dist_point_segment(const V2 &p, const V2 &a, const V2 &b, float *out_t = nullptr) {
	V2 ab = b - a;
	float l2 = ab.len2();
	float t = l2 > POINT_EPS ? clampf((p - a).dot(ab) / l2, 0.f, 1.f) : 0.f;
	if (out_t) *out_t = t;
	return (p - (a + ab * t)).len();
}

// Intersection of the infinite lines p + t*d and q + s*e. Returns false when parallel.
inline bool line_line(const V2 &p, const V2 &d, const V2 &q, const V2 &e, float &t, float &s) {
	float den = d.cross(e);
	if (std::fabs(den) < PARALLEL_CROSS) return false;
	V2 w = q - p;
	t = w.cross(e) / den;
	s = w.cross(d) / den;
	return true;
}

// Proper intersection of segments ab and cd (endpoints excluded by a small margin).
inline bool segments_cross(const V2 &a, const V2 &b, const V2 &c, const V2 &d, float *out_t = nullptr) {
	float t, u;
	if (!line_shares(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y, SHARE_PARALLEL, t, u)) return false;
	if (out_t) *out_t = t;
	constexpr float FAR_END = 1.f - END_MARGIN;
	return t > END_MARGIN && t < FAR_END && u > END_MARGIN && u < FAR_END;
}

} // namespace city
