// Block scope, a local class, direct-initialization and destruction, by the C++ rules.
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace fx {

struct Job {
	std::function<void()> work;
};

struct Pool {
	std::unordered_map<int, Job> slots;
	std::unordered_map<int, int> newest;
	void submit(int key, std::function<void()> work) { slots[key].work = work; }
	void loop(int key) {
		std::function<void()> work;
		{
			auto it = slots.find(key);
			work = std::move(it->second.work);
		}
		work();
		{
			auto it = newest.find(key);
			it->second = 0;
		}
	}
	void each(int n, const std::function<void(int)> &fn) {
		struct Batch {
			const std::function<void(int)> *fn = nullptr;
		};
		auto batch = std::make_shared<Batch>();
		batch->fn = &fn;
		auto helper = [batch]() { (*batch->fn)(0); };
		helper();
	}
	~Pool() {}
};

void compute() {}
void visit(int) {}

struct Facing {
	explicit Facing(const std::function<void()> &f) : call(f) {}
	void run() { call(); }
	std::function<void()> call;
};

void place(const std::function<void()> &facing) {
	Facing f(facing);
	f.run();
}

struct Part {
	~Part() {}
};

struct Base {
	virtual ~Base() {}
};

struct Owner : Base {
	std::unique_ptr<Pool> pool;
	std::vector<Part> parts;
	~Owner() {}
};

struct Holder {
	Part part;
};

void use() {
	Pool p;
	p.submit(1, [] { compute(); });
	p.loop(1);
	p.each(1, [](int i) { visit(i); });
	place([] { compute(); });
	Holder h;
}

// Binary `*` is multiplication: the operator* of two operands, whatever it returns (O2).
struct W {
	void go() {}
};

struct V {
	W operator*(const V &o) const { return W(); }
};

void scale(V a, V b) { (a * b).go(); }

// An array is an aggregate: each element is initialized from its own clause ([dcl.init.aggr]); a pointer constructs
// nothing.
struct P {
	float x = 0;
	P() = default;
	P(float px, float py) : x(px) {}
};

P make_p() { return P(1, 2); }

void corners() {
	const P corner[] = { make_p(), make_p() };
	P pair[2] = { { 1, 2 } };
	P *ptrs[2] = { nullptr, nullptr };
}

// `T x(a, b);` declares an object when a or b is a value, else a function ([dcl.ambig.res]).
const float SIDE = 2;
const P CORNER(SIDE, SIDE);
P maker(P);

void builds() {
	P q(SIDE, SIDE);
}

void only_declares() {
	P made(P);
}

void moves(std::function<void()> fn) {
	Facing taken(std::move(fn));
	taken.run();
}

}  // namespace fx
