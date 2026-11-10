# Installing trace-fix

1. Copy this folder to `<project>/.claude/skills/trace-fix/` (`SKILL.md`, `ledger.py`, `commands.py`, `operations.py`,
   `route.py`, `card.py`, `writers.py`, `photos.py`, `rules.json`, `store.py`, `proof.py`, `views.py`, `read_guard.py`, `references.json`,
   `owed.py`, `sessions.py`, `resume.py`, `trace_files.py`,
   `selftest.py`, `selftest_guard.py`). The skill is then listed in every session of the project and is invoked by `/trace-fix` or by itself
   when a problem is reported.
2. Required: the hooks, all one script (`read_guard.py`, told apart by the event it is given). In the user's
   `~/.claude/settings.json` (it reaches sessions already running) or in `<project>/.claude/settings.json`, with the
   path of the installed files:

```json
{
  "hooks": {
    "PreToolUse": [
      {
        "matcher": "Edit|Write|MultiEdit",
        "hooks": [
          { "type": "command", "command": "python \"<project>/.claude/skills/trace-fix/read_guard.py\"" }
        ]
      }
    ],
    "PostToolUse": [
      {
        "matcher": "Read|Edit|Write|MultiEdit|mcp__.*context7.*",
        "hooks": [
          { "type": "command", "command": "python \"<project>/.claude/skills/trace-fix/read_guard.py\"" }
        ]
      }
    ],
    "SessionStart": [
      {
        "matcher": "startup|resume|compact",
        "hooks": [
          { "type": "command", "command": "python \"<project>/.claude/skills/trace-fix/read_guard.py\"" },
          { "type": "command", "command": "python \"<project>/.claude/skills/trace-fix/ledger.py\" resume --hook" }
        ]
      }
    ]
  }
}
```

   What each does:
   - `PostToolUse` on `Read`: every Read of a file of the repository becomes a record, the proof of a reading.
     Without it `ledger.py read` and `record` refuse every chunk.
   - `PreToolUse` on `Edit|Write`: the edit is refused (exit code 2, the reason and the missing Read calls go to
     Claude) until the session has read the lines the edit replaces, as the file is now, since its last compaction (a
     file written over: the whole of it). A file opened in part stops nothing.
   - `PostToolUse` on `Edit|Write`: the file the session has just changed itself stays known to it.
   - `PostToolUse` on context7's tools: each documentation lookup is kept; a file that calls the engine is changed only
     after one made by the session (owed.py, with what else a session owes the trace it works on now).
   - `SessionStart`: after a compaction, what was read before no longer counts for an edit; and every session start
     and compaction prints the open trace (a short answer, silent when nothing is open or in another project), so the
     reading is never restarted or lost.
3. Optional: one line in CLAUDE.md, in its section on the working method:
   `A reported problem is solved by the trace-fix skill: run "python .claude/skills/trace-fix/ledger.py resume" first.`

Check the tools: `python .claude/skills/trace-fix/selftest.py` (exits non-zero on a miss).
The traces live in `dist/trace/<slug>/` (`ledger.json`, and `notes.md` and `report.md` rewritten after every command),
the Read records in `dist/trace/reads/` (by file) and `dist/trace/readers/` (by session), and the notes kept by content
in `dist/trace/notes/` (all outside git); `python ledger.py list` shows every trace. A failure of the hook is kept in
`read_guard_errors.txt` beside it. `photos` needs the orchestrator (`tools/tasks`) and Pillow (the asset tools' own
dependency). Two sessions in one working tree: each sets `TRACE_FIX_SLUG=<its slug>` so neither moves the other's
active trace.
The skill is versioned with the game (`git add -f`: `.claude/` is excluded locally); its documents are allowed by
`tools/gates/rules.json` `allowed_docs`.
