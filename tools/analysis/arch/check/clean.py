"""Cleanliness: no rare name of the analysed repository in the program's own text, and no gold file opened by a scan. Rare is the repository's own
measure (document frequency, cut where its sample breaks); what the language, the engine or a library defines is not the repository's."""
import ast
import glob
import importlib
import math
import os
import pickle
import re
import sys

from ..adapters import cpp_build
from ..core import IDENT, natural_break, program_root

WHOLE_WORDS = re.compile(r"\w*\}\w+|\w+\{\w*")


def _program_files():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    return sorted(glob.glob(os.path.join(here, "**", "*.py"), recursive=True)) + [here + ".py"]


def _libraries(text):
    out = set()
    for node in ast.walk(ast.parse(text)):
        names = [a.name for a in node.names] if isinstance(node, ast.Import) else [node.module] if isinstance(node, ast.ImportFrom) and node.level == 0 else []
        for name in names:
            if os.path.isdir(os.path.join(program_root(), "tools", name.split(".")[0])):
                continue  # the repository's own orchestration, imported by the program: its words are the repository's, never a library's
            module = sys.modules.get(name) or importlib.import_module(name)
            for word in dir(module):
                out.add(word)
                try:
                    out.update(dir(getattr(module, word)))
                except (AttributeError, ImportError, RuntimeError):
                    continue
    return out


def outside_vocabulary(repo, foreign_tokens, api_words):
    """Every word something outside the project defines: the compiler's and the libraries' headers, the foreign folders, the engine."""
    folder = os.path.join(program_root(), "dist", "arch", "external")
    memo = os.path.join(folder, "vocabulary.pkl")
    if os.path.exists(memo):
        with open(memo, "rb") as f:
            tokens = pickle.load(f)
    else:
        dirs = set()
        for entries in cpp_build._entries(repo).values():
            for words in [w for w in map(cpp_build._split_words, entries) if w and "++" in os.path.basename(w[0])][:1]:
                dirs |= {w[2:] for w in words if w.startswith("-I") and not os.path.normcase(w[2:]).startswith(os.path.normcase(repo.root))}
                dirs |= {words[i + 1] for i, w in enumerate(words[:-1]) if w == "-isystem"}
                base = os.path.dirname(os.path.dirname(words[0]))
                dirs |= {os.path.join(base, "include"), os.path.join(base, "lib", "clang")}
        tokens = set()
        files = [p for d in sorted(dirs) for p in glob.glob(os.path.join(d, "**", "*"), recursive=True) if os.path.isfile(p)]
        files += glob.glob(os.path.join(os.path.dirname(os.__file__), "*.py"))
        for path in files:
            with open(path, "rb") as f:
                tokens.update(IDENT.findall(f.read().decode("latin1")))
        os.makedirs(folder, exist_ok=True)
        with open(memo + ".tmp", "wb") as f:
            pickle.dump(tokens, f)
        os.replace(memo + ".tmp", memo)
    return set(tokens) | set(dir(__builtins__)) | set(sys.builtin_module_names) | set(api_words) | set(foreign_tokens)


def check(repo, base):
    """`base`: what the baseline scan left: the repository's words per file, its foreign files, its names, the files the scan opened."""
    outside = outside_vocabulary(repo, base["foreign_tokens"], base["api_words"])
    own = set()
    for path in _program_files():
        with open(path, encoding="utf8") as f:
            text = f.read()
        own.update(IDENT.findall(WHOLE_WORDS.sub(" ", text)))
        outside |= _libraries(text)
    df = {}
    for words in base["tokens"].values():
        for w in words:
            df[w] = df.get(w, 0) + 1
    project = {w for w in base["names"] - outside if w in df}
    cut = math.exp(natural_break([math.log(df[w]) for w in project])) if len(project) > 1 else 0
    rare = {w for w in project if df[w] <= cut}
    hits = sorted(w for w in own & rare if len(w) > 2)
    keys = [p for p in base["opened"] if "arch_eval" in p.replace("\\", "/")]
    ok = not hits and not keys and bool(base["opened"]) and bool(rare)
    return ok, {"rare_names": len(rare), "in_the_program": hits[:60], "gold_opened_by_scan": keys[:5], "files_opened": len(base["opened"])}
