// Builds the street furniture meshes from their data parts and copies them into chunk meshes.
#include "render/props/prop_kit.h"

#include "core/color.h"
#include "core/math.h"
#include "core/units.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HORIZONTAL_EPS = 1e-6f; // a direction this flat has no up
constexpr int LIGHT_SIDES = 18, BEAM_SEGMENTS = 4; // Volumetric Light Beam: 18 sides, at least 3 segments for a smooth fall
constexpr float BEAM_LENS = 0.075f; // the cut apex over the radius at the ground: the lens, some 0.28 m of 3.7
constexpr float LIGHT_BEAM_PART = 2.f;
const Color WHITE(1, 1, 1, 1);

// A cylinder narrowing from r0 at its base to r1 at its top, its top closed.
void frustum(MeshBuilder &b, const Vector3 &base, float r0, float r1, float h, int seg, const Color &col, const Vector2 &t2) {
	const Vector3 top = base + Vector3(0, h, 0), up(0, 1, 0);
	const float lean = (r0 - r1) / h;
	for (int i = 0; i < seg; i++) {
		const float a0 = TAU_F * i / seg, a1 = TAU_F * (i + 1) / seg;
		const Vector3 d0(std::cos(a0), 0, std::sin(a0)), d1(std::cos(a1), 0, std::sin(a1));
		const Vector3 n0 = (d0 + up * lean).normalized(), n1 = (d1 + up * lean).normalized(), nn = (n0 + n1).normalized();
		const int i0 = b.add(base + d0 * r0, n0, col, Vector2(), t2), i1 = b.add(base + d1 * r0, n1, col, Vector2(), t2);
		const int i2 = b.add(top + d1 * r1, n1, col, Vector2(), t2), i3 = b.add(top + d0 * r1, n0, col, Vector2(), t2);
		b.tri(i0, i1, i2, nn);
		b.tri(i0, i2, i3, nn);
		const int c0 = b.add(top, up, col, Vector2(), t2), c1 = b.add(top + d0 * r1, up, col, Vector2(), t2), c2 = b.add(top + d1 * r1, up, col, Vector2(), t2);
		b.tri(c0, c1, c2, up);
	}
}

// A bar of section width x height from a to b.
void beam(MeshBuilder &b, const Vector3 &from, const Vector3 &to, float w, float h, const Color &col, const Vector2 &t2) {
	const Vector3 along = (to - from).normalized();
	const Vector3 side = (std::fabs(along.y) > 1.f - HORIZONTAL_EPS ? Vector3(1, 0, 0) : along.cross(Vector3(0, 1, 0)).normalized()) * (w * 0.5f);
	const Vector3 up = side.cross(along).normalized() * (h * 0.5f);
	const Vector3 c[4] = { side + up, up - side, -side - up, side - up };
	for (int i = 0; i < 4; i++) {
		const Vector3 p = c[i], q = c[(i + 1) % 4];
		b.quad(from + p, from + q, to + q, to + p, col, (p + q).normalized(), nullptr, t2);
	}
	b.quad(from + c[0], from + c[1], from + c[2], from + c[3], col, -along, nullptr, t2);
	b.quad(to + c[0], to + c[1], to + c[2], to + c[3], col, along, nullptr, t2);
}

// One part in `col`, its UV2 `t2` (its PropTag; a lens's Lens or a pole's PolePart in y); `most_sides` caps a round part's sides
// (0: its own).
void shape(MeshBuilder &b, const DataValue &p, const Color &col, const Vector2 &t2, int most_sides = 0) {
	const String shape = p["shape"].text();
	const auto sides = [&]() { const int s = p["segments"].whole(); return most_sides > 0 ? std::min(s, most_sides) : s; };
	if (shape == "box") b.box(p["min"].v3(), p["max"].v3(), col, t2, true);
	else if (shape == "cylinder") b.cylinder(p["base"].v3(), p["radius"].f(), p["height"].f(), sides(), col, true, t2);
	else if (shape == "blob") b.blob(p["center"].v3(), p["radius"].v3(), col, t2);
	else if (shape == "frustum") frustum(b, p["base"].v3(), p["radius"].f(), p["top_radius"].f(), p["height"].f(), sides(), col, t2);
	else if (shape == "beam") beam(b, p["from"].v3(), p["to"].v3(), p["width"].f(), p["thick"].f(), col, t2);
	else p["shape"].fail(String("unknown shape '") + shape + String("'"));
}

Color part_color(const DataValue &p) { return rgba8_to_color(p["color"].colour()); }

PropTag part_tag(const DataValue &p) {
	if (p.has("glow") && p["glow"].text() == "night") return PropTag::NIGHT_GLOW;
	if (!(p.has("wood") && p["wood"].flag())) return PropTag::FIXED;
	return p["shape"].text() == "box" ? PropTag::PLANK : PropTag::WOOD; // sawn, or a trunk
}

// The pole part a part is (props.gdshader shows or hides each per pole), 0 when it says none.
float part_of(const DataValue &p) { return p.has("part") ? p["part"].f() : 0.f; }

// Where on itself a part's vertices lie, for the surface the shader gives it: a plank's length and across it, anything else its height off the ground (props.gdshader).
void mark_surface(MeshBuilder &b, int64_t from, PropTag tag) {
	for (int64_t i = from; i < b.v.size(); i++) {
		const Vector3 &v = b.v[i];
		b.uv.set(i, tag == PropTag::PLANK ? Vector2(v.x, v.y + v.z) : Vector2(0, v.y));
	}
}

// A prop's parts, its lenses (parts with "lens") left to PropKit::lenses.
MeshBuilder parts(const DataValue &props, const char *name) {
	MeshBuilder b;
	const DataValue list = props[name];
	for (int i = 0; i < list.size(); i++) {
		const DataValue p = list[i];
		if (p.has("lens")) continue;
		const int64_t from = b.v.size();
		shape(b, p, part_color(p), Vector2(float(int(part_tag(p))), part_of(p)));
		mark_surface(b, from, part_tag(p));
	}
	return b;
}

// The longest side of a part: what decides whether its shadow reads.
float part_size(const DataValue &p) {
	const String kind = p["shape"].text();
	if (kind == "box") {
		const Vector3 d = p["max"].v3() - p["min"].v3();
		return std::max({ std::fabs(d.x), std::fabs(d.y), std::fabs(d.z) });
	}
	if (kind == "cylinder" || kind == "frustum") return std::max(p["height"].f(), 2.f * p["radius"].f());
	return BIG;
}

// What a far prop casts: its parts long enough to show in a shadow, round ones with fewer sides. The parts keep their marks, so
// the shader that shows or hides them per instance goes on working.
MeshBuilder cast_parts(const DataValue &props, const char *name, const DataValue &cast) {
	MeshBuilder b;
	const DataValue list = props[name];
	const float least = cast["least_m"].f();
	const int sides = cast["segments"].whole();
	for (int i = 0; i < list.size(); i++) {
		const DataValue p = list[i];
		if (p.has("lens") || part_size(p) < least) continue;
		shape(b, p, part_color(p), Vector2(float(int(part_tag(p))), part_of(p)), sides);
	}
	return b;
}

// A wire: `chords` straight pieces of a tube of `sides` sides round the x axis from 0 to 1, flat-shaded per side. UV is the
// wire's height and its seat along an urban pole's arm, UV2.y its seat along a rural pole's (or WIRE_SECONDARY_MARK), which
// props.gdshader hangs from the poles by their kinds.
void wire_tube(MeshBuilder &b, const Vector2 &place, float rural_seat, const Color &colour, float radius, int chords, int sides) {
	const Vector2 t2(float(int(PropTag::WIRE)), rural_seat);
	const Vector2 uvs[4] = { place, place, place, place };
	for (int s = 0; s < chords; s++) {
		const float t0 = float(s) / chords, t1 = float(s + 1) / chords;
		for (int k = 0; k < sides; k++) {
			const float a0 = TAU_F * k / sides, a1 = TAU_F * (k + 1) / sides;
			const Vector3 r0(0, std::cos(a0) * radius, std::sin(a0) * radius), r1(0, std::cos(a1) * radius, std::sin(a1) * radius);
			b.quad(Vector3(t0, 0, 0) + r0, Vector3(t1, 0, 0) + r0, Vector3(t1, 0, 0) + r1, Vector3(t0, 0, 0) + r1, colour, (r0 + r1).normalized(), uvs, t2);
		}
	}
}

// Every wire of a span (streets.json wires): the primary ones on their seats (WireSeats), then the secondary ones at the rest of
// y_m, on the urban rack (secondary_z_m).
void wire_mesh(MeshBuilder &b, const DataValue &wires, const WireSeats &seats, int chords, int sides) {
	const DataValue ys = wires["y_m"];
	for (size_t k = 0; k < seats.urban.size(); k++) wire_tube(b, Vector2(seats.height, seats.urban[k]), seats.rural[k], seats.colour, seats.radius, chords, sides);
	for (int i = 1; i < ys.size(); i++) wire_tube(b, Vector2(ys[i].f(), wires["secondary_z_m"].f()), WIRE_SECONDARY_MARK, seats.colour, seats.radius, chords, sides);
}

void light_mesh(MeshBuilder &b) {
	const Vector3 up(0, 1, 0);
	// The beam: a unit cone cut at the lens, in rings so the fall along it stays smooth; apex up, foot on the ground.
	std::vector<int> ring(size_t(LIGHT_SIDES) * size_t(BEAM_SEGMENTS + 1));
	for (int s = 0; s <= BEAM_SEGMENTS; s++) {
		const float k = float(s) / BEAM_SEGMENTS, radius = BEAM_LENS + (1.f - BEAM_LENS) * k;
		for (int i = 0; i < LIGHT_SIDES; i++) {
			const float a = TAU_F * i / LIGHT_SIDES;
			const Vector3 d(std::cos(a), 0, std::sin(a));
			ring[size_t(s * LIGHT_SIDES + i)] = b.add(d * radius + up * (1.f - k), (d + up * (1.f - BEAM_LENS)).normalized(), WHITE, Vector2(0, 1.f - k), Vector2(0, LIGHT_BEAM_PART));
		}
	}
	for (int s = 0; s < BEAM_SEGMENTS; s++)
		for (int i = 0; i < LIGHT_SIDES; i++) {
			const int j = (i + 1) % LIGHT_SIDES, a = ring[size_t(s * LIGHT_SIDES + i)], c = ring[size_t(s * LIGHT_SIDES + j)];
			const int d = ring[size_t((s + 1) * LIGHT_SIDES + i)], e = ring[size_t((s + 1) * LIGHT_SIDES + j)];
			const float mid = TAU_F * (i + 0.5f) / LIGHT_SIDES;
			const Vector3 out(std::cos(mid), 1.f - BEAM_LENS, std::sin(mid));
			b.tri(a, d, c, out);
			b.tri(c, d, e, out);
		}
}
// A steel sheet bent along x, from -0.5 to 0.5: `section` its points across, [y, z] in the unit box; both faces seen.
void sheet_mesh(MeshBuilder &b, const DataValue &section) {
	const Vector2 t2(float(int(PropTag::FINISHED)), 0);
	const float half = 0.5f;
	for (int i = 0; i + 1 < section.size(); i++) {
		const Vector2 p = section[i].v2(), q = section[i + 1].v2();
		const Vector3 a(-half, p.x, p.y), c(-half, q.x, q.y);
		const Vector3 d(half, c.y, c.z), e(half, a.y, a.z);
		const Vector3 out = (c - a).cross(Vector3(1, 0, 0)).normalized();
		b.quad(a, c, d, e, WHITE, out, nullptr, t2);
		b.quad(a, c, d, e, WHITE, -out, nullptr, t2);
	}
}
} // namespace

bool WireSeats::load(const DataValue &wires) {
	const DataValue ys = wires["y_m"], u = wires["z_m"], r = wires["rural_z_m"];
	if (ys.failed || u.failed || r.failed) return false;
	if (u.size() != r.size() || ys.size() == 0 || u.size() == 0) {
		wires.fail("z_m and rural_z_m of different lengths, or y_m or z_m empty");
		return false;
	}
	height = ys[0].f();
	radius = wires["radius_m"].f();
	colour = rgba8_to_color(wires["color"].colour());
	urban.clear();
	rural.clear();
	for (int k = 0; k < u.size(); k++) {
		urban.push_back(u[k].f());
		rural.push_back(r[k].f());
	}
	return true;
}

bool PropKit::build(const DataValue &props, const DataValue &wires, const DataValue &beam_section, PackedStringArray &errors) {
	const int64_t before = errors.size();
	meshes_[int(PropKind::SIGNAL)] = parts(props, "signal");
	meshes_[int(PropKind::HYDRANT)] = parts(props, "hydrant");
	meshes_[int(PropKind::BIN)] = parts(props, "bin");
	meshes_[int(PropKind::BENCH)] = parts(props, "bench");
	meshes_[int(PropKind::POSTBOX)] = parts(props, "postbox");
	meshes_[int(PropKind::PHONE)] = parts(props, "phone");
	meshes_[int(PropKind::DRAIN)] = parts(props, "drain");
	meshes_[int(PropKind::TREE_PIT)] = parts(props, "tree_pit");
	meshes_[int(PropKind::MEADOW)] = parts(props, "meadow");
	pole_ = parts(props, "pole");
	const DataValue cast = props["pole_cast"];
	if (errors.size() != before) return false;
	pole_cast_ = cast_parts(props, "pole", cast);
	cast_from_ = cast["from_m"].f();
	WireSeats seats;
	wires["secondary_z_m"].f();
	if (!seats.load(wires) || errors.size() != before) return false;
	wire_mesh(wire_near_, wires, seats, wires["near_chords"].whole(), wires["near_sides"].whole());
	wire_mesh(wire_far_, wires, seats, wires["far_chords"].whole(), wires["far_sides"].whole());
	box_.box(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f), WHITE, Vector2(float(int(PropTag::FINISHED)), 0), true);
	sheet_mesh(beam_, beam_section);
	light_mesh(light_);
	const DataValue dark = props["lens_dark"]; // each lens unlit: its colour and its own faint glow
	bool seen[int(Lens::COUNT)] = {};
	const DataValue list = props["signal"];
	for (int i = 0; i < list.size(); i++) {
		const DataValue p = list[i];
		if (!p.has("lens")) continue;
		const int k = p["lens"].whole();
		if (k < 0 || k >= int(Lens::COUNT)) {
			p["lens"].fail("a lens outside 0..4");
			continue;
		}
		const DataValue unlit = dark[k][0];
		unlit.colour(); // '#rrggbb', or an error
		shape(lenses_, p, Color::html(unlit.text()), Vector2(float(int(PropTag::LENS)), float(k)));
		seen[k] = true;
	}
	for (bool s : seen)
		if (!s) list.fail("one of the lenses 0..4 is missing");
	return errors.size() == before;
}

void PropKit::append(MeshBuilder &dst, const MeshBuilder &src, const Basis &b, const Vector3 &o) {
	int base = int(dst.v.size());
	for (int64_t i = 0; i < src.v.size(); i++)
		dst.add(b.xform(src.v[i]) + o, b.xform(src.n[i]).normalized(), src.c[i], src.uv[i], src.uv2[i]);
	for (int64_t i = 0; i < src.idx.size(); i++) dst.idx.push_back(src.idx[i] + base);
}

Basis PropKit::facing(const Vector3 &face) {
	Vector3 z(face.x, 0, face.z);
	if (z.length_squared() < HORIZONTAL_EPS) z = Vector3(0, 0, 1);
	z = z.normalized();
	Vector3 y(0, 1, 0), x = y.cross(z);
	return Basis(x, y, z);
}

} // namespace city
