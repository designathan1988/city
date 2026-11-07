"""Checks about time and the outside: prediction against the folder and chance, cleanliness of the program's own text, determinism, other repositories."""
import ast
import filecmp
import glob
import importlib
import json
import os
import pickle
import re
import random
import subprocess
import sys

from ..cache import Cache
from ..core import IDENT, digest, natural_break, program_root
from ..pipeline import analyse, publish
from .. import predict as predict_module
from ..predict import rolling
from ..repo import Repo, toplevel
from ..scan import Run
from .checks_compare import beats
from .supervisor import Job
from .checks_keys import _load, score_pairs

SIGNIFICANT = 0.01
TRIALS = 2000
WHOLE_WORDS = re.compile(r"\w*\}\w+|\w+\{\w*")  # `{word}_tail`, `head_{word}`: the piece of a name a case makes up, never a name of the project


def _predict(repo, out):
    from .. import roles
    tracked = repo.files()
    history = repo.history()
    cache = Cache(os.path.join(out, "cache"), True)
    # The whole replay depends on the commits and on this program: the same pair gives the same numbers.
    known = cache.table("predicted")
    with open(predict_module.__file__, encoding="utf8") as f:
        key = digest(f.read() + (history[-1][0] if history else ""))
    if key not in known:
        known[key] = _replayed(repo, tracked, history, cache, roles)
        cache.save()
    return known[key]


def _replayed(repo, tracked, history, cache, roles):
    found, _ = roles.classify(repo, tracked, history, cache)
    keep = found.own(tracked) - found.index
    rows, _, wins = rolling(repo, history, keep, cache)
    rng = random.Random(1)
    versus_folder = beats(rows, 0, 1, rng, TRIALS)
    versus_chance = beats(rows, 0, 2, rng, TRIALS)
    total = sum(r[3] for r in rows) or 1.0
    means = [round(sum(r[i] * r[3] for r in rows) / total, 4) for i in range(3)]
    ok = bool(rows) and versus_folder[0] and versus_chance[0] and versus_folder[1] < SIGNIFICANT and versus_chance[1] < SIGNIFICANT
    return ok, {"cases": len(rows), "model": means[0], "folder": means[1], "chance": means[2], "p_folder": versus_folder[1], "p_chance": versus_chance[1],
                "weight_of_words": round(wins["lex"] / (wins["lex"] + wins["co"]), 3)}


def prediction_jobs(ctx):
    return [Job("prediction", "predicted", {"path": ctx.path, "out": os.path.join(ctx.home, "jobs", "prediction")}, "prediction")]


def predicted(spec):
    ok, detail = _predict(Repo(spec["path"]), spec["out"])
    return {"pass": ok, "detail": detail}


def prediction(ctx, out):
    return out("prediction")


def _program_files():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    return sorted(glob.glob(os.path.join(here, "**", "*.py"), recursive=True)) + sorted(glob.glob(os.path.join(here, "**", "*.cjs"), recursive=True)) + [here + ".py"]


def _libraries(text):
    """What the libraries the program imports call things: their words are theirs, not the repository's."""
    out = set()
    for node in ast.walk(ast.parse(text)):
        names = [a.name for a in node.names] if isinstance(node, ast.Import) else [node.module] if isinstance(node, ast.ImportFrom) and node.level == 0 else []
        for name in names:
            module = sys.modules.get(name) or importlib.import_module(name)
            for word in dir(module):
                out.add(word)
                try:
                    out.update(dir(getattr(module, word)))
                except (AttributeError, ImportError, RuntimeError):
                    continue
    return out


def cleanliness(ctx):
    """No rare name of the project in the program, and no answer key opened by a scan."""
    run, _ = ctx.plain()
    from .rename import outside_vocabulary
    outside = outside_vocabulary(ctx.repo, run.tracked, run.roles, run.api)
    own = set()
    for path in _program_files():
        with open(path, encoding="utf8") as f:
            text = f.read()
        own.update(IDENT.findall(WHOLE_WORDS.sub(" ", text)))
        if path.endswith(".py"):
            outside |= _libraries(text)
    df = {}
    for path, words in run.roles.tokens.items():
        if path not in run.roles.foreign:
            for w in words:
                df[w] = df.get(w, 0) + 1
    names = {u["name"] for u in run.graph.units.values()} - outside
    project = {w for w in names if w in df}
    import math
    cut = math.exp(natural_break([math.log(df[w]) for w in project])) if len(project) > 1 else 0
    rare = {w for w in project if df[w] <= cut}
    hits = sorted(w for w in own & rare if len(w) > 2)
    opened = sorted(ctx.opened)
    keys = [p for p in opened if "arch_eval" in p.replace("\\", "/")]
    return {"pass": not hits and not keys and bool(opened), "detail": {"rare_names": len(rare), "in_the_program": hits[:60], "keys_opened": keys[:5], "files_opened": len(opened)}}


def determinism_jobs(ctx):
    """The small repository read end to end by two processes, other hash seeds: one with every shortcut of the compiler, one with none."""
    from .checks_copies import _on_small
    return [_on_small(ctx, "small_slow", "determinism_small", publish=os.path.join(ctx.base, "reports_slow"), want=(), env={"ARCH_PCH": "0", "PYTHONHASHSEED": "23"})]


REPORTS = ("queue.json", "groups.json", "levels.json", "coverage.json", "atoms.json")


def _bytes_apart(first, second, names):
    return [n for n in names if not filecmp.cmp(os.path.join(first, n), os.path.join(second, n), shallow=False)]


def determinism_small(ctx, out):
    ctx.ready.wait()
    diff = _bytes_apart(os.path.join(ctx.base, "reports_fast"), os.path.join(ctx.base, "reports_slow"), (*REPORTS, "residual.json"))
    return {"pass": not diff, "detail": {"differ": diff, "compared": len(REPORTS) + 1}}


def second_seed_jobs(ctx):
    """The analysis of the real graph again in another process, under another hash seed."""
    from .checks_copies import _on_graph
    job = _on_graph(ctx, "graph_seed", "determinism", "second_seed", out=os.path.join(ctx.base, "reports_seed"))
    job.env = {"PYTHONHASHSEED": "23"}
    return [job]


def determinism(ctx, out):
    diff = _bytes_apart(ctx.home, os.path.join(ctx.base, "reports_seed"), REPORTS)
    return {"pass": not diff, "detail": {"differ": diff, "compared": len(REPORTS)}}


def _others(ctx):
    """The repositories the request names, in the key: this one's nested repository by its path, the other by its own."""
    found = [(os.path.join(ctx.repo.root, x["path"]), x.get("key")) for x in _load("repositories.json")]
    return [(p, key) for p, key in found if os.path.isdir(p)]


def portability_jobs(ctx):
    """The same program, not one line edited, on the other repositories; the groups it found are written out to be read."""
    roots = [(toplevel(p), key) for p, key in _others(ctx)]
    ctx.state["others"] = [os.path.basename(r) for r, _ in roots]
    return [Job(f"other_{os.path.basename(r)}", "other_repository", {"root": r, "key": key, "base": ctx.base, "out": os.path.join(ctx.home, "jobs", "other_" + os.path.basename(r))}, "portability", True) for r, key in roots]


def other_repository(spec):
    """Read once for a commit of theirs and a text of this program: the same pair gives the same answer."""
    root, base, out = spec["root"], spec["base"], spec["out"]
    name = os.path.basename(root)
    program = "".join(open(p, encoding="utf8").read() for p in sorted(_program_files(), key=os.path.basename))
    head = subprocess.run(("git", "-C", root, "rev-parse", "HEAD"), capture_output=True, text=True, check=True).stdout
    known = os.path.join(out, f"answer_{digest(program + head)}.pkl")
    groups_file = os.path.join(base, f"groups_{name}.txt")
    if os.path.exists(known):
        with open(known, "rb") as f:
            answer, text = pickle.load(f)
        with open(groups_file, "w", encoding="utf8", newline="\n") as f:
            f.write(text)
        return answer
    answer = _read_other(spec, root, base, out, name)
    with open(groups_file, encoding="utf8") as f:
        text = f.read()
    with open(known, "wb") as f:
        pickle.dump((answer, text), f)
    return answer


def _read_other(spec, root, base, out, name):
    os.environ["ARCH_OUT"] = out
    run = Run(root, cache=True, quiet=True, out=out)
    run.home = os.path.normpath(program_root())
    run.key = digest(run.repo.root)
    run.read()
    a = analyse(run)
    publish(run, a)
    run.cache.save()
    with open(os.path.join(out, "groups.json"), encoding="utf8") as f:
        groups = json.load(f)
    top = sorted(groups, key=lambda g: (-g["units"], g["id"]))
    with open(os.path.join(base, f"groups_{name}.txt"), "w", encoding="utf8", newline="\n") as f:
        for g in top:
            f.write(f"{g['units']} units, layer {g['layer']}, {' '.join(g['words'][:8])}: {' '.join(list(g['files'])[:4])}\n")
    good, numbers = _predict(Repo(root), out)
    detail = {"groups": len(groups), "prediction": numbers, "read": f"groups_{name}.txt", "largest": [" ".join(g["words"][:5]) for g in top[:6]]}
    if spec["key"]:
        detail["features_key"] = score_pairs(run, a, _load(spec["key"]))
    return good and len(groups) > 1, detail


def portability(ctx, out):
    got = {name: out(f"other_{name}") for name in ctx.state["others"]}
    return {"pass": bool(got) and all(ok for ok, _ in got.values()), "detail": {name: detail for name, (_, detail) in got.items()}}
