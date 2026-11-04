# Converter

Converts features of a TypeScript game (the source) into the architecture of the Godot + C++ game (the target),
and audits code for wrong methodologies. It never writes to the target game: it reads it, and everything it makes
goes to an output folder. Paths are always given on the command line or in a configuration file.

## Stages

| Command | What it does |
|---|---|
| `graph` | Traces a feature (or the whole source) through the source's own type checker: every declaration it uses, typed edges, external APIs, patterns, fingerprints. |
| `audit` | Runs the methodology catalog over a graph: each finding says what is wrong, the correct methodology, its form in the target, and the primary sources. |
| `sandbox` | Tests a conversion package in an isolated clone of the target: the target's own setup and verification before and after the package; prints the manual integration. |
| `baseline` | The target's own setup and verification on its base alone, which proves a sandbox configuration. |

Run with Node 23.6 or newer: `node src/cli.ts <command> --flag value`.

## A conversion package

One feature, stored outside the target:

- `manifest.json`: the source commit and the SHA-256 of every source file read, the target's base commit, the
  converter's commit (packaging refuses a converter with uncommitted changes) and the identity of the commits.
- `patches/`: the conversion as a series of commits, one per topic, each compiling alone (`git format-patch`).
- `evidence/`: every log and photo of the sandbox, before and after.

Integration into the target is manual: the sandbox prints the commands (fetch the reviewed branch, a worktree,
the target's usual integration checks). Nothing integrates by itself.

## Rules the conversion follows

The target's `CLAUDE.md` and gate rules are the authority; these are how the converter applies them.

1. **One owner per feature.** A feature is converted into the subsystem that owns it (one folder), entering the
   target only through the points the architecture designates: its state as a member of `CityState`, its time step
   called by `Simulation::tick`, the player's actions as `sim/commands`, its tunables in `data/**` with a schema read
   by `DataTables`, its drawing in `render/` (read only), its changes announced in the `ChangeLog`. No feature code is
   written into another owner's files.
2. **What another owner must provide is that owner's work.** When a feature needs a capability its neighbour does
   not offer yet, the package names it and is not complete; the converter never writes it into the neighbour, and
   never leaves a declaration without an implementation.
3. **Roles come from each unit's facts, not from its folder.** A source unit that mixes responsibilities (input,
   state, simulation, drawing) is split by responsibility; it is never carried over whole.
4. **Every constant needs a source.** A value moved into `data/**` carries where it came from; the package lists each
   one and is complete only when each has a measurement or a primary source.
5. **The correct methodology is written every time.** Each pattern of the audit catalog is converted to its correct
   form (instancing, reused buffers, a spatial index, a ring buffer, state in its owner), whatever its static rating.
6. **The target's gates judge the result** in the sandbox, with its build, tests and photos.

## Tests

`CONVERTER_TYPESCRIPT=<typescript package dir> node --test "test/**/*.test.ts"`; type check with
`node scripts/typecheck.ts --typescript <dir> --types <node_modules/@types dir>`.
