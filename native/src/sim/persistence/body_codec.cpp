// Building bodies as bytes (body_codec.h).
#include "sim/persistence/body_codec.h"

#include "core/random.h"
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/b40/data.h"

#include <cstring>
#include <utility>
#include <vector>

namespace city {

using namespace b40;

namespace {
// The least bytes an entry of each list takes, in any version (the writers below): what a count read is checked against.
constexpr size_t POINT_BYTES = 2 * sizeof(float);
constexpr size_t RELIEF_BYTES = 5 * sizeof(int32_t) + sizeof(float);
constexpr size_t ROOF_PART_MIN_BYTES = sizeof(uint8_t) + 6 * sizeof(float); // without the flag of the later versions
constexpr size_t PAINT_BYTES = sizeof(int8_t) + sizeof(uint32_t);
constexpr size_t VOLUME_MIN_BYTES = 5 * sizeof(float) + 6 * sizeof(int32_t) + 4 * sizeof(uint8_t); // its fixed head
constexpr size_t ELEMENT_MIN_BYTES = sizeof(uint8_t) + 7 * sizeof(float) + 2 * sizeof(int32_t); // without the angle
constexpr size_t CORE_MIN_BYTES = 2 * sizeof(float) + sizeof(int32_t); // without the kind
constexpr size_t FLOOR_MIN_BYTES = sizeof(int32_t) + sizeof(uint32_t); // its level and its pieces' count
constexpr size_t PIECE_BYTES = sizeof(uint8_t) + 3 * sizeof(float);
// Bytes that start with the magic carry a version; the bytes before it (saves and the player's model files) start with
// the plan's kind byte, never these four. 1: the flag of a roof mast (spire, lantern).
constexpr uint32_t BODY_MAGIC = 0x31594442U; // 'BDY1'
constexpr int FACES_BEFORE = 16; // a block's faces in the bytes of version 2 and earlier
constexpr uint32_t FACES_VERSION = 3;
constexpr uint32_t MODE_VERSION = 4, INSIDE_VERSION = 5, ANGLE_VERSION = 6, LIFT_VERSION = 7;
constexpr uint32_t BODY_VERSION = 7, FLAG_VERSION = 1, SIGNATURE_VERSION = 2, UNVERSIONED = 0;

void put_side(ByteWriter &w, const SidePlan &p) {
	w.put<uint8_t>(uint8_t(p.use));
	w.put<float>(p.width);
	w.put<float>(p.walk);
}

SidePlan get_side(ByteReader &r) {
	SidePlan p;
	p.use = SideUse(r.get<uint8_t>());
	p.width = r.get<float>();
	p.walk = r.get<float>();
	return p;
}

void put_plan(ByteWriter &w, const LotPlan &p) {
	w.put<uint8_t>(uint8_t(p.kind));
	for (float v : { p.W, p.D, p.front_depth, p.back_depth, p.x0, p.y0, p.x1, p.y1 }) w.put<float>(v);
	w.put<uint8_t>(uint8_t(p.front));
	put_side(w, p.left);
	put_side(w, p.right);
	w.put<uint8_t>(uint8_t(p.back));
	w.put<int32_t>(p.rows);
}

LotPlan get_plan(ByteReader &r) {
	LotPlan p;
	p.kind = Kind(r.get<uint8_t>());
	for (float *v : { &p.W, &p.D, &p.front_depth, &p.back_depth, &p.x0, &p.y0, &p.x1, &p.y1 }) *v = r.get<float>();
	p.front = FrontUse(r.get<uint8_t>());
	p.left = get_side(r);
	p.right = get_side(r);
	p.back = BackUse(r.get<uint8_t>());
	p.rows = r.get<int32_t>();
	return p;
}

void put_face(ByteWriter &w, const FaceGeometry &f) {
	w.put<int32_t>(f.bays);
	for (float v : { f.window_w, f.window_h, f.sill, f.pier_w, f.pier_d }) w.put<float>(v);
	w.put<int32_t>(f.pier_every);
}

FaceGeometry get_face(ByteReader &r) {
	FaceGeometry f;
	f.bays = r.get<int32_t>();
	for (float *v : { &f.window_w, &f.window_h, &f.sill, &f.pier_w, &f.pier_d }) *v = r.get<float>();
	f.pier_every = r.get<int32_t>();
	return f;
}

// A map by key order (ByteWriter::put_by_key): the same entries give the same bytes, whatever the order they went in.
template <typename V>
void put_map(ByteWriter &w, const std::unordered_map<uint32_t, V> &m) {
	w.put_by_key(m, [&](const auto &kv) {
		w.put<uint32_t>(kv.first);
		w.put<V>(kv.second);
	});
}

template <typename V>
bool get_map(ByteReader &r, std::unordered_map<uint32_t, V> &m) {
	const uint32_t n = r.get_count(sizeof(uint32_t) + sizeof(V));
	if (!r.ok) return false;
	m.reserve(n);
	for (uint32_t i = 0; i < n && r.ok; i++) {
		uint32_t key = r.get<uint32_t>();
		m[key] = r.get<V>();
	}
	return r.ok;
}

void put_volume(ByteWriter &w, const Volume &v) {
	for (float x : { v.x, v.y, v.w, v.d, v.pitch }) w.put<float>(x);
	for (int x : { v.base, v.storeys, v.ridge, v.fall, v.wall, v.roof_paint }) w.put<int32_t>(x);
	w.put<uint8_t>(uint8_t(v.roof));
	w.put<uint8_t>(uint8_t(v.lines));
	w.put<uint8_t>(uint8_t(v.crown));
	w.put<uint8_t>(v.keep ? 1 : 0);
	w.put<uint32_t>(uint32_t(v.outline.size()));
	for (const V2 &p : v.outline) {
		w.put<float>(p.x);
		w.put<float>(p.y);
	}
	w.put_vec(v.fill);
	put_map(w, v.bays);
	put_map(w, v.side_fill);
	put_map(w, v.storey_paint);
	for (const FaceGeometry &f : v.faces) put_face(w, f);
	w.put<uint32_t>(uint32_t(v.reliefs.size()));
	for (const Relief &r : v.reliefs) {
		for (int x : { r.side, r.bay0, r.bay1, r.storey0, r.storey1 }) w.put<int32_t>(x);
		w.put<float>(r.depth);
	}
	w.put<uint32_t>(uint32_t(v.gear.size()));
	for (const RoofPart &p : v.gear) {
		w.put<uint8_t>(p.kind);
		for (float x : { p.x, p.y, p.w, p.d, p.h, p.turn }) w.put<float>(x);
		w.put<int8_t>(p.flag.pattern);
		for (uint32_t c : p.flag.rgb) w.put<uint32_t>(c);
	}
	w.put<uint8_t>(uint8_t(v.mode));
	w.put<float>(v.lift);
}

bool get_volume(ByteReader &r, Volume &v, uint32_t version) {
	for (float *x : { &v.x, &v.y, &v.w, &v.d, &v.pitch }) *x = r.get<float>();
	for (int *x : { &v.base, &v.storeys, &v.ridge, &v.fall, &v.wall, &v.roof_paint }) *x = r.get<int32_t>();
	v.roof = Roof(r.get<uint8_t>());
	v.lines = Lines(r.get<uint8_t>());
	v.crown = Crown(r.get<uint8_t>());
	v.keep = r.get<uint8_t>() != 0;
	uint32_t n = r.get_count(POINT_BYTES);
	if (!r.ok) return false;
	v.outline.resize(n);
	for (V2 &p : v.outline) {
		p.x = r.get<float>();
		p.y = r.get<float>();
	}
	r.get_vec(v.fill);
	if (!get_map(r, v.bays) || !get_map(r, v.side_fill) || !get_map(r, v.storey_paint)) return false;
	const int faces = version >= FACES_VERSION ? MAX_FACES : FACES_BEFORE;
	for (int i = 0; i < faces; i++) v.faces[i] = get_face(r);
	n = r.get_count(RELIEF_BYTES);
	if (!r.ok) return false;
	v.reliefs.resize(n);
	for (Relief &e : v.reliefs) {
		for (int *x : { &e.side, &e.bay0, &e.bay1, &e.storey0, &e.storey1 }) *x = r.get<int32_t>();
		e.depth = r.get<float>();
	}
	n = r.get_count(ROOF_PART_MIN_BYTES);
	if (!r.ok) return false;
	v.gear.resize(n);
	for (RoofPart &p : v.gear) {
		p.kind = r.get<uint8_t>();
		for (float *x : { &p.x, &p.y, &p.w, &p.d, &p.h, &p.turn }) *x = r.get<float>();
		if (version < FLAG_VERSION) continue;
		p.flag.pattern = r.get<int8_t>();
		for (uint32_t &c : p.flag.rgb) c = r.get<uint32_t>();
	}
	if (version >= MODE_VERSION) v.mode = BlockMode(r.get<uint8_t>());
	if (version >= LIFT_VERSION) v.lift = r.get<float>();
	return r.ok;
}

// The version the bytes at the reader announce, taken when they announce one; the old bytes stay where they are.
uint32_t body_version(ByteReader &r) {
	uint32_t magic = 0;
	if (r.o + sizeof(magic) > r.n) return UNVERSIONED;
	std::memcpy(&magic, r.p + r.o, sizeof(magic));
	if (magic != BODY_MAGIC) return UNVERSIONED;
	r.get<uint32_t>();
	return r.get<uint32_t>();
}

void put_element(ByteWriter &w, const LotElement &e) {
	w.put<uint8_t>(uint8_t(e.kind));
	for (float x : { e.x, e.y, e.w, e.d, e.z, e.h, e.base }) w.put<float>(x);
	w.put<int32_t>(e.facing);
	w.put<int32_t>(e.paint);
	w.put<float>(e.angle);
}

LotElement get_element(ByteReader &r, uint32_t version) {
	LotElement e;
	e.kind = Element(r.get<uint8_t>());
	for (float *x : { &e.x, &e.y, &e.w, &e.d, &e.z, &e.h, &e.base }) *x = r.get<float>();
	e.facing = r.get<int32_t>();
	e.paint = r.get<int32_t>();
	if (version >= ANGLE_VERSION) e.angle = r.get<float>();
	return e;
}
} // namespace

void write_body(ByteWriter &w, const Body &b) {
	w.put<uint32_t>(BODY_MAGIC);
	w.put<uint32_t>(BODY_VERSION);
	put_plan(w, b.plan);
	for (uint8_t x : { uint8_t(b.kind), uint8_t(b.use), uint8_t(b.ground), b.style }) w.put<uint8_t>(x);
	for (int x : { b.palette, b.wall, b.roof, b.trim, b.entrance_volume }) w.put<int32_t>(x);
	for (float x : { b.ground_h, b.storey_h, b.entrance_x }) w.put<float>(x);
	w.put<uint8_t>(b.signature);
	for (float x : { b.signature_x, b.signature_y }) w.put<float>(x);
	w.put<uint32_t>(uint32_t(b.paints.size()));
	for (const Paint &p : b.paints) {
		w.put<int8_t>(p.finish);
		w.put<uint32_t>(p.rgb);
	}
	w.put<uint32_t>(uint32_t(b.volumes.size()));
	for (const Volume &v : b.volumes) put_volume(w, v);
	w.put<uint32_t>(uint32_t(b.elements.size()));
	for (const LotElement &e : b.elements) put_element(w, e);
	w.put<uint32_t>(uint32_t(b.cores.size()));
	for (const Core &c : b.cores) {
		w.put<float>(c.x);
		w.put<float>(c.y);
		w.put<int32_t>(c.to);
		w.put<uint8_t>(uint8_t(c.kind));
	}
	w.put<uint32_t>(uint32_t(b.furnishing.size()));
	for (const auto &floor : b.furnishing) {
		w.put<int32_t>(floor.first);
		w.put<uint32_t>(uint32_t(floor.second.size()));
		for (const PlacedPiece &p : floor.second) {
			w.put<uint8_t>(p.kind);
			for (float x : { p.x, p.y, p.angle }) w.put<float>(x);
		}
	}
}

bool read_body(ByteReader &r, Body &b) {
	const uint32_t version = body_version(r);
	if (version > BODY_VERSION) return false;
	b.plan = get_plan(r);
	b.kind = Kind(r.get<uint8_t>());
	b.use = Use(r.get<uint8_t>());
	b.ground = Ground(r.get<uint8_t>());
	b.style = r.get<uint8_t>();
	for (int *x : { &b.palette, &b.wall, &b.roof, &b.trim, &b.entrance_volume }) *x = r.get<int32_t>();
	for (float *x : { &b.ground_h, &b.storey_h, &b.entrance_x }) *x = r.get<float>();
	if (version >= SIGNATURE_VERSION) {
		b.signature = r.get<uint8_t>();
		for (float *x : { &b.signature_x, &b.signature_y }) *x = r.get<float>();
	}
	uint32_t n = r.get_count(PAINT_BYTES);
	if (!r.ok) return false;
	b.paints.resize(n);
	for (Paint &p : b.paints) {
		p.finish = r.get<int8_t>();
		p.rgb = r.get<uint32_t>();
	}
	n = r.get_count(VOLUME_MIN_BYTES);
	if (!r.ok) return false;
	b.volumes.resize(n);
	for (Volume &v : b.volumes)
		if (!get_volume(r, v, version)) return false;
	n = r.get_count(ELEMENT_MIN_BYTES);
	if (!r.ok) return false;
	b.elements.resize(n);
	for (LotElement &e : b.elements) e = get_element(r, version);
	n = r.get_count(CORE_MIN_BYTES);
	if (!r.ok) return false;
	b.cores.resize(n);
	for (Core &c : b.cores) {
		c.x = r.get<float>();
		c.y = r.get<float>();
		c.to = r.get<int32_t>();
		if (version >= INSIDE_VERSION) c.kind = CoreKind(r.get<uint8_t>());
	}
	if (version < INSIDE_VERSION) return r.ok;
	n = r.get_count(FLOOR_MIN_BYTES);
	if (!r.ok) return false;
	for (uint32_t i = 0; i < n; i++) {
		std::vector<PlacedPiece> &floor = b.furnishing[r.get<int32_t>()];
		const uint32_t pieces = r.get_count(PIECE_BYTES);
		if (!r.ok) return false;
		floor.resize(pieces);
		for (PlacedPiece &p : floor) {
			p.kind = r.get<uint8_t>();
			for (float *x : { &p.x, &p.y, &p.angle }) *x = r.get<float>();
		}
	}
	return r.ok;
}

uint32_t body_digest(const Body &b) {
	ByteWriter w;
	write_body(w, b);
	uint32_t h = 0;
	for (uint8_t byte : w.buf) h = hash_combine(h, byte);
	return h;
}

void write_edited_bodies(ByteWriter &w, const BuildingStore &bs) {
	const size_t count_at = w.buf.size();
	w.put<uint32_t>(0); // the count, set once the records are walked
	uint32_t count = 0;
	for (size_t i = 0; i < bs.items.size(); i++) {
		const Building &rec = bs.items[i];
		if (!rec.alive || !rec.body) continue;
		w.put<int32_t>(int32_t(i));
		write_body(w, *rec.body);
		count++;
	}
	std::memcpy(w.buf.data() + count_at, &count, sizeof(count));
}

bool read_edited_bodies(ByteReader &r, BuildingStore &bs) {
	const uint32_t n = r.get_count(sizeof(int32_t)); // each: the record's id, then its body
	if (!r.ok || n > bs.items.size()) return false;
	for (uint32_t i = 0; i < n; i++) {
		int id = r.get<int32_t>();
		Body body;
		if (!r.ok || id < 0 || size_t(id) >= bs.items.size() || !bs.items[size_t(id)].alive || !read_body(r, body)) return false;
		bs.items[size_t(id)].body = bs.designs.edited->add(std::move(body));
	}
	return r.ok;
}

} // namespace city
