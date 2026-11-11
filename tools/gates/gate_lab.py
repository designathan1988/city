"""LAB_COPY: the roadcraft lab is read for behaviour only and never copied, renamed or not (CLAUDE.md, owner 2026-11-11). A
change is judged where it is written: every function a changed line falls in, the changed code outside functions and the
programs in strings it touches (lab_units.py), against the reference's store (lab_prints.py). A piece is refused when the
passages it shares with one reference unit (each position of either side used once: greedy string tiling, as JPlag
compares programs) hold informative k-grams worth min_tokens tokens and min_share of all of its own; or when one
reference unit holds min_numbers of its non-trivial numbers in their order. A k-gram or a number spread over more than
max_spread reference units is an idiom (MOSS's -m) and is no evidence, nor is a k-gram that repeats a short pattern (a
list's shape). A name the change defines
for others (outside function bodies) is refused when its normal form is one the reference defines. Values are what the
game must do and stand in data; a comment citing the reference is no code. Lines the change does not touch are the scan's
(tools/gates/lab_scan.py)."""
import os

import lab_prints
import lab_tokens
import lab_units


class Piece:
    """What is judged of one span: the line it is known by, its neutral token texts, its informative k-grams [(hash,
    token index)] and its non-trivial numbers in order."""
    __slots__ = ("line", "tokens", "grams", "numbers")

    def __init__(self, line, tokens=(), grams=(), numbers=()):
        self.line, self.tokens, self.grams, self.numbers = line, list(tokens), list(grams), list(numbers)


class Match:
    __slots__ = ("kind", "ref", "measure")

    def __init__(self, kind, ref, measure):
        self.kind, self.ref, self.measure = kind, ref, measure

    def __repr__(self):
        return f"Match({self.kind}, {self.ref}, {self.measure})"


class Verdict:
    """A judged piece: its line, the share and the count of its informative k-grams its best reference unit's passages
    hold, its most numbers in one reference unit's order, and the matches past the thresholds."""
    __slots__ = ("line", "share", "covered", "numbers", "matches")

    def __init__(self, line, share, covered, numbers, matches):
        self.line, self.share, self.covered, self.numbers, self.matches = line, share, covered, numbers, matches


def in_paths(cfg, path):
    """Whether a file is of the rule's paths and extensions."""
    return os.path.splitext(path)[1] in cfg["extensions"] and any(path == p or path.startswith(p + "/") for p in cfg["paths"])


def applies(ctx, path):
    return path in ctx.changed and in_paths(ctx.rules["lab_copy"], path)


def store_key(ctx):
    """What a verdict depends on besides the file: the store, or the reason there is none."""
    try:
        return [lab_prints.store_key(ctx)]
    except lab_prints.NoReference as e:
        return [f"no reference: {e}"]


def tiled(ours, theirs, seeds, informative, k):
    """How many distinct informative k-grams of `ours` ({first index: hash}) lie in the passages it shares with `theirs`:
    each matched k-gram (our index, their index) grown token by token both ways to the whole equal passage, a passage
    weighed by the distinct informative k-grams it holds (a list repeats the same ones, a stretch of a short pattern holds
    none); laid heaviest first, a position of either side used once (greedy string tiling)."""
    found = set()
    for p, u in seeds:
        while p > 0 and u > 0 and ours[p - 1] == theirs[u - 1]:
            p, u = p - 1, u - 1
        n = 0
        while p + n < len(ours) and u + n < len(theirs) and ours[p + n] == theirs[u + n]:
            n += 1
        weight = len({informative[q] for q in range(p, p + n - k + 1) if q in informative})
        if weight:
            found.add((weight, p, u, n))
    used_ours, used_theirs, covered = [], [], 0
    for weight, p, u, n in sorted(found, key=lambda m: (-m[0], m[1], m[2])):
        if any(p < y and x < p + n for x, y in used_ours) or any(u < y and x < u + n for x, y in used_theirs):
            continue
        used_ours.append((p, p + n))
        used_theirs.append((u, u + n))
        covered += weight
    return covered


def best_share(store, cfg, piece):
    """(share, covered, 'file:line') of the reference unit whose shared passages hold the most of the piece's informative
    k-grams, idioms left out: covered is how many they hold, share what part of all of them that is."""
    informative = {p: h for h, p in piece.grams if h not in store.base}
    seeds = {}
    for h, p in piece.grams:
        if store.print_spread.get(h, 0) > cfg["max_spread"] or h in store.base:
            continue
        for f, line, u in store.prints.get(h, ()):
            seeds.setdefault(f"{f}:{line}", []).append((p, u))
    best, where = 0, None
    for unit in sorted(seeds):
        covered = tiled(piece.tokens, store.unit_tokens(unit), seeds[unit], informative, cfg["k"])
        if covered > best:
            best, where = covered, unit
    own = len(set(informative.values()))
    return (min(1.0, best / own) if own else 0.0), best, where


def _in_order(a, b):
    """How many values `a` and `b` share in the same order (their longest common subsequence)."""
    row = [0] * (len(b) + 1)
    for x in a:
        prev = 0
        for j, y in enumerate(b, 1):
            cur = row[j]
            row[j] = prev + 1 if x == y else max(row[j], row[j - 1])
            prev = cur
    return row[-1]


def best_numbers(store, cfg, piece):
    """(count, 'file:line') of the reference unit holding the most of the piece's numbers in their order, idioms left out."""
    values = [v for v in piece.numbers if len(store.number_holders.get(v, ())) <= cfg["max_spread"] and v not in store.base]
    units = {u for v in set(values) for u in store.number_holders.get(v, ())}
    best, where = 0, None
    for unit in sorted(units):
        n = _in_order(values, store.numbers[unit])
        if n > best:
            best, where = n, unit
    return best, where


def matches(store, cfg, piece):
    """[Match] of one piece past the thresholds: shared passages, numbers in order."""
    out = []
    share, covered, unit = best_share(store, cfg, piece)
    if covered + cfg["k"] - 1 >= cfg["min_tokens"] and share >= cfg["min_share"]:
        out.append(Match("passages", unit, share))
    count, unit = best_numbers(store, cfg, piece)
    if count >= cfg["min_numbers"]:
        out.append(Match("numbers", unit, count))
    return out


def judge(ctx, src):
    """[Verdict] of every piece of `src` the change touches."""
    cfg = ctx.rules["lab_copy"]
    store = lab_prints.load_store(ctx)
    lines = ctx.changed.get(src.path)
    trivial = set(float(x) for x in cfg["trivial_numbers"])
    out = []
    for span in lab_units.spans(src, trivial, lines):
        piece = Piece(span.line, [t for t, _l in span.tokens], lab_tokens.kgrams(span.tokens, cfg["k"], informative=True),
                      span.values)
        share, covered, _u = best_share(store, cfg, piece)
        count, _u = best_numbers(store, cfg, piece)
        out.append(Verdict(span.line, share, covered, count, matches(store, cfg, piece)))
    return out


def findings(ctx, src):
    """{line: [(what, the reference's place)]} of the pieces and the names of `src` the change touches that are past the
    rule; raises lab_prints.NoReference when the reference is not there."""
    cfg = ctx.rules["lab_copy"]
    store = lab_prints.load_store(ctx)
    lines = ctx.changed.get(src.path)
    found = {}
    for name, line in lab_units.api_names(src) + lab_units.program_names(src):
        norm = lab_tokens.normal_name(name, cfg["min_name_len"])
        if norm in store.names and (lines is None or line in lines):
            found.setdefault(line, []).append((f"defines '{name}', a name the reference defines", store.names[norm][0]))
    for v in judge(ctx, src):
        for m in v.matches:
            what = (f"its passages shared with the reference cover {m.measure:.0%} of it" if m.kind == "passages"
                    else f"{m.measure} of its numbers stand in the reference in the same order")
            found.setdefault(v.line, []).append((what, m.ref))
    return found


def check_lab_copy(ctx, src, err):
    if src.binary:
        return
    try:
        found = findings(ctx, src)
    except lab_prints.NoReference as e:
        err("LAB_COPY", src.path, f"cannot be judged without the reference: {e}")
        return
    for line in sorted(found):
        err("LAB_COPY", f"{src.path}:{line}", "; ".join(f"{what} ({ref})" for what, ref in found[line])
            + ": the roadcraft is read for what the game does, never copied; design this for this game (CLAUDE.md, "
            "log/how-we-work.md)")
