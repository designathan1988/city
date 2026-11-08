// Pool.
#pragma once
namespace city {
enum class JobLane { FOG, ROADS };
struct JobKey {
	JobLane lane;
	long id;
	JobKey bake() const { return JobKey{ lane, id }; }
};
class JobPool {
public:
	void submit(JobKey k, float p, int w);
};
}
