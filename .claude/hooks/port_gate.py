"""The hook's refusals (PreToolUse, called by port_state.py): what may not be done, by CLAUDE.md, whatever the session thinks.

A shell command (Bash, PowerShell): the engine or the compiler started outside the project's tools; a script or a file's text
typed into the shell (a heredoc, a here-string); a file of the project changed by the shell instead of Edit or Write; a run of
the project's tools muted (piped, redirected), joined to another command or kept in the foreground; a failed verification's
cause named before the debugging skill was followed; a block's work committed before the verification skill was.
An edit (Edit, Write): a Python or JSON file left unreadable by it; a work's note left with no `## Now` or one too long to be
given back whole; the game's code with no block open, or before the skill `port`, Context7 and the internet were used in the
session.
Any edit or command, while a block is not closed, before the session stood on its records again: since its start or its last
compaction the skill `port` was not invoked or the block's note was not Read (a compaction keeps a summary; the note is what
was read and decided). The game's code, a run of the project's tools and a commit, while the note is older than the work.
An agent: started while the port is in hand.
After an edit (after_edit): the gates of the file just written, so a defect a reading shows is fixed where it was written.
What the session did is read from its transcript's own records."""

import json
import re
import subprocess
import sys
from pathlib import Path

import port_note

ROOT = Path(__file__).resolve().parents[2]
GATE_SECONDS = 15  # the gates of one file answer in about three
GATE_SHOWN = 12     # findings of a file shown after an edit
BLOCK = ROOT / "dist" / "block" / "state.json"
GAME_CODE = ("native/src/", "project/")      # what the edit gate holds
SHELLS = ("Bash", "PowerShell")
AGENTS = ("Agent", "Task")
SOUND = (".py", ".json")                     # files an edit may not leave unreadable
OWN = r"(?:\.claude|native[/\\]src|project|tools)[/\\]"   # the project's places a shell does not write into
START = r"(?:^|[;&|(\n]\s*)"                 # where a command starts inside a command line
# the engine, the compiler's driver or the build system started as a command of its own (not named inside another's arguments)
ENGINE = re.compile(r"(?:^|[;&|(]\s*)(?:\"?[\w:/\\.-]*godot[\w.-]*\"?|ninja|cmake)(?:\s|$)", re.IGNORECASE)
# a run of the project's tools, as a command of its own
RUN = (START + r"(?:(?:python[\d.]*(?:\s+-\w+)*|bash|sh)\s+)?[\"']?[\w:./\\-]*"
       r"(?:cli\.py[\"']?\s+run\b|build\.sh|km24\.py|hunt\.py|deliver\.sh|quick\.sh)")
RUNS = re.compile(RUN)
LISTED = re.compile(r"\s--list\b")           # a run that only names its tasks
MUTED = re.compile(RUN + r"[^;&\n]*(?:\||>)")  # its output piped away or sent somewhere
JOINED = re.compile(r"&&|\|\||;|\n")
LEAD = re.compile(r"^\s*cd\s+[^;&|\n]+(?:&&|;)\s*")   # the one thing a run may follow: the folder it runs in
# a script fed to an interpreter from the command line itself, or a file's text laid by a heredoc
TYPED = re.compile(START + r"(?:python[\d.]*|py|node)\b[^\n;&|]*<<"
                   r"|@['\"]\s*\r?\n[\s\S]*?\r?\n['\"]@\s*\|\s*(?:python[\d.]*|py|node)\b"
                   r"|\b(?:cat|tee)\b[^\n;&|<]*>[^\n;&|<]*<<"
                   r"|\b(?:cat|tee)\b\s*<<-?\s*['\"]?\w+['\"]?\s*>"
                   r"|\btee\b[^\n;&|<]*<<", re.IGNORECASE)
# a file of the project changed by the shell
SHELL_WRITE = re.compile(r"(?:\bsed\s+(?:-\w+\s+)*-\w*i\w*|\bperl\s+-\w*i\w*|\btee|\bSet-Content|\bOut-File|\bAdd-Content)\b"
                         r"[^;&|\n]*" + OWN + r"|>>?\s*[\"']?(?:[\w:./\\-]*[/\\])?" + OWN, re.IGNORECASE)
CONTEXT7 = ("ToolSearch `select:mcp__plugin_context7_context7__resolve-library-id,"
            "mcp__plugin_context7_context7__query-docs`")
MARKS = ("compact_boundary", '"Skill"', '"Read"', "context7__query-docs", "WebSearch", "WebFetch")   # a record `used` reads
COMMIT = re.compile(r"\bgit\b[^|;&]*\bcommit\b")


def block() -> dict:
    if not BLOCK.is_file():
        return {"state": "closed", "name": None}
    try:
        return json.loads(BLOCK.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as fault:
        return {"state": f"unreadable ({fault})", "name": None}


def used(transcript: str) -> dict:
    """What this session did since its start or its last compaction, from its transcript's own records (a compaction's boundary
    is a system entry, a call is a tool_use block of an assistant's message; the same words inside a file read or a command's
    output are no record): the skills invoked, the files Read, whether Context7 was asked, whether the internet was searched
    or read. None for a transcript that cannot be read: it holds nothing back."""
    path = Path(transcript) if transcript else None
    if path is None or not path.is_file():
        return {}
    did = {"skills": set(), "read": set(), "docs": False, "web": False}
    with path.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if not any(mark in line for mark in MARKS):
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue   # the line being written now
            if entry.get("type") == "system" and entry.get("subtype") == "compact_boundary":
                did = {"skills": set(), "read": set(), "docs": False, "web": False}
                continue
            message = entry.get("message")
            content = message.get("content") if isinstance(message, dict) else None
            for part in content if isinstance(content, list) else ():
                if not isinstance(part, dict) or part.get("type") != "tool_use":
                    continue
                name = str(part.get("name", ""))
                if name == "Skill":
                    did["skills"].add(str((part.get("input") or {}).get("skill", "")))
                if name == "Read":
                    did["read"].add(port_note.key((part.get("input") or {}).get("file_path", "")))
                did["docs"] = did["docs"] or name.endswith("context7__query-docs")
                did["web"] = did["web"] or name in ("WebSearch", "WebFetch")
    return did


def deny(reason: str) -> dict:
    return {"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "deny", "permissionDecisionReason": reason}}


def porting(did: dict) -> bool:
    """The port is in hand: its block is not closed, or its skill was invoked in the session."""
    return block()["state"] != "closed" or any(k == "port" or k.endswith(":port") for k in did.get("skills", ()))


def shell(command: str, background: bool, did: dict) -> dict | None:
    """What a shell command may not be."""
    skills = did.get("skills", set())
    if ENGINE.search(command):
        return deny("CLAUDE.md: the engine and the compiler are started only by the project's tools, which hold the block's "
                    "rule (nothing runs while code is written, a failed check is not run again as it is): "
                    "`python tools/tasks/cli.py run <task>`, `tools/build/build.sh core-windows`.")
    if TYPED.search(command):
        return deny("A script or a file's text is never typed into the shell: a heredoc eats its backslashes and has broken "
                    "this project's files (the hook itself among them). Write the script with the Write tool into the "
                    "scratchpad and run it by its path; change a file with Edit.")
    if SHELL_WRITE.search(command):
        return deny("A file of the project is changed with Edit or Write, where the gates of an edit hold; never by sed -i, a "
                    "redirect, tee or Set-Content from the shell.")
    if RUNS.search(command) and not LISTED.search(command):
        if MUTED.search(command):
            return deny("CLAUDE.md, Working: a process shows its output as it runs. A run of the project's tools is never piped "
                        "through grep, tail or head nor redirected: start it as it is, watch it with "
                        "`python tools/tasks/cli.py status`, and read its output when it ends.")
        if JOINED.search(LEAD.sub("", command).strip()):
            return deny("One run, one command: a run of the project's tools is not joined to another command (&&, ;, a new "
                        "line), so that none waits on another and each one's result is read by itself. Only `cd <folder> &&` "
                        "may come before it.")
        if not background:
            return deny("A run of the project's tools is started with run_in_background: true: it never holds the session, "
                        "its end is told to you, `python tools/tasks/cli.py status` shows it as it goes. Start it again so.")
    if not did:
        return None
    late = port_note.stale(block()) if (RUNS.search(command) and not LISTED.search(command)) or COMMIT.search(command) else ""
    if late:
        return deny(late)
    if re.search(r"\bblock\s+open\b", command) and "--cause" in command and not any("systematic-debugging" in k for k in skills):
        return deny("CLAUDE.md: a failed check's cause is found by the skill superpowers:systematic-debugging (invoke it "
                    "with the Skill tool and follow it: read the failing lines, the code they name, the evidence) before "
                    "it is named at `block open --cause`. A guess is no cause.")
    if COMMIT.search(command) and block()["state"] == "verify" and not any(
            "verification-before-completion" in k for k in skills):
        return deny("CLAUDE.md: before a block's work is committed, invoke superpowers:verification-before-completion "
                    "with the Skill tool and follow it (each claim against the output of its check).")
    return None


def unreadable(path: str, text: str) -> str:
    """Why a Python or JSON file's text cannot be read by its own reader; nothing when it can."""
    try:
        if path.endswith(".py"):
            compile(text, path, "exec")
        else:
            json.loads(text)
    except (SyntaxError, ValueError) as fault:
        return f"{type(fault).__name__}: {fault}"
    return ""


def texts(tool: str, data: dict, path: str) -> tuple:
    """A file's text before an edit and after it; None for one that is not there or cannot be told (the tool answers that
    itself)."""
    file = Path(path)
    try:
        before = file.read_text(encoding="utf-8").replace("\r\n", "\n") if file.is_file() else None
    except (OSError, UnicodeDecodeError):
        return None, None
    if tool == "Write":
        return before, str(data.get("content", ""))
    old, new = str(data.get("old_string", "")).replace("\r\n", "\n"), str(data.get("new_string", "")).replace("\r\n", "\n")
    if tool != "Edit" or before is None or not old or old not in before:
        return before, None
    return before, before.replace(old, new) if data.get("replace_all") else before.replace(old, new, 1)


def left_broken(tool: str, data: dict, path: str) -> str:
    """Why this edit would leave a Python or JSON file unreadable, or a work's note with no state to go on from; nothing
    when it would not, when the file was unreadable before it, or when what the edit leaves cannot be told."""
    noted = port_note.key(path).startswith(port_note.key(port_note.WORK) + "/") and path.endswith(".md")
    if not noted and not path.endswith(SOUND):
        return ""
    before, after = texts(tool, data, path)
    if after is None:
        return ""
    if noted:
        return port_note.now_fault(after)
    return "" if before is not None and unreadable(path, before) else unreadable(path, after)


def ungrounded(did: dict, k: dict) -> str:
    """Why nothing may be changed or run yet: a block is not closed and, since this session's start or its last compaction,
    it did not stand on its records again. Nothing when it did, or when there is no block."""
    if not did or k.get("state") in (None, "closed"):
        return ""
    note = port_note.note_file(k)
    owed = [what for what, done in (
        ("invoke the project's skill `port` with the Skill tool (the procedure and its rules)",
         any(s == "port" or s.endswith(":port") for s in did["skills"])),
        (f"Read the note doc/work/{k.get('name')}.md whole with the Read tool (what was read, designed, done and left: a "
         "compaction's summary is no source)", note is None or port_note.key(note) in did["read"])) if not done]
    if not owed:
        return ""
    return (f"CLAUDE.md: the block {k.get('name')} is {k.get('state')} and this session, since its start or its last "
            "compaction, has not stood on its records. Before any edit or command: " + "; ".join(owed) + ". Then do this again.")


def after_edit(payload: dict) -> str:
    """What the gates say of a file of the project right after an edit of it (PostToolUse of Edit and Write): the findings no
    open problem of the register names, to be fixed at once, in the code, where they were just written. A defect a reading
    shows is never left for a build or a test to find. Nothing for a file outside the project, a clean one, or gates that
    do not answer in time."""
    if payload.get("tool_name") not in ("Edit", "Write", "MultiEdit"):
        return ""
    written = str((payload.get("tool_input") or {}).get("file_path", "")).replace("\\", "/")
    root = ROOT.as_posix()
    if not written.lower().startswith(root.lower() + "/") or not Path(written).is_file():
        return ""
    inside = written[len(root) + 1:]
    if inside.startswith(("dist/", ".git/")):
        return ""
    try:
        done = subprocess.run([sys.executable, str(ROOT / "tools" / "gates" / "run_gates.py"), "--file", inside, "--register",
                               str(ROOT / "doc" / "problems.jsonl")], cwd=str(ROOT), capture_output=True, text=True,
                              encoding="utf-8", errors="replace", timeout=GATE_SECONDS)
    except (OSError, subprocess.TimeoutExpired):
        return ""
    found = [line for line in done.stdout.splitlines() if line.startswith("NEW ")]
    if done.returncode == 0 or not found:
        return ""
    return (f"The gates of {inside}, read right after this edit ({len(found)} finding(s) no open problem names). Fix each one "
            "now, in the code, before the next file: nothing is built or run to find what a reading shows.\n" + "\n".join(found[:GATE_SHOWN]))


def gate(payload: dict) -> dict | None:
    """The refusal of a tool call, or nothing (the module's head names each)."""
    tool = str(payload.get("tool_name", ""))
    data = payload.get("tool_input") or {}
    did = used(payload.get("transcript_path", ""))
    k = block()
    if tool in AGENTS:
        return deny("CLAUDE.md: the session does its own work. No agent reads, traces or codes the port: do it yourself, "
                    "with Read, the tracer and Edit.") if porting(did) else None
    adrift = ungrounded(did, k)
    if adrift:
        return deny(adrift)
    if tool in SHELLS:
        return shell(str(data.get("command", "")), bool(data.get("run_in_background")), did)
    written = str(data.get("file_path", "")).replace("\\", "/")
    fault = left_broken(tool, data, written)
    if fault:
        return deny(f"This edit would leave {written} unfit ({fault}). Make the edit whole and right; nothing was written.")
    root = ROOT.as_posix()
    inside = written[len(root) + 1:] if written.lower().startswith(root.lower() + "/") else written
    if not inside.startswith(GAME_CODE):
        return None
    if k["state"] != "open":
        return deny(f"CLAUDE.md: the game's code is written only with its block open (the block is {k['state']}). "
                    + ("A fix after a failed check: `python tools/tasks/cli.py block open --cause \"<check>: <file:line>: <why>\"`."
                       if k["state"] == "verify" else "`python tools/tasks/cli.py block open <area>` first."))
    if not did:
        return None
    late = port_note.stale(k)
    if late:
        return deny(late)
    owed = [what for what, done in (
        ("invoke the project's skill `port` with the Skill tool (the procedure: it is invoked again after every compaction)",
         any(k == "port" or k.endswith(":port") for k in did["skills"])),
        (f"load Context7 ({CONTEXT7}) and ask it (query-docs) about the APIs this code calls", did["docs"]),
        ("search the internet (WebSearch, WebFetch) for the design decision and for each value of the real world this code "
         "takes from a norm or a published method", did["web"])) if not done]
    return deny("CLAUDE.md, before the game's code is edited: " + "; ".join(owed) + ". Then make this edit again.") if owed else None
