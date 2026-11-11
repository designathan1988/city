// The semantics modules (each the port of the heap/ module of its name) and the kinds of transfer their subscriptions
// run: every closure the Python semantics subscribes is one kind here, its captured values packed in one tuple term.
#pragma once

#include "analysis.h"

namespace ftengine {

enum TransferKind : uint32_t {
    kMemberRead = 1, kSuperRead, kIndexByObject, kIndexKeys, kCallByReceiver, kCallFunctions, kSuperByThis,
    kPlainCall, kKeys, kConstructByValue, kConstructReturned, kNewWritten, kWriteByObject, kStoreByObject,
    kReadLink, kPrototypes, kTypeIndex, kBothOuter, kBothInner, kConsumed, kStructuralFn, kReceivers,
    kSummaryReceive, kSummaryCalled, kSummaryFields, kSummaryMemberOf, kKept, kIterElements, kIterIterator,
    kIterNext, kIterValue, kMethodCall, kSettle, kJsxFactory, kJsxChildren, kOpenCalled,
};

// The dispatcher the solver calls (transfers.cpp).
class Semantics : public Transfers {
public:
    explicit Semantics(Analysis& an) : an_(an) {}
    void apply(Solver& solver, Node target, const Transfer& transfer, std::span<const Obj> objects) override;
    std::string profile();                       // calls and objects handed, per kind, since the last call

private:
    Analysis& an_;
    std::unordered_map<uint32_t, std::pair<uint64_t, uint64_t>> handed_;
};

// A transfer of a kind with its captured values.
Transfer pack(Analysis& an, TransferKind kind, std::initializer_list<Value> captured);

namespace trees {
void wire(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx);
void wire_local(Analysis& an, Value node, const UnitRec& unit, Value name, Value ctx);
Value this_of(Analysis& an, const UnitRec& unit, Value ctx);
std::vector<Value> base_keys(Analysis& an, const UnitRec& unit);
Value constructed(Analysis& an, Value node, Value made);
Obj made(Analysis& an, Obj obj, Value file, Value site, Value ctx);
std::vector<Value> literal_kinds(Analysis& an, Value kind);
void construct_by_value(Analysis& an, Value target, Value node, Value args, std::span<const Obj> objs);
void index_by_object(Analysis& an, Value target, Value key, Value known, std::span<const Obj> objs);
}  // namespace trees

namespace refs {
void lower(Analysis& an, const UnitRec& unit, Value ctx);
void escape(Analysis& an, int64_t gid, int64_t line, Value value, Value ctx);
}

namespace names {
void wire_name(Analysis& an, Value node, const UnitRec& unit, Value name, Value ctx);
Value scope(Analysis& an, const UnitRec* unit, Value ctx, int64_t local_id);
std::pair<const UnitRec*, Value> enclosing(Analysis& an, const UnitRec& unit, Value name, Value ctx);
std::vector<int64_t> definitions(Analysis& an, const UnitRec& unit, Value name);
void value_of(Analysis& an, int64_t gid, Value node);
}  // namespace names

namespace members {
void read(Analysis& an, Value target, Obj obj, Value name);
bool declared_read(Analysis& an, Value target, Obj obj, Value key, Value name);
Value method_env(Analysis& an, Obj obj, int64_t gid);
Value environment(Analysis& an, Obj obj, Value owner);
void read_all(Analysis& an, Value target, Obj obj);
void write(Analysis& an, Value source, Obj obj, Value name);
void link_fields(Analysis& an, const UnitRec& unit, Obj obj, Value name, int64_t line, bool writing);
Obj prototype(Analysis& an, Value key, int64_t maker = -1);
Value field(Analysis& an, Obj obj, Value name);
Value every(Analysis& an, Obj obj);
std::vector<int64_t> lookup(Analysis& an, Value key, Value name);   // Index.lookup: members, a module's exports, bases
Value runs(Analysis& an, const UnitRec& prop, Value which, const UnitRec* reader);   // Unit.runs
}  // namespace members

namespace typesys {
Value resolve(Analysis& an, Value text, Value file, Value owner);
const std::vector<Value>& group(Analysis& an, Value key);   // [key] where it merges with none
Value identity(Analysis& an, Value key);
Value global_type(Analysis& an, Value kind, size_t index);
std::vector<int64_t> lookup(Analysis& an, Value key, Value name);
std::vector<Value> own_types(Analysis& an, Obj obj);
bool is_open_type(Analysis& an, Value key);
std::vector<Value> ancestors(Analysis& an, Value key);
bool has_member(Analysis& an, Obj obj, Value name);
bool declared(Analysis& an, int64_t gid);
Value place(Analysis& an, Obj obj, Value key, int64_t index);
std::vector<Value> parts(Analysis& an, Value tree);
std::vector<Value> values(Analysis& an, Value tree);
const std::vector<Value>& chain(Analysis& an, Value key);
const std::vector<Value>& base_keys(Analysis& an, Value key);
}  // namespace typesys

namespace values {
Value sub(Analysis& an, Value site, Value part);
Value member_site(Analysis& an, int64_t gid, Value site);
Value produce(Analysis& an, Value tree, Value env, Value site);
bool reads(Analysis& an, Value tree);
void consume(Analysis& an, Value tree, Value env, Value source, Value site, Value origin);
std::vector<Obj> language_objects(Analysis& an, Value values);
Value arguments(Analysis& an, Value args, std::span<const Value> defaults);
Value substitute(Analysis& an, Value tree, Value args);
bool on_path(Analysis& an, Value site, Value part);
void type_index(Analysis& an, Value target, Value texts, std::span<const Obj> objs);
void both_outer(Analysis& an, Value other, std::span<const Obj> objs);
void both_inner(Analysis& an, Value target, Obj o, std::span<const Obj> srcs);
}  // namespace values

namespace declared {
Value call_site(Analysis& an, Value node);
void apply(Analysis& an, Value node, int64_t gid, int64_t holder, std::span<const Value> args, Value site);
Value parameter(Analysis& an, const UnitRec& unit, size_t index);
Value positional(Analysis& an, Value types, int64_t rest, size_t index);
void taken(Analysis& an, Obj obj, Value tree, Value env, Value site, Value origin);
Value signature(Analysis& an, const UnitRec& unit);
void callback(Analysis& an, Obj fn, Value tree, Value env, Value site, Value origin, int64_t this_obj);
void call_object(Analysis& an, Value node, Obj fn, int64_t receiver, std::span<const Value> args, Value where);
void construct(Analysis& an, Value node, Obj obj, std::span<const Value> args);
Value an_node(Analysis& an, Obj obj);
bool reaches(Analysis& an, Value tree);
}  // namespace declared

namespace summaries {
bool is_open(Analysis& an, Value tree);
Value pool(Analysis& an, Value origin);
void summarize(Analysis& an, Value node, const UnitRec& unit, std::span<const Value> args, Value site);
void open_call(Analysis& an, Value node, std::span<const Value> args, Value site);
void receive(Analysis& an, Value node, Obj obj, Value names, Value where);
void reflected(Analysis& an, Value node, Value call, Obj obj, Obj value);
void called(Analysis& an, Value node, Obj fn, Value where);
void keep(Analysis& an, Obj holder, Obj fn, Value tree);
void kept(Analysis& an, Value node, Obj holder, const UnitRec& unit, std::span<const Value> args, Value site);
void kept_call(Analysis& an, Value node, Obj entry, Value offered, Value where);
bool is_object(Analysis& an, Obj obj);
}  // namespace summaries

namespace protocols {
void iterate(Analysis& an, Value node, Value source);
void method(Analysis& an, Obj obj, Value name, Value call, Value args);
void settle(Analysis& an, Value node, Value source);
void one(Analysis& an, Value node, Obj obj);
}  // namespace protocols

namespace jsx {
void value(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx);
Value call_node(Analysis& an, Value node, Value tree, const UnitRec& unit, Value ctx);
void written(Analysis& an, const UnitRec& unit, const RefRec& ref, Value ctx);
void named_children(Analysis& an, Obj obj, Value name);
}  // namespace jsx

namespace texts {
// The texts an expression can have (a frozenset term), or NONE where not all are known.
Value of(Analysis& an, Value tree, const UnitRec& unit);
Value argument(Analysis& an, Value node, size_t index);
}  // namespace texts

}  // namespace ftengine
