// The measure of a species' model (see drawn_body.h): each point once, a card's leaves as a reach round its centre.
#include "render/trees/drawn_body.h"

#include "render/trees/tree_part.h"

#include <algorithm>
#include <cmath>
#include <tuple>

using namespace godot;

namespace city {

namespace {
constexpr float CORNER = 1.41421356f; // a card's corner stands this many half sides from its centre: its leaves reach no further
constexpr float NO_SIZE = 1e-6f; // a coordinate under this gives a scale nothing to hold

bool is_card(const Vector2 &tag) { return int(std::lround(tag.x)) == int(TreePart::CARD); }

auto key(const DrawnPoint &p) { return std::tie(p.x, p.y, p.z, p.reach); }
} // namespace

void DrawnBody::measure(const MeshBuilder &body, const MeshBuilder &cards, float tuft) {
	points_.clear();
	faces_x_ = 0;
	for (int i = 0; i < body.v.size(); i++) {
		const Vector3 p = body.v[i];
		points_.push_back({ p.x, p.y, p.z, 0 });
		faces_x_ = std::max(faces_x_, std::fabs(p.x));
	}
	for (int i = 0; i < cards.v.size(); i++) {
		const Vector3 p = cards.v[i];
		const Vector2 tag = cards.uv2[i];
		// a card's side is twice its half; its leaves stand `tuft` of that side from its centre, and never past its corner
		const float reach = is_card(tag) ? tag.y * std::min(2 * tuft, CORNER) : 0;
		points_.push_back({ p.x, p.y, p.z, reach });
	}
	// a face's corner is written once a face and a card's centre four times: each point once
	std::sort(points_.begin(), points_.end(), [](const DrawnPoint &a, const DrawnPoint &b) { return key(a) < key(b); });
	points_.erase(std::unique(points_.begin(), points_.end(), [](const DrawnPoint &a, const DrawnPoint &b) { return key(a) == key(b); }), points_.end());
}

float DrawnBody::back_x(float sx) const {
	float m = 0;
	for (const DrawnPoint &p : points_) m = std::max(m, (p.reach - p.x) * sx);
	return m;
}

float DrawnBody::fore_x(float sx) const {
	float m = 0;
	for (const DrawnPoint &p : points_) m = std::max(m, (p.reach + p.x) * sx);
	return m;
}

float DrawnBody::half_z(float sx, float sz) const {
	float m = 0;
	for (const DrawnPoint &p : points_) m = std::max(m, std::fabs(p.z) * sz + p.reach * sx);
	return m;
}

float DrawnBody::around(float sx, float sz) const {
	float m = 0;
	for (const DrawnPoint &p : points_) m = std::max(m, std::hypot(p.x * sx, p.z * sz) + p.reach * sx);
	return m;
}

float DrawnBody::top(float sx, float sy) const {
	float m = 0;
	for (const DrawnPoint &p : points_) m = std::max(m, p.y * sy + p.reach * sx);
	return m;
}

float DrawnBody::deep_within(float sx, float half) const {
	float most = half / NO_SIZE;
	for (const DrawnPoint &p : points_) {
		const float room = half - p.reach * sx;
		if (room <= 0) return 0;
		if (std::fabs(p.z) > NO_SIZE) most = std::min(most, room / std::fabs(p.z));
	}
	return most;
}

float DrawnBody::tall_within(float sx, float top) const {
	float most = top / NO_SIZE;
	for (const DrawnPoint &p : points_) {
		const float room = top - p.reach * sx;
		if (room <= 0) return 0;
		if (p.y > NO_SIZE) most = std::min(most, room / p.y);
	}
	return most;
}

} // namespace city
