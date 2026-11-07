// Renders.
#include "core/job_pool.h"
namespace city {
void go(JobPool &workers, long k) {
	workers.submit(JobKey{ JobLane::FOG, k }, 0.f, 0);
	workers.submit({ JobLane::FOG, k }, 0.f, 0);
	JobKey made{ JobLane::FOG, k };
}
}
