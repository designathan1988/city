# Contract: the camera's solids (CityCore queries, native/src/api/city_core_sight.cpp)

Each under a shared hold of the map; each reads the buildings whose lot meets the box asked (`BuildingRecords::meeting`), their
volumes stood on the lot's floor with their own plan and roof.

| Query | Arguments | Answer |
|---|---|---|
| `eye_free` | focus, eye, skin | [focus, eye] by the reference's rules (research R1): both inside walls: both over the roof plus the eye's lift; eye inside, point not: in along the line to where it leaves, `skin` further; eye in the margin: back along the line up to `eye_step_back_m` out of it; still inside: over the floor |
| `floor_at` | x, z | the lowest the eye may stand (data-model: floor at); -inf on open ground |
| `clear_reach` | from, to, samples | the share of the line out of the walls |
| `sight_pull` | focus, eye, samples, soft | the pull a line through a building's margin asks, 0..1 |
| `highest_roof` | x, z, radius | {hit, position}: the highest top within radius |

The rig (`CityCamera`, the camera's one writer) asks them through `CameraQueries` (one answer per arguments per map revision), applies
the corrections at once, eases only the pull by the four times, and shortens the near plane by the gap.
