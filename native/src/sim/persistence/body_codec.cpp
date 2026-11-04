// Building bodies as bytes (body_codec.h).
#include "sim/persistence/body_codec.h"

#include "core/random.h"
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/b40/data.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr uint32_t MAX_ITEMS = 1u << 20; // what a damaged file may claim; far past any body

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

// A map by key order: the same entries give the same bytes, whatever the order they went in.
template <typename V>
void put_map(ByteWriter &w, const std::unordered_map<uint32_t, V> &m) {
	std::vector<std::pair<uint32_t, V>> sorted(m.begin(), m.end());
	std::sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
	w.put<uint32_t>(uint32_t(sorted.size()));
	for (const auto &e : sorted) {
		w.put<uint32_t>(e.first);
		w.put<V>(e.second);
	}
}

template <typename V>
bool get_map(ByteReader &r, std::unordered_map<uint32_t, V> &m) {
	uint32_t n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
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
	for (const Point2 &p : v.outline) {
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
}

bool get_volume(ByteReader &r, Volume &v) {
	for (float *x : { &v.x, &v.y, &v.w, &v.d, &v.pitch }) *x = r.get<float>();
	for (int *x : { &v.base, &v.storeys, &v.ridge, &v.fall, &v.wall, &v.roof_paint }) *x = r.get<int32_t>();
	v.roof = Roof(r.get<uint8_t>());
	v.lines = Lines(r.get<uint8_t>());
	v.crown = Crown(r.get<uint8_t>());
	v.keep = r.get<uint8_t>() != 0;
	uint32_t n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
	v.outline.resize(n);
	for (Point2 &p : v.outline) {
		p.x = r.get<float>();
		p.y = r.get<float>();
	}
	r.get_vec(v.fill);
	if (!get_map(r, v.bays) || !get_map(r, v.side_fill) || !get_map(r, v.storey_paint)) return false;
	for (FaceGeometry &f : v.faces) f = get_face(r);
	n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
	v.reliefs.resize(n);
	for (Relief &e : v.reliefs) {
		for (int *x : { &e.side, &e.bay0, &e.bay1, &e.storey0, &e.storey1 }) *x = r.get<int32_t>();
		e.depth = r.get<float>();
	}
	return r.ok;
}

void put_element(ByteWriter &w, const LotElement &e) {
	w.put<uint8_t>(uint8_t(e.kind));
	for (float x : { e.x, e.y, e.w, e.d, e.z, e.h, e.base }) w.put<float>(x);
	w.put<int32_t>(e.facing);
	w.put<int32_t>(e.paint);
}

LotElement get_element(ByteReader &r) {
	LotElement e;
	e.kind = Element(r.get<uint8_t>());
	for (float *x : { &e.x, &e.y, &e.w, &e.d, &e.z, &e.h, &e.base }) *x = r.get<float>();
	e.facing = r.get<int32_t>();
	e.paint = r.get<int32_t>();
	return e;
}
} // namespace

void write_body(ByteWriter &w, const Body &b) {
	put_plan(w, b.plan);
	for (uint8_t x : { uint8_t(b.kind), uint8_t(b.use), uint8_t(b.ground), b.style }) w.put<uint8_t>(x);
	for (int x : { b.palette, b.wall, b.roof, b.trim, b.entrance_volume }) w.put<int32_t>(x);
	for (float x : { b.ground_h, b.storey_h, b.entrance_x }) w.put<float>(x);
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
	}
}

bool read_body(ByteReader &r, Body &b) {
	b.plan = get_plan(r);
	b.kind = Kind(r.get<uint8_t>());
	b.use = Use(r.get<uint8_t>());
	b.ground = Ground(r.get<uint8_t>());
	b.style = r.get<uint8_t>();
	for (int *x : { &b.palette, &b.wall, &b.roof, &b.trim, &b.entrance_volume }) *x = r.get<int32_t>();
	for (float *x : { &b.ground_h, &b.storey_h, &b.entrance_x }) *x = r.get<float>();
	uint32_t n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
	b.paints.resize(n);
	for (Paint &p : b.paints) {
		p.finish = r.get<int8_t>();
		p.rgb = r.get<uint32_t>();
	}
	n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
	b.volumes.resize(n);
	for (Volume &v : b.volumes)
		if (!get_volume(r, v)) return false;
	n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
	b.elements.resize(n);
	for (LotElement &e : b.elements) e = get_element(r);
	n = r.get<uint32_t>();
	if (!r.ok || n > MAX_ITEMS) return false;
	b.cores.resize(n);
	for (Core &c : b.cores) {
		c.x = r.get<float>();
		c.y = r.get<float>();
		c.to = r.get<int32_t>();
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
	std::vector<std::pair<int, BodyRef>> edited;
	for (size_t i = 0; i < bs.items.size(); i++) {
		const Building &rec = bs.items[i];
		if (!rec.alive || rec.body == 0) continue;
		if (BodyRef body = bs.designs.edited->get(rec.body)) edited.emplace_back(int(i), std::move(body));
	}
	w.put<uint32_t>(uint32_t(edited.size()));
	for (const auto &e : edited) {
		w.put<int32_t>(e.first);
		write_body(w, *e.second);
	}
}

bool read_edited_bodies(ByteReader &r, BuildingStore &bs) {
	uint32_t n = r.get<uint32_t>();
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
