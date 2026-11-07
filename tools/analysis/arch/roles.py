"""What each tracked file is, told by behaviour alone: binary, sidecar, foreign (vendored or copied), generated, index, or the project's own."""
import collections
import math
import re

from . import workers
from .core import IDENT, natural_break, text_of, words_of
from .repo import reader

MENTION = re.compile(r"[\w.-]+\.[A-Za-z]\w*")
HEAD_COMMENT = re.compile(r"\A\s*(?://|#|/\*|\"\"\")\s*([^\n]+)")
# What adapters read as code: an index is a text about the repository, never a part of it.
CODE = {"cpp", "cc", "cxx", "c", "h", "hh", "hpp", "hxx", "inl", "gd", "gdshader", "gdshaderinc", "py", "ts", "tsx", "js", "jsx", "mjs", "cjs", "sh", "tscn", "tres", "godot", "cfg", "cmake"}
SPECIFIC = re.compile(r"[\w./\\-]*[/\\.][\w./\\-]+")


class Roles:
    def __init__(self):
        self.binary, self.sidecar, self.foreign, self.generated, self.index = set(), set(), set(), set(), set()
        self.foreign_dirs = []
        self.tokens = {}
        self.mentions = {}

    def own(self, tracked):
        """The files adapters read: everything that is the project's text."""
        out = set(tracked) - self.binary - self.sidecar - self.foreign
        return out

    def describe(self):
        return {"binary": len(self.binary), "sidecar": len(self.sidecar), "foreign": len(self.foreign), "generated": sorted(self.generated),
                "index": sorted(self.index), "foreign_dirs": self.foreign_dirs}


def _dirs_of(path):
    parts = path.split("/")[:-1]
    return ["/".join(parts[:i + 1]) for i in range(len(parts))]


def _activity(files, history):
    """Per folder: commits that touched its files after they arrived. A copy arrives and is left; code goes on changing, however many files join it."""
    arrival, touches = {}, collections.defaultdict(set)
    for i, (_, changes) in enumerate(history):
        for status, path, _, _ in changes:
            if path not in files:
                continue
            if status == "A" and path not in arrival:
                arrival[path] = i
            elif status == "M":
                touches[path].add(i)
    folders = collections.defaultdict(list)
    for f in files:
        for d in _dirs_of(f):
            folders[d].append(f)
    out = {}
    for d, members in folders.items():
        later = set()
        for f in members:
            later |= {c for c in touches.get(f, ()) if c > arrival.get(f, -1)}
        out[d] = len(later)
    return out


def _file_words(task):
    spec, items = task
    read = reader(spec)
    out = {}
    for key, blob in items:
        text = text_of(read(blob))
        out[key] = None if text is None else (set(IDENT.findall(text)), set(MENTION.findall(text)))
    return out


def _read_words(repo, tracked, cache):
    """Per file its words and the file names it mentions, or None for what is not text. A blob is read and searched once, on every core."""
    table = cache.table("words")
    keys = {p: repo.content_id(b) for p, b in tracked.items()}
    need = sorted({(k, tracked[p]) for p, k in keys.items() if k not in table})
    for key, blob in (x for x in need if x[1].startswith("v:")):
        text = repo.text(blob)
        table[key] = None if text is None else (set(IDENT.findall(text)), set(MENTION.findall(text)))
    rest = [x for x in need if not x[1].startswith("v:")]
    size = max(1, len(rest) // (workers.cores() * 4))
    spec = repo.spec()
    for part in workers.each(_file_words, [(spec, rest[i:i + size]) for i in range(0, len(rest), size)]):
        table.update(part)
    return {p: table[k] for p, k in keys.items()}


def classify(repo, tracked, history, cache):
    roles = Roles()
    names = set(tracked)
    known = _read_words(repo, tracked, cache)
    for path in sorted(tracked):
        if known[path] is None:
            roles.binary.add(path)
        elif path.rsplit(".", 1)[0] in names:
            roles.sidecar.add(path)
        else:
            roles.tokens[path], roles.mentions[path] = known[path]
    _index_files(roles, names)
    _foreign(roles, set(roles.tokens) | roles.binary, history)
    own = sorted(p for p in roles.tokens if p not in roles.foreign)
    repo.prefetch(tracked[p] for p in own)
    texts = {p: repo.text(tracked[p]) for p in own}
    _generated_heads(roles, texts)
    return roles, texts


def _together(files, history):
    """Per folder, the share of its files that came in its biggest single arrival: a library lands at once, code grows."""
    arrival = {}
    for i, (_, changes) in enumerate(history):
        for status, path, _, _ in changes:
            if status == "A" and path in files and path not in arrival:
                arrival[path] = i
    per = collections.defaultdict(collections.Counter)
    for f in files:
        for d in _dirs_of(f):
            per[d][arrival.get(f, -1)] += 1
    return {d: c.most_common(1)[0][1] / sum(c.values()) for d, c in per.items()}


def _outward(files, roles):
    """Per folder, the share of its files that name a file outside it, by its file name or by the type it declares (a name made of the
    same words as the file's). Only unique names count: a common one proves nothing."""
    base = collections.Counter(f.rsplit("/", 1)[-1] for f in files)
    where_is = {f.rsplit("/", 1)[-1]: f for f in files if base[f.rsplit("/", 1)[-1]] == 1}
    stems = collections.Counter()
    stem_of = {}
    for f in files:
        words = words_of(f.rsplit("/", 1)[-1].split(".")[0])
        if len(words) > 1:
            key = "".join(words)
            stems[key] += 1
            stem_of[key] = f
    reaching = collections.defaultdict(set)
    count = collections.Counter()
    for f in sorted(roles.tokens):
        dirs = _dirs_of(f)
        count.update(dirs)
        if f in roles.index:
            continue
        targets = [where_is[w] for w in roles.mentions[f] if w in where_is and where_is[w] != f]
        for token in roles.tokens[f]:
            key = "".join(words_of(token))
            if stems.get(key) == 1 and stem_of[key] != f:
                targets.append(stem_of[key])
        for d in dirs:
            if any(not p.startswith(d + "/") for p in targets):
                reaching[d].add(f)
    return {d: len(reaching[d]) / n for d, n in count.items()}


def _foreign(roles, files, history):
    """A folder is not the project's when nothing in it lives on (it arrived and was left, and so did every folder inside it), and its
    files do not know the rest of the repository: a library, a copy, records of past work."""
    rate = _activity(files, history)
    moving = [math.log(r) for r in rate.values() if r > 0]
    if not moving:
        return
    cut = math.exp(natural_break(moving)) if len(set(moving)) > 1 else moving[0]
    children = collections.defaultdict(set)
    for d in rate:
        if "/" in d:
            children[d.rsplit("/", 1)[0]].add(d)
    settled = {}

    def quiet(d):
        if d not in settled:
            settled[d] = rate[d] < cut and all(quiet(c) for c in sorted(children.get(d, ())))
        return settled[d]

    reach = _outward(files, roles)
    calm = {d: reach.get(d, 0.0) for d in rate if quiet(d)}
    limit = natural_break(list(calm.values())) if len(calm) > 1 else 0.0
    together = _together(files, history)
    whole = natural_break([together[d] for d in calm]) if len(calm) > 1 else 0.0
    for d in sorted(calm, key=lambda x: (x.count("/"), x)):
        if calm[d] < limit and together[d] >= whole and not any(d.startswith(k + "/") for k in roles.foreign_dirs):
            roles.foreign_dirs.append(d)
    keep = tuple(k + "/" for k in roles.foreign_dirs)
    roles.foreign = {f for f in files if f.startswith(keep)} if keep else set()


def _index_files(roles, names):
    """A text that names far more of the repository's files than any other is an index of it, not part of what it lists."""
    base = {n.rsplit("/", 1)[-1] for n in names}
    counts = {path: len(found & base) for path, found in roles.mentions.items()}
    if len(counts) > 1:
        cut = natural_break(list(counts.values()))
        roles.index = {p for p, v in counts.items() if v >= cut and p.rsplit(".", 1)[-1].lower() not in CODE}


def _generated_heads(roles, texts):
    """Files that open with the same sentence, paths aside, came out of one generator."""
    heads = collections.defaultdict(list)
    for path, text in texts.items():
        if path in roles.foreign:
            continue
        m = HEAD_COMMENT.match(text)
        if m:
            heads[SPECIFIC.sub("*", m.group(1))].append(path)
    for head, files in heads.items():
        if len(files) > 1 and "*" in head:
            roles.generated.update(files)
