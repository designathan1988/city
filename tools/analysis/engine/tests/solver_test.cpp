// The solver's own test (log/trace-engine-plan.md, Task E2): identity edges, subscriptions and their first
// applications, cycles and duplicates reach the least fixpoint; a failed check prints its line and exits non-zero.
#include <cstdio>
#include <cstdlib>
#include <set>
#include <vector>

#include "solver.h"

namespace {

int failures = 0;

void check(bool ok, int line) {
    if (!ok) {
        std::printf("FAIL line %d\n", line);
        ++failures;
    }
}
#define CHECK(x) check((x), __LINE__)

std::set<ftengine::Obj> held(const ftengine::Solver& s, ftengine::Node n) {
    auto h = s.held(n);
    return {h.begin(), h.end()};
}

constexpr uint32_t kShift = 1;                   // the test transfer: each object o into the target as o + shift
constexpr uint32_t kSubscribe = 2;               // the test transfer: each object subscribes its own node

class Plus : public ftengine::Transfers {
public:
    std::vector<ftengine::Node> made;
    void apply(ftengine::Solver& s, ftengine::Node target, const ftengine::Transfer& t,
               std::span<const ftengine::Obj> objects) override {
        for (ftengine::Obj o : objects) {
            if (t.kind == kShift) {
                s.add(target, o + t.a);
            } else if (t.kind == kSubscribe) {          // a transfer that makes nodes and subscriptions
                ftengine::Node n = s.node();
                made.push_back(n);
                s.add(n, o);
                s.follow(n, target);
            }
        }
    }
};

void edges_and_cycles() {
    ftengine::Solver s;
    Plus plus;
    auto a = s.node(), b = s.node(), c = s.node();
    s.follow(a, b);
    s.follow(b, c);
    s.follow(c, a);
    s.add(a, 1);
    s.run(plus);
    CHECK((held(s, c) == std::set<ftengine::Obj>{1}));
    s.add(b, 2);
    s.run(plus);
    CHECK((held(s, a) == std::set<ftengine::Obj>{1, 2}));
    s.follow(a, b);                                  // a duplicate edge does nothing
    CHECK(s.held(b).size() == 2);
}

void subscriptions_and_first_applications() {
    ftengine::Solver s;
    Plus plus;
    auto a = s.node(), b = s.node();
    s.add(a, 1);
    s.add(a, 2);
    CHECK(s.subscribe(a, b, 0, {kShift, 10}));
    CHECK(!s.subscribe(a, b, 0, {kShift, 99}));     // once per (source, target, key)
    s.run(plus);
    CHECK((held(s, b) == std::set<ftengine::Obj>{11, 12}));
    s.add(a, 3);
    s.run(plus);
    CHECK((held(s, b) == std::set<ftengine::Obj>{11, 12, 13}));
    auto c = s.node();
    s.follow(a, c);                                  // a late edge gets what a already held
    s.run(plus);
    CHECK((held(s, c) == std::set<ftengine::Obj>{1, 2, 3}));
}

void transfers_that_grow_the_graph() {
    ftengine::Solver s;
    Plus plus;
    auto a = s.node(), b = s.node();
    for (ftengine::Obj o = 0; o < 100; ++o) s.add(a, o);   // past the set's index threshold
    s.subscribe(a, b, 0, {kSubscribe});
    s.run(plus);
    CHECK(s.held(b).size() == 100);
    CHECK(plus.made.size() == 100);
    CHECK(s.holds(b, 99) && !s.holds(b, 100));
}

}  // namespace

int main() {
    edges_and_cycles();
    subscriptions_and_first_applications();
    transfers_that_grow_the_graph();
    std::printf(failures ? "solver_test: %d failed\n" : "solver_test: ok\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
