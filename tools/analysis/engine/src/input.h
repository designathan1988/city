// The engine's input decoded (featuretrace/engine_input.py writes it): the units, references and classes as records of
// the values the readers made, and every resolution table keyed for lookup; the vocabulary by its owners' names.
#pragma once

#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "terms.h"

namespace ftengine {

struct RefRec {
    Value kind, name, base, args, text, key;     // args: a tuple or list term
    int64_t line = 0, target = -1, site = -1;
};

struct UnitRec {
    Value kind, lang, name, qual, owner, file, accessor_of, constructs, this_of;
    int64_t line = 0, local_id = -1, gid = -1, outer = -1;
    std::vector<Value> params;                   // names
    bool variadic = false, is_static = false, generator = false, asynchronous = false, decl_only = false;
    bool ambient = false;
    std::unordered_map<uint64_t, std::vector<Value>> types;      // name bits -> trees
    std::vector<Value> values, returns, yields, param_types, tparams;
    std::unordered_map<uint64_t, int64_t> lambdas;
    std::vector<RefRec> refs;
    std::unordered_set<uint64_t> captured;
    std::unordered_map<uint64_t, std::vector<int64_t>> written_by;
    std::unordered_map<uint64_t, Value> accessors;
    Value result_type, this_type, predicate_type;   // predicate_type: (x's index, T, asserts) or ()
    std::vector<std::pair<Value, Value>> indexes;
    int param_index(Value name) const;           // -1 when not a parameter
};

struct ClassRec {
    Value key, name, file, outer;
    bool shape = false, ambient = false;
    int64_t made = -1;
    std::vector<Value> tparams, heritage;
    // class tables
    std::vector<Value> bases, group, chain;
    std::vector<Value> proto_chain;              // what its `extends` reaches, itself first (heap/guards.py)
    bool proto_unresolved = false, proto_ambient = false;
    Value identity;
    std::unordered_map<uint64_t, Value> inner;
};

struct NameRow {
    std::vector<int64_t> definitions;
    std::vector<Value> bound;                    // the modules an import binds
    Value class_of;
};

struct Export {
    std::vector<int64_t> gids;
    std::vector<Value> modules;
};

struct JsxRow {
    std::unordered_map<uint64_t, Value> options;
    Value runtime, children;
};

// A key of several values, hashed as one.
struct Key3 {
    uint64_t a, b, c;
    bool operator==(const Key3&) const = default;
};
struct Key3Hash {
    size_t operator()(const Key3& k) const;
};

class Input {
public:
    Store store;
    std::vector<UnitRec> units;
    std::unordered_map<uint64_t, ClassRec> classes;                       // key bits -> class
    std::unordered_map<uint64_t, std::vector<std::pair<Value, std::vector<int64_t>>>> members;   // in written order
    std::unordered_map<Key3, NameRow, Key3Hash> names;                    // (gid, name)
    std::unordered_map<Key3, Value, Key3Hash> types;                      // (text, file, owner) -> (kind, target)|None
    std::unordered_map<uint64_t, std::vector<Value>> intrinsics;          // kind -> global type keys (None kept)
    std::unordered_set<uint64_t> modules;                                 // module files
    std::unordered_map<Key3, Value, Key3Hash> imports;                    // (file, spec) -> module | None
    std::unordered_map<Key3, Export, Key3Hash> exports;                   // (module, name)
    std::unordered_map<uint64_t, JsxRow> jsx;                             // file
    std::unordered_map<Key3, std::vector<int64_t>, Key3Hash> queries;     // (file, owner, name)
    std::unordered_map<Key3, std::vector<Value>, Key3Hash> operators;     // (op, unary)
    std::unordered_map<uint64_t, std::vector<Value>> literals;            // kind
    std::unordered_set<uint64_t> numeric_names;                           // canonical numeric strings: indexes
    std::unordered_set<int64_t> contextual;                               // functions whose runs need contexts
    std::unordered_map<uint64_t, int64_t> offset;                         // file -> first gid
    std::unordered_map<uint64_t, std::vector<int64_t>> module_units;      // file -> its module unit
    std::unordered_map<std::string_view, Value> vocabulary;
    uint64_t missing = 0;                         // resolutions asked for and not in the file (a bug of one side)
    std::vector<std::string> missed;              // the first of them, named
    void miss(std::string_view what, std::initializer_list<Value> key);

    Value vocab(std::string_view name) const;     // a constant by owner.NAME; throws if absent
    const ClassRec* cls(Value key) const;
    const std::vector<std::pair<Value, std::vector<int64_t>>>& members_of(Value owner) const;
    const std::vector<int64_t>& member(Value owner, Value name) const;
    const NameRow& name(int64_t gid, Value name);
    Value resolve(Value text, Value file, Value owner);
    const Export& export_of(Value module, Value name) const;
    const std::vector<int64_t>& query(Value file, Value owner, Value name);
};

// Reads the input file's root value into an Input.
void decode(Value root, Input& in);

}  // namespace ftengine
