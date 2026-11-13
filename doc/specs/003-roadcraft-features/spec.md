# Feature Specification: Bring the reference game's features, correctly and fast

**Feature Branch**: `main` (the project works on main, CLAUDE.md)

**Created**: 2026-11-13

**Status**: Draft

**Input**: The owner, in conversation (Portuguese, faithfully): analyse what must be done in every sense: how to make the camera;
trace the roadcraft's features and build them for this game; follow the architecture; code without delay; never launch slow,
heavy, unwatched, silent processes, or ones that block the next tasks when the previous stalls; have tools that speed the work up
for good; take photos when implementing something, quickly, in seconds; tests in seconds; never trial and error; understand the
project, its rules, what must be done and the right way. Plus: a script that takes the inventory dynamically (never a closed list)
of what exists in the old game and in this one and pairs them; and the open items this session found (the 24 km opening that makes
`window_traffic_test` take 90 s, the frozen street view inside a building, the roofs plan aimed at the streets, the text keys equal
to the reference's, the camera tests that only passed because the sight query was blind).

## User Scenarios & Testing *(mandatory)*

The actors: the **owner**, who orders features and judges the game; the **player**, who uses the game; the **working session**,
which brings each feature in by the method of CLAUDE.md.

### User Story 1 - The camera never enters a building and never jolts (Priority: P1)

The player moves, turns and zooms the camera anywhere in the city, from a few metres over a street to the whole town. The eye
never stands inside a building, the picture never cuts into a wall, the point looked at never leaps onto a roof the player did not
aim at, and the motion has no jolt the eye sees. The behaviour is the reference game's own camera behaviour (traced there: how
its view keeps out of the buildings, when it moves in front of a wall, how it eases), built in this game's design; where the
reference has no answer, the standard games' practice decides (a camera arm that comes in front of a wall and lets go slowly).

**Why this priority**: the camera is how the player sees everything else; a camera inside a wall or jumping is the first thing a
player notices, and this session showed the current one only looked right because it could not see the buildings.

**Independent Test**: the camera's own test scenes walk, turn, drag and zoom the camera over a real town at every zoom and bearing,
and count the frames with the eye in a building, the frames where the picture jolts and where the eye leapt; photos at street
level, mid and far show the view outside the buildings.

**Acceptance Scenarios**:

1. **Given** the town and the camera a few metres over a street, **When** the player walks the view past a row of buildings,
   **Then** the eye is outside every building on every frame and the view has no jolt.
2. **Given** the camera at any zoom, **When** the point looked at passes into a building's lot, **Then** the view keeps looking
   at that point from outside the building and does not leap onto its roof.
3. **Given** a photo plan aimed at the highest roof of the town's centre, **When** it runs, **Then** each near and mid photo shows
   the roofs from above, not the street between the towers.

---

### User Story 2 - Know what each game has and how they pair (Priority: P1)

Before a feature is asked for or brought, whoever works reads, in seconds, what the reference game has and what this game has in
every area (roads, junctions, signals, traffic, people, transit, buildings, lots, growth, zones, terrain, street furniture,
effects, sky, camera, saves, interface, lab), each feature with where it is declared, and the pairing between the two by evidence
(the same text the player reads, the same data id): paired, only in the reference, only here. Then, for one feature, the tracer's
comparison of both games says what it must do, what this game does, and every difference.

**Why this priority**: without it every request starts from guesses; with it the gaps are a list the owner and the session read
alike, and nothing is claimed brought that is not.

**Independent Test**: the inventory command runs on both trees and prints the per-area table and the reports; a sample of its
rows is checked against their source lines; one feature's comparison sheet is produced by the tracer for both games.

**Acceptance Scenarios**:

1. **Given** both trees on the machine, **When** the inventory runs, **Then** it ends in seconds with every feature's source line
   and the pairing per area, and nothing of it is a hand-written list of features.
2. **Given** a feature named by the owner, **When** the session prepares to bring it, **Then** a comparison sheet exists with the
   reference's behaviour (conditions, cases, values), this game's, the differences, and the reference's faults never to bring.

---

### User Story 3 - Each item checked in seconds, nothing slow or unwatched (Priority: P1)

Each item (one problem or one feature) is coded with nothing run, then checked in seconds: the test scenes it reaches and photos
aimed at what it changed; a pass is committed and pushed before the next item. The suites at a block's end each take under a
minute. No process runs silent, unwatched, without a deadline, or so that one stalled task holds others back.

**Why this priority**: the owner's repeated order; the cost of this session's minutes-long runs and of tests that waited behind
one slow holder of a lock.

**Independent Test**: an item's check is timed from its start to its verdict; every task of the suites carries its recorded time;
the orchestrator shows each task's lines as they come and every wait has a deadline.

**Acceptance Scenarios**:

1. **Given** an item that changes one test scene's subject, **When** its check runs, **Then** the scene and its photos give their
   verdict in seconds, the lines shown as they come.
2. **Given** the whole test suite, **When** it runs at a block's end, **Then** no scene takes a minute or more, and no task waits
   behind another that holds a lock it could share.

---

### User Story 4 - Features brought from the reference in this game's architecture (Priority: P2)

For each feature the inventory shows only in the reference, or diverging, the session brings it: traced in both games, read whole,
its rules, cases and values understood, built as this game's own code (the city's one state, changed only by commands, advanced
only by the simulation; the render only reads; values in the data tables; one owner per definition; within the budgets), never a
line, name or key of the reference, never one of its faults (whole-map work, per-frame work on unchanged inputs, allocation in hot
loops, quadratic proximity, a simulation tied to the camera, heavy geometry, half-done features). Areas go in the order the owner
names; with none named, the areas whose gap the inventory shows largest for the player first.

**Why this priority**: it is the purpose of the project's work; it depends on US2 (knowing the gaps) and US3 (the fast loop).

**Independent Test**: each feature brought has its comparison sheet, its test scene and photos proving its cases, and the
inventory afterwards shows it paired.

**Acceptance Scenarios**:

1. **Given** a feature only in the reference, **When** it is brought, **Then** its test scene proves every case of the sheet, its
   photos show it, the copy scan finds nothing of the reference in it, and the inventory pairs it.

---

### User Story 5 - The open items found by this session closed (Priority: P2)

The 24 km city opens fast enough that the scene measuring its traffic window proves its point in under a minute and the opening
meets its budget; the frozen street view shows a street; the text keys of this game stop being the reference's own; the camera
tests check with a sight that sees the buildings.

**Why this priority**: they were found while checking; each is a registered defect the owner ordered closed.

**Independent Test**: each has its own check: the traffic window scene's time, the opening time in the 24 km measurement, the
frozen photo, the copy scan over the text tables, the camera scenes.

**Acceptance Scenarios**:

1. **Given** the 24 km map, **When** a city is generated, **Then** the opening takes no more than its budget and the traffic window
   scene ends in under a minute with the same check.
2. **Given** the text tables, **When** the copy scan reads them, **Then** no key of this game is a key the reference defines.

---

### User Story 6 - One source of truth: no scattered, contradicting or dead documents and tools (Priority: P1)

The owner, in conversation: see what is wrong in CLAUDE.md, the documents and the scattered files, and correct all of it; remove
the tools that do not work or are no longer used; fix the documents that contradict each other. Whoever works reads one set of
orders (CLAUDE.md), one map of the files, one register of problems, one history per area, and finds no document saying otherwise,
no tool that is dead, broken or unused, and no file outside its place.

**Why this priority**: every contradiction costs a session a wrong turn; every dead tool is a trap; the port of the features
(US4) cannot be fast while the orders and the records disagree.

**Independent Test**: a survey of every document and tool lists each contradiction with its two places, each tool with its
callers and whether it runs; after the fix the same survey finds none, the gates pass on the whole tree against the register, and
every tool's own tests pass.

**Acceptance Scenarios**:

1. **Given** the whole repository, **When** the survey reads every document against CLAUDE.md and against each other, **Then** it
   lists no order said two ways, no stale command, path or number, and no document outside doc/ but CLAUDE.md and tool
   configuration.
2. **Given** every tool under tools/, **When** each one's callers and its run are checked, **Then** none is unused, broken or
   duplicated; each one runs under a minute with its lines shown.

---

### User Story 7 - Beyond the reference: what it lacks or does wrong, at Steam quality on a median PC (Priority: P2)

The owner, in conversation: implement also what is missing or wrong in the old game; not everything there is right or complete;
many resources are still missing (simulations, resources); make sure this game has the right architecture, full optimisation,
performance and quality, every feature working right; it will be sold on Steam, so everything is done for the best performance on
median computers.

The player gets the systems a city builder of its class has (the standard games' set: services and utilities, the economy, the
simulations the city's life needs), each working right by a cited norm or published method, and the game holds its budgets on the
median and floor profiles.

**Why this priority**: the reference is a working base, not the finish line; the product is judged against the genre on Steam.

**Independent Test**: a gap list of the genre's systems (from the standard games' documented features) against both games'
inventories; each system brought has its sheet, its scene, its photos and its budget measured; km24 on the median profile within the
budgets of doc/specification.md.

**Acceptance Scenarios**:

1. **Given** a reference feature whose sheet shows a wrong rule or a missing case, **When** it is brought, **Then** this game does
   the right rule (its source cited) and the missing case, and the sheet records the difference from the reference as intended.
2. **Given** a system the genre has and neither game has, **When** it is brought, **Then** it works by its cited method, has its
   scene and photos, and the game still holds its budgets on the median profile.

### Edge Cases

- A feature that exists in both games under different texts: the pairing says it is unpaired by text; the comparison sheet
  (US2) decides by behaviour, and the inventory records the pair the sheet proves.
- A reference feature that is itself one of its faults (a heavy geometry, a whole-map pass): its behaviour is brought, its cost
  never; the sheet names the fault and the design that avoids it.
- A camera at a zoom where the arm must come in by tens of metres in one frame (a wall the look ahead could not see): the eye stays
  outside, the reaction is counted and reported, and making it rarer is tracked, never hidden.
- A check that cannot finish in a minute: it is a defect of its tool, registered and fixed at its cause, never a raised deadline.
- The reference tree missing on the machine: the inventory and the tracer refuse with the reason; nothing is judged against an
  absent reference.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The camera MUST keep its eye outside every building on every frame, at every zoom and bearing, including while the
  point looked at is inside a building's lot.
- **FR-002**: The camera MUST NOT move the point looked at onto a roof the player did not aim at.
- **FR-003**: The camera's behaviour against buildings MUST be the one traced in the reference game, and where the reference has
  none, the standard games' camera arm (in front of a wall at once, letting go slowly, looking ahead to start early).
- **FR-004**: Every motion of the camera that comes from its own smoothing MUST stay within the jolt limit of the camera tests;
  every at-once reaction to a wall the look ahead could not see MUST be counted and reported by those tests.
- **FR-005**: The camera tests MUST judge the eye with a sight that finds every building whose lot meets the eye, not only those
  whose lot centre is near it.
- **FR-006**: The inventory MUST be produced from the two trees each time it runs, by extraction rules, never from a hand-written
  list of features, and MUST give every feature its source file and line.
- **FR-007**: The inventory MUST pair features only by evidence (the same player text in a language both games hold, the same data
  id) and MUST list unpaired features of each game per area, and every place it could not classify.
- **FR-008**: For any feature to bring, the working session MUST produce a comparison sheet from the tracer on both games: outputs,
  conditions, cases, values, state, costs, the differences, and the reference's faults never to bring.
- **FR-009**: Every feature brought MUST be built in this game's architecture and pass the copy scan (no line, name or data key of
  the reference).
- **FR-010**: Each item MUST be checked by the test scenes it reaches and by photos aimed at what it changed, each check taking
  seconds, and a passing item MUST be committed and pushed before the next.
- **FR-011**: No check, tool or suite task MUST take a minute or more; one that does is registered and fixed at its cause.
- **FR-012**: Every process MUST show its lines as they come, have a deadline, and never make an independent task wait or fail
  because another stalled; a task that waits for a lock MUST NOT hold back tasks that can share it.
- **FR-013**: Photo plans MUST be able to aim at the element changed (a building's roof, a pole, a vehicle), not only at the ground
  under a point.
- **FR-014**: The 24 km city MUST open within its budget, and the traffic window scene MUST prove its point in under a minute
  without changing what it proves.
- **FR-015**: This game's text keys MUST NOT be keys the reference defines, and the copy scan MUST read the text tables.
- **FR-016**: The frozen street view of the quick photos MUST show a street with the eye outside every building.
- **FR-017**: CLAUDE.md, the map, the register, the histories, the research record, the specification documents and the skill MUST
  agree; an order is said once (in CLAUDE.md), a fact once (in its owner), and every document's command, path and number MUST be
  true of the tree.
- **FR-018**: Every tool under tools/ MUST have a caller (a suite task, a command of CLAUDE.md or the map, another tool) and MUST
  run; an unused, broken or duplicated tool MUST be removed or fixed at its cause.
- **FR-020**: Every item MUST record, before its code, the reference traced and read (US2's sheet), the Context7 source, question
  and answer for each API its code calls, and the skills used at their moments (the method's checklist at the start, systematic
  debugging for a failure, verification before any "done", the spec skills for a specification); the block's close MUST refuse
  while the work file lacks them, so a fix by trial and error (code changed and run again without a cause read) cannot close.
- **FR-021**: Each comparison sheet MUST judge the reference's behaviour too: a rule against its norm or published method, a case it
  misses; this game does the right rule and every case, never a fault or a gap of the reference.
- **FR-022**: A gap list of the genre's systems (the standard city builders' documented features) MUST be kept against both games'
  inventories, and the systems missing from both MUST be brought as items of the loop, each by a cited method.
- **FR-023**: Every feature brought MUST be measured against the budgets of doc/specification.md on the median and floor profiles
  (km24 at the block's end, on the owner's order), and none may be closed while it breaks a budget the previous build kept.
- **FR-019**: The port of the reference's features MUST proceed area by area as items of the fast loop (FR-010), each item's
  sheet, code, check, photos, commit and push recorded, until the inventory shows every reference feature the player uses paired
  or justified as not brought (a fault, a lab-only tool, a superseded design).

### Key Entities

- **Feature**: something a game has that the player uses or sees (an action, a tool, a text, a system, a data entry), with its
  game, area, kind, name, the player's label, description and source line.
- **Pair**: a feature of the reference and one of this game matched by evidence, with how it was found.
- **Comparison sheet**: for one feature, what the reference does and what this game does, from the tracer, the differences and the
  faults never to bring.
- **Item**: one problem or one feature, coded with nothing run, checked in seconds, committed and pushed.
- **Check**: a test scene or photo plan with its recorded time and verdict.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Over the camera tests' rides (every zoom, tilt and bearing of the town), the eye is inside a building in at most 1
  frame in 1,000, and the picture of no ride jolts from the camera's own smoothing.
- **SC-002**: The at-once reactions of the camera to unseen walls are reported, and fall from today's 2.2% of frames to at most
  0.1% of frames.
- **SC-003**: The inventory of both games ends in under 5 seconds and every sampled row (at least 20) matches its source line.
- **SC-004**: Every test scene and photo plan takes under 60 seconds; an item's check (its scenes and photos) gives its verdict in
  under 60 seconds.
- **SC-005**: The 24 km city opens within its budget on the median profile and the traffic window scene ends in under 60 seconds.
- **SC-006**: Every feature brought has a comparison sheet, a test, photos analysed region by region, and a zero copy-scan result;
  the inventory shows it paired afterwards.
- **SC-007**: No commit of the work holds more than one item; every passing item reaches the remote before the next starts.
- **SC-008**: No run of the work is silent, unwatched or without a deadline (the orchestrator's records show each task's lines and
  deadline).
- **SC-009**: The document and tool survey finds 0 contradictions, 0 stale commands or paths, 0 unused or broken tools; the whole
  gates against the register report 0 NEW findings.
- **SC-011**: Every closed block's history shows, per item, the reference read, the Context7 answers and the skills used; no item
  needed more than one check run that failed for a cause its reading should have found.
- **SC-012**: The genre gap list has every system marked present (with its scene), brought, or justified; km24 on the median
  profile holds 60 fps p90 frame time and the floor profile 30 fps, with every feature brought.
- **SC-010**: The inventory's "reference only" count for the areas the player uses falls to 0 once each remaining entry is paired
  or justified in the comparison sheets.

## Assumptions

- The reference game stays at its place on this machine (tools/tasks/locations.py) and is read only for behaviour.
- The owner's chosen order of areas comes with each request; without one, the inventory's gaps decide.
- The budgets are those of doc/specification.md (median and floor profiles).
- The standard games' camera practice (Unreal's spring arm, Cinemachine's deoccluder) is the reference for what the reference game
  does not answer.
- Spec Kit's files are not copied into the repository; the specification lives in doc/specs (CLAUDE.md).
