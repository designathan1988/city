// Save/open: the worker still serializes under a shared map lock (immutable source snapshots remain required).
// Serialization uses stable pages; disk publication compresses and releases pages outside that lock.
#include "api/city_core.h"
#include "api/save_file.h"
#include "core/bytes.h"

#include "core/laps.h"
#include "sim/persistence/save_format.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>

#include <string>
#include <algorithm>
#include <array>
#include <utility>
#include <vector>

using namespace godot;

namespace city {

namespace {
	constexpr size_t FILE_TRANSFER_BYTES = 64 * 1024; // bounded transfer storage, independent of an untrusted length header

std::string utf8(const String &s) {
	return std::string(s.utf8().get_data());
}


} // namespace

void CityCore::join_save() {
	if (save_.thread.joinable()) save_.thread.join();
}

Dictionary CityCore::save_status() {
	Dictionary d;
	const int state = save_.state.load();
	d["busy"] = state == 1;
	d["ok"] = state == 2;
	d["error"] = state == 3 ? String(save_.error.c_str()) : String(); // written before the state turns 3
	d["bytes"] = save_.bytes.load();
	return d;
}

Dictionary CityCore::save_city(const String &path) {
	Dictionary d;
	d["ok"] = false;
	d["error"] = "save.blocked"; // no whole city on screen (every result carries its error key)
	join_save();
	{
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
		if (!state_->ready) return d;
	}
	const std::string target = utf8(path);
	save_.bytes.store(0);
	save_.error.clear(); // no save thread runs now (joined above)
	save_.state.store(1);
	save_.thread = std::thread([this, target]() {
		uint64_t revision = 0;
		bool ok = false;
		std::string error = "save.writeFailed";
		{
			BytePages bytes(std::numeric_limits<uint32_t>::max());
			bool encoded = false;
			{
				std::shared_lock<std::shared_mutex> lk(state_->map_mutex); // immutable/paged source capture remains open
				ByteWriter writer(bytes);
				write_city(writer, *state_);
				encoded = writer.good();
				revision = state_->revision;
			}
			save_.bytes.store(int64_t(bytes.size()));
			ok = encoded && write_save_file(target, bytes, error);
		} // retire all remaining pages before publishing idle/success/failure; the next save must not join that cleanup
		if (ok) {
			std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
			state_->saved_revision = revision;
			save_.state.store(2);
		} else {
			save_.error = error;
			save_.state.store(3);
		}
	});
	d["ok"] = true;
	d["error"] = "";
	d["pending"] = true;
	return d;
}

Dictionary CityCore::load_city(const String &path, bool paused) {
	join_save();
	Dictionary d;
	d["ok"] = false;
	Ref<FileAccess> f = FileAccess::open_compressed(path, FileAccess::READ, FileAccess::COMPRESSION_ZSTD);
	if (f.is_null() || !catalog_ok_) {
		d["error"] = "save.none";
		return d;
	}
	std::vector<uint8_t> bytes;
	std::array<uint8_t, FILE_TRANSFER_BYTES> buffer;
	const uint64_t length = f->get_length();
	bool complete = length <= bytes.max_size();
	while (complete && bytes.size() < length) {
		const uint64_t wanted = std::min(uint64_t(buffer.size()), length - bytes.size());
		const uint64_t got = f->get_buffer(buffer.data(), wanted);
		if (got != wanted) { complete = false; break; }
		bytes.insert(bytes.end(), buffer.begin(), buffer.begin() + std::ptrdiff_t(got));
	}
	f->close();
	std::string error = "save.file.damaged";
	if (!complete) { d["error"] = String(error.c_str()); return d; }
	auto candidate = make_candidate();
	Laps laps;
	OpenTimes times;
	if (!decode_city(*candidate, bytes.data(), bytes.size(), error, &times)) {
		d["error"] = String(error.c_str()); // refused before anything changed: the open city runs on
		return d;
	}
	Dictionary spent;
	spent["check"] = laps.lap();
	candidate->buildings.designs.edited->release_pending();
	traffic::reset_traffic(*candidate);
	spent["traffic"] = laps.lap();
	spent["read"] = times.read;
	spent["grade"] = times.grade;
	spent["compose"] = times.compose;
	spent["lots"] = times.lots;
	d["ms"] = spent;
	publish_city(std::move(candidate), true, paused);
	d["ok"] = true;
	d["error"] = String(error.c_str());
	return d;
}

} // namespace city
