// What the analysis shows of itself while it runs (log/trace-research.md F19-F21): the nodes and objects made
// since the last report by their key's tag, the nodes holding the most objects and what they are made of, the
// nodes whose deliveries cost the most; each printed by the command line every so many items.
#include <algorithm>

#include "analysis.h"

namespace ftengine {

namespace {
constexpr size_t kGrowthShown = 6;
constexpr size_t kComposed = 6;          // the largest nodes whose objects are listed
constexpr size_t kFedShown = 2;          // the costliest properties whose sources are listed
constexpr size_t kOriginSteps = 40;      // how far back the origin of the costliest node is followed
constexpr size_t kFedSources = 12;       // how many sources of each
constexpr size_t kComposedKinds = 12;
constexpr size_t kLabel = 60;
constexpr size_t kContextsShown = 3;     // the functions with the most runs whose last contexts are shown, and how many
constexpr int kContextDepth = -6;        // a context described deeper than other keys (its environments nest)

std::string tag_text(const Analysis& an, Value key) {
    Value tag = an.item(key, 0);
    if (tag.kind() == Kind::Str) return std::string(an.text(tag));
    if (tag.kind() == Kind::Int) return std::to_string(tag.data());
    return "?";
}

// A context as text: its environments nested, each call's site object by its key.
std::string context_text(Analysis& an, Value ctx, int depth) {
    if (ctx.kind() == Kind::Int && ctx.data() >= 0 && static_cast<size_t>(ctx.data()) < an.objects()) {
        return "#" + an.describe(an.okey(static_cast<Obj>(ctx.data())), 1);
    }
    if (ctx.kind() != Kind::Term || depth >= 0) return an.describe(ctx);
    std::string out = "(";
    for (Value x : an.at(ctx)) out += context_text(an, x, depth + 1) + " ";
    return out + ")";
}

std::string top(std::unordered_map<std::string, size_t>& counts) {
    std::vector<std::pair<size_t, std::string>> rows;
    for (auto& [tag, n] : counts) rows.emplace_back(n, tag);
    std::sort(rows.rbegin(), rows.rend());
    std::string out;
    for (size_t i = 0; i < rows.size() && i < kGrowthShown; ++i) {
        out += " " + rows[i].second + "=" + std::to_string(rows[i].first);
    }
    return out;
}
}  // namespace

std::string Analysis::growth() {
    std::unordered_map<std::string, size_t> nodes, objects;
    for (size_t n = grown_nodes_; n < node_keys_.size(); ++n) {
        Value key = node_keys_[n];
        std::string tag = tag_text(*this, key);
        if (item(key, 0) == V.PRODUCED) {          // which declared type, and whether a receiver is in its env
            tag += "/" + tag_text(*this, item(key, 1));
            tag += item(item(key, 2), 0).kind() == Kind::None ? "/-" : "/r";
        }
        ++nodes[tag];
    }
    std::unordered_map<std::string, Value> sample;            // the last key made of each tag, shown for the most made
    for (size_t o = grown_objects_; o < objects_.size(); ++o) {
        std::string tag = tag_text(*this, objects_[o]);
        if (item(objects_[o], 0) == V.SITE_DECLARED) tag += ":" + describe(item(objects_[o], 1));   // of which type
        ++objects[tag];
        sample[tag] = objects_[o];
    }
    std::string shown;
    size_t most = 0;
    for (auto& [tag, n] : objects) {
        if (n > most) most = n, shown = "\n         e.g. " + describe(sample[tag], kContextDepth) + site_chain(sample[tag]);
    }
    grown_nodes_ = node_keys_.size();
    grown_objects_ = objects_.size();
    return "new nodes:" + top(nodes) + " | new objects:" + top(objects) + shown;
}

// The objects a key's site is made from, outward: the first object id among its parts after its tag, then that
// object's, and so on (a site made from a site made from a site shows how its keys nest).
std::string Analysis::site_chain(Value key) {
    constexpr int kLinks = 5;
    std::string out;
    for (int link = 0; link < kLinks; ++link) {
        std::vector<Value> todo;
        auto parts = at(key);
        for (size_t i = parts.size(); i > 1; --i) todo.push_back(parts[i - 1]);
        int64_t found = -1;
        while (!todo.empty() && found < 0) {
            Value v = todo.back();
            todo.pop_back();
            if (v.kind() == Kind::Int && v.data() >= 0 && static_cast<size_t>(v.data()) < objects_.size()) {
                found = v.data();
            } else if (v.kind() == Kind::Term) {
                auto inner = at(v);
                for (size_t i = inner.size(); i > 0; --i) todo.push_back(inner[i - 1]);
            }
        }
        if (found < 0) break;
        key = objects_[static_cast<size_t>(found)];
        out += "\n            from #" + std::to_string(found) + " " + describe(key, kContextDepth);
    }
    return out;
}

std::string Analysis::describe(Value v, int depth) {
    constexpr int kDepth = 3;
    switch (v.kind()) {
    case Kind::None: return "None";
    case Kind::True: return "True";
    case Kind::False: return "False";
    case Kind::Int: return std::to_string(v.data());
    case Kind::Str: case Kind::Class: {
        std::string t(text(v));
        auto slash = t.rfind('/');
        return slash == std::string::npos ? t : t.substr(slash + 1);
    }
    case Kind::Term: {
        if (depth >= kDepth) return "(..)";
        std::string out = "(";
        for (Value x : at(v)) out += describe(x, depth + 1) + ",";
        return out + ")";
    }
    }
    return "?";
}

std::string Analysis::stores(size_t count) {
    std::vector<std::pair<uint64_t, uint64_t>> rows;
    for (auto& [site, n] : store_probe) rows.emplace_back(n.first * n.second, site);
    std::sort(rows.rbegin(), rows.rend());
    std::string out;
    for (size_t i = 0; i < rows.size() && i < count; ++i) {
        Value site = Value::from_bits(rows[i].second);
        const UnitRec& u = unit(item(site, 0).data());
        auto& n = store_probe[rows[i].second];
        out += "\n      " + std::to_string(n.first) + " bases x " + std::to_string(n.second) + " values at " +
               describe(u.qual) + ":" + std::to_string(item(site, 1).data());
    }
    return out;
}

std::string Analysis::runs(size_t count) {
    std::unordered_map<int64_t, size_t> nodes_of;          // a function's nodes: those keyed by its gid second
    for (Value key : node_keys_) {
        Value tag = item(key, 0);
        if ((tag == V.TREE || tag == V.LOCAL || tag == V.PARAM || tag == V.THIS || tag == V.RET) &&
            item(key, 1).kind() == Kind::Int) {
            ++nodes_of[item(key, 1).data()];
        }
    }
    std::vector<std::pair<size_t, int64_t>> rows;
    for (const auto& [gid, ctxs] : active_order) rows.emplace_back(ctxs.size(), static_cast<int64_t>(gid));
    std::sort(rows.rbegin(), rows.rend());
    size_t total = 0;
    for (const auto& r : rows) total += r.first;
    std::string out = " " + std::to_string(rows.size()) + " functions in " + std::to_string(total) + " runs";
    for (size_t i = 0; i < rows.size() && i < count; ++i) {
        const UnitRec& u = unit(rows[i].second);
        out += "\n      " + std::to_string(rows[i].first) + " runs, " + std::to_string(nodes_of[rows[i].second]) +
               " nodes: " + std::string(text(u.qual)).substr(0, kLabel) + " params=" + std::to_string(u.params.size()) +
               " result=" + describe(u.result_type);
        if (i >= kContextsShown) continue;
        const auto& ctxs = active_order[static_cast<uint64_t>(rows[i].second)];
        for (size_t j = 0; j < ctxs.size() && j < kContextsShown; ++j) {   // the last ones made: what grows
            out += "\n         context " + context_text(*this, ctxs[ctxs.size() - 1 - j], kContextDepth);
        }
        out += "\n         " + context_spread(ctxs) + callers(ctxs);
    }
    return out;
}

// The units whose calls a function's runs come from (each context's own call), the most first, one call of each shown.
std::string Analysis::callers(const std::vector<Value>& ctxs) {
    constexpr size_t kCallers = 10;
    constexpr int kDeeper = -2;
    std::unordered_map<int64_t, std::pair<size_t, Value>> by_unit;     // gid -> (distinct calls, one call's key)
    std::unordered_set<uint64_t> seen;
    for (Value c : ctxs) {
        Value site = item(c, 1);
        if (!seen.insert(site.bits()).second) continue;
        Value key = site.kind() == Kind::Int && site.data() >= 0 ? okey(static_cast<Obj>(site.data())) : site;
        Value gid = item(key, 1);
        auto& row = by_unit[gid.kind() == Kind::Int ? gid.data() : -1];
        ++row.first;
        row.second = key;
    }
    std::vector<std::pair<size_t, int64_t>> rows;
    for (const auto& [gid, row] : by_unit) rows.emplace_back(row.first, gid);
    std::sort(rows.rbegin(), rows.rend());
    std::string out;
    for (size_t i = 0; i < rows.size() && i < kCallers; ++i) {
        int64_t gid = rows[i].second;
        bool known = gid >= 0 && gid < static_cast<int64_t>(in.units.size());
        out += "\n            " + std::to_string(rows[i].first) + " calls in " +
               (known ? std::string(text(unit(gid).qual)) : std::string("?")) + ": " +
               describe(by_unit[gid].second, kDeeper);
    }
    return out;
}

// How many different values each level of a function's contexts takes: its call, its environment's call, and so on.
std::string Analysis::context_spread(const std::vector<Value>& ctxs) {
    std::string out = "distinct per level (call, its environment's call, ...):";
    std::vector<Value> level(ctxs.begin(), ctxs.end());
    for (int depth = 0; depth < -kContextDepth && !level.empty(); ++depth) {
        std::unordered_map<uint64_t, size_t> calls;
        std::vector<Value> envs;
        std::unordered_set<uint64_t> seen;
        for (Value c : level) {
            ++calls[item(c, 1).bits()];
            Value env = item(c, 0);
            if (at(env).size() == 2 && seen.insert(env.bits()).second) envs.push_back(env);
        }
        uint64_t top_bits = 0;
        size_t top_n = 0;
        for (auto& [bits, n] : calls) {
            if (n > top_n) top_bits = bits, top_n = n;
        }
        out += " " + std::to_string(calls.size()) + " calls (the most common " +
               context_text(*this, Value::from_bits(top_bits), kContextDepth) + " x" + std::to_string(top_n) + ") /";
        level.swap(envs);
    }
    return out;
}

// Where a node's objects come from: from it, back along identity edges, each step to the source holding the most
// objects, until a node no identity edge feeds (a tree's own wiring, a transfer) or one already on the way.
std::string Analysis::origin(Node n) {
    std::string out = "\n         origin:";
    std::unordered_set<Node> seen{solver.find(n)};
    for (size_t step = 0; step < kOriginSteps; ++step) {
        auto from = solver.sources(n);
        Node best = n;
        size_t most = 0;
        for (Node f : from) {
            size_t held = solver.held(f).size();
            if (held > most && !seen.count(solver.find(f))) best = f, most = held;
        }
        if (best == n) {
            out += "\n            (fed by no identity edge, or only by nodes on the way: " +
                   std::to_string(from.size()) + " sources)";
            break;
        }
        seen.insert(solver.find(best));
        Value key = node_keys_[best];
        out += "\n            held " + std::to_string(most) + " of " + std::to_string(from.size()) + " sources: " +
               describe(key, -2);
        if (item(key, 0) == V.FIELD) out += " obj=" + describe(okey(static_cast<Obj>(item(key, 1).data())));
        if (item(key, 0) == V.TREE && item(key, 1).kind() == Kind::Int) {
            out += " in " + std::string(text(unit(item(key, 1).data()).qual));
        }
        n = best;
    }
    return out;
}

std::string Analysis::costliest(size_t count) {
    std::string out;
    size_t shown = 0;
    for (const auto& [cost, n] : solver.take_costs(count)) {
        Value key = node_keys_[n];
        out += "\n      " + std::to_string(cost) + " held " + std::to_string(solver.held(n).size()) + " " + describe(key);
        if (item(key, 0) == V.FIELD || item(key, 0) == V.MEMBER || item(key, 0) == V.GD_MEMBER) {
            out += " obj=" + describe(okey(static_cast<Obj>(item(key, 1).data())));
        }
        if (shown == 0) out += origin(n);                          // where the costliest node's objects come from
        if (shown < kFedShown && item(key, 0) == V.FIELD) {      // what feeds a costly property: its identity edges
            auto from = solver.sources(n);
            out += "\n         fed by " + std::to_string(from.size()) + " nodes:";
            for (size_t j = 0; j < from.size() && j < kFedSources; ++j) {
                Value source = node_keys_[from[j]];
                out += "\n            held " + std::to_string(solver.held(from[j]).size()) + " " + describe(source, -1);
                if (item(source, 0) == V.FIELD) out += " obj=" + describe(okey(static_cast<Obj>(item(source, 1).data())));
            }
        }
        if (shown++ < kComposed) out += composition(n);
    }
    return out;
}

std::string Analysis::largest(size_t count) {
    std::vector<std::pair<size_t, Node>> rows;
    for (Node n = 0; n < solver.nodes(); ++n) rows.emplace_back(solver.held(n).size(), n);
    std::partial_sort(rows.begin(), rows.begin() + std::min(count, rows.size()), rows.end(),
                      [](const auto& a, const auto& b) { return a.first > b.first; });
    std::string out;
    for (size_t i = 0; i < count && i < rows.size(); ++i) {
        Value key = node_keys_[rows[i].second];
        std::string shown = describe(key);
        Value tag = item(key, 0);
        if (tag == V.MEMBER || tag == V.FIELD) shown += " obj=" + describe(okey(static_cast<Obj>(item(key, 1).data())));
        out += "\n      " + std::to_string(rows[i].first) + " " + shown;
        if (i < kComposed) out += composition(rows[i].second);
    }
    return out;
}

std::string Analysis::composition(Node n) {
    std::unordered_map<std::string, size_t> kinds;
    for (Obj o : solver.held(n)) {
        Value k = okey(o);
        std::string what = describe(item(k, 0));
        if (item(k, 0) == V.SITE_FUNCTION) {
            what += ":" + std::string(text(unit(item(k, 1).data()).qual)).substr(0, kLabel);
        } else if (item(k, 1).kind() == Kind::Str) {
            what += ":" + describe(item(k, 1));
        }
        ++kinds[what];
    }
    std::vector<std::pair<size_t, std::string>> parts;
    for (auto& [w, count] : kinds) parts.emplace_back(count, w);
    std::sort(parts.rbegin(), parts.rend());
    std::string out = "\n         made of " + std::to_string(parts.size()) + " kinds:";
    for (size_t j = 0; j < parts.size() && j < kComposedKinds; ++j) {
        out += " " + parts[j].second + "=" + std::to_string(parts[j].first);
    }
    return out;
}

}  // namespace ftengine
