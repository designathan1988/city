// Renders.
#include "core/job_pool.h"
namespace city {
// JobKey{ JobLane::FOG, 1 } is never written by hand
void go(JobPool &workers, JobKey job) { workers.submit(job, 0.f, 0); }
}
