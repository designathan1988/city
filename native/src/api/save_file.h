// Atomic disk publication of the existing Godot GCMP/ZSTD container, without FileAccessCompressed's full-file buffer.
#pragma once
#include <string>
namespace city {
class BytePages;
bool write_save_file(const std::string &target, BytePages &bytes, std::string &error);
} // namespace city
