// Little binary writer/reader used by the save format (sim/persistence).
#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace city {

struct ByteWriter {
	std::vector<uint8_t> buf;
	bool canonical = false; // the city as a number: what a save keeps for its ids, dead slots and their count, is left out
	void put_slots(size_t n) {
		if (!canonical) put<uint32_t>(uint32_t(n));
	}
	bool put_alive(bool alive) {
		if (!canonical) put<uint8_t>(alive ? 1 : 0);
		return alive;
	}
	template <typename T>
	void put(const T &v) {
		size_t o = buf.size();
		buf.resize(o + sizeof(T));
		std::memcpy(buf.data() + o, &v, sizeof(T));
	}
	void put_bytes(const void *p, size_t n) {
		size_t o = buf.size();
		buf.resize(o + n);
		if (n) std::memcpy(buf.data() + o, p, n);
	}
	template <typename T>
	void put_vec(const std::vector<T> &v) {
		put<uint32_t>(uint32_t(v.size()));
		put_bytes(v.data(), v.size() * sizeof(T));
	}
	void put_string(const std::string &s) { // its length, then its bytes: get_string reads it
		put<uint32_t>(uint32_t(s.size()));
		put_bytes(s.data(), s.size());
	}
	// The one owner of order for what the state keeps unordered: the save and the state hash are functions of the state, so
	// equal contents give equal bytes whatever history built them. A hash map: its count, then `entry` for each (key, value)
	// in key order.
	template <typename Map, typename Entry>
	void put_by_key(const Map &m, Entry entry) {
		std::vector<const typename Map::value_type *> order;
		order.reserve(m.size());
		for (const auto &kv : m) order.push_back(&kv);
		std::sort(order.begin(), order.end(), [](const auto *a, const auto *b) { return a->first < b->first; });
		put<uint32_t>(uint32_t(order.size()));
		for (const auto *kv : order) entry(*kv);
	}
	// A list whose order means nothing (a set kept in hash cells, or removed from by swap): as put_vec, in the order `less`
	// gives its values.
	template <typename T, typename Less>
	void put_set(std::vector<T> items, Less less) {
		std::sort(items.begin(), items.end(), less);
		put_vec(items);
	}
};

// The reader is the one owner of what a count may claim: every count is read with the least bytes each of its items takes, and
// one that could not fit in what is left of the bytes is refused before anything is sized by it. Readers keep only the limits
// that mean something to the game (a catalogue's most clouds, a lot's corners); a damaged file stops here.
struct ByteReader {
	const uint8_t *p = nullptr;
	size_t n = 0, o = 0;
	bool ok = true;
	template <typename T>
	T get() {
		T v{};
		if (o + sizeof(T) > n) {
			ok = false;
			return v;
		}
		std::memcpy(&v, p + o, sizeof(T));
		o += sizeof(T);
		return v;
	}
	void get_bytes(void *dst, size_t c) {
		if (o + c > n) {
			ok = false;
			return;
		}
		std::memcpy(dst, p + o, c);
		o += c;
	}
	// Whether `count` items of at least `min_item_bytes` (one or more) each can still follow; ok turns false when they cannot.
	// For a count read before its items (a table of sizes up front): check it here before sizing anything by it.
	bool claim(uint64_t count, size_t min_item_bytes) {
		if (ok && count > uint64_t(n - o) / std::max<size_t>(min_item_bytes, 1)) ok = false;
		return ok;
	}
	// A count of items that each take at least `min_item_bytes` (one or more) of what follows: 0 and ok false when that many
	// could not be there.
	uint32_t get_count(size_t min_item_bytes) {
		const uint32_t c = get<uint32_t>();
		return claim(c, min_item_bytes) ? c : 0;
	}
	template <typename T>
	void get_vec(std::vector<T> &v) {
		const uint32_t c = get_count(sizeof(T));
		if (!ok) return;
		v.resize(c);
		get_bytes(v.data(), size_t(c) * sizeof(T));
	}
	// The same, with the most items the game allows (a semantic limit, not a guard against damage).
	template <typename T>
	void get_vec(std::vector<T> &v, size_t most) {
		const uint32_t c = get_count(sizeof(T));
		if (!ok || c > most) {
			ok = false;
			return;
		}
		v.resize(c);
		get_bytes(v.data(), size_t(c) * sizeof(T));
	}
	std::string get_string() { // what put_string wrote
		const uint32_t c = get_count(1);
		std::string s;
		if (!ok) return s;
		s.assign(reinterpret_cast<const char *>(p + o), c);
		o += c;
		return s;
	}
};

} // namespace city
