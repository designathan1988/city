// Building bodies as bytes (body_codec.h).
#include "sim/persistence/body_codec.h"

#include "core/random.h"
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/b40/data.h"
#include "sim/buildings/b40/body_content.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr uint32_t FILE_CLAIM_CAP = 1u << 20; // what a damaged file may claim; far past any body
// Bytes that start with the magic carry a version; the bytes before it (saves and the player's model files) start with
// the plan's kind byte, never these four. 1: the flag of a roof mast (spire, lantern).
constexpr uint32_t BODY_MAGIC = 0x31594442U; // 'BDY1'
constexpr int FACES_BEFORE = 16; // a block's faces in the bytes of version 2 and earlier
constexpr uint32_t FACES_VERSION = 3;
constexpr uint32_t MODE_VERSION = 4, INSIDE_VERSION = 5, ANGLE_VERSION = 6, LIFT_VERSION = 7;
constexpr uint32_t BODY_VERSION = 7, FLAG_VERSION = 1, SIGNATURE_VERSION = 2, UNVERSIONED = 0;

SidePlan get_side(ByteReader &r) {
	SidePlan p;
	p.use = SideUse(r.get<uint8_t>());
	p.width = r.get<float>();
	p.walk = r.get<float>();
	return p;
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

FaceGeometry get_face(ByteReader &r) {
	FaceGeometry f;
	f.bays = r.get<int32_t>();
	for (float *v : { &f.window_w, &f.window_h, &f.sill, &f.pier_w, &f.pier_d }) *v = r.get<float>();
	f.pier_every = r.get<int32_t>();
	return f;
}

template <typename V>
bool get_map(ByteReader &r, std::unordered_map<uint32_t, V> &m) {
	uint32_t n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(uint32_t) + sizeof(V), FILE_CLAIM_CAP)) return false;
	m.reserve(n);
	for (uint32_t i = 0; i < n && r.ok; i++) {
		uint32_t key = r.get<uint32_t>();
		m[key] = r.get<V>();
	}
	return r.ok;
}

bool get_volume(ByteReader &r, Volume &v, uint32_t version) {
	for (float *x : { &v.x, &v.y, &v.w, &v.d, &v.pitch }) *x = r.get<float>();
	for (int *x : { &v.base, &v.storeys, &v.ridge, &v.fall, &v.wall, &v.roof_paint }) *x = r.get<int32_t>();
	v.roof = Roof(r.get<uint8_t>());
	v.lines = Lines(r.get<uint8_t>());
	v.crown = Crown(r.get<uint8_t>());
	v.keep = r.get<uint8_t>() != 0;
	uint32_t n = r.get<uint32_t>();
	if (!r.require_items(n, 2 * sizeof(float), FILE_CLAIM_CAP)) return false;
	v.outline.resize(n);
	for (Point2 &p : v.outline) {
		p.x = r.get<float>();
		p.y = r.get<float>();
	}
	r.get_vec(v.fill);
	if (!get_map(r, v.bays) || !get_map(r, v.side_fill) || !get_map(r, v.storey_paint)) return false;
	const int faces = version >= FACES_VERSION ? MAX_FACES : FACES_BEFORE;
	for (int i = 0; i < faces; i++) v.faces[i] = get_face(r);
	n = r.get<uint32_t>();
	if (!r.require_items(n, 5 * sizeof(int32_t) + sizeof(float), FILE_CLAIM_CAP)) return false;
	v.reliefs.resize(n);
	for (Relief &e : v.reliefs) {
		for (int *x : { &e.side, &e.bay0, &e.bay1, &e.storey0, &e.storey1 }) *x = r.get<int32_t>();
		e.depth = r.get<float>();
	}
	n = r.get<uint32_t>();
	const size_t gear_bytes = sizeof(uint8_t) + 6 * sizeof(float) + (version >= FLAG_VERSION ? sizeof(int8_t) + FLAG_COLOURS * sizeof(uint32_t) : 0);
	if (!r.require_items(n, gear_bytes, FILE_CLAIM_CAP)) return false;
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
	if (!r.ok || r.remaining() < sizeof(magic)) return UNVERSIONED;
	std::memcpy(&magic, r.p + r.o, sizeof(magic));
	if (magic != BODY_MAGIC) return UNVERSIONED;
	r.get<uint32_t>();
	return r.get<uint32_t>();
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
	b40::body_content::write(w, b);
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
	uint32_t n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(int8_t) + sizeof(uint32_t), FILE_CLAIM_CAP)) return false;
	b.paints.resize(n);
	for (Paint &p : b.paints) {
		p.finish = r.get<int8_t>();
		p.rgb = r.get<uint32_t>();
	}
	n = r.get<uint32_t>();
	const size_t face_bytes = 2 * sizeof(int32_t) + 5 * sizeof(float);
	const size_t volume_bytes = 5 * sizeof(float) + 6 * sizeof(int32_t) + 4 * sizeof(uint8_t) + 7 * sizeof(uint32_t)
		+ (version >= FACES_VERSION ? MAX_FACES : FACES_BEFORE) * face_bytes
		+ (version >= MODE_VERSION ? sizeof(uint8_t) : 0) + (version >= LIFT_VERSION ? sizeof(float) : 0);
	if (!r.require_items(n, volume_bytes, FILE_CLAIM_CAP)) return false;
	b.volumes.resize(n);
	for (Volume &v : b.volumes)
		if (!get_volume(r, v, version)) return false;
	n = r.get<uint32_t>();
	const size_t element_bytes = sizeof(uint8_t) + 7 * sizeof(float) + 2 * sizeof(int32_t) + (version >= ANGLE_VERSION ? sizeof(float) : 0);
	if (!r.require_items(n, element_bytes, FILE_CLAIM_CAP)) return false;
	b.elements.resize(n);
	for (LotElement &e : b.elements) e = get_element(r, version);
	n = r.get<uint32_t>();
	if (!r.require_items(n, 2 * sizeof(float) + sizeof(int32_t) + (version >= INSIDE_VERSION ? sizeof(uint8_t) : 0), FILE_CLAIM_CAP)) return false;
	b.cores.resize(n);
	for (Core &c : b.cores) {
		c.x = r.get<float>();
		c.y = r.get<float>();
		c.to = r.get<int32_t>();
		if (version >= INSIDE_VERSION) c.kind = CoreKind(r.get<uint8_t>());
	}
	if (version < INSIDE_VERSION) return r.ok;
	n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(int32_t) + sizeof(uint32_t), FILE_CLAIM_CAP)) return false;
	for (uint32_t i = 0; i < n; i++) {
		const int level = r.get<int32_t>();
		const uint32_t pieces = r.get<uint32_t>();
		if (!r.require_items(pieces, sizeof(uint8_t) + 3 * sizeof(float), FILE_CLAIM_CAP) || b.furnishing.count(level)) return false;
		std::vector<PlacedPiece> &floor = b.furnishing[level];
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
	const size_t count_at = w.size();
	w.put<uint32_t>(0);
	uint32_t count = 0;
	for (size_t i = 0; i < bs.items.size(); i++) {
		const Building &rec = bs.items[i];
		if (!rec.alive || rec.body == 0) continue;
		if (BodyRef body = bs.designs.edited->get(rec.body)) {
			if (count == std::numeric_limits<uint32_t>::max() || i > size_t(std::numeric_limits<int32_t>::max())) { w.invalidate(); return; }
			w.put<int32_t>(int32_t(i));
			write_body(w, *body);
			++count;
		}
	}
	w.patch<uint32_t>(count_at, count);
}

bool read_edited_bodies(ByteReader &r, BuildingStore &bs) {
	uint32_t n = r.get<uint32_t>();
	if (!r.require_items(n, sizeof(int32_t), bs.items.size())) return false;
	for (uint32_t i = 0; i < n; i++) {
		int id = r.get<int32_t>();
		Body body;
		if (!r.ok || id < 0 || size_t(id) >= bs.items.size() || !bs.items[size_t(id)].alive || bs.items[size_t(id)].body != 0 || !read_body(r, body)) return false;
		bs.set_body(id, bs.designs.edited->add(std::move(body)));
	}
	return r.ok;
}

} // namespace city
