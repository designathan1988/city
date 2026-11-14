"""Every process the session starts ends by itself, in seconds, and none is left running (called by port_gate.py at a shell
command, by port_state.py at a stop and after a command).

The owner's order of 2026-11-14 (after a trace sat for seven hours): every process sent lasts seconds at most, is watched,
shows its output, and none ever stays running. Why the deadline is the process's own and not the call's `timeout`: "A local session you
work in from a terminal, the desktop app, or the VS Code extension has no time limit on background commands", and "When a
foreground command reaches its timeout without finishing, Claude Code moves it to the background instead of stopping it"
(code.claude.com/docs/en/tools-reference); tried here: `sleep 40` started in the background with `timeout: 5000` ended by
itself 40 seconds later. So:
- a run of the orchestrator (`cli.py run`, `build.sh`) carries `--seconds N`: the run ends itself there, its trees killed
  (tools/tasks/deadline.py); N is MOST_S at most, more only with the owner's order standing (port_looks.ORDER);
- a trace has the tracer's own deadline (tools/tracer/port.py, Watch); `trace.py` has none unless told `--seconds N`;
- anything else sent to the background starts under coreutils `timeout N` (Bash), which ends it; PowerShell has no such
  word, so nothing is sent to the background from it;
- a command still going at a stop with no deadline of its own (one Claude Code moved to the background) is named, to be
  stopped by its ID (`stray`: the Stop hook's `background_tasks`), and said at once when it is moved (`moved`)."""

import json
import re

MOST_S = 300        # a deadline of seconds: what a run, a trace or a background command may carry at most
START = r"(?:^|[;&|(\n]\s*)"                 # where a command starts inside a command line
# a run of the project's tools, as a command of its own
RUN = (START + r"(?:(?:python[\d.]*(?:\s+-\w+)*|bash|sh)\s+)?[\"']?[\w:./\\-]*"
       r"(?:cli\.py[\"']?\s+run\b|build\.sh|km24\.py|hunt\.py|deliver\.sh|quick\.sh)")
RUNS = re.compile(RUN)
TIMED = re.compile(START + r"(?:(?:python[\d.]*(?:\s+-\w+)*|bash|sh)\s+)?[\"']?[\w:./\\-]*(?:cli\.py[\"']?\s+run\b|build\.sh)")
LISTED = re.compile(r"\s--list\b")           # a run that only names its tasks
TRACE = re.compile(START + r"(?:python[\d.]*(?:\s+-\w+)*\s+)?[\"']?[\w:./\\-]*tracer[/\\](?:port|trace)\.py")
TRACE_OPEN = re.compile(r"tracer[/\\]trace\.py")       # trace.py has no deadline of its own unless told
SECONDS = re.compile(r"\s--seconds(?:\s+|=)(\d+(?:\.\d+)?)\b")
LEAD = re.compile(r"^\s*cd\s+[^;&|\n]+(?:&&|;)\s*")   # the one thing a command may follow: the folder it runs in
UNDER = re.compile(r"^\s*timeout\s+(?:-k\s+\S+\s+)?(\d+(?:\.\d+)?)s?\s+\S")   # coreutils `timeout N command`
MOVED = "moved to the background"            # Claude Code's own words for a foreground command past its timeout
SHOWN = 160                                  # characters of a command named in a refusal


def own(command: str) -> float | None:
    """The deadline a command carries of its own, seconds; None when it has none. 0 for one whose tool holds its tasks'
    deadlines and runs only on the owner's order (km24.py, hunt.py, quick.sh, deliver.sh) and for a run that only lists."""
    if TIMED.search(command) and not LISTED.search(command):
        told = SECONDS.search(command)
        return float(told.group(1)) if told else None
    if RUNS.search(command):
        return 0.0
    if TRACE.search(command):
        told = SECONDS.search(command)
        return float(told.group(1)) if told else (None if TRACE_OPEN.search(command) else 0.0)
    under = UNDER.match(LEAD.sub("", command))
    return float(under.group(1)) if under else None


def undated(command: str, background: bool, powershell: bool, ordered: bool) -> str:
    """Why this command may not start: it would run with no deadline of its own, or with one that is no deadline of
    seconds. Nothing for a command of the foreground that is no run and no trace (Claude Code holds it to the call's
    timeout; one that passes it is named by `moved` and `stray`)."""
    tool = (TIMED.search(command) and not LISTED.search(command)) or TRACE.search(command)
    if not (tool or background):
        return ""
    if powershell and background and not tool:
        return ("CLAUDE.md, Working: every process ends by itself. Nothing is sent to the background from PowerShell (its "
                "`timeout` is a wait, not a deadline): start it with the Bash tool as `timeout <n> <command>`, "
                f"{MOST_S} s at most.")
    has = own(command)
    if has is None and TIMED.search(command):
        return ("CLAUDE.md, Working: every process ends by itself, in seconds. A run carries its own deadline: add "
                f"`--seconds <n>` (what it needs and no more, {MOST_S} at most; `tools/build/build.sh core-windows --seconds "
                "<n>` for the build). Past it the run ends itself, its unfinished tasks TIMEOUT. The call's `timeout` is no "
                "deadline: Claude Code does not apply it to a background command of this session.")
    if has is None and TRACE_OPEN.search(command):
        return ("CLAUDE.md, Working: every process ends by itself. `trace.py` has no deadline unless told: give it "
                f"`--seconds <n>` ({MOST_S} at most; the tracer stops there and says what it did not answer).")
    if has is None:
        return ("CLAUDE.md, Working: every process ends by itself, in seconds, and none is left running. A command sent to "
                f"the background starts under its own deadline: `timeout <n> <command>` (coreutils, seconds, {MOST_S} at "
                "most; only `cd <folder> &&` may come before it). The call's `timeout` is no deadline: Claude Code does not "
                "apply it to a background command of this session.")
    if has > MOST_S and not ordered:
        return (f"CLAUDE.md, Working: a deadline of {has:g} s is no deadline of seconds ({MOST_S} at most). A check that "
                "needs longer is a defect of its own: name fewer tasks, or fix in the code what makes it slow. Longer only "
                "on the owner's order (`python tools/tasks/cli.py block order \"<the owner's own words>\"`).")
    return ""


def stray(tasks) -> list:
    """The shell commands still going (a Stop hook's `background_tasks`) that carry no deadline of their own: each as
    "<id>: <command>"."""
    found = []
    for task in tasks if isinstance(tasks, list) else ():
        if not isinstance(task, dict) or task.get("type") != "shell":
            continue
        command = str(task.get("command", ""))
        if own(command) is None:
            found.append(f"{task.get('id')}: {command[:SHOWN]}")
    return found


def unended(tasks) -> str:
    """Why the session may not stop: a command of its own still runs with nothing to end it."""
    found = stray(tasks)
    if not found:
        return ""
    return ("CLAUDE.md, Working: no process is left running with no deadline. Still going, with nothing to end it: "
            + "; ".join(found) + ". Stop each by its ID (TaskStop), read what it wrote, and find in the code why it did "
            f"not end; what must run again starts under its own deadline (`timeout <n> <command>`, {MOST_S} s at most).")


def moved(payload: dict) -> str:
    """What to do with a foreground command Claude Code just moved to the background at its timeout (PostToolUse of a shell
    command): it now runs with nothing to end it."""
    given = payload.get("tool_input") or {}
    if payload.get("tool_name") not in ("Bash", "PowerShell") or given.get("run_in_background"):
        return ""
    answer = payload.get("tool_response")
    said = answer if isinstance(answer, str) else json.dumps(answer, ensure_ascii=False, default=str)
    if MOVED not in said or own(str(given.get("command", ""))) is not None:
        return ""
    return ("CLAUDE.md, Working: this command passed the call's timeout and Claude Code moved it to the background, where "
            "nothing ends it. Stop it now by the task ID above (TaskStop), read what it wrote, and find in the code why it "
            f"did not end; start it again only under its own deadline (`timeout <n> <command>`, {MOST_S} s at most).")
