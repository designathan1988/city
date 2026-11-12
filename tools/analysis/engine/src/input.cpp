// The input decoded: each record's fields read by the names the model gives them (the file's vocabulary), each table
// keyed for the lookups the semantics make.
#include "input.h"

#include <stdexcept>
#include <string>

namespace ftengine {

namespace {

constexpr uint64_t kMix = 0x9E3779B97F4A7C15ULL;
constexpr int kRotate = 21;
constexpr size_t kMissedNamed = 20;

std::span<const Value> seq(const Store& s, Value v) {
    if (v.kind() != Kind::Term) return {};
    return s.items(v.id());
}

// An object's fields: its state dict's pairs, found by name.
class Fields {
public:
    Fields(const Store& s, Value obj) : items_(seq(s, obj)) {}
    Value get(uint32_t name) const {
        for (size_t i = 0; i + 1 < items_.size(); i += 2) {
            if (items_[i].kind() == Kind::Str && items_[i].id() == name) return items_[i + 1];
        }
        return Value();
    }

private:
    std::span<const Value> items_;
};

int64_t num(Value v) { return v.kind() == Kind::Int ? v.data() : v.kind() == Kind::True ? 1 : 0; }
bool flag(Value v) { return v.kind() == Kind::True || (v.kind() == Kind::Int && v.data() != 0); }

std::vector<Value> list(const Store& s, Value v) {
    auto items = seq(s, v);
    return {items.begin(), items.end()};
}

std::vector<int64_t> ints(const Store& s, Value v) {
    std::vector<int64_t> out;
    for (Value x : seq(s, v)) out.push_back(num(x));
    return out;
}

Value part(const Store& s, Value root, std::string_view name) {
    auto items = seq(s, root);
    for (size_t i = 0; i + 1 < items.size(); i += 2) {
        if (items[i].kind() == Kind::Str && s.text(items[i].id()) == name) return items[i + 1];
    }
    throw std::runtime_error("the input has no part " + std::string(name));
}

class Decoder {
public:
    Decoder(Input& in) : in_(in), s_(in.store) {}

    uint32_t field(std::string_view owner, std::string_view name) {
        std::string full(owner);
        full += '.';
        full += name;
        return s_.string(name);
    }

    void unit(Value obj) {
        Fields f(s_, obj);
        UnitRec u;
        auto get = [&](const char* n) { return f.get(s_.string(n)); };
        u.kind = get("kind"); u.lang = get("lang"); u.name = get("name"); u.qual = get("qual");
        u.owner = get("owner"); u.file = get("file"); u.accessor_of = get("accessor_of");
        u.constructs = get("constructs"); u.this_of = get("this_of");
        u.line = num(get("line")); u.local_id = num(get("local_id")); u.gid = num(get("gid"));
        u.outer = num(get("outer"));
        u.params = list(s_, get("params"));
        u.variadic = flag(get("variadic")); u.is_static = flag(get("static"));
        u.generator = flag(get("generator")); u.asynchronous = flag(get("asynchronous"));
        u.decl_only = flag(get("decl_only")); u.ambient = flag(get("ambient"));
        auto types = seq(s_, get("types"));
        for (size_t i = 0; i + 1 < types.size(); i += 2) u.types[types[i].bits()] = list(s_, types[i + 1]);
        u.values = list(s_, get("values"));
        for (Value r : seq(s_, get("returns"))) {
            auto parts = seq(s_, r);
            if (parts.size() > 2) u.returns.push_back(parts[2]);
        }
        u.yields = list(s_, get("yields"));
        u.param_types = list(s_, get("param_types"));
        u.tparams = list(s_, get("tparams"));
        auto lambdas = seq(s_, get("lambdas"));
        for (size_t i = 0; i + 1 < lambdas.size(); i += 2) u.lambdas[lambdas[i].bits()] = num(lambdas[i + 1]);
        for (Value c : seq(s_, get("captured"))) u.captured.insert(c.bits());
        auto written = seq(s_, get("written_by"));
        for (size_t i = 0; i + 1 < written.size(); i += 2) u.written_by[written[i].bits()] = ints(s_, written[i + 1]);
        auto accessors = seq(s_, get("accessors"));
        for (size_t i = 0; i + 1 < accessors.size(); i += 2) u.accessors[accessors[i].bits()] = accessors[i + 1];
        u.result_type = get("result_type"); u.predicate_type = get("predicate_type");
        u.this_type = get("this_type");
        for (Value pair : seq(s_, get("indexes"))) {
            auto kv = seq(s_, pair);
            if (kv.size() == 2) u.indexes.emplace_back(kv[0], kv[1]);
        }
        for (Value r : seq(s_, get("refs"))) {
            Fields rf(s_, r);
            auto rget = [&](const char* n) { return rf.get(s_.string(n)); };
            RefRec ref;
            ref.kind = rget("kind"); ref.name = rget("name"); ref.base = rget("base"); ref.args = rget("args");
            ref.text = rget("text"); ref.key = rget("key");
            ref.line = num(rget("line")); ref.target = num(rget("target")); ref.site = num(rget("site"));
            u.refs.push_back(ref);
        }
        in_.units.push_back(std::move(u));
    }

    void cls(Value obj) {
        Fields f(s_, obj);
        auto get = [&](const char* n) { return f.get(s_.string(n)); };
        ClassRec c;
        c.key = get("key"); c.name = get("name"); c.file = get("file"); c.outer = get("outer");
        c.shape = flag(get("shape")); c.made = num(get("made")); c.ambient = flag(get("ambient"));
        c.tparams = list(s_, get("tparams")); c.heritage = list(s_, get("heritage"));
        c.identity = c.key;
        in_.classes.emplace(c.key.bits(), std::move(c));
    }

private:
    Input& in_;
    Store& s_;
};

}  // namespace

size_t Key3Hash::operator()(const Key3& k) const {
    uint64_t h = k.a * kMix;
    h = (h ^ (h >> kRotate)) + k.b * kMix;
    h = (h ^ (h >> kRotate)) + k.c * kMix;
    return static_cast<size_t>(h ^ (h >> kRotate));
}

int UnitRec::param_index(Value name) const {
    for (size_t i = 0; i < params.size(); ++i) {
        if (params[i] == name) return static_cast<int>(i);
    }
    return -1;
}

void decode(Value root, Input& in) {
    Store& s = in.store;
    Decoder d(in);
    for (Value row : seq(s, part(s, root, "vocabulary"))) {
        auto kv = seq(s, row);
        in.vocabulary.emplace(s.text(kv[0].id()), kv[1]);
    }
    for (Value u : seq(s, part(s, root, "units"))) d.unit(u);
    for (Value c : seq(s, part(s, root, "classes"))) d.cls(c);
    for (const UnitRec& u : in.units) in.offset.emplace(u.file.bits(), u.gid - u.local_id);
    for (Value row : seq(s, part(s, root, "members"))) {
        auto r = seq(s, row);
        in.members[r[0].bits()].emplace_back(r[1], ints(s, r[2]));
    }
    for (Value row : seq(s, part(s, root, "names"))) {
        auto r = seq(s, row);
        in.names[{static_cast<uint64_t>(num(r[0])), r[1].bits(), 0}] = {ints(s, r[2]), list(s, r[3]), r[4]};
    }
    for (Value row : seq(s, part(s, root, "types"))) {
        auto r = seq(s, row);
        in.types[{r[0].bits(), r[1].bits(), r[2].bits()}] = r[3];
    }
    for (Value row : seq(s, part(s, root, "class_tables"))) {
        auto r = seq(s, row);
        auto found = in.classes.find(r[0].bits());
        if (found == in.classes.end()) continue;
        ClassRec& c = found->second;
        c.bases = list(s, r[1]); c.group = list(s, r[2]); c.identity = r[3]; c.chain = list(s, r[4]);
        for (Value pair : seq(s, r[5])) {
            auto kv = seq(s, pair);
            c.inner[kv[0].bits()] = kv[1];
        }
        c.proto_chain = list(s, r[6]); c.proto_unresolved = flag(r[7]); c.proto_ambient = flag(r[8]);
    }
    for (Value row : seq(s, part(s, root, "intrinsics"))) {
        auto r = seq(s, row);
        in.intrinsics[r[0].bits()] = list(s, r[1]);
    }
    for (Value row : seq(s, part(s, root, "modules"))) {
        auto r = seq(s, row);
        in.modules.insert(r[0].bits());
        for (Value imp : seq(s, r[2])) {
            auto kv = seq(s, imp);
            in.imports[{r[0].bits(), kv[0].bits(), 0}] = kv[1];
        }
    }
    for (Value row : seq(s, part(s, root, "exports"))) {
        auto r = seq(s, row);
        in.exports[{r[0].bits(), r[1].bits(), 0}] = {ints(s, r[2]), list(s, r[3])};
    }
    for (Value row : seq(s, part(s, root, "jsx"))) {
        auto r = seq(s, row);
        JsxRow j;
        for (Value pair : seq(s, r[1])) {
            auto kv = seq(s, pair);
            j.options[kv[0].bits()] = kv[1];
        }
        j.runtime = r[2];
        j.children = r[3];
        in.jsx[r[0].bits()] = j;
    }
    for (Value row : seq(s, part(s, root, "queries"))) {
        auto r = seq(s, row);
        in.queries[{r[0].bits(), r[1].bits(), r[2].bits()}] = ints(s, r[3]);
    }
    for (Value row : seq(s, part(s, root, "operators"))) {
        auto r = seq(s, row);
        in.operators[{r[0].bits(), static_cast<uint64_t>(flag(r[1])), 0}] = list(s, r[2]);
    }
    for (Value row : seq(s, part(s, root, "literals"))) {
        auto r = seq(s, row);
        in.literals[r[0].bits()] = list(s, r[1]);
    }
    for (Value name : seq(s, part(s, root, "numeric_names"))) in.numeric_names.insert(name.bits());
    for (Value gid : seq(s, part(s, root, "contextual"))) in.contextual.insert(num(gid));
    Value module_unit = in.vocab("js_read.MODULE_UNIT");
    for (const UnitRec& u : in.units) {
        if (u.name == module_unit) in.module_units[u.file.bits()].push_back(u.gid);
    }
}

void Input::miss(std::string_view what, std::initializer_list<Value> key) {
    ++missing;
    if (missed.size() >= kMissedNamed) return;
    std::string line(what);
    for (Value v : key) {
        line += ' ';
        line += v.kind() == Kind::Str ? std::string(store.text(v.id())) : std::to_string(v.data());
    }
    missed.push_back(line);
}

Value Input::vocab(std::string_view name) const {
    auto found = vocabulary.find(name);
    if (found == vocabulary.end()) throw std::runtime_error("the vocabulary has no " + std::string(name));
    return found->second;
}

const ClassRec* Input::cls(Value key) const {
    auto found = classes.find(key.bits());
    return found == classes.end() ? nullptr : &found->second;
}

const std::vector<std::pair<Value, std::vector<int64_t>>>& Input::members_of(Value owner) const {
    static const std::vector<std::pair<Value, std::vector<int64_t>>> kNone;
    auto found = members.find(owner.bits());
    return found == members.end() ? kNone : found->second;
}

const std::vector<int64_t>& Input::member(Value owner, Value name) const {
    static const std::vector<int64_t> kNone;
    for (const auto& [n, gids] : members_of(owner)) {
        if (n == name) return gids;
    }
    return kNone;
}

const NameRow& Input::name(int64_t gid, Value name) {
    static const NameRow kNone;
    auto found = names.find({static_cast<uint64_t>(gid), name.bits(), 0});
    if (found == names.end()) {
        miss("name", {Value::make(Kind::Int, gid), name});
        return kNone;
    }
    return found->second;
}

Value Input::resolve(Value text, Value file, Value owner) {
    auto found = types.find({text.bits(), file.bits(), owner.bits()});
    if (found == types.end()) {
        miss("type", {text, file, owner});
        return Value();
    }
    return found->second;
}

const Export& Input::export_of(Value module, Value name) const {
    static const Export kNone;
    auto found = exports.find({module.bits(), name.bits(), 0});
    return found == exports.end() ? kNone : found->second;
}

const std::vector<int64_t>& Input::query(Value file, Value owner, Value name) {
    static const std::vector<int64_t> kNone;
    auto found = queries.find({file.bits(), owner.bits(), name.bits()});
    if (found == queries.end()) {
        miss("query", {file, owner, name});
        return kNone;
    }
    return found->second;
}

}  // namespace ftengine
