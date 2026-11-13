# Task and Result: one piece of work declared as data (process, inputs, deadline, verdict) and what came of it.
from __future__ import annotations

import hashlib
import json
import sys
from dataclasses import asdict, dataclass, field
from pathlib import Path

PASS, FAIL, TIMEOUT, BROKEN, SKIPPED = "PASS", "FAIL", "TIMEOUT", "BROKEN", "SKIPPED"
CACHED_PASS, CACHED_FAIL = "CACHED_PASS", "CACHED_FAIL"
PASSED = frozenset({PASS, CACHED_PASS})
CACHEABLE = frozenset({PASS, FAIL})  # they follow from the inputs; a timeout or a crash may follow from the machine

HERE = Path(__file__).resolve().parent
TOOLS = HERE.parent
ROOT = TOOLS.parent
CALL = HERE / "call.py"
SPECS = "dist/tasks/specs"  # spec files of Python callables, named by their content


@dataclass
class Task:
    """A process with a deadline. `inputs` (repo-relative paths or globs, `!` excludes) and `extra` key its result;
    `outputs` are restored from the cache on a hit; `after` names the tasks whose results it needs."""
    name: str
    argv: list
    timeout_s: float
    inputs: list = field(default_factory=list)
    extra: list = field(default_factory=list)
    outputs: list = field(default_factory=list)
    after: list = field(default_factory=list)
    cwd: str | None = None
    env: dict | None = None
    hidden: bool = False
    verdict: str | None = None
    resources: dict = field(default_factory=dict)
    params: dict = field(default_factory=dict)  # read by the verdict
    fresh: bool = False  # the declared outputs are removed before the process starts: nothing stale survives a run

    def __post_init__(self):
        if not isinstance(self.name, str) or not self.name:
            raise ValueError("a task needs a name")
        if not isinstance(self.argv, list) or not self.argv or not all(isinstance(a, str) for a in self.argv):
            raise ValueError(f"{self.name}: argv must be a non-empty list of strings")
        if isinstance(self.timeout_s, bool) or not isinstance(self.timeout_s, (int, float)) or not self.timeout_s > 0:
            raise ValueError(f"{self.name}: timeout_s must be a positive number of seconds")
        for f in ("inputs", "extra", "outputs", "after"):
            v = getattr(self, f)
            if not isinstance(v, list) or not all(isinstance(x, str) for x in v):
                raise ValueError(f"{self.name}: {f} must be a list of strings")
        locks = (self.resources or {}).get("locks", {})
        if any(mode not in ("shared", "exclusive") for mode in locks.values()):
            raise ValueError(f"{self.name}: a lock is 'shared' or 'exclusive'")

    @classmethod
    def call(cls, name: str, target: str, spec: dict, timeout_s: float, root: Path | None = None, **kw) -> "Task":
        """A Python function as a task: `python tools/tasks/call.py module:function spec.json`. The spec and the module's
        source key the result."""
        root = Path(root or ROOT)
        text = json.dumps(spec, sort_keys=True, ensure_ascii=False)
        path = root / SPECS / f"{hashlib.sha256(text.encode()).hexdigest()}.json"
        if not path.exists():
            path.parent.mkdir(parents=True, exist_ok=True)
            tmp = path.with_suffix(f".tmp{id(path)}")
            tmp.write_text(text, encoding="utf-8")
            tmp.replace(path)
        module = target.split(":", 1)[0]
        source = (TOOLS / Path(*module.split("."))).with_suffix(".py")
        kw["inputs"] = list(kw.get("inputs", [])) + [source.as_posix()]
        kw["extra"] = list(kw.get("extra", [])) + [text]
        return cls(name=name, argv=[sys.executable, str(CALL), target, str(path)], timeout_s=timeout_s, **kw)


@dataclass
class Result:
    name: str
    status: str
    exit_code: int | None
    seconds: float
    log: str
    key: str
    message: str = ""

    @property
    def passed(self) -> bool:
        return self.status in PASSED

    def to_json(self) -> dict:
        return asdict(self)
