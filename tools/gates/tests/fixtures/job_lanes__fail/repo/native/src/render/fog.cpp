// Fog.
#include "core/job_pool.h"
namespace city {
void fog(JobPool &p, long k) { p.submit(JobKey{ JobLane::FOG, k }, 0.f, 0); }
}
