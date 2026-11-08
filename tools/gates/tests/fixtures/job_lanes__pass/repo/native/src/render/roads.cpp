// Roads.
#include "core/job_pool.h"
namespace city {
void roads(JobPool &p, long k) { p.submit(JobKey{ JobLane::ROADS, k }, 0.f, 0); }
}
