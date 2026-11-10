"""PostToolUse hook of the trace-fix skill: the proof that official documentation was read.

Every documentation lookup of a session of this repository (context7's `resolve-library-id` and `query-docs`, a web search,
a page fetched) becomes one small record under dist/trace/docs/: the tool, what was asked, the time. `ledger.py fix` accepts
a cause that touches an outside API only when a lookup names that API. The hook blocks nothing and always exits 0.
"""
import sys

sys.dont_write_bytecode = True

import json  # noqa: E402
import os  # noqa: E402
import time  # noqa: E402


def main():
    try:
        event = json.load(sys.stdin)
    except (json.JSONDecodeError, ValueError):
        return
    cwd = event.get("cwd")
    if cwd and os.path.isdir(cwd):
        os.chdir(cwd)
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import proof
    import store

    here = os.path.normcase(os.path.dirname(os.path.abspath(__file__)))
    if not here.startswith(os.path.normcase(store.ROOT)) and not os.environ.get("TRACE_FIX_DIR"):
        return  # a session of another project
    row = {"t": store.now(), "tool": event.get("tool_name", ""), "input": event.get("tool_input") or {}, "session": event.get("session_id", "")}
    proof.write_whole(os.path.join(store.TRACES, "docs", f"{time.time_ns()}-{os.getpid()}.json"), row)


if __name__ == "__main__":
    try:
        main()
    except Exception:  # noqa: BLE001 - a logging hook never breaks the tool it watches; the failure is kept, not hidden
        import traceback
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "read_log_errors.txt"), "a", encoding="utf-8") as f:
            f.write(time.strftime("%Y-%m-%d %H:%M:%S") + "\n" + traceback.format_exc() + "\n")
    sys.exit(0)
