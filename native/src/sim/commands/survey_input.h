// Immutable reference input views. The source owns its indexed vertex/image storage; survey loops need no expanded soup.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace city {
struct SurveyPoint { float x = 0, y = 0, z = 0; };
using SurveyTriangle = std::array<SurveyPoint, 3>;

class SurveyTriangles {
public:
	using Read = SurveyTriangle (*)(const void *, size_t, size_t &);
	SurveyTriangles() = default;
	SurveyTriangles(std::shared_ptr<const void> root, const void *context, size_t count, Read read) :
			root_(std::move(root)), context_(context), count_(count), read_(read) {}
	// Only for a synchronous legacy call whose vector already outlives the complete survey.
	static SurveyTriangles borrow(const std::vector<SurveyTriangle> &values) {
		return SurveyTriangles({}, &values, values.size(), [](const void *p, size_t i, size_t &) {
			return (*static_cast<const std::vector<SurveyTriangle> *>(p))[i];
		});
	}
	struct Iterator {
		const SurveyTriangles *source;
		size_t at = 0;
		mutable size_t hint = 0; // a sequential source's part cursor; each pass/reader has its own
		SurveyTriangle operator*() const { return source->read_(source->context_, at, hint); }
		Iterator &operator++() { ++at; return *this; }
		bool operator!=(const Iterator &other) const { return source != other.source || at != other.at; }
	};
	Iterator begin() const { return Iterator{this, 0, 0}; }
	Iterator end() const { return Iterator{this, count_, 0}; }
	size_t size() const { return count_; }
	bool empty() const { return count_ == 0; }
private:
	std::shared_ptr<const void> root_;
	const void *context_ = nullptr;
	size_t count_ = 0;
	Read read_ = nullptr;
};

struct SurveyPixels {
	std::shared_ptr<const void> root;
	const uint8_t *pixels = nullptr;
	size_t count = 0;
	size_t size() const { return count; }
	const uint8_t *data() const { return pixels; }
	uint8_t operator[](size_t i) const { return pixels[i]; }
};
} // namespace city
