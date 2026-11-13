"""What enters the register from a tool's own output, deduplicated against the problems already there: the gates' standing
findings (`problems import-gates`, one problem per rule and file, origin gate:<RULE>:<file>). Every problem is added through
the register's one writer (problems.Register.add); nothing here writes the file."""
import os
import statistics
import sys
from datetime import datetime

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import areas  # noqa: E402
from atlas import problems as register  # noqa: E402

STAMPED = "%Y-%m-%dT%H:%M"
SECONDS_PER_MINUTE = 60

UNPREFIXED = "(no prefix)"  # an origin written without a kind before a colon


def gate_findings(reg, findings, when=None):
    """One open problem per (rule, file) of the findings that no problem of the register names yet; the new ones returned."""
    named = {o for p in reg.problems for o in (p.get("origin") or [])}
    groups = {}
    for f in findings:
        groups.setdefault((f["rule"], f["file"]), []).append(f)
    added = []
    for (rule, file), these in sorted(groups.items()):
        origin = register.gate_origin(rule, file)
        if origin in named:
            continue
        lines = sorted({f["line"] for f in these if f.get("line")})
        at = f" at line{'s' if len(lines) > 1 else ''} {', '.join(map(str, lines))}" if lines else ""
        added.append(reg.add(
            title=f"{rule} in {file}",
            symptom=f"the gate {rule} reports {len(these)} finding(s){at}: {these[0]['message']}",
            reading=f"{file}{at}, against the rule {rule} (tools/gates/registry.py, CLAUDE.md)",
            owner=file, area=areas.area_of(file), kind="architecture",
            criterion=f"python tools/gates/run_gates.py --file {file} reports no {rule}",
            origin=[origin], opened_at=when))
    return added


def _minutes(a, b):
    try:
        return (datetime.strptime(b, STAMPED) - datetime.strptime(a, STAMPED)).total_seconds() / SECONDS_PER_MINUTE
    except (TypeError, ValueError):  # a date without its time (a problem brought from an older record): not measured
        return None


def stats(reg, problems=None):
    """Counts by area, status and origin, and the median minutes from a report (opened) to its located cause (traced), of
    the problems given (the register's, by default). A problem added with its cause is traced in the minute it is opened: its
    span measures no search and is counted apart (`traced when added`), not as a zero in the median."""
    problems = reg.problems if problems is None else problems

    def count(key):
        out = {}
        for p in problems:
            k = key(p)
            out[k] = out.get(k, 0) + 1
        return dict(sorted(out.items()))
    spans = [m for m in (_minutes(p.get("opened"), p.get("traced")) for p in problems) if m is not None and m >= 0]
    searched = [m for m in spans if m > 0]
    return {"problems": len(problems), "by_area": count(lambda p: p.get("area")),
            "by_status": count(lambda p: p.get("status")),
            "by_origin": count(lambda p: origin_kind((p.get("origin") or [""])[0])),
            "minutes_to_cause": {"measured": len(searched), "median": statistics.median(searched) if searched else None},
            "traced when added": len(spans) - len(searched)}


def origin_kind(origin):
    """What an origin is, by the prefix before its colon (gate, trace, run, block...); one written without a prefix is said
    so, not a kind of its own."""
    prefix, colon, _ = origin.partition(":")
    return prefix if colon and prefix else UNPREFIXED
