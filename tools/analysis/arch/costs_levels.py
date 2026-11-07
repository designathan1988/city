"""Costs of a choice of levels: what each level of a setting turns on, and a default that picks the dearest."""
import collections

from .core import words_of


def _as_number(text):
    if text in ("true", "false"):
        return float(text == "true")
    try:
        return float(text)
    except ValueError:
        return None


def _priced(k):
    """Every family of levels with what each level turns on, over what repeats."""
    graph = k.graph
    value = {c[0]: _as_number(c[6]["value"]) for c in k.by_kind["data_value"]}
    out = []
    for levels in (x for x in _levels(graph) if _in_steps(k, x)):
        options = collections.defaultdict(dict)
        for level in levels:
            for c in graph.children[level]:
                if value.get(c) is not None:
                    options[graph.units[c]["name"]][level] = c
        price = collections.Counter()
        paying = collections.Counter()
        for name, cells in sorted(options.items()):
            read = any(e.src in k.when.repeats for c in cells.values() for e in graph.into.get(c, ()))
            order = sorted({value[c] for c in cells.values()})
            if read and len(order) > 1:
                for level, c in cells.items():
                    price[level] += order.index(value[c]) / (len(order) - 1)
                    paying[level] += value[c] != order[0]
        out.append((levels, price, paying))
    return out


def default_tier(k):
    """A default that names the level that turns on the most: its price is paid by everyone who never chose."""
    dear = {}
    for levels, price, _ in _priced(k):
        if price:
            top = max(levels, key=lambda x: (price[x], x))
            dear[k.graph.units[top]["name"]] = top
    out = []
    for space, text, unit, file, line, role in k.run.facts.refs:
        u = k.graph.units.get(unit)
        if role == "data" and text in dear and u is not None and {"default", "auto", "preset", "tier"} & set(words_of(u["name"])):
            out.append(k.finding("default_tier", unit, file, line, level=text))
    return out


def _in_steps(k, levels):
    """Levels of one choice go up together: read in one order, the settings move one way. Any few columns fit some order by chance, as many as there are
    levels; a choice moves more than that, and an odd setting among them does not undo it. Entries of a catalogue that merely share their keys do not."""
    value = {c[0]: _as_number(c[6]["value"]) for c in k.by_kind["data_value"]}
    rows = [{k.graph.units[c]["name"]: value.get(c) for c in k.graph.children[level]} for level in levels]
    columns = [[r.get(n) for r in rows] for n in rows[0]]
    columns = [c for c in columns if None not in c and len(set(c)) > 1]
    if not columns or len(levels) < 3:
        return False
    order = sorted(range(len(rows)), key=lambda i: sum(sorted(c).index(c[i]) for c in columns))
    steady = lambda c: all(a <= b for a, b in zip(c, c[1:])) or all(a >= b for a, b in zip(c, c[1:]))  # noqa: E731
    together = sum(steady([c[i] for i in order]) for c in columns)
    return together > len(levels) and together * 2 > len(columns)


def _levels(graph):
    """Sibling tables with the same keys: each key's value tells that sibling's setting."""
    tables = collections.defaultdict(list)
    for uid, u in graph.units.items():
        keys = [c for c in graph.children.get(uid, ()) if graph.units[c]["kind"] == "key"]
        if u["kind"] == "key" and keys and u["parent"]:
            tables[(u["parent"], frozenset(graph.units[c]["name"] for c in keys))].append(uid)
    return [sorted(v) for _, v in sorted(tables.items(), key=lambda kv: sorted(kv[0][1])) if len(v) > 1]


def tier_price(k):
    """What each level of a choice turns on, counted over what repeats: an option read on the repeating path adds to the levels that set it higher."""
    out = []
    for levels, price, paying in _priced(k):
        for level in levels:
            if price[level]:
                u = k.graph.units[level]
                out.append(k.finding("tier_price", level, u["file"], u["line"], price=round(price[level], 6), options=paying[level]))
    return out


CATALOGUE = (("default_tier", "default that turns on the most expensive", default_tier), ("tier_price", "what each graphics level turns on", tier_price))
