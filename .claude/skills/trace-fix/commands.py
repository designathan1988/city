"""What each command of the trace-fix ledger does: it loads the active trace, applies the operation (operations.py),
saves, and answers through the bounded views (views.py). 
"""
import copy
import json
import os
import sys

import photos
import proof
import sessions
import store
import trace_files
import views
from operations import do_doc, do_flow, do_link, do_off, do_plan, do_problem, do_read, keep_notes
from store import DROPPED, FIXED, OPEN, Refused, fail


def run(operation, *args, **kwargs):
    try:
        return operation(*args, **kwargs)
    except Refused as refusal:
        fail(str(refusal))


def cmd_open(args):
    if os.path.exists(store.ledger_path(args.slug)):
        fail(f"trace {args.slug} exists: `switch {args.slug}` and `resume`")
    state = store.new_state(args.slug, args.request, args.reuse)
    sessions.bind(state, sessions.current_session())
    store.save(state)
    store.set_active(args.slug)
    print(f"opened {args.slug}. Next: find the entry point of the symptom and add it with `flow`.")


def cmd_switch(args):
    state = store.load(args.slug)
    if sessions.bind(state, sessions.current_session()):
        store.save(state)
    store.set_active(args.slug)
    print("active:", args.slug)


def cmd_list(_args):
    folder = store.TRACES
    slugs = sorted(d for d in os.listdir(folder) if os.path.exists(store.ledger_path(d))) if os.path.isdir(folder) else []
    if not slugs:
        print("no traces")
    for slug in slugs:
        state = store.load(slug)
        mark = "*" if slug == store.active_slug() else " "
        print(f"{mark} {slug}: {store.phase(state)} | {views.clip(state['request'], 90)}")


def cmd_flow(args):
    state = store.load()
    added = run(do_flow, state, args.paths, args.why, args.after)
    store.save(state)
    out = views.Out()
    for path in added:
        if not out.add(f"+ {path}"):
            break
    out.add(f"{len(added)} file(s) added, {len(state['files'])} on the flow.")
    out.flush(f"{len(added)} file(s) were added; `status` shows the flow by folder.")


def cmd_off(args):
    state = store.load()
    entry = run(do_off, state, args.path, args.why)
    store.save(state)
    print(f"{entry['path']} is off the flow ({len(state['files'])} file(s) on it); kept in the flow file with why.")


def cmd_next(_args):
    out = views.Out()
    views.next_step(store.load(), out)
    out.flush()


def cmd_read(args):
    state = store.load()
    entry = run(do_read, state, args.path, args.start, args.end, args.saw, args.called_by, args.state)
    store.save(state)
    keep_notes(state, [entry])
    print(f"{entry['path']}: {store.lines_read(entry)}/{entry['lines']} lines read.")


def cmd_carry(args):
    state = store.load()
    if not state.get("reuse"):
        fail("this trace reads everything anew: carrying earlier readings over is chosen at `open --reuse`")
    entry = store.find_file(state, args.path)
    if not entry:
        fail(f"{store.rel(args.path)} is not on the flow")
    note = proof.prior_note(entry["path"], state["slug"])
    if not note:
        fail(f"no other trace read {entry['path']} as it is now: read it")
    entry["lines"] = store.line_count(entry["path"])
    entry["ranges"] = [[1, entry["lines"]]]
    entry["saw"].append({"from": 1, "to": entry["lines"], "text": args.saw.strip(), "t": store.now(),
                         "carried": note["trace"]})
    store.save(state)
    print(f"{entry['path']}: carried over from trace {note['trace']}.")


def cmd_record(args):
    """A batch from a JSON file (or - for stdin): flow, reads, docs, problems and a note, recorded whole or not at all."""
    state = store.load()
    try:
        text = sys.stdin.read() if args.file == "-" else open(args.file, encoding="utf-8").read()
        batch = json.loads(text)
    except (OSError, json.JSONDecodeError) as error:
        fail(f"the batch is not readable JSON: {error}")
    unknown = set(batch) - {"flow", "reads", "docs", "problems", "note"} if isinstance(batch, dict) else {"not an object"}
    if unknown:
        fail(f"a batch is an object with flow, reads, docs, problems and note; unknown: {sorted(unknown)}")
    if (batch.get("reads") or batch.get("problems")) and not str(batch.get("note") or "").strip():
        fail("a batch with reads or problems ends with its `note`: the next action, as you would tell it to someone taking over now "
             "(it is what a compaction leaves of where you are)")
    trial, errors, touched, pids, added = copy.deepcopy(state), [], [], [], []

    def attempt(label, item, call):
        try:
            return call(item)
        except Refused as refusal:
            errors.append(f"{label}: {refusal}")
        except (KeyError, TypeError, ValueError) as error:
            errors.append(f"{label} {json.dumps(item, ensure_ascii=False)[:120]}: missing or wrong field {error}")
        return None

    for item in batch.get("flow", []):
        added += attempt("flow", item, lambda i: do_flow(trial, i["paths"], i["why"], i.get("after"))) or []
    for item in batch.get("reads", []):
        entry = attempt("read", item, lambda i: do_read(trial, i["path"], i["from"], i["to"], i.get("saw"), i.get("called_by"), i.get("state")))
        if entry:
            touched.append(entry)
    for item in batch.get("docs", []):
        attempt("doc", item, lambda i: do_doc(trial, i["api"], i["source"], i["says"]))
    for item in batch.get("problems", []):
        pid = attempt("problem", item, lambda i: do_problem(
            trial, i["where"], i["what"], i["cause"], i.get("symptom", ""), i.get("of", ""), i.get("fix", "")))
        if pid:
            pids.append(pid)
    if errors:
        out = views.Out()
        for error in errors:
            out.add("REFUSED " + error)
        out.add(f"Nothing of this batch was recorded ({len(errors)} refusal(s)). Correct it and record again.")
        out.flush("More refusals than fit; the first ones are above.")
        sys.exit(1)
    if batch.get("note"):
        trial["note"] = f"[{store.now()}] {batch['note']}"
    store.save(trial)
    keep_notes(trial, touched)
    out = views.Out()
    out.add(f"recorded: {len(touched)} chunk(s), {len(batch.get('docs', []))} documentation note(s), {len(pids)} problem(s)"
            + (f" ({pids[0]}-{pids[-1]})" if pids else "")
            + f", {len(added)} file(s) added to the flow.")
    views.next_step(trial, out)
    views.owed_block(out, sessions.current_session())
    out.flush()


def cmd_problem(args):
    state = store.load()
    pid = run(do_problem, state, args.where, args.what, args.cause, args.symptom, args.of, args.fix)
    store.save(state)
    entry = store.find_file(state, state["problems"][-1]["path"])
    keep_notes(state, [entry])
    print(f"{pid} recorded at {state['problems'][-1]['path']}:{state['problems'][-1]['line']}")


def cmd_plan(args):
    state = store.load()
    problem = run(do_plan, state, args.id, args.fix)
    store.save(state)
    print(f"{problem['id']}: fix planned.")


def cmd_cause_add(args):
    state = store.load()
    cid = f"C{len(state['causes']) + 1}"
    state["causes"].append({"id": cid, "what": args.what.strip(), "owner": args.owner.strip(), "plan": args.plan.strip(),
                            "status": OPEN, "how": "", "commit": "", "made": store.now()})
    run(do_link, state, cid, args.problems or [])
    store.save(state)
    print(f"{cid} recorded with {len(args.problems or [])} problem(s).")


def cmd_cause_link(args):
    state = store.load()
    cause = run(do_link, state, args.id, args.problems)
    store.save(state)
    print(f"{cause['id']}: {sum(1 for p in state['problems'] if p['cause_id'] == cause['id'])} problem(s).")


def cmd_cause_set(args):
    state = store.load()
    cause = run(store.cause_of, state, args.id)
    for key in ("what", "owner", "plan"):
        if getattr(args, key):
            cause[key] = getattr(args, key).strip()
    store.save(state)
    print(f"{cause['id']} updated.")


def cmd_fix(args):
    """Marks a cause (with its open problems) or one problem fixed. Nothing is asked here: the fix is in the code, the test is at the end."""
    state = store.load()
    how, commit = args.how.strip(), args.commit or ""
    if args.id[:1].lower() == "c":
        cause = run(store.cause_of, state, args.id)
        cause.update(status=FIXED, how=how, commit=commit, closed=store.now())
        mine = [p for p in state["problems"] if p["cause_id"] == cause["id"] and p["status"] == OPEN]
    else:
        mine = [run(store.problem_of, state, args.id)]
    for p in mine:
        p.update(status=FIXED, how=how, commit=commit, closed=store.now())
    store.save(state)
    print(f"{args.id.upper()} fixed: {len(mine)} problem(s) closed. Open: {sum(1 for q in state['problems'] if q['status'] == OPEN)}")


def cmd_drop(args):
    state = store.load()
    p = run(store.problem_of, state, args.id)
    p.update(status=DROPPED, how=args.why.strip(), closed=store.now())
    store.save(state)
    print(f"{p['id']} dropped: {args.why}")


def cmd_rules(_args):
    with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "rules.json"), encoding="utf-8") as f:
        rules = json.load(f)["rules"]
    out = views.Out()
    for r in rules:
        out.add(f"{r['id']}  {r['rule']}")
        out.add(f"      in a photo: {r['photo']}")
        out.add(f"      in the code: {r['state']}")
    out.flush()


def cmd_close(_args):
    """Closes the trace once no problem is open and the game was photographed and every photo analysed; says what is missing."""
    state = store.load()
    left = [p for p in state["problems"] if p["status"] == OPEN]
    if left:
        out = views.Out()
        for p in left:
            if not out.add(f"open: {p['id']} {p['path']}:{p['line']} {views.clip(p['what'], 90)}"):
                break
        out.add(f"{len(left)} problem(s) still open: fix them, or `drop` what the code proved not to be one.")
        out.flush(f"{len(left)} problem(s) still open.")
        sys.exit(1)
    if getattr(_args, "no_photos", None):
        state["no_photos"] = _args.no_photos.strip()
    else:
        missing = photos.not_looked_at(state)
        if missing:
            fail(missing)
    state["closed"] = store.now()
    store.save(state)
    print(f"trace {state['slug']} closed.")


def cmd_note(args):
    state = store.load()
    state["note"] = f"[{store.now()}] {args.text}"
    store.save(state)
    print("noted")


def cmd_notes(args):
    views.notes(store.load(), args.where, args.start, args.prior)


def cmd_problems(args):
    views.problems(store.load(), args.all, args.start)



def cmd_export(_args):
    trace_files.export(store.load())
