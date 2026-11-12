---
name: watch-session-clangd
description: "the session's own clangd grows by GBs over a long C++ session; check it and stop it by PID before the user has to complain"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-10T14:18:21.380Z
---

On 2026-11-10 the user showed Task Manager twice with `clangd.exe` at 5 GB, then 9 GB ("esse clangd está cada vez mais alto"). It was this session's own LSP server (child of the same `claude.exe` as my shell, started `--background-index`), 17 GB private after a day of reading and editing C++ files.

**Why:** the machine has 32 GB and the game, the builds and the tests need it; the user had to interrupt the work to point at it.

**How to apply:** in a long C++ session look at clangd's memory now and then (`Get-CimInstance Win32_Process -Filter "Name='clangd.exe'"`, confirm its parent is this session's `claude.exe`) and stop it by PID when it passes about 3 GB; it starts again empty. A local `.clangd` at the repo root (listed in `.git/info/exclude`, never committed) turns off the background index. My reading of the cause (not measured): every file the session opens stays open in clangd with its godot-cpp preamble. See [[no-unwatched-processes]].
