"""When code runs: what repeats (frame, tick, thread), what loads, what only tests reach. Everything is carried along the flow from the entries
the program itself shows, with the path that proves it."""
import collections

from .core import words_of
from .graph import FLOW

CALLABLE = {"function", "method", "entry", "lambda", "branch"}
# Making a closure, binding a handler or storing a job runs nothing: what they point at runs later, from an event of its own.
LATER = {"bind", "callable", "stored", "callback", "connection", "signal_connect", "thread"}


def _calling_by_name(graph):
    """Functions that run what they are given by name, there and then. A name handed to any other function is kept for later: a button's command."""
    out = {"call", "callv"}
    for e in graph.edges.values():
        if e.dst.startswith("ext:") and e.dst.rsplit(".", 1)[-1] in ("call", "callv"):
            home = graph.units.get(graph.callable_of(e.src))
            if home is not None:
                out.add(home["name"])
    return out


def flow_edges(graph):
    """Execution paths: certain edges, plus names that reach a table entry or a function without doubt."""
    out = collections.defaultdict(list)
    at_once = _calling_by_name(graph)
    for e in graph.edges.values():
        role = e.attrs.get("role", "")
        if e.kind == "name_ref" and "#" in role and role.split("#")[0] not in at_once:
            e.attrs["handed"] = True
        if e.generic or e.src not in graph.units or e.dst not in graph.units:
            continue
        if e.kind in FLOW or (e.kind == "name_ref" and e.conf == 1.0 and e.attrs.get("named") and graph.units[e.dst]["kind"] in CALLABLE):
            out[e.src].append(e)
    return _in_order(out)


def _in_order(edges):
    for es in edges.values():
        es.sort(key=lambda x: (x.dst, x.kind))
    return edges


Contains = collections.namedtuple("Contains", "src dst kind sites")


def later(e):
    """Taken some other time: what is registered, and a function a script writes as an argument, which runs when something calls it. C++ lambdas are read by the
    compiler and mostly run in the call they are given to."""
    return e.kind in LATER or (e.kind == "lambda" and "ast" not in e.sources) or bool(e.attrs.get("handed"))


def timed_edges(edges):
    """The edges along which a context carries on: certain, and taken at that moment."""
    deferred = {(e.src, e.dst) for es in edges.values() for e in es if later(e)}
    return {src: [e for e in es if e.conf == 1.0 and not later(e) and not (e.kind == "lambda" and (e.src, e.dst) in deferred)] for src, es in edges.items()}


def wide_edges(graph):
    """Everything certain that leads from one unit to another, what contains it included: how a thing nothing runs still hangs from what runs."""
    out = collections.defaultdict(list)
    for e in graph.edges.values():
        if not e.generic and e.src in graph.units and e.dst in graph.units:
            out[e.src].append(e)
    for uid, u in graph.units.items():
        home = u["parent"] if u["parent"] in graph.units else "file:" + u["file"]
        if home in graph.units and home != uid:
            out[home].append(Contains(home, uid, "contains", [(u["file"], u["line"])]))
            out[uid].append(Contains(uid, home, "part_of", [(u["file"], u["line"])]))
    return _in_order(out)


def _carry(starts, edges):
    """Everything reached from the starts, each with the step that reached it first, in a fixed order."""
    came = {s: None for s in starts}
    queue = collections.deque(sorted(starts))
    while queue:
        n = queue.popleft()
        for e in edges.get(n, ()):
            if e.dst not in came:
                came[e.dst] = e
                queue.append(e.dst)
    return came


def _chain(uid, came):
    """Walks back to where the spread began: that start, and the steps from it to the unit."""
    steps = []
    x = uid
    while came.get(x) is not None and len(steps) < len(came):
        e = came[x]
        steps.append({"from": e.src, "to": e.dst, "kind": e.kind, "file": e.sites[0][0], "line": e.sites[0][1]})
        x = e.src
    steps.reverse()
    return x, steps


def _answers_the_engine(graph, api, u):
    """A script's `_run` is a callback only in a class whose engine base declares it: elsewhere it is a function that happens to have that name."""
    owners = {cls for cls, _, _, _ in api.methods.get(u["name"], ())}
    if not u["attrs"].get("callback") or not api.classes or not u["id"].startswith("gd:"):
        return True
    seen, cls = set(), u["parent"]
    while cls is not None and cls not in seen:
        seen.add(cls)
        cls = next((e.dst for e in graph.out.get(cls, ()) if e.kind == "inherit"), None)
    engine = next((c[4:] for c in seen if c and c.startswith("ext:")), None)
    while engine:
        if engine in owners:
            return True
        engine = api.classes.get(engine)
    return False


class When:
    def __init__(self, run):
        graph, api = run.graph, run.api
        self.graph = graph
        self.edges = flow_edges(graph)
        frame, boot, event = set(), set(), set()
        for uid, u in graph.units.items():
            a = u["attrs"]
            if u["kind"] not in ("function", "method") or not _answers_the_engine(graph, api, u):
                continue
            if a.get("repeating") or (a.get("overrides") and u["name"] in api.repeating):
                frame.add(uid)
            elif a.get("input") or (a.get("overrides") and u["name"] in api.input):
                event.add(uid)
            elif a.get("callback") or (a.get("overrides") and u["name"] in api.virtual):
                boot.add(uid)
        forever = {c[0] for c in run.facts.costs if c[1] == "loop" and c[6].get("forever")}
        threads = {e.src for e in graph.edges.values() if e.kind == "thread"}
        tick = set()
        spun = _carry(threads, timed_edges(self.edges))
        for uid in forever:
            if uid in spun:
                tick.add(uid)
        # What is registered for scripts to call is no entry by itself: it runs when something calls it, and that call is an edge of its own.
        event |= {e.dst for es in self.edges.values() for e in es if later(e) and e.dst in graph.units and not (e.kind == "bind" and "ast" in e.sources)}
        self.roots = {"frame": frame, "tick": tick, "boot": boot, "event": event, "thread": threads}
        self.tests = self._tests(run)
        # A context is the delivered program's: a test's own frame says nothing of how often the game runs what it calls.
        self.roots = {name: starts - self.left_out for name, starts in self.roots.items()}
        timed = timed_edges(self.edges)
        # A test of a remembered value (`if rev != seen`) does not cut the path: how often the value moves is not in the text, and a revision that moved
        # every tick is how the sky came to read everything each frame.
        self.reach = {name: _carry(starts, timed) for name, starts in self.roots.items()}
        self.repeats = set(self.reach["frame"]) | set(self.reach["tick"])
        self.jobs = _carry({e.src for e in graph.edges.values() if e.kind in ("stored", "callback")}, self.edges)
        self.near = _carry({u for came in self.reach.values() for u in came}, wide_edges(graph))

    def _tests(self, run):
        """What only code left out of the delivered build reaches: the export preset's own filter says what is left out."""
        filters = run.info.get("godot", {}).get("filters", {})
        patterns = filters.get("exclude_filter", [])
        self.left_out = set()
        if not patterns:
            return set()
        import fnmatch
        left_out = self.left_out
        for uid, u in self.graph.units.items():
            tail = u["file"].split("/", 1)[-1]
            if any(fnmatch.fnmatch(tail, p) for p in patterns):
                left_out.add(uid)
        delivered = set(self.graph.units) - left_out
        entries = {u for starts in self.roots.values() for u in starts if u in delivered}
        entries |= {e.dst for e in self.graph.edges.values() if e.src in delivered and self.graph.units.get(e.src, {}).get("kind") in ("key", "row", "entry")}
        kept = _carry({u for u in delivered if self.graph.units[u]["kind"] == "class" and self.graph.units[u]["attrs"].get("script")} | entries, self.edges)
        tested = _carry(left_out, self.edges)
        return {u for u in tested if u not in kept} | left_out

    def labels(self, uid):
        return {name for name in ("frame", "tick", "thread", "event", "boot") if uid in self.reach[name]} or {"cold"}

    def label(self, uid):
        for name in ("frame", "tick", "thread", "event", "boot"):
            if uid in self.reach[name]:
                return name
        return "cold"

    def path(self, uid):
        """The proof of a finding: the chain of edges from an entry to the unit. What no entry runs shows how it hangs from what does, and what
        hangs from nothing shows the file it lives in: a step, always."""
        for name in ("frame", "tick", "thread", "event", "boot"):
            if uid in self.reach[name]:
                root, steps = _chain(uid, self.reach[name])
                return name, root, steps or self._home(uid)
        if uid in self.near:
            joint, bridge = _chain(uid, self.near)
            for name in ("frame", "tick", "thread", "event", "boot"):
                if joint in self.reach[name]:
                    root, steps = _chain(joint, self.reach[name])
                    return "cold", root, steps + bridge
        return "cold", uid, self._home(uid)

    def _home(self, uid):
        u = self.graph.units[uid]
        return [{"from": "file:" + u["file"], "to": uid, "kind": "contains", "file": u["file"], "line": u["line"]}]


def api_words(key):
    """The words of an outside call: `Class.method` gives both sides."""
    owner, _, name = key.rpartition(".")
    return set(words_of(owner)), set(words_of(name))
