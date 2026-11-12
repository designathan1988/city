// The rows of a MultiMesh: the layout of an instance and the buffer that holds them.
#pragma once

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cstring>

namespace city {

// Floats per instance: 12 transform + 4 colour + 4 custom. Props/agents/trees keep colour white and put
// their tint in custom.rgb (the instance colour would also multiply fixed-colour parts).
constexpr int MM_STRIDE = 20;
// Without the colour: 12 transform + 4 custom. The renderer keeps three copies of every instance's floats on the CPU, whether
// it interpolates them or not (Godot servers/rendering/storage/mesh_storage.cpp multimesh_allocate_data), so a thing that
// repeats by the thousand and has no colour of its own goes without.
constexpr int MM_PLAIN_STRIDE = 16;

// One instance into a row of InstanceRows, in the documented MultiMesh layout (row-major 3x4 transform, colour, custom).
inline void mm_row(float *w, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o, const godot::Color &col,
		float c0, float c1, float c2, float c3) {
	const float v[MM_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, col.r, col.g, col.b, col.a, c0, c1, c2, c3 };
	std::memcpy(w, v, sizeof(v));
}

// Appended to a buffer.
inline void mm_push(godot::PackedFloat32Array &buf, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z,
		const godot::Vector3 &o, const godot::Color &col, float c0, float c1, float c2, float c3) {
	int64_t at = buf.size();
	buf.resize(at + MM_STRIDE);
	mm_row(buf.ptrw() + at, x, y, z, o, col, c0, c1, c2, c3);
}

inline void mm_row_plain(float *w, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z, const godot::Vector3 &o, float c0, float c1,
		float c2, float c3) {
	const float v[MM_PLAIN_STRIDE] = { x.x, y.x, z.x, o.x, x.y, y.y, z.y, o.y, x.z, y.z, z.z, o.z, c0, c1, c2, c3 };
	std::memcpy(w, v, sizeof(v));
}

// The same without the colour, for a material that reads only INSTANCE_CUSTOM.
inline void mm_push_plain(godot::PackedFloat32Array &buf, const godot::Vector3 &x, const godot::Vector3 &y, const godot::Vector3 &z,
		const godot::Vector3 &o, float c0, float c1, float c2, float c3) {
	int64_t at = buf.size();
	buf.resize(at + MM_PLAIN_STRIDE);
	mm_row_plain(buf.ptrw() + at, x, y, z, o, c0, c1, c2, c3);
}

// The rows of a MultiMesh refilled every frame. The array lives from frame to frame at the size of the draw's room, so a frame costs
// one ptrw, a cursor per row and no allocation, and what goes to the RenderingServer is the array itself, the size it asks.
class InstanceRows {
public:
	explicit InstanceRows(int stride = MM_STRIDE) : stride_(stride) {}
	void restart() {
		count_ = 0;
		base_ = rows_.is_empty() ? nullptr : rows_.ptrw();
	}
	float *add() {
		if (int64_t(count_ + 1) * stride_ > rows_.size()) grow(count_ + 1);
		return base_ + int64_t(count_++) * stride_;
	}
	void append(const InstanceRows &more) {
		if (more.count_ == 0) return;
		if (int64_t(count_ + more.count_) * stride_ > rows_.size()) grow(count_ + more.count_);
		std::memcpy(base_ + int64_t(count_) * stride_, more.base_, sizeof(float) * size_t(more.count_) * size_t(stride_));
		count_ += more.count_;
	}
	const float *data() const { return rows_.is_empty() ? nullptr : rows_.ptr(); }
	int count() const { return count_; }
	int stride() const { return stride_; }
	// RowKeeper's: the rows stay from the last frame, each one written or dropped.
	void reopen() { base_ = rows_.is_empty() ? nullptr : rows_.ptrw(); }
	float *row(int i) { return base_ + int64_t(i) * stride_; }
	void drop_last() { count_--; }
	const godot::PackedFloat32Array &fitted(int room) {
		if (rows_.size() != int64_t(room) * stride_) {
			rows_.resize(int64_t(room) * stride_);
			base_ = rows_.ptrw();
		}
		return rows_;
	}

private:
	void grow(int wanted) {
		rows_.resize(int64_t(std::max(wanted, count_ * 2)) * stride_);
		base_ = rows_.ptrw();
	}
	godot::PackedFloat32Array rows_;
	float *base_ = nullptr;
	int count_ = 0, stride_;
};

} // namespace city
