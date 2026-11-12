---
name: mandatory-pause-2026-11-11
description: "BINDING owner order 2026-11-11 night - no new game code until three phases are done (survey, research with proof, fix); the image protocol; the 14 standing orders"
metadata:
  node_type: memory
  type: project
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-12T03:23:15.380Z
---

The owner stopped the work on 2026-11-11 (the /goal of the five steps was cleared) with a long order, "Pausa obrigatória:
levantar, pesquisar, provar e corrigir". The full text is in the session transcript (search "Pausa obrigatória"). It wins over
CLAUDE.md, memory, skills and records where they differ, and the differing text is corrected in the same change.

**Three phases, in order, before any new game code:**
1. Survey: read CLAUDE.md, log/pendencias.md, log/how-we-work.md, log/live-instance-design.md and every record; check each claim
   in the code (nothing the earlier session said is accepted unchecked). Write as found: `log/pendencias.md` (every open item:
   symptom, root cause or "unknown", what was tried, verifiable done-criterion), `log/specification.md` (goals with measurable
   targets, constraints, doc/code divergences, why development is slow, the optimisation architecture with a reference hardware
   profile and budgets, research questions), `log/research.md`. Both new files into log/mapa.md and CLAUDE.md's record list.
   Mandatory items: roadcraft code still standing (lab_scan plus reading for renamed copies; core/ runtime, flow_control); every
   simulation checked against the correct way; every feature tested as a real player (mouse, keyboard) against the lab's
   behaviour; the whole game measured against the median-PC budgets at each graphics tier with photos; scattered documentation
   consolidated (one current document per subject); contradictory instructions fixed; architecture/language/contract violations;
   photos (why images were not analysed, fixed at the cause with a guard; re-analyse the photos of earlier fixes); development
   tools of live-instance-design.md (live instance not stealing focus, real-time monitor vs budgets, real-player input, measuring
   independent of the owner's PC), each studied before built; the slowness of development.
2. Research on the internet and Context7 until each question has a PROVEN solution: problem reproduced with a before measure or
   image, solution applied in an isolated test folder outside the game tree, after measure meeting the criterion, steps to repeat
   with real output. Recorded in log/research.md as done. Never stop early.
3. Fix every pending item, one theme each, by CLAUDE.md's Method, starting with what unblocks the rest (diagnostic tools,
   documentation, contradictory instructions); evidence recorded; commit, push, executable per theme.
End: a summary in Portuguese with the table of pending items (root cause, solution, evidence, status) and what stayed open.

**Standing orders:** (1) never copy roadcraft code - renaming, line-by-line translation or the same structure is copying; it is
read for behaviour only; lab_scan is not proof. (2) the whole game optimised, working, on any median PC: define a reference
hardware profile and budgets (frame, CPU, GPU, memory, load) tied to the graphics tiers; measure against them besides km24
before/after; assess what x86-64-v3 means for that profile; visual quality checked at each tier. (3) root cause, costliest
solution, never a patch nor cheap options. (4) total attention: read the WHOLE output of every build, test and gate; re-read the
request item by item before calling anything done; every doubt investigated. (5) proof, not claims; say failures plainly.
(6) speed without cutting anything; slowness is a defect, recorded and fixed. (7) record in a file at once. (8) no `[c:...]`
contract broken again; every new or changed rule enters CLAUDE.md with a tag and a guard in tools/gates/contracts.json that can
fail. (9) use the existing owners (CityState, CityStore, GameFlow), state controllers, one owner each, no god file, tokens.
(10) one current document per subject; pending only in log/pendencias.md; files created or deleted in log/mapa.md. (11) code,
files and docs in English; player texts in Brazilian Portuguese; conversation in Portuguese. (12) no agents; Context7 on every
implementation, fix and research; the fitting skills always (trace-fix, systematic-debugging, verification-before-completion).
(13) do the whole analysis myself; ask only what is the owner's decision. (14) no extra documents or steps; no loosening.

**CLAUDE.md points changed:** `[c:hidden-runs]` the live instance may open a window that does not steal focus (tests and
captures stay hidden); `[c:investigate-reading]` quick research tests in an isolated folder and measuring the whole game against
the budgets are allowed (causes still found by reading; no trial and error in game code); `[c:research]` Context7 on everything;
every measurement also compared with the median-profile budgets.

**Image protocol (every photo):** write first what the photo must prove and what must appear; plan photos by what the change
asks (the changed element near, far, several angles, day/night, each tier, plus general ones); open each with Read; whole image
then by regions (3x3 crops at real resolution); look for each defect type (missing/extra, broken, deformed, scale, orientation,
overlap, floating/buried, holes, seams, textures, colour, light, shadow, LOD pops, artefacts, UI/text); record the analysis per
photo with positions; a photo with a defect fails the theme; closing fails with an unanalysed photo. See [[photos-follow-the-change]],
[[judge-photos-by-quality]], [[label-and-wait-photos]], [[review-street-edges]].

**Where the work stood when the pause came:** uncommitted in the tree P476/P485/P486 (a building's cap, edges first) and P487
(walkers in at the window's edge); P489 (a pair overlapping) traced with its design in the ledger (rec-549, rec-550, C45), no
code; the owner's photo of a pole with no wires recorded as a ledger problem, cause unknown, not reproduced.

**Where it stands (session 2, 2026-11-12, commit a5b9d9dd):** Phase 1 recorded in `log/specification.md` (goals, median and
floor hardware profiles with budgets, the town measured on both, simulations table, documents inventory, photos re-analysed),
`log/pendencias.md` (50 structured items R/S/F/M/V/D/I/A/T/L) and `log/research.md` (Q1, Q2 proven; Q3-Q20 open). Still owed in
Phase 1: features played as a player (needs the input tool), km24 measured on both profiles, the rest of the photos. The
profiles run on this machine: `dist/research/measure/run_game.py <median|floor> <tag> <deadline> <scene>` (RTX 3060 + six
P-cores; UHD 770 + four E-cores). Another session is committing `tools/analysis/**` in the same tree: never touch its files.
Never `sed` a `log/` file (the hook then wants it re-read whole); `log/mapa.md` costs 686 lines to edit.
By commit 1bed397b: Q1 (median PC), Q2 (profiles in hardware), Q4 (unload/replace/load a GDExtension in a running official
game works; typed scripts need `reload(true)` after; no object alive at unload), Q5 (GPU/VRAM/RAM/CPU by PID from outside;
the engine's monitors understate memory), Q6 (full-field synthetic input works, ui_driver's bare events do not drag or type),
Q16 (the pole with no wires: two engine visibility ranges with hysteresis leave a band where neither wire level is drawn;
trace `pole-no-wires` P1/P2, cause found, fix is Phase 3) are proven with tests in `dist/research/q*/`. km24 measured on the
median profile (GPU p95 up to 14.9 ms, 151 frames over 33 ms, 73.6 s to generate on the debug DLL). Open: Q3, Q7-Q15, Q17-Q20.
A probe's first result may be wrong: Q16's first run said "failed" because the probe's box moved with its instance.

**Why:** the earlier session did not check its work, did not analyse images (the owner's photo: a pole with no wires), made
errors and was slow.
**How to apply:** after any compaction re-read log/specification.md, log/pendencias.md and log/research.md before continuing;
state where each phase stands there.
