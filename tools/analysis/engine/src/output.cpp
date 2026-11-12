// The result as a pickle: opcodes of protocol 4 (pickle.py's bytes; Lib/pickletools.py describes them); a tuple term
// written once and fetched from the memo where it repeats.
#include "output.h"

#include <algorithm>
#include <map>

#include "semantics.h"

namespace ftengine {

namespace {

enum Op : uint8_t {
    kMark = '(', kStop = '.', kNone = 'N', kBinInt = 'J', kBinUnicode = 'X', kAppends = 'e', kEmptyDict = '}',
    kEmptyList = ']', kTuple = 't', kSetItems = 'u', kLongBinGet = 'j', kProto = 0x80, kNewTrue = 0x88,
    kNewFalse = 0x89, kLong1 = 0x8A, kEmptySet = 0x8F, kAddItems = 0x90, kFrozenSet = 0x91, kMemoize = 0x94,
};
constexpr uint8_t kProtocol = 4;
constexpr int kByteBits = 8;
constexpr int kLongBytes = 8;

class Writer {
public:
    explicit Writer(const Store& s) : s_(s) {
        out_ += static_cast<char>(kProto);
        out_ += static_cast<char>(kProtocol);
    }
    void op(uint8_t o) { out_ += static_cast<char>(o); }
    void le(uint64_t v, int n) {
        for (int i = 0; i < n; ++i) out_ += static_cast<char>((v >> (kByteBits * i)) & 0xFF);
    }
    void integer(int64_t v) {
        if (v >= INT32_MIN && v <= INT32_MAX) {
            op(kBinInt);
            le(static_cast<uint64_t>(static_cast<int32_t>(v)), sizeof(int32_t));
            return;
        }
        op(kLong1);
        out_ += static_cast<char>(kLongBytes);
        le(static_cast<uint64_t>(v), kLongBytes);
    }
    void text(std::string_view t) {
        op(kBinUnicode);
        le(t.size(), sizeof(uint32_t));
        out_.append(t);
    }
    void value(Value v) {
        switch (v.kind()) {
        case Kind::None: op(kNone); return;
        case Kind::True: op(kNewTrue); return;
        case Kind::False: op(kNewFalse); return;
        case Kind::Int: integer(v.data()); return;
        case Kind::Str: case Kind::Class: text(s_.text(v.id())); return;
        case Kind::Term: term(v.id()); return;
        }
    }
    void term(uint32_t id) {
        auto items = s_.items(id);
        switch (s_.shape(id)) {
        case Shape::Tuple: {
            auto found = memo_.find(id);
            if (found != memo_.end()) {
                op(kLongBinGet);
                le(found->second, sizeof(uint32_t));
                return;
            }
            op(kMark);
            for (Value v : items) value(v);
            op(kTuple);
            op(kMemoize);
            memo_.emplace(id, next_++);
            return;
        }
        case Shape::List: case Shape::Object:
            op(kEmptyList); ++next_; op(kMemoize);
            op(kMark);
            for (Value v : items) value(v);
            op(kAppends);
            return;
        case Shape::Dict:
            op(kEmptyDict); ++next_; op(kMemoize);
            op(kMark);
            for (Value v : items) value(v);
            op(kSetItems);
            return;
        case Shape::Set:
            op(kEmptySet); ++next_; op(kMemoize);
            op(kMark);
            for (Value v : items) value(v);
            op(kAddItems);
            return;
        case Shape::FrozenSet:
            op(kMark);
            for (Value v : items) value(v);
            op(kFrozenSet);
            op(kMemoize);
            ++next_;
            return;
        }
    }
    void open_list() { op(kEmptyList); op(kMemoize); ++next_; op(kMark); }
    void close_list() { op(kAppends); }
    void open_dict() { op(kEmptyDict); op(kMemoize); ++next_; op(kMark); }
    void close_dict() { op(kSetItems); }
    std::string done() {
        op(kStop);
        return std::move(out_);
    }

private:
    const Store& s_;
    std::string out_;
    std::unordered_map<uint32_t, uint32_t> memo_;
    uint32_t next_ = 0;
};

void ints(Writer& w, std::vector<Obj> objs) {
    std::sort(objs.begin(), objs.end());
    objs.erase(std::unique(objs.begin(), objs.end()), objs.end());
    w.open_list();
    for (Obj o : objs) w.integer(o);
    w.close_list();
}

struct Volume {                                    // what a function's nodes hold (heap/scaler.py reads it)
    size_t facts = 0, receivers = 0;
    std::vector<size_t> params;
};

std::map<int64_t, Volume> volumes(Analysis& an) {
    const Vocab& V = an.V;
    std::map<int64_t, Volume> out;
    for (Node n = 0; n < an.solver.nodes(); ++n) {
        Value key = an.key_of(n);
        Value tag = an.item(key, 0);
        if (tag != V.TREE && tag != V.LOCAL && tag != V.PARAM && tag != V.THIS && tag != V.RET) continue;
        if (an.item(key, 1).kind() != Kind::Int) continue;
        size_t held = an.solver.held(n).size();
        Volume& v = out[an.item(key, 1).data()];
        v.facts += held;
        if (tag == V.THIS) v.receivers += held;
        if (tag == V.PARAM) {
            auto index = static_cast<size_t>(an.item(key, 2).data());
            if (v.params.size() <= index) v.params.resize(index + 1);
            v.params[index] += held;
        }
    }
    return out;
}

}  // namespace

std::string result_pickle(Analysis& an, Semantics& semantics, bool names) {
    std::vector<std::pair<int64_t, std::vector<Value>>> rets;
    for (const auto& [gid, ctxs] : an.active_order) {
        std::vector<Value> nodes;
        for (Value ctx : ctxs) nodes.push_back(an.ret(static_cast<int64_t>(gid), ctx));
        rets.emplace_back(static_cast<int64_t>(gid), nodes);
        if (!names) continue;
        const UnitRec& u = an.unit(static_cast<int64_t>(gid));
        for (Value ctx : ctxs) {
            for (Value p : u.params) an.local(u, p, ctx);
            for (const auto& [name, trees] : u.types) an.local(u, Value::from_bits(name), ctx);
        }
    }
    an.solver.run(semantics);
    Writer w(an.s);
    w.open_dict();
    w.text("links");
    w.open_list();
    for (const Link& l : an.links) {
        w.op(kMark);
        w.integer(l.src); w.integer(l.dst); w.value(l.kind); w.integer(l.line);
        w.op(kTuple);
    }
    w.close_list();
    w.text("objects");
    w.open_list();
    for (Obj o = 0; o < an.objects(); ++o) w.value(an.okey(o));
    w.close_list();
    w.text("returned");
    w.open_dict();
    for (const auto& [gid, nodes] : rets) {
        std::vector<Obj> objs;
        for (Value n : nodes) {
            auto held = an.held(n);
            objs.insert(objs.end(), held.begin(), held.end());
        }
        w.integer(gid);
        ints(w, objs);
    }
    w.close_dict();
    w.text("names");
    w.open_dict();
    if (names) {
    std::unordered_map<uint64_t, std::vector<Obj>> by_name;
    std::vector<std::pair<int64_t, Value>> order;
    for (Node n = 0; n < an.solver.nodes(); ++n) {
        Value key = an.key_of(n);
        Value pair;
        if (an.item(key, 0) == an.V.LOCAL) {
            pair = an.tuple({an.item(key, 1), an.item(key, 2)});
        } else if (an.item(key, 0) == an.V.TREE && an.item(an.item(key, 2), 0) == an.V.NAME) {
            pair = an.tuple({an.item(key, 1), an.item(an.item(key, 2), 1)});
        } else {
            continue;
        }
        auto [found, made] = by_name.try_emplace(pair.bits());
        if (made) order.emplace_back(an.item(pair, 0).data(), an.item(pair, 1));
        auto held = an.solver.held(n);
        found->second.insert(found->second.end(), held.begin(), held.end());
    }
    for (const auto& [gid, name] : order) {
        Value pair = an.tuple({I(gid), name});
        w.value(pair);
        ints(w, by_name[pair.bits()]);
    }
    }
    w.close_dict();
    w.text("runs");
    w.open_dict();
    for (const auto& [gid, ctxs] : an.active_order) {
        w.integer(static_cast<int64_t>(gid));
        w.integer(static_cast<int64_t>(ctxs.size()));
    }
    w.close_dict();
    w.text("volumes");                              // per function: its nodes' facts, `this`'s and each parameter's
    w.open_dict();
    for (const auto& [gid, v] : volumes(an)) {
        w.integer(gid);
        w.op(kMark);
        w.integer(static_cast<int64_t>(v.facts));
        w.integer(static_cast<int64_t>(v.receivers));
        w.open_list();
        for (size_t n : v.params) w.integer(static_cast<int64_t>(n));
        w.close_list();
        w.op(kTuple);
    }
    w.close_dict();
    w.text("missing");
    w.integer(static_cast<int64_t>(an.in.missing));
    w.close_dict();
    return w.done();
}

}  // namespace ftengine
