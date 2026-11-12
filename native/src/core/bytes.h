// Little binary writer/reader used by the save format (sim/persistence).
#pragma once

#include "core/byte_pages.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace city {

struct ByteWriter {
	std::vector<uint8_t> buf;
	BytePages *pages = nullptr; // optional non-owning output; only the default writer uses buf
	bool ok = true;
	bool canonical = false; // state probes omit dead-slot counts; disk saves retain them
	ByteWriter() = default;
	explicit ByteWriter(BytePages &output) : pages(&output) {}
	bool good() const { return ok && (!pages || pages->good()); }
	void invalidate() { ok = false; }
	size_t size() const { return pages ? pages->size() : buf.size(); }
	void truncate(size_t n) {
		if (!good() || n > size()) { invalidate(); return; }
		if (pages) pages->truncate(n); else buf.resize(n);
	}
	template<class T> void patch(size_t at, const T &v) {
		static_assert(std::is_trivially_copyable_v<T>);
		if (!good()) return;
		if (pages) pages->patch(at, &v, sizeof(T));
		else {
		 if (at > buf.size() || sizeof(T) > buf.size() - at) { invalidate(); return; }
		 std::memcpy(buf.data() + at, &v, sizeof(T));
		}
	}
	void put_slots(size_t n) {
		if (!canonical) put<uint32_t>(uint32_t(n));
	}
	bool put_alive(bool alive) {
		if (!canonical) put<uint8_t>(alive ? 1 : 0);
		return alive;
	}
	template<class T> void put(const T &v) {
		static_assert(std::is_trivially_copyable_v<T>);
		put_bytes(&v, sizeof(T));
	}
	void put_bytes(const void *p, size_t n) {
		if (!good()) return;
		if (pages) { pages->append(p, n); return; }
		const size_t o = buf.size();
		if (n > buf.max_size() - o) { invalidate(); return; }
		buf.resize(o + n);
		if (n) std::memcpy(buf.data() + o, p, n);
	}
	template<class T> void put_vec(const std::vector<T> &v) {
		put<uint32_t>(uint32_t(v.size()));
		put_bytes(v.data(), v.size() * sizeof(T));
	}
};

struct ByteReader {
	const uint8_t *p = nullptr;
	size_t n = 0, o = 0;
	bool ok = true;
	size_t remaining() const { return o <= n ? n - o : 0; }
	bool require(size_t bytes) {
		if (!ok || o > n || bytes > remaining()) { ok = false; return false; }
		return true;
	}
	bool require_items(size_t count, size_t item_bytes, size_t max_count = std::numeric_limits<size_t>::max()) {
		// Division validates the product without overflowing. Check before any resize/reserve/slot insertion.
		if (!require(0) || item_bytes == 0 || count > max_count || count > remaining() / item_bytes) { ok = false; return false; }
		return true;
	}
	template <typename T>
	T get() {
		static_assert(std::is_trivially_copyable_v<T>);
		T v{};
		if (!require(sizeof(T))) return v;
		std::memcpy(&v, p + o, sizeof(T));
		o += sizeof(T);
		return v;
	}
	void get_bytes(void *dst, size_t c) {
		if (!require(c)) return;
		if (c) std::memcpy(dst, p + o, c);
		o += c;
	}
	template <typename T>
	void get_vec(std::vector<T> &v, size_t max_count = 200000000) {
		static_assert(std::is_trivially_copyable_v<T>);
		uint32_t c = get<uint32_t>();
		if (!require_items(c, sizeof(T), max_count)) return;
		v.resize(c);
		get_bytes(v.data(), size_t(c) * sizeof(T));
	}
	void get_string(std::string &v) {
		const uint32_t count = get<uint32_t>();
		if (!require_items(count, sizeof(char))) return;
		v.resize(count);
		get_bytes(v.data(), count);
	}
};

} // namespace city
