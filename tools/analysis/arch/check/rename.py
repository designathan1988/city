"""A bijection over the project's own words. Words are renamed one by one inside identifiers, so names a macro pastes together still meet, and the
same map runs over code, data, paths and strings. Whatever the language, the engine or a library owns is left alone."""
import glob
import os
import pickle
import re
import sys

from ..adapters import cpp_build
from ..core import IDENT, digest, program_root, words_of

PIECE = re.compile(r"[A-Z]+(?![a-z])|[A-Z]?[a-z]+|[0-9]+|[^A-Za-z0-9]+")
MAPPED = re.compile(r"[Qq][Xx][A-Za-z]{10}")
MANGLED = re.compile(r"(\d+)([A-Za-z_]\w*)")


def outside_vocabulary(repo, tracked, roles, api):
    """Every word anything outside the project defines: library headers, the compiler's, the standard library, foreign folders, the engine."""
    folder = os.path.join(program_root(), "dist", "arch", "external")
    memo = os.path.join(folder, "vocabulary.pkl")
    tokens = set()
    if os.path.exists(memo):
        with open(memo, "rb") as f:
            tokens = pickle.load(f)
    else:
        dirs = set()
        for entries in cpp_build._entries(repo).values():
            for words in [w for w in map(cpp_build._split_words, entries) if w and "++" in os.path.basename(w[0])][:1]:
                dirs |= {w[2:] for w in words if w.startswith("-I") and "/src" not in w}
                dirs |= {words[i + 1] for i, w in enumerate(words[:-1]) if w == "-isystem"}
                base = os.path.dirname(os.path.dirname(words[0]))
                dirs |= {os.path.join(base, "include"), os.path.join(base, "lib", "clang")}
        files = []
        for d in sorted(dirs):
            files += [p for p in glob.glob(os.path.join(d, "**", "*"), recursive=True) if os.path.isfile(p)]
        files += glob.glob(os.path.join(os.path.dirname(os.__file__), "*.py"))
        files += glob.glob(os.path.join(folder, "*.rst"))
        files += glob.glob(os.path.join(folder, "node_modules", "typescript", "lib", "lib.*.d.ts"))
        for path in files:
            with open(path, "rb") as f:
                tokens.update(IDENT.findall(f.read().decode("latin1")))
        os.makedirs(folder, exist_ok=True)
        with open(memo, "wb") as f:
            pickle.dump(tokens, f)
    tokens = set(tokens) | set(dir(__builtins__)) | set(sys.builtin_module_names) | set(api.words) | set(api.props) | set(api.classes)
    for path in sorted(roles.foreign):
        text = roles.tokens.get(path)
        if text:
            tokens |= text
    return tokens


class Renamer:
    def __init__(self, project_tokens, outside):
        self.outside = outside
        self.memo = {}
        kept = set()
        for t in outside:
            kept.update(words_of(t))
        self.forward, self.back = {}, {}
        for token in sorted(project_tokens - outside):
            for w in words_of(token):
                if w not in kept and w.isalpha() and len(w) > 1 and w not in self.forward:
                    fresh = "qx" + "".join(chr(97 + int(c, 16) % 26) for c in digest(w)[:10])
                    self.forward[w] = fresh
                    self.back[fresh] = w

    def _piece(self, p):
        if not p[0].isalpha():
            return p
        m = self.forward.get(p.lower())
        if m is None:
            return p
        if p.isupper() and len(p) > 1:
            return m.upper()
        return m.capitalize() if p[0].isupper() else m

    def token(self, t):
        found = self.memo.get(t)
        if found is None:
            found = t if t in self.outside else "".join(self._piece(p) for p in PIECE.findall(t))
            self.memo[t] = found
        return found

    def text(self, text):
        return IDENT.sub(lambda m: self.token(m.group(0)), text)

    def path(self, path):
        head, _, tail = path.rpartition("/")
        stem, dot, ext = tail.partition(".")
        moved = "/".join(self.token(p) for p in head.split("/")) if head else ""
        name = self.token(stem) + dot + ext
        return f"{moved}/{name}" if moved else name

    def undo(self, text):
        def one(m):
            w = m.group(0)
            base = self.back.get(w.lower())
            if base is None:
                return w
            if w.isupper():
                return base.upper()
            return base.capitalize() if w[0].isupper() else base
        return MAPPED.sub(one, text)

    def profile(self, text):
        """Mangled names carry each identifier's length: the map runs inside them and the lengths follow."""
        def one(m):
            size = int(m.group(1))
            word, rest = m.group(2)[:size], m.group(2)[size:]
            if len(m.group(2)) < size:
                return m.group(0)
            fresh = self.token(word)
            return f"{len(fresh)}{fresh}{rest}"
        return "\n".join(MANGLED.sub(one, line) if line.startswith("_Z") or ";_Z" in line else line for line in text.split("\n"))
