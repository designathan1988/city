---
name: feature-compare
description: How a functionality of this game is identified, compared with the reference game (roadcraft) and built here. Use it for any work on a functionality of the game — a wrong behaviour, something missing, something the reference does that the game should do, how a feature is built — before reading code by hand: it gives, in seconds, the whole functionality in both games from the compilers' own graphs, side by side, with what differs and what the reference does that must not be carried over.
---

# Feature compare

The reference game (`C:\Users\Jonathan\Documents\Projects\lab\roadcraft`, TypeScript) has every functionality working.
It is read for **what** a functionality does, never copied (CLAUDE.md [c:no-roadcraft-copy]). This game is built in
its own architecture (C++ core, GDScript, data), within the budgets of `log/specification.md`.

The graphs come from the languages' own compilers, so nothing is guessed from a name: clang binds every C++ call
(templates, overloads, lambdas, virtual methods), the TypeScript checker every TypeScript call. A function handed as
a value is reached only through the code that handed it. GDScript is joined to the C++ by the names the C++ code
registers.

## 1. Find the places where the functionality produces something, in both games

The place where it draws, writes the state the player sees, or answers the player: a `path:line` or a function's
name in each game. Search the code for the thing (a text the player sees, a data key, a draw), not for an idea of it.

## 2. One command

```bash
python tools/feature/compare.py --game-points <path:line>[,<path:line>] --ref-points <path:line>[,...] --frame _process,requestAnimationFrame,setAnimationLoop,onBeforeRender
```

The first run reads both programs (C++ about two minutes, TypeScript about 15 s); later runs read only what changed.
The sheet is `dist/feature/feature.html` (and `.txt`, `.json`): both games side by side, and above them:

- **What sets it off**: every way from where the program is entered to the place, with the conditions on each step.
- **What it does at the place**: its calls and writes, each with its condition; what leaves the program (the engine).
- **What it runs**, the **named constants** and **numbers**, the **data** under the keys it uses, the **state** read
  near it and who writes it.
- **Values one game has that the other has not**: a threshold, a size, a count the reference uses and the game does
  not is a behaviour the game is missing or does otherwise. Read both lines where each value is used.
- **Cost**: the loops on the way and run, which run every frame, which are inside another loop, and what they call.

## 3. Decide

For each difference, read the lines the sheet names on both sides (whole functions, per CLAUDE.md) and write down:

- the behaviour the reference has (what the player sees, under which condition, with which value);
- what this game does instead and the line that decides it — the cause;
- what is **not** carried over: a reference loop that runs every frame over a collection that grows with the city, a
  loop inside a loop over growing data, work redone each frame whose inputs do not change, and anything the reference's
  own reports name as a problem (`reports/`, `lab-reports/`, `ARQUITETURA.md`). The game does the same behaviour within
  its architecture: decided once per chunk or per change, on its own threads, budgets respected.

## 4. Build, then check once

Code every difference of the block at its cause (`python tools/tasks/cli.py block open <name>`), nothing run inside.
Then, once: both DLLs (`tools/build/build.sh core-windows`), `tools/build/quick.sh <linked scenes>`, the photos of the
functionality near, mid and far, day and night, by the image protocol, and the sheet run again: the values and the
behaviour the reference has are now in the game's column. Close the block (`block verify`, `block close`).

## When the sheet is not enough

- A place the sheet cannot find: the path's end or the function's name is wrong — `--list`-like search: open
  `dist/feature/game_cpp.json` (functions by name) or `ref_ts.json`.
- Shader code is not in the graphs: read the shader the material names (`project/data/render/materials.json`) and the
  reference's shader strings the sheet's constants name.
