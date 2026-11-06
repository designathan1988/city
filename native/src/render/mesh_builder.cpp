// Primitive emitters. Smooth normals only for blobs; everything else is flat shaded (toon look).
#include "render/mesh_builder.h"

#include "render/mesh_order.h"

#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int BLOB_RINGS = 4, BLOB_SEGMENTS = 7;
// Unit directions of the two edges of ring segment i (XZ plane).
void ring_dirs(int i, int seg, Vector3 &d0, Vector3 &d1) {
	float a0 = TAU_F * i / seg, a1 = TAU_F * (i + 1) / seg;
	d0 = Vector3(std::cos(a0), 0, std::sin(a0));
	d1 = Vector3(std::cos(a1), 0, std::sin(a1));
}
constexpr float DEGENERATE = 1e-12f;
constexpr float BYTE = 255.f;
constexpr int R_SHIFT = 24, G_SHIFT = 16, B_SHIFT = 8;
constexpr uint32_t BYTE_MASK = 0xFF;
} // namespace

Color rgba8_to_color(uint32_t c) {
	return Color(float((c >> R_SHIFT) & BYTE_MASK) / BYTE, float((c >> G_SHIFT) & BYTE_MASK) / BYTE, float((c >> B_SHIFT) & BYTE_MASK) / BYTE,
			float(c & BYTE_MASK) / BYTE);
}

Color rgb8_to_color(uint32_t c) {
	return rgba8_to_color((c << B_SHIFT) | BYTE_MASK);
}

int MeshBuilder::add(const Vector3 &p, const Vector3 &nn, const Color &col, const Vector2 &t, const Vector2 &t2) {
	v.push_back(p);
	n.push_back(nn);
	c.push_back(col);
	uv.push_back(t);
	uv2.push_back(t2);
	return int(v.size()) - 1;
}

void MeshBuilder::tri(int a, int b, int cc, const Vector3 &out) {
	Vector3 e = (v[b] - v[a]).cross(v[cc] - v[a]);
	if (e.dot(out) > 0) std::swap(b, cc); // clockwise seen from `out`
	idx.push_back(a);
	idx.push_back(b);
	idx.push_back(cc);
}

void MeshBuilder::quad(const Vector3 &a, const Vector3 &b, const Vector3 &cc, const Vector3 &d, const Color &col, const Vector3 &out,
		const Vector2 uvs[4], const Vector2 &t2) {
	Vector3 nn = (b - a).cross(d - a);
	if (nn.length_squared() < DEGENERATE) nn = (cc - b).cross(a - b);
	nn = nn.normalized();
	if (nn.dot(out) < 0) nn = -nn;
	const Vector2 def[4] = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };
	const Vector2 *t = uvs ? uvs : def;
	int i0 = add(a, nn, col, t[0], t2), i1 = add(b, nn, col, t[1], t2), i2 = add(cc, nn, col, t[2], t2), i3 = add(d, nn, col, t[3], t2);
	tri(i0, i1, i2, nn);
	tri(i0, i2, i3, nn);
}

void MeshBuilder::box(const Vector3 &mn, const Vector3 &mx, const Color &col, const Vector2 &t2, bool bottom) {
	const Vector3 p[BOX_CORNERS] = {
		Vector3(mn.x, mn.y, mn.z), Vector3(mx.x, mn.y, mn.z), Vector3(mx.x, mn.y, mx.z), Vector3(mn.x, mn.y, mx.z),
		Vector3(mn.x, mx.y, mn.z), Vector3(mx.x, mx.y, mn.z), Vector3(mx.x, mx.y, mx.z), Vector3(mn.x, mx.y, mx.z),
	};
	const int faces = bottom ? BOX_FACES : BOX_FACES - 1;
	for (int f = 0; f < faces; f++) {
		const int *q = BOX_FACE_CORNERS[f];
		const int *n = BOX_FACE_NORMALS[f];
		quad(p[q[0]], p[q[1]], p[q[2]], p[q[3]], col, Vector3(float(n[0]), float(n[1]), float(n[2])), nullptr, t2);
	}
}

void MeshBuilder::cylinder(const Vector3 &base, float r, float h, int seg, const Color &col, bool cap, const Vector2 &t2) {
	const Vector3 up(0, 1, 0), top = base + up * h;
	for (int i = 0; i < seg; i++) {
		Vector3 d0, d1;
		ring_dirs(i, seg, d0, d1);
		Vector3 nn = (d0 + d1).normalized();
		int i0 = add(base + d0 * r, d0, col, Vector2(), t2), i1 = add(base + d1 * r, d1, col, Vector2(), t2);
		int i2 = add(top + d1 * r, d1, col, Vector2(), t2), i3 = add(top + d0 * r, d0, col, Vector2(), t2);
		tri(i0, i1, i2, nn);
		tri(i0, i2, i3, nn);
		if (!cap) continue;
		int c0 = add(top, up, col, Vector2(), t2), c1 = add(top + d0 * r, up, col, Vector2(), t2), c2 = add(top + d1 * r, up, col, Vector2(), t2);
		tri(c0, c1, c2, up);
	}
}

void MeshBuilder::cone(const Vector3 &base, float r, float h, int seg, const Color &col, const Vector2 &t2) {
	const Vector3 tip = base + Vector3(0, h, 0), down(0, -1, 0);
	for (int i = 0; i < seg; i++) {
		Vector3 d0, d1;
		ring_dirs(i, seg, d0, d1);
		Vector3 nn = ((d0 + d1).normalized() * h + Vector3(0, r, 0)).normalized();
		int i0 = add(base + d0 * r, nn, col, Vector2(), t2), i1 = add(base + d1 * r, nn, col, Vector2(), t2), i2 = add(tip, nn, col, Vector2(), t2);
		tri(i0, i1, i2, nn);
		int b0 = add(base, down, col, Vector2(), t2), b1 = add(base + d0 * r, down, col, Vector2(), t2), b2 = add(base + d1 * r, down, col, Vector2(), t2);
		tri(b0, b1, b2, down);
	}
}

void MeshBuilder::blob(const Vector3 &center, const Vector3 &radius, const Color &col, const Vector2 &t2) {
	std::vector<int> ids;
	for (int r = 0; r <= BLOB_RINGS; r++) {
		float phi = PI_F * r / BLOB_RINGS;
		for (int s = 0; s <= BLOB_SEGMENTS; s++) {
			float th = TAU_F * s / BLOB_SEGMENTS;
			Vector3 dir(std::sin(phi) * std::cos(th), std::cos(phi), std::sin(phi) * std::sin(th));
			ids.push_back(add(center + Vector3(dir.x * radius.x, dir.y * radius.y, dir.z * radius.z), dir, col, Vector2(), t2));
		}
	}
	const int row = BLOB_SEGMENTS + 1;
	for (int r = 0; r < BLOB_RINGS; r++)
		for (int s = 0; s < BLOB_SEGMENTS; s++) {
			int a = ids[r * row + s], b = ids[r * row + s + 1], cc = ids[(r + 1) * row + s + 1], d = ids[(r + 1) * row + s];
			Vector3 out = (v[a] + v[cc]) * 0.5f - center;
			if (r > 0) tri(a, b, cc, out);
			if (r < BLOB_RINGS - 1) tri(a, cc, d, out);
		}
}

static Array surface_arrays(const MeshBuilder &b) {
	Array arr;
	arr.resize(Mesh::ARRAY_MAX);
	arr[Mesh::ARRAY_VERTEX] = b.v;
	arr[Mesh::ARRAY_NORMAL] = b.n;
	arr[Mesh::ARRAY_COLOR] = b.c;
	arr[Mesh::ARRAY_TEX_UV] = b.uv;
	arr[Mesh::ARRAY_TEX_UV2] = b.uv2;
	PackedInt32Array ordered = b.idx;
	order_triangles(ordered.ptrw(), size_t(ordered.size()), size_t(b.v.size()));
	arr[Mesh::ARRAY_INDEX] = ordered;
	return arr;
}

Ref<ArrayMesh> MeshBuilder::commit() const {
	Ref<ArrayMesh> m;
	m.instantiate();
	if (!empty()) m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_arrays(*this));
	return m;
}

int64_t MeshBuilder::bytes() const {
	return int64_t((v.size() + n.size()) * sizeof(Vector3) + c.size() * sizeof(Color) + (uv.size() + uv2.size()) * sizeof(Vector2) +
			idx.size() * sizeof(int32_t));
}

OwnedMesh MeshBuilder::bake() const {
	OwnedMesh m;
	if (empty()) return m;
	m.rid = RenderingServer::get_singleton()->mesh_create();
	commit_to(m.rid);
	return m;
}

void MeshBuilder::commit_to(const RID &mesh) const {
	if (empty()) return;
	RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, RenderingServer::PRIMITIVE_TRIANGLES, surface_arrays(*this), Array(),
			Dictionary(), RenderingServer::ARRAY_FLAG_COMPRESS_ATTRIBUTES);
}

} // namespace city
