// See save_lots.h.
#include "sim/persistence/save_lots.h"

#include "core/bytes.h"
#include "sim/lots/lot_book.h"

namespace city {

namespace {
constexpr uint32_t MOST_LOTS = 1000000, MOST_CORNERS = 4096;
} // namespace

bool write_lots_block(ByteWriter &w, const LotBook &book) {
	if (book.lots.empty()) return false;
	w.put<int32_t>(book.next_id);
	w.put<uint32_t>(uint32_t(book.lots.size()));
	for (const PlayerLot &l : book.lots) {
		w.put<int32_t>(l.id);
		w.put<int32_t>(l.zone);
		w.put<uint32_t>(uint32_t(l.corners.size()));
		for (const LotPoint &p : l.corners) {
			w.put<double>(p.x);
			w.put<double>(p.z);
		}
	}
	return true;
}

bool read_lots_block(ByteReader &r, LotBook &book) {
	book.clear();
	book.next_id = r.get<int32_t>();
	const uint32_t n = r.get<uint32_t>();
	if (!r.ok || n > MOST_LOTS) return false;
	book.lots.resize(n);
	for (PlayerLot &l : book.lots) {
		l.id = r.get<int32_t>();
		l.zone = r.get<int32_t>();
		const uint32_t corners = r.get<uint32_t>();
		if (!r.ok || corners > MOST_CORNERS) return false;
		l.corners.resize(corners);
		for (LotPoint &p : l.corners) {
			p.x = r.get<double>();
			p.z = r.get<double>();
		}
	}
	return r.ok;
}

} // namespace city
