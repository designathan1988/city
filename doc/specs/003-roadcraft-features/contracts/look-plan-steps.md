# Contract: look plan steps that aim at an element

A look plan (project/tests/visual/look.gd) aims each photo at the element an item changed, found by the core, never by guessed
coordinates. Existing: `setView [x, y, zoom, azimuth, elevation]` (the ground under a point), `walker [zoom, azimuth, elevation]`.
Added by this feature:

| Step | Arguments | Aims at | Found by |
|---|---|---|---|
| `setViewRoof` | x, y, radius, zoom, azimuth, elevation | the highest roof within radius (held with `CityCamera.pin`) | `CityCore::highest_roof` (done) |
| `setViewNear` | kind, x, y, zoom, azimuth, elevation | the nearest walker or vehicle; for `lamp`, `pole`, `sign`, the nearest of that kind on the first street, from the vehicles nearest the point outwards, that has one; for `building`, the tallest within 60 m, at its foot | `shot_targets` (the clock stopped), `segment_at` and `furniture_spots` (`FurnitureKind`), `street_poles`, `highest_roof` |

A plan runs the clock before aiming at traffic or street furniture (`{"dispatch": "set_speed", "args": [1]}`, frames).

Every aim prints what it found (kind and position); finding none is a FAIL of the plan. Each shot waits
`{"until": "render_pending", "equals": 0}` (CLAUDE.md image protocol).
