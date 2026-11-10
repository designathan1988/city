// Fixture core: what building a road does.
#include "core.h"

using namespace godot;

namespace fx {

namespace {
int helper() { return 1; }
} // namespace

void Core::_bind_methods() {
	ClassDB::bind_method(D_METHOD("build_road", "p", "lift"), &Core::build_road, DEFVAL(0.0));
	ADD_SIGNAL(MethodInfo("built"));
}

Dictionary Core::build_road(float p, float lift) {
	auto &l = state.nodes[0].segs;
	l.push_back(1);
	state.log.road_chunks.push_back(2);
	for (auto &[id, n] : state.by_id) n.touch();
	std::unique_ptr<Base> b = std::make_unique<Derived>();
	b->run();
	settle(helper());
	if (p > 0) {
		state.revision++;
	}
	emit_signal("built");
	return Dictionary();
}

void Core::settle(int a) { settle(a, 0); }
void Core::settle(int a, int b) {}
void Node2::touch() {}
void Base::run() {}
void Derived::run() {}

int reader(const State &s) { return s.revision + int(s.log.road_chunks.size()); }

} // namespace fx

namespace fx2 {
struct Arg {};
int adl_target(Arg a) { return 0; }
} // namespace fx2

namespace fx {
auto deduced() { return Node2(); }
int callers() {
	deduced().touch();
	return adl_target(fx2::Arg{});
}
} // namespace fx

namespace fx {
struct Vec {
	Vec();
	~Vec();
	Vec operator+(const Vec &o) const;
	int *begin();
	int *end();
};
struct Holder : public Base {
	Vec v;
	Holder();
};
Vec::Vec() {}
Vec::~Vec() {}
Vec Vec::operator+(const Vec &o) const { return *this; }
int *Vec::begin() { return nullptr; }
int *Vec::end() { return nullptr; }
Holder::Holder() {}
#define TWICE(x) ((x) * 2)
int implicit_calls() {
	Vec a;
	Vec c = a + a;
	for (int i : a) {}
	return TWICE(1);
}
} // namespace fx
