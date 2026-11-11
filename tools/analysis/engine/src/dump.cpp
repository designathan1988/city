// The canonical text of a value: each term's text made once and reused where the term is shared.
#include "dump.h"

#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ftengine {

namespace {

constexpr char kHexDigits[] = "0123456789abcdef";
constexpr int kNibbleBits = 4;
constexpr unsigned kNibbleMask = 0xF;

void hex(std::string& out, std::string_view bytes) {
    for (unsigned char c : bytes) {
        out += kHexDigits[c >> kNibbleBits];
        out += kHexDigits[c & kNibbleMask];
    }
}

class Dumper {
public:
    explicit Dumper(const Store& store) : store_(store) {}

    void value(std::string& out, Value v) {
        switch (v.kind()) {
        case Kind::None: out += 'N'; return;
        case Kind::True: out += 'T'; return;
        case Kind::False: out += 'F'; return;
        case Kind::Int: out += 'i'; out += std::to_string(v.data()); return;
        case Kind::Str: out += 's'; hex(out, store_.text(v.id())); return;
        case Kind::Class: out += 'c'; hex(out, store_.text(v.id())); return;
        case Kind::Term: out += term(v.id()); return;
        }
    }

private:
    const std::string& term(uint32_t id) {
        auto found = done_.find(id);
        if (found != done_.end()) return found->second;
        std::string out;
        auto items = store_.items(id);
        switch (store_.shape(id)) {
        case Shape::Tuple: sequence(out, '(', items, ')'); break;
        case Shape::List: sequence(out, '[', items, ']'); break;
        case Shape::Dict: pairs(out, items); break;
        case Shape::Set: sorted(out, "<", items); break;
        case Shape::FrozenSet: sorted(out, "f<", items); break;
        case Shape::Object:
            out += '@';
            hex(out, store_.text(store_.head(id)));
            pairs(out, items);
            break;
        }
        return done_.emplace(id, std::move(out)).first->second;
    }
    void sequence(std::string& out, char open, std::span<const Value> items, char close) {
        out += open;
        for (size_t i = 0; i < items.size(); ++i) {
            if (i) out += ',';
            value(out, items[i]);
        }
        out += close;
    }
    void pairs(std::string& out, std::span<const Value> items) {
        out += '{';
        for (size_t i = 0; i + 1 < items.size(); i += 2) {
            if (i) out += ',';
            value(out, items[i]);
            out += ':';
            value(out, items[i + 1]);
        }
        out += '}';
    }
    void sorted(std::string& out, const char* open, std::span<const Value> items) {
        std::vector<std::string> texts(items.size());
        for (size_t i = 0; i < items.size(); ++i) value(texts[i], items[i]);
        std::sort(texts.begin(), texts.end());
        out += open;
        for (size_t i = 0; i < texts.size(); ++i) {
            if (i) out += ',';
            out += texts[i];
        }
        out += '>';
    }

    const Store& store_;
    std::unordered_map<uint32_t, std::string> done_;
};

}  // namespace

std::string dump(const Store& store, Value value) {
    std::string out;
    Dumper(store).value(out, value);
    return out;
}

}  // namespace ftengine
