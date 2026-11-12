// The street's block of the save: what the player placed (sim/street), one of the tagged blocks (save_blocks.h): a save
// without it opens with no street records.
#include "sim/persistence/save_street.h"

#include "sim/persistence/save_format.h"

#include "core/bytes.h"
#include "sim/state/city_state.h"

namespace city {

namespace {
// A pole's flags byte: its lamp, and its kind (a save from before the kinds holds 0 or 1: every pole urban).
constexpr uint8_t POLE_LAMP_BIT = 1, POLE_RURAL_BIT = 2;

void write_records(ByteWriter &w, const StreetWorks &k) {
	w.put_slots(k.items.items.size());
	for (const StreetItem &i : k.items.items) {
		if (!w.put_alive(i.alive)) continue;
		w.put<uint8_t>(uint8_t(i.kind));
		w.put<uint8_t>(uint8_t(i.sign));
		w.put<float>(i.x);
		w.put<float>(i.z);
		std::vector<uint8_t> text(i.text.begin(), i.text.end());
		w.put_vec(text);
	}
	w.put_slots(k.poles.items.size());
	for (const UtilityPole &p : k.poles.items) {
		if (!w.put_alive(p.alive)) continue;
		w.put<float>(p.x);
		w.put<float>(p.z);
		w.put<uint8_t>(uint8_t((p.lamp ? POLE_LAMP_BIT : 0) | (p.kind == PoleKind::RURAL ? POLE_RURAL_BIT : 0)));
	}
	w.put_slots(k.spans.items.size());
	for (const UtilitySpan &p : k.spans.items) {
		if (!w.put_alive(p.alive)) continue;
		w.put<int32_t>(p.a);
		w.put<int32_t>(p.b);
	}
	w.put_slots(k.barriers.items.size());
	for (const Barrier &b : k.barriers.items) {
		if (!w.put_alive(b.alive)) continue;
		w.put<uint8_t>(uint8_t(b.kind));
		w.put_vec(b.pts);
	}
	w.put_slots(k.cuts.items.size());
	for (const StreetCut &c : k.cuts.items) {
		if (!w.put_alive(c.alive)) continue;
		w.put<float>(c.x);
		w.put<float>(c.z);
	}
}

bool read_records(ByteReader &r, StreetWorks &k) {
	uint32_t count = r.get<uint32_t>();
	if (!r.require_items(count, sizeof(uint8_t), MAX_RECORDS)) return false;
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
	if (!r.require_items(count, sizeof(uint8_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		UtilityPole p;
		p.x = r.get<float>();
		p.z = r.get<float>();
		const uint8_t flags = r.get<uint8_t>();
		if ((flags & ~(POLE_LAMP_BIT | POLE_RURAL_BIT)) != 0) return false;
		p.lamp = (flags & POLE_LAMP_BIT) != 0;
		p.kind = (flags & POLE_RURAL_BIT) != 0 ? PoleKind::RURAL : PoleKind::URBAN;
		if (r.ok) k.put_pole(int(i), p);
	}
	count = r.get<uint32_t>();
	if (!r.require_items(count, sizeof(uint8_t), MAX_RECORDS)) return false;
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
	if (!r.require_items(count, sizeof(uint8_t), MAX_RECORDS)) return false;
	for (uint32_t i = 0; i < count && r.ok; i++) {
		if (!r.get<uint8_t>()) continue;
		Barrier b;
		b.kind = BarrierKind(r.get<uint8_t>());
		r.get_vec(b.pts);
		if (int(b.kind) >= BARRIER_KINDS) return false;
		if (r.ok) k.put_barrier(int(i), b);
	}
	count = r.get<uint32_t>();
	if (!r.require_items(count, sizeof(uint8_t), MAX_RECORDS)) return false;
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

bool write_street_block(ByteWriter &w, const StreetWorks &k) {
	if (k.items.alive == 0 && k.poles.alive == 0 && k.spans.alive == 0 && k.barriers.alive == 0 && k.cuts.alive == 0) return false;
	write_records(w, k);
	return true;
}

bool read_street_block(ByteReader &r, StreetWorks &k) { return read_records(r, k); }

} // namespace city
