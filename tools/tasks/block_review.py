# The static side of the block rule (block.py): what the code must hold as it is written, said when a block opens (`limits`), and
# the review that closes the coding (`review`: the gates of every changed file against the register, no build and no engine).
# `block verify` is refused until the code as it stands passed the review, so nothing a reading shows is left for a build or a
# test to find. And what a verification may run (`run_bounds`), with the owner's order for a run past a session's own (`order`).
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

try:
    from tasks import block
    from tasks.plans import FAR_VIEWS, photo_plans
except ImportError:  # run beside block.py as a script (the build scripts)
    import block
    from plans import FAR_VIEWS, photo_plans

REVIEW_FILES = Path("dist/block/review_files.txt")  # the changed files the review judged
ORDER = Path("dist/block/order.json")  # the owner's order standing for one run (written by `order`, spent by the hook)
GATES = ("tools/gates/run_gates.py", "--register", "doc/problems.jsonl")
GATES_SECONDS = 300  # the review's own deadline: a process of seconds, as every other (CLAUDE.md, Working)
MEASURES = ("lab_test",)  # the quick CPU, GPU and memory measure, run alone: a verification's second and last run
MAX_CHECKS = 6  # tasks a verification's run of checks may name: the block's own test, its photo plan, e2e; never a whole suite

def kind_of(tasks) -> str | None:
    """What a run of these tasks is in a verification: the measure (the lab's test alone), the checks (anything else that is no
    build), or nothing counted (builds only)."""
    real = [n for n in tasks or () if n not in block.BUILDS]
    if not real:
        return None
    return "measure" if all(n.split("@")[0] in MEASURES for n in real) else "checks"


def run_bounds(doc: dict, what: str, tasks) -> str | None:
    """Why a run of `tasks` is past what a verification may run, or None: a verification is one run of a few checks that take
    seconds and one measure; a whole suite only on the owner's order."""
    kind = kind_of(tasks)
    many = [n for n in tasks or () if n not in block.BUILDS]
    if kind == "checks" and len(many) > MAX_CHECKS and not doc.get("suite_by_owner"):
        return (f"refused: {len(many)} checks in one run. A block's verification is a small test of seconds: its own test, its "
                f"photo plan, e2e ({MAX_CHECKS} tasks at most). A whole suite runs only on the owner's order "
                f"(`block verify --owner \"<the owner's words>\"` allows it for that verification). Asked: {what}")
    if kind and (doc.get("ran") or {}).get(kind):
        return (f"refused: this verification of '{doc['name']}' already had its run of {kind}. A verification is one run of "
                f"checks (every test and photo plan named in one command) and one measure (lab_test alone): read what they "
                f"answered; everything found is fixed in the code in one opening (`block open --cause`), reviewed (`block "
                f"review`), and verified again ({doc.get('verifies') or 0} of {block.MAX_VERIFIES} used). Asked: {what}")
    return None


def closing(doc: dict, root: Path) -> None:
    """Why a block in verification may not close, raised: it ran no check, or the functionality was not photographed by a
    plan of its own. The far views of the generated town (FAR_VIEWS) are the same for every functionality and show nothing
    of what a block built: they close none."""
    if not (doc.get("ran") or {}).get("checks"):
        raise block.BlockError("this verification ran no check: a block closes on checks that pass, its one run named in one "
                               "command (`python tools/tasks/cli.py run <its test>,<its own photo plan>,e2e`)")
    plans = photo_plans(Path(root) / "tools" / "tasks" / "suites.json")
    own = [n for n, verdict in (doc.get("runs") or {}).items() if n in plans and n != FAR_VIEWS and verdict in block.PASSED]
    if not own:
        raise block.BlockError("the functionality was not photographed: a block closes on its own photo plan (a plan of "
                               "project/tests/visual written for its cases: the tool in hand, before, the action, the result, "
                               f"each variant, from near), run in this verification and passed. `{FAR_VIEWS}` is the same far "
                               "view for every functionality and is no such plan")


def order(words: str | None, root: Path = block.ROOT) -> int:
    """The owner's order for one run past what a session may run by itself (a second look with no block open, a whole suite,
    the 24 km measures, the hunt), written down in the owner's own words: the hook (.claude/hooks/port_looks.py) lets this
    command pass only when the owner wrote them in the session, and the run that follows spends the order."""
    if not words:
        print("refused: an order is given in the owner's own words: `block order \"<the owner's sentence>\"`", file=sys.stderr)
        return 1
    path = Path(root) / ORDER
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps({"words": words, "at": block._now()}, indent=1) + "\n", encoding="utf-8")
    print(f"the owner's order stands for one run: {words}")
    return 0


def changed_files(root: Path) -> list[str]:
    """Every file changed or new in the work tree, as git names them."""
    done = subprocess.run(["git", "-C", str(root), "ls-files", "-m", "-o", "--exclude-standard"], capture_output=True, text=True,
                          encoding="utf-8", errors="replace", timeout=block.GIT_SECONDS)
    return sorted({f for f in done.stdout.splitlines() if (Path(root) / f).is_file()}) if done.returncode == 0 else []


def review(root: Path = block.ROOT) -> int:
    """The static review of the block's code: the gates of every changed file against the register, their lines shown as they
    come. 0 when nothing new is found (then `verify` may follow), 1 otherwise: every finding is fixed in the code and the review
    is run again, before anything is built."""
    files = changed_files(root)
    path = Path(root) / REVIEW_FILES
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("".join(f + "\n" for f in files), encoding="utf-8", newline="\n")
    done = subprocess.run([sys.executable, str(Path(root) / GATES[0]), "--files-from", str(path), *GATES[1:]], cwd=str(root),
                          timeout=GATES_SECONDS)
    if done.returncode == 0:
        block.reviewed(root)
        print(f"block review: {len(files)} changed file(s) pass the gates; `block verify` may follow")
    else:
        print("block review: findings above: fix every one in the code, then `block review` again (nothing is built before)",
              file=sys.stderr)
    return done.returncode


def limits(root: Path = block.ROOT) -> str:
    """What the code of a block must hold as it is written, from the gates' own rules: said when a block opens."""
    try:
        rules = json.loads((Path(root) / "tools" / "gates" / "rules.json").read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return ""
    sizes = ", ".join(f"{ext} {n}" for ext, n in (rules.get("max_lines") or {}).items())
    bodies = ", ".join(f"{ext} {n}" for ext, n in (rules.get("max_function_lines") or {}).items())
    return (f"The code holds, as it is written (tools/gates/rules.json; the hook reads each file's gates after every edit): "
            f"lines a file at most: {sizes}; lines a function: {bodies}; no name the reference defines; no number in the code "
            f"(a named constant in its owner, or the data); an enum stepped as a state has its one setter and table "
            f"(rules.json states); a new file starts with its responsibility and enters doc/map.md. Then `block review` (the "
            f"gates of every changed file), then `block verify`: one run of a few checks that take seconds (the block's own "
            f"test, its own photo plan, e2e; `--seconds <n>`, the run's deadline) and one measure (lab_test). The block's own "
            f"photo plan is written with the code: a small scene for its cases, each photographed from near (`{FAR_VIEWS}` "
            f"closes no block). The whole suite runs only on the owner's order.")
