// A call on an expression of a class of the program runs that class's method in C++ (its static type, ISO C++
// [expr.ref]), whatever engine values the tracer's receiver also holds (a parameter of a function the engine calls):
// a lambda passed to it is the method's to call, never handed to the engine (log/trace-rules.md, J6).
struct Pool {
	template <typename F>
	void each(F &&fn) { fn(1); }
};

struct Watcher {
	void _notification(Pool *what) {
		what->each([&](int k) {});
	}
};
