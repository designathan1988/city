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

}  // namespace fx
