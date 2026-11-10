"""Self-test of the trace-fix tools, in a throwaway trace folder: refusals, proof, batches, causes, reuse, limits.

Runs ledger.py and read_guard.py as the skill does (the hook fed the events Claude Code produces) against small files made
for the test: the proof of a reading, and the edit that is refused until its file is read whole, and a made-up ledger of 1000 files and 200 problems for the output limit. Exits non-zero on any
miss, and a run that checked nothing fails. Usage: python selftest.py
"""
import sys

sys.dont_write_bytecode = True

import json  # noqa: E402
import os  # noqa: E402
import subprocess  # noqa: E402
import tempfile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import selftest_guard  # noqa: E402
import views  # noqa: E402  (only for the limit the outputs are held to)

checks = failures = 0


def check(name, got, want):
    global checks, failures
    checks += 1
    ok = got == want
    failures += not ok
    print(("ok   " if ok else "FAIL ") + f"{name}: got {got}, want {want}")


def main():
    root = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True).stdout.strip()
    root = os.path.normpath(root or os.getcwd())
    with tempfile.TemporaryDirectory() as work:
        traces = os.path.join(work, "trace")
        runs = os.path.join(work, 'tasks')
        reference = os.path.join(work, "reference")  # a tree outside the repository, read whole like its own files
        os.makedirs(reference)
        for name in ("lab.ts", os.path.join("..", "elsewhere.txt")):
            with open(os.path.join(reference, name), "w", encoding="utf-8") as f:
                f.write("".join(f"line {i}" + chr(10) for i in range(1, 31)))
        env = dict(os.environ, TRACE_FIX_DIR=traces, TRACE_FIX_TASKS=runs, TRACE_FIX_REFERENCES=reference)
        # the files under test live inside the repository (the ledger records repository paths), in a folder of their own
        files = tempfile.mkdtemp(prefix="trace_fix_selftest_", dir=os.path.join(root, "dist"))
        small, big, late, edited = (os.path.join(files, n) for n in ("small.txt", "big.txt", "late.txt", "edited.txt"))
        for path, lines in ((small, 15), (big, 900), (late, 20), (edited, 30)):
            with open(path, "w", encoding="utf-8") as f:
                f.write("".join(f"line {i}\n" for i in range(1, lines + 1)))

        def ledger(*args, stdin=None):
            done = subprocess.run([sys.executable, os.path.join(HERE, "ledger.py"), *args], env=env, cwd=root,
                                  capture_output=True, text=True, encoding="utf-8", input=stdin)
            ledger.out = done.stdout
            return done.returncode

        def state(slug):
            with open(os.path.join(traces, slug, "ledger.json"), encoding="utf-8") as f:
                return json.load(f)

        def hook(event, tool, path, session="selftest", offset=None, limit=None, **top):
            """One call of the hook, as Claude Code makes it; the exit code (2 refuses the tool), stderr kept in hook.err."""
            tool_input = {"file_path": path}
            if offset:
                tool_input.update(offset=offset, limit=limit)
            doc = {"hook_event_name": event, "tool_name": tool, "session_id": session, "cwd": root, "tool_input": tool_input, **top}
            done = subprocess.run([sys.executable, os.path.join(HERE, "read_guard.py")], input=json.dumps(doc), env=env,
                                  cwd=root, capture_output=True, text=True, encoding="utf-8")
            hook.err = done.stderr
            return done.returncode

        def tool_read(path, offset=None, limit=None, session="selftest"):
            assert hook("PostToolUse", "Read", path, session, offset, limit) == 0

        def batch(value):
            path = os.path.join(work, "batch.json")
            with open(path, "w", encoding="utf-8") as f:
                json.dump(value, f)
            return path

        try:
            # ---- reading and its proof
            check("status without a trace", ledger("status"), 1)
            check("resume --hook with nothing open is silent", (ledger("resume", "--hook"), ledger.out), (0, ""))
            check("open", ledger("open", "t", "--request", "selftest"), 0)
            check("flow", ledger("flow", small, big, "--why", "selftest"), 0)
            check("read without a Read call", ledger("read", small, "1", "15", "--saw", "x"), 1)
            tool_read(small)
            check("a Read alone counts the lines as read", state("t")["files"][0]["ranges"] if ledger("status") == 0 else None, [[1, 15]])
            check("a note may still be attached to what was read", ledger("read", small, "1", "15", "--saw", "fifteen lines"), 0)
            tool_read(big, 1, 400)
            check("read past what the Read returned", ledger("read", big, "401", "800", "--saw", "x"), 1)
            check("a chunk above the limit", ledger("read", big, "1", "900", "--saw", "x"), 1)

            # ---- batches: whole or not at all
            tool_read(big, 401, 400)
            bad = batch({"reads": [{"path": big, "from": 1, "to": 400, "saw": "first"},
                                   {"path": big, "from": 801, "to": 900, "saw": "not read"}],
                         "problems": [{"where": small + ":3", "what": "w", "cause": "c"}]})
            before = (len(state("t")["problems"]), sum(len(e["saw"]) for e in state("t")["files"]))
            check("a batch with one unproven chunk is refused", ledger("record", bad), 1)
            check("and records nothing of it", (len(state("t")["problems"]), sum(len(e["saw"]) for e in state("t")["files"])) == before, True)
            tool_read(big, 801, 100)
            good = batch({"flow": [{"paths": [late], "why": "called by small", "after": small}],
                          "reads": [{"path": big, "from": 1, "to": 400, "saw": "first"},
                                    {"path": big, "from": 401, "to": 800, "saw": "second"},
                                    {"path": big, "from": 801, "to": 900, "saw": "third"}],
                          "docs": [{"api": "Node.add_child", "source": "context7 /godotengine/godot-docs", "says": "main thread only"}],
                          "problems": [{"where": small + ":3", "what": "w1", "cause": "c1", "symptom": "s", "fix": "change line 3"},
                                       {"where": big + ":500", "what": "w2", "cause": "c2"}],
                          "note": "go on"})
            check("a whole batch is recorded", ledger("record", good), 0)
            now = state("t")
            check("flow --after puts the file in run order", [os.path.basename(e["path"]) for e in now["files"]],
                  ["small.txt", "late.txt", "big.txt"])
            check("the batch's problems", [p["id"] for p in now["problems"]], ["P1", "P2"])
            check("a problem keeps what its fix will be", now["problems"][0]["fix"], "change line 3")
            check("the batch's documentation note", [d["api"] for d in now["docs"]], ["Node.add_child"])
            with open(os.path.join(traces, "t", "notes.md"), encoding="utf-8") as f:
                flow_file = f.read()
            check("the flow is on disk, whole, after the command: notes, order and documentation",
                  ("first" in flow_file, "Node.add_child" in flow_file, flow_file.index("small.txt") < flow_file.index("big.txt")),
                  (True, True, True))
            check("a documentation note without what it says", ledger("record", batch({"docs": [{"api": "x", "source": "y", "says": ""}]})), 1)
            check("a file with a problem on it stays on the flow", ledger("off", small, "--why", "x"), 1)
            check("a file no call reaches leaves the flow, with why",
                  (ledger("off", late, "--why", "named alike, never called"), [os.path.basename(e["path"]) for e in state("t")["files"]],
                   state("t")["off"][0]["why"]), (0, ["small.txt", "big.txt"], "named alike, never called"))
            ledger("flow", late, "--why", "called by small", "--after", small)
            check("a batch from stdin", ledger("record", "-", stdin=json.dumps({"note": "from stdin"})), 0)
            check("an unknown key in a batch", ledger("record", batch({"read": []})), 1)

            # ---- a file changed after it was read is read again
            tool_read(late)
            with open(late, "a", encoding="utf-8") as f:
                f.write("line 21\n")
            check("a file changed since the Read", ledger("read", late, "1", "21", "--saw", "x"), 1)
            tool_read(late)
            check("read again", ledger("read", late, "1", "21", "--saw", "twenty-one lines"), 0)

            # ---- problems, causes, fixes: nothing is asked before a fix
            tool_read(late)
            check("close with open problems", ledger("close"), 1)
            check("fix one problem", ledger("fix", "P1", "--how", "changed it"), 0)
            check("a cause groups problems", ledger("cause", "add", "--what", "one decision", "--owner", "big.txt", "--plan",
                                                    "change it", "--problems", "P2"), 0)
            check("fix a cause closes its problems", ledger("fix", "C1", "--how", "changed the decision", "--commit", "abc"), 0)
            check("both problems are fixed", [p["status"] for p in state("t")["problems"]], ["fixed", "fixed"])
            check("fix of what does not exist", ledger("fix", "P9", "--how", "h"), 1)
            check("export", ledger("export"), 0)
            check("export wrote the report and the notes",
                  [os.path.exists(os.path.join(traces, "t", n)) for n in ("report.md", "notes.md")], [True, True])
            check("close without a photo of the game", ledger("close"), 1)
            shots = os.path.join(traces, "t", "photos")
            os.makedirs(shots)
            open(os.path.join(shots, "01-street_near.png"), "wb").close()
            with open(os.path.join(shots, "analysis.md"), "w", encoding="utf-8") as f:
                f.write("# another photo")
            check("close with a photo nobody analysed", ledger("close"), 1)
            with open(os.path.join(shots, "analysis.md"), "w", encoding="utf-8") as f:
                f.write("## 01-street_near: B1 no body in another: the three cars of crop r2c2 stand apart")
            check("close with every photo analysed", ledger("close"), 0)
            check("resume --hook after the close is silent", (ledger("resume", "--hook"), ledger.out), (0, ""))

            # ---- reuse of a reading by another trace
            check("open without reuse", ledger("open", "plain", "--request", "second"), 0)
            ledger("flow", small, "--why", "again")
            check("carry in a trace without reuse", ledger("carry", small, "--saw", "x"), 1)
            check("open with reuse", ledger("open", "again", "--request", "third", "--reuse"), 0)
            ledger("flow", small, late, "--why", "again")
            check("next names the earlier reading", (ledger("next"), "read in trace t" in ledger.out), (0, True))
            check("notes --prior shows it", (ledger("notes", small, "--prior"), "fifteen lines" in ledger.out), (0, True))
            check("carry an unchanged file", ledger("carry", small, "--saw", "nothing of it touches this symptom"), 0)
            with open(late, "a", encoding="utf-8") as f:
                f.write("line 22\n")
            check("carry a file changed since", ledger("carry", late, "--saw", "x"), 1)

            # ---- the flow found by script: the repository's tracer on its own small fixture
            fixture = 'tools/analysis/ftrace/tests/fixtures/notes_app'
            if os.path.isdir(os.path.join(root, fixture)):
                check('open for a route', ledger('open', 'route', '--request', 'storing a note'), 0)
                check('trace with a seed nothing matches', ledger('trace', '--seed', 'NoSuch::thing', '--include', fixture), 1)
                check('trace --dry records nothing', (ledger('trace', '--seed', 'NotesApi::store_note', '--include', fixture, '--dry'),
                                                      len(state('route')['files'])), (0, 0))
                check('trace puts the route on the flow', ledger('trace', '--seed', 'NotesApi::store_note', '--include', fixture), 0)
                on_flow = [e['path'] for e in state('route')['files']]
                check('the seed file and what it runs are on the flow, each once',
                      (any(p.endswith('native/notes_api.cpp') for p in on_flow), any(p.endswith('native/note_book.cpp') for p in on_flow),
                       len(on_flow) == len(set(on_flow)), len(on_flow) >= 3), (True, True, True, True))
                check('every file of the route says why', all(e['why'] for e in state('route')['files']), True)
                check('next names the functions on the route', (ledger('next'), 'on the route:' in ledger.out), (0, True))
                store_note = next(p for p in on_flow if p.endswith('native/notes_api.cpp'))
                ledger('problem', store_note + ':20', '--what', 'w', '--cause', 'c')
                ledger('cause', 'add', '--what', 'd', '--owner', 'o', '--plan', 'p', '--problems', 'P1')
                check('the fix card names the function, its lines and its callers',
                      (ledger('card', 'C1', '--include', fixture), 'Where to edit: 1 function(s)' in ledger.out, ', lines ' in ledger.out,
                       'called by' in ledger.out, os.path.exists(os.path.join(traces, 'route', 'card-C1.md'))), (0, True, True, True, True))
                check('a card of a cause that does not exist', ledger('card', 'C9'), 1)
                check('writers of a state name the functions that write it',
                      (ledger('writers', 'NoteBook::notes', '--rule', 'a note is never lost', '--include', fixture, '--dry'),
                       'NoteBook::add' in ledger.out, 'NoteBook::clear' in ledger.out), (0, True, True))
                check('writers of a state that does not exist', ledger('writers', 'NoSuch::thing', '--rule', 'r', '--include', fixture), 1)

            selftest_guard.run(check, hook, tool_read, ledger, edited, files, big, root, work, traces, reference, late)

            check("rules lists the world's rules", (ledger("rules"), "B1" in ledger.out, "in the code:" in ledger.out), (0, True, True))

            # ---- every answer under the limit, on a ledger far larger than a real one
            made = {"slug": "huge", "request": "r " * 2000, "opened": "2026-01-01 00:00", "note": "n " * 2000,
                    "read_gate": None, "closed": None,
                    "files": [{"path": f"native/src/area{i % 37}/sub{i % 5}/file{i}.cpp", "lines": 300,
                               "why": "why " * 30, "ranges": [[1, 300]] if i % 3 else [],
                               "saw": [{"from": 1, "to": 300, "text": "what this chunk does " * 12}] if i % 3 else []}
                              for i in range(1000)],
                    "problems": [{"id": f"P{i + 1}", "path": f"native/src/area{i % 37}/sub{i % 5}/file{i}.cpp",
                                  "line": 10, "what": "what " * 40, "cause": "cause " * 40, "symptom": "symptom " * 20,
                                  "status": "open", "how": "", "commit": ""} for i in range(200)]}
            os.makedirs(os.path.join(traces, "huge"))
            with open(os.path.join(traces, "huge", "ledger.json"), "w", encoding="utf-8") as f:
                json.dump(made, f)
            check("switch to the large ledger (written before causes existed)", ledger("switch", "huge"), 0)
            for name, args in (("resume", ["resume"]), ("resume --hook", ["resume", "--hook"]), ("status", ["status"]),
                               ("next", ["next"]), ("notes of everything", ["notes", "native"]),
                               ("notes continued", ["notes", "native", "--from", "400"]),
                               ("problems", ["problems"]), ("problems continued", ["problems", "--from", "150"]),
                               ("close", ["close"])):
                ledger(*args)
                check(f"{name} stays under the limit and says something",
                      0 < len(ledger.out) <= views.OUTPUT_CAP, True)
            ledger("resume")
            check("resume names the request, the next batch and the problems",
                  all(word in ledger.out for word in ("REQUEST:", "DO NOW:", "PROBLEMS:", "problems --from")), True)
            ledger("notes", "native")
            check("a cut listing says where to continue", "Continue: `notes native --from" in ledger.out, True)
        finally:
            for path in (small, big, late, edited):
                os.remove(path)
            os.rmdir(files)
    print(f"FAILURES: {failures}")
    sys.exit(1 if failures or not checks else 0)


if __name__ == "__main__":
    main()
