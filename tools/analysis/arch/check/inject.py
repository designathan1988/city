"""Builds the injected cases and tells which were found. The same cases go into a small repository of their own and into a copy of the real one."""
import collections
import json
import os
import random
import shutil
import stat
import string
import subprocess

from ..adapters.cpp_build import _entries, _split_words
from ..classes import Seen, link_classes
from ..classes_more import more_classes
from ..scan import Run
from ..design import OWNER_KINDS
from ..groups import leaves_of
from . import inject_cpp, inject_design, inject_extra, inject_more, inject_script


def words(seed):
    """Fresh words that read like any others, never seen in any repository: nothing marks them as injected."""
    rng = random.Random(seed)
    used = set()

    def fresh():
        while True:
            w = "".join(rng.choice("bdfgklmnprstvz") + rng.choice("aeiou") for _ in range(4))
            if w not in used:
                used.add(w)
                return w
    return fresh


def _homes(tracked, rng, scope, avoid):
    """A random existing folder for each kind of file, so a case can land anywhere the delivered program can run it."""
    def folder(*exts):
        options = sorted({p.rsplit("/", 1)[0] for p in scope if p.endswith(exts) and "/" in p and p not in avoid})
        return rng.choice(options) if options else ""
    return {"cpp": folder(".cpp"), "gd": folder(".gd"), "json": folder(".json"), "shader": folder(".gdshader"), "scene": folder(".tscn"), "py": folder(".py"),
            "any_script": lambda: folder(".gd")}


def build(seed, api, tracked, scope, avoid=(), define="", small=False):
    """path -> bytes of every injected file, and (class or category, word) of every case. `define` is a macro only one build sets."""
    rng = random.Random(seed)
    w = words(seed)
    places = _homes(tracked, rng, scope, set(avoid))
    expect = []
    join = lambda d, n: f"{d}/{n}" if d else n  # noqa: E731
    stem = w()
    gd = join(places["gd"] or "project", f"{stem}.gd")
    sh = join(places["shader"] or "project", f"{stem}.gdshader")
    inc_body = None
    data = join(places["json"] or "project", f"{stem}.json")
    scene = join(places["scene"] or "project", f"{stem}.tscn")
    files = {join(places["cpp"] or "native", f"{stem}.cpp"): inject_cpp.source(w, expect)}
    json_text, gd_tail = inject_script.data_text(w, expect)
    sh_text, inc_body, inc_word = inject_script.shader_text(w, expect)
    inc_name = next(c[1] for c in expect if c[0] == "B45") + "_inc.gdshaderinc"
    script = inject_script.texts(w, expect, api, f"{stem}.json", f"{stem}.gdshader", f"{stem}.tscn", f"{stem}.gd", stem)
    script += f'\nconst DATA_{stem} := "{stem}.json"\nfunc read_{stem}() -> void:\n{gd_tail}	material_{inc_word}("{inc_word}_boost")\n\nfunc material_{inc_word}(n: String) -> void:\n	pass\n'
    connection = w()
    script += f"\nfunc {connection}_pressed() -> void:\n	pass\n"
    expect.append(("shader_cost", stem))
    files[gd] = script
    files[data] = json_text
    files[sh] = sh_text
    files[join(places["shader"] or "project", inc_name)] = inc_body
    files[scene] = inject_script.scene_text(w, expect, f"{stem}.gd", connection)
    files.update(inject_more.files(w, expect, api, places, join, stem))
    files.update(inject_design.files(w, expect, places, join))
    cpp_home = places["cpp"] or "native"
    header, second, generated, shared = inject_extra.small_cpp(w, expect, stem, define)
    files[join(cpp_home, f"{stem}_more.h")] = header
    files[join(cpp_home, f"{stem}_more.cpp")] = second
    files.update({join(cpp_home, k): v for k, v in generated.items()})
    extra, tables, shaders, tools = inject_extra.script(w, expect, stem, api, shared)
    files[join(places["gd"] or "project", f"{stem}_extra.gd")] = extra
    files.update({join(places["json"] or "project", k): v for k, v in tables.items()})
    files.update({join(places["shader"] or "project", k): v for k, v in shaders.items()})
    files.update({join(places["py"] or "tools", k): v for k, v in tools.items()})
    files[gd + ".uid"] = f"uid://{stem}\n"
    files[scene] = files[scene].replace(f'path="res://{stem}.gd"', f'uid="uid://{stem}" path="res://{stem}.gd"')
    expect.append(("D11", stem))
    inject_extra.extras(expect, stem)
    out = {k: v.encode("utf8") for k, v in files.items()}
    if not small:
        # What nearly everything uses cannot be made in a dozen files: in the real repository that class stands on the small one's proof.
        return out, [c for c in expect if c[0] != "B98"]
    word, code, profile = inject_extra.recorded(w, expect)
    out[join(cpp_home, f"{stem}_ran.cpp")] = code.encode("utf8")
    return out, expect, profile


def define_of_one(repo):
    """A macro one build configuration sets and another does not, as the builds on disk say."""
    seen = {}
    for config, entries in sorted(_entries(repo).items()):
        usual = collections.Counter(tuple(sorted(x[2:].split("=")[0] for x in _split_words(e) if x.startswith("-D"))) for e in entries)
        seen[config] = set(sorted(usual, key=lambda k: (-usual[k], k))[0]) if usual else set()
    built = sorted(c for c, words in seen.items() if words)
    for config in built:
        for other in built:
            only = sorted(seen[config] - seen[other])
            if only:
                return only[0]
    return ""


def _commit(folder, title):
    git = ("git", "-C", folder)
    subprocess.run((*git, "add", "-A"), check=True, capture_output=True)
    subprocess.run((*git, "-c", "user.name=check", "-c", "user.email=check@example.invalid", "commit", "-q", "-m", title), check=True)


def _write(folder, files):
    for path, data in files.items():
        target = os.path.join(folder, path)
        if data is None:
            os.remove(target)
            continue
        os.makedirs(os.path.dirname(target), exist_ok=True)
        with open(target, "wb") as f:
            f.write(data)


def _made_profile(folder, tool, text):
    """The text form of a recording, made binary by the toolchain's own tool, as a real recording would be."""
    source = os.path.join(folder, "recording.txt")
    with open(source, "w", encoding="utf8", newline="\n") as f:
        f.write(text)
    subprocess.run((tool, "merge", "-o", os.path.join(folder, "recording.profdata"), source), check=True, capture_output=True)
    os.remove(source)


def fixture(folder, files, base_repo, api_path, define="", profile="", later=()):
    """A small repository holding only the injected files, built twice by the same compiler the real project uses, one build with a macro of
    its own. `later` is a list of file changes, one commit each."""
    def writable(fn, path, _):
        os.chmod(path, stat.S_IWRITE)
        fn(path)

    if os.path.isdir(folder):
        shutil.rmtree(folder, onexc=writable)
    os.makedirs(folder)
    _write(folder, files)
    compiler = next(w[0] for v in _entries(base_repo).values() for w in map(_split_words, v) if w and "++" in os.path.basename(w[0]))
    for name, flag in (("build", ""), ("build_more", f" -D{define}" if define else "")):
        entries = [{"directory": folder.replace("\\", "/"), "file": p, "command": f'"{compiler}" -std=c++17{flag} -c {p}'} for p in sorted(files) if p.endswith(".cpp")]
        os.makedirs(os.path.join(folder, name))
        with open(os.path.join(folder, name, "compile_commands.json"), "w", encoding="utf8") as f:
            json.dump(entries, f)
    with open(os.path.join(folder, ".gitignore"), "w", encoding="utf8") as f:
        f.write("build/\nbuild_more/\n")
    if api_path:
        shutil.copy(api_path, os.path.join(folder, "extension_api.json"))
    if profile:
        _made_profile(folder, os.path.join(os.path.dirname(compiler), "llvm-profdata"), profile)
    subprocess.run(("git", "-C", folder, "init", "-q"), check=True)
    _commit(folder, "cases")
    for i, change in enumerate(later):
        _write(folder, change)
        _commit(folder, f"change {i}")
    return folder


def _one_feature(run, analysis, word):
    """A feature is found when all its pieces, in more than one language, fall in one cluster of some level of the map, far from the whole:
    no one told the program it exists."""
    pieces = {u for u, x in run.graph.units.items() if word in x["name"] and u in analysis["atoms"].rank}
    languages = {u.split(":", 1)[0] for u in pieces}
    tree = analysis["tree"]
    found = [node for level in analysis["upper"] for node in level if pieces <= set(leaves_of(tree, node))]
    found += [g["tree"] for g in analysis["groups"] if pieces <= set(g["members"])]
    smallest = min((len(leaves_of(tree, node)) for node in found), default=len(analysis["atoms"].rank))
    return len(pieces) > 2 and len(languages) > 1 and smallest < len(analysis["atoms"].rank) / 2


def judge(run, analysis, expect):
    """Each case found or not. A class is found through its own test; a cost through a finding; never through a shared word."""
    seen = Seen(run, analysis)
    table = {cid: fn for cid, _, fn in (*link_classes(), *more_classes())}
    items = {}
    by_category = {}
    for f in analysis["findings"]:
        by_category.setdefault(f["category"], []).append(json.dumps(f, ensure_ascii=False, default=str))
    by_category["missing_owner"] = [x for kind in OWNER_KINDS for x in by_category.get(kind, ())]
    out = {}
    for cid, word in expect:
        if cid == "feature":
            out[(cid, word)] = _one_feature(run, analysis, word)
            continue
        if cid not in items:
            try:
                items[cid] = [str(x) for x in table[cid](seen)] if cid in table else by_category.get(cid, [])
            except KeyError:
                items[cid] = []
        out[(cid, word)] = any(word in x for x in items[cid])
    return out


def run_on(view_or_path, cache, quiet, off=(), base=None):
    from ..pipeline import analyse
    run = Run(view_or_path if isinstance(view_or_path, str) else None, repo=None if isinstance(view_or_path, str) else view_or_path, cache=cache, off=off, quiet=quiet, out=base)
    run.read()
    return run, analyse(run)


__all__ = ["build", "fixture", "judge", "run_on", "string"]
