// The transfers the subscriptions run: each kind is the body of one closure the Python semantics subscribe (one
// function here, found in a table by its kind), its captured values unpacked from the transfer's tuple term, applied to
// the objects new to the subscription's source.
#include <array>

#include "semantics.h"

namespace ftengine {

namespace {

// The values a transfer captured.
struct Captured {
    std::span<const Value> items;
    Value operator[](size_t i) const { return i < items.size() ? items[i] : NONE; }
};
using Objs = std::span<const Obj>;
using Handler = void (*)(Analysis&, Value target, const Captured&, Objs);

std::vector<Value> list(Analysis& an, Value v) {
    auto items = an.at(v);
    return {items.begin(), items.end()};
}
Obj obj_of(Value v) { return static_cast<Obj>(v.data()); }

void member_read(Analysis& an, Value t, const Captured& c, Objs objs) {
    for (Obj o : objs) members::read(an, t, o, c[0]);
}
void super_read(Analysis& an, Value t, const Captured& c, Objs objs) {
    auto bases = trees::base_keys(an, an.unit(c[1].data()));
    for (Obj o : objs) {
        for (Value k : bases) members::declared_read(an, t, o, k, c[0]);
    }
}
void index_by_object(Analysis& an, Value t, const Captured& c, Objs objs) {
    trees::index_by_object(an, t, c[0], c[1], objs);
}
void index_keys(Analysis& an, Value t, const Captured& c, Objs objs) {
    for (Obj k : objs) members::load_by_key(an, t, obj_of(c[0]), k);
}
void store_by_key(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj k : objs) members::store_by_key(an, c[0], obj_of(c[1]), k);
}
void call_by_receiver(Analysis& an, Value t, const Captured& c, Objs objs) {
    for (Obj obj : objs) {
        Value held = an.tuple({an.V.MEMBER, I(obj), c[0]});
        members::read(an, held, obj, c[0]);
        an.subscribe(held, t, an.tuple({an.V.CALL, I(obj)}), pack(an, kCallFunctions, {I(obj), c[1]}));
    }
}
void call_functions(Analysis& an, Value t, const Captured& c, Objs objs) {
    auto args = list(an, c[1]);
    for (Obj fn : objs) an.call(t, fn, c[0].data(), args, an.V.CALL);
}
void super_by_this(Analysis& an, Value t, const Captured& c, Objs objs) {
    auto bases = trees::base_keys(an, an.unit(c[2].data()));
    for (Obj obj : objs) {
        Value held = an.tuple({an.V.SUPER, I(obj), c[0], c[2]});
        for (Value k : bases) members::declared_read(an, held, obj, k, c[0]);
        an.subscribe(held, t, an.tuple({an.V.CALL, I(obj)}), pack(an, kCallFunctions, {I(obj), c[1]}));
    }
}
void plain_call(Analysis& an, Value t, const Captured& c, Objs objs) {
    auto args = list(an, c[0]);
    for (Obj fn : objs) an.call(t, fn, -1, args, an.V.CALL);
}
void keys(Analysis& an, Value t, const Captured&, Objs objs) {
    for (Obj o : objs) an.add(t, an.make({an.V.SITE_KEY, I(o)}));
}
void construct_by_value(Analysis& an, Value t, const Captured& c, Objs objs) {
    trees::construct_by_value(an, t, c[0], c[1], objs);
}
void construct_returned(Analysis& an, Value t, const Captured&, Objs objs) {
    for (Obj o : objs) {
        if (an.okind(o) != an.V.SITE_PRIMITIVE && an.okind(o) != an.V.SITE_TEXT) an.add(t, o);
    }
}
void new_written(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) {
        Value made = an.made_by(o);
        an.written(trees::constructed(an, c[0], made.kind() != Kind::None ? made : I(o)), c[1].data(), c[2].data());
    }
}
void write_by_object(Analysis& an, Value, const Captured& c, Objs objs) {
    const UnitRec& unit = an.unit(c[2].data());
    for (Obj o : objs) {
        if (an.okind(o) == an.V.SITE_UNKNOWN) {
            refs::escape(an, c[2].data(), c[3].data(), c[0], c[4]);
            continue;
        }
        members::write(an, c[0], o, c[1]);
        members::link_fields(an, unit, o, c[1], c[3].data(), true);
    }
}
void store_by_object(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) {
        if (an.okind(o) == an.V.SITE_UNKNOWN) {
            refs::escape(an, c[2].data(), c[3].data(), c[0], c[4]);
            continue;
        }
        if (an.okind(o) == an.V.SITE_PRIMITIVE) continue;
        if (c[1].kind() != Kind::None) {
            for (Value text : an.at(c[1])) members::write(an, c[0], o, text);
        } else {
            auto& probe = an.store_probe[an.tuple({c[2], c[3]}).bits()];
            ++probe.first;
            probe.second = an.held(c[0]).size();
            an.subscribe(c[5], an.tuple({an.V.STORE, c[2], c[3], c[4], I(o)}), an.tuple({an.V.STORE, I(o)}),
                         pack(an, kStoreByKey, {c[0], I(o)}));
        }
    }
}
void read_link(Analysis& an, Value, const Captured& c, Objs objs) {
    const UnitRec& unit = an.unit(c[0].data());
    for (Obj o : objs) members::link_fields(an, unit, o, c[1], c[2].data(), false);
}
void prototypes(Analysis& an, Value t, const Captured& c, Objs objs) {
    for (Obj p : objs) members::read(an, t, p, c[0]);
}
void type_index(Analysis& an, Value t, const Captured& c, Objs objs) { values::type_index(an, t, c[0], objs); }
void both_outer(Analysis& an, Value, const Captured& c, Objs objs) { values::both_outer(an, c[0], objs); }
void both_inner(Analysis& an, Value t, const Captured& c, Objs objs) { values::both_inner(an, t, obj_of(c[0]), objs); }
void consumed(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) declared::taken(an, o, c[0], c[1], c[2], c[3]);
}
void structural_fn(Analysis& an, Value, const Captured& c, Objs objs) {
    const UnitRec& unit = an.unit(c[0].data());
    Value sig = declared::signature(an, unit);
    Value site = values::member_site(an, unit.gid, c[2]);
    for (Obj f : objs) declared::callback(an, f, sig, c[1], site, c[3], c[4].data());
}
void receivers(Analysis& an, Value, const Captured& c, Objs objs) {
    auto args = list(an, c[2]);
    for (Obj o : objs) an.call(c[0], obj_of(c[1]), o, args, an.V.HANDED, c[3]);
}
void summary_receive(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) {
        if (summaries::is_object(an, o)) summaries::receive(an, c[0], o, c[1], c[2]);
    }
}
void summary_called(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) {
        if (an.function(o) >= 0) summaries::called(an, c[0], o, c[1]);
    }
}
void summary_fields(Analysis& an, Value t, const Captured& c, Objs objs) {
    Obj self = obj_of(c[0]);
    for (Obj o : objs) {
        if (o != self) an.add(t, o);
    }
}
void summary_member_of(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) summaries::reflected(an, c[0], c[1], obj_of(c[2]), o);
}
void kept(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj e : objs) summaries::kept_call(an, c[0], e, c[1], c[2]);
}
void iter_elements(Analysis& an, Value t, const Captured&, Objs objs) {
    for (Obj o : objs) an.follow(members::field(an, o, an.V.ELEMENTS_FIELD), t);
}
void iter_iterator(Analysis& an, Value t, const Captured&, Objs objs) {
    for (Obj o : objs) protocols::method(an, o, an.V.ITERATOR, t, an.tuple({}));
}
void iter_next(Analysis& an, Value t, const Captured&, Objs objs) {
    for (Obj o : objs) protocols::method(an, o, an.V.NEXT, t, an.tuple({}));
}
void iter_value(Analysis& an, Value t, const Captured&, Objs objs) {
    for (Obj o : objs) members::read(an, t, o, an.V.VALUE);
}
void method_call(Analysis& an, Value t, const Captured& c, Objs objs) {
    auto args = list(an, c[1]);
    for (Obj f : objs) an.call(t, f, c[0].data(), args, an.V.CALL);
}
void settle(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) protocols::one(an, c[0], o);
}
void jsx_factory(Analysis& an, Value t, const Captured& c, Objs objs) {
    auto args = list(an, c[0]);
    for (Obj f : objs) an.call(t, f, -1, args, an.V.CALL);
}
void jsx_children(Analysis& an, Value, const Captured& c, Objs objs) {
    for (Obj o : objs) jsx::named_children(an, o, c[0]);
}

// The handler of each kind, in the order of TransferKind (from kMemberRead).
constexpr std::array<Handler, kStoreByKey> kHandlers = {
    member_read, super_read, index_by_object, index_keys, call_by_receiver, call_functions, super_by_this,
    plain_call, keys, construct_by_value, construct_returned, new_written, write_by_object, store_by_object,
    read_link, prototypes, type_index, both_outer, both_inner, consumed, structural_fn, receivers,
    summary_receive, summary_called, summary_fields, summary_member_of, kept, iter_elements, iter_iterator,
    iter_next, iter_value, method_call, settle, jsx_factory, jsx_children, summary_called, store_by_key,
};

}  // namespace

std::string Semantics::profile() {
    constexpr size_t kShown = 8;
    std::vector<std::pair<uint64_t, uint32_t>> rows;
    for (auto& [kind, n] : handed_) rows.emplace_back(n.second, kind);
    std::sort(rows.rbegin(), rows.rend());
    std::string out = "handed (kind=calls/objects):";
    for (size_t i = 0; i < rows.size() && i < kShown; ++i) {
        out += " " + std::to_string(rows[i].second) + "=" + std::to_string(handed_[rows[i].second].first) + "/" +
               std::to_string(rows[i].first);
    }
    handed_.clear();
    return out;
}

Transfer pack(Analysis& an, TransferKind kind, std::initializer_list<Value> captured) {
    return {kind, an.tuple(captured).id(), 0, 0};
}

void Semantics::apply(Solver&, Node target, const Transfer& transfer, std::span<const Obj> objs) {
    auto& count = handed_[transfer.kind];
    ++count.first;
    count.second += objs.size();
    kHandlers[transfer.kind - kMemberRead](an_, an_.key_of(target), Captured{an_.at(TV(transfer.a))}, objs);
}

}  // namespace ftengine
