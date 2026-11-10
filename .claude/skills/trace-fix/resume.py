"""What a session is shown of its work, for the trace-fix skill: `status`, `resume`, and the session-start hook.

A session is shown the trace it works on (store.active_slug: the one it opened, switched to or ran a command on), with
what it owes before any edit (owed.py, in views.owed_block). At a session's start or after its compaction, the hook gives
the session its own trace; a session that works on none is shown the open traces of the repository, never told to
continue one of them.
"""
import json
import os
import sys

import sessions
import store
import views


def cmd_status(_args):
    views.show(store.load(), owed_session=sessions.current_session())


def cmd_resume(args):
    here = os.path.normcase(os.path.abspath(__file__))
    if args.hook and not here.startswith(os.path.normcase(store.ROOT) + os.sep) and not os.environ.get("TRACE_FIX_DIR"):
        return  # a session of another project: this skill speaks only in the repository it is installed in
    session = sessions.current_session()
    if args.hook:  # the hook's event names the session on stdin
        try:
            session = str(json.loads(sys.stdin.read() or "{}").get("session_id") or session)
        except (ValueError, AttributeError):
            pass
    slug = os.environ.get(store.PINNED) or (sessions.session_slug(session) if session else store.active_slug())
    state = store.load(slug) if slug and os.path.exists(store.ledger_path(slug)) else None
    if state and not state.get("closed"):
        views.show(state, f"trace-fix: THIS SESSION WORKS ON TRACE {slug}. Continue it from here (skill trace-fix); the ledger's "
                          "commands go to it with no prefix. What was read and found is on disk: `export` gives the Read calls "
                          "of notes.md and report.md; read them instead of the code again.", owed_session=session)
        return
    traces = sessions.open_traces()
    if not traces:
        if not args.hook:
            print("trace-fix: no open trace.")
        return
    out = views.Out()
    out.add("trace-fix: open traces in this repository; other sessions may be working on them. To continue one: "
            "`TRACE_FIX_SLUG=<slug> python .claude/skills/trace-fix/ledger.py resume` once (then every command goes to it with no "
            "prefix). A new problem: `open <slug> --request \"...\"`.")
    for name, other in traces:
        if not out.add(f"  {name}: {store.phase(other)} | {views.clip(other['request'], 120)}"):
            break
    out.flush()
