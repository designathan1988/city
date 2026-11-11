"""The roadcraft's fingerprints, the store LAB_COPY holds a change against: every name the reference tree defines, in its
normal form, the winnowed k-gram fingerprints of its code read as language-neutral tokens (lab_tokens.py), and the runs of its
non-trivial numbers; kept by the content of every file read, so the tree is read again only when it changed."""
import hashlib
import os
import sys

import lab_tokens
from source import Source

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import locations  # noqa: E402  the one owner of this machine's paths

CODE_EXTENSIONS = frozenset({".ts", ".tsx", ".js", ".mjs", ".cpp", ".h", ".hpp", ".gd"})


class Store:
    """The reference's names (normal form -> ["file:line"]), fingerprints (hash -> [(file, line)]) and number runs
    (values -> [(file, line)])."""

    def __init__(self, names, prints, numbers):
        self.names, self.prints, self.numbers = names, prints, numbers

    def to_json(self):
        return {"names": self.names, "prints": {str(h): v for h, v in self.prints.items()},
                "numbers": {"|".join(repr(x) for x in key): v for key, v in self.numbers.items()}}

    @staticmethod
    def from_json(d):
        return Store(d["names"], {int(h): [tuple(x) for x in v] for h, v in d["prints"].items()},
                     {tuple(float(x) for x in key.split("|")): [tuple(x) for x in v] for key, v in d["numbers"].items()})


def reference_files(ref_root, cfg):
    """The reference's code files, relative to `ref_root`, under its `reference_dirs` (all of it when none)."""
    skip = set(cfg.get("skip_dirs", []))
    tops = cfg.get("reference_dirs") or ["."]
    out = []
    for top in tops:
        for dirpath, dirs, names in os.walk(os.path.join(ref_root, top)):
            dirs[:] = sorted(d for d in dirs if d not in skip and not d.startswith("."))
            for n in sorted(names):
                if os.path.splitext(n)[1] in CODE_EXTENSIONS:
                    out.append(os.path.relpath(os.path.join(dirpath, n), ref_root).replace(os.sep, "/"))
    return out


def build_store(ref_root, cfg):
    names, prints, numbers = {}, {}, {}
    trivial = set(float(x) for x in cfg["trivial_numbers"])
    for rel in reference_files(ref_root, cfg):
        src = Source(ref_root, rel)
        if src.binary or src.lang is None:
            continue
        for name, line in lab_tokens.defined_at(src).items():
            norm = lab_tokens.normal_name(name, cfg["min_name_len"])
            if norm:
                names.setdefault(norm, []).append(f"{rel}:{line}")
        for h, line in lab_tokens.fingerprints(lab_tokens.neutral_tokens(src), cfg["k"], cfg["w"]):
            prints.setdefault(h, []).append((rel, line))
        for key, line in lab_tokens.number_runs(src, trivial, cfg["number_run"]):
            numbers.setdefault(key, []).append((rel, line))
    return Store(names, prints, numbers)


def tree_digest(ref_root, cfg):
    """The key of the store: every reference file's path and bytes, and the settings it was built with."""
    h = hashlib.sha256(repr(sorted((k, v) for k, v in cfg.items() if not k.startswith("_"))).encode("utf-8"))
    for rel in reference_files(ref_root, cfg):
        h.update(rel.encode("utf-8"))
        with open(os.path.join(ref_root, rel), "rb") as fh:
            h.update(hashlib.sha256(fh.read()).digest())
    return h.hexdigest()


def reference_root(ctx, cfg):
    """The reference tree: `@name` is a location of this machine (tools/tasks/locations.py), else a path under the root."""
    ref = cfg["reference"]
    if ref.startswith("@"):
        return locations.locate(ref[1:])
    return os.path.join(ctx.root, ref)


def load_store(ctx):
    cfg = ctx.rules["lab_copy"]
    root = reference_root(ctx, cfg)
    if not os.path.isdir(root):
        raise FileNotFoundError(f"the reference tree {root} is not there: set it (tools/tasks/locations.py)")
    key = "lab_prints:" + tree_digest(root, cfg)
    return ctx.lazy(key, lambda: Store.from_json(ctx.cache.get(key, lambda: build_store(root, cfg).to_json())))
