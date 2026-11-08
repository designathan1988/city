// A design of its own as the walls a fall cuts (signature_bodies.h ruin_faces): its cooked faces placed, each corner painted by its look,
// the colour times what its picture shows round the corner, read from a small copy of the picture (the fracture interpolates between
// corners, so a picture's detail finer than a piece would only flicker across it). The glass goes apart, to be drawn dark and broken; what
// a picture cuts (railings, leaves) does not come down as a wall. A face cooked with its other side (a twin on the same three corners:
// a roof seen from above and from under) goes apart too, since it needs no back of the fracture's.
#include "render/buildings/signature_bodies.h"

#include "render/buildings/shell.h"
#include "render/buildings/signature_file.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/color.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <unordered_map>

using namespace godot;

namespace city {

using namespace signature_file;

namespace {
constexpr int PAINT_CELLS = 8; // the picture's copy a corner is painted from: its cells along each side
constexpr float WELD_PER_M = 1000.f; // corners closer than a millimetre are the same corner: a face's twin is on its very corners
constexpr size_t KEY_NUMBERS = 9; // a face's key: its three corners, three numbers each
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL, FNV_PRIME = 1099511628211ULL; // FNV-1a, 64 bits

// The colour a look gives round a uv: the record's colour times its picture's cell there, in display space as the shell's colours
// are (building_body.gdshaderinc to_linear), the alpha the surface kind of a shell face (0).
struct Paint {
	Color base; // linear
	Ref<Image> cells; // the picture's copy, none for a look with no picture

	Color at(const Vector2 &uv) const {
		Color c = base;
		if (cells.is_valid()) {
			const int x = std::clamp(int((uv.x - std::floor(uv.x)) * float(PAINT_CELLS)), 0, PAINT_CELLS - 1);
			const int y = std::clamp(int((uv.y - std::floor(uv.y)) * float(PAINT_CELLS)), 0, PAINT_CELLS - 1);
			c *= cells->get_pixel(x, y).srgb_to_linear();
		}
		Color out = c.linear_to_srgb();
		out.a = 0.f;
		return out;
	}
};

Paint paint_of(const LookRecord &rec, Ref<Image> picture) {
	Paint p;
	p.base = Color(rec.v[RED], rec.v[RED + 1], rec.v[RED + 2]).srgb_to_linear();
	if (picture.is_valid()) {
		picture->resize(PAINT_CELLS, PAINT_CELLS, Image::INTERPOLATE_TRILINEAR); // through its mipmaps: each cell the mean of what it covers
		p.cells = picture;
	}
	return p;
}

// A corner placed and painted, and a face of three.
struct Placed {
	Vector3 p, n;
	Color c;
};
using Face = std::array<Placed, 3>;

// A face's three corners, welded and in order, whatever their winding: its twin has the same.
using FaceKey = std::array<int64_t, KEY_NUMBERS>;
FaceKey key_of(const Face &f) {
	std::array<std::array<int64_t, 3>, 3> c;
	for (int k = 0; k < 3; k++)
		c[size_t(k)] = { std::llround(f[size_t(k)].p.x * WELD_PER_M), std::llround(f[size_t(k)].p.y * WELD_PER_M), std::llround(f[size_t(k)].p.z * WELD_PER_M) };
	std::sort(c.begin(), c.end());
	return { c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2] };
}
struct FaceHash {
	size_t operator()(const FaceKey &k) const {
		uint64_t h = FNV_OFFSET; // over the nine numbers
		for (int64_t v : k) h = (h ^ uint64_t(v)) * FNV_PRIME;
		return size_t(h);
	}
};

void put(b40::Shell &into, const Face &f) {
	const int32_t base = int32_t(into.pos.size());
	for (const Placed &c : f) {
		into.pos.push_back(c.p);
		into.nrm.push_back(c.n);
		into.col.push_back(c.c);
		into.uv.push_back(Vector2());
		into.uv2.push_back(Vector2()); // no finish (shell.h: UV2.x is the finish + 1)
	}
	for (int32_t k = 0; k < 3; k++) into.idx.push_back(base + k);
}
} // namespace

bool SignatureBodies::ruin_faces(int kind, const Transform3D &at, b40::Shell &walls, b40::Shell &glass, b40::Shell &sided) {
	if (kind <= 0 || kind > b40::SIGNATURE_COUNT) return false;
	std::vector<RawSurface> raw;
	if (!read_body(kind, raw)) return false;
	std::unordered_map<uint32_t, Paint> paints; // by look
	std::vector<Face> faces;
	std::vector<bool> panes;
	for (const RawSurface &sf : raw) {
		const LookRecord rec = record_at(table_, looks_[sf.look].at);
		if (rec.v[ALPHA_CUT] > 0.f) continue;
		const bool pane = (rec.flags & (FLAG_BLENDED | FLAG_WINDOWS)) != 0;
		auto paint = paints.find(sf.look);
		if (paint == paints.end()) paint = paints.emplace(sf.look, paint_of(rec, picture(sf.look))).first;
		for (int64_t i = 0; i + 2 < sf.idx.size(); i += 3) {
			Face f;
			for (int k = 0; k < 3; k++) {
				const int32_t v = sf.idx[i + k];
				f[size_t(k)] = Placed{ at.xform(sf.pos[v]), at.basis.xform(sf.nor[v]).normalized(), paint->second.at(sf.uv[v]) };
			}
			faces.push_back(f);
			panes.push_back(pane);
		}
	}
	std::unordered_map<FaceKey, int, FaceHash> seen;
	for (size_t i = 0; i < faces.size(); i++)
		if (!panes[i]) seen[key_of(faces[i])]++;
	for (size_t i = 0; i < faces.size(); i++) put(panes[i] ? glass : seen[key_of(faces[i])] > 1 ? sided : walls, faces[i]);
	return true;
}

} // namespace city
