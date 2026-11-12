#include "api/save_file.h"
#include "core/byte_pages.h"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <vector>

namespace city {
using namespace godot;
namespace {
constexpr uint32_t BLOCK_BYTES = 4096; // FileAccessCompressed::configure default; GCMP size fields are uint32.
constexpr uint8_t MAGIC[] = { 'G', 'C', 'M', 'P' };
// RFC 8878: single segment, content size 0, last raw block of size 0. PackedByteArray::compress skips empty inputs.
constexpr uint8_t EMPTY_ZSTD[] = { 0x28, 0xb5, 0x2f, 0xfd, 0x20, 0, 1, 0, 0 };
static_assert(BytePages::PAGE_BYTES % BLOCK_BYTES == 0);

bool store_bytes(const Ref<FileAccess> &file, const uint8_t *data, size_t size) {
	const uint64_t start = file->get_position();
	file->store_buffer(data, size); // godot-cpp's raw-pointer overload returns void
	return file->get_error() == OK && file->get_position() == start + size;
}

bool write_compressed(const Ref<FileAccess> &file, BytePages &bytes) {
	if (!bytes.good() || bytes.size() > std::numeric_limits<uint32_t>::max()) return false; // GCMP cannot represent a larger file.
	const uint32_t total = uint32_t(bytes.size()), count = total / BLOCK_BYTES + 1;
	if (!store_bytes(file, MAGIC, sizeof(MAGIC)) || !file->store_32(FileAccess::COMPRESSION_ZSTD) ||
			!file->store_32(BLOCK_BYTES) || !file->store_32(total)) return false;
	// Only the block-size index is retained (at most 4 MiB for the uint32 container), never a second city payload.
	std::vector<uint32_t> sizes;
	sizes.reserve(count);
	const std::array<uint8_t, BytePages::PAGE_BYTES> zero{};
	size_t left = size_t(count) * sizeof(uint32_t);
	while (left) {
		const size_t n = std::min(left, zero.size());
		if (!store_bytes(file, zero.data(), n)) return false;
		left -= n;
	}
	PackedByteArray input;
	if (!bytes.drain([&](const uint8_t *page, size_t n) {
		for (size_t at = 0; at < n; at += BLOCK_BYTES) {
			const size_t size = std::min(size_t(BLOCK_BYTES), n - at);
			if (input.resize(int64_t(size)) != OK) return false;
			std::memcpy(input.ptrw(), page + at, size);
			const PackedByteArray compressed = input.compress(FileAccess::COMPRESSION_ZSTD);
			if (compressed.is_empty() || !file->store_buffer(compressed)) return false;
			sizes.push_back(uint32_t(compressed.size()));
		}
		return true;
	})) return false;
	if (total % BLOCK_BYTES == 0) {
		if (!store_bytes(file, EMPTY_ZSTD, sizeof(EMPTY_ZSTD))) return false;
		sizes.push_back(uint32_t(sizeof(EMPTY_ZSTD)));
	}
	if (sizes.size() != count) return false;
	const uint64_t end = file->get_position();
	file->seek(16); // GCMP header followed by one little-endian compressed length per block.
	if (file->get_position() != 16) return false;
	for (uint32_t size : sizes) if (!file->store_32(size)) return false;
	file->seek(end);
	if (file->get_position() != end) return false;
	if (!store_bytes(file, MAGIC, sizeof(MAGIC))) return false;
	file->flush();
	return file->get_error() == OK;
}
} // namespace

bool write_save_file(const std::string &target, BytePages &bytes, std::string &error) {
	const String to(target.c_str()), tmp = to + String(".tmp"), old = to + String(".old");
	error = "save.writeFailed";
	bool complete = false;
	Ref<FileAccess> file = FileAccess::open(tmp, FileAccess::WRITE);
	if (file.is_valid()) {
		complete = write_compressed(file, bytes);
		file->close();
	}
	if (!complete) { DirAccess::remove_absolute(tmp); return false; }
	if (FileAccess::file_exists(old) && DirAccess::remove_absolute(old) != OK) { DirAccess::remove_absolute(tmp); return false; }
	const bool had_previous = FileAccess::file_exists(to);
	if (had_previous && DirAccess::rename_absolute(to, old) != OK) { DirAccess::remove_absolute(tmp); return false; }
	if (DirAccess::rename_absolute(tmp, to) != OK) {
		if (had_previous) DirAccess::rename_absolute(old, to);
		DirAccess::remove_absolute(tmp); return false;
	}
	if (had_previous) DirAccess::remove_absolute(old);
	error.clear();
	return true;
}
} // namespace city
