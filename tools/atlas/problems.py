"""The problem register (log/problems.jsonl), its one owner: read, judged, changed and written only here. One problem a line,
written as UTF-8 bytes with LF; a status moves only through `Register.move`, which reads MOVES (the transitions of the feature's
data model); an id is P<area>-<n>, n one past the largest the area ever had, so no id is reused. The fields and their enums are
the schema's (tools/gates/schemas/problem.schema.json); `faults` is the rule the register gate (tools/gates/gate_data.py) holds."""
import datetime
import json
import os
import subprocess
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import inputs  # noqa: E402  the tools' one wrapper of git (a deadline, no console window)
from tasks import schema as json_schema  # noqa: E402  the tools' one JSON Schema checker

ROOT = os.path.abspath(os.path.join(TOOLS_DIR, ".."))
REGISTER = "log/problems.jsonl"
SCHEMA = "tools/gates/schemas/problem.schema.json"
UNKNOWN = "unknown"  # a cause not found yet; the reading that would find it is then required
GIT_REF = "git:"  # evidence that is a git reference (an archive tag, a commit), not a path on disk
FINAL = ("fixed", "dropped")
LISTS = ("evidence", "origin", "related")  # the fields an update adds to instead of replacing
# The moves of a problem's status. A problem the owner judges no problem may be dropped from any state that is not final.
MOVES = {
    "open": ("traced", "dropped"),
    "traced": ("planned", "dropped"),
    "planned": ("fixed-pending-check", "dropped"),
    "fixed-pending-check": ("fixed", "traced", "dropped"),
    "fixed": (),
    "dropped": (),
}


class RegisterError(Exception):
    """A register that cannot be read, or a change the rules refuse: the message says which problem and why."""


def now():
    """The time a move is stamped with, to the minute (the time from a report to its located cause is measured in minutes)."""
    return datetime.datetime.now().strftime("%Y-%m-%dT%H:%M")


def load_schema(root=ROOT):
    with open(os.path.join(root, SCHEMA), encoding="utf-8") as fh:
        return json.load(fh)


def read(path):
    """The problems of a register file in their order; a line that is no JSON object is a RegisterError naming it. A register
    not written yet holds no problem."""
    if not os.path.isfile(path):
        return []
    with open(path, "rb") as fh:
        text = fh.read().decode("utf-8")
    problems = []
    for n, raw in enumerate(text.split("\n"), 1):
        if not raw.strip():
            continue
        try:
            problem = json.loads(raw)
        except json.JSONDecodeError as e:
            raise RegisterError(f"{path}:{n}: not JSON ({e.msg})") from e
        if not isinstance(problem, dict):
            raise RegisterError(f"{path}:{n}: a line holds one problem, a JSON object")
        problems.append(problem)
    return problems


def git_ref_exists(root, ref):
    """Whether git names an object by this reference (a tag, a branch, a commit) in the repository at `root`."""
    try:
        inputs.git(root, "rev-parse", "--verify", "--quiet", ref + "^{object}")
        return True
    except (RuntimeError, OSError, subprocess.TimeoutExpired):
        return False


def _rules(problem, ids):
    """What no schema keyword states about one problem."""
    out = []
    status = problem.get("status")
    if problem.get("cause") == UNKNOWN and not problem.get("reading"):
        out.append("the cause is unknown and no reading says what would find it")
    if status in FINAL and not problem.get("fix"):
        out.append(f"{status} with no fix written (for a dropped problem: why it is none)")
    if (problem.get("closed") is not None) != (status in FINAL):
        out.append("a closing date goes with the status fixed or dropped, and only with them")
    for other in problem.get("related") or []:
        if other not in ids:
            out.append(f"related {other} is no problem of the register")
    return out


def faults(problems, schema, root, ref_exists):
    """Everything wrong with a register as (problem id or line, reason): the schema's verdict on each problem, ids used twice,
    the rules no keyword states, and evidence that is not there (`ref_exists` answers for a git reference)."""
    out, seen = [], set()
    ids = {p.get("id") for p in problems}
    for n, problem in enumerate(problems, 1):
        where = str(problem.get("id") or f"line {n}")
        out += [(where, e) for e in json_schema.errors(problem, schema, complete=True)]
        if problem.get("id") in seen:
            out.append((where, "id used twice"))
        seen.add(problem.get("id"))
        out += [(where, e) for e in _rules(problem, ids)]
        for item in problem.get("evidence") or []:
            if not isinstance(item, str):
                continue
            if item.startswith(GIT_REF):
                if not ref_exists(item[len(GIT_REF):]):
                    out.append((where, f"evidence {item}: no such git reference"))
            elif not os.path.exists(os.path.join(root, item)):
                out.append((where, f"evidence {item}: not on disk"))
    return out


class Register:
    """The register loaded whole; every change through its methods, written back whole by `save` once the rules hold."""

    def __init__(self, path=None, root=ROOT):
        self.root = root
        self.path = path or os.path.join(root, REGISTER)
        self.schema = load_schema(root)
        self.problems = read(self.path)

    def fields(self):
        return list(self.schema["properties"])

    def enum(self, field):
        return list(self.schema["properties"][field]["enum"])

    def get(self, pid):
        for problem in self.problems:
            if problem.get("id") == pid:
                return problem
        raise RegisterError(f"{pid}: no such problem in {self.path}")

    def next_id(self, area):
        prefix = f"P{area}-"
        taken = [int(p["id"][len(prefix):]) for p in self.problems if str(p.get("id", "")).startswith(prefix)
                 and p["id"][len(prefix):].isdigit()]
        return f"{prefix}{max(taken, default=0) + 1}"

    def add(self, opened_at=None, traced_at=None, **given):
        """A new problem, status open (traced at once when its cause is given), its id assigned. `opened_at` and `traced_at`
        carry the times a problem brought from another record already had there (the migration); a new one is stamped now."""
        area = given.get("area")
        if area not in self.enum("area"):
            raise RegisterError(f"area {area!r} is not one of {self.enum('area')}")
        problem = {field: None for field in self.fields()}
        problem.update({"evidence": [], "origin": [], "related": [], "cause": UNKNOWN})
        for field, value in given.items():
            if field not in problem or field in ("id", "status", "opened", "traced", "closed"):
                raise RegisterError(f"{field}: not a field a new problem is given")
            problem[field] = list(dict.fromkeys(value)) if field in LISTS else value
        problem.update({"id": self.next_id(area), "status": "open", "opened": opened_at or now()})
        self.problems.append(problem)
        if problem["cause"] != UNKNOWN:
            problem["traced"] = traced_at
            self.move(problem, "traced")
        return problem

    def update(self, pid, status=None, **given):
        """Fields set (a list field added to); a known cause found for an open problem traces it; `status` moves it."""
        problem = self.get(pid)
        for field, value in given.items():
            if field not in problem or field in ("id", "status", "opened", "traced", "closed"):
                raise RegisterError(f"{field}: not a field an update sets (the status moves through --status)")
            problem[field] = list(dict.fromkeys((problem[field] or []) + list(value))) if field in LISTS else value
        if given.get("cause") not in (None, UNKNOWN) and problem["status"] == "open":
            self.move(problem, "traced")
        if status is not None and status != problem["status"]:
            self.move(problem, status)
        return problem

    def move(self, problem, status):
        """The one setter of a problem's status: refused unless MOVES allows it; stamps the first trace and the close."""
        allowed = MOVES.get(problem["status"], ())
        if status not in allowed:
            raise RegisterError(f"{problem['id']}: {problem['status']} cannot move to {status} "
                                f"(it may move to {', '.join(allowed) or 'nothing: it is final'})")
        if status == "traced" and problem.get("traced") is None:
            problem["traced"] = now()
        problem["status"] = status
        problem["closed"] = now() if status in FINAL else None

    def close(self, pid, fix):
        problem = self.get(pid)
        problem["fix"] = fix
        self.move(problem, "fixed")
        return problem

    def drop(self, pid, why):
        problem = self.get(pid)
        problem["fix"] = why
        self.move(problem, "dropped")
        return problem

    def query(self, area=None, owner=None, status=None, cause=None):
        """The problems matching every filter given: area and status exactly, owner as a part of it, cause by all its words."""
        words = cause.lower().split() if cause else []
        return [p for p in self.problems
                if (area is None or p.get("area") == area) and (status is None or p.get("status") == status)
                and (owner is None or owner.lower() in str(p.get("owner", "")).lower())
                and all(w in str(p.get("cause", "")).lower() for w in words)]

    def faults(self):
        return faults(self.problems, self.schema, self.root, lambda ref: git_ref_exists(self.root, ref))

    def save(self):
        """The register written whole, one problem a line, bytes with LF, only when every rule holds."""
        wrong = self.faults()
        if wrong:
            raise RegisterError("the register would not hold, nothing written:\n" + "\n".join(f"  {w}: {r}" for w, r in wrong))
        data = "".join(json.dumps(p, ensure_ascii=False) + "\n" for p in self.problems).encode("utf-8")
        os.makedirs(os.path.dirname(self.path), exist_ok=True)
        part = self.path + ".part"
        with open(part, "wb") as fh:
            fh.write(data)
        os.replace(part, self.path)
