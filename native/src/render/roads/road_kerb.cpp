// The kerbed edge round a corner or a turning bulb, band by band from the inner line out to the lot.
#include "render/roads/road_mesh.h"

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);

// One band of a kerbed edge between two sections (prev, cur): kerb face, stone top, 1 cm step, footway, skirt. `run` is how far
// along the kerb a section stands, what its faces and its top read, as on a segment.
struct Section {
	Vector3 i, k, o, n;
	float run;
};

void band(const RoadEmit &e, const Section &a, const Section &b, float across, bool kerbed, const Color &walk) {
	const StreetLook &L = e.L;
	float top = kerbed ? L.h_curb : L.h_sidewalk, sh = L.h_sidewalk;
	Vector3 up_t(0, top, 0), up_s(0, sh, 0), down(0, -L.skirt, 0);
	Vector3 ak = kerbed ? a.k : a.i, bk = kerbed ? b.k : b.i;
	const Vector2 none[4] = { Vector2(POOL_NONE, 0), Vector2(POOL_NONE, 0), Vector2(POOL_NONE, 0), Vector2(POOL_NONE, 0) };
	auto xz = [](const Vector3 &p) { return Vector2(p.x, p.z); };
	auto face = [&](const Vector3 p0, const Vector3 p1, const Vector3 p2, const Vector3 p3, const Color &c, const Vector3 &out) {
		const Vector3 p[4] = { p0, p1, p2, p3 };
		const Vector2 uv[4] = { Vector2(a.run, 0), Vector2(b.run, 0), Vector2(b.run, 0), Vector2(a.run, 0) };
		emit_quad(e.mb, p, uv, none, road_color(c, RoadMat::SKIRT), out);
	};
	face(a.i, b.i, b.i + up_t, a.i + up_t, L.curb, a.n);
	if (kerbed) {
		const Vector3 p[4] = { a.i + up_t, b.i + up_t, b.k + up_t, a.k + up_t };
		const float wa = (a.k - a.i).length(), wb = (b.k - b.i).length();
		const Vector2 uv[4] = { Vector2(across, a.run), Vector2(across, b.run), Vector2(across + wb, b.run), Vector2(across + wa, a.run) };
		emit_quad(e.mb, p, uv, none, road_color(L.curb, RoadMat::KERB), UP);
		face(a.k + up_s, b.k + up_s, b.k + up_t, a.k + up_t, L.curb, -a.n);
	}
	{ // the footway round the corner: no kerb at the inner line, the lot at the outer
		const float wa = (a.o - ak).length(), wb = (b.o - bk).length();
		const Vector3 p[4] = { ak + up_s, bk + up_s, b.o + up_s, a.o + up_s };
		const Vector2 uv[4] = { xz(p[0]) + Vector2(0, p[0].y), xz(p[1]) + Vector2(0, p[1].y), xz(p[2]) + Vector2(0, p[2].y), xz(p[3]) + Vector2(0, p[3].y) };
		const Color cols[4] = { wear_color(0, wa, 0, RoadMat::FOOTWAY, 0), wear_color(0, wb, 0, RoadMat::FOOTWAY, 0),
				wear_color(wb, 0, 0, RoadMat::FOOTWAY, 0), wear_color(wa, 0, 0, RoadMat::FOOTWAY, 0) };
		emit_quad(e.mb, p, uv, none, cols, UP);
	}
	face(a.o + down, b.o + down, b.o + up_s, a.o + up_s, walk, -a.n);
}
} // namespace

void kerbed_line(const RoadEmit &e, const std::vector<Vector3> &in, const std::vector<Vector3> &bk, const std::vector<Vector3> &out,
		float across, bool kerbed, const Color &walk) {
	Section prev;
	float run = 0;
	for (size_t k = 0; k < in.size(); k++) {
		Vector3 nn = in[k] - out[k];
		nn.y = 0;
		nn = nn.length() > 0 ? nn.normalized() : UP;
		if (k > 0) run += (in[k] - in[k - 1]).length();
		Section cur{ in[k], bk[k], out[k], nn, run };
		if (k > 0) band(e, prev, cur, across, kerbed, walk);
		prev = cur;
	}
}

} // namespace city
