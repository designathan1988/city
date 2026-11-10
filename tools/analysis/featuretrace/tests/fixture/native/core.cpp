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
