"""The catalogue of costs and the detectors of those that follow from repetition: what is paid every frame, every tick, in every thread."""
import collections
import re

from .context import api_words
from .core import words_of


class Costs:
    """Shared reads for the detectors: facts by kind, what guards what, and how a finding is written."""

    def __init__(self, run, when, atoms):
        self.run, self.when, self.atoms, self.graph = run, when, atoms, run.graph
        self.by_kind = collections.defaultdict(list)
        for c in run.facts.costs:
            self.by_kind[c[1]].append(c)
        self.writers = collections.defaultdict(set)
        for e in run.graph.edges.values():
            if e.kind == "write":
                self.writers[e.dst].add(e.src)
        self.calls_with = collections.defaultdict(list)
        for i, c in enumerate(self.by_kind["boundary"]):
            for word in set().union(*api_words(c[6]["api"])):
                self.calls_with[word].append(i)
        self.reads = collections.defaultdict(set)
        for e in run.graph.edges.values():
            if e.kind == "read":
                self.reads[run.graph.callable_of(e.src)].add(e.dst)
        self.guards = collections.defaultdict(list)
        for c in self.by_kind["guard"]:
            self.guards[c[0]].extend(x for x in c[6].get("on", ()) if x)

    def gated(self, uid):
        """Work behind a test of something only other code moves (a revision): it runs when its input changed, not every time."""
        for a in (uid, *self.graph.ancestors(uid)):
            for field in self.guards.get(a, ()):
                if self.writers.get(field) and not (self.writers[field] & self.when.repeats):
                    return True
        return False

    def repeated(self, c):
        return c[0] in self.when.repeats and c[0] not in self.when.tests and not self.gated(c[0])

    def finding(self, category, uid, file, line, **detail):
        context, root, steps = self.when.path(uid)
        attrs = self.graph.units[uid]["attrs"] if uid in self.graph.units else {}
        return {"category": category, "unit": uid, "file": file, "line": line, "context": context, "root": root, "path": steps, "detail": detail,
                "ran": {"count": attrs["ran"], "hot": bool(attrs.get("hot")), "stale": bool(attrs.get("stale"))} if "ran" in attrs else None}


NAMES = re.compile(r"\w+")
COLLECTION = {"vector", "map", "set", "list", "deque", "array", "dictionary", "queue"}


def is_collection(k, field):
    """State that holds many: what a loop over it costs grows with what it holds."""
    u = k.graph.units.get(field)
    if u is None:
        return False
    words = set(u["attrs"].get("words") or words_of((u["attrs"].get("type") or "").split("<")[0].split("[")[0]))
    return bool(words & COLLECTION) and not any(w.isdigit() for w in words) and not u["attrs"].get("constant")


def _boundary_with(k, words):
    return [k.by_kind["boundary"][i] for i in sorted({i for w in words for i in k.calls_with.get(w, ())})]


def grows_with_state(k):
    return [k.finding("grows_with_state", c[0], c[2], c[3], over=c[6]["over"]) for c in k.by_kind["loop"] if is_collection(k, c[6]["over"]) and not c[6]["early"] and k.repeated(c)]


def nested_loops(k):
    return [k.finding("nested_loops", c[0], c[2], c[3], depth=c[4]) for c in k.by_kind["loop"] if c[4] > 1 and k.repeated(c)]


def allocation(k):
    return [k.finding("allocation", c[0], c[2], c[3], what=c[6]["what"]) for c in k.by_kind["alloc"] if k.repeated(c)]


def lock_contention(k):
    """A lock taken on the repeating path that another context takes too: one waits for the other's whole step."""
    takers = collections.defaultdict(set)
    for c in k.by_kind["lock"]:
        takers[c[6]["mutex"]].update(k.when.labels(c[0]))
    return [k.finding("lock_contention", c[0], c[2], c[3], mutex=c[6]["mutex"]) for c in k.by_kind["lock"]
            if c[6]["mutex"] and len(takers[c[6]["mutex"]]) > 1 and k.repeated(c)]


def lock_held_work(k):
    """Loops and calls to the outside while a lock is held: whoever waits for it pays them too."""
    out = []
    for kind in ("loop", "boundary"):
        out += [k.finding("lock_held_work", c[0], c[2], c[3], under=c[5][-1], what=kind) for c in k.by_kind[kind] if c[5]]
    return out


def boundary_in_loop(k):
    return [k.finding("boundary_in_loop", c[0], c[2], c[3], api=c[6]["api"], depth=c[4]) for c in k.by_kind["boundary"] if c[4] > 0 and k.repeated(c)]


def _same_answer(k, uid):
    """A function that takes nothing and reads nothing the repeating path moves: all it can do is say the same thing again."""
    home = k.graph.callable_of(uid)
    takes = k.graph.units[home]["attrs"].get("inputs") if home in k.graph.units else 0
    return not takes and not any(k.writers.get(f, set()) & k.when.repeats for f in k.reads[home])


def redone_work(k):
    """A loop on the repeating path over state nothing on that path changes, with no other input: the same answer computed again."""
    out = []
    for c in k.by_kind["loop"]:
        field = c[6]["over"]
        if is_collection(k, field) and k.repeated(c) and k.writers.get(field) and not (k.writers[field] & k.when.repeats) and _same_answer(k, c[0]):
            out.append(k.finding("redone_work", c[0], c[2], c[3], over=field))
    return out


def copies(k):
    return [k.finding("copy", c[0], c[2], c[3], what=c[6]["what"]) for c in k.by_kind["copy"] if k.repeated(c)]


def linear_search(k):
    return [k.finding("linear_search", c[0], c[2], c[3], api=c[6]["api"]) for c in k.by_kind["search"] if c[4] > 0 and k.repeated(c)]


def query_per_frame(k):
    """A by-name question asked every frame of code that locks or loops, with no revision in front of it."""
    costly = {c[0] for c in k.by_kind["lock"]} | {c[0] for c in k.by_kind["loop"]}
    out = []
    for e in k.graph.edges.values():
        if e.kind in ("name_ref", "route", "name_call") and e.conf == 1.0 and (e.attrs.get("named") or e.kind != "name_ref") and e.src in k.when.reach["frame"] and e.src not in k.when.tests and not k.gated(e.src) and k.graph.units[e.src]["kind"] != "entry":
            target = e.dst
            hop = [x.dst for x in k.graph.out.get(target, ()) if x.kind in ("route", "name_ref")]
            for t in [target, *hop]:
                if t in costly:
                    out.append(k.finding("query_per_frame", e.src, *e.sites[0], target=t, text=e.attrs.get("text", "")))
                    break
    return out


def _engine_arrays(k):
    methods = k.run.api.methods
    return [c for c in k.by_kind["boundary"] if any("packed" in " ".join(t for _, t in args).lower() for _, args, _, _ in methods.get(c[6]["api"].rsplit(".", 1)[-1], ()))]


def gpu_upload(k):
    return [k.finding("gpu_upload", c[0], c[2], c[3], api=c[6]["api"]) for c in _engine_arrays(k) if k.repeated(c)]


def gpu_pass(k):
    return [k.finding("gpu_pass", c[0], c[2], c[3], hint=c[6]["hint"]) for c in k.by_kind["screen_read"]]


def shadow(k):
    return [k.finding("shadow", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"shadow", "shadows"}) if c[4] > 0 or k.repeated(c)]


def material(k):
    return [k.finding("material", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"material", "shader"}) if k.repeated(c)]


def variants(k):
    per = collections.defaultdict(list)
    for c in k.by_kind["variant"]:
        per[c[0]].append(c[6]["condition"])
    # One switch is two programs; it takes several for the count of pipelines to matter.
    return [k.finding("variants", u, k.graph.units[u]["file"], 1, count=len(v), conditions=sorted(set(v))) for u, v in sorted(per.items()) if u in k.graph.units and len(set(v)) > 1]


def _on_the_stack(k, field, held):
    """A field of a record no field holds, made as a local or plain data: what it takes goes when the record goes."""
    graph = k.graph
    owner = graph.units.get(field, {}).get("parent")
    if owner not in graph.units or graph.units[owner]["name"] in held:
        return False
    return any(e.kind == "raii" for e in graph.into.get(owner, ())) or not any(graph.units[c]["kind"] in ("method", "function") for c in graph.children.get(owner, ()))


def memory_growth(k):
    """State that takes entries on the repeating path and never gives any back. Sizing a field to a number is not taking entries."""
    grown = collections.defaultdict(list)
    shrunk = set()
    held = {w for u in k.graph.units.values() if u["kind"] in ("field", "global") for w in NAMES.findall(u["attrs"].get("type") or "")}
    for c in k.by_kind["grow"]:
        field = c[6].get("field")
        if field and not c[6].get("sized") and k.repeated(c) and not _on_the_stack(k, field, held):
            grown[field].append(c)
    for c in k.by_kind["shrink"]:
        if c[6].get("field"):
            shrunk.add(c[6]["field"])
    return [k.finding("memory_growth", v[0][0], v[0][2], v[0][3], field=f) for f, v in sorted(grown.items()) if f not in shrunk]


def streaming(k):
    """Jobs started on the repeating path with no budget in front: the frame's cost is whatever happens to be pending."""
    out = []
    for e in k.graph.edges.values():
        if e.kind == "stored":
            parent = k.graph.units.get(e.src, {}).get("parent")
            if parent in k.when.repeats and not k.gated(parent):
                out.append(k.finding("streaming", parent, *e.sites[0], job=e.src))
    return out


def loading(k):
    """Heavy work on the opening path that runs in sequence on the main thread, or is decoded twice."""
    boot = k.when.reach["boot"]
    heavy = {c[0]: c for c in k.by_kind["loop"] if c[4] > 1 and is_collection(k, c[6]["over"])}
    out = []
    callers = collections.Counter()
    for e in k.graph.edges.values():
        if e.kind == "call" and e.src in boot and e.src not in k.when.tests and e.dst in heavy:
            callers[e.dst] += 1
    for uid, n in sorted(callers.items()):
        c = heavy[uid]
        if n > 1 and uid not in k.when.tests:
            out.append(k.finding("loading", uid, c[2], c[3], callers=n))
    return out


CATALOGUE = (
    ("grows_with_state", "work that grows with the state", grows_with_state), ("nested_loops", "nested loops", nested_loops),
    ("allocation", "allocation on a repeating path", allocation), ("lock_contention", "lock shared with another context", lock_contention),
    ("lock_held_work", "work done while holding a lock", lock_held_work), ("boundary_in_loop", "call across the boundary inside a loop", boundary_in_loop),
    ("redone_work", "work redone with no change of input", redone_work), ("copy", "copy on a repeating path", copies),
    ("linear_search", "linear search on a repeating path", linear_search), ("query_per_frame", "question asked every frame", query_per_frame),
    ("gpu_upload", "data sent to the GPU repeatedly", gpu_upload), ("gpu_pass", "full-screen pass or screen read", gpu_pass),
    ("shadow", "shadow work per item", shadow), ("material", "material changed on a repeating path", material),
    ("variants", "shader variants", variants), ("memory_growth", "state that only grows", memory_growth),
    ("streaming", "jobs started with no budget", streaming), ("loading", "heavy work repeated on the opening path", loading),
)
