"""What a compaction gives back from the session's own record, its transcript on disk (port_state.py, SessionStart of the
compact source, its second text): the owner's words in this session, verbatim, newest first; the files changed and the
commands run since the note's `## Now` was last rewritten; every uncommitted file.

Why: a compaction "summarizes the conversation history", and "detailed instructions from early in the conversation may be
lost" (code.claude.com/docs/en/how-claude-code-works); what the owner said only in the conversation must not rest on that
summary nor on the session having copied it into the note. The transcript keeps every entry after a compaction (the
boundary is one more entry), so the owner's words are read back from it. Claude Code measures each hook's text on its own,
10,000 characters (code.claude.com/docs/en/hooks: "Claude Code measures each string on its own, even when several hooks run
for the same event"), so this text has its own room beside the state's. Words that do not fit it are written whole, in
order, to WORDS, which the gate has Read after the compaction (port_gate.ungrounded)."""

import json
import os
import re
import time
from pathlib import Path

import port_looks
import port_note

ROOT = Path(__file__).resolve().parents[2]
# the owner's words of the session, whole, when the hook's text cannot hold them (CITY_OWNER_WORDS: the tests' own place)
WORDS = Path(os.environ.get("CITY_OWNER_WORDS") or ROOT / "dist" / "block" / "owner_words.md")
# what wraps a message in the record and is none of its words: a command's name and message, the marks of its arguments
# and of a pasted text, the command typed before a pasted text
WRAP = re.compile(r"<command-name>.*?</command-name>|<command-message>.*?</command-message>|</?command-args>"
                  r"|</?pasted_content[^>]*>|^\s*/[\w:-]+\s", re.DOTALL)
EDITS = ("Edit", "Write", "MultiEdit", "NotebookEdit")
SHELLS = ("Bash", "PowerShell")
COMMANDS = 10          # the last commands given back
COMMAND_SHOWN = 240    # characters of a command given back (its first line): what it was, not its script
WORK_MOST = 3500       # characters of the work's part (files and commands): the owner's words have the rest of the text


def owner_words(transcript: str) -> list:
    """The owner's messages in this session, in order, each once (a prompt queued while the session worked is recorded
    twice, and a command's again inside its own wrapping): port_looks.owner_texts says what a message of the owner's is."""
    seen, words = set(), []
    for text in port_looks.owner_texts(transcript):
        said = text.strip()
        key = port_looks.norm(WRAP.sub(" ", said))
        if said and key and key not in seen:
            seen.add(key)
            words.append(said)
    return words


def since(transcript: str, after: float) -> tuple:
    """The files the session changed ({path: times}) and the commands it ran (the last COMMANDS), by its own calls recorded
    after `after` (seconds since the epoch)."""
    changed, commands = {}, []
    path = Path(transcript) if transcript else None
    if path is None or not path.is_file():
        return changed, commands
    with path.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if '"tool_use"' not in line:
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue   # the line being written now
            if entry.get("type") != "assistant" or port_looks.when(entry.get("timestamp")) < after:
                continue   # a call is an assistant's entry with its time; the record's copies of a request have none
            message = entry.get("message")
            content = message.get("content") if isinstance(message, dict) else None
            for part in content if isinstance(content, list) else ():
                if not isinstance(part, dict) or part.get("type") != "tool_use":
                    continue
                name, given = str(part.get("name", "")), part.get("input") or {}
                if name in EDITS:
                    file = port_note.rel(Path(str(given.get("file_path") or given.get("notebook_path") or "")))
                    changed[file] = changed.get(file, 0) + 1
                elif name in SHELLS:
                    first = (str(given.get("command", "")).strip().splitlines() or [""])[0]
                    commands.append(first[:COMMAND_SHOWN])
    return changed, commands[-COMMANDS:]


def work_part(transcript: str, k: dict, uncommitted: list) -> str:
    """The files changed and the commands run since `## Now` was rewritten, and every uncommitted file."""
    at = float(port_note.now_seen(k).get("at", 0.0))
    changed, commands = since(transcript, at)
    when = time.strftime("%Y-%m-%d %H:%M", time.localtime(at)) if at else "never"
    lines = [f"Since the note's `## Now` was last rewritten ({when}), by this session's own calls:"]
    lines.append("- files changed: " + (", ".join(f"{f} ({n})" for f, n in changed.items()) if changed else "none"))
    lines += ["- the last commands run:"] + [f"  - {c}" for c in commands] if commands else ["- no command run"]
    lines.append(f"Uncommitted (git status, {len(uncommitted)}): " + (", ".join(uncommitted) if uncommitted else "nothing"))
    text = "\n".join(lines)
    return text if len(text) <= WORK_MOST else text[:WORK_MOST - 40] + "\n[the rest: `git status`]"


def keep_words(words: list) -> None:
    """The owner's words written whole, in order, for the session to Read when the hook's text cannot hold them."""
    WORDS.parent.mkdir(parents=True, exist_ok=True)
    WORDS.write_text("# The owner's words in this session, in order, verbatim (from its transcript)\n\n"
                     + "\n\n".join(f"## {n}\n\n{text}" for n, text in enumerate(words, 1)) + "\n", encoding="utf-8")


def given_back(transcript: str, k: dict, uncommitted: list) -> str:
    """The text a compaction gives back from the record: the work's part, then the owner's words, newest first, as many
    whole as the hook's text holds; when some do not fit, all of them are written to WORDS and the text says so."""
    work = work_part(transcript, k, uncommitted)
    words = owner_words(transcript)
    head = ("The session's own record (its transcript on disk), given back after the compaction. The owner's words in this "
            f"session, verbatim, newest first ({len(words)} in all):")
    pointer = (f"\n[The owner's words that did not fit here are in {port_note.rel(WORDS)}, with all the others in order: Read "
               "it whole (the hook refuses every edit and command until it is).]")
    room = port_note.HOOK_MOST - len(head) - len(work) - len(pointer) - 8
    shown, used = [], 0
    for n in range(len(words), 0, -1):
        piece = f"\n[{n}] {words[n - 1]}"
        if used + len(piece) > room:
            break
        shown.append(piece)
        used += len(piece)
    whole = len(shown) == len(words)
    if whole:
        WORDS.unlink(missing_ok=True)
    else:
        keep_words(words)
        if not shown:
            shown.append(f"\n[{len(words)}] {words[-1][:max(room - 40, 0)]} [cut]")
    return port_note.fit(head + "".join(shown) + ("" if whole else pointer) + "\n\n" + work)
