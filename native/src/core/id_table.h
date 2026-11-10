// A table of records under stable ids (IdTable). The live city's is a vector by id, as every store keeps its records (roads,
// buildings, what stands on the streets). A cut of the city (sim/state/city_cut.h: what a job copies under a short map lock) holds
// only the records of its place, under the same ids, so every reader indexes both alike. A record a cut does not hold stops the
// tests' DLL where it is read (core/checks.h: the cut is too small for what its job reads); the export's DLL reads a default one.
#pragma once

#include "core/checks.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace city {

template <typename T>
class IdTable {
public:
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

	// The ids the table spans: a cut spans its city's.
	size_t size() const { return sparse_ ? ids_ : items_.size(); }
	bool empty() const { return size() == 0; }
	T &operator[](size_t id) { return items_[slot(id)]; }
	const T &operator[](size_t id) const { return items_[slot(id)]; }
	// Every record held: by id in the live city, in the order they were put in a cut (after its default record).
	iterator begin() { return items_.begin(); }
	iterator end() { return items_.end(); }
	const_iterator begin() const { return items_.begin(); }
	const_iterator end() const { return items_.end(); }

	// The live table, changed as the vector it is.
	void resize(size_t n) { whole().resize(n); }
	void clear() {
		items_.clear();
		index_.clear();
		sparse_ = false;
		ids_ = 0;
	}
	void push_back(const T &rec) { whole().push_back(rec); }
	void push_back(T &&rec) { whole().push_back(std::move(rec)); }
	T &back() { return whole().back(); }
	const T &back() const { return whole().back(); }
	operator std::vector<T> &() { return whole(); }
	operator const std::vector<T> &() const { return whole(); }

	// A cut of a table of `ids` ids: empty until records are put.
	static IdTable cut(size_t ids) {
		IdTable t;
		t.sparse_ = true;
		t.ids_ = ids;
		t.items_.emplace_back(); // what an id it does not hold reads in the export's DLL
		return t;
	}
	bool holds(size_t id) const { return sparse_ ? index_.count(uint32_t(id)) != 0 : id < items_.size(); }
	// The record of `id` taken into a cut; true when it was not held yet.
	bool put(size_t id, const T &rec) {
		if (!index_.emplace(uint32_t(id), uint32_t(items_.size())).second) return false;
		items_.push_back(rec);
		return true;
	}

private:
	size_t slot(size_t id) const {
		if (!sparse_) return id;
		const auto it = index_.find(uint32_t(id));
		if (it != index_.end()) return it->second;
		if (CHECKS) city_stop("a record read that the job's cut of the city does not hold");
		return 0;
	}
	std::vector<T> &whole() {
		if (CHECKS && sparse_) city_stop("a cut of the city changed as the live table");
		return items_;
	}
	const std::vector<T> &whole() const {
		if (CHECKS && sparse_) city_stop("a cut of the city read as the live table");
		return items_;
	}
	std::vector<T> items_;
	std::unordered_map<uint32_t, uint32_t> index_; // a cut's: id -> place in items_
	size_t ids_ = 0;
	bool sparse_ = false;
};

} // namespace city
