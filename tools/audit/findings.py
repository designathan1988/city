"""A finding of the game's audit and the report that groups findings by their owner.

A finding says which rule broke (`domain`, `rule`), who owns what broke (`owner`: a source place, a data key, a control, a
command: whatever the fix has to change), what was seen (`what`), how often (`count`), where (`where`: tasks, places in the
world, controls) and the first evidence line. Findings with the same domain, rule, owner and wording are one cause seen many
times: the report lists causes, most seen first, each owner once with everything that broke under it.
"""
from __future__ import annotations

import json
import re
from dataclasses import asdict, dataclass, field
from pathlib import Path

NUMBER = re.compile(r"(?<![A-Za-z0-9_.'\"])-?\d+(?:\.\d+)?(?:e-?\d+)?(?![A-Za-z_])")  # a number standing alone, never the digits of a name
WHERE_SHOWN = 6  # places listed under a cause in the written report; the JSON keeps all


@dataclass
class Finding:
    domain: str
    rule: str
    owner: str
    what: str
    count: int = 1
    where: list[str] = field(default_factory=list)
    evidence: str = ""
    expected: bool = False  # declared by the run that produced it (logs.py EXPECTED): listed, never counted as a defect

    def key(self) -> tuple[str, str, str, str, bool]:
        return (self.domain, self.rule, self.owner, wording(self.what), self.expected)


def wording(text: str) -> str:
    """The text with its numbers taken out: two lines that differ only in a count or a place are the same wording."""
    return NUMBER.sub("N", text).strip()


def merge(findings: list[Finding]) -> list[Finding]:
    """One finding a cause: the counts added, the places joined in the order met, the first evidence kept."""
    merged: dict[tuple, Finding] = {}
    for f in findings:
        seen = merged.get(f.key())
        if seen is None:
            merged[f.key()] = Finding(f.domain, f.rule, f.owner, wording(f.what), f.count, list(f.where), f.evidence, f.expected)
            continue
        seen.count += f.count
        for place in f.where:
            if place not in seen.where:
                seen.where.append(place)
    return sorted(merged.values(), key=lambda f: (f.expected, -f.count, f.domain, f.owner))


def from_json(line: str) -> Finding:
    """A finding a run printed as `FINDING {json}`: the keys of Finding; domain, rule, owner and what are required."""
    doc = json.loads(line)
    missing = [k for k in ("domain", "rule", "owner", "what") if not str(doc.get(k, "")).strip()]
    if missing:
        raise ValueError("a FINDING line lacks %s: %s" % (", ".join(missing), line.strip()))
    return Finding(str(doc["domain"]), str(doc["rule"]), str(doc["owner"]), str(doc["what"]), int(doc.get("count", 1)),
                   [str(w) for w in doc.get("where", [])], str(doc.get("evidence", "")), bool(doc.get("expected", False)))


def write_report(findings: list[Finding], sources: list[str], out_dir: Path) -> tuple[int, int]:
    """Writes report.json and report.md under `out_dir`; returns (causes, occurrences) of what is not expected."""
    causes = merge(findings)
    live = [f for f in causes if not f.expected]
    out_dir.mkdir(parents=True, exist_ok=True)
    doc = {"sources": sources, "causes": len(live), "occurrences": sum(f.count for f in live), "findings": [asdict(f) for f in causes]}
    (out_dir / "report.json").write_bytes(json.dumps(doc, indent=1, ensure_ascii=False).encode("utf-8"))
    lines = ["# Game audit", "", "Sources: %s" % ", ".join(sources), "",
             "%d cause(s), %d occurrence(s); %d expected by the run that produced them." % (len(live), doc["occurrences"], len(causes) - len(live)), ""]
    by_owner: dict[str, list[Finding]] = {}
    for f in live:
        by_owner.setdefault(f.owner, []).append(f)
    for owner, group in sorted(by_owner.items(), key=lambda kv: -sum(f.count for f in kv[1])):
        lines.append("## %s" % owner)
        for f in group:
            lines.append("- [%s / %s] x%d: %s" % (f.domain, f.rule, f.count, f.what))
            if f.where:
                more = "" if len(f.where) <= WHERE_SHOWN else " (+%d more)" % (len(f.where) - WHERE_SHOWN)
                lines.append("  - where: %s%s" % ("; ".join(f.where[:WHERE_SHOWN]), more))
            if f.evidence:
                lines.append("  - evidence: `%s`" % f.evidence)
        lines.append("")
    expected = [f for f in causes if f.expected]
    if expected:
        lines.append("## Expected (declared by their run)")
        for f in expected:
            lines.append("- [%s / %s] x%d %s: %s" % (f.domain, f.rule, f.count, f.owner, f.what))
        lines.append("")
    (out_dir / "report.md").write_bytes("\n".join(lines).encode("utf-8"))
    return len(live), doc["occurrences"]
