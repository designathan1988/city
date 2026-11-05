// The city as one number (the state hash of a deterministic game, as Factorio's map CRC): the save's own bytes hashed whole and by
// part, so whatever a save keeps is covered without a list to keep up, and a difference says which part it is in.
#include "api/city_probes.h"

#include "sim/persistence/save_format.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr uint64_t FNV_BASIS = 14695981039346656037ULL, FNV_PRIME = 1099511628211ULL; // FNV-1a, 64 bits

String hex_of(const uint8_t *from, const uint8_t *to) {
	uint64_t h = FNV_BASIS;
	for (const uint8_t *p = from; p != to; p++) h = (h ^ *p) * FNV_PRIME;
	return String::num_uint64(h, 16);
}
} // namespace

Dictionary CityProbes::state_hash() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::vector<SavePart> marks;
	const std::vector<uint8_t> bytes = encode_city(*state_, &marks);
	Dictionary parts;
	size_t from = 0;
	for (const SavePart &m : marks) {
		parts[String(m.name)] = hex_of(bytes.data() + from, bytes.data() + m.end);
		from = m.end;
	}
	Dictionary d;
	d["whole"] = hex_of(bytes.data(), bytes.data() + bytes.size());
	d["bytes"] = int64_t(bytes.size());
	d["parts"] = parts;
	return d;
}

} // namespace city
