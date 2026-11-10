// The ECMAScript built-ins a translated program calls: Map and Set in insertion order (ECMA-262 24.1, 24.2), with ECMAScript's semantics (ECMA-262, 15th edition,
// sections cited at each one). Each is checked against Node by the converter (test/js-runtime.test.ts).
#pragma once

#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace js {


/**
 * A Map as JavaScript's: entries in insertion order (a key set again keeps its place; deleted and set again, it goes
 * last), found by hash. A delete leaves a hole that iteration skips, so deleting while iterating is safe as in
 * JavaScript; the holes are compacted by an insertion once they outnumber the entries.
 */
template <class K, class V>
class Map {
public:
	using value_type = std::pair<K, V>;

	class iterator {
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = Map::value_type;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type *;
		using reference = value_type &;
		iterator(Map *m, size_t i) : m_(m), i_(i) { skip(); }
		value_type &operator*() const { return *m_->slots_[i_]; }
		value_type *operator->() const { return &*m_->slots_[i_]; }
		iterator &operator++() { i_++; skip(); return *this; }
		bool operator==(const iterator &o) const { return i_ == o.i_; }
		bool operator!=(const iterator &o) const { return i_ != o.i_; }
	private:
		// the end is read each step: an entry set while iterating is visited, as JavaScript visits it
		void skip() { while (i_ < m_->slots_.size() && !m_->slots_[i_]) i_++; if (i_ >= m_->slots_.size()) i_ = END; }
		Map *m_;
		size_t i_;
		friend class Map;
	};
	using const_iterator = iterator;
	static constexpr size_t END = size_t(-1);

	Map() = default;
	/** new Map(pairs): each element a [key, value] (a struct of first and second). */
	template <class Range, class = decltype(std::declval<const Range &>().begin())>
	explicit Map(const Range &pairs) {
		for (const auto &p : pairs) insert_or_assign(p.first, p.second);
	}

	iterator begin() const { return iterator(const_cast<Map *>(this), 0); }
	iterator end() const { return iterator(const_cast<Map *>(this), END); }
	int size() const { return int(index_.size()); }
	bool empty() const { return index_.empty(); }
	size_t count(const K &k) const { return index_.count(k); }
	iterator find(const K &k) const {
		const auto it = index_.find(k);
		return it == index_.end() ? end() : iterator(const_cast<Map *>(this), it->second);
	}
	V &operator[](const K &k) {
		const auto it = index_.find(k);
		if (it != index_.end()) return slots_[it->second]->second;
		return insert_new(k, V{});
	}
	void insert_or_assign(const K &k, V v) {
		const auto it = index_.find(k);
		if (it != index_.end()) slots_[it->second]->second = std::move(v);
		else insert_new(k, std::move(v));
	}
	size_t erase(const K &k) {
		const auto it = index_.find(k);
		if (it == index_.end()) return 0;
		slots_[it->second].reset();
		index_.erase(it);
		return 1;
	}
	void clear() { slots_.clear(); index_.clear(); }
	void reserve(size_t n) { slots_.reserve(n); index_.reserve(n); }

private:
	V &insert_new(const K &k, V v) {
		if (slots_.size() > 2 * index_.size() + COMPACT_SLACK) compact();
		index_.emplace(k, slots_.size());
		slots_.emplace_back(std::in_place, k, std::move(v));
		return slots_.back()->second;
	}
	void compact() {
		size_t w = 0;
		for (size_t r = 0; r < slots_.size(); r++) {
			if (!slots_[r]) continue;
			if (w != r) slots_[w] = std::move(slots_[r]);
			index_[slots_[w]->first] = w;
			w++;
		}
		slots_.resize(w);
	}
	static constexpr size_t COMPACT_SLACK = 32;
	std::vector<std::optional<value_type>> slots_;
	std::unordered_map<K, size_t> index_;
};

/** A Set as JavaScript's: members in insertion order, found by hash (a Map of members to nothing). */
template <class T>
class Set {
	struct None {};
public:
	class iterator {
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = const T *;
		using reference = const T &;
		explicit iterator(typename Map<T, None>::iterator it) : it_(it) {}
		const T &operator*() const { return it_->first; }
		const T *operator->() const { return &it_->first; }
		iterator &operator++() { ++it_; return *this; }
		bool operator!=(const iterator &o) const { return it_ != o.it_; }
		bool operator==(const iterator &o) const { return it_ == o.it_; }
	private:
		typename Map<T, None>::iterator it_;
	};
	Set() = default;
	/** new Set(items). */
	template <class Range, class = decltype(std::declval<const Range &>().begin())>
	explicit Set(const Range &items) {
		for (const auto &x : items) insert(x);
	}
	iterator begin() const { return iterator(m_.begin()); }
	iterator end() const { return iterator(m_.end()); }
	int size() const { return m_.size(); }
	bool empty() const { return m_.empty(); }
	size_t count(const T &x) const { return m_.count(x); }
	/** add: true when it was not there (the member keeps its place otherwise). */
	bool insert(const T &x) {
		if (m_.count(x)) return false;
		m_[x];
		return true;
	}
	size_t erase(const T &x) { return m_.erase(x); }
	/** delete: whether it was there. */
	bool remove(const T &x) { return m_.erase(x) != 0; }
	void clear() { m_.clear(); }
	void reserve(size_t n) { m_.reserve(n); }
private:
	Map<T, None> m_;
};

/** map.keys(), map.values() and set.values() as lists, in insertion order. */
template <class K, class V>
std::vector<K> keys(const Map<K, V> &m) {
	std::vector<K> out;
	for (const auto &e : m) out.push_back(e.first);
	return out;
}
template <class K, class V>
std::vector<V> values(const Map<K, V> &m) {
	std::vector<V> out;
	for (const auto &e : m) out.push_back(e.second);
	return out;
}
template <class T>
std::vector<T> values(const Set<T> &s) {
	std::vector<T> out;
	for (const auto &x : s) out.push_back(x);
	return out;
}
/** Object.keys, Object.values and Object.entries of a dictionary, in insertion order. */
template <class V>
std::vector<std::string> object_keys(const Map<std::string, V> &m) {
	return keys(m);
}
template <class V>
std::vector<V> object_values(const Map<std::string, V> &m) {
	return values(m);
}
template <class V>
std::vector<std::pair<std::string, V>> object_entries(const Map<std::string, V> &m) {
	std::vector<std::pair<std::string, V>> out;
	for (const auto &e : m) out.push_back(e);
	return out;
}
/** map.keys(): a list of them, so the map may change while it is walked. */
template <class K, class V>
std::vector<K> keys_of(const Map<K, V> &m) {
	std::vector<K> out;
	for (const auto &e : m) out.push_back(e.first);
	return out;
}

/** map.values(): each value itself, as JavaScript hands an object out, in insertion order; a list when kept. */
template <class K, class V>
class ValuesOf {
public:
	explicit ValuesOf(const Map<K, V> &m) : m_(m) {}
	class iterator {
	public:
		explicit iterator(typename Map<K, V>::iterator at) : at_(at) {}
		V &operator*() const { return at_->second; }
		iterator &operator++() { ++at_; return *this; }
		bool operator!=(const iterator &o) const { return at_ != o.at_; }
	private:
		typename Map<K, V>::iterator at_;
	};
	iterator begin() const { return iterator(m_.begin()); }
	iterator end() const { return iterator(m_.end()); }
	int size() const { return m_.size(); }
	operator std::vector<V>() const {
		std::vector<V> out;
		for (const auto &e : m_) out.push_back(e.second);
		return out;
	}
private:
	const Map<K, V> &m_;
};
template <class K, class V>
ValuesOf<K, V> values_of(const Map<K, V> &m) { return ValuesOf<K, V>(m); }

/** map.clear(), set.clear(). */
template <class C>
void clear(C &c) {
	c.clear();
}
/** Array.from(items), Float64Array.from(items): a list of what the target holds. */
template <class T, class Range>
std::vector<T> array_from(const Range &items) {
	std::vector<T> out;
	for (const auto &x : items) out.push_back(T(x));
	return out;
}

} // namespace js
