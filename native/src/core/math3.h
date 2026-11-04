// Three-dimensional vectors, quaternions and 3x3 matrices of the rigid bodies (sim/physics): plain single-precision structs, no
// Godot, so a body's motion is the same function of its inputs on every machine. Right-handed, +Y up.
#pragma once

#include <cmath>

namespace city::phys {

struct V3 {
	float x = 0, y = 0, z = 0;
	V3() = default;
	V3(float a, float b, float c) : x(a), y(b), z(c) {}
	V3 operator+(const V3 &o) const { return { x + o.x, y + o.y, z + o.z }; }
	V3 operator-(const V3 &o) const { return { x - o.x, y - o.y, z - o.z }; }
	V3 operator*(float k) const { return { x * k, y * k, z * k }; }
	V3 operator-() const { return { -x, -y, -z }; }
	V3 &operator+=(const V3 &o) { x += o.x; y += o.y; z += o.z; return *this; }
	V3 &operator-=(const V3 &o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
	float &operator[](int i) { return i == 0 ? x : i == 1 ? y : z; }
	float operator[](int i) const { return i == 0 ? x : i == 1 ? y : z; }
};

inline float dot(const V3 &a, const V3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline V3 cross(const V3 &a, const V3 &b) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }
inline float length(const V3 &a) { return std::sqrt(dot(a, a)); }
// The unit vector, or `fallback` when `a` is too short to have a direction.
inline V3 unit(const V3 &a, const V3 &fallback = V3(0, 1, 0)) {
	const float l = length(a);
	return l > 1e-9f ? a * (1.f / l) : fallback;
}

struct M3 {
	float m[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 }; // row-major
	float &at(int r, int c) { return m[r * 3 + c]; }
	float at(int r, int c) const { return m[r * 3 + c]; }
	V3 operator*(const V3 &v) const {
		return { m[0] * v.x + m[1] * v.y + m[2] * v.z, m[3] * v.x + m[4] * v.y + m[5] * v.z, m[6] * v.x + m[7] * v.y + m[8] * v.z };
	}
	M3 operator*(const M3 &o) const {
		M3 r;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) r.at(i, j) = at(i, 0) * o.at(0, j) + at(i, 1) * o.at(1, j) + at(i, 2) * o.at(2, j);
		return r;
	}
	M3 transposed() const {
		M3 r;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) r.at(i, j) = at(j, i);
		return r;
	}
};

// The inverse of a symmetric positive matrix (the adjugate over the determinant); the identity scaled to zero when singular.
inline M3 inverse(const M3 &a) {
	const float c00 = a.at(1, 1) * a.at(2, 2) - a.at(1, 2) * a.at(2, 1), c01 = a.at(1, 2) * a.at(2, 0) - a.at(1, 0) * a.at(2, 2),
				c02 = a.at(1, 0) * a.at(2, 1) - a.at(1, 1) * a.at(2, 0);
	const float det = a.at(0, 0) * c00 + a.at(0, 1) * c01 + a.at(0, 2) * c02;
	M3 r;
	if (std::fabs(det) < 1e-24f) {
		for (float &v : r.m) v = 0;
		return r;
	}
	const float k = 1.f / det;
	r.at(0, 0) = c00 * k;
	r.at(0, 1) = (a.at(0, 2) * a.at(2, 1) - a.at(0, 1) * a.at(2, 2)) * k;
	r.at(0, 2) = (a.at(0, 1) * a.at(1, 2) - a.at(0, 2) * a.at(1, 1)) * k;
	r.at(1, 0) = c01 * k;
	r.at(1, 1) = (a.at(0, 0) * a.at(2, 2) - a.at(0, 2) * a.at(2, 0)) * k;
	r.at(1, 2) = (a.at(0, 2) * a.at(1, 0) - a.at(0, 0) * a.at(1, 2)) * k;
	r.at(2, 0) = c02 * k;
	r.at(2, 1) = (a.at(0, 1) * a.at(2, 0) - a.at(0, 0) * a.at(2, 1)) * k;
	r.at(2, 2) = (a.at(0, 0) * a.at(1, 1) - a.at(0, 1) * a.at(1, 0)) * k;
	return r;
}

struct Q4 {
	float w = 1, x = 0, y = 0, z = 0;
	Q4() = default;
	Q4(float a, float b, float c, float d) : w(a), x(b), y(c), z(d) {}
	Q4 operator*(const Q4 &o) const {
		return { w * o.w - x * o.x - y * o.y - z * o.z, w * o.x + x * o.w + y * o.z - z * o.y, w * o.y - x * o.z + y * o.w + z * o.x,
			w * o.z + x * o.y - y * o.x + z * o.w };
	}
	Q4 conjugate() const { return { w, -x, -y, -z }; }
	Q4 normalized() const {
		const float l = std::sqrt(w * w + x * x + y * y + z * z);
		return l > 1e-12f ? Q4(w / l, x / l, y / l, z / l) : Q4();
	}
	// The vector rotated by this unit quaternion.
	V3 rotate(const V3 &v) const {
		const V3 u(x, y, z);
		const V3 t = cross(u, v) * 2.f;
		return v + t * w + cross(u, t);
	}
	M3 matrix() const {
		M3 r;
		r.at(0, 0) = 1 - 2 * (y * y + z * z);
		r.at(0, 1) = 2 * (x * y - z * w);
		r.at(0, 2) = 2 * (x * z + y * w);
		r.at(1, 0) = 2 * (x * y + z * w);
		r.at(1, 1) = 1 - 2 * (x * x + z * z);
		r.at(1, 2) = 2 * (y * z - x * w);
		r.at(2, 0) = 2 * (x * z - y * w);
		r.at(2, 1) = 2 * (y * z + x * w);
		r.at(2, 2) = 1 - 2 * (x * x + y * y);
		return r;
	}
};

// The unit quaternion that turns `axis` (unit) by `angle` radians.
inline Q4 axis_angle(const V3 &axis, float angle) {
	const float s = std::sin(angle * 0.5f);
	return { std::cos(angle * 0.5f), axis.x * s, axis.y * s, axis.z * s };
}

// The smallest rotation taking the unit vector `a` onto the unit vector `b` (a half turn about any axis perpendicular to a when opposite).
inline Q4 shortest_arc(const V3 &a, const V3 &b) {
	const float d = dot(a, b);
	if (d < -0.999999f) {
		const V3 p = std::fabs(a.x) < 0.9f ? cross(a, V3(1, 0, 0)) : cross(a, V3(0, 1, 0));
		return axis_angle(unit(p), 3.14159265358979f);
	}
	const V3 c = cross(a, b);
	return Q4(1.f + d, c.x, c.y, c.z).normalized();
}

// q advanced by the rotation vector `w * dt / 2` (the first-order update of the XPBD paper), unit again.
inline Q4 integrate(const Q4 &q, const V3 &rot) {
	const Q4 d = Q4(0, rot.x, rot.y, rot.z) * q;
	return Q4(q.w + 0.5f * d.w, q.x + 0.5f * d.x, q.y + 0.5f * d.y, q.z + 0.5f * d.z).normalized();
}

} // namespace city::phys
