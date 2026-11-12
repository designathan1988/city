# Contract: the run report

Every development run of the game (an engine-free scenario, an engine test, a capture, a measurement, the live instance) writes one
report when it stops: `dist/runs/<task>/<run id>/report.json`, read by `atlas problems import`. Written in under 10 s after the run
stops.

```json
{
  "run": "<id>", "commit": "<sha>", "build": "debug|profile", "profile": "median|floor|none", "tier": "low|medium|high|ultra",
  "scenario": {"map": 0, "seed": 1, "commands": "path or inline", "camera": "path or inline", "ticks": 1200},
  "state_hash": "<hex>",
  "violations": [
    {"rule": "B1", "owner": "sim/traffic/...:symbol", "tick": 812, "place": [x, z],
     "what": "vehicle 41 and vehicle 77 overlap by 0.6 m", "evidence": "dist/evidence/<problem id>/..."}
  ],
  "budgets": [{"metric": "frame_p90_ms", "limit": 16.7, "measured": 18.2, "over": 41, "worst": {"frame": 3021, "zones": ["..."]}}],
  "needless": [{"kind": "road_chunks", "consumer": "render/roads", "units": 410, "identical": 388}],
  "writes_outside_declaration": [{"part": "lots", "by": "symbol", "scope": "COMMAND(roads)"}],
  "verdict": "pass|fail"
}
```

A run fails when it has a violation, a budget over its limit, a write outside a declaration, or needless work above the bound the
change kind declares. The orchestrator streams the counts every 5 s while the run goes on.
