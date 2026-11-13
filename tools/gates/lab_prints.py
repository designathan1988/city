"""The roadcraft's fingerprints, the store LAB_COPY holds a change against: every name the reference tree defines for others,
in its normal form, the winnowed k-gram fingerprints of each of its units (a function body, the code outside them, a program
in a string: lab_units.py) read as language-neutral tokens (lab_tokens.py), and each unit's non-trivial numbers in order;
and the base, what independent code on this machine also writes (the vendored and fetched libraries: every k-gram and
number of theirs), which is no evidence of a copy (MOSS's base files, -b). Kept by the content of the gate sources the store
is built by (STORE_SOURCES) and each tree file's path, size and modification time, so the trees are read again only when one
of their files or one of those sources changed, and an edit of another gate file keeps the store."""
import hashlib
import os
import sys

import lab_tokens
import lab_units
from source import Source

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import locations  # noqa: E402  the one owner of this machine's paths

CODE_EXTENSIONS = frozenset({".ts", ".tsx", ".js", ".mjs", ".cpp", ".cc", ".c", ".h", ".hpp", ".gd", ".gdshader",
                             ".gdshaderinc", ".glsl"})
STORE_SETTINGS = ("k", "w", "min_name_len", "trivial_numbers")  # what of rules.json lab_copy the store is made with
# The gate sources build_store reaches through their imports (this file, lab_tokens, lab_units, source and theirs): the store
# is made by them alone, so their content keys it and no other gate file's edit rebuilds it. tasks/locations is left out: it
# finds the reference tree, whose place the trees' digest holds.
STORE_SOURCES = tuple(f"tools/gates/{name}.py" for name in (
    "lab_prints", "lab_tokens", "lab_units", "source", "cpp_syntax", "gd_syntax", "facts", "lex_c", "lex_gd", "lex_other",
    "tokens"))


class NoReference(Exception):
    """The reference tree or a base library is not on this machine: nothing can be judged against them."""


class Store:
    """The reference's names (normal form -> ["file:line"]), fingerprints (hash -> [[file, unit line, token index in the
    unit]]), units' numbers ("file:unit line" -> [values]) and units' neutral tokens ("file:unit line" -> "D I = ...", what
    a shared passage is grown along); a fingerprint's and a number's spread is how many units hold it. `base` holds the
    k-grams' hashes and the numbers the base libraries write."""

    def __init__(self, names, prints, numbers, tokens=None, base=None):
        self.names, self.prints, self.numbers = names, prints, numbers
        self.tokens = tokens or {}
        self.base = set(base or ())
        self.print_spread = {h: len({(u[0], u[1]) for u in v}) for h, v in prints.items()}
        holders = {}
        for unit, values in numbers.items():
            for v in set(values):
                holders.setdefault(v, set()).add(unit)
        self.number_holders = holders
        self._split = {}

    def unit_tokens(self, unit):
        """The neutral token texts of a reference unit ("file:line")."""
        got = self._split.get(unit)
        if got is None:
            got = self._split[unit] = self.tokens.get(unit, "").split(" ")
        return got

    def to_json(self):
        return {"names": self.names, "prints": {str(h): v for h, v in self.prints.items()}, "numbers": self.numbers,
                "tokens": self.tokens, "base": sorted(self.base, key=repr)}

    @staticmethod
    def from_json(d):
        return Store(d["names"], {int(h): v for h, v in d["prints"].items()}, d["numbers"], d["tokens"], d["base"])


def reference_files(ref_root, place):
    """The reference's code files, relative to `ref_root`, under the place's `dirs` (all of it when none)."""
    return code_files(ref_root, place.get("dirs") or ["."], set(place.get("skip_dirs", [])))


def code_files(root, tops, skip=frozenset()):
    """The code files under `root`'s folders `tops`, relative to `root`, folders in `skip` and hidden ones left out."""
    out = []
    for top in tops:
        for dirpath, dirs, names in os.walk(os.path.join(root, top)):
            dirs[:] = sorted(d for d in dirs if d not in skip and not d.startswith("."))
            for n in sorted(names):
                if os.path.splitext(n)[1] in CODE_EXTENSIONS:
                    out.append(os.path.relpath(os.path.join(dirpath, n), root).replace(os.sep, "/"))
    return out


def base_of(repo_root, place, cfg):
    """The k-grams' hashes and the numbers of every base library's code (lab_reference.base, folders of this repository)."""
    out = set()
    trivial = set(float(x) for x in cfg["trivial_numbers"])
    for rel in code_files(repo_root, place.get("base", [])):
        src = Source(repo_root, rel)
        if src.binary or src.lang is None:
            continue
        for span in lab_units.spans(src, trivial):
            out.update(h for h, _at in lab_tokens.kgrams(span.tokens, cfg["k"]))
            out.update(span.values)
    return out


def build_store(ref_root, place, cfg, repo_root=None):
    names, prints, numbers, tokens = {}, {}, {}, {}
    trivial = set(float(x) for x in cfg["trivial_numbers"])
    for rel in reference_files(ref_root, place):
        src = Source(ref_root, rel)
        if src.binary or src.lang is None:
            continue
        for name, line in lab_units.api_names(src) + lab_units.program_names(src):
            norm = lab_tokens.normal_name(name, cfg["min_name_len"])
            if norm:
                names.setdefault(norm, []).append(f"{rel}:{line}")
        for span in lab_units.spans(src, trivial):
            marks = lab_tokens.fingerprints(span.tokens, cfg["k"], cfg["w"])
            for h, at in marks:
                prints.setdefault(h, []).append([rel, span.line, at])
            if marks:
                tokens[f"{rel}:{span.line}"] = " ".join(t for t, _line in span.tokens)
            if span.values:
                numbers[f"{rel}:{span.line}"] = span.values
    base = base_of(repo_root, place, cfg) if repo_root else set()
    return Store(names, prints, numbers, tokens, base)


def tree_digest(ref_root, place, cfg, repo_root):
    """The trees' part of the store's key: where they are read from, the settings the store is made with, and every
    reference and base file's path, size and modification time (os.stat, no byte read: the trees are outside git, whose
    blob ids key the repository's files, and hashing their bytes cost every run a read of both trees whole)."""
    settings = {k: cfg[k] for k in STORE_SETTINGS}
    h = hashlib.sha256(repr((ref_root, repo_root, sorted(settings.items()), place.get("dirs"), place.get("skip_dirs"),
                             place.get("base"))).encode("utf-8"))
    files = [(ref_root, rel) for rel in reference_files(ref_root, place)]
    files += [(repo_root, rel) for rel in code_files(repo_root, place.get("base", []))]
    for root, rel in files:
        st = os.stat(os.path.join(root, rel))
        h.update(f"{rel}\0{st.st_size}\0{st.st_mtime_ns}\n".encode("utf-8"))
    return h.hexdigest()


def reference_root(ctx):
    """The reference tree: `@name` is a location of this machine (tools/tasks/locations.py), else a path from the root;
    every base library must be there too."""
    place = ctx.rules["lab_reference"]
    try:
        root = locations.locate(place["place"][1:]) if place["place"].startswith("@") else os.path.join(ctx.root, place["place"])
    except locations.Missing as e:
        raise NoReference(str(e)) from e
    if not os.path.isdir(root):
        raise NoReference(f"the reference tree {root} is not there: set it (tools/tasks/locations.py)")
    for lib in place.get("base", []):
        if not os.path.isdir(os.path.join(ctx.root, lib)):
            raise NoReference(f"the base library {lib} is not there: build the DLLs (tools/build/build.sh), CMake fetches it")
    return root


def store_key(ctx):
    """The store's key, once a run: the content key of the sources the store is built by (STORE_SOURCES, not the gates'
    whole version) and the trees' digest."""
    def build():  # the key names a cache file: letters, digits and '_' only
        root = reference_root(ctx)
        digest = tree_digest(root, ctx.rules["lab_reference"], ctx.rules["lab_copy"], ctx.root)
        sources = ctx.cache.key_of(STORE_SOURCES, versioned=False)
        return "lab_prints_" + hashlib.sha256((sources + digest).encode()).hexdigest()
    return ctx.lazy("lab_prints key", build)


def load_store(ctx):
    key = store_key(ctx)
    root = reference_root(ctx)
    return ctx.lazy(key, lambda: Store.from_json(ctx.cache.get(key, lambda: build_store(
        root, ctx.rules["lab_reference"], ctx.rules["lab_copy"], ctx.root).to_json())))
