# Contract: the camera's solids (CityCore queries, native/src/api/city_core_sight.cpp)

Each under a shared hold of the map; each reads the buildings whose lot meets the box asked (`BuildingRecords::meeting`), their
volumes stood on the lot's floor with their own plan and roof. `room` is a Vector3 the rig passes from camera.json:
(`eye_wall_reach_m`, `eye_roof_clearance_m`, `eye_step_back_m`).

| Query | Arguments | Answer |
|---|---|---|
| `eye_free` | focus, eye, lift, room | [focus, eye] by the reference's rules (research R1), at once: eye and point both inside walls: both over the point's roof plus `lift`; eye inside, point not: in along the line to where it first leaves; still inside: over the floor; out of the walls but inside the margin: back along the line, up to `room.z`, to the margin's edge |
| `floor_at` | x, z, room | the lowest the eye may stand (data-model: floor at); -inf on open ground |
| `clear_reach` | from, to, samples | the share of the line out of the walls |
| `sight_pull` | focus, eye, samples, soft | the pull a line through a building's margin asks, 0..1 |
| `highest_roof` | x, z, radius | {hit, position}: the highest top within radius |

The rig (`CityCamera`, the camera's one writer) asks them through `CameraQueries` (one answer per arguments per map revision): it
applies `eye_free` at once to the eye it places, eases only the pull (camera.json `pull_*`, Cinemachine's four times), and shortens
the near plane to `max(near_min, min(distance * near_per_m, (gap + 1 m) * near_gap_share))`, `gap` the eye's height over `floor_at`.
