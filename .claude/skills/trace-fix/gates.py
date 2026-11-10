"""The gates of the trace-fix ledger, each able to fail: `gate read` (the whole flow read), `gate plan` (every open problem under a cause that
says its owner, its correction, the outside APIs it touches and what it can break) and `gate close` (every problem fixed)."""
import sys

import store
import views
from store import FIXED, OPEN


def cmd_gate(args):
    state = store.load()
    errors = []
    open_problems = [p for p in state["problems"] if p["status"] == OPEN]
    if args.which == "read":
        if not state["files"]:
            errors.append("no file on the flow: an empty flow fails")
        errors += [f"unread: {e['path']} lines {a}-{b}" for e in state["files"] for a, b in store.gaps(e)]
        if not errors:
            state["read_gate"] = state.get("read_gate") or store.now()
    elif args.which == "plan":
        if not state.get("read_gate"):
            errors.append("the read gate did not pass")
        if not state["problems"]:
            errors.append("no problem recorded: the symptom of the request has a cause in the code")
        known = {c["id"] for c in state["causes"]}
        errors += [f"without a cause: {p['id']} {p['path']}:{p['line']} {views.clip(p['what'], 90)}"
                   for p in open_problems if p["cause_id"] not in known]
        for c in state["causes"]:
            if any(p["cause_id"] == c["id"] for p in open_problems) and not (c["owner"] and c["plan"]):
                errors.append(f"{c['id']} has no owner or no plan")
            if any(p["cause_id"] == c["id"] for p in open_problems) and not (c.get("api") and c.get("risks")):
                errors.append(f"{c['id']} does not say the outside APIs its fix touches (--api, or none) or what the fix can break and its guards (--risks)")
        if not errors:
            state["plan_gate"] = store.now()
    else:
        if not state.get("read_gate"):
            errors.append("the read gate did not pass")
        if not state.get("plan_gate"):
            errors.append("the plan gate did not pass")
        if not any(c["status"] == FIXED for c in state["causes"]):
            errors.append("no fixed cause: the symptom of the request has a cause in the code, plan and fix it")
        errors += [f"open: {p['id']} {p['path']}:{p['line']} {views.clip(p['what'], 90)}" for p in open_problems]
        if not errors:
            state["closed"] = store.now()
    store.save(state)
    out = views.Out()
    for i, error in enumerate(errors):
        if not out.add("FAIL " + error):
            break
    out.add(f"gate {args.which}: {'FAILED, ' + str(len(errors)) + ' item(s)' if errors else 'passed'}")
    out.flush(f"gate {args.which}: FAILED, {len(errors)} item(s); the first ones are above.")
    sys.exit(1 if errors else 0)
