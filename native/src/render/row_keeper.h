// Where each agent sits in the MultiMesh rows of its draw. The renderer keeps the last frame's rows beside the new ones and reads an
// instance's motion from its row in both (Godot renderer_rd/storage_rd/mesh_storage.cpp, _multimesh_set_buffer), and the temporal
// antialiasing lives on that motion: rows refilled in the order of the fleet shift by one when anybody leaves, and every car after
// it would report the jump to its neighbour's place. So an agent keeps its row; whoever leaves is replaced by the last row, one agent
// wrong for one frame, and no row is left empty to pay its vertices.
#pragma once

#include "render/rs_util.h"

#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace city {

class RowKeeper {
public:
	// `dense` keys (ids of a fleet, from 0) have a place in a table; the others a map.
	explicit RowKeeper(uint64_t dense = 0) : dense_limit_(dense) {}
	void begin() { frame_++; }
	void draws(size_t n) { owners_.resize(n); }
	int draw_of(uint64_t key) const {
		const Slot *found = find_slot(key);
		return found ? found->draw : -1;
	} // a query never allocates ownership of a row
	// The row of `key` in draw `draw`, its rows `rows`: its own if it was there on the last frame, else a new one at the end.
	float *place(uint64_t key, int draw, InstanceRows &rows) {
		Slot &s = slot(key);
		if (s.draw != draw) {
			s.draw = draw;
			s.row = rows.count();
			rows.add();
			owners_[size_t(draw)].push_back(key);
		}
		s.frame = frame_;
		return rows.row(s.row);
	}
	// Whoever was not placed this frame, or went to another draw, leaves its row; `rows_of(draw)` gives a draw's rows.
	template <typename RowsOf>
	void sweep(RowsOf &&rows_of) {
		for (int draw = 0; draw < int(owners_.size()); draw++) {
			std::vector<uint64_t> &owners = owners_[size_t(draw)];
			if (owners.empty()) continue;
			InstanceRows &rows = rows_of(draw);
			for (int i = 0; i < int(owners.size());) {
				const Slot *s = find_slot(owners[size_t(i)]);
				const bool mine = s && s->draw == draw && s->row == i;
				if (mine && s->frame == frame_) {
					i++;
					continue;
				}
				if (mine) forget(owners[size_t(i)]);
				const int last = int(owners.size()) - 1;
				if (i != last) { // the last row takes the place
					std::memcpy(rows.row(i), rows.row(last), sizeof(float) * size_t(rows.stride()));
					owners[size_t(i)] = owners[size_t(last)];
					Slot *moved = find_slot(owners[size_t(i)]);
					if (moved && moved->draw == draw && moved->row == last) moved->row = i;
				}
				owners.pop_back();
				rows.drop_last();
			}
		}
	}
	const std::vector<uint64_t> &owners(size_t draw) const { return owners_[draw]; }
	void clear() {
		dense_.clear();
		sparse_.clear();
		for (auto &list : owners_) list.clear();
	}

private:
	struct Slot {
		uint32_t frame = 0;
		int draw = -1, row = -1;
	};
	const Slot *find_slot(uint64_t key) const {
		if (key < dense_limit_) return key < dense_.size() ? &dense_[size_t(key)] : nullptr;
		auto it = sparse_.find(key);
		return it == sparse_.end() ? nullptr : &it->second;
	}
	Slot *find_slot(uint64_t key) {
		return const_cast<Slot *>(static_cast<const RowKeeper *>(this)->find_slot(key));
	}
	Slot &slot(uint64_t key) {
		if (key >= dense_limit_) return sparse_[key];
		if (key >= dense_.size()) dense_.resize(size_t(key) + 1 + size_t(key) / 2);
		return dense_[size_t(key)];
	}
	void forget(uint64_t key) {
		if (key >= dense_limit_) sparse_.erase(key);
		else dense_[size_t(key)] = Slot();
	}
	std::vector<Slot> dense_;
	std::unordered_map<uint64_t, Slot> sparse_;
	std::vector<std::vector<uint64_t>> owners_; // per draw: whose each row is
	uint64_t dense_limit_;
	uint32_t frame_ = 0;
};

} // namespace city
