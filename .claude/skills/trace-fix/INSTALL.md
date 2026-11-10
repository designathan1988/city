# Installing trace-fix

1. Copy this folder to `<project>/.claude/skills/trace-fix/` (`SKILL.md`, `ledger.py`, `commands.py`, `operations.py`, `route.py`,
   `store.py`, `proof.py`, `views.py`, `read_log.py`, `selftest.py`).
   The skill is then listed in every session of the project and is invoked by `/trace-fix` or by itself when a problem
   is reported.
2. Required: the hook that proves the reading. Without it `ledger.py read` refuses every chunk. In the user's
   `~/.claude/settings.json` (it reaches sessions already running) or in `<project>/.claude/settings.json`, with the
   path of the installed `read_log.py`:

```json
{
  "hooks": {
    "PostToolUse": [
      {
        "matcher": "Read",
        "hooks": [
          { "type": "command", "command": "python \"<project>/.claude/skills/trace-fix/read_log.py\"" }
        ]
      }
    ]
  }
}
```

3. Required for the memory to hold: every session start and every compaction prints the open trace (a short answer,
   silent when nothing is open or in another project), so the reading is never restarted or lost. Same file:

```json
{
  "hooks": {
    "SessionStart": [
      {
        "matcher": "startup|resume|compact",
        "hooks": [
          { "type": "command", "command": "python \"<project>/.claude/skills/trace-fix/ledger.py\" resume --hook" }
        ]
      }
    ]
  }
}
```

4. Optional: one line in CLAUDE.md, in its section on the working method:
   `A reported problem is solved by the trace-fix skill: run "python .claude/skills/trace-fix/ledger.py resume" first.`

Check the tools: `python .claude/skills/trace-fix/selftest.py` (exits non-zero on a miss).
The traces live in `dist/trace/<slug>/ledger.json`, the Read records in `dist/trace/reads/` and the notes kept by
content in `dist/trace/notes/` (outside git);
`python ledger.py list` shows every trace. A failure of the hook is kept in `read_log_errors.txt` beside it.
The skill is versioned with the game (`git add -f`: `.claude/` is excluded locally); its documents are allowed by
`tools/gates/rules.json` `allowed_docs`.
