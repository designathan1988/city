// The pickle reader: a stack machine over the opcodes of protocol 5 the facts use (Lib/pickletools.py names and
// describes each; pickle.py gives their bytes). A list, dict, set or object is a container under construction until
// the pickle hands it to its parent or stops; then it becomes one hash-consed term.
#include "pickle_reader.h"

#include <cstring>
#include <vector>

namespace ftengine {

namespace {

// The opcodes (pickle.py's constants, the format's own bytes).
enum Op : uint8_t {
    kMark = '(', kStop = '.', kNone = 'N', kBinInt = 'J', kBinInt1 = 'K', kBinInt2 = 'M', kBinUnicode = 'X',
    kAppend = 'a', kBuild = 'b', kEmptyDict = '}', kAppends = 'e', kBinGet = 'h', kLongBinGet = 'j',
    kEmptyList = ']', kSetItem = 's', kTuple = 't', kEmptyTuple = ')', kSetItems = 'u',
    kProto = 0x80, kNewObj = 0x81, kTuple1 = 0x85, kTuple2 = 0x86, kTuple3 = 0x87, kNewTrue = 0x88,
    kNewFalse = 0x89, kLong1 = 0x8A, kShortBinUnicode = 0x8C, kBinUnicode8 = 0x8D, kEmptySet = 0x8F,
    kAddItems = 0x90, kFrozenSet = 0x91, kStackGlobal = 0x93, kMemoize = 0x94, kFrame = 0x95,
};
constexpr uint8_t kHighestProtocol = 5;          // PEP 574
constexpr int kByteBits = 8;
constexpr size_t kLongBytes = 8;                 // the widest LONG1 an engine value holds

struct Slot {
    Value value{};
    int32_t temp = -1;                           // a container under construction: its index
};

struct Temp {
    Shape shape;
    uint32_t head = 0;
    std::vector<Value> items{};
    bool done = false;
    Value value{};
};

class Reader {
public:
    Reader(std::span<const uint8_t> data, Store& store) : data_(data), store_(store) {}

    Value run() {
        for (;;) {
            uint8_t op = byte();
            switch (op) {
            case kProto: if (byte() > kHighestProtocol) fail("protocol above 5"); break;
            case kFrame: take(sizeof(uint64_t)); break;
            case kStop: return value(pop());
            case kMark: marks_.push_back(stack_.size()); break;
            case kMemoize: memo_.push_back(top()); break;
            case kBinGet: push(memo(byte())); break;
            case kLongBinGet: push(memo(unsigned_le(sizeof(uint32_t)))); break;
            case kNone: atom(Kind::None, 0); break;
            case kNewTrue: atom(Kind::True, 0); break;
            case kNewFalse: atom(Kind::False, 0); break;
            case kBinInt: atom(Kind::Int, static_cast<int32_t>(unsigned_le(sizeof(int32_t)))); break;
            case kBinInt1: atom(Kind::Int, byte()); break;
            case kBinInt2: atom(Kind::Int, static_cast<int64_t>(unsigned_le(sizeof(uint16_t)))); break;
            case kLong1: long1(); break;
            case kShortBinUnicode: text(byte()); break;
            case kBinUnicode: text(unsigned_le(sizeof(uint32_t))); break;
            case kBinUnicode8: text(unsigned_le(sizeof(uint64_t))); break;
            case kEmptyTuple: made(Shape::Tuple, 0, {}); break;
            case kTuple1: case kTuple2: case kTuple3: tuple(stack_.size() - (op - kTuple1 + 1)); break;
            case kTuple: tuple(mark()); break;
            case kFrozenSet: { size_t at = mark(); made_from(Shape::FrozenSet, at); break; }
            case kEmptyList: open(Shape::List, 0); break;
            case kEmptyDict: open(Shape::Dict, 0); break;
            case kEmptySet: open(Shape::Set, 0); break;
            case kAppend: { Value v = value(pop()); building(top(), Shape::List).items.push_back(v); break; }
            case kSetItem: { Value v = value(pop()); Value k = value(pop()); add_pair(top(), k, v); break; }
            case kAppends: case kAddItems: case kSetItems: fill(op); break;
            case kStackGlobal: stack_global(); break;
            case kNewObj: new_object(); break;
            case kBuild: build(); break;
            default: fail("an opcode outside the engine's subset: " + std::to_string(op));
            }
        }
    }

private:
    [[noreturn]] void fail(const std::string& why) const {
        throw PickleError(why + " at byte " + std::to_string(at_));
    }
    const uint8_t* take(size_t n) {
        if (n > data_.size() - at_) fail("the stream ends inside an opcode");
        const uint8_t* p = data_.data() + at_;
        at_ += n;
        return p;
    }
    uint8_t byte() { return *take(1); }
    uint64_t unsigned_le(size_t n) {
        const uint8_t* p = take(n);
        uint64_t v = 0;
        for (size_t i = 0; i < n; ++i) v |= static_cast<uint64_t>(p[i]) << (kByteBits * i);
        return v;
    }
    void push(Slot s) { stack_.push_back(s); }
    Slot pop() {
        if (stack_.empty() || (!marks_.empty() && stack_.size() <= marks_.back())) fail("pop below a mark");
        Slot s = stack_.back();
        stack_.pop_back();
        return s;
    }
    Slot& top() {
        if (stack_.empty()) fail("an empty stack");
        return stack_.back();
    }
    Slot memo(uint64_t index) {
        if (index >= memo_.size()) fail("a memo index never stored");
        return memo_[index];
    }
    size_t mark() {
        if (marks_.empty()) fail("no mark");
        size_t at = marks_.back();
        marks_.pop_back();
        return at;
    }
    void atom(Kind kind, int64_t data) { push({Value::make(kind, data)}); }
    void text(uint64_t n) {
        const uint8_t* p = take(n);
        atom(Kind::Str, store_.string({reinterpret_cast<const char*>(p), n}));
    }
    void long1() {
        size_t n = byte();
        if (n > kLongBytes) fail("an integer wider than 64 bits");
        uint64_t v = n ? unsigned_le(n) : 0;
        if (n && n < kLongBytes && (v >> (kByteBits * n - 1)) & 1) v |= ~uint64_t{0} << (kByteBits * n);
        auto signed_v = static_cast<int64_t>(v);
        if (signed_v > kIntMax || signed_v < kIntMin) fail("an integer wider than a value holds");
        atom(Kind::Int, signed_v);
    }
    Value value(const Slot& s) {
        if (s.temp < 0) return s.value;
        Temp& t = temps_[s.temp];
        if (!t.done) {
            t.value = Value::make(Kind::Term, store_.term(t.shape, t.head, t.items));
            t.done = true;
            std::vector<Value>().swap(t.items);
        }
        return t.value;
    }
    void made(Shape shape, uint32_t head, std::span<const Value> items) {
        atom(Kind::Term, store_.term(shape, head, items));
    }
    void made_from(Shape shape, size_t at) {
        if (at > stack_.size()) fail("a mark above the stack");
        items_.clear();
        for (size_t i = at; i < stack_.size(); ++i) items_.push_back(value(stack_[i]));
        stack_.resize(at);
        made(shape, 0, items_);
    }
    void tuple(size_t at) { made_from(Shape::Tuple, at); }
    void open(Shape shape, uint32_t head) {
        temps_.push_back({shape, head});
        push({Value(), static_cast<int32_t>(temps_.size() - 1)});
    }
    Temp& building(const Slot& s, Shape shape) {
        if (s.temp < 0 || temps_[s.temp].shape != shape) fail("an item added to what is not that container");
        Temp& t = temps_[s.temp];
        if (t.done) fail("a container changed after its parent took it (a cycle)");
        return t;
    }
    void add_pair(const Slot& dict, Value k, Value v) {
        Temp& t = building(dict, Shape::Dict);
        t.items.push_back(k);
        t.items.push_back(v);
    }
    void fill(uint8_t op) {
        size_t at = mark();
        if (at == 0 || at > stack_.size()) fail("a container's mark at the stack's bottom");
        Shape shape = op == kAppends ? Shape::List : op == kAddItems ? Shape::Set : Shape::Dict;
        Slot target = stack_[at - 1];
        items_.clear();
        for (size_t i = at; i < stack_.size(); ++i) items_.push_back(value(stack_[i]));
        stack_.resize(at);
        Temp& t = building(target, shape);
        if (shape == Shape::Dict && items_.size() % 2) fail("a dict's items without their values");
        t.items.insert(t.items.end(), items_.begin(), items_.end());
    }
    void stack_global() {
        Value name = value(pop()), module = value(pop());
        if (name.kind() != Kind::Str || module.kind() != Kind::Str) fail("a global not named by two strings");
        std::string full(store_.text(module.id()));
        full += '.';
        full += store_.text(name.id());
        atom(Kind::Class, store_.string(full));
    }
    void new_object() {
        Value args = value(pop()), cls = value(pop());
        if (cls.kind() != Kind::Class) fail("an object of no class");
        if (args.kind() != Kind::Term || !store_.items(args.id()).empty()) fail("an object made with arguments");
        open(Shape::Object, cls.id());
    }
    void build() {
        Value state = value(pop());
        if (state.kind() != Kind::Term || store_.shape(state.id()) != Shape::Dict) fail("an object's state not a dict");
        Temp& t = building(top(), Shape::Object);
        auto items = store_.items(state.id());
        t.items.insert(t.items.end(), items.begin(), items.end());
    }

    std::span<const uint8_t> data_;
    Store& store_;
    size_t at_ = 0;
    std::vector<Slot> stack_, memo_;
    std::vector<size_t> marks_;
    std::vector<Temp> temps_;
    std::vector<Value> items_;
};

}  // namespace

Value read_pickle(std::span<const uint8_t> data, Store& store) {
    return Reader(data, store).run();
}

}  // namespace ftengine
