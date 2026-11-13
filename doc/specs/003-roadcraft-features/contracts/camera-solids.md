# Contract: the camera among the buildings (research R1b)

Three parts, each continuous in its inputs by construction, so no answer can jump while the view moves.

## The core's queries (native/src/api/city_core_sight.cpp and the camera floor's owner)

Each under a shared hold of the map; each reads only the buildings whose lot meets the box asked (`BuildingRecords::meeting`).

| Query | Arguments | Answer |
|---|---|---|
| `camera_floor` | x, z | the floor at (x, z), bilinear over the floor's 2 m cells: the highest roof top plus `eye_roof_clearance_m` over a cell (the ground where none stands), slope-limited to `floor_slope` by the two-pass sweep; the tiles it needs are built on first ask from CityState and kept in a bounded LRU, dropped when ChangeLog reports a building in them |
| `eye_clear` | point, eye, radius | the eye's body (a sphere of `radius`) pushed out of the buildings it overlaps, along the line toward `point`, to where the sphere overlaps none; the eye itself when it overlaps nothing |
| `sight_pull` | focus, eye, samples, soft | unchanged: the share of the line a building in its way asks, 0..1 |
| `highest_roof` | x, z, radius | unchanged: {hit, position} |

`eye_free` and `eye_floor` go (their at-once rules jump: research R1b).

## The rig (project/game/camera: CityCamera, CameraHeights, CameraSolids, CameraPull)

- **The point's rest** (CameraHeights): the higher of the ground and `camera_floor` at the point, by the pull's zoom weight
  (`pull_full_half` full, `pull_none_half` none), through the ground spring's two stages (`ground_smooth_s`).
- **The eye's body** (CameraSolids): `radius = max(near half diagonal + eye_body_pad_m, eye_body_move_share * the frame's move)`;
  the eye placed at `eye_clear(point, orbit eye, radius)`; the frames it moved the eye by more than a jolt are `snapped` (counted).
- **The line** (CameraPull): unchanged (`pull_wait_s`, `pull_in_s`, `pull_hold_s`, `pull_out_s`).
- **The near plane**: `clamp(distance * near_per_m, near_min, near_max)`, never longer than the eye's clearance from the nearest wall
  minus the pad (the body's radius covers the near plane's corners).

## Values (project/data/game/camera.json, schema tools/gates/schemas/camera.schema.json)

`floor_slope`, `floor_cell_m`, `floor_tile_m`, `floor_tiles_kept`, `eye_body_pad_m`, `eye_body_move_share`; the margin values of
the superseded design (`eye_wall_reach_m`, `eye_step_back_m`, `eye_margin_share`, `eye_correct_s`, `near_gap_*`) go with it.
