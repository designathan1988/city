// Draws the tree pictures (see tree_pictures.h): a small software rasteriser with a depth buffer. The picture's frame is
// square, `share` of the tree's height across, centred on the middle of the tree, seen along the elevation asked; x runs
// right, y up the view, z towards the eye, the same as view space, so the shader takes the normal as it is.
#include "render/trees/tree_pictures.h"

#include "core/math.h"
#include "core/units.h"
#include "render/trees/tree_kit.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr int RGBA = 4;
constexpr float BYTE = 255.f, CENTRE_Y = 0.5f, NORMAL_HALF = 0.5f, ALPHA_CUT = 0.5f;
constexpr int CARD_CORNERS = 4, TRI = 3;
constexpr float CARD_BULGE = 0.6f; // a card leans outwards from its centre, so a tuft reads as a lump and not a disc

// A vertex tone under an atlas texel, as the near level multiplies them: both in linear (vegetation_cards.gdshader), the
// atlas being a source_color texture there and a plain image here.
Color leaf_tone(const Color &tone, const Color &leaf) {
	Color c = (tone.srgb_to_linear() * leaf.srgb_to_linear().r).linear_to_srgb();
	c.a = 1;
	return c;
}
} // namespace

// Calls fn(x, y, w0, w1, w2) for the pixels of a projected triangle, inside the frame, with their barycentric weights.
template <typename F>
void each_pixel(const Vector3 p[TRI], int px, F fn) {
	float area = (p[1].x - p[0].x) * (p[2].y - p[0].y) - (p[1].y - p[0].y) * (p[2].x - p[0].x);
	if (std::fabs(area) < std::numeric_limits<float>::epsilon()) return;
	int x0 = clampi(int(std::floor(std::min({ p[0].x, p[1].x, p[2].x }))), 0, px - 1), x1 = clampi(int(std::ceil(std::max({ p[0].x, p[1].x, p[2].x }))), 0, px - 1);
	int y0 = clampi(int(std::floor(std::min({ p[0].y, p[1].y, p[2].y }))), 0, px - 1), y1 = clampi(int(std::ceil(std::max({ p[0].y, p[1].y, p[2].y }))), 0, px - 1);
	for (int y = y0; y <= y1; y++)
		for (int x = x0; x <= x1; x++) {
			float qx = float(x) + NORMAL_HALF, qy = float(y) + NORMAL_HALF;
			float w0 = ((p[1].x - qx) * (p[2].y - qy) - (p[1].y - qy) * (p[2].x - qx)) / area;
			float w1 = ((p[2].x - qx) * (p[0].y - qy) - (p[2].y - qy) * (p[0].x - qx)) / area;
			float w2 = 1.f - w0 - w1;
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) fn(x, y, w0, w1, w2);
		}
}

struct TreePictures::Frame {
	int px = 0;
	Vector3 right, up, eye;
	float scale = 1;
	std::vector<float> depth;
	std::vector<Color> colour;
	std::vector<Vector3> normal;
	Vector3 project(const Vector3 &p) const {
		Vector3 r = p - Vector3(0, CENTRE_Y, 0);
		return Vector3((r.dot(right) * scale + NORMAL_HALF) * float(px), (NORMAL_HALF - r.dot(up) * scale) * float(px), r.dot(eye));
	}
	Vector3 view(const Vector3 &n) const { return Vector3(n.dot(right), n.dot(up), n.dot(eye)); }
	void put(int x, int y, float z, const Color &c, const Vector3 &n) {
		size_t i = size_t(y) * px + x;
		if (z <= depth[i]) return;
		depth[i] = z;
		colour[i] = c;
		normal[i] = n;
	}
};

bool TreePictures::begin(const Dictionary &spec, int species, const Ref<Image> &leaves, PackedStringArray &errors) {
	if (!spec.has("picture_px") || !spec.has("elevations_deg") || !spec.has("picture_share")) {
		errors.push_back("streets.json trees: falta pictures");
		return false;
	}
	px_ = spec["picture_px"];
	share_ = spec["picture_share"];
	rows_ = species;
	Array e = spec["elevations_deg"];
	elevations_.clear();
	for (int i = 0; i < e.size(); i++) elevations_.push_back(float(e[i]));
	leaves_ = leaves;
	if (leaves_->is_compressed()) leaves_->decompress();
	int64_t bytes = int64_t(px_) * px_ * RGBA * frames() * rows_;
	colour_.resize(bytes);
	normals_.resize(bytes);
	return true;
}

void TreePictures::frame_body(Frame &f, const MeshBuilder &m) const {
	for (int64_t t = 0; t + TRI - 1 < m.idx.size(); t += TRI) {
		int a = m.idx[t], b = m.idx[t + 1], c = m.idx[t + 2];
		Vector3 p[TRI] = { f.project(m.v[a]), f.project(m.v[b]), f.project(m.v[c]) };
		Vector3 n = f.view(m.n[a]);
		each_pixel(p, f.px, [&](int x, int y, float w0, float w1, float w2) { f.put(x, y, p[0].z * w0 + p[1].z * w1 + p[2].z * w2, m.c[a], n); });
	}
}

// Leaf cards as the tufts of the atlas; fronds as textured triangles of its other region.
void TreePictures::frame_cards(Frame &f, const MeshBuilder &m) const {
	const int aw = leaves_->get_width(), ah = leaves_->get_height();
	for (int64_t k = 0; k < m.v.size();) {
		int part = int(std::lround(m.uv2[k].x));
		if (part != int(TreePart::CARD)) { k++; continue; }
		Vector3 c = f.project(m.v[k]);
		float half = m.uv2[k].y * f.scale * float(f.px), roll = m.c[k].a * TAU_F;
		float cs = std::cos(roll), sn = std::sin(roll);
		Vector3 n = f.view(m.n[k]);
		int reach = int(std::ceil(half * std::sqrt(2.f)));
		for (int y = int(c.y) - reach; y <= int(c.y) + reach; y++)
			for (int x = int(c.x) - reach; x <= int(c.x) + reach; x++) {
				if (x < 0 || y < 0 || x >= f.px || y >= f.px) continue;
				float dx = (float(x) + NORMAL_HALF - c.x) / half, dy = (float(y) + NORMAL_HALF - c.y) / half;
				float u = cs * dx + sn * dy, v = -sn * dx + cs * dy;
				if (std::fabs(u) > 1.f || std::fabs(v) > 1.f) continue;
				Color leaf = leaves_->get_pixel(clampi(int((u * NORMAL_HALF + NORMAL_HALF) * ATLAS_REGION_W * float(aw)), 0, aw - 1), clampi(int((v * NORMAL_HALF + NORMAL_HALF) * float(ah)), 0, ah - 1));
				if (leaf.a < ALPHA_CUT) continue;
				Color col = leaf_tone(m.c[k], leaf);
				f.put(x, y, c.z + (1.f - u * u - v * v) * half / (f.scale * float(f.px)), col, (n + Vector3(dx, -dy, 0) * CARD_BULGE).normalized());
			}
		k += CARD_CORNERS;
	}
	for (int64_t t = 0; t + TRI - 1 < m.idx.size(); t += TRI) {
		int a = m.idx[t], b = m.idx[t + 1], c = m.idx[t + 2];
		if (int(std::lround(m.uv2[a].x)) != int(TreePart::FROND)) continue;
		Vector3 p[TRI] = { f.project(m.v[a]), f.project(m.v[b]), f.project(m.v[c]) };
		each_pixel(p, f.px, [&](int x, int y, float w0, float w1, float w2) {
			Vector2 t2 = m.uv[a] * w0 + m.uv[b] * w1 + m.uv[c] * w2;
			Color leaf = leaves_->get_pixel(clampi(int((FROND_REGION_U + clampf(t2.x, 0.f, 1.f) * ATLAS_REGION_W) * float(aw)), 0, aw - 1), clampi(int(t2.y * float(ah)), 0, ah - 1));
			if (leaf.a < ALPHA_CUT) return;
			Color col = leaf_tone(m.c[a], leaf);
			f.put(x, y, p[0].z * w0 + p[1].z * w1 + p[2].z * w2, col, f.view(m.n[a]));
		});
	}
}

void TreePictures::store(const Frame &f, int row, int col, const Color &tone) {
	const int width = px_ * frames();
	uint8_t *c = colour_.ptrw(), *n = normals_.ptrw();
	for (int y = 0; y < px_; y++)
		for (int x = 0; x < px_; x++) {
			size_t i = size_t(y) * px_ + x, o = (size_t(row * px_ + y) * width + size_t(col * px_ + x)) * RGBA;
			bool hit = f.depth[i] > -std::numeric_limits<float>::max();
			// Empty texels hold the crown's mean tone: the mips then fade to it and not to black.
			Color k = hit ? f.colour[i] : tone;
			Vector3 v = hit ? f.normal[i].normalized() : Vector3(0, 0, 1);
			c[o] = uint8_t(clampf(k.r, 0.f, 1.f) * BYTE); c[o + 1] = uint8_t(clampf(k.g, 0.f, 1.f) * BYTE); c[o + 2] = uint8_t(clampf(k.b, 0.f, 1.f) * BYTE);
			c[o + 3] = hit ? uint8_t(BYTE) : 0;
			n[o] = uint8_t((v.x * NORMAL_HALF + NORMAL_HALF) * BYTE); n[o + 1] = uint8_t((v.y * NORMAL_HALF + NORMAL_HALF) * BYTE); n[o + 2] = uint8_t((v.z * NORMAL_HALF + NORMAL_HALF) * BYTE);
			n[o + 3] = c[o + 3];
		}
}

void TreePictures::draw(int row, const MeshBuilder &body, const MeshBuilder &cards, const Color &tone) {
	for (int col = 0; col < frames(); col++) {
		float e = elevations_[size_t(col)] * DEG_TO_RAD;
		Frame f;
		f.px = px_;
		f.right = Vector3(1, 0, 0);
		f.up = Vector3(0, std::cos(e), -std::sin(e));
		f.eye = Vector3(0, std::sin(e), std::cos(e));
		f.scale = 1.f / share_;
		f.depth.assign(size_t(px_) * px_, -std::numeric_limits<float>::max());
		f.colour.resize(f.depth.size());
		f.normal.resize(f.depth.size());
		frame_body(f, body);
		frame_cards(f, cards);
		store(f, row, col, tone);
	}
}

void TreePictures::finish() {
	const int width = px_ * frames(), height = px_ * rows_;
	Ref<Image> a = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, colour_);
	Ref<Image> n = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, normals_);
	a->generate_mipmaps();
	n->generate_mipmaps();
	albedo_ = ImageTexture::create_from_image(a);
	normal_ = ImageTexture::create_from_image(n);
	colour_ = PackedByteArray();
	normals_ = PackedByteArray();
	leaves_ = Ref<Image>();
}

} // namespace city
