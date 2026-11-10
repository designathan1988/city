"""What a fix must show before the trace-fix ledger accepts it, each checked against something on disk, never taken on word:
the official documentation was read for every outside API the cause touches (the log of documentation lookups), what
can go wrong was written down with its guard, a test passed after the last edit of the changed files (the orchestrator's
own run records), and, when a changed file can reach the screen, photos were taken after that edit and analysed.
"""
import json
import os
import re
import subprocess
import time

import store

DOCS = "docs"  # dist/trace/docs: one record a documentation lookup (docs_log.py)
NONE = "none"
# Where a change can alter what is drawn (CLAUDE.md, the project's structure).
SCREEN = ("native/src/render/", "native/src/sim/terrain/", "project/render/", "project/data/render/", "project/data/game/", "project/ui/",
          "project/assets/")
IMAGES = (".png", ".jpg", ".jpeg", ".webp")
PHOTOS_MIN = 3   # near, mid and far: fewer is not a look at the element
SEEN_MIN = 200   # characters: an analysis names what is in each photo; "looks fine" does not
WORD = re.compile(r"[A-Za-z_][A-Za-z0-9_]{2,}")


def tasks_folder():
    return os.environ.get("TRACE_FIX_TASKS") or os.path.join(store.ROOT, "dist", "tasks")


def lookups(since):
    """The text of every documentation lookup logged since a moment."""
    folder = os.path.join(store.TRACES, DOCS)
    out = []
    for name in sorted(os.listdir(folder)) if os.path.isdir(folder) else ():
        if name.endswith(".json"):
            with open(os.path.join(folder, name), encoding="utf-8") as f:
                row = json.load(f)
            if row.get("t", "") >= since:
                out.append(json.dumps(row.get("input", ""), ensure_ascii=False).lower())
    return out


def changed_files(state, cause, commit):
    files = {p["path"] for p in state["problems"] if p["cause_id"] == cause["id"]}
    if commit:
        shown = subprocess.run(["git", "-C", store.ROOT, "show", "--name-only", "--format=", commit], capture_output=True, timeout=60)
        if shown.returncode != 0:
            return files, f"commit {commit} is not in this repository"
        files |= {f for f in shown.stdout.decode("utf-8", "replace").splitlines() if f}
    return files, None


def last_edit(files):
    times = [os.path.getmtime(os.path.join(store.ROOT, f)) for f in files if os.path.isfile(os.path.join(store.ROOT, f))]
    return max(times, default=0.0)


def passed_after(test, moment):
    """Did the orchestrator record this test (any test, when none is named) passing after a moment? (dist/tasks/<run>/events.jsonl)"""
    folder = tasks_folder()
    for run in sorted(os.listdir(folder), reverse=True) if os.path.isdir(folder) else ():
        events = os.path.join(folder, run, "events.jsonl")
        if not os.path.isfile(events):
            continue
        with open(events, encoding="utf-8") as f:
            for line in f:
                e = json.loads(line)
                if e.get("event") == "finish" and e.get("status") == "PASS" and e.get("t", 0) >= moment and (
                        e.get("task") == test if test else "test" in str(e.get("task", ""))):
                    return True
    return False


def world_rules():
    """The rules of the game's world (rules.json beside this file)."""
    with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "rules.json"), encoding="utf-8") as f:
        return json.load(f)["rules"]


def unanswered(state, path):
    """Why a rule sheet is not whole: every rule of the world answered `ok`, `not in view` or with the id of a problem of this trace."""
    if not path or not os.path.isfile(path if os.path.isabs(path) else os.path.join(store.ROOT, path)):
        return ["`--checked <file.json>` is missing: a sheet with every rule of `rules` answered from the photos (ok, not in view, or P<n>)"]
    with open(path if os.path.isabs(path) else os.path.join(store.ROOT, path), encoding="utf-8") as f:
        sheet = json.load(f)
    known = {p["id"] for p in state["problems"]}
    out = []
    for rule in world_rules():
        answer = str(sheet.get(rule["id"], "")).strip()
        if answer.lower() not in ("ok", "not in view") and answer not in known:
            out.append(f"rule {rule['id']} ({rule['rule']}) is not answered: ok, not in view, or the problem recorded for it"
                       + (f" (`{answer}` is no problem of this trace)" if answer else ""))
    return out


def missing(state, cause, args):
    """Why this fix is not accepted yet: an empty list when everything it must show is on disk."""
    out = []
    api, risks = cause.get("api", "").strip(), cause.get("risks", "").strip()
    if not api:
        out.append(f"{cause['id']} does not say which outside APIs the fix touches (Godot, godot-cpp, Terrain3D, a library): "
                   f"`cause set {cause['id']} --api \"Class.method, ...\"`, or `--api none` when it touches none")
    elif api.lower() != NONE:
        seen = lookups(state["opened"])
        for name in [a.strip() for a in api.split(",") if a.strip()]:
            words = [w.lower() for w in WORD.findall(name)]
            if words and not any(words[-1] in text for text in seen):
                out.append(f"no documentation lookup names `{name}` since this trace opened: read the official documentation "
                           "(context7 `resolve-library-id` and `query-docs`, or the vendor's page) before changing code that calls it")
    if not risks:
        out.append(f"{cause['id']} does not say what the fix can break and what guards each: `cause set {cause['id']} --risks \"...\"`")
    files, problem = changed_files(state, cause, args.commit)
    if problem:
        out.append(problem)
    edited = last_edit(files)
    if not args.tests and not passed_after(None, edited):
        out.append("the orchestrator has no test PASS after the last edit of the changed files "
                   f"({time.strftime('%H:%M:%S', time.localtime(edited))}): run the tests the card lists (`python tools/tasks/cli.py run fast <scene>`)")
    for test in args.tests or ():
        if not passed_after(test, edited):
            out.append(f"the orchestrator has no PASS of `{test}` after the last edit of the changed files "
                       f"({time.strftime('%H:%M:%S', time.localtime(edited))}): run it (`python tools/tasks/cli.py run fast <scene>`)")
    if any(f.startswith(SCREEN) for f in files):
        folder = os.path.join(store.ROOT, args.photos) if args.photos and not os.path.isabs(args.photos) else args.photos
        shots = [os.path.join(base, n) for base, _, names in os.walk(folder) for n in names if n.lower().endswith(IMAGES)] if folder and os.path.isdir(folder) else []
        fresh = [s for s in shots if os.path.getmtime(s) >= edited]
        if len(fresh) < PHOTOS_MIN:
            out.append(f"the fix changes what can reach the screen and `--photos` holds {len(fresh)} photo(s) taken after the last edit "
                       f"(at least {PHOTOS_MIN}: near, mid, far; several angles, day and night)")
        if len((args.seen or "").strip()) < SEEN_MIN:
            out.append("`--seen` must say what each photo shows, element by element, and every problem seen (the skill's photo step)")
        out += unanswered(state, getattr(args, "checked", None))
    return out
