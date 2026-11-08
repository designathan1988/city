// Save and open (sim/persistence): a worker thread encodes the city under the map lock shared (it only reads), then
// writes it with ZSTD through a temporary file and one replacing rename, so the frame never waits for the save. A city opened is read
// once into a state of its own while the open one runs on, and taken in whole under the exclusive lock.
#include "api/city_core.h"

#include "core/laps.h"
#include "sim/persistence/save_format.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/project_settings.hpp>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace godot;

namespace city {

namespace {

std::string utf8(const String &s) {
	return std::string(s.utf8().get_data());
}

// The OS path of a Godot path (user://, res://, or already absolute), as std::filesystem takes it. Main thread: ProjectSettings.
std::filesystem::path os_path(const String &path) { return std::filesystem::u8path(utf8(ProjectSettings::get_singleton()->globalize_path(path))); }

// Where a save goes: the file, and the temporary file it is written to first.
struct SaveTarget {
	String tmp;
	std::filesystem::path tmp_os, to_os;
};

// Writes `bytes` through the temporary file and one replacing rename. std::filesystem::rename replaces in one step (MoveFileExW
// with MOVEFILE_REPLACE_EXISTING on Windows, rename(2) elsewhere: libc++ posix_compat.h), where DirAccess's rename deletes the
// old file first (DirAccessWindows::rename): no moment holds neither the old save nor the new one, so a crash leaves one whole.
bool write_atomic(const SaveTarget &target, const std::vector<uint8_t> &bytes, std::string &error) {
	Ref<FileAccess> f = FileAccess::open_compressed(target.tmp, FileAccess::WRITE, FileAccess::COMPRESSION_ZSTD);
	if (f.is_null()) {
		error = "save.writeFailed";
		return false;
	}
	f->store_buffer(bytes.data(), bytes.size());
	f->close();
	std::error_code ec;
	std::filesystem::rename(target.tmp_os, target.to_os, ec);
	if (ec) {
		std::filesystem::remove(target.tmp_os, ec);
		error = "save.writeFailed";
		return false;
	}
	return true;
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
	const SaveTarget target{ path + String(".tmp"), os_path(path + String(".tmp")), os_path(path) };
	const size_t expect = size_t(std::max<int64_t>(save_.bytes.load(), 0)); // the last save's size: the new one is about as big
	save_.bytes.store(0);
	save_.error.clear(); // no save thread runs now (joined above)
	save_.state.store(1);
	save_.thread = std::thread([this, target, expect]() {
		std::vector<uint8_t> bytes;
		uint64_t revision = 0;
		{
			std::shared_lock<std::shared_mutex> lk(state_->map_mutex); // the encode only reads the city
			bytes = encode_city(*state_, nullptr, expect);
			revision = state_->revision; // the revision this file holds
		}
		save_.bytes.store(int64_t(bytes.size()));
		std::string error;
		const bool ok = write_atomic(target, bytes, error);
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

void CityCore::join_load() {
	if (load_.thread.joinable()) load_.thread.join();
	load_.read.reset();
	load_.state.store(0);
}

Dictionary CityCore::load_city(const String &path, bool paused) {
	join_save();
	join_load();
	Dictionary d;
	d["ok"] = false;
	Ref<FileAccess> f = FileAccess::open_compressed(path, FileAccess::READ, FileAccess::COMPRESSION_ZSTD);
	if (f.is_null() || !catalog_ok_) {
		d["error"] = "save.none";
		return d;
	}
	// The file is read here, by the engine, on the main thread; only the decoding, plain C++, runs aside.
	std::vector<uint8_t> bytes(f->get_length());
	const uint64_t got = f->get_buffer(bytes.data(), bytes.size());
	f->close();
	if (got != bytes.size()) {
		d["error"] = "save.file.damaged";
		return d;
	}
	// The city is read once, aside, while the open one runs on: it is taken in whole only when the file is whole.
	load_.read = std::make_unique<CityState>(state_->jobs);
	{
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex); // the road variants change it under the exclusive lock
		load_.read->catalog = state_->catalog;
	}
	load_.error.clear();
	load_.times = OpenTimes{};
	load_.paused = paused;
	load_.state.store(1);
	load_.thread = std::thread([this, bytes = std::move(bytes)]() {
		std::string error = "save.file.damaged";
		const bool whole = decode_city(*load_.read, bytes.data(), bytes.size(), error, &load_.times);
		load_.error = error;
		load_.state.store(whole ? 2 : 3);
	});
	d["ok"] = true;
	d["error"] = "";
	d["pending"] = true;
	return d;
}

Dictionary CityCore::load_poll() {
	Dictionary d;
	const int state = load_.state.load();
	if (state == 1) {
		d["pending"] = true;
		return d;
	}
	d["ok"] = false;
	if (state == 0) {
		d["error"] = "save.none"; // nothing is being opened
		return d;
	}
	if (load_.thread.joinable()) load_.thread.join();
	std::string error = load_.error;
	if (state == 3) {
		join_load();
		d["error"] = String(error.c_str()); // refused before anything changed: the open city runs on
		return d;
	}
	std::unique_ptr<CityState> read = std::move(load_.read);
	const OpenTimes times = load_.times;
	const bool paused = load_.paused;
	join_load();
	Dictionary spent;
	Laps laps;
	sim_.stop();
	state_->jobs->pause(); // no job is inside the city while it is replaced: some read the catalogue and the tables outside the lock
	{
		std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
		state_->interior = InteriorView(); // the layer closes with the city it showed
		take_city(*state_, *read);
		state_->jobs->resume();
		spent["take"] = laps.lap();
		traffic::reset_traffic(*state_); // a load: full lane graph and an empty fleet
		spent["traffic"] = laps.lap();
	}
	read.reset(); // the old city, freed with the lock let go
	spent["read"] = times.read;
	spent["grade"] = times.grade;
	spent["compose"] = times.compose;
	spent["lots"] = times.lots;
	d["ms"] = spent;
	need_full_rebuild_ = true;
	sim_.start(*state_, paused); // paused: no tick between the opening and whoever looks at the city
	d["ok"] = true;
	d["error"] = String(error.c_str());
	return d;
}

} // namespace city
