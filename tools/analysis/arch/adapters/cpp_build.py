"""C and C++ compile commands, found by what a build left on disk: every configuration, with the driver's own target and paths."""
import collections
import json
import os
import re
import shlex
import shutil
import subprocess

SOURCE = re.compile(r"\.(c|cc|cpp|cxx)$")
HEADER = re.compile(r"\.(h|hh|hpp|hxx|inl)$")
KEEP_ONE = ("-D", "-U", "-I", "-std", "-f", "-m", "--target")
KEEP_TWO = ("-isystem", "-include", "-iquote", "-idirafter")
_KNOWN = {}


def _entries(repo):
    """The compile commands of every build folder on disk, each folder a configuration. Asked of the tools once a process."""
    if repo.root not in _KNOWN:
        _KNOWN[repo.root] = _ask_builds(repo)
    return _KNOWN[repo.root]


def _ask_builds(repo):
    out = {}
    ninja = shutil.which("ninja")
    for ninja_file in repo.on_disk("*build.ninja"):
        folder = os.path.join(repo.root, os.path.dirname(ninja_file))
        if ninja is None:
            continue
        run = subprocess.run((ninja, "-t", "compdb"), cwd=folder, capture_output=True, check=False)
        if run.returncode == 0:
            out[os.path.dirname(ninja_file)] = json.loads(run.stdout.decode("utf8", "replace") or "[]")
    for db in repo.on_disk("*compile_commands.json"):
        with open(os.path.join(repo.root, db), encoding="utf8") as f:
            out[os.path.dirname(db)] = json.load(f)
    return out


def _split_words(entry):
    command = entry.get("command")
    return shlex.split(command.replace("\\", "/")) if command else list(entry.get("arguments", []))


def _driver(compiler, memo):
    """What the compiler driver adds by itself: its target and its include search list."""
    if compiler in memo:
        return memo[compiler]
    args = []
    try:
        run = subprocess.run((compiler, "-E", "-v", "-x", "c++", "-"), input=b"", capture_output=True, check=False)
        text = run.stderr.decode("utf8", "replace").replace("\r\n", "\n").replace("\\", "/")
        target = re.search(r"^Target: (\S+)", text, re.M)
        if target:
            args.append("--target=" + target.group(1))
        block = re.search(r"#include <\.\.\.> search starts here:\n(.*?)\nEnd of search list", text, re.S)
        for line in (block.group(1).splitlines() if block else []):
            args += ["-isystem", line.strip()]
    except OSError:
        pass
    memo[compiler] = args
    return args


def _flags(words, repo, export):
    """The flags that matter to parsing, repository paths moved to the export of the analysed commit."""
    out, i = [], 1
    while i < len(words):
        w = words[i]
        if w in KEEP_TWO and i + 1 < len(words):
            out += [w, _moved(words[i + 1], repo, export)]
            i += 2
            continue
        if w.startswith("-I") and len(w) > 2:
            out.append("-I" + _moved(w[2:], repo, export))
        elif w.startswith(("-D", "-U")):
            out.append(repo.flag(w))
        elif w.startswith(KEEP_ONE) and not w.startswith(("-fprofile", "-flto", "-fdebug", "-fmodules")):
            out.append(w)
        i += 1
    return out


def _moved(path, repo, export):
    p = path.replace("\\", "/")
    base = repo.root.replace("\\", "/")
    if p.lower().startswith(base.lower() + "/"):
        rel = repo.translate(p[len(base) + 1:])
        if os.path.exists(os.path.join(export, rel)):
            return os.path.join(export, rel).replace("\\", "/")
    return p


def _relative(entry, root):
    path = entry["file"].replace("\\", "/")
    if not os.path.isabs(path):
        path = os.path.join(entry["directory"], path).replace("\\", "/")
    path = os.path.normpath(path).replace("\\", "/")
    base = root.replace("\\", "/")
    return path[len(base) + 1:] if path.lower().startswith(base.lower() + "/") else None


def compile_units(repo, tracked, export):
    """Tasks (config, file, flags) for every tracked source a build knows, in every configuration, plus the sources no build
    knows (a commit of another time, a copy): those take the flags most sources of that configuration use."""
    tasks, memo = [], {}
    for config, entries in sorted(_entries(repo).items()):
        by_file, common = {}, collections.defaultdict(collections.Counter)
        for entry in entries:
            rel = _relative(entry, repo.root)
            rel = repo.translate(rel) if rel is not None else None
            if rel is None or rel not in tracked:
                continue
            words = _split_words(entry)
            if not words:
                continue
            flags = _driver(words[0], memo) + _flags(words, repo, export)
            by_file[rel] = flags
            common[_home(rel)][tuple(flags)] += 1
        if not common:
            continue
        usual = {home: list(sorted(c, key=lambda k: (-c[k], k))[0]) for home, c in common.items()}
        for rel in sorted(p for p in tracked if SOURCE.search(p) and _home(p) in usual):
            tasks.append((config, rel, by_file.get(rel, usual[_home(rel)]), rel in by_file))
    return tasks


def _home(path):
    return "/".join(path.split("/")[:2])


def compiler_of(repo):
    """The compiler a build used, for what only it can answer."""
    return next((w[0] for es in _entries(repo).values() for w in map(_split_words, es) if w and "++" in os.path.basename(w[0])), None)


def macros(compiler, flags, source):
    """Every macro and its text after a file, as the compiler itself lists them."""
    words = [compiler, "-E", "-dM", "-x", "c++", *[f for f in flags if f != "-include-pch" and not f.endswith(".pch")], source]
    out = subprocess.run(words, capture_output=True, check=False).stdout.decode("utf8", "replace")
    table = {}
    for line in out.split("\n"):
        if line.startswith("#define "):
            head, _, body = line[8:].partition(" ")
            table[head.split("(")[0]] = (head, body)
    return table
