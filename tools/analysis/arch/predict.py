"""What changes together, learned only from the past of each moment: at every commit, what the repository knew just before it predicts it.
Evidence is shared rare words and shared commits; how much each counts is learned from the commits already seen."""
import collections
import math

from .cache import Cache
from .core import IDENT
from .repo import Repo


class Model:
    """Word postings and co-change, growing commit by commit."""

    def __init__(self):
        self.words = {}
        self.posting = collections.defaultdict(set)
        self.together = collections.defaultdict(collections.Counter)
        self.touches = collections.Counter()

    def update_file(self, path, text):
        old = self.words.get(path, set())
        new = set(IDENT.findall(text)) if text is not None else set()
        for w in old - new:
            self.posting[w].discard(path)
        for w in new - old:
            self.posting[w].add(path)
        if text is None:
            self.words.pop(path, None)
        else:
            self.words[path] = new

    def learn(self, files):
        files = sorted(files)
        self.touches.update(files)
        if len(files) > 1:
            share = 1.0 / (len(files) - 1)
            for a in files:
                for b in files:
                    if a != b:
                        self.together[a][b] += share

    def lexical(self, path):
        """Files sharing rare words with this one; a word's worth is one over how many files have it, which needs no corpus size."""
        out = collections.Counter()
        for w in self.words.get(path, ()):
            users = self.posting[w]
            if 1 < len(users):
                worth = 1.0 / (len(users) - 1)
                if worth * len(self.words) > 1:  # a word in nearly every file says nothing
                    for f in users:
                        if f != path:
                            out[f] += worth
        return out

    def scored(self, path, weight):
        """Each kind of evidence as ranks, so neither scale decides; the weight comes from past commits."""
        lex = _ranks(self.lexical(path))
        co = _ranks(self.together.get(path, {}))
        out = collections.Counter()
        for f, r in lex.items():
            out[f] += weight * r
        for f, r in co.items():
            out[f] += (1.0 - weight) * r
        return out


def _ranks(scores):
    if not scores:
        return {}
    ordered = sorted(scores.items(), key=lambda kv: -kv[1])
    n = len(ordered)
    out, i = {}, 0
    while i < n:
        j = i
        while j + 1 < n and ordered[j + 1][1] == ordered[i][1]:
            j += 1
        for f, _ in ordered[i:j + 1]:
            out[f] = 1.0 - ((i + j) / 2.0) / n
        i = j + 1
    return out


def reciprocal(scores, wanted):
    """1 / rank of the first file of the commit in a ranking; ties share their middle rank, so names decide nothing."""
    if not scores:
        return 0.0
    values = sorted(scores.values(), reverse=True)
    best = max((scores.get(f, -1.0) for f in wanted), default=-1.0)
    if best < 0:
        return 0.0
    above = sum(1 for v in values if v > best)
    equal = sum(1 for v in values if v == best)
    return 1.0 / (above + (equal + 1) / 2.0)


def rolling(repo, history, keep):
    """Rolling origin: every commit is predicted from everything before it, and then learned. Returns per-commit scores for the model,
    the folder and chance, comparable commit by commit."""
    m = Model()
    rows = []
    wins = collections.Counter({"lex": 1.0, "co": 1.0})
    versions = repo.stream([c[2] for _, changes in history for c in changes if c[1] in keep and c[0] != "D"])
    for sha, changes in history:
        touched = [c for c in changes if c[1] in keep]
        known = [c[1] for c in touched if c[1] in m.words]
        if 1 < len(set(known)):
            weight = wins["lex"] / (wins["lex"] + wins["co"])
            files = sorted(set(known))
            size = len(m.words)
            for f in files:
                others = set(files) - {f}
                model = reciprocal(m.scored(f, weight), others)
                folder = f.rsplit("/", 1)[0] if "/" in f else ""
                near = {g: 1.0 for g in m.words if g != f and (g.rsplit("/", 1)[0] if "/" in g else "") == folder}
                by_folder = reciprocal(near, others)
                chance = len(others) / max(size - 1, 1)
                rows.append((model, by_folder, chance, 1.0 / len(files)))
                lex = reciprocal(_ranks(m.lexical(f)), others)
                co = reciprocal(_ranks(m.together.get(f, {})), others)
                wins["lex"] += lex
                wins["co"] += co
        for status, path, blob, _ in touched:
            m.update_file(path, None if status == "D" else next(versions))
        m.learn(c[1] for c in touched if c[0] != "D")
    return rows, m, wins


def predict_for(path, ident):
    """The files most likely to change with a file, given everything the repository knows now."""
    repo = Repo(path)
    from . import roles
    tracked = repo.files()
    history = repo.history()
    found, _ = roles.classify(repo, tracked, history, Cache("", False))
    keep = found.own(tracked) - found.index
    _, m, wins = rolling(repo, history, keep)
    weight = wins["lex"] / (wins["lex"] + wins["co"])
    target = next((p for p in sorted(keep) if p == ident or p.endswith(ident)), None)
    if target is None:
        return []
    top = m.scored(target, weight)
    best = sorted(top.items(), key=lambda kv: (-kv[1], kv[0]))
    cut = int(math.sqrt(len(best))) + 1
    return best[:cut]
