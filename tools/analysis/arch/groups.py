"""Features found without a resolution: a tree of merges by reciprocal best pairs, then the clusters that persist longest in it. A change far
away cannot move a merge here, which is what a global objective cannot promise."""
import collections
import heapq

from . import atoms
from .core import natural_break, words_of


class Tree:
    def __init__(self, leaves):
        self.members = {i: [n] for i, n in enumerate(leaves)}
        self.height = {i: None for i in range(len(leaves))}
        self.children = {}
        self.parent = {}
        self.next = len(leaves)

    def join(self, parts, level):
        node = self.next
        self.next += 1
        self.children[node] = list(parts)
        self.height[node] = level
        for p in parts:
            self.parent[p] = node
        return node


def agglomerate(nodes, rank, sims):
    """Average linkage over a sparse graph. Everything tied at the top merges together, so order among equals decides nothing."""
    ordered = sorted(nodes, key=rank.get)
    index = {n: i for i, n in enumerate(ordered)}
    tree = Tree(ordered)
    near = collections.defaultdict(dict)
    for (a, b), s in sims.items():
        if a in index and b in index and s > 0:
            i, j = index[a], index[b]
            near[i][j] = s
            near[j][i] = s
    size = {i: 1 for i in range(len(ordered))}
    heap = [(-round(s, atoms.PRECISION), min(i, j), max(i, j)) for i, nb in near.items() for j, s in nb.items() if i < j]
    heapq.heapify(heap)
    while heap:
        top = heap[0][0]
        batch = []
        while heap and heap[0][0] == top:
            neg, i, j = heapq.heappop(heap)
            if i in near and j in near[i] and round(near[i][j], atoms.PRECISION) == -neg:
                batch.append((i, j))
        if not batch:
            continue
        for parts in _components(batch):
            _merge(tree, near, size, heap, parts, -top)
    return tree


def _components(pairs):
    parent = {}

    def find(x):
        while parent.setdefault(x, x) != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    for a, b in pairs:
        parent[find(a)] = find(b)
    out = collections.defaultdict(list)
    for x in sorted(parent):
        out[find(x)].append(x)
    return [sorted(v) for _, v in sorted(out.items())]


def _merge(tree, near, size, heap, parts, level):
    node = tree.join(parts, level)
    total = sum(size[p] for p in parts)
    size[node] = total
    joined = collections.Counter()
    for p in parts:
        for q, s in near.pop(p, {}).items():
            if q not in parts:
                joined[q] += s * size[p]
                near[q].pop(p, None)
    near[node] = {}
    for q, w in joined.items():
        s = w / total
        if s > 0:
            near[node][q] = s
            near[q][node] = s
            heapq.heappush(heap, (-round(s, atoms.PRECISION), min(node, q), max(node, q)))


def select(tree, roots):
    """Excess of mass (Campello et al. 2013): a cluster stands if it lasts longer than its parts together. No size or level is chosen."""
    size = {}
    for i in tree.members:
        size[i] = 1
    order = sorted(tree.children)
    for node in order:
        size[node] = sum(size[c] for c in tree.children[node])
    stability, keep = {}, {}
    for node in order:
        birth = tree.height.get(tree.parent.get(node), 0.0) or 0.0
        own = size[node] * max((tree.height[node] or 0.0) - birth, 0.0)
        below = sum(stability.get(c, 0.0) for c in tree.children[node])
        if node in roots or below > own:
            stability[node] = below
            keep[node] = False
        else:
            stability[node] = own
            keep[node] = True
    chosen = []
    stack = sorted(roots)
    while stack:
        node = stack.pop()
        if node in tree.children:
            if keep[node]:
                chosen.append(node)
            else:
                stack.extend(tree.children[node])
    return sorted(chosen), size


def leaves_of(tree, node):
    stack, out = [node], []
    while stack:
        n = stack.pop()
        if n in tree.children:
            stack.extend(tree.children[n])
        else:
            out.append(tree.members[n][0])
    return out


def label_of(members, graph, frequency):
    """A group is called by the words most its own: those whose share inside it stands out. Ties all stay, so nothing is picked by name."""
    inside = collections.Counter()
    for m in members:
        inside.update(set(words_of(graph.units[m]["name"])))
    score = {w: (c / frequency[w]) * (c / len(members)) for w, c in inside.items() if frequency[w]}
    if not score:
        return []
    cut = natural_break(list(score.values()))
    top = [w for w, s in score.items() if s >= cut] if cut != float("inf") else list(score)
    return sorted(top)


def _sizes(tree):
    size = {i: 1 for i in tree.members}
    for node in sorted(tree.children):
        size[node] = sum(size[c] for c in tree.children[node])
    return size


def _branches(tree, current):
    """Where two or more of the current clusters meet, and each one's next such place above: chains of single units are not levels."""
    order = sorted(tree.children)
    under = {i: 0 for i in tree.members}
    meets = set()
    for node in order:
        parts = [c for c in tree.children[node] if under[c]]
        under[node] = 1 if node in current else sum(under[c] for c in parts)
        if node not in current and len(parts) > 1:
            meets.add(node)
    above = {}
    for node in reversed(order):
        up = node if node in meets else above.get(node)
        for c in tree.children[node]:
            above[c] = up
    return meets, above


def _lasting(tree, size, meets, above):
    """The same persistence rule over the meeting places. What every path ends in, a whole connected part, is never a level by itself."""
    stable, keep = {}, {}
    below = collections.Counter()
    for node in sorted(meets):
        up = above.get(node)
        birth = (tree.height[up] or 0.0) if up is not None else 0.0
        own = size[node] * max((tree.height[node] or 0.0) - birth, 0.0)
        keep[node] = up is not None and own >= below[node]
        stable[node] = own if keep[node] else below[node]
        if up is not None:
            below[up] += stable[node]
    return keep


def levels_above(tree, chosen):
    """The levels above the groups: each level's clusters become the leaves of the next, under the rule that found them. Nothing fixes how many."""
    size = _sizes(tree)
    out = []
    current = set(chosen)
    while True:
        meets, above = _branches(tree, current)
        keep = _lasting(tree, size, meets, above)
        cover = {}
        for node in sorted(meets, reverse=True):
            up = above.get(node)
            cover[node] = up is not None and (keep[up] or cover[up])
        top = {n for n in meets if keep[n] and not cover[n]}
        if not top:
            break
        inside = lambda c: above.get(c) is not None and (keep[above[c]] or cover[above[c]])  # noqa: E731
        lifted = {c for c in current if not inside(c)} | top
        if lifted == current:
            break
        out.append(sorted(lifted))
        current = lifted
    out.append(sorted(n for n in tree.children if n not in tree.parent))
    return out


def describe_levels(tree, upper, atoms):
    """Every level above the groups, each cluster called by its own words: what a person reads as features within features."""
    graph = atoms.graph
    frequency = collections.Counter()
    for n in atoms.nodes:
        frequency.update(set(words_of(graph.units[n]["name"])))
    rows = []
    for depth, level in enumerate(upper, 1):
        for node in level:
            members = leaves_of(tree, node)
            if len(members) > 1:
                files = collections.Counter(graph.units[m]["file"] for m in members)
                rows.append({"level": depth, "units": len(members), "words": label_of(members, graph, frequency)[:12], "files": [f for f, _ in sorted(files.items(), key=lambda kv: (-kv[1], kv[0]))[:12]]})
    return rows


def discover(atoms):
    """The feature tree: first level by persistence, then the same rule over what is left above it."""
    graph = atoms.graph
    tree = agglomerate(atoms.nodes, atoms.rank, atoms.similarity())
    roots = {n for n in tree.children if n not in tree.parent}
    chosen, _ = select(tree, roots)
    frequency = collections.Counter()
    for n in atoms.nodes:
        frequency.update(set(words_of(graph.units[n]["name"])))
    groups = []
    group_of = {}
    for i, node in enumerate(chosen):
        members = sorted(leaves_of(tree, node), key=atoms.rank.get)
        for m in members:
            group_of[m] = i
        groups.append({"id": i, "members": members, "words": label_of(members, graph, frequency), "height": tree.height[node], "tree": node})
    return groups, group_of, tree, levels_above(tree, chosen)
