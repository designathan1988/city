// The XPBD step of rigid.h: small steps, each one advancing the bodies freely, correcting joints, limits and ground contact once,
// then taking the velocities from the change of place (Mueller et al. 2020, algorithm 2). All constraints are hard (compliance 0).
#include "sim/physics/rigid.h"

#include <algorithm>

namespace city::phys {

namespace {
constexpr float NO_DIRECTION = 1e-9f; // below this a length or an angle has no direction
constexpr float NEGLIGIBLE_M = 1e-7f; // a gap or a penetration this small is no constraint
constexpr float SPEED_LEVER_SHARE = 1.f; // a body's rotation counts as its gyration radius times the angular speed

Q4 flipped_up(const Q4 &q) { return q.w < 0 ? Q4(-q.w, -q.x, -q.y, -q.z) : q; } // the same rotation with w >= 0
} // namespace

void Articulation::refresh() {
	for (Body &b : bodies) {
		const M3 r = b.q.matrix();
		b.inv_world = r * b.inv_inertia * r.transposed();
	}
}

float Articulation::inverse_mass_at(const Body &b, const V3 &r, const V3 &n) const {
	const V3 rn = cross(r, n);
	return b.inv_mass + dot(rn, b.inv_world * rn);
}

void Articulation::push(Body &b, const V3 &p, const V3 &r) const {
	b.x += p * b.inv_mass;
	b.q = integrate(b.q, b.inv_world * cross(r, p));
}

void Articulation::advance_body(Body &b, float h) const {
	b.v.y -= params.gravity * h; // no gyroscopic term: the bodies turn slowly, and its explicit step blew up on the thin parts
	b.x_prev = b.x;
	b.x += b.v * h;
	b.q_prev = b.q;
	b.q = integrate(b.q, b.w * h);
	const M3 n = b.q.matrix();
	b.inv_world = n * b.inv_inertia * n.transposed();
}

void Articulation::solve_joint(const Joint &j) {
	Body &p = bodies[size_t(j.parent)], &c = bodies[size_t(j.child)];
	const V3 rp = p.q.rotate(j.r_parent), rc = c.q.rotate(j.r_child);
	const V3 gap = (c.x + rc) - (p.x + rp);
	const float len = length(gap);
	if (len < NEGLIGIBLE_M) return;
	const V3 n = gap * (1.f / len);
	const float w = inverse_mass_at(p, rp, n) + inverse_mass_at(c, rc, n);
	const V3 impulse = n * (-len / w); // the child's pivot goes back toward the parent's
	push(c, impulse, rc);
	push(p, -impulse, rp);
}

void Articulation::measure(const Joint &j, Q4 &anat, float &su, float &st, float &tau) const {
	const Q4 rel = bodies[size_t(j.parent)].q.conjugate() * bodies[size_t(j.child)].q; // the child in the parent's frame
	anat = flipped_up((rel * j.offset).normalized()); // ... measured from the anatomical zero
	tau = 2.f * std::atan2(dot(V3(anat.x, anat.y, anat.z), j.ey), anat.w); // twist about ey: q = swing * twist
	const V3 d = anat.rotate(j.ey);
	const float c = std::clamp(dot(d, j.ey), -1.f, 1.f), sigma = std::acos(c);
	const V3 side = d - j.ey * c;
	const float l = length(side);
	if (l < NO_DIRECTION) {
		su = st = 0;
		return;
	}
	const V3 m = side * (1.f / l); // where the bone swings to; the swing vector is sigma along it
	su = sigma * dot(m, j.ez);
	st = sigma * -dot(m, j.ex);
}

V3 Articulation::rotate_pair(const Joint &j, const Q4 &delta) {
	const Q4 d = flipped_up(delta);
	const V3 v(d.x, d.y, d.z);
	const float s = length(v);
	if (s < NO_DIRECTION) return V3();
	Body &p = bodies[size_t(j.parent)], &c = bodies[size_t(j.child)];
	const float angle = 2.f * std::atan2(s, d.w);
	const V3 n = p.q.rotate(v * (1.f / s)); // the axis, in the world
	const float wp = dot(n, p.inv_world * n), wc = dot(n, c.inv_world * n), sum = wp + wc;
	if (sum < NO_DIRECTION) return V3();
	c.q = (axis_angle(n, angle * wc / sum) * c.q).normalized();
	p.q = (axis_angle(n, -angle * wp / sum) * p.q).normalized();
	return n;
}

void Articulation::solve_limits(size_t joint) {
	const Joint &j = joints[joint];
	Q4 qa;
	float su, st, tau;
	measure(j, qa, su, st, tau);
	const float su2 = std::clamp(su, j.flex_lo, j.flex_hi), st2 = std::clamp(st, j.lat_lo, j.lat_hi), tau2 = std::clamp(tau, j.twist_lo, j.twist_hi);
	if (su2 == su && st2 == st && tau2 == tau) return;
	const float sigma = std::sqrt(su2 * su2 + st2 * st2);
	Q4 swing2;
	if (sigma > NO_DIRECTION) {
		const V3 m = (j.ez * su2 + (-j.ex) * st2) * (1.f / sigma);
		swing2 = axis_angle(unit(cross(j.ey, m)), sigma); // the axis a with a x ey = m
	}
	const Q4 target = swing2 * axis_angle(j.ey, tau2);
	limit_axis_[joint] = rotate_pair(j, (target * qa.conjugate()).normalized());
}

// A limb that met its limit stops there: the part of the turning speed about that axis the correction gave back is taken out, shared
// by inverse inertia (an inelastic stop, as a joint's ligaments).
void Articulation::stop_at_limits() {
	for (size_t i = 0; i < joints.size(); i++) {
		const V3 n = limit_axis_[i];
		if (dot(n, n) < NO_DIRECTION) continue;
		const Joint &j = joints[i];
		Body &p = bodies[size_t(j.parent)], &c = bodies[size_t(j.child)];
		const float wp = dot(n, p.inv_world * n), wc = dot(n, c.inv_world * n), sum = wp + wc;
		if (sum < NO_DIRECTION) continue;
		const float rel = dot(c.w - p.w, n);
		c.w -= n * (rel * wc / sum);
		p.w += n * (rel * wp / sum);
	}
}

void Articulation::solve_contacts(const Ground &ground, bool record) {
	if (record) contacts_.clear();
	const float e = params.slope_span_m;
	for (const ContactPoint &pt : points) {
		Body &b = bodies[size_t(pt.body)];
		if (!b.alive) continue;
		V3 r = b.q.rotate(pt.r);
		const V3 at = b.x + r;
		const float depth = ground.height(at.x, at.z) - at.y;
		if (depth <= 0) continue;
		const V3 n = unit(V3(ground.height(at.x - e, at.z) - ground.height(at.x + e, at.z), 2.f * e, ground.height(at.x, at.z - e) - ground.height(at.x, at.z + e)));
		const float c = depth * n.y; // how deep along the surface normal
		push(b, n * (c / inverse_mass_at(b, r, n)), r);
		// static friction: where the point has gone along the ground since the step began is undone when under mu times the depth
		r = b.q.rotate(pt.r);
		const V3 moved = (b.x + r) - (b.x_prev + b.q_prev.rotate(pt.r));
		const V3 slide = moved - n * dot(moved, n);
		const float mag = length(slide);
		if (mag > NEGLIGIBLE_M && mag < params.friction * c) {
			const V3 t = slide * (1.f / mag);
			push(b, t * (-mag / inverse_mass_at(b, r, t)), r);
		}
		if (record) contacts_.push_back({ pt.body, r, n, c });
	}
}

void Articulation::velocities(float h) {
	for (Body &b : bodies) {
		if (!b.alive) continue;
		b.v = (b.x - b.x_prev) * (1.f / h);
		const Q4 dq = flipped_up(b.q * b.q_prev.conjugate());
		b.w = V3(dq.x, dq.y, dq.z) * (2.f / h);
	}
}

// Dynamic friction at the velocity level: a point sliding loses at most mu times the speed the contact gave back along the normal.
void Articulation::friction_velocity(float h) {
	for (const Contact &c : contacts_) {
		Body &b = bodies[size_t(c.body)];
		const V3 vp = b.v + cross(b.w, c.r);
		const V3 vt = vp - c.n * dot(vp, c.n);
		const float speed = length(vt);
		if (speed < NO_DIRECTION) continue;
		const V3 t = vt * (1.f / speed);
		const float dv = std::min(params.friction * c.depth / h, speed);
		const V3 impulse = t * (-dv / inverse_mass_at(b, c.r, t));
		b.v += impulse * b.inv_mass;
		b.w += b.inv_world * cross(c.r, impulse);
	}
}

V3 Articulation::point_at(const ContactPoint &p) const {
	const Body &b = bodies[size_t(p.body)];
	return b.x + b.q.rotate(p.r);
}

void Articulation::step(float dt, const Ground &ground, StepCost &cost) {
	const float h = dt / float(params.substeps);
	before_.resize(points.size());
	for (size_t i = 0; i < points.size(); i++) before_[i] = point_at(points[i]);
	limit_axis_.assign(joints.size(), V3());
	for (int s = 0; s < params.substeps; s++) {
		for (Body &b : bodies)
			if (b.alive) advance_body(b, h);
		solve_contacts(ground, true);
		cost.contacts += int(contacts_.size());
		for (int it = 0; it < params.joint_iterations; it++)
			for (size_t i = 0; i < joints.size(); i++) {
				const Joint &j = joints[i];
				if (!j.attached || !bodies[size_t(j.parent)].alive || !bodies[size_t(j.child)].alive) continue;
				solve_limits(i); // the limits first: the joint then closes the gap they open
				solve_joint(j);
			}
		solve_contacts(ground, false); // the ground once more, so the joints did not push a body into it
		velocities(h);
		stop_at_limits();
		friction_velocity(h);
		limit_axis_.assign(joints.size(), V3());
	}
	for (const Body &b : bodies) cost.bodies += b.alive ? 1 : 0;
	for (const Joint &j : joints) cost.joints += j.attached ? 1 : 0;
	cost.points_tested += int(points.size()) * params.substeps;
	cost.substeps += params.substeps;
	// at rest when no skin point travelled more than rest_speed over the frame: the average speed, which the noise of the small steps does not fake
	float moved = 0;
	for (size_t i = 0; i < points.size(); i++)
		if (bodies[size_t(points[i].body)].alive) moved = std::max(moved, length(point_at(points[i]) - before_[i]));
	quiet_s = moved < params.rest_speed * dt ? quiet_s + dt : 0.f;
}

float Articulation::fastest() const {
	float top = 0;
	for (const Body &b : bodies) {
		if (!b.alive || b.inv_mass <= 0) continue;
		const float gyration = std::sqrt((b.inertia.at(0, 0) + b.inertia.at(1, 1) + b.inertia.at(2, 2)) * b.inv_mass / 3.f);
		top = std::max(top, length(b.v) + length(b.w) * gyration * SPEED_LEVER_SHARE);
	}
	return top;
}

float Articulation::gap_of(size_t joint) const {
	const Joint &j = joints[joint];
	const Body &p = bodies[size_t(j.parent)], &c = bodies[size_t(j.child)];
	if (!j.attached || !p.alive || !c.alive) return 0.f;
	return length((c.x + c.q.rotate(j.r_child)) - (p.x + p.q.rotate(j.r_parent)));
}

float Articulation::overshoot_of(size_t joint, int &component) const {
	const Joint &j = joints[joint];
	component = 0;
	if (!j.attached || !bodies[size_t(j.parent)].alive || !bodies[size_t(j.child)].alive) return 0.f;
	Q4 qa;
	float su, st, tau;
	measure(j, qa, su, st, tau);
	const float over[3] = { std::max(j.flex_lo - su, su - j.flex_hi), std::max(j.lat_lo - st, st - j.lat_hi), std::max(j.twist_lo - tau, tau - j.twist_hi) };
	float top = 0;
	for (int k = 0; k < 3; k++)
		if (over[k] > top) {
			top = over[k];
			component = k;
		}
	return top;
}

float Articulation::worst_gap() const {
	float top = 0;
	for (size_t j = 0; j < joints.size(); j++) top = std::max(top, gap_of(j));
	return top;
}

float Articulation::worst_overshoot() const {
	float top = 0;
	int component;
	for (size_t j = 0; j < joints.size(); j++) top = std::max(top, overshoot_of(j, component));
	return top;
}

} // namespace city::phys
