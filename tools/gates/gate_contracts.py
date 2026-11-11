"""CONTRACT_UNGUARDED: no rule of CLAUDE.md without a guard. Every item of a rule section of CLAUDE.md (a bullet, a numbered
step or a paragraph; a table is no item) carries a marker [c:<id>]; every marker names an entry of tools/gates/contracts.json
and every entry is marked once; every entry has guards, and every guard is real: a gate a rule the gates declare (its name or
one of its ids), a test or a
tool a file of the repository, a hook a file of .claude/skills/trace-fix/, a runtime probe a name the store's API declares;
conduct, for a rule no mechanism can hold, says why no mechanism can."""
import json
import os
import re

import registry

CLAUDE_MD = "CLAUDE.md"
CONTRACTS = "tools/gates/contracts.json"
STORE_API = "project/game/store/store_api.gd"
HOOKS = ".claude/skills/trace-fix/"
MARK = re.compile(r"\[c:([a-z0-9-]+)\]")
ITEM_START = re.compile(r"^(- |\d+\. )")
KINDS = frozenset({"gate", "test", "tool", "hook", "runtime", "conduct"})


def inputs(ctx, path):
    return path in (CLAUDE_MD, CONTRACTS, STORE_API)


def extra(ctx):
    """What a verdict reads besides its inputs: the ids the gates declare and which guard files are there."""
    try:
        with open(os.path.join(ctx.root, CONTRACTS), encoding="utf-8") as fh:
            guards = [g for c in json.load(fh).get("contracts", []) for g in c.get("guards", [])]
    except (OSError, json.JSONDecodeError):
        guards = []
    files = sorted({g["name"] for g in guards if g.get("kind") in ("test", "tool", "hook")})
    return [sorted(i for r in registry.all_rules(ctx.rules) for i in (r.name, *r.ids)),
            [f for f in files if os.path.isfile(os.path.join(ctx.root, f))]]


def items(text, sections):
    """[(first line, text)] of every item of the rule sections: a bullet or a numbered step with its continued lines, or a
    paragraph; the text before the first heading is the section ''."""
    out, section, block = [], "", []

    def flush():
        current = None  # [first line, text, a bullet or step?]
        for n, line in block:
            if line.startswith("|"):  # a table row: no item, and it ends the one before
                current = None
                continue
            continued = current is not None and (line[:1].isspace() or not current[2]) and not ITEM_START.match(line)
            if continued:
                current[1] += "\n" + line
            else:
                current = [n, line, bool(ITEM_START.match(line))]
                out.append(current)
        block.clear()

    for n, line in enumerate(text.splitlines(), 1):
        if line.startswith("#"):
            flush()
            section = line.lstrip("#").strip() if line.startswith("## ") else section
            continue
        if not line.strip():
            flush()
            continue
        if any(section == s or section.startswith(s + " ") for s in sections) or (section == "" and "" in sections):
            block.append((n, line))
    flush()
    return [(n, t) for n, t, _step in out]


def _guard_problem(ctx, guard, gate_ids, probes):
    kind, name = guard.get("kind"), guard.get("name", "")
    if kind not in KINDS:
        return f"a guard of kind '{kind}', none of {sorted(KINDS)}"
    if kind == "gate" and name not in gate_ids:
        return f"the gate '{name}' is no rule the gates declare (by its name or one of its ids)"
    if kind in ("test", "tool") and not os.path.isfile(os.path.join(ctx.root, name)):
        return f"the {kind} '{name}' is no file of the repository"
    if kind == "hook" and (not name.startswith(HOOKS) or not os.path.isfile(os.path.join(ctx.root, name))):
        return f"the hook '{name}' is no file of {HOOKS}"
    if kind == "runtime" and name not in probes:
        return f"the runtime probe '{name}' is no name {STORE_API} declares"
    if kind == "conduct" and not guard.get("why"):
        return f"the conduct '{name}' does not say why no mechanism can hold its rule"
    return None


def check_contracts(ctx, err):
    try:
        with open(os.path.join(ctx.root, CONTRACTS), encoding="utf-8") as fh:
            data = json.load(fh)
        with open(os.path.join(ctx.root, CLAUDE_MD), encoding="utf-8") as fh:
            text = fh.read()
    except (OSError, json.JSONDecodeError) as e:
        err("CONTRACT_UNGUARDED", CONTRACTS, f"cannot be read: {e}")
        return
    entries = {c["id"]: c for c in data.get("contracts", [])}
    marked = {}
    for n, line in enumerate(text.splitlines(), 1):
        for m in MARK.finditer(line):
            marked.setdefault(m.group(1), []).append(n)
    for n, body in items(text, data.get("sections", [])):
        if not MARK.search(body):
            err("CONTRACT_UNGUARDED", f"{CLAUDE_MD}:{n}", "a rule with no marker [c:<id>]: name its contract in contracts.json "
                "and its guards")
    for cid, lines in sorted(marked.items()):
        if cid not in entries:
            err("CONTRACT_UNGUARDED", f"{CLAUDE_MD}:{lines[0]}", f"the marker [c:{cid}] names no entry of {CONTRACTS}")
        elif len(lines) > 1:
            err("CONTRACT_UNGUARDED", f"{CLAUDE_MD}:{lines[1]}", f"the marker [c:{cid}] stands more than once")
    gate_ids = {i for r in registry.all_rules(ctx.rules) for i in (r.name, *r.ids)}
    probes = _probes(ctx)
    for cid, entry in sorted(entries.items()):
        if cid not in marked:
            err("CONTRACT_UNGUARDED", CONTRACTS, f"the contract '{cid}' is marked nowhere in {CLAUDE_MD}")
        if not entry.get("guards"):
            err("CONTRACT_UNGUARDED", CONTRACTS, f"the contract '{cid}' has no guard")
        for guard in entry.get("guards", []):
            wrong = _guard_problem(ctx, guard, gate_ids, probes)
            if wrong:
                err("CONTRACT_UNGUARDED", CONTRACTS, f"the contract '{cid}': {wrong}")


def _probes(ctx):
    """Every word of the store's API (the command and query names it declares)."""
    try:
        with open(os.path.join(ctx.root, STORE_API), encoding="utf-8") as fh:
            return set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", fh.read()))
    except OSError:
        return set()
