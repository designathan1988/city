// The pictures of a 3D reference (body_survey.h): a point of the model on a picture, what stands in front of it, and the colour it shows there.
#include "sim/commands/body_survey.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float HIDDEN_M = 1.2f;
constexpr int ALPHA_SEEN = 128, RGB_MAX = 255;

float dot(const SurveyPoint &a, const SurveyPoint &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
SurveyPoint minus(const SurveyPoint &a, const SurveyPoint &b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }

struct Pixel {
	float col, row, depth;
};
Pixel on_picture(const SurveyView &v, const SurveyPoint &p) {
	const SurveyPoint d = minus(p, v.centre);
	return { (dot(d, v.right) / v.half_w + 1) / 2 * float(v.w), (1 - dot(d, v.up) / v.half_h) / 2 * float(v.h), dot(d, v.out) };
}
} // namespace

// How near the model comes at each pixel, the model laid over the picture triangle by triangle: what stands in front of a face hides it.
bool survey_depth(SurveyView &v, const SurveyTriangles &tris) {
	v.nearest.assign(size_t(v.w) * size_t(v.h), -std::numeric_limits<float>::max());
	for (const SurveyTriangle &t : tris) {
		const Pixel a = on_picture(v, t[0]), b = on_picture(v, t[1]), c = on_picture(v, t[2]);
		for (const Pixel &p : {a, b, c})
			if (!std::isfinite(p.col) || !std::isfinite(p.row) || !std::isfinite(p.depth)) return false;
		const float area = (b.col - a.col) * (c.row - a.row) - (b.row - a.row) * (c.col - a.col);
		if (!std::isfinite(area)) return false;
		if (area == 0) continue;
		// Clip in floating point before narrowing; off-picture coordinates can exceed the signed integer range.
		const int x0 = int(std::clamp(std::floor(double(std::min({a.col, b.col, c.col}))), 0.0, double(v.w)));
		const int x1 = int(std::clamp(std::ceil(double(std::max({a.col, b.col, c.col}))), -1.0, double(v.w - 1)));
		const int y0 = int(std::clamp(std::floor(double(std::min({a.row, b.row, c.row}))), 0.0, double(v.h)));
		const int y1 = int(std::clamp(std::ceil(double(std::max({a.row, b.row, c.row}))), -1.0, double(v.h - 1)));
		for (int y = y0; y <= y1; y++)
			for (int x = x0; x <= x1; x++) {
				const float px = float(x) + .5f, py = float(y) + .5f;
				const float wa = ((b.col - px) * (c.row - py) - (b.row - py) * (c.col - px)) / area, wb = ((c.col - px) * (a.row - py) - (c.row - py) * (a.col - px)) / area, wc = 1 - wa - wb;
				if (wa < 0 || wb < 0 || wc < 0) continue;
				float &n = v.nearest[size_t(y) * size_t(v.w) + size_t(x)];
				n = std::max(n, wa * a.depth + wb * b.depth + wc * c.depth);
			}
	}
	return true;
}

// What the model shows at a point of its surface, seen along the normal; false for nothing there.
bool survey_sample(const std::vector<SurveyView> &views, const SurveyPoint &p, float nx, float ny, SurveyColour &out) {
	const SurveyPoint look = nx == 0 && ny == 0 ? SurveyPoint{ 0, 0, 1 } : SurveyPoint{ nx, ny, 0 };
	const SurveyView *v = &views.front();
	for (const SurveyView &o : views)
		if (dot(o.out, look) > dot(v->out, look)) v = &o;
	const Pixel at = on_picture(*v, p);
	const int col = int(at.col), row = int(at.row);
	if (at.col < 0 || at.row < 0 || col >= v->w || row >= v->h) return false;
	const size_t i = size_t(row) * size_t(v->w) + size_t(col);
	if (v->rgba[i * 4 + 3] < ALPHA_SEEN || v->nearest[i] > at.depth + HIDDEN_M) return false;
	for (int c = 0; c < 3; c++) out[size_t(c)] = float(v->rgba[i * 4 + size_t(c)]) / float(RGB_MAX);
	return true;
}

} // namespace city
