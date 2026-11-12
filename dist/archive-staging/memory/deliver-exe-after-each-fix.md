---
name: deliver-exe-after-each-fix
description: "the owner wants to open the game any time; after each closed fix run tools/build/deliver.sh export so the Desktop's Roadcraft shortcut opens the newest build"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-11T14:07:28.047Z
---

SUPERSEDED IN CADENCE on 2026-11-12 by [[no-city-tests-mid-work]]: the export runs ONCE, at the block's close, not after each fix or commit.

The owner asked (2026-11-11) for a way to open the game whenever he wants, to test and see how it is going.
`tools/build/deliver.sh export` (160812f5) exports to `dist/builds/<date>_<commit>[-dirty]` and points
`Desktop\Roadcraft.lnk` at the newest; a game left open never blocks the next export.

**Why:** a single dist/windows folder could not be replaced while his game was open, and the exe lagged a day behind the fixes.

**How to apply:** after each commit that changes the game (project/ or the DLLs), run the export (about a minute, foreground) and
tell him the shortcut now opens that commit. Never close his open game. See [[commit-push-regularly]].
