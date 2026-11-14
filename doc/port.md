# The port's board: every functionality, in order

The one list of what the port has to do and where it stands. A functionality is a whole system the player uses (the complete
traffic, the complete roads), never one of its rows and never a button. **The next is the first `- [ ]` line**; the lines
under it are its cases, all done in one block: traced together, read, coded whole, then built, tested and photographed once.
A line is checked (`- [x]`) in the commit that passes its checks. `python .claude/hooks/port_state.py` prints the next.

The cases come from the area's sheet (`doc/specs/003-roadcraft-features/sheets/<area>.md`: what differs from the reference,
with `file:line` on both sides), which the port tool reads: `python tools/tracer/port.py sheet <area>`. How it is done:
`CLAUDE.md`, "One functionality, start to end". An order of the owner in the conversation goes before this list.

## 0. The backbone the port lands in (doc/specs/003-roadcraft-features/architecture.md)

- [x] T036 errors seen: an engine ERROR or WARNING fails a test; every refusal told in one place; the city kept on a failed save (02beaa45)
- [x] T037 records kept true: record invariants after every command and at every test's end; a removal reaches every table (90b67e27)

## 1. Roads (sheets/roads.md; work so far: doc/work/t015-roads.md)

- [ ] roads, complete: every case below (`python tools/tracer/port.py sheet roads`; the note: doc/work/roads.md). The first block closed 4e38bad2 with parts left out; by the owner's addendum of 2026-11-14 (nothing is left out for cost) the line is unchecked until each is in the game, in a block of its own: the carriageway's surface in the profile editor (a versioned save) and drawn; the fill, cutting, piers and buried stretch drawn over the draft; the paint of a width transition and of a mini-roundabout's island; the stop word and the triangle on automatic priorities; the start over a raised road (the better rule, decided by the code); one owner of the arc conversion with its test (Proads-35); a near photo of the legends (Proads-38)
  - the heights of the roads, solved in this game's design (done: road_grade_test with its seam cases passes)
  - laying a road, what it will be and cost (done: the draft's price is the build's, its stretches named, the limited end told; the fill and cut are named in words, not painted along the draft)
  - the network's links and bans kept through edits (T037, 90b67e27) (done)
  - markings (done in code: turn arrows, the stop word, the give-way triangle, the bus word, far lines by kind and colour; no near photo shows a legend yet: Proads-38; the taper's and the mini-roundabout's paint not brought: Proads-6)
  - the profile editor (done: class, paving, centre line, each footway its own, Save's problems, a saved road deleted; the carriageway's surface is drawn by no renderer: Proads-37)
  - streets and their life (done: traffic from the map's edge, street names and house numbers in the inspector, the basic furniture set)
  - lanes that obey the road (done: bus lanes, solid lines, per-lane turns read by the traffic)

## 2. Junctions and signals (sheets/junctions.md)

- [ ] junctions, complete: every case below, in one block (`python tools/tracer/port.py sheet junctions`)
  - gap acceptance by turn
  - signal stages from conflicts, protected turns
  - demand-driven signals
  - walkers' calls at signals
  - bus priority at signals
  - drivers give way to people waiting at a zebra
  - ring metering and the zipper merge
  - mini-roundabout and "none" control priority
  - spillback and deadlock
  - the junction panel
  - turn classes, walkers-only stages, replanning after an edit

## 3. Traffic (sheets/traffic.md)

- [x] traffic, complete: every case below, in one block (`python tools/tracer/port.py sheet traffic`)
  - the tracer's empty `--cost` on `city.traffic.step` (tools/tracer/tracer/cost.py), needed by this area's sheets (done: `python tools/tracer/trace.py --cost city.traffic.step --root native/src/sim/traffic` prints the loops; the tracer's 20 test files pass)
  - slowing for bends (done; the note: doc/work/traffic.md)
  - lane changes (politeness, indicators, positioning) (done)
  - cars leave only out of sight (done)
  - each driver different (done)
  - following that anticipates (done)
  - the mix of vehicles (done)
  - lanes in the route (done)
  - far traffic as flows; an adaptive detailed window (done: flows_test, flow_control_test, records_traffic_test, driving_test, quality_test, e2e and the measure pass; the far cars seen in the photos of the plan far_cars, by day and by night, on a layer of their own)
  - re-routing by patience (done)

## 4. Zones, lots and growth (sheets/zones-lots-growth.md)

- [ ] zones-lots-growth, complete: every case below, in one block (`python tools/tracer/port.py sheet zones-lots-growth`)
  - growth's order and pace
  - the 24 km city opens fast (research.md R4: a candidate's look from its massing; `fill_all` in a 2 x 2 checkerboard of 256 m cells over the job pool, the same city for any pool size; `settle_sites` in parallel); `window_traffic_test` under 60 s with the same check
  - rezoning keeps the building
  - lots cut to the land and kept
  - buildings that age into decay
  - a polygon lot's building fits its outline; set-backs and lower densities tried; eras by quarter
  - industrial densities; the generator's parks

## 5. Transit (sheets/transit.md)

- [ ] transit, complete: every case below, in one block (`python tools/tracer/port.py sheet transit`)
  - random city buses stop to board
  - the transit tools' help
  - picking and previewing without whole-plan walks
  - trains redesigned off the reference's costs
  - buses leave only out of sight
  - a save's lines clamped on load; trains kept through an edit only when their track is the same
  - transit vehicles culled by the view
  - bus lanes (with roads 7)

## 6. Buildings (sheets/buildings.md), with the people's and the landmarks' own cook (Pbuildings-43)

- [ ] buildings, complete: every case below, in one block (`python tools/tracer/port.py sheet buildings`)
  - drawing a building from a shape, as a new building, a wing, a block on top or a cut
  - the numeric inspector
  - buildings that age and are renovated
  - roof shape (pitch, ridge, the side a shed falls to)
  - copy, paste, duplicate, mirror, group, delete the selected building
  - grown buildings stepping down a slope, the garage under a house
  - paint presets, swatches, custom colour, trim
  - facade zone scope; patterns following the scope
  - snap modes, hide others
  - landmarks sized to the building; party walls between neighbours

## 7. People (sheets/people.md and sheets/people-*.md; doc/history/people.md)

- [ ] people, complete: every case below, in one block (`python tools/tracer/port.py sheet people`)
  - where people walk (sheets/people-walking.md) (done)
  - casualties and the body that falls (sheets/people-casualties.md) (done)
  - what people do, the motions and activities (sheets/people-motions.md; needs the cook of buildings, Pbuildings-43)
  - the cast (sheets/people-cast.md)
  - what people hold (sheets/people-held.md)
  - faces and their expressions (sheets/people-faces.md)
  - how people are drawn, the body and its detail levels (sheets/people-drawing.md)
  - the person maker (justified in sheets/people-maker.md: a half-done remnant of the reference, not brought) (not brought)

## 8. Terrain (sheets/terrain.md)

- [ ] terrain, complete: every case below, in one block (`python tools/tracer/port.py sheet terrain`)
  - the relief brush's feel
  - rivers that follow the land
  - the ground's colour from the ecosystem
  - natural relief at map start
  - the ecosystem reads all the land
  - water flows
  - landforms with their outline
  - ground paint blends
  - the ground beside a road at the footway's top

## 9. Street furniture (sheets/street-furniture.md)

- [ ] street-furniture, complete: every case below, in one block (`python tools/tracer/port.py sheet street-furniture`)
  - the basic set
  - lamps' beams at night
  - street trees that grow, and down planted medians
  - detail levels by size on screen
  - rows placed by hand along the curve
  - speed plates by the marking style, none on highways
  - signal heads' sway and halos

## 10. Effects and sky (sheets/effects-sky.md)

- [ ] effects-sky, complete: every case below, in one block (`python tools/tracer/port.py sheet effects-sky`)
  - vehicle exhaust and road dust
  - lamp beams in rain at night
  - canyon light through the day
  - cloud shadows by the light
  - poles and wires in a blast; a signal post knocked out
  - the bulldozer's reach
  - smoke with the weather's wind
  - small differences

## 11. Saves and interface (sheets/saves-interface.md)

- [ ] saves-interface, complete: every case below, in one block (`python tools/tracer/port.py sheet saves-interface`)
  - the player told when a city cannot be read or a save fails (the failed save is T036's, done; the unreadable city is open)
  - autosave silent
  - the inspector's parking and street numbers
  - a new map undoable, or the old city kept
  - the session with the city
  - the help and small defects
  - this game's text keys stop being the reference's own: LAB_COPY and lab_scan read the text tables (tools/gates/gate_lab.py, lab_scan.py, rules.json), then the keys the reference defines renamed area by area in project/data/i18n/*.csv with every `tr("KEY")` and data `*_key`. Done when: the gate's fixtures prove the new finding; `python tools/gates/lab_scan.py` finds no reference key

## 12. The camera and the rest (sheets/camera-and-rest.md), last

- [ ] camera-and-rest, complete: every case below, in one block (`python tools/tracer/port.py sheet camera-and-rest`)
  - the eye never in a building, no jolt: research.md R1b built by contracts/camera-solids.md (the camera floor as the core's derived tiles, `eye_clear`, the point's rest over the floor by the zoom weight, the pull unchanged, values in camera.json), its sheet first with the proof before code R1b names. Done when: `python tools/tasks/cli.py run camera_feel_test camera_input_test look_view_test` passes, each under 60 s (eye inside walls at most 1 frame in 1,000; no frame's jolt past 1% of the view outside the counted push-outs; push-outs at most 0.1% of frames); the frozen street view (`run photos`, 02-street) shows a street with the eye outside every building
  - the camera kept over the map
  - saves in the folder Steam syncs
  - reset view frames the map; perspective/isometric remembered; the building tool's frame selection and top view
  - the camera's timings
  - problems logged on the player's disk
  - Steam achievements
  - a name per person
  - the lab's developer tabs (justified in the sheet: developer-only, not brought) (not brought)

## 13. After the port: what the genre has and neither game does (doc/specs/003-roadcraft-features/genre-gaps.md)

- [ ] genre: water and sewage networks
- [ ] genre: happiness and well-being
- [ ] genre: land value
- [ ] genre: pollution
- [ ] genre: disasters
- [ ] genre: modding

## Only on the owner's order

Measurements at scale: `python tools/tasks/km24.py` (the 24 km city, two builds compared) and the scale probe that was T038
(one scene on the 24 km map: memory by system, CPU by section of the frame and by tick, GPU time, draws and triangles against
`doc/specification.md`'s budgets; not built). `tools/build/quick.sh --ready`, the whole gates against the register,
`python tools/hunt/hunt.py`.
