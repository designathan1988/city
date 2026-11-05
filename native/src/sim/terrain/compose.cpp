// The final heights composed from the base heights, the modifiers and the ground shaped to the roads (heightfield.h), and the
// held composition of a bulk edit: while a hold is open the rectangles wait, and when the last one is let go they are merged
// into blocks and each block is composed once, so n inserts cost n and not n times everything that overlaps them.
#include "sim/terrain/heightfield.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int COMPOSE_BLOCK = 64; // samples per side of a block of the held composition (256 m at 4 m spacing)
constexpr float EDGE_M = 0.01f; // regions of the pieces overlap this much: a sample on an edge, rounded, is never left out
constexpr float PIECE_MIN_M = 1e-4f; // a piece (or a half-plane normal component) shorter than this has no direction

struct Half { // the half-plane nx * x + nz * z <= c
	float nx, nz, c;
};
} // namespace

void Heightfield::hold() { held++; }

void Heightfield::release(bool compose) {
	if (held <= 0 || --held > 0) return;
	std::vector<SampleRect> rects;
	rects.swap(waiting);
	if (!compose || rects.empty() || N <= 0) return;
	const int nb = (N + COMPOSE_BLOCK - 1) / COMPOSE_BLOCK;
	std::vector<uint8_t> dirty(size_t(nb) * size_t(nb), 0);
	size_t rect_samples = 0, block_samples = 0;
	for (const SampleRect &r : rects) {
		rect_samples += size_t(r.x1 - r.x0 + 1) * size_t(r.z1 - r.z0 + 1);
		const int bx0 = r.x0 / COMPOSE_BLOCK, bx1 = r.x1 / COMPOSE_BLOCK, bz0 = r.z0 / COMPOSE_BLOCK, bz1 = r.z1 / COMPOSE_BLOCK;
		for (int bz = bz0; bz <= bz1; bz++)
			for (int bx = bx0; bx <= bx1; bx++) {
				uint8_t &b = dirty[size_t(bz) * size_t(nb) + size_t(bx)];
				block_samples += b ? 0 : size_t(COMPOSE_BLOCK) * COMPOSE_BLOCK;
				b = 1;
			}
	}
	if (rect_samples <= block_samples) { // few rectangles that hardly overlap (one edit): each one as it came
		for (const SampleRect &r : rects) recompose(r.x0, r.z0, r.x1, r.z1);
		return;
	}
	for (int bz = 0; bz < nb; bz++)
		for (int bx = 0; bx < nb;) { // each run of dirty blocks along a row is one rectangle
			if (!dirty[size_t(bz) * size_t(nb) + size_t(bx)]) {
				bx++;
				continue;
			}
			const int start = bx;
			while (bx < nb && dirty[size_t(bz) * size_t(nb) + size_t(bx)]) bx++;
			recompose(start * COMPOSE_BLOCK, bz * COMPOSE_BLOCK, std::min(N, bx * COMPOSE_BLOCK) - 1, std::min(N, (bz + 1) * COMPOSE_BLOCK) - 1);
		}
}

void Heightfield::recompose_world_rect(float minx, float minz, float maxx, float maxz) {
	recompose(ix_of(minx) - 1, ix_of(minz) - 1, ix_of(maxx) + 1, ix_of(maxz) + 1);
}

void Heightfield::recompose(int x0, int z0, int x1, int z1) {
	x0 = clampi(x0, 0, N - 1); x1 = clampi(x1, 0, N - 1);
	z0 = clampi(z0, 0, N - 1); z1 = clampi(z1, 0, N - 1);
	if (x0 > x1 || z0 > z1) return;
	if (held > 0) {
		waiting.push_back({ x0, z0, x1, z1 });
		return;
	}
	for (int z = z0; z <= z1; z++)
		for (int x = x0; x <= x1; x++) final_h[size_t(z) * N + x] = dequant(base_q[size_t(z) * N + x]);
	std::vector<float> best_w(size_t(x1 - x0 + 1) * (z1 - z0 + 1), 0.f);
	float minx = x_of(x0), maxx = x_of(x1), minz = x_of(z0), maxz = x_of(z1);
	std::vector<int> ids;
	mod_hash.query(minx, minz, maxx, maxz, [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
	for (int id : ids)
		if (mods[id].alive) apply_modifier(mods[id], x0, z0, x1, z1, best_w);
	ids.clear();
	shape_hash.query(minx, minz, maxx, maxz, [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
	for (int id : ids)
		if (road_shapes[size_t(id)].alive) apply_road_shape(road_shapes[size_t(id)], x0, z0, x1, z1, best_w);
	if (log) log->terrain_rects.insert(log->terrain_rects.end(), { x0, z0, x1, z1 });
}

// Each piece visits only the samples whose nearest point of the polyline can lie on it: the slab along it, the wedge
// at the vertex where it meets the next (outside both slabs: the vertex is the nearest point there) and the caps past the
// ends. The regions cover every sample within `reach` of the polyline once (and their edges twice, in order), so the
// work is the length times the reach, and each sample still learns the nearest piece, the first of equals.
const std::vector<NearSample> &Heightfield::nearest(const std::vector<float> &pts, float reach, int ax0, int az0, int ax1, int az1) const {
	thread_local std::vector<NearSample> near;
	thread_local std::vector<V2> dir;
	const int w = ax1 - ax0 + 1;
	near.assign(size_t(w) * size_t(az1 - az0 + 1), NearSample());
	const size_t np = pts.size() / 3;
	if (np < 2) return near;
	const size_t pieces = np - 1;
	auto at = [&](size_t k) { return V2(pts[k * 3], pts[k * 3 + 2]); };
	dir.assign(pieces, V2());
	for (size_t i = 0; i < pieces; i++) {
		const V2 d = at(i + 1) - at(i);
		const float l = d.len();
		if (l > PIECE_MIN_M) dir[i] = d * (1.f / l); // a piece shorter than that has no direction: its disc covers it
	}
	const float r = reach + EDGE_M;
	auto visit = [&](size_t i, int x, int z) {
		const float *a = &pts[i * 3], *b = a + 3;
		float t;
		const float d = dist_point_segment(V2(x_of(x), x_of(z)), V2(a[0], a[2]), V2(b[0], b[2]), &t);
		NearSample &n = near[size_t(z - az0) * size_t(w) + size_t(x - ax0)];
		if (d < n.d) {
			n.d = d;
			n.surface = lerpf(a[1], b[1], t);
		}
	};
	// The samples of a convex region: half-planes n.p <= c and, if `disc`, the circle of radius r around `o`.
	auto scan = [&](size_t i, const Half *h, int nh, bool disc, const V2 &o, float minx, float minz, float maxx, float maxz) {
		const int z0 = std::max(az0, ix_of(minz)), z1 = std::min(az1, ix_of(maxz) + 1);
		for (int z = z0; z <= z1; z++) {
			const float zw = x_of(z);
			float lo = std::max(minx, x_of(ax0)), hi = std::min(maxx, x_of(ax1));
			for (int k = 0; k < nh && lo <= hi; k++) {
				const float rest = h[k].c - h[k].nz * zw;
				if (std::fabs(h[k].nx) < PIECE_MIN_M) {
					if (rest < 0.f) lo = hi + 1.f;
				} else if (h[k].nx > 0.f) {
					hi = std::min(hi, rest / h[k].nx);
				} else {
					lo = std::max(lo, rest / h[k].nx);
				}
			}
			if (disc) {
				const float dz = zw - o.y, q = r * r - dz * dz;
				if (q < 0.f) continue;
				lo = std::max(lo, o.x - std::sqrt(q));
				hi = std::min(hi, o.x + std::sqrt(q));
			}
			if (lo > hi) continue;
			const int x0 = std::max(ax0, int(std::ceil(lo / spacing + N * 0.5f))), x1 = std::min(ax1, int(std::floor(hi / spacing + N * 0.5f)));
			for (int x = x0; x <= x1; x++) visit(i, x, z);
		}
	};
	auto round = [&](size_t i, const V2 &o, const Half *h, int nh) { scan(i, h, nh, true, o, o.x - r, o.y - r, o.x + r, o.y + r); };
	for (size_t i = 0; i < pieces; i++) {
		const V2 a = at(i), b = at(i + 1), u = dir[i];
		if (u.len2() == 0.f) {
			round(i, a, nullptr, 0);
			continue;
		}
		const V2 n = u.perp();
		const Half slab[4] = { { -u.x, -u.y, -u.dot(a) + EDGE_M }, { u.x, u.y, u.dot(b) + EDGE_M },
			{ n.x, n.y, n.dot(a) + r }, { -n.x, -n.y, -n.dot(a) + r } };
		scan(i, slab, 4, false, a, std::min(a.x, b.x) - r, std::min(a.y, b.y) - r, std::max(a.x, b.x) + r, std::max(a.y, b.y) + r);
		if (i == 0) {
			const Half cap[1] = { { u.x, u.y, u.dot(a) + EDGE_M } };
			round(i, a, cap, 1);
		}
		if (i + 1 == pieces) {
			const Half cap[1] = { { -u.x, -u.y, -u.dot(b) + EDGE_M } };
			round(i, b, cap, 1);
		} else if (dir[i + 1].len2() > 0.f) { // a next piece with no direction covers the vertex with its disc
			const V2 v = dir[i + 1];
			const Half wedge[2] = { { -u.x, -u.y, -u.dot(b) + EDGE_M }, { v.x, v.y, v.dot(b) + EDGE_M } };
			round(i, b, wedge, 2);
		}
	}
	return near;
}

void Heightfield::apply_modifier(const TerrainModifier &m, int x0, int z0, int x1, int z1, std::vector<float> &best_w) {
	const int ax0 = std::max(x0, ix_of(m.minx)), ax1 = std::min(x1, ix_of(m.maxx) + 1);
	const int az0 = std::max(z0, ix_of(m.minz)), az1 = std::min(z1, ix_of(m.maxz) + 1);
	if (ax0 > ax1 || az0 > az1) return;
	const bool pad = m.pts.size() / 3 == 1; // one point: a round pad, the piece from it to itself
	const std::vector<float> pad_piece = pad ? std::vector<float>{ m.pts[0], m.pts[1], m.pts[2], m.pts[0], m.pts[1], m.pts[2] } : std::vector<float>();
	const std::vector<NearSample> &near = nearest(pad ? pad_piece : m.pts, m.flat + m.max_run, ax0, az0, ax1, az1);
	const int W = x1 - x0 + 1, AW = ax1 - ax0 + 1;
	for (int z = az0; z <= az1; z++)
		for (int x = ax0; x <= ax1; x++) {
			const NearSample &n = near[size_t(z - az0) * size_t(AW) + size_t(x - ax0)];
			if (n.d >= BIG) continue;
			const size_t gi = size_t(z) * N + x;
			const float base = dequant(base_q[gi]);
			const float run = clampf(std::fabs(base - n.surface) * tune->terrain_embankment_ratio, tune->terrain_min_run_m, m.max_run);
			float w = n.d <= m.flat ? 1.f : 1.f - (n.d - m.flat) / run;
			if (w <= 0.f) continue;
			w = smooth01(w);
			const size_t li = size_t(z - z0) * W + (x - x0);
			if (w > best_w[li]) {
				best_w[li] = w;
				final_h[gi] = lerpf(base, n.surface - tune->terrain_road_sink_m, w); // terrain sits just under the road surface
			}
		}
}

} // namespace city
