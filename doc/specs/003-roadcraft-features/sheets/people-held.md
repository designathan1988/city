# people: what people hold

- State: read
- Inventory rows: r2819 `render/people/heldProps`, r2820-r2825 `HELD/*` | this game: native/src/render/agents/held_things.cpp/.h,
  project/render/shaders/people_held.gdshader, project/data/render/agents.json held

## Reference (read for behaviour; nothing copied)
- Read whole (2026-11-13): src/render/people/heldProps.ts 1-178 of 178: a book or magazine for reading, a cup for drinking, a phone, a
  camera, a bag, an umbrella, and a box carried in both arms; what hangs or stands by its weight (cup, bag, umbrella) kept upright at
  the fist; one instanced mesh a thing, its matrices made on the CPU each frame from the hand's (a fault: per-frame CPU matrices).

## This game
- Read whole: native/src/render/agents/held_things.cpp 1-72 of 72, held_things.h 1-38 of 38, project/render/shaders/
  people_held.gdshader 1-33 of 33: one mesh a thing from agents.json, posed on the GPU by the hand's bone of the same clip at the same
  phase (nothing worked out on the CPU), upright things round the fist, a thing growing out of the fist as the act comes in. Things:
  phone, camera (photo), bag.

## Differences
- Missing here: the book (read), the cup (drink), the umbrella (rain), the box carried in both arms; each comes with its activity
  (people-motions' design, item 3): the umbrella with the rain (the weather's own state), the box with a carrying walk.
- Here better: the hold on the GPU, no matrices per frame.

## Design here
- With each activity of people-motions item 3, its thing in agents.json held.things (boxes in the palm's frame, colours, upright),
  the box held in both hands by a second bone (the other hand's) in people_held.gdshader. Owner: native/src/render/agents/
  held_things.cpp. Part of the gait-and-activities item (registered: walkers have one gait and act only at the kerb).

## Proof
- Photos of a reader, a drinker, an umbrella in the rain and a box carried (setViewNear walker), near, day and rain.
