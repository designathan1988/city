"""Gate self-test: every rule runs on its fixture repositories (tools/gates/tests/fixtures/<case>/repo) and must report
exactly the problems its case.json expects; every rule needs a passing case and every problem id a failing one, or the
self-test fails. Fixtures run uncached and never enter the real scope."""
import json
import os

import registry
from context import Cache, Context
from run_gates import load_config, run_rules
from scope import excluded

GATES = os.path.dirname(os.path.abspath(__file__))
FIXTURES = os.path.join(GATES, "tests", "fixtures")


def _walk(repo):
    out = []
    for dirpath, _dirs, names in os.walk(repo):
        for n in names:
            out.append(os.path.relpath(os.path.join(dirpath, n), repo).replace(os.sep, "/"))
    return sorted(out)


def case_context(case_dir, base_rules, base_owners):
    """(case, the uncached Context its rule runs in): the case's repo, the real rules with the case's whole top-level entries
    in their place, its changed lines."""
    with open(os.path.join(case_dir, "case.json"), encoding="utf-8") as fh:
        case = json.load(fh)
    rules = dict(base_rules)
    rules.update(case.get("overrides", {}))  # a case replaces whole top-level entries of rules.json
    repo = os.path.join(case_dir, "repo")
    listed = _walk(repo)
    files = [f for f in listed if not excluded(f, rules["exclude_dirs"])]
    changed = {p: (None if v == "all" else set(v)) for p, v in case.get("changed", {}).items()}
    return case, Context(repo, files, listed, rules, case.get("owners", base_owners), changed, Cache(repo, enabled=False),
                         before_root=os.path.join(case_dir, "before"))


def run_case(case_dir, base_rules, base_owners, rules_by_name):
    """(rule name, expected failure ids, list of mismatch messages) for one fixture case."""
    case, ctx = case_context(case_dir, base_rules, base_owners)
    rule = next((r for r in registry.all_rules(ctx.rules) if r.name == case["rule"]), None)
    if rule is None or case["rule"] not in rules_by_name:
        return case["rule"], set(), [f"unknown rule '{case['rule']}'"]
    files = ctx.files
    got = sorted(f"{gid} {where}" for gid, where, _msg in run_rules(ctx, files, [rule]))
    want = sorted(case["expect"])
    issues = []
    if got != want:
        issues.append(f"expected {want}, got {got}")
    stray = {g.split(" ", 1)[0] for g in got} - set(rule.ids)
    if stray:
        issues.append(f"ids outside the rule: {sorted(stray)}")
    return case["rule"], {w.split(" ", 1)[0] for w in want}, issues


def main():
    base_rules, base_owners = load_config(os.path.abspath(os.path.join(GATES, "..", "..")))
    rules_by_name = {r.name: r for r in registry.all_rules(base_rules)}
    passing, failing_ids, failures = set(), set(), []
    cases = sorted(d for d in os.listdir(FIXTURES) if os.path.isdir(os.path.join(FIXTURES, d))) if os.path.isdir(FIXTURES) else []
    for d in cases:
        try:
            name, ids, issues = run_case(os.path.join(FIXTURES, d), base_rules, base_owners, rules_by_name)
        except Exception as e:  # noqa: BLE001  a crashing case is a failed case
            name, ids, issues = d, set(), [f"crashed: {e!r}"]
        if issues:
            failures += [f"{d}: {i}" for i in issues]
            print(f"FAIL {d}")
        else:
            print(f"ok   {d}")
            if ids:
                failing_ids |= ids
            else:
                passing.add(name)
    for name, r in sorted(rules_by_name.items()):
        if name not in passing:
            failures.append(f"rule '{name}' has no passing fixture")
        for gid in r.ids:
            if gid not in failing_ids:
                failures.append(f"rule '{name}': id {gid} has no failing fixture (the rule is not shown able to fail)")
    if not cases:
        failures.append(f"no fixtures under {FIXTURES}")
    for f in failures:
        print(f"[SELFTEST] {f}")
    print(f"Self-test {'FAILED' if failures else 'OK'}: {len(cases)} case(s), {len(rules_by_name)} rule(s)")
    return 1 if failures else 0
