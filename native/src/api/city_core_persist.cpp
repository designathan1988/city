// Save and open (sim/persistence): a worker thread encodes the city under the map lock shared (it only reads), then
// writes it with ZSTD through a temporary file and renames it, so the frame never waits for the save. A city opened is read
// once into a state of its own while the open one runs on, and taken in whole under the exclusive lock.
#include "api/city_core.h"

#include "core/laps.h"
#include "sim/persistence/save_format.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace godot;

namespace city {

namespace {

std::string utf8(const String &s) {
	return std::string(s.utf8().get_data());
}

// Writes `bytes` to `target` atomically: temporary file, old generation aside, rename, then drop the old one.
bool write_atomic(const std::string &target, const std::vector<uint8_t> &bytes, std::string &error) {
	const String tmp = String((target + ".tmp").c_str());
	Ref<FileAccess> f = FileAccess::open_compressed(tmp, FileAccess::WRITE, FileAccess::COMPRESSION_ZSTD);
	if (f.is_null()) {
		error = "save.writeFailed";
		return false;
	}
	f->store_buffer(bytes.data(), bytes.size());
	f->close();
	const String to = String(target.c_str());
	const String old = to + String(".old");
	DirAccess::remove_absolute(old);
	if (FileAccess::file_exists(to)) DirAccess::rename_absolute(to, old);
	if (DirAccess::rename_absolute(tmp, to) != OK) {
		if (FileAccess::file_exists(old)) DirAccess::rename_absolute(old, to);
		DirAccess::remove_absolute(tmp);
		error = "save.writeFailed";
		return false;
	}
	DirAccess::remove_absolute(old);
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
	const std::string target = utf8(path);
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

Dictionary CityCore::load_city(const String &path, bool paused) {
	join_save();
	Dictionary d;
	d["ok"] = false;
	Ref<FileAccess> f = FileAccess::open_compressed(path, FileAccess::READ, FileAccess::COMPRESSION_ZSTD);
	if (f.is_null() || !catalog_ok_) {
		d["error"] = "save.none";
		return d;
	}
	std::vector<uint8_t> bytes(f->get_length());
	uint64_t got = f->get_buffer(bytes.data(), bytes.size());
	f->close();
	std::string error = "save.file.damaged";
	if (got != bytes.size()) {
		d["error"] = String(error.c_str());
		return d;
	}
	// The city is read once, aside, while the open one runs on: it is taken in whole only when the file is whole.
	auto read = std::make_unique<CityState>(state_->jobs);
	{
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex); // the road variants change it under the exclusive lock
		read->catalog = state_->catalog;
	}
	OpenTimes times;
	if (!decode_city(*read, bytes.data(), bytes.size(), error, &times)) {
		d["error"] = String(error.c_str()); // refused before anything changed: the open city runs on
		return d;
	}
	Dictionary spent;
	Laps laps;
	sim_.stop();
	// No job in flight while the city is taken in: take_city moves the catalogue and the tables, which a mesh job reads after it
	// let the map go (its cut refers to them). Paused before the map is taken: a job still gathering needs it shared to end.
	state_->jobs->pause();
	{
		std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
		state_->interior = InteriorView(); // the layer closes with the city it showed
		take_city(*state_, *read);
		spent["take"] = laps.lap();
		traffic::reset_traffic(*state_); // a load: full lane graph and an empty fleet
		spent["traffic"] = laps.lap();
	}
	state_->jobs->resume();
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
