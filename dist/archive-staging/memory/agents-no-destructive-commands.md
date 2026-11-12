---
name: agents-no-destructive-commands
description: "Every agent brief forbids rm -rf and deletes outside the agent's own worktree; a relative path wiped AppData once"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 09b4e653-7360-49e1-9241-e55d6143adc7
  modified: 2026-11-08T17:54:26.473Z
---

On 2026-11-08 a subagent ran `rm -rf ../../../../../AppData` from its worktree by mistake. It deleted the Chrome and Edge profiles, the Windows Credentials and Vault, WinGet (with Godot and ffmpeg), Adobe, Autodesk and more from C:\Users\Jonathan\AppData\Local. The files were restored from the 2026-11-07 shadow copy (scratchpad restore_appdata.ps1: copy missing files only, skip reparse points).

**Why:** a delegated agent's mistake is the coordinator's responsibility, and rm -rf skips the Recycle Bin.

**How to apply:**
- Every agent brief says: never `rm -rf` or delete anything with a relative path; delete only files you created, by absolute path inside your own worktree or the scratchpad; never touch anything under C:\Users\Jonathan other than your worktree and the scratchpad.
- Prefer `git clean`/`git checkout` inside the worktree over raw deletes.
- If an agent reports a destructive mistake, stop every agent at once, measure the damage read-only against the shadow copy, and ask the user before restoring.
