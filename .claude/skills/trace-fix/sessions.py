"""Which trace a session works on, for the trace-fix skill.

A session (Claude Code's session id: CLAUDE_CODE_SESSION_ID in the ledger's environment, session_id in a hook's event) works on
the trace it opened, switched to or ran a command on: the ledger's commands then go to that trace with no prefix, whatever
another session in the same tree works on, and the session is held to that trace's record (owed.py).
"""
import os
import re

import store

SESSIONS = "sessions"  # under the trace folder: one file a session, the slug of the trace it works on


def current_session():
    """The Claude Code session running this command (its environment); '' outside a session."""
    return os.environ.get("CLAUDE_CODE_SESSION_ID", "")


def session_file(session):
    return os.path.join(store.TRACES, SESSIONS, re.sub(r"[^A-Za-z0-9_.-]", "_", session) + ".txt")


def session_slug(session):
    """The trace a session works on: the last it opened, switched to or ran a command on."""
    if not session or not os.path.exists(session_file(session)):
        return None
    with open(session_file(session), encoding="utf-8") as f:
        return f.read().strip() or None


def set_session_slug(session, slug):
    os.makedirs(os.path.join(store.TRACES, SESSIONS), exist_ok=True)
    with open(session_file(session), "w", encoding="utf-8") as f:
        f.write(slug)


def bind(state, session):
    """The session works on this trace: its commands go here with no prefix, and it is held to the trace's record (owed.py).
    True when the trace gained the session (the ledger is then to be saved)."""
    if not session:
        return False
    if session_slug(session) != state["slug"]:
        set_session_slug(session, state["slug"])
    if session in state["sessions"]:
        return False
    state["sessions"].append(session)
    return True


def open_traces(session=None):
    """[(slug, state)] of every open trace, or of those this session works on."""
    out = []
    for slug in sorted(os.listdir(store.TRACES)) if os.path.isdir(store.TRACES) else []:
        if os.path.exists(store.ledger_path(slug)):
            state = store.load(slug)
            if not state.get("closed") and (session is None or session in state["sessions"]):
                out.append((slug, state))
    return out
