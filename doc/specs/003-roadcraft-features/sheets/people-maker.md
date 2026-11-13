# people: the person maker

- State: read (2026-11-13)
- Inventory rows: r124 `hint.person` (src/ui/i18n/en.ts:130); r2060 `TOOLS/person`, r2076 `Tool.person` (src/editor/toolMachine.ts:20-23);
  r2227 `people/spec` (src/people/spec.ts); r2125 `lab/catalog/people` (src/lab/catalog/people.ts) | this game: none

## Reference (read for behaviour; nothing copied)
- src/editor/toolMachine.ts 1-85 of 85: `person` is one of the tools the machine can hold, outside the Roads group.
- src/people/spec.ts 1-80 of 80 (read for people-cast): a saved person (id, name up to 40 letters, mood 0..1, a body of the cast), old
  saves' ids and the old creator's sliders and garments mapped to the nearest body of the cast.
- src/world/doc.ts 350-401 of 3,943 (the people's part of the document: `people`, `savePerson`, `removePerson`, `nextPersonId`,
  `peopleRevision`) and the lines Grep found for its save and load (2255, 2574, 2676-2680, 2849, 2902, 2926-2927, 3127-3131): the
  saved people go with the city, its undo and its tiles' world part.
- tests/people/spec.spec.ts 1-82 of 82: save and load, range checks, old ids, old sliders, undo, the cast's spread.
- src/lab/catalog/people.ts 1-50 of 50: the lab's sheet of the sixteen cooked bodies: the lab area's (T025).
- What the player has: nothing. A Grep of the whole tree for `savePerson(`, `removePerson(`, `peopleRevision` and the tool's name finds
  no caller outside doc.ts and the tests, no button in the interface's data, no panel; the simulation never reads `doc.people`. The
  hint (en.ts:130) promises "drag the figure to turn it, wheel to look closer, Save puts them in the city": the panel it describes
  was removed (spec.spec.ts:54 names "the old Person Creator sliders and garments"); what is left is a record nobody writes.

## This game
- No person maker, no saved people (Grep of project/ and native/src for the feature: none).

## Differences
- The reference's maker is a remnant (a tool slot, a hint, a saved list with no writer and no reader): a half-done feature, which the
  orders keep out (CLAUDE.md, "Never"). Not brought; nothing to register here.
- The genre: city builders draw their people from a generated cast (Cities: Skylines II's citizens, genre-gaps.md) and name them; a
  maker of individuals belongs to life simulations. What the player meets of a person here is the cast (people-cast) and the
  inspector of a walker (interface area, T024).

## Design here
- None for the maker. The lab's sheet of the cooked bodies goes with the lab area (T025).

## Proof
- The Grep above, repeated at T007's close: the reference has no writer or reader of `doc.people` outside doc.ts and its tests.
