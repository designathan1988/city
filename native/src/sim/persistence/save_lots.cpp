// See save_lots.h.
#include "sim/persistence/save_lots.h"

#include "core/bytes.h"
#include "sim/lots/lot_book.h"

#include <cmath>
#include <unordered_set>

namespace city {

namespace {
constexpr uint32_t MOST_LOTS = 1000000, MOST_CORNERS = 4096;
} // namespace

bool write_lots_block(ByteWriter &w, const LotBook &book) {
	if (book.empty()) return false;
	w.put<int32_t>(book.next_id);
	w.put<uint32_t>(uint32_t(book.records().size()));
	for (const PlayerLot &l : book.records()) {
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
	const int next_id = r.get<int32_t>();
	const uint32_t n = r.get<uint32_t>();
	constexpr size_t LOT_HEADER_BYTES = 2 * sizeof(int32_t) + sizeof(uint32_t), CORNER_BYTES = 2 * sizeof(double);
	if (next_id <= 0 || !r.require_items(n, LOT_HEADER_BYTES, MOST_LOTS)) return false;
	std::unordered_set<int> ids;
	std::vector<PlayerLot> lots(n);
	for (PlayerLot &l : lots) {
		l.id = r.get<int32_t>();
		l.zone = r.get<int32_t>();
		const uint32_t corners = r.get<uint32_t>();
		if (l.id <= 0 || l.id >= next_id || !ids.insert(l.id).second || l.zone < 0 || corners < 3 || !r.require_items(corners, CORNER_BYTES, MOST_CORNERS)) return false;
		l.corners.resize(corners);
		for (LotPoint &p : l.corners) {
			p.x = r.get<double>();
			p.z = r.get<double>();
			if (!std::isfinite(p.x) || !std::isfinite(p.z)) return false;
		}
	}
	if (!r.ok) return false;
	book.restore(std::move(lots), next_id);
	return true;
}

} // namespace city
