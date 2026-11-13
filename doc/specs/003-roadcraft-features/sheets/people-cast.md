# people: the cast (who the town's people are)

- State: read
- Inventory rows: r2211-r2226 `cast/*` (src/people/cast.json) | this game: project/assets/people/people.bin (its variations),
  native/src/render/agents/people_asset.h, people_renderer.cpp, native/src/sim/traffic/ped_spawn.cpp

## Reference (read for behaviour; nothing copied)
- Read whole (2026-11-13): src/people/cast.json 1-130 of 130 (sixteen bodies: female and male; adult, young, senior, child; casual,
  sport casual, smart casual, business; whether the body rides a bicycle or a motorcycle; a resting mood); src/people/spec.ts 1-80 of 80
  (a person as saves know them: id, name, mood, body; older saves' body ids and Person Creator sliders mapped to the nearest body by
  sex, age band (children 6-12, young 17-28, adults 30-58, seniors 66-84) and wardrobe).

## This game
- Read whole: native/src/render/agents/people_asset.h 1-78 of 78 (the variations of people.bin, MakeHuman CC0 on the Rocketbox MIT
  skeleton: female, senior, child, mood, height, the walk, the palm, the helmet shell, the ragdoll's joints); native/src/sim/traffic/
  ped_spawn.cpp 1-221 of 221 (an age by its share, a pace by age, a look hash that picks the body of that age, a child never alone);
  people_renderer.cpp (read for people-motions): bodies by age (130-138), a rider any adult body (247-252).
- The same sixteen bodies (commit 5e6bd2c3, "the cast of the 4.0"), their moods kept (people_renderer.cpp:66-68).

## Differences
- The reference seats on a bicycle or a motorcycle only the bodies that ride (`rides`: sport and smart casual adults, the young man,
  cast.json); this game seats any adult body (people_renderer.cpp:251): a businesswoman on a motorcycle. Registered.
- The reference's young band names bodies and maps old saves (spec.ts:40-41); it changes no behaviour there, and this game's ages are
  by pace (child, adult, elder): justified, not brought.
- The reference's person record (name, mood, body) for saves and the person maker is people-maker's.

## Design here
- The cast as data: each body's sex, age, wardrobe and whether it rides, in a table of the people asset (project/data/render/agents.json
  people.cast, schema in tools/gates/schemas), read by DataTables; riders drawn among the bodies that ride (people_renderer.cpp seat).
  Owner: native/src/render/agents/people_renderer.cpp. One item of T008.

## Proof
- A scene seating riders a thousand times: none on a body that does not ride; a photo of riders in traffic (setViewNear vehicle).
