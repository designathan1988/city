// A path off the lanes (manoeuvre.h): the legs sampled by arc length, the speed along them and the pose at any point.
#include "sim/traffic/manoeuvre.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {

constexpr int SAMPLES = 28; // per leg: a leg is a few metres to a few tens of metres long
constexpr float H_DERIV = 6.f; // the derivative of the Hermite basis: 6 u^2 - 6 u
constexpr float DONE_EPS = 1e-4f, UNIT_EPS = 1e-6f;
constexpr float RAMP_START = 0.15f, RAMP_END = 0.1f; // shares of top speed at a leg's ends (a leg always ends)

V2 unit_or(const V2 &v, const V2 &fallback) {
	const float l = v.len();
	return l > UNIT_EPS ? v * (1.f / l) : fallback;
}

// Forward legs through `points`, leaving the first along `t0` and arriving at the last along `t1`.
void through(const std::vector<V2> &points, const V2 &t0, const V2 &t1, std::vector<ManoeuvreLeg> &legs) {
	auto dir_at = [&](size_t i) {
		if (i == 0) return t0;
		if (i + 1 == points.size()) return t1;
		const V2 u = unit_or(points[i] - points[i - 1], t0), v = unit_or(points[i + 1] - points[i], t1);
		return unit_or(u + v, v);
	};
	for (size_t i = 0; i + 1 < points.size(); i++) legs.push_back({ points[i], dir_at(i), points[i + 1], dir_at(i + 1), false });
}

} // namespace

Manoeuvre::Manoeuvre(const std::vector<ManoeuvreLeg> &legs, float tangent) {
	float at = 0;
	for (size_t i = 0; i < legs.size(); i++) {
		const ManoeuvreLeg &g = legs[i];
		const float k = tangent * (g.p1 - g.p0).len();
		float px = g.p0.x, pz = g.p0.y;
		for (int j = 0; j <= SAMPLES; j++) {
			const float u = float(j) / SAMPLES, u2 = u * u, u3 = u2 * u;
			const float h00 = 2 * u3 - 3 * u2 + 1, h10 = u3 - 2 * u2 + u, h01 = -2 * u3 + 3 * u2, h11 = u3 - u2;
			const float x = h00 * g.p0.x + h10 * k * g.t0.x + h01 * g.p1.x + h11 * k * g.t1.x;
			const float z = h00 * g.p0.y + h10 * k * g.t0.y + h01 * g.p1.y + h11 * k * g.t1.y;
			// the tangent of the curve (the basis differentiated): the direction of motion here
			const float d00 = H_DERIV * (u2 - u), d10 = 3 * u2 - 4 * u + 1, d01 = -d00, d11 = 3 * u2 - 2 * u;
			const float dx = d00 * g.p0.x + d10 * k * g.t0.x + d01 * g.p1.x + d11 * k * g.t1.x;
			const float dz = d00 * g.p0.y + d10 * k * g.t0.y + d01 * g.p1.y + d11 * k * g.t1.y;
			float motion = dx == 0 && dz == 0 ? std::atan2(g.t0.y, g.t0.x) : std::atan2(dz, dx);
			if (j > 0) at += std::hypot(x - px, z - pz);
			else if (i > 0) continue; // the joint is the previous leg's last sample
			px = x;
			pz = z;
			samples_.push_back({ x, z, g.reverse ? motion + PI_F : motion, at });
		}
		ends_.push_back(at);
		reverse_.push_back(g.reverse);
	}
	length_ = at;
}

Manoeuvre::Manoeuvre(const std::vector<OffStep> &steps) {
	float at = 0;
	for (size_t i = 0; i < steps.size(); i++) {
		const OffStep &q = steps[i];
		if (i > 0) at += std::hypot(q.x - steps[i - 1].x, q.z - steps[i - 1].z);
		samples_.push_back({ q.x, q.z, q.angle, at });
		if (i + 1 == steps.size() || steps[i + 1].reverse != q.reverse) {
			ends_.push_back(at);
			reverse_.push_back(q.reverse);
		}
	}
	length_ = at;
}

float Manoeuvre::at_step(int i) const {
	return samples_.empty() ? 0.f : samples_[size_t(std::clamp(i, 0, int(samples_.size()) - 1))].at;
}

bool Manoeuvre::done() const { return s >= length_ - DONE_EPS; }

int Manoeuvre::leg_now() const {
	int i = 0;
	while (i + 1 < int(ends_.size()) && s >= ends_[size_t(i)] - DONE_EPS) i++;
	return i;
}

float Manoeuvre::speed(const ManoeuvreSpeeds &v) const {
	if (ends_.empty()) return 0;
	const int i = leg_now();
	const float start = i == 0 ? 0.f : ends_[size_t(i - 1)], end = ends_[size_t(i)];
	const float top = reverse_[size_t(i)] ? v.backward : v.forward;
	const float ramp = std::min({ 1.f, (s - start) / v.ramp + RAMP_START, (end - s) / v.ramp + RAMP_END });
	return std::max(v.creep, top * std::max(0.f, ramp));
}

void Manoeuvre::advance(float ds) { s = std::min(length_, s + std::max(0.f, ds)); }

OffPose Manoeuvre::pose_at(float at) const {
	if (samples_.empty()) return {};
	size_t lo = 0, hi = samples_.size() - 1;
	while (hi - lo > 1) {
		const size_t mid = (lo + hi) / 2;
		if (samples_[mid].at <= at) lo = mid;
		else hi = mid;
	}
	const Sample &a = samples_[lo], &b = samples_[hi];
	const float t = b.at > a.at ? clampf((at - a.at) / (b.at - a.at), 0.f, 1.f) : 0.f;
	return { a.x + (b.x - a.x) * t, a.z + (b.z - a.z) * t, a.angle + wrap_angle(b.angle - a.angle) * t };
}

bool across_footway(const V2 &edge, const V2 &p, const V2 &t, float into, float least, V2 &out) {
	const float along = (edge - p).dot(t);
	if (along > -least) return false;
	const V2 q = p + t * along, n = edge - q;
	if (n.len() < UNIT_EPS) return false;
	out = q + n.norm() * into;
	return true;
}

Manoeuvre bay_departure(const V2 &at, const V2 &open, const std::vector<V2> &via, const V2 &p, const V2 &t, float lead, float into, float least,
		float tangent) {
	const V2 aisle = via.empty() ? at + open * lead : via.front();
	std::vector<V2> ahead = via.empty() ? std::vector<V2>{ aisle } : via;
	V2 across;
	if (!via.empty() && across_footway(via.back(), p, t, into, least, across)) ahead.push_back(across);
	ahead.push_back(p);
	const V2 d = unit_or(ahead[1] - aisle, t);
	std::vector<ManoeuvreLeg> legs{ { at, open, aisle, d * -1.f, true } };
	through(ahead, d, t, legs);
	return Manoeuvre(legs, tangent);
}

Manoeuvre bay_arrival(const V2 &from, float angle, const V2 &at, const V2 &open, const std::vector<V2> &via, float lead, float into, float least,
		float tangent) {
	const V2 h(std::cos(angle), std::sin(angle));
	std::vector<V2> inward(via.rbegin(), via.rend());
	const V2 aisle = inward.empty() ? at + open * lead : inward.back();
	std::vector<V2> points{ from };
	V2 across;
	if (!inward.empty() && across_footway(inward.front(), from, h * -1.f, into, least, across)) points.push_back(across);
	if (inward.empty()) points.push_back(aisle);
	else points.insert(points.end(), inward.begin(), inward.end());
	std::vector<ManoeuvreLeg> legs;
	through(points, h, open * -1.f, legs);
	legs.push_back({ aisle, open * -1.f, at, open * -1.f, false });
	return Manoeuvre(legs, tangent);
}

} // namespace city
