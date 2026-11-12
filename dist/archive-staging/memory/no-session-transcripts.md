---
name: no-session-transcripts
description: "never read other sessions' transcripts (.jsonl, list_events, episodic memory); check their work by branch, diff, files and logs"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 82e887f0-3bf5-483b-8e1a-9df0f96ca163
  modified: 2026-11-09T23:34:24.456Z
---

Never read session transcripts (the per-session .jsonl files, their tool-results and subagent folders),
`list_events` / `search_session_transcripts` / `export_transcript`, or the episodic-memory archive.

**Why:** on 2026-11-09 sessions were blocked by the `reasoning_extraction` safeguard after a session read another
session's transcript to check its work: transcripts carry thinking blocks. The user ordered it removed and blocked: the
thinking blocks were stripped from all transcripts, the episodic-memory plugin disabled and its index deleted, and a
PreToolUse hook (`~/.claude/hooks/block_transcripts.py`, self-test `block_transcripts_test.py`) plus deny rules in
`~/.claude/settings.json` refuse those reads.

**How to apply:** to see whether another session's work is right, read its branch, `git diff`/`log`, the files it
changed, its test logs under `dist/`, and its final report; never its transcript. Related: [[coordinate-parallel-sessions]].
