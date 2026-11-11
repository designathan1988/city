// The engine's values and terms (log/trace-engine-plan.md, Task E2): an atom (none, a boolean, an integer, an interned
// string, a class) or a hash-consed term (a tuple, list, dict, set, frozenset or object), each value one 64-bit word and
// equal terms one id, so a tree the facts repeat is held and compared once.
#pragma once

#include <cstdint>
#include <deque>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ftengine {

enum class Kind : uint8_t { None, True, False, Int, Str, Term, Class };

// A value in one word: the kind in the low bits, the integer, string id or term id above them.
class Value {
public:
    static constexpr int kKindBits = 3;
    static constexpr uint64_t kKindMask = (uint64_t{1} << kKindBits) - 1;

    constexpr Value() = default;
    static constexpr Value make(Kind kind, int64_t data) {
        return Value((static_cast<uint64_t>(data) << kKindBits) | static_cast<uint64_t>(kind));
    }
    constexpr Kind kind() const { return static_cast<Kind>(bits_ & kKindMask); }
    constexpr int64_t data() const { return static_cast<int64_t>(bits_) >> kKindBits; }
    constexpr uint32_t id() const { return static_cast<uint32_t>(data()); }
    constexpr uint64_t bits() const { return bits_; }
    constexpr bool operator==(const Value&) const = default;
    constexpr bool operator<(const Value& other) const { return bits_ < other.bits_; }

private:
    constexpr explicit Value(uint64_t bits) : bits_(bits) {}
    uint64_t bits_ = 0;
};

// The largest integer a value holds (what the kind bits leave).
constexpr int64_t kIntMax = INT64_MAX >> Value::kKindBits;
constexpr int64_t kIntMin = INT64_MIN >> Value::kKindBits;

enum class Shape : uint8_t { Tuple, List, Dict, Set, FrozenSet, Object };

// Strings interned and terms hash-consed: `term` returns the id of the one term with that shape, head and items.
class Store {
public:
    Store();
    uint32_t string(std::string_view text);
    std::string_view text(uint32_t id) const { return texts_[id]; }
    size_t strings() const { return texts_.size(); }

    // A term: its shape, its head (an object's class string id, 0 otherwise) and its items. A set's items are put in
    // the values' order first, so two sets of the same values are one term.
    uint32_t term(Shape shape, uint32_t head, std::span<const Value> items);
    Shape shape(uint32_t id) const { return recs_[id].shape; }
    uint32_t head(uint32_t id) const { return recs_[id].head; }
    std::span<const Value> items(uint32_t id) const {
        return {pool_.data() + recs_[id].first, recs_[id].count};
    }
    size_t terms() const { return recs_.size(); }

private:
    struct Rec {
        Shape shape;
        uint32_t head;
        uint64_t first;
        uint32_t count;
        uint64_t hash;
    };
    static uint64_t hash_of(Shape shape, uint32_t head, std::span<const Value> items);
    bool equal(uint32_t id, Shape shape, uint32_t head, std::span<const Value> items) const;
    void grow();

    std::deque<std::string> texts_;                       // a deque: a string never moves, its view stays valid
    std::unordered_map<std::string_view, uint32_t> string_ids_;
    std::vector<Rec> recs_;
    std::vector<Value> pool_;
    std::vector<uint32_t> slots_;                          // open addressing: term id + 1, 0 empty
    std::vector<Value> scratch_;
};

}  // namespace ftengine
