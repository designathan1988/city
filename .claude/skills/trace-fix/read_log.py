"""PostToolUse hook of the trace-fix skill: the proof that a chunk was read with the Read tool.

Every successful Read of a file of this repository becomes one small file of its own under dist/trace/reads/<key of
the path>/ (TRACE_FIX_DIR overrides the folder): path, first and last line returned, the file's content hash, the time
and the session. One file a Read, written whole and renamed into place: Reads run in parallel and no two hooks ever
write the same file. `ledger.py read` accepts a chunk only when these records cover it. The hook blocks nothing and
never fails a Read: it always exits 0.
"""
import sys

sys.dont_write_bytecode = True  # the skill's files are swapped in place: no stale bytecode beside them

import hashlib  # noqa: E402
import json  # noqa: E402
import os  # noqa: E402
import time  # noqa: E402

# The Read tool's default window when no limit is given. Source: the Read tool's description ("up to 2000 lines").
READ_DEFAULT_LIMIT = 2000


def main():
    try:
        event = json.load(sys.stdin)
    except (json.JSONDecodeError, ValueError):
        return
    if event.get("tool_name") != "Read":
        return
    tool_input = event.get("tool_input") or {}
    path = tool_input.get("file_path")
    if not path or not os.path.isfile(path):
        return
    cwd = event.get("cwd")
    if cwd and os.path.isdir(cwd):
        os.chdir(cwd)
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import proof  # the one owner of the Read records
    import store  # the one owner of the repository root and the trace folder

    here = os.path.normcase(os.path.dirname(os.path.abspath(__file__)))
    if not here.startswith(os.path.normcase(store.ROOT)) and not os.environ.get("TRACE_FIX_DIR"):
        return  # a session of another project: this skill's log is of the repository it is installed in
    relative = store.rel(path)
    if relative.startswith(".."):
        return  # outside this repository
    with open(path, "rb") as f:
        data = f.read()
    total = data.count(b"\n") + (1 if data and not data.endswith(b"\n") else 0)
    response = event.get("tool_response")
    shown = response.get("file") if isinstance(response, dict) else None
    if isinstance(shown, dict) and shown.get("numLines") is not None:
        if "content" not in shown:
            return  # an image, a PDF or a notebook: not lines of text
        start = int(shown.get("startLine") or 1)
        count = int(shown["numLines"])
    else:
        start = int(tool_input.get("offset") or 1)
        count = int(tool_input.get("limit") or READ_DEFAULT_LIMIT)
    start = max(start, 1)
    end = min(start + count - 1, total)
    if end < start:
        return
    row = {"t": time.strftime("%Y-%m-%d %H:%M:%S"), "session": event.get("session_id", ""), "path": relative,
           "from": start, "to": end, "sha": hashlib.sha1(data).hexdigest()}
    proof.write_read(row)


if __name__ == "__main__":
    try:
        main()
    except Exception:  # noqa: BLE001 - a logging hook never breaks the tool it watches; the failure is kept, not hidden
        import traceback
        here = os.path.dirname(os.path.abspath(__file__))
        with open(os.path.join(here, "read_log_errors.txt"), "a", encoding="utf-8") as f:
            f.write(time.strftime("%Y-%m-%d %H:%M:%S") + "\n" + traceback.format_exc() + "\n")
    sys.exit(0)
