// Seekable serialization pages. Appending never moves previous bytes; after framing is complete the consumer
// releases each page as it writes it. This removes whole-file reallocations, not the snapshot's total size.
#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <limits>
#include <memory>

namespace city {
class BytePages {
public:
	static constexpr size_t PAGE_BYTES = 64 * 1024;
	using Page = std::array<uint8_t, PAGE_BYTES>;
	explicit BytePages(size_t maximum = std::numeric_limits<size_t>::max()) : maximum_(maximum) {}
	size_t size() const { return size_; }
	bool good() const { return good_; }
	void append(const void *source, size_t count) {
		if (!good_ || draining_ || count > maximum_ - size_) { good_ = false; return; }
		const auto *p = static_cast<const uint8_t *>(source);
		while (count) {
			const size_t offset = size_ % PAGE_BYTES;
			if (!offset) pages_.emplace_back(new Page);
			const size_t n = std::min(count, PAGE_BYTES - offset);
			std::memcpy(pages_.back()->data() + offset, p, n);
			p += n; count -= n; size_ += n;
		}
	}
	void patch(size_t at, const void *source, size_t count) {
		if (!good_ || draining_ || at > size_ || count > size_ - at) { good_ = false; return; }
		const auto *p = static_cast<const uint8_t *>(source);
		while (count) {
			const size_t offset = at % PAGE_BYTES, n = std::min(count, PAGE_BYTES - offset);
			std::memcpy(pages_[at / PAGE_BYTES]->data() + offset, p, n);
			at += n; p += n; count -= n;
		}
	}
	void truncate(size_t size) {
		if (!good_ || draining_ || size > size_) { good_ = false; return; }
		const size_t keep = size / PAGE_BYTES + (size % PAGE_BYTES != 0);
		while (pages_.size() > keep) pages_.pop_back();
		size_ = size;
	}
	template<class Write> bool drain(Write &&write) {
		if (!good_ || draining_) return false;
		draining_ = true;
		size_t left = size_;
		while (!pages_.empty()) {
			const size_t n = std::min(left, PAGE_BYTES);
			if (!write(pages_.front()->data(), n)) return false;
			pages_.pop_front(); left -= n;
		}
		return true;
	}
private:
	std::deque<std::unique_ptr<Page>> pages_;
	size_t size_ = 0, maximum_;
	bool good_ = true;
	bool draining_ = false;
};
} // namespace city
