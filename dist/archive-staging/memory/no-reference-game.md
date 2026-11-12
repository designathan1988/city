---
name: no-reference-game
description: "The lab (C:/Users/Jonathan/Documents/Projects/lab/roadcraft) is the working reference for WHAT the game does, never code to copy or translate (2026-11-10 order); roadcraft-4.0-main is an old version"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 9333d05d-f46e-4071-8c08-f79155ff9844
  modified: 2026-11-10T05:31:42.472Z
---

The reference game, with everything working, is `C:/Users/Jonathan/Documents/Projects/lab/roadcraft`. It is used ONLY as a
reference for the functionality to reach (what the player sees, the rules, the cases, the numbers). Its code is never
copied, translated line by line or reproduced in structure. The user's words (2026-11-10): "ele não pode copiar o código,
só usar como referência para atingir a funcionalidade. Dizer isso a você é o mesmo que dizer copie. Então tem que frisar isso."

**Why:** the wrong ports are the cause of the game's performance and behaviour problems: code made for a browser
(TypeScript, one thread, its own caches and workers) carried into a C++ core with threads, CityState and Godot's servers.
"Use the lab" was read as "copy the lab" before, and that is exactly what broke the game.

**How to apply:** in every prompt, brief or skill that names the lab, say it in these terms and stress it: read the lab to
write down the BEHAVIOUR (what happens, in which cases, with which values); then design the code for this game's own
architecture (CLAUDE.md: CityState as the single source, the layers, the threads and locks, data in project/data); check
the result against the lab's behaviour, in tests and photos. Never say "port from the lab" without this.

History: 2026-11-07 the user forbade carrying the old source's code (roadcraft-4.0-main) in; 2026-11-08 he found every port
had come from that OLD version (verge, polygon-union roads) and ordered them redone from the lab by tooling; 2026-11-10 he
made clear the lab is a reference for functionality only. 4.0-main is never a reference.
Related: [[fix-cause-effort-neutral]], [[read-before-delegating]], [[study-before-coding]].
