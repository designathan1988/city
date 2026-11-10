"""State of the trace-fix skill: where the traces live, one trace's ledger file and the arithmetic of what is read.

The one owner of the repository root, the trace folder (dist/trace; TRACE_FIX_DIR overrides it), the ledger's
schema and its defaults, the line ranges and the phase of a trace. It prints nothing but a refusal.
"""
import hashlib
import json
import os
import subprocess
import sys
import time

# One reading step. Source: the skill's rule "read in chunks": small enough to be read with attention and to
# leave room for the notes of the chunk before the next one enters the context.
CHUNK_LINES = 400
# One batch of `next`: chunks read in parallel in one message. Source: the Read tool's own window (2000 lines) as
# the lines a message takes in, and 25 as the calls of one message that still leave room to write the notes.
BATCH_LINES = 2000
BATCH_CHUNKS = 25
# What a folder added to the flow brings in (code, scenes, shaders, data, scripts of the tools).
SOURCE_EXT = {".h", ".hpp", ".cpp", ".c", ".inc", ".gd", ".gdshader", ".gdshaderinc", ".tscn", ".tres", ".json",
              ".py", ".sh", ".ts", ".js", ".cmake", ".txt"}
SKIP_DIRS = {".git", "dist", "godot-cpp", "addons", "bin", "node_modules", "__pycache__", ".godot"}
OPEN, FIXED, DROPPED = "open", "fixed", "dropped"


class Refused(Exception):
    """An operation the ledger does not accept; the message says what to do instead."""


def repo_root():
    try:
        out = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, timeout=20)
        if out.returncode == 0 and out.stdout.strip():
            return os.path.normpath(out.stdout.strip())
    except (OSError, subprocess.SubprocessError):
        pass
    return os.getcwd()


ROOT = repo_root()
TRACES = os.environ.get("TRACE_FIX_DIR") or os.path.join(ROOT, "dist", "trace")
ACTIVE = os.path.join(TRACES, "ACTIVE")


def fail(message):
    print("ERROR: " + message, file=sys.stderr)
    sys.exit(1)


def now():
    return time.strftime("%Y-%m-%d %H:%M")


def rel(path):
    full = path if os.path.isabs(path) else os.path.join(ROOT, path)
    return os.path.relpath(os.path.normpath(full), ROOT).replace("\\", "/")


def file_bytes(path):
    with open(os.path.join(ROOT, path), "rb") as f:
        return f.read()


def line_count(path):
    data = file_bytes(path)
    return data.count(b"\n") + (1 if data and not data.endswith(b"\n") else 0)


def sha_of(path):
    return hashlib.sha1(file_bytes(path)).hexdigest()


def trace_folder(slug):
    return os.path.join(TRACES, slug)


def ledger_path(slug):
    return os.path.join(trace_folder(slug), "ledger.json")


def active_slug():
    if not os.path.exists(ACTIVE):
        return None
    with open(ACTIVE, encoding="utf-8") as f:
        return f.read().strip() or None


def set_active(slug):
    os.makedirs(TRACES, exist_ok=True)
    with open(ACTIVE, "w", encoding="utf-8") as f:
        f.write(slug)


def new_state(slug, request, reuse):
    return {"slug": slug, "request": request, "opened": now(), "note": "", "reuse": bool(reuse), "read_gate": None,
            "plan_gate": None, "closed": None, "files": [], "problems": [], "causes": []}


def load(slug=None):
    slug = slug or active_slug()
    if not slug or not os.path.exists(ledger_path(slug)):
        fail("no active trace: open one with `open <slug> --request \"...\"`")
    with open(ledger_path(slug), encoding="utf-8") as f:
        state = json.load(f)
    # a ledger written before causes, the plan gate and reuse existed
    for key, value in new_state(slug, "", False).items():
        state.setdefault(key, value)
    for problem in state["problems"]:
        problem.setdefault("cause_id", "")
    return state


def save(state):
    path = ledger_path(state["slug"])
    os.makedirs(os.path.dirname(path), exist_ok=True)
    tmp = path + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump(state, f, ensure_ascii=False, indent=1)
    os.replace(tmp, path)


def merged(ranges):
    out = []
    for a, b in sorted(ranges):
        if out and a <= out[-1][1] + 1:
            out[-1][1] = max(out[-1][1], b)
        else:
            out.append([a, b])
    return out


def gaps(entry):
    """Line ranges of the file not read yet."""
    out, at = [], 1
    for a, b in merged(entry["ranges"]):
        if a > at:
            out.append([at, a - 1])
        at = max(at, b + 1)
    if at <= entry["lines"]:
        out.append([at, entry["lines"]])
    return out


def lines_read(entry):
    return sum(b - a + 1 for a, b in merged(entry["ranges"]))


def find_file(state, path):
    path = rel(path)
    for entry in state["files"]:
        if entry["path"] == path:
            return entry
    return None


def next_chunks(state):
    """The next batch to read, in the order of the flow: (entry, first line, last line) each."""
    out, lines = [], 0
    for entry in state["files"]:
        for a, b in gaps(entry):
            while a <= b:
                end = min(b, a + CHUNK_LINES - 1)
                out.append((entry, a, end))
                lines += end - a + 1
                a = end + 1
                if lines >= BATCH_LINES or len(out) >= BATCH_CHUNKS:
                    return out
    return out


def cause_of(state, cid):
    for cause in state["causes"]:
        if cause["id"].lower() == str(cid).lower():
            return cause
    raise Refused(f"no cause {cid}")


def problem_of(state, pid):
    for problem in state["problems"]:
        if problem["id"].lower() == str(pid).lower():
            return problem
    raise Refused(f"no problem {pid}")


def phase(state):
    if state.get("closed"):
        return "closed"
    if not state["files"]:
        return "trace"
    if not state.get("read_gate"):
        return "read"
    if not state.get("plan_gate"):
        return "plan"
    if any(p["status"] == OPEN for p in state["problems"]):
        return "fix"
    return "close"
