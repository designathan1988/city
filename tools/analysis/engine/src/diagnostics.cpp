// What the analysis shows of itself while it runs (log/trace-research.md F19-F21): the nodes and objects made
// since the last report by their key's tag, the nodes holding the most objects and what they are made of, the
// nodes whose deliveries cost the most; each printed by the command line every so many items.
#include <algorithm>

#include "analysis.h"

namespace ftengine {

namespace {
constexpr size_t kGrowthShown = 6;
constexpr size_t kComposed = 6;          // the largest nodes whose objects are listed
constexpr size_t kComposedKinds = 12;
constexpr size_t kLabel = 60;

std::string tag_text(const Analysis& an, Value key) {
    Value tag = an.item(key, 0);
    if (tag.kind() == Kind::Str) return std::string(an.text(tag));
    if (tag.kind() == Kind::Int) return std::to_string(tag.data());
    return "?";
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
    for (size_t o = grown_objects_; o < objects_.size(); ++o) ++objects[tag_text(*this, objects_[o])];
    grown_nodes_ = node_keys_.size();
    grown_objects_ = objects_.size();
    return "new nodes:" + top(nodes) + " | new objects:" + top(objects);
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

std::string Analysis::costliest(size_t count) {
    std::string out;
    for (const auto& [cost, n] : solver.take_costs(count)) {
        Value key = node_keys_[n];
        out += "\n      " + std::to_string(cost) + " held " + std::to_string(solver.held(n).size()) + " " + describe(key);
        if (item(key, 0) == V.FIELD || item(key, 0) == V.MEMBER) {
            out += " obj=" + describe(okey(static_cast<Obj>(item(key, 1).data())));
        }
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
        if (i >= kComposed) continue;
        std::unordered_map<std::string, size_t> kinds;
        for (Obj o : solver.held(rows[i].second)) {
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
        for (auto& [w, n] : kinds) parts.emplace_back(n, w);
        std::sort(parts.rbegin(), parts.rend());
        out += "\n         made of " + std::to_string(parts.size()) + " kinds:";
        for (size_t j = 0; j < parts.size() && j < kComposedKinds; ++j) {
            out += " " + parts[j].second + "=" + std::to_string(parts[j].first);
        }
    }
    return out;
}

}  // namespace ftengine
