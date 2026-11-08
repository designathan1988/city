// Another producer on the fog's lane.
#include "core/job_pool.h"
namespace city {
void other(JobPool &p, long k) { p.submit(JobKey{ JobLane::FOG, k }, 0.f, 0); }
}
