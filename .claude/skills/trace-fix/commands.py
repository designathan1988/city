"""What each command of the trace-fix ledger does: it loads the active trace, applies the operation (operations.py),
saves, and answers through the bounded views (views.py). The gates are gates.py.
"""
import copy
import json
import os
import sys

import evidence
import proof
import store
import views
from operations import do_flow, do_link, do_problem, do_read, keep_notes
from store import DROPPED, FIXED, OPEN, Refused, fail


def run(operation, *args, **kwargs):
    try:
        return operation(*args, **kwargs)
    except Refused as refusal:
        fail(str(refusal))


def cmd_open(args):
    if os.path.exists(store.ledger_path(args.slug)):
        fail(f"trace {args.slug} exists: `switch {args.slug}` and `resume`")
    store.save(store.new_state(args.slug, args.request, args.reuse))
    store.set_active(args.slug)
    print(f"opened {args.slug}. Next: find the entry point of the symptom and add it with `flow`.")


def cmd_switch(args):
    store.load(args.slug)
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


def cmd_next(_args):
    out = views.Out()
    views.next_step(store.load(), out)
    out.flush()


def cmd_read(args):
    state = store.load()
    entry = run(do_read, state, args.path, args.start, args.end, args.saw)
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
    """A batch from a JSON file (or - for stdin): flow, reads, problems and a note, recorded whole or not at all."""
    state = store.load()
    try:
        text = sys.stdin.read() if args.file == "-" else open(args.file, encoding="utf-8").read()
        batch = json.loads(text)
    except (OSError, json.JSONDecodeError) as error:
        fail(f"the batch is not readable JSON: {error}")
    unknown = set(batch) - {"flow", "reads", "problems", "note"} if isinstance(batch, dict) else {"not an object"}
    if unknown:
        fail(f"a batch is an object with flow, reads, problems and note; unknown: {sorted(unknown)}")
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
        entry = attempt("read", item, lambda i: do_read(trial, i["path"], i["from"], i["to"], i["saw"]))
        if entry:
            touched.append(entry)
    for item in batch.get("problems", []):
        pid = attempt("problem", item, lambda i: do_problem(
            trial, i["where"], i["what"], i["cause"], i.get("symptom", ""), i.get("of", "")))
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
    out.add(f"recorded: {len(touched)} chunk(s), {len(pids)} problem(s)" + (f" ({pids[0]}-{pids[-1]})" if pids else "")
            + f", {len(added)} file(s) added to the flow.")
    views.next_step(trial, out)
    out.flush()


def cmd_problem(args):
    state = store.load()
    pid = run(do_problem, state, args.where, args.what, args.cause, args.symptom, args.of)
    store.save(state)
    entry = store.find_file(state, state["problems"][-1]["path"])
    keep_notes(state, [entry])
    print(f"{pid} recorded at {state['problems'][-1]['path']}:{state['problems'][-1]['line']}")


def cmd_cause_add(args):
    state = store.load()
    cid = f"C{len(state['causes']) + 1}"
    state["causes"].append({"id": cid, "what": args.what.strip(), "owner": args.owner.strip(), "plan": args.plan.strip(),
                            "api": (args.api or "").strip(), "risks": (args.risks or "").strip(),
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
    for key in ("what", "owner", "plan", "api", "risks"):
        if getattr(args, key):
            cause[key] = getattr(args, key).strip()
    store.save(state)
    print(f"{cause['id']} updated.")


def cmd_fix(args):
    state = store.load()
    if args.id[:1].lower() == "p":
        fail("a fix goes by cause, not by problem: `cause add --what --owner --plan --problems P..` (or `cause link`), "
             "`gate plan`, then `fix C<n> --how \"...\" --commit <sha>` closes every problem of that cause")
    cause = run(store.cause_of, state, args.id)
    if not (cause["owner"] and cause["plan"]):
        fail(f"{cause['id']} has no owner or no plan: `cause set {cause['id']} --owner ... --plan ...`")
    if "needs" not in cause:
        fail(f"{cause['id']} has no fix card yet: `card {cause['id']}` says where to edit and puts on the flow who calls and reads it")
    held = {p["path"] for p in state["problems"] if p["cause_id"] == cause["id"]} | set(cause["needs"])
    unread = [f"{e['path']} lines {a}-{b}" for e in state["files"] if e["path"] in held for a, b in store.gaps(e)]
    if unread:
        fail(f"{cause['id']} cannot be fixed before its own flow is read in whole (the files of its problems, their callers and "
             "readers): " + "; ".join(unread[:12]) + (f" and {len(unread) - 12} more" if len(unread) > 12 else ""))
    lacking = evidence.missing(state, cause, args)
    if lacking:
        fail(f"{cause['id']} is not accepted as fixed yet: " + " | ".join(lacking))
    cause.update(status=FIXED, how=args.how.strip(), commit=args.commit or "", closed=store.now())
    closed = 0
    for p in state["problems"]:
        if p["cause_id"] == cause["id"] and p["status"] == OPEN:
            p.update(status=FIXED, how=f"by {cause['id']}: {args.how.strip()}", commit=args.commit or "",
                     closed=store.now())
            closed += 1
    store.save(state)
    print(f"{cause['id']} fixed: {closed} problem(s) closed. "
          f"Open: {sum(1 for q in state['problems'] if q['status'] == OPEN)}")


def cmd_drop(args):
    state = store.load()
    p = run(store.problem_of, state, args.id)
    p.update(status=DROPPED, how=args.why.strip(), closed=store.now())
    store.save(state)
    print(f"{p['id']} dropped: {args.why}")


def cmd_rules(_args):
    out = views.Out()
    for r in evidence.world_rules():
        out.add(f"{r['id']}  {r['rule']}")
        out.add(f"      in a photo: {r['photo']}")
        out.add(f"      in the code: {r['state']}")
    out.flush()


def cmd_note(args):
    state = store.load()
    state["note"] = f"[{store.now()}] {args.text}"
    store.save(state)
    print("noted")


def cmd_status(_args):
    views.show(store.load())


def cmd_resume(args):
    if args.hook and not os.path.normcase(os.path.abspath(__file__)).startswith(os.path.normcase(store.ROOT) + os.sep):
        return  # a session of another project: this skill speaks only in the repository it is installed in
    slug = store.active_slug()
    if not slug or not os.path.exists(store.ledger_path(slug)):
        if not args.hook:
            print("trace-fix: no active trace.")
        return
    state = store.load(slug)
    if state.get("closed"):
        if not args.hook:
            print(f"trace-fix: last trace {slug} is closed.")
        return
    views.show(state, "trace-fix: A TRACE IS OPEN. Continue it from here (skill trace-fix); do not read again what "
                      "the ledger shows as read: `notes <file or folder>` gives what was read there.")


def cmd_notes(args):
    views.notes(store.load(), args.where, args.start, args.prior)


def cmd_problems(args):
    views.problems(store.load(), args.all, args.start)



def cmd_export(_args):
    views.export(store.load())
