"""The past of every piece of code: identity that survives renames, moves and splits, and which pieces changed in which commit."""
import collections
import math

from . import workers
from .core import digest, natural_break
from .repo import reader
from .spans import spans


class Lineage:
    __slots__ = ("id", "touched", "names", "born")

    def __init__(self, uid, born):
        self.id, self.touched, self.names, self.born = uid, [], [], born


class History:
    def __init__(self):
        self.lineages = []
        self.alive = {}      # (file, name) -> Lineage
        self.commits = []    # per commit: the lineages it really changed
        self.moved = 0
        self.ignored = set()

    def new(self, key, i):
        lin = Lineage(len(self.lineages), i)
        lin.names.append(key)
        self.lineages.append(lin)
        self.alive[key] = lin
        return lin


def _shapes(task):
    spec, items = task
    read = reader(spec)
    return {key: {k: v[2] for k, v in spans(path, read(blob)).items()} for key, path, blob in items}


def _key(repo, path, blob):
    return digest(path.rsplit(".", 1)[-1] + repo.content_id(blob))


def _read_all(repo, history, ignored, cache):
    """The shapes of every version any commit holds, each once, over every core: nothing in the walk of the commits waits for a file any more."""
    table = cache.table("spans")
    need = {}
    for _, changes in history:
        for status, path, new_blob, old_blob in changes:
            if path in ignored:
                continue
            for blob, there in ((new_blob, status != "D"), (old_blob, status != "A")):
                if there:
                    key = _key(repo, path, blob)
                    if key not in table:
                        need[key] = (key, path, blob)
    made = [x for x in need.values() if x[2].startswith("v:")]
    for key, path, blob in made:
        table[key] = {k: v[2] for k, v in spans(path, repo.blob(blob)).items()}
    rest = sorted(x for x in need.values() if not x[2].startswith("v:"))
    size = max(1, len(rest) // (workers.cores() * 8))
    spec = repo.spec()
    for part in workers.each(_shapes, [(spec, rest[i:i + size]) for i in range(0, len(rest), size)]):
        table.update(part)
    return table


def _wanted(path, ignored):
    return path not in ignored


def build(repo, history, ignored, cache, log):
    """Walks every commit once. A piece that disappears here and appears there with the same shape is the same piece; so is one that keeps
    its name inside a commit that moved it. Only a change of shape counts as a change: a rename or a move costs no history."""
    h = History()
    h.ignored = ignored
    table = _read_all(repo, history, ignored, cache)
    shapes = {}
    for i, (_, changes) in enumerate(history):
        gone, born, touched = {}, {}, set()
        for status, path, new_blob, old_blob in changes:
            if not _wanted(path, ignored):
                continue
            old = table[_key(repo, path, old_blob)] if status != "A" else {}
            new = table[_key(repo, path, new_blob)] if status != "D" else {}
            for name, sh in old.items():
                if name not in new:
                    gone[(path, name)] = sh
            for name, sh in new.items():
                if name not in old:
                    born[(path, name)] = sh
                elif old[name] != sh:
                    lin = h.alive.get((path, name)) or h.new((path, name), i)
                    lin.touched.append(i)
                    touched.add(lin.id)
                shapes[(path, name)] = sh
        by_shape = collections.defaultdict(list)
        by_name = collections.defaultdict(list)
        for key, sh in sorted(gone.items()):
            by_shape[sh].append(key)
            by_name[key[1]].append(key)
        for key, sh in sorted(born.items()):
            source = None
            if by_shape.get(sh):
                source = by_shape[sh].pop(0)
                by_name[source[1]].remove(source)
                h.moved += 1
            elif by_name.get(key[1]) and key[1]:
                source = by_name[key[1]].pop(0)
                by_shape[gone[source]].remove(source)
                h.moved += 1
            if source is not None and source in h.alive:
                lin = h.alive.pop(source)
                h.alive[key] = lin
                lin.names.append(key)
                if gone[source] != sh:
                    lin.touched.append(i)
                    touched.add(lin.id)
            else:
                lin = h.new(key, i)
                touched.add(lin.id)
        for key in by_shape.values():
            for k in key:
                h.alive.pop(k, None)
        h.commits.append(sorted(touched))
    log(f"history: {len(h.lineages)} lineages, {h.moved} moves, {sum(1 for c in h.commits if c)} commits with changes")
    return h


def giant_cut(h):
    """The size above which a commit is a mass event and not a change: the sample's own break, on the log of sizes."""
    sizes = [math.log(len(c)) for c in h.commits if len(c) > 1]
    if len(set(sizes)) < 2:
        return math.inf
    return math.exp(natural_break(sizes))


def cochange(h):
    """Pairs of lineages by the commits they share, each commit worth what its size allows; mass events say nothing about a pair."""
    cut = giant_cut(h)
    weight = collections.Counter()
    touches = collections.Counter()
    for c in h.commits:
        if len(c) < 2 or len(c) > cut:
            continue
        share = 1.0 / (len(c) - 1)
        touches.update(c)
        for a in range(len(c)):
            for b in range(a + 1, len(c)):
                weight[(c[a], c[b])] += share
    return {k: v / math.sqrt(touches[k[0]] * touches[k[1]]) for k, v in weight.items()}
