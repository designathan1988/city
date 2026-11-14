"""The hook's refusals (PreToolUse, called by port_state.py): what may not be done, by CLAUDE.md, whatever the session thinks.

A shell command (Bash, PowerShell): the engine or the compiler started outside the project's tools; a script or a file's text
typed into the shell (a heredoc, a here-string); a file of the project changed by the shell instead of Edit or Write; a run of
the project's tools muted (piped, redirected), joined to another command or kept in the foreground; a failed verification's
cause named before the debugging skill was followed; a block's work committed before the verification skill was.
An edit (Edit, Write): a Python or JSON file left unreadable by it; a work's note left with no `## Now` or one too long to be
given back whole; the game's code with no block open, or before the skill `port`, Context7 and the internet were used in the
session, or before the domain skill of the file's kind was (port_skills.py: a shader, the interface, the renderer, ...).
Any edit or command, while a block is not closed, before the session stood on its records again: since its start or its last
compaction the skill `port` was not invoked or the block's note was not Read (a compaction keeps a summary; the note is what
was read and decided), or, after a compaction, the owner's words the hook's text could not hold were not Read
(port_memory.WORDS). The game's code, a run of the project's tools and a commit, while the note's `## Now` is older than
the work or was written before the block last moved.
An agent: started while the port is in hand.
After an edit (after_edit): the gates of the file just written, so a defect a reading shows is fixed where it was written.
What the session did is read from its transcript's own records."""

import json
import re
from pathlib import Path

import port_deadline
import port_edits
import port_looks
import port_memory
import port_note
import port_skills

ROOT =Path(__file__).resolve().parents[2]
BLOCK = ROOT / "dist" / "block" / "state.json"
GAME_CODE = ("native/src/", "project/")      # what the edit gate holds
SHELLS = ("Bash", "PowerShell")
AGENTS = ("Agent", "Task")
OWN = r"(?:\.claude|native[/\\]src|project|tools)[/\\]"   # the project's places a shell does not write into
START, RUN, RUNS, LISTED, LEAD = (port_deadline.START, port_deadline.RUN, port_deadline.RUNS, port_deadline.LISTED,
                                  port_deadline.LEAD)   # a run of the project's tools, as port_deadline.py reads it
# the engine, the compiler's driver or the build system started as a command of its own (not named inside another's arguments)
ENGINE = re.compile(r"(?:^|[;&|(]\s*)(?:\"?[\w:/\\.-]*godot[\w.-]*\"?|ninja|cmake)(?:\s|$)", re.IGNORECASE)
MUTED = re.compile(RUN + r"[^;&\n]*(?:\||>)")  # its output piped away or sent somewhere
JOINED = re.compile(r"&&|\|\||;|\n")
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
MARKS = ("compact_boundary", '"Skill"', '"Read"', '"Write"', "context7__query-docs", "WebSearch", "WebFetch", "SendUserFile",
         '"is_error"')               # what `used` reads
WHOLE = ("Read", "Write")            # the calls that leave a file's whole text before the session
COMMIT = re.compile(r"\bgit\b[^|;&]*\bcommit\b")
LOOKED = ("PASS", "CACHED_PASS")     # a run's verdicts that count (tools/tasks/block.py PASSED)


def block() -> dict:
    if not BLOCK.is_file():
        return {"state": "closed", "name": None}
    try:
        return json.loads(BLOCK.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as fault:
        return {"state": f"unreadable ({fault})", "name": None}


def used(transcript: str) -> dict:
    """What this session did since its start or its last compaction, from its transcript's own records (a compaction's boundary
    is a system entry, a call is a tool_use block of an `assistant` entry, its answer a tool_result block of a `user` entry;
    the same words inside a file read, a command's output or the record's copies of a request, which carry no time, are no
    record): the skills invoked (one whose call answered with an error was not: "A failed command aborts the entire skill
    invocation", code.claude.com/docs/en/skills), the files Read, what was sent to the owner, whether Context7 was asked,
    whether the internet was searched or read. None for a transcript that cannot be read: it holds nothing back."""
    path = Path(transcript) if transcript else None
    if path is None or not path.is_file():
        return {}
    fresh = lambda compacted: {"asked": {}, "read": {}, "sent": set(), "docs": False, "web": False,   # noqa: E731
                               "compacted": compacted, "record": str(path)}
    did = fresh(False)
    with path.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if not any(mark in line for mark in MARKS):
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue   # the line being written now
            kind = entry.get("type")
            if kind == "system" and entry.get("subtype") == "compact_boundary":
                did = fresh(True)
                continue
            message = entry.get("message")
            content = message.get("content") if isinstance(message, dict) else None
            for part in content if isinstance(content, list) and kind in ("assistant", "user") else ():
                if isinstance(part, dict) and kind == "user" and part.get("type") == "tool_result" and part.get("is_error"):
                    did["asked"].pop(part.get("tool_use_id"), None)   # a skill whose call failed gave the session nothing
                if not isinstance(part, dict) or kind != "assistant" or part.get("type") != "tool_use":
                    continue
                name, given = str(part.get("name", "")), part.get("input") or {}
                if name == "Skill":
                    did["asked"][part.get("id") or len(did["asked"])] = str(given.get("skill", ""))
                if name == "SendUserFile":   # what was put before the owner
                    did["sent"].update(port_note.key(f) for f in given.get("files") or () if isinstance(f, str))
                if name in WHOLE and not (given.get("offset") or given.get("limit")):   # a file read or written whole
                    did["read"][port_note.key(given.get("file_path", ""))] = port_looks.when(entry.get("timestamp"))
                did["docs"] = did["docs"] or name.endswith("context7__query-docs")
                did["web"] = did["web"] or name in ("WebSearch", "WebFetch")
    did["skills"] = set(did.pop("asked").values())
    return did


def deny(reason: str) -> dict:
    return {"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "deny", "permissionDecisionReason": reason}}


def porting(did: dict) -> bool:
    """The port is in hand: its block is not closed, or its skill was invoked in the session."""
    return block()["state"] != "closed" or any(k == "port" or k.endswith(":port") for k in did.get("skills", ()))


def shell(command: str, background: bool, did: dict, powershell: bool = False) -> dict | None:
    """What a shell command may not be."""
    skills = did.get("skills", set())
    late_to_end = port_deadline.undated(command, background, powershell, port_looks.ORDER.is_file())
    if late_to_end:
        return deny(late_to_end)
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
    running = bool(RUNS.search(command)) and not LISTED.search(command)
    late = port_note.stale(block()) if running or COMMIT.search(command) else ""
    if late:
        return deny(late)
    words = port_looks.claimed(command)
    forged = port_looks.unsaid(did["record"], words) if words is not None else ""
    if forged:
        return deny("CLAUDE.md: " + forged)
    over = port_looks.held(command, block()["state"]) if running else ""
    if over:
        return deny(over)
    if re.search(r"\bblock\s+open\b", command) and "--cause" in command and not any("systematic-debugging" in k for k in skills):
        return deny("CLAUDE.md: a failed check's cause is found by the skill superpowers:systematic-debugging (invoke it "
                    "with the Skill tool and follow it: read the failing lines, the code they name, the evidence) before "
                    "it is named at `block open --cause`. A guess is no cause.")
    committing = bool(COMMIT.search(command)) and block()["state"] == "verify"
    own = port_looks.own_plans(block().get("runs"), LOOKED) if committing else []
    if committing and not own:
        return deny("CLAUDE.md: a block's work is committed only after the functionality itself was photographed. Its own "
                    "photo plan: a plan of project/tests/visual written or changed in this block (a task of suites.json), that "
                    "drives the game through each case and photographs it from near: the tool in hand, the state before, the "
                    "action done, the result, each variant. The far views of the generated town (`zoom_ladder`) show nothing "
                    "of what a block built and are no proof of it. List the cases, write the plan, name it in this "
                    "verification's one run, then open and judge each photo.")
    unseen = [f for plan in own for f in port_looks.unseen(did["read"], plan, False)]
    if unseen:
        return deny("CLAUDE.md: the functionality was photographed and not looked at. Open each of these whole with the Read "
                    "tool and say of each what it shows of its case (what the player sees working, or what is wrong: a wrong "
                    "photo is a failed check, its cause read in the code), and only then commit: " + ", ".join(unseen))
    hidden = [plan for plan in own if port_looks.unshown(did.get("sent", set()), plan)]
    if hidden:
        return deny("CLAUDE.md: the owner is shown the functionality working. Send the photos of " + ", ".join(hidden)
                    + " whole with SendUserFile (dist/captures/<plan>/*.png), each named by what it proves, and only then commit.")
    if committing and not any(
            "verification-before-completion" in k for k in skills):
        return deny("CLAUDE.md: before a block's work is committed, invoke superpowers:verification-before-completion "
                    "with the Skill tool and follow it (each claim against the output of its check).")
    return None


def ungrounded(did: dict, k: dict) -> str:
    """Why nothing may be changed or run yet: a block is not closed and, since this session's start or its last compaction,
    it did not stand on its records again. Nothing when it did, or when there is no block."""
    closed = k.get("state") in (None, "closed")
    if not did or (closed and not did["compacted"]):
        return ""
    note = port_note.note_file(k)
    work = port_note.key(port_note.WORK) + "/"
    stood = note is None or port_note.key(note) in did["read"] or (closed and any(f.startswith(work) for f in did["read"]))
    words = port_memory.WORDS
    owed = [what for what, done in (
        ("invoke the project's skill `port` with the Skill tool (the procedure and its rules)",
         any(s == "port" or s.endswith(":port") for s in did["skills"])),
        (f"Read the note {port_note.rel(note) if note else ''} whole with the Read tool, no offset and no limit (what was "
         "read, designed, done and left: a compaction's summary is no source)", stood),
        (f"Read {port_note.rel(words)} whole with the Read tool (the owner's words of this session that the hook's text "
         "could not hold after the compaction)",
         not (did["compacted"] and words.is_file()) or port_note.key(words) in did["read"])) if not done]
    if not owed:
        return ""
    hand = "no block is open, the conversation was compacted" if closed else f"the block {k.get('name')} is {k.get('state')}"
    return (f"CLAUDE.md: {hand} and this session, since its start or its last "
            "compaction, has not stood on its records. Before any edit or command: " + "; ".join(owed) + ". Then do this again.")


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
        return shell(str(data.get("command", "")), bool(data.get("run_in_background")), did, tool == "PowerShell")
    written = str(data.get("file_path", "")).replace("\\", "/")
    fault = port_edits.left_broken(tool, data, written)
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
    if owed:
        return deny("CLAUDE.md, before the game's code is edited: " + "; ".join(owed) + ". Then make this edit again.")
    unskilled = port_skills.owed(inside, did)
    return deny(unskilled) if unskilled else None
