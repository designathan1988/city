# Contract: look plan steps that aim at an element

A look plan (project/tests/visual/look.gd) aims each photo at the element an item changed, found by the core, never by guessed
coordinates. Existing: `setView [x, y, zoom, azimuth, elevation]` (the ground under a point), `walker [zoom, azimuth, elevation]`.
Added by this feature:

| Step | Arguments | Aims at | Found by |
|---|---|---|---|
| `setViewRoof` | x, y, radius, zoom, azimuth, elevation | the highest roof within radius (held with `CityCamera.pin`) | `CityCore::highest_roof` (done) |
| `setViewNear` | kind, x, y, zoom, azimuth, elevation | the nearest element of `kind` (pole, vehicle, walker, building, sign) to the point | the core's existing probes (`shot_targets`, `street_watch`, `probe_building`) |

Every aim prints what it found (kind and position); finding none is a FAIL of the plan. Each shot waits
`{"until": "render_pending", "equals": 0}` (CLAUDE.md image protocol).
