# Buildings: the area's index (specification 003, T022, T023)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present: building kinds and massing with the 12 styles; facade patterns and bays with balconies; the 6 roof kinds, 7 roof gear
  kinds, roof plant; 14 flag patterns; windows lit at night (the same Kelvin values) and the room behind the glass; interiors with
  the same 22 furniture pieces; block booleans; pushed faces, offset, bevel; primitives; the 3D reference model; the player's own
  models; near and far detail levels; facade and balcony props; ruins breaking apart; ground graded to the lot; the camera kept
  above roofs; all 38 landmark kinds.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | Drawing a building from a shape (rect, L, U, circle, hexagon, octagon, chamfered, cross, stepped, sketch) as a new building, a wing, a block on top or a cut | PARTIAL, high: the cards do nothing | the cards send `do:"shape"`, no handler takes it, `tool.failed` answers; the core already makes the shapes; no sketch, no "draw where" | builder.json:65, building_tool.gd:186-222, build_hands.gd:56-75, body_edits.gd:52, body_plan.cpp:1; catalog.ts:143-169, buildingTool.ts:124,2001 |
| 2 | The numeric inspector (floors, storey and ground heights, roof pitch, relief, bays, window size, sill, piers, block size, offset, base, lift, function) | ABSENT, high | only the floor count, read-only; body_heights and body_set_face exist with no caller in the UI | inspector_panel.gd:72-78, city_core_body.cpp:174; buildingsWiring.ts:779-872 |
| 3 | Buildings that age and are renovated | ABSENT, medium | the build time stored and the decay values loaded, read by nothing; weathering a fixed noise; the renovate texts unused | building_store.h:43, catalog.h:76, building_common.gdshaderinc:40-46, strings.csv:171; types.ts:454-456, decayWatch.ts:9-25, main.ts:1429-1434 |
| 4 | Roof shape: pitch, ridge, the side a shed falls to | PARTIAL, medium | op_roof_shape called only by a test | body_finish.cpp:70; shell.ts:1760-1766 |
| 5 | Copy, paste, duplicate, mirror, group, delete the selected building | ABSENT, medium | only "keep as my model" | model_shelf.gd:36; buildingsWiring.ts:570-620,1100-1103 |
| 6 | Grown buildings stepping down a slope, the garage under a house | PARTIAL, medium on hills | growth never sets a lift; grown buildings sit on a graded lot | b40/types.h:134, lot_shape.h:1-8; splitLevel.ts:100,286 |
| 7 | Paint presets, swatches, custom colour, trim | PARTIAL, medium | one fixed colour a finish, no presets, no trim card (the core has a trim scope) | builder.json:318-327, types.h:31; materials.ts:259-296 |
| 8 | Facade zone scope; patterns following the scope | PARTIAL, low | no zone card; a pattern covers the whole block | builder.json:169-174, body_edits.gd:47-48; catalog.ts:172 |
| 9 | Snap modes, hide others | PARTIAL, low | Alt only turns grid snapping off; no hide others | handle_drag.gd:178; workspace.ts:108, fade.ts |
| 10 | Landmarks sized to the building; party walls between neighbours | PARTIAL, low | one fixed mesh a kind, unscaled; hidden bays checked against the building's own blocks only | signature_bodies.h:1-3, building_renderer_build.cpp:168, placed.cpp:38-44; signature.ts:1755-1766, partyWalls.ts:11 |

## Translated from the reference (behaviour and data, not its code structure)

b40/types.h (its enums), b40/blocks.h:1, b40/signature_kinds.h:1, likeness.h:1, body_plan.cpp:1, body_blocks.cpp:1,
body_model.cpp:1, ruin_fracture.h:1, flag_cloth.cpp:1, interior_cut.cpp:3, roof_gear.cpp:19, lot_props.cpp:17,
building_parts.gdshader:8-56, builder.json (tab and tool ids), bay_props.json. The copy scan also lists roof_outline.cpp and
roof_pitched.cpp (93 % shared passages), pane_rooms.cpp, shell.cpp, plan_area.cpp, interior/plan.cpp (76 %).

## Reference faults never brought

The mesh built in JS arrays, an object per placement or vertex (buildingMesh.ts:339-975); frame bars over every pane (:1102);
window reveals as faces, 16 triangles a window (:1112); the far level laid bay by bay (:1844); bay keys as text (:1916); a whole
job's furniture held at once, 15 MB (:4082); each landmark merged again per building (signature.ts:1777-1790); photographing every
option as a menu opened, 4 fps (photo.ts:3-4); the town emitted again to fade it (fade.ts:1); and from the long audits (architecture.md):
lot surfaces as instanced boxes, facade parts at full model however small, rounded furniture boxes, joined buildings kept whole.
