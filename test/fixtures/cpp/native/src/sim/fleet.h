// Fixture: a tiny C++ subsystem with every pattern the C++ graph must see.
#pragma once

#include <vector>

namespace city {

constexpr float SPEED_LIMIT = 13.9f;

int g_counter = 0;

struct Car {
	float s = 0, v = 0;
};

class Mover {
public:
	virtual ~Mover() = default;
	virtual float step(Car &c, float dt) = 0;
};

class Cruise : public Mover {
public:
	float step(Car &c, float dt) override;
};

struct Fleet {
	std::vector<Car> cars;
	std::vector<int> queue;
	float total = 0;
	void tick(Mover &m, float dt);
	float drain();
	float probe();
};

float clampf(float v, float lo, float hi);

// What the engine calls every frame.
struct Driver {
	Fleet fleet;
	Cruise cruise;
	void _process(double delta);
};

} // namespace city
