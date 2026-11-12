# Feature Specification: Generic defect finder

**Feature Branch**: `main` (work on main, CLAUDE.md [c:main-direct])

**Created**: 2026-11-12

**Status**: Draft

**Input**: User description: "uma ferramenta genérica que identifica os problemas todos, sem travar caso a caso; não só os que já se
sabe, mas os que ainda não foram identificados; mostrar onde está o problema no código, o motivo, cruzar o que está causando; o
trabalho humano tem que ser mínimo; rápida, precisa, leve; resolver causas e não efeitos; como o rastreador de fluxo, genérico, sem
engessamento, sem cenário fechado" (the owner, 2026-11-12). The tool observes the running game generically, judges what it sees
against universal rules of a physical, playable world, traces every violation back to the code that caused it, groups violations by
cause, flags what a change broke, and hands over a ranked list ready for a fix.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Every defect of the game found in one command (Priority: P1)

The owner (or the developer acting for him) runs one command. The game runs hidden over a sweep of cities, camera places, hours of
the day and graphics tiers, and every thing the player would see is checked against rules that hold for any city builder's world:
two solid bodies never stand in one another, nothing floats above or sinks into the ground it stands on, nothing that should be on
screen is missing, every part that needs a partner has it (a span of wire between two poles, a pole under a lamp, a door on a
building's street side), nothing jumps between two moments of time faster than it moves, no value is undefined, the engine reports
no error, and a frame stays within its budget. The result is a list of every violation found, including kinds nobody reported.

**Why this priority**: today each defect is found by the owner looking at photos and then hunted case by case in the code, for
days. A list of all violations, known and unknown, is the precondition of everything else.

**Independent Test**: run the command on the current build; it lists violations with where they are (city, place, time, tier) and
what was seen; inject a known defect (two vehicles placed in one another, a wire level hidden) and the list names it.

**Acceptance Scenarios**:

1. **Given** the current build, **When** the command runs, **Then** it ends within its time budget with a list of violations, each
   with its rule, its place, the state seen and how many times it was met.
2. **Given** a build with a defect of a kind no rule was written for in particular (e.g. a bench inside a hydrant, a car on a
   footway, a floating lamp), **When** the command runs, **Then** the defect appears under the general rule it breaks.
3. **Given** a run that observed nothing (no bodies, no frames), **When** it ends, **Then** it fails: an empty observation is a
   failure, never a pass.

---

### User Story 2 - Each defect traced to the code that causes it (Priority: P1)

For every violation the tool names the code that wrote the offending state: the place in the source that set the position, the
visibility, the level or the value that is wrong, found by following the data flow backwards from the observed field to its
writers. Violations with the same causing writer are grouped: one cause appears once, with all its symptoms, places and counts, so
fixing one cause is seen to fix all of them.

**Why this priority**: a symptom without its cause sends the work back to reading code by hand; the owner orders the cause fixed,
not the effect.

**Independent Test**: for an injected defect whose writer is known, the tool's top cause names that writer's file and line; two
defects injected by the same writer appear as one cause with two symptoms.

**Acceptance Scenarios**:

1. **Given** a list of violations, **When** the tracing step runs, **Then** each cause names its writer's file and line, the
   mechanism (which state, set by which path), the symptoms it produces and one example state.
2. **Given** violations that no writer can be found for, **When** the step runs, **Then** they are listed as "cause unknown" with
   the fields followed, never silently dropped.

---

### User Story 3 - What a change broke is flagged (Priority: P2)

The tool compares its results on the build before a change and on the build after it: a rule that held before and fails after is
a regression, flagged first, with the change's files among the candidate causes. A fix that makes violations disappear is shown as
such.

**Why this priority**: the owner fears that fixing one thing breaks another that worked; the tool must show it at once.

**Independent Test**: run on two builds where the second breaks a rule the first kept; the regression is listed first with the
changed files.

**Acceptance Scenarios**:

1. **Given** results of a previous build, **When** a new build is checked, **Then** new violations are marked "regression", gone
   ones "fixed", and the rest "standing".

---

### User Story 4 - The list feeds the work directly (Priority: P2)

The ranked list (most symptoms first, regressions on top) enters the problem register: each new cause becomes a problem with its
owner file, cause, symptoms and evidence; a cause that disappears closes its problem with the run that proved it. The developer's
work is reading the named code and fixing the cause.

**Independent Test**: after a run, the register holds one problem per new cause; after a fix and a new run, that problem is closed
with the run as its proof.

**Acceptance Scenarios**:

1. **Given** a run with causes not yet in the register, **When** it ends, **Then** each is added once (no duplicates on a second
   run).

### Edge Cases

- A violation that comes and goes from tick to tick (a race): it is counted with how often it was met, not dropped for being rare.
- A rule broken by thousands of instances of one cause (every lot car): one cause, with its count, never thousands of lines.
- A writer reached through a container or several layers (a value copied into a buffer, then into an instance): the trace follows
  the copies to the first writer that decided the value.
- A violation the engine itself causes (a draw the engine culls): it names the engine call that was told the state.
- The game crashes or hangs during the sweep: the run stops at its deadline, keeps what was observed, and lists the crash with its
  last state as a violation.
- A rule that does not apply to a body by design (a car crossing the footway at a lot's gate, a pedestrian in a crowd touching
  another): a body's declared kind and state say so; no special case is written per scene.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The tool MUST observe, in one hidden run, every body the simulation and the renderer hold near the camera (its kind,
  extent, place, heading, the ground under it, whether it is drawn and at which level), every engine error and every frame's cost.
- **FR-002**: The tool MUST judge observations only by general rules of the world, each stated once, applying to every kind of body
  by its declared properties (solid or not, standing on the ground or hung, moving or fixed, which partners it needs); no rule names
  a particular scene, street or object.
- **FR-003**: The rules MUST include at least: no interpenetration of solid bodies; standing bodies on their ground (neither floating
  nor sunk); hung bodies attached at both ends; every body that should be drawn is drawn by exactly one level; required partners
  present; no displacement between two ticks larger than the body's speed allows; no undefined numbers in any observed field; no
  engine error; each frame within its budget from `log/specification.md`.
- **FR-004**: The tool MUST sweep automatically a set of cities (sizes and seeds), camera places (street level, play height, far),
  hours (day and night) and graphics tiers, chosen to cover every kind of body the game has, within its time budget.
- **FR-005**: For each violation, the tool MUST find the writers of the observed fields by following the data flow backwards through
  the game's code (C++ core and scripts) with the project's generic tracer, and name the writer that decided the wrong value.
- **FR-006**: The tool MUST group violations by causing writer and rank causes by regression first, then by number of symptoms.
- **FR-007**: The tool MUST compare a run with the previous build's run and mark each cause as regression, fixed or standing.
- **FR-008**: The tool MUST fail (non-zero exit) when it observed nothing, when the game could not run, or when a violation stands;
  it MUST print what it observed in counts so an empty or partial run is visible.
- **FR-009**: The tool MUST write its result to one file and enter new causes into the problem register through its one owner, and
  close a problem whose cause no run shows any longer, with the run as evidence.
- **FR-010**: Adding a new kind of body to the game MUST make it checked by the existing rules through its declared properties, with
  no change to the tool.

### Key Entities

- **Observation**: what one moment of the game showed of one body: kind, properties, place, extent, ground, draw state, the fields
  it was read from.
- **Rule**: a general statement about bodies and frames, applied to every observation whose properties it concerns.
- **Violation**: an observation (or pair) breaking a rule, with its place, time, city, tier and state.
- **Cause**: the code that wrote the offending state (file, line, function), the path from it to the observed field, and the
  violations it produced.
- **Run**: one sweep with its build, inputs, counts observed, violations and causes; compared with the previous run.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: One command finds every injected defect of a test set covering every rule (100 % of them), each under its rule.
- **SC-002**: For injected defects with a known writer, the top cause named is that writer in at least 90 % of cases.
- **SC-003**: A whole sweep ends in under 10 minutes on the median machine of `log/specification.md`.
- **SC-004**: A defect the owner reported in the past (vehicles in one another, a pole with no wires, furniture in one another,
  bare ground in a lot) is found by the tool without any rule written for that defect in particular.
- **SC-005**: A change that breaks a rule that held before is reported as a regression in the first run after it, at the top of
  the list.
- **SC-006**: The developer's time from a reported symptom to the code to change drops from days to the length of one run plus
  reading the named code.

## Assumptions

- The core already exposes probes that read bodies, draws and poses (vehicle bodies, street pieces, wire levels, road surfaces,
  lot covers, the state hash); the tool generalises them into one observation of every body rather than one probe per case.
- The generic tracer (`tools/tracer`) can follow a field backwards to its writers in the C++ core and the scripts; where it cannot,
  the gap is fixed in the tracer, never by a table of known writers.
- Bodies declare their properties (solid, standing, hung, partners) in data or in the core's kinds; where a kind lacks them, adding
  them is part of this feature.
- The hidden-run orchestrator (`tools/tasks`) runs the sweep with its deadlines; the problem register (`tools/atlas`) receives the
  causes.
- Visual defects that no state can show (a colour, a shading) are outside the rules of this feature; they stay with the photos.
