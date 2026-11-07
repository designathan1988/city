"""Injected C++: one small case of each class and each cost, under fresh words. Each case says what must be found and by which word."""


def source(w, expect):
    """The translation unit. `w` gives a fresh word per case; `expect` records (class or category, word)."""
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    d1, p1, m1, s1, l1 = case("B1"), case("B3"), case("B4"), case("B5"), case("B6")
    t1, o1, r1, k1, c1 = case("B7"), case("B9"), case("B10"), case("B11"), case("B12")
    v1, h1, q1, x1 = case("B2"), case("B13"), case("B14"), case("D3")
    a1, n1, f1, cc = case("B61"), case("B62"), case("B56"), case("B60")
    al, th, nt, vr = case("B59"), case("B74"), case("D7"), case("B8")
    bd, st, rf, bf, tc = case("B28"), case("B27"), case("B57"), case("B63"), case("test_code_shipped")
    g = {c: case(c) for c in ("grows_with_state", "nested_loops", "allocation", "lock_contention", "lock_held_work", "boundary_in_loop", "copy", "linear_search",
                              "redone_work", "memory_growth", "streaming", "loading", "serial_decode", "long_save", "thread_churn", "false_sharing", "no_effect",
                              "costly_option", "boundary_conversion")}
    return f"""// Injected by the selfcheck: every case below stands for one class of link or one cost.
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define {x1}_LIST(X) X(tag_{x1}, leaf_{x1})
#define {vr}_MACRO(a) (a + 1)

namespace injected_{d1} {{

struct Base_{h1} {{
	virtual ~Base_{h1}() {{}}
	virtual int {v1}_run() {{ return 1; }}
	virtual void _process(double delta) {{ (void)delta; }}
	virtual void _ready() {{}}
}};

struct Child_{h1} : Base_{h1} {{
	int {v1}_run() override {{ return 2; }}
}};

struct {o1}_num {{
	int v = 0;
	{o1}_num operator+(const {o1}_num &b) const {{ return {o1}_num{{v + b.v}}; }}
}};

struct {r1}_guard {{
	{r1}_guard() {{}}
	~{r1}_guard() {{}}
}};

struct {c1}_text {{
	{c1}_text(const char *) {{}}
}};

struct {st}_object {{
	int call(const char *name) {{ return name != nullptr; }}
}};

template <typename T> T {t1}_twice(T v) {{ return v + v; }}
int {q1}_over(int a) {{ return a; }}
int {q1}_over(float a) {{ return int(a); }}
int {d1}_callee(int x) {{ return x + 1; }}
int {d1}_caller() {{ return {d1}_callee(1); }}
int {p1}_target(int x) {{ return x; }}
void {c1}_grab({c1}_text t) {{ (void)t; }}
void {bd}_bind(const char *name, void (*fn)()) {{ (void)name; (void)fn; }}
void {bd}_method() {{}}
void {tc}_native() {{}}
void {bd}_register() {{
	{bd}_bind("{bd}_method", &{bd}_method);
	{bd}_bind("{tc}_native", &{tc}_native);
}}
void {rf}_alter(int &out) {{ out = 1; }}

struct Frames_{bf} {{
	int frames[3] = {{ 0, 0, 0 }};
}};

struct World_{d1} : Base_{h1} {{
	std::vector<int> items, {g["redone_work"]}_table, {g["memory_growth"]}_kept;
	int {f1}_value = 0, {g["no_effect"]}_dropped = 0, {al}_real = 0, {m1}_slot = 0, {rf}_field = 0;
	std::function<int()> {s1}_held;
	std::atomic<int> {a1}_count{{ 0 }}, {g["false_sharing"]}_a{{ 0 }}, {g["false_sharing"]}_b{{ 0 }};
	std::mutex {n1}_gate, {g["lock_contention"]}_gate, {k1}_gate, {g["lock_held_work"]}_gate, {g["long_save"]}_gate;
	std::condition_variable {nt}_ready;
	int {g["costly_option"]}_mode = 0;
	std::function<void()> {l1}_later;
	std::vector<std::function<void()>> pending;

	void {f1}_write() {{ {f1}_value = 1; }}
	int {f1}_read() const {{ return {f1}_value; }}
	void add(int v) {{ items.push_back(v); {g["memory_growth"]}_kept.push_back(v); {g["redone_work"]}_table.push_back(v); {g["costly_option"]}_mode = v; {g["no_effect"]}_dropped = v; }}
	void {al}_alter() {{ int &r = {al}_real; r = 2; }}
	void {cc}_unlock() const {{ const_cast<World_{d1} *>(this)->{f1}_value = 3; }}
	int {m1}_read() {{ int World_{d1}::*part = &World_{d1}::{m1}_slot; return this->*part; }}
	int {p1}_employ() {{ int (*fn)(int) = &{p1}_target; return fn(1); }}
	int {s1}_employ() {{ return {s1}_held(); }}
	void {l1}_keep(std::function<void()> job) {{ {l1}_later = job; }}
	void {l1}_employ() {{ {l1}_keep([this]() {{ {f1}_value = 8; }}); }}
	void {k1}_grab() {{ std::lock_guard<std::mutex> hold({k1}_gate); {f1}_value = 4; }}
	void {rf}_employ() {{ {rf}_alter({rf}_field); }}
	void {th}_start() {{ std::thread t([this]() {{ while (true) {{ {g["lock_contention"]}_tick(); {g["false_sharing"]}_a = 1; }} }}); t.detach(); }}
	void {g["lock_contention"]}_tick() {{ std::lock_guard<std::mutex> hold({g["lock_contention"]}_gate); {f1}_value = 5; }}
	void {nt}_wake() {{ {nt}_ready.notify_one(); }}
	void submit(std::function<void()> job) {{ pending.push_back(job); }}
	int {g["loading"]}_heavy() {{ int total = 0; for (int v : items) total += v; return total; }}
	int {g["serial_decode"]}_read() {{ return int(items.size()); }}
	void {g["loading"]}_first() {{ {g["loading"]}_heavy(); }}
	void {g["loading"]}_second() {{ {g["loading"]}_heavy(); }}
	void _ready() override {{
		{g["loading"]}_first();
		{g["loading"]}_second();
		{g["serial_decode"]}_read();
		{g["serial_decode"]}_read();
		{g["long_save"]}_save();
		{g["thread_churn"]}_each();
	}}
	void {g["long_save"]}_save() {{ std::lock_guard<std::mutex> hold({g["long_save"]}_gate); {f1}_value = 6; }}
	void {g["thread_churn"]}_each() {{ for (int i = 0; i < 3; i++) {{ std::thread t([]() {{}}); t.detach(); }} }}
	int {g["grows_with_state"]}_sum() {{ int total = 0; for (int v : items) total += v; return total; }}
	int {g["nested_loops"]}_pairs() {{ int total = 0; for (int a : items) for (int b : items) total += a * b; return total; }}
	int {g["redone_work"]}_sum() {{ int total = 0; for (int v : {g["redone_work"]}_table) total -= v; return total; }}
	void {g["allocation"]}_make() {{ for (int i = 0; i < 3; i++) {{ std::vector<int> scratch; scratch.push_back(i); }} }}
	int {g["lock_held_work"]}_clasp() {{ int total = 0; std::lock_guard<std::mutex> hold({g["lock_held_work"]}_gate); for (int v : items) total ^= v; return total; }}
	int {g["boundary_in_loop"]}_ask() {{ int total = 0; for (int i = 0; i < 3; i++) total += int(std::to_string(i).size()); return total; }}
	int {g["copy"]}_grab() {{ std::vector<int> mine = items; return int(mine.size()); }}
	int {g["linear_search"]}_scan() {{ int total = 0; for (int i = 0; i < 3; i++) total += int(std::count(items.begin(), items.end(), i)); return total; }}
	void {g["streaming"]}_start() {{ submit([this]() {{ {f1}_value = 7; }}); }}
	int {g["costly_option"]}_pay() {{ int total = 0; if ({g["costly_option"]}_mode) {{ for (int v : items) total += v; }} return total; }}
	void {g["boundary_conversion"]}_say() {{ std::string text = "x"; {c1}_grab("y"); (void)text; }}
	void _process(double delta) override {{
		(void)delta;
		{g["grows_with_state"]}_sum();
		{g["nested_loops"]}_pairs();
		{g["redone_work"]}_sum();
		{g["allocation"]}_make();
		{g["lock_contention"]}_tick();
		{g["lock_held_work"]}_clasp();
		{g["boundary_in_loop"]}_ask();
		{g["copy"]}_grab();
		{g["linear_search"]}_scan();
		{g["streaming"]}_start();
		{g["costly_option"]}_pay();
		{g["boundary_conversion"]}_say();
		{g["false_sharing"]}_b = 1;
		(void){vr}_MACRO(1);
		(void){t1}_twice(2);
		(void)({o1}_num{{1}} + {o1}_num{{2}});
		{r1}_guard grip_{r1};
		{st}_object o;
		o.call("{st}_answer");
	}}
}};

}} // namespace injected_{d1}
"""
