"""The roadcraft's fingerprints, the store LAB_COPY holds a change against: every name the reference tree defines for others,
in its normal form, the winnowed k-gram fingerprints of each of its units (a function body, the code outside them, a program
in a string: lab_units.py) read as language-neutral tokens (lab_tokens.py), and each unit's non-trivial numbers in order;
kept by the gates' own version and the content of every file read, so the tree is read again only when either changed."""
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

CODE_EXTENSIONS = frozenset({".ts", ".tsx", ".js", ".mjs", ".cpp", ".h", ".hpp", ".gd"})
STORE_SETTINGS = ("k", "w", "min_name_len", "trivial_numbers")  # what of rules.json lab_copy the store is made with


class NoReference(Exception):
    """The reference tree is not on this machine: nothing can be judged against it."""


class Store:
    """The reference's names (normal form -> ["file:line"]), fingerprints (hash -> [[file, unit line, token index in the
    unit]]), units' numbers ("file:unit line" -> [values]) and units' neutral tokens ("file:unit line" -> "D I = ...", what
    a shared passage is grown along); a fingerprint's and a number's spread is how many units hold it."""

    def __init__(self, names, prints, numbers, tokens=None):
        self.names, self.prints, self.numbers = names, prints, numbers
        self.tokens = tokens or {}
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
                "tokens": self.tokens}

    @staticmethod
    def from_json(d):
        return Store(d["names"], {int(h): v for h, v in d["prints"].items()}, d["numbers"], d["tokens"])


def reference_files(ref_root, place):
    """The reference's code files, relative to `ref_root`, under the place's `dirs` (all of it when none)."""
    skip = set(place.get("skip_dirs", []))
    out = []
    for top in place.get("dirs") or ["."]:
        for dirpath, dirs, names in os.walk(os.path.join(ref_root, top)):
            dirs[:] = sorted(d for d in dirs if d not in skip and not d.startswith("."))
            for n in sorted(names):
                if os.path.splitext(n)[1] in CODE_EXTENSIONS:
                    out.append(os.path.relpath(os.path.join(dirpath, n), ref_root).replace(os.sep, "/"))
    return out


def build_store(ref_root, place, cfg):
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
    return Store(names, prints, numbers, tokens)


def tree_digest(ref_root, place, cfg):
    """The reference's key: every file's path and bytes, where it was read from and the settings the store is made with."""
    settings = {k: cfg[k] for k in STORE_SETTINGS}
    h = hashlib.sha256(repr((sorted(settings.items()), place.get("dirs"), place.get("skip_dirs"))).encode("utf-8"))
    for rel in reference_files(ref_root, place):
        h.update(rel.encode("utf-8"))
        with open(os.path.join(ref_root, rel), "rb") as fh:
            h.update(hashlib.sha256(fh.read()).digest())
    return h.hexdigest()


def reference_root(ctx):
    """The reference tree: `@name` is a location of this machine (tools/tasks/locations.py), else a path from the root."""
    place = ctx.rules["lab_reference"]["place"]
    try:
        root = locations.locate(place[1:]) if place.startswith("@") else os.path.join(ctx.root, place)
    except locations.Missing as e:
        raise NoReference(str(e)) from e
    if not os.path.isdir(root):
        raise NoReference(f"the reference tree {root} is not there: set it (tools/tasks/locations.py)")
    return root


def store_key(ctx):
    """The store's key, once a run: the gates' own version and the reference's digest."""
    def build():  # the key names a cache file: letters, digits and '_' only
        root = reference_root(ctx)
        return "lab_prints_" + hashlib.sha256((ctx.cache.version + tree_digest(root, ctx.rules["lab_reference"],
                                                                                 ctx.rules["lab_copy"])).encode()).hexdigest()
    return ctx.lazy("lab_prints key", build)


def load_store(ctx):
    key = store_key(ctx)
    root = reference_root(ctx)
    return ctx.lazy(key, lambda: Store.from_json(ctx.cache.get(key, lambda: build_store(
        root, ctx.rules["lab_reference"], ctx.rules["lab_copy"]).to_json())))
