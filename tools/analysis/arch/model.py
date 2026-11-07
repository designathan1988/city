"""What adapters say about a file: units, typed edges with their evidence, names, and the summaries of functions and classes."""

# Sources of evidence; how much each is worth is measured later, one against the others.
AST, TYPES, GRAMMAR, NAME, LEXICAL, HISTORY, RECORDED = "ast", "types", "grammar", "name", "lexical", "history", "recorded"


class Facts:
    """Everything one adapter found in one file or translation unit. Plain lists, so it pickles and caches as it is."""

    __slots__ = ("units", "edges", "defs", "refs", "sites", "costs", "deps", "summaries")

    def __init__(self):
        self.units = []  # (id, kind, name, file, line, end, parent, shape, attrs)
        self.edges = []  # (src, kind, dst, file, line, source, attrs)
        self.defs = []   # (space, name, unit)
        self.refs = []   # (space or None, text, unit, file, line, role)
        self.sites = []  # (file, line, column, kind)
        self.costs = []  # (unit, kind, file, line, depth, locks, detail)
        self.deps = []   # files a result depends on
        self.summaries = []  # one dict per function, lambda, class or global (C and C++)

    def unit(self, uid, kind, name, file, line, end, parent=None, shape="", **attrs):
        self.units.append((uid, kind, name, file, line, end, parent, shape, attrs))
        return uid

    def edge(self, src, kind, dst, file, line, source=AST, **attrs):
        self.edges.append((src, kind, dst, file, line, source, attrs))

    def define(self, space, name, unit):
        self.defs.append((space, name, unit))

    def ref(self, space, text, unit, file, line, role=""):
        self.refs.append((space, text, unit, file, line, role))

    def site(self, file, line, column, kind):
        self.sites.append((file, line, column, kind))

    def cost(self, unit, kind, file, line, depth=0, locks=(), **detail):
        self.costs.append((unit, kind, file, line, depth, tuple(locks), detail))

    def merge(self, other):
        for name in self.__slots__:
            getattr(self, name).extend(getattr(other, name))

    def __getstate__(self):
        return tuple(getattr(self, name) for name in self.__slots__)

    def __setstate__(self, state):
        for name, value in zip(self.__slots__, state):
            setattr(self, name, value)
