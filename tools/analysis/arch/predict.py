"""What changes together, learned only from the past of each moment: at every commit, what the repository knew just before it predicts it.
Evidence is shared rare words and shared commits; how much each counts is learned from the commits already seen."""
import collections
import math

from .cache import Cache
from . import roles
from .repo import Repo


class Model:
    """Word postings and co-change, growing commit by commit."""

    def __init__(self):
        self.words = {}
        self.posting = collections.defaultdict(set)
        self.together = collections.defaultdict(collections.Counter)
        self.touches = collections.Counter()
        self.number = {}
        self.names = []
        self.holders = {}

    def update_file(self, path, words):
        """`words` of the new version, None when the file is gone or is not text."""
        old = self.words.get(path, set())
        new = words if words is not None else set()
        if path not in self.number:
            self.number[path] = len(self.names)
            self.names.append(path)
        for w in old - new:
            self.posting[w].discard(path)
            self.holders.pop(w, None)
        for w in new - old:
            self.posting[w].add(path)
            self.holders.pop(w, None)
        if words is None:
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

    def word_sums(self, path):
        """Files sharing rare words with this one, by number; a word's worth is one over how many files have it, which needs no corpus size."""
        import numpy
        files, sizes = [], []
        for w in sorted(self.words.get(path, ())):  # sums in one order: a tie of ranks never depends on the run
            many = len(self.posting[w])
            if 1 < many < len(self.words) + 1:  # a word in nearly every file says nothing
                if w not in self.holders:
                    self.holders[w] = numpy.fromiter((self.number[f] for f in self.posting[w]), numpy.int64, many)
                files.append(self.holders[w])
                sizes.append(many)
        if not files:
            return numpy.zeros(len(self.names))
        sizes = numpy.array(sizes)
        totals = numpy.bincount(numpy.concatenate(files), numpy.repeat(1.0 / (sizes - 1), sizes), len(self.names))
        totals[self.number[path]] = 0
        return totals

    def lexical(self, path):
        import numpy
        totals = self.word_sums(path)
        return {self.names[i]: float(totals[i]) for i in numpy.flatnonzero(totals)}

    def together_sums(self, path):
        import numpy
        totals = numpy.zeros(len(self.names))
        met = self.together.get(path)
        if met:
            totals[[self.number[f] for f in met]] = list(met.values())
        return totals

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


def _rank_all(totals):
    """`_ranks` over an array, for what has a score: ties share their middle rank, nothing else differs."""
    import numpy
    out = numpy.zeros(len(totals))
    present = numpy.flatnonzero(totals)
    n = len(present)
    if n:
        order = numpy.argsort(-totals[present], kind="stable")
        ordered = totals[present][order]
        first = numpy.flatnonzero(numpy.r_[True, ordered[1:] != ordered[:-1]])
        last = numpy.r_[first[1:], n] - 1
        out[present[order]] = numpy.repeat(1.0 - ((first + last) / 2.0) / n, last - first + 1)
    return out


def _reciprocal_all(scores, wanted):
    """`reciprocal` over an array: what has no score is no candidate."""
    best = scores[wanted].max() if len(wanted) else 0.0
    if best <= 0:
        return 0.0
    return 1.0 / (float((scores > best).sum()) + (float((scores == best).sum()) + 1) / 2.0)


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


def _folder(path):
    return path.rsplit("/", 1)[0] if "/" in path else ""


def rolling(repo, history, keep, cache):
    """Rolling origin: every commit is predicted from everything before it, and then learned. Returns per-commit scores for the model,
    the folder and chance, comparable commit by commit."""
    m = Model()
    rows = []
    wins = collections.Counter({"lex": 1.0, "co": 1.0})
    versions = roles.words_of_versions(repo, [c[2] for _, changes in history for c in changes if c[1] in keep and c[0] != "D"], cache)
    for sha, changes in history:
        touched = [c for c in changes if c[1] in keep]
        known = [c[1] for c in touched if c[1] in m.words]
        if 1 < len(set(known)):
            weight = wins["lex"] / (wins["lex"] + wins["co"])
            files = sorted(set(known))
            size = len(m.words)
            folders = collections.Counter(_folder(g) for g in m.words)
            for f in files:
                others = sorted(set(files) - {f})
                wanted = [m.number[g] for g in others]
                by_words = _rank_all(m.word_sums(f))
                by_commits = _rank_all(m.together_sums(f))
                model = _reciprocal_all(weight * by_words + (1.0 - weight) * by_commits, wanted)
                # Every file of a folder scores the same: the rank of the first of them is the middle of that tie.
                near = any(_folder(g) == _folder(f) for g in others)
                by_folder = 1.0 / ((folders[_folder(f)] - 1 + 1) / 2.0) if near else 0.0
                chance = len(others) / max(size - 1, 1)
                rows.append((model, by_folder, chance, 1.0 / len(files)))
                wins["lex"] += _reciprocal_all(by_words, wanted)
                wins["co"] += _reciprocal_all(by_commits, wanted)
        for status, path, blob, _ in touched:
            m.update_file(path, None if status == "D" or versions[blob] is None else versions[blob][0])
        m.learn(c[1] for c in touched if c[0] != "D")
    return rows, m, wins


def predict_for(path, ident):
    """The files most likely to change with a file, given everything the repository knows now."""
    repo = Repo(path)
    tracked = repo.files()
    history = repo.history()
    found, _ = roles.classify(repo, tracked, history, Cache("", False))
    keep = found.own(tracked) - found.index
    _, m, wins = rolling(repo, history, keep, Cache("", False))
    weight = wins["lex"] / (wins["lex"] + wins["co"])
    target = next((p for p in sorted(keep) if p == ident or p.endswith(ident)), None)
    if target is None:
        return []
    top = m.scored(target, weight)
    best = sorted(top.items(), key=lambda kv: (-kv[1], kv[0]))
    cut = int(math.sqrt(len(best))) + 1
    return best[:cut]
