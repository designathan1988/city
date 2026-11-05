// The code of src/lab/tests/benchmarks.ts that is the feature's (the rest of the file is shared).

// src/lab/tests/benchmarks.ts:118-155 slice of src/lab/tests/benchmarks.ts#stress (shared): the feature's statements; "…" marks code left out
async function stress(ctx: LabContext, options: BenchmarkOptions, cancel: Cancel, progress: (l: string) => void, rows: BenchRow[]): Promise<void> {
  // … 1 lines
  const was = { speed: sim.clock.speed, paused: sim.clock.paused, traffic: sim.trafficIntensity, people: sim.pedestrianIntensity };
  // … 5 lines
  try {
    for (const c of cases) {
      // … 2 lines
      sim.clock.speed = c.speed;
      sim.trafficIntensity = c.intensity;
      sim.pedestrianIntensity = c.intensity === was.traffic ? was.people : c.intensity;
    // … 14 lines
    }
  } finally {
    sim.clock.speed = was.speed;
    sim.trafficIntensity = was.traffic;
    sim.pedestrianIntensity = was.people;
    // … 1 lines
    sim.clock.paused = was.paused;
  }
}
