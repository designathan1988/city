// Fixture core: a bound class, a class template with operator[], a virtual hierarchy, overloads.
#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

namespace fx {

template <typename T> class Table {
public:
	T &operator[](int i);
	std::vector<T> items;
};

struct Node2 {
	std::vector<int> segs;
	void touch();
};

struct Log {
	std::vector<int> road_chunks;
};

struct State {
	Table<Node2> nodes;
	Log log;
	std::unordered_map<int, Node2> by_id;
	int revision = 0;
};

class Base {
public:
	virtual void run();
};

class Derived : public Base {
public:
	void run() override;
};

class Core : public godot::Object {
	GDCLASS(Core, Object)

protected:
	static void _bind_methods();

public:
	godot::Dictionary build_road(float p, float lift);
	void settle(int a);
	void settle(int a, int b);
	State state;
};

} // namespace fx
