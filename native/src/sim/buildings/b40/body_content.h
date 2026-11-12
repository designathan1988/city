// The persisted body fields, in format order. Derived cut/view state is deliberately outside authored identity.
#pragma once
#include "sim/buildings/b40/types.h"
#include <algorithm>
#include <cstring>
#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace city::b40::body_content {
inline auto fields(const SidePlan &v) { return std::tie(v.use, v.width, v.walk); }
inline auto fields(const LotPlan &v) { return std::tie(v.kind, v.W, v.D, v.front_depth, v.back_depth, v.x0, v.y0, v.x1, v.y1, v.front, v.left, v.right, v.back, v.rows); }
inline auto fields(const FaceGeometry &v) { return std::tie(v.bays, v.window_w, v.window_h, v.sill, v.pier_w, v.pier_d, v.pier_every); }
inline auto fields(const Point2 &v) { return std::tie(v.x, v.y); }
inline auto fields(const Relief &v) { return std::tie(v.side, v.bay0, v.bay1, v.storey0, v.storey1, v.depth); }
inline auto fields(const FlagDesign &v) { return std::tie(v.pattern, v.rgb); }
inline auto fields(const RoofPart &v) { return std::tie(v.kind, v.x, v.y, v.w, v.d, v.h, v.turn, v.flag); }
inline auto fields(const Paint &v) { return std::tie(v.finish, v.rgb); }
inline auto fields(const Volume &v) {
	return std::tie(v.x, v.y, v.w, v.d, v.pitch, v.base, v.storeys, v.ridge, v.fall, v.wall, v.roof_paint,
		v.roof, v.lines, v.crown, v.keep, v.outline, v.fill, v.bays, v.side_fill, v.storey_paint, v.faces, v.reliefs, v.gear, v.mode, v.lift);
}
inline auto fields(const LotElement &v) { return std::tie(v.kind, v.x, v.y, v.w, v.d, v.z, v.h, v.base, v.facing, v.paint, v.angle); }
inline auto fields(const Core &v) { return std::tie(v.x, v.y, v.to, v.kind); }
inline auto fields(const PlacedPiece &v) { return std::tie(v.kind, v.x, v.y, v.angle); }
inline auto fields(const Body &v) {
	return std::tie(v.plan, v.kind, v.use, v.ground, v.style, v.palette, v.wall, v.roof, v.trim, v.entrance_volume,
		v.ground_h, v.storey_h, v.entrance_x, v.signature, v.signature_x, v.signature_y, v.paints, v.volumes, v.elements, v.cores, v.furnishing);
}

template<class T> struct Sequence : std::false_type {};
template<class T, class A> struct Sequence<std::vector<T, A>> : std::true_type {};
template<class T> struct OrderedMap : std::false_type {};
template<class K, class V, class C, class A> struct OrderedMap<std::map<K, V, C, A>> : std::true_type {};
template<class T> struct UnorderedMap : std::false_type {};
template<class K, class V, class H, class E, class A> struct UnorderedMap<std::unordered_map<K, V, H, E, A>> : std::true_type {};

template<class T> bool same(const T &a, const T &b);
template<class A, class B, size_t... I> bool same_fields(const A &a, const B &b, std::index_sequence<I...>) {
	return (same(std::get<I>(a), std::get<I>(b)) && ...);
}
// Allocation-free, exact persistent identity: no hash collision, map sort or byte-vector construction.
template<class T> bool same(const T &a, const T &b) {
	if (&a == &b) return true;
	if constexpr (std::is_floating_point_v<T>) return std::memcmp(&a, &b, sizeof(T)) == 0;
	else if constexpr (std::is_integral_v<T> || std::is_enum_v<T>) return a == b;
	else if constexpr (std::is_array_v<T>) {
		for (size_t i = 0; i < std::extent_v<T>; ++i) if (!same(a[i], b[i])) return false;
		return true;
	} else if constexpr (Sequence<T>::value) {
		if (a.size() != b.size()) return false;
		for (size_t i = 0; i < a.size(); ++i) if (!same(a[i], b[i])) return false;
		return true;
	} else if constexpr (UnorderedMap<T>::value) {
		if (a.size() != b.size()) return false;
		for (const auto &v : a) { const auto found = b.find(v.first); if (found == b.end() || !same(v.second, found->second)) return false; }
		return true;
	} else if constexpr (OrderedMap<T>::value) {
		if (a.size() != b.size()) return false;
		auto i = a.begin(), j = b.begin();
		for (; i != a.end(); ++i, ++j) if (!same(i->first, j->first) || !same(i->second, j->second)) return false;
		return true;
	} else {
		const auto left = fields(a), right = fields(b);
		return same_fields(left, right, std::make_index_sequence<std::tuple_size_v<decltype(left)>>{});
	}
}

// ByteWriter-compatible archive; constants/version framing stay in body_codec. Integers retain their existing widths.
template<class Writer, class T> void write(Writer &w, const T &v) {
	static_assert(sizeof(int) == sizeof(int32_t), "body wire format requires 32-bit int");
	if constexpr (std::is_same_v<T, bool>) w.template put<uint8_t>(v ? 1 : 0);
	else if constexpr (std::is_enum_v<T>) w.template put<std::underlying_type_t<T>>(std::underlying_type_t<T>(v));
	else if constexpr (std::is_arithmetic_v<T>) w.template put<T>(v);
	else if constexpr (std::is_array_v<T>) { for (const auto &item : v) write(w, item); }
	else if constexpr (Sequence<T>::value) {
		w.template put<uint32_t>(uint32_t(v.size()));
		if constexpr (std::is_same_v<typename T::value_type, uint8_t>) w.put_bytes(v.data(), v.size());
		else for (const auto &item : v) write(w, item);
	} else if constexpr (UnorderedMap<T>::value) {
		std::vector<typename T::key_type> keys;
		keys.reserve(v.size()); for (const auto &item : v) keys.push_back(item.first);
		std::sort(keys.begin(), keys.end());
		w.template put<uint32_t>(uint32_t(keys.size()));
		for (const auto &key : keys) { write(w, key); write(w, v.find(key)->second); }
	} else if constexpr (OrderedMap<T>::value) {
		w.template put<uint32_t>(uint32_t(v.size()));
		for (const auto &item : v) { write(w, item.first); write(w, item.second); }
	} else std::apply([&w](const auto &...items) { (write(w, items), ...); }, fields(v));
}
} // namespace city::b40::body_content
