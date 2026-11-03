// Little binary writer/reader used by the save format (sim/persistence).
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace city {

struct ByteWriter {
	std::vector<uint8_t> buf;
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
};

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
	template <typename T>
	void get_vec(std::vector<T> &v, size_t max_count = 200000000) {
		uint32_t c = get<uint32_t>();
		if (!ok || c > max_count || o + size_t(c) * sizeof(T) > n) {
			ok = false;
			return;
		}
		v.resize(c);
		get_bytes(v.data(), size_t(c) * sizeof(T));
	}
};

} // namespace city
