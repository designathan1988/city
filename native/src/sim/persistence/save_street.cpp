// The street's block of the save: what the player placed (sim/street). A block of its own, a tag and its size in front, so a
// save without it opens with no street records and a reader skips what a later block adds to it.
#include "sim/persistence/save_street.h"

#include "core/bytes.h"
#include "sim/state/city_state.h"

namespace city {

namespace {
constexpr uint32_t STREET_TAG = 0x54525453U; // 'STRT'
constexpr uint32_t MAX_RECORDS = 50000000; // a corrupt count stops here, before any allocation

void write_records(ByteWriter &w, const StreetWorks &k) {
	w.put<uint32_t>(uint32_t(k.items.items.size()));
	for (const StreetItem &i : k.items.items) {
		w.put<uint8_t>(i.alive ? 1 : 0);
		if (!i.alive) continue;
		w.put<uint8_t>(uint8_t(i.kind));
		w.put<uint8_t>(uint8_t(i.sign));
		w.put<float>(i.x);
		w.put<float>(i.z);
		std::vector<uint8_t> text(i.text.begin(), i.text.end());
		w.put_vec(text);
	}
	w.put<uint32_t>(uint32_t(k.poles.items.size()));
	for (const UtilityPole &p : k.poles.items) {
		w.put<uint8_t>(p.alive ? 1 : 0);
		if (!p.alive) continue;
		w.put<float>(p.x);
		w.put<float>(p.z);
		w.put<uint8_t>(p.lamp ? 1 : 0);
	}
	w.put<uint32_t>(uint32_t(k.spans.items.size()));
	for (const UtilitySpan &p : k.spans.items) {
		w.put<uint8_t>(p.alive ? 1 : 0);
		if (!p.alive) continue;
		w.put<int32_t>(p.a);
		w.put<int32_t>(p.b);
	}
	w.put<uint32_t>(uint32_t(k.barriers.items.size()));
	for (const Barrier &b : k.barriers.items) {
		w.put<uint8_t>(b.alive ? 1 : 0);
		if (!b.alive) continue;
		w.put<uint8_t>(uint8_t(b.kind));
		w.put_vec(b.pts);
	}
	w.put<uint32_t>(uint32_t(k.cuts.items.size()));
	for (const StreetCut &c : k.cuts.items) {
		w.put<uint8_t>(c.alive ? 1 : 0);
		if (!c.alive) continue;
		w.put<float>(c.x);
		w.put<float>(c.z);
	}
}

bool read_records(ByteReader &r, StreetWorks &k) {
	uint32_t count = r.get<uint32_t>();
	if (!r.ok || count > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		StreetItem it;
		it.kind = ItemKind(r.get<uint8_t>());
		it.sign = SignType(r.get<uint8_t>());
		it.x = r.get<float>();
		it.z = r.get<float>();
		std::vector<uint8_t> text;
		r.get_vec(text);
		it.text.assign(text.begin(), text.end());
		if (int(it.kind) >= ITEM_KINDS || int(it.sign) >= SIGN_TYPES) return false;
		if (r.ok) k.put_item(int(i), it);
	}
	count = r.get<uint32_t>();
	if (!r.ok || count > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		UtilityPole p;
		p.x = r.get<float>();
		p.z = r.get<float>();
		p.lamp = r.get<uint8_t>() != 0;
		if (r.ok) k.put_pole(int(i), p);
	}
	count = r.get<uint32_t>();
	if (!r.ok || count > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		UtilitySpan s;
		s.a = r.get<int32_t>();
		s.b = r.get<int32_t>();
		const int poles = int(k.poles.items.size());
		if (s.a < 0 || s.b < 0 || s.a >= poles || s.b >= poles || !k.poles.items[size_t(s.a)].alive || !k.poles.items[size_t(s.b)].alive) return false;
		if (r.ok) k.put_span(int(i), s);
	}
	count = r.get<uint32_t>();
	if (!r.ok || count > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		Barrier b;
		b.kind = BarrierKind(r.get<uint8_t>());
		r.get_vec(b.pts);
		if (int(b.kind) >= BARRIER_KINDS) return false;
		if (r.ok) k.put_barrier(int(i), b);
	}
	count = r.get<uint32_t>();
	if (!r.ok || count > MAX_RECORDS) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		StreetCut c;
		c.x = r.get<float>();
		c.z = r.get<float>();
		if (r.ok) k.put_cut(int(i), c);
	}
	return r.ok;
}
} // namespace

void write_street(ByteWriter &w, const CityState &s) {
	ByteWriter block;
	write_records(block, s.street);
	w.put<uint32_t>(STREET_TAG);
	w.put<uint32_t>(uint32_t(block.buf.size()));
	w.put_bytes(block.buf.data(), block.buf.size());
}

bool read_street(ByteReader &r, CityState &s) {
	ByteReader peek = r;
	if (peek.get<uint32_t>() != STREET_TAG) return r.ok; // a save from before the block: no street records
	r.get<uint32_t>();
	const uint32_t size = r.get<uint32_t>();
	if (!r.ok || size > r.n - r.o) return false;
	ByteReader block;
	block.p = r.p + r.o;
	block.n = size;
	if (!read_records(block, s.street)) return false;
	r.o += size;
	return true;
}

} // namespace city
