// The tagged blocks of a save (save_blocks.h).
#include "sim/persistence/save_blocks.h"

#include "core/bytes.h"
#include "sim/persistence/save_demolition.h"
#include "sim/persistence/save_format.h"
#include "sim/persistence/save_links.h"
#include "sim/persistence/save_lots.h"
#include "sim/persistence/save_street.h"
#include "sim/state/city_state.h"

namespace city {

namespace {
// The payload `fill` writes, framed with its tag and size; nothing when it writes none.
template <typename F>
void put_framed(ByteWriter &w, uint32_t tag, F &&fill) {
	ByteWriter block;
	block.canonical = w.canonical;
	if (!fill(block)) return;
	w.put<uint32_t>(tag);
	w.put<uint32_t>(uint32_t(block.buf.size()));
	w.put_bytes(block.buf.data(), block.buf.size());
}
} // namespace

void write_extra_blocks(ByteWriter &w, const CityState &s, const std::function<void(const char *)> &written) {
	put_framed(w, SAVE_TAG_DEMOLITION, [&](ByteWriter &b) { return write_damage_block(b, s.damage); });
	written("demolition");
	put_framed(w, SAVE_TAG_STREET, [&](ByteWriter &b) { return write_street_block(b, s.street); });
	written("street");
	put_framed(w, SAVE_TAG_LOTS, [&](ByteWriter &b) { return write_lots_block(b, s.lot_book); });
	written("lots");
	put_framed(w, SAVE_TAG_LINKS, [&](ByteWriter &b) { return write_links_block(b, s.roads); });
	written("links");
	put_framed(w, SAVE_TAG_CROSSINGS, [&](ByteWriter &b) { return write_crossings_block(b, s.roads); });
	written("crossings");
}

bool read_extra_blocks(ByteReader &r, CityState &s) {
	for (;;) {
		const uint32_t tag = r.get<uint32_t>();
		if (!r.ok) return false;
		if (tag == SAVE_END) return true;
		const uint32_t size = r.get<uint32_t>();
		if (!r.ok || r.o + size > r.n) return false;
		ByteReader block{ r.p + r.o, size };
		if (tag == SAVE_TAG_DEMOLITION && !read_damage_block(block, s.damage)) return false;
		if (tag == SAVE_TAG_STREET && !read_street_block(block, s.street)) return false;
		if (tag == SAVE_TAG_LOTS && !read_lots_block(block, s.lot_book)) return false;
		if (tag == SAVE_TAG_LINKS && !read_links_block(block, s.roads)) return false;
		if (tag == SAVE_TAG_CROSSINGS && !read_crossings_block(block, s.roads)) return false;
		r.o += size; // a block this reader does not know is skipped by its size
	}
}

} // namespace city
