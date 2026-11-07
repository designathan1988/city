"""The C and C++ adapter: every translation unit of every build configuration parsed by the compiler's own library, each file walked by the one
unit chosen for it before anything starts, and a unit parsed again only when a file it can see changed."""
import collections
import marshal
import os
import re

from .. import workers
from ..core import IDENT, digest, natural_break
from ..model import Facts
from .cpp_build import HEADER, SOURCE, compile_units, compiler_of, macros

LANG = ("c", "cc", "cpp", "cxx", "h", "hh", "hpp", "hxx", "inl")
INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*["<]([^">]+)[">]', re.M)
DEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+(\w+)', re.M)
ANGLED = re.compile(r'^[ \t]*#[ \t]*include[ \t]*<([^>]+)>', re.M)


def _parse(task):
    """One translation unit. Walks only the files given to it: the compiler reads the others, nobody walks them twice."""
    from clang import cindex as ci

    from .cpp_walk import walk
    config, rel, flags, export, scope, owned = task
    path = os.path.join(export, rel)
    index = ci.Index.create()
    options = ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
    base = export.replace("\\", "/").lower() + "/"
    memo = {}
    walked = set()

    def relative(name):
        if name is None:
            return None
        found = memo.get(name, 0)
        if found == 0:
            p = os.path.normpath(name).replace("\\", "/")
            found = p[len(base):] if p.lower().startswith(base) else None
            if found is not None and found not in scope:
                found = None
            memo[name] = found
        return found

    def claim(file):
        if file in owned:
            walked.add(file)
            return True
        return False

    try:
        tu = index.parse(path, args=flags, options=options)
    except ci.TranslationUnitLoadError:
        if "-include-pch" not in flags:
            raise
        tu = index.parse(path, args=flags[:flags.index("-include-pch")], options=options)
    errors = [d for d in tu.diagnostics if d.severity >= 3]
    includes = [(relative(inc.source.name), relative(inc.include.name)) for inc in tu.get_includes()]
    facts = walk(tu, relative, config, claim, any(here is None and there is not None for here, there in includes[1:] if includes))
    deps = sorted({there for _, there in includes if there} | {rel})
    return facts, [(relative(d.location.file.name if d.location.file else None) or rel, d.location.line, d.spelling[:160]) for d in errors], deps, sorted(walked)


def _prefix(flags, export, config, repo, tracked, own):
    """What nearly every unit includes from outside, compiled once for a configuration and read by all of them: the compiler stops parsing the same
    headers hundreds of times. Which headers comes from the sample, and `ARCH_PCH=0` gives the road without it, which the selfcheck compares byte by byte."""
    if os.environ.get("ARCH_PCH") == "0":
        return None
    from clang import cindex as ci
    counts = collections.Counter()
    for path in own:
        counts.update(n for n in set(ANGLED.findall(repo.text(tracked[path]) or "")) if n.rsplit(".", 1)[-1] in ("hpp", "h") or "." not in n)
    cut = natural_break(list(counts.values()))
    names = sorted(n for n, c in counts.items() if c >= cut) if cut != float("inf") else sorted(counts)
    if not names:
        return None
    source = os.path.join(export, f"shared_{digest(config)}_{os.getpid()}.hpp")
    with open(source, "w", encoding="utf8", newline="\n") as f:
        f.write("".join(f"#include <{n}>\n" for n in names))
    tu = ci.Index.create().parse(source, args=["-x", "c++-header", *flags], options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD | ci.TranslationUnit.PARSE_INCOMPLETE)
    if any(d.severity >= 3 for d in tu.diagnostics):
        return None
    tu.save(source + ".pch")
    return source + ".pch", source


def _differ(compiler, first, second, source, repo, tracked, own):
    """Which of our files can read differently in two configurations: those that name a macro with another text there, or one defined in such a file,
    or one that expands to such a macro. The compiler lists the macros; the rest is a fixed point over names. Everything else is the same code."""
    a, b = macros(compiler, first, source), macros(compiler, second, source)
    changed = {m for m in set(a) | set(b) if a.get(m) != b.get(m)}
    users = collections.defaultdict(set)
    for name, (_, body) in {**a, **b}.items():
        for word in IDENT.findall(body):
            users[word].add(name)
    stack = list(changed)
    while stack:
        for name in users.get(stack.pop(), ()):
            if name not in changed:
                changed.add(name)
                stack.append(name)
    words = {p: set(IDENT.findall(repo.text(tracked[p]) or "")) for p in own}
    defined = {p: set(DEFINE.findall(repo.text(tracked[p]) or "")) for p in own}
    apart = set()
    grew = True
    while grew:
        grew = False
        for p in own - apart:
            if words[p] & changed:
                apart.add(p)
                changed |= defined[p]
                grew = True
    return apart


def _files(tracked):
    return {p for p in tracked if p.rsplit(".", 1)[-1].lower() in LANG}


def _included(repo, tracked, own):
    """What each file can include, from its text: more than it may really see, which only makes a change look wider than it is."""
    by_name = collections.defaultdict(list)
    for p in sorted(own):
        by_name[p.rsplit("/", 1)[-1]].append(p)
    out = {}
    for path in own:
        found = set()
        folder = path.rsplit("/", 1)[0] if "/" in path else ""
        for name in INCLUDE.findall(repo.text(tracked[path]) or ""):
            near = os.path.normpath(os.path.join(folder, name)).replace("\\", "/")
            if near in own:
                found.add(near)
            else:
                found.update(p for p in by_name.get(name.rsplit("/", 1)[-1], ()) if p == name or p.endswith("/" + name))
        out[path] = found
    return out


def _seen_from(start, included):
    seen, stack = {start}, [start]
    while stack:
        for nxt in included.get(stack.pop(), ()):
            if nxt not in seen:
                seen.add(nxt)
                stack.append(nxt)
    return seen


def _owners(rels, sight, weight):
    """Each header goes to the unit that sees it and has the least to walk so far: the walks spread, and no process waits for another."""
    load = collections.Counter()
    seers = collections.defaultdict(list)
    for rel in rels:
        for file in sight[rel]:
            if HEADER.search(file):
                seers[file].append(rel)
    owned = {rel: {rel} for rel in rels}
    for file in sorted(seers, key=lambda f: (-weight.get(f, 0), f)):
        best = min(seers[file], key=lambda r: (load[r], r))
        owned[best].add(file)
        load[best] += weight.get(file, 0)
    return owned


def _unique(facts):
    """A fact seen in two configurations or two units counts once. Units are merged in a fixed order, so nothing else is needed for the same bytes."""
    for name in Facts.__slots__:
        seen, kept = set(), []
        for v in getattr(facts, name):
            key = marshal.dumps(v)
            if key not in seen:
                seen.add(key)
                kept.append(v)
        setattr(facts, name, kept)


class _Pass:
    """One round of units: those already known come from the table, the rest are parsed, longest first."""

    def __init__(self, repo, tracked, export, own, table, world):
        self.repo, self.tracked, self.export, self.own, self.table, self.world = repo, tracked, export, own, table, world
        self.results = {}
        self.shared = {}
        self.prepare = None
        self.made = {}

    def flags_of(self, flags):
        """A unit with the usual flags reads the shared headers; one with its own flags parses everything itself."""
        usual = self.shared.get(tuple(flags))
        if usual is None or self.prepare is None:
            return flags
        if usual[0] not in self.made:
            self.made[usual[0]] = self.prepare(*usual)
        made = self.made[usual[0]]
        return [*flags, "-include-pch", made[0]] if made else flags

    def key(self, config, rel, flags, sight, owned):
        stable = [f.replace(self.export, "") for f in flags]
        seen = sorted(f"{p}:{self.repo.content_id(self.tracked[p])}" for p in sight)
        return digest("|".join((config, rel, self.world, *stable, *seen, "#", *sorted(owned))))

    def run(self, tasks, log):
        """tasks: (config, rel, flags, sight, owned)."""
        pending = {}
        for config, rel, flags, sight, owned in tasks:
            key = self.key(config, rel, flags, sight, owned)
            if key in self.table:
                self.results[(config, rel, key)] = self.table[key]
            else:
                pending[key] = (config, rel, flags, sight, owned)
        order = sorted(pending, key=lambda k: (-len(pending[k][3]), pending[k][1], pending[k][0]))
        log(f"c++: {len(order)} of {len(tasks)} units to parse")
        jobs = [(pending[k][0], pending[k][1], self.flags_of(pending[k][2]), self.export, frozenset(self.own), frozenset(pending[k][4]), k) for k in order]
        for key, result in workers.each(_keyed, jobs):
            self.table[key] = result
            self.results[(pending[key][0], pending[key][1], key)] = result


def _keyed(job):
    return job[-1], _parse(job[:-1])


def scan(repo, tracked, scope, export, cache, log):
    """Facts for the project's C and C++ code, or none when no build left compile commands."""
    sources = _files(tracked)
    if not sources:
        return Facts(), {"translation_units": 0, "errors": []}
    tasks = compile_units(repo, sources, export)
    own = {p for p in sources if p in scope}
    missing_flags = [p for p in sorted(own) if SOURCE.search(p)] if not tasks else []
    included = _included(repo, tracked, own)
    weight = {p: len(repo.text(tracked[p]) or "") for p in own}
    world = digest("".join(sorted(repo.content_id(tracked[p]) for p in sources - own)))
    work = collections.defaultdict(list)
    for config, rel, flags, _ in tasks:
        if rel in own:
            work[config].append((rel, flags))
    round_one = _Pass(repo, tracked, export, own, cache.table("cpp"), world)
    # Compiling the shared headers pays when many units read them; a handful parses faster without.
    if sum(len(u) for u in work.values()) >= 2 * workers.cores():
        round_one.prepare = lambda config, flags: _prefix(flags, export, config, repo, tracked, own)
    plan, only, same = [], {}, {}
    units_total = 0
    first = None
    for config, units in sorted(work.items()):
        rels = sorted(r for r, _ in units)
        sight = {r: _seen_from(r, included) for r in rels}
        owned = _owners(rels, sight, weight)
        usual = collections.Counter(tuple(f) for _, f in units).most_common(1)[0][0]
        round_one.shared[usual] = (config, list(usual))
        units_total += len(units)
        if first is None:
            first = (config, list(usual))
        else:
            # Only with the shared headers on: the road without them walks everything, and the selfcheck compares the two byte by byte.
            made = round_one.flags_of(list(usual))
            base = round_one.flags_of(first[1])
            if "-include-pch" in made and "-include-pch" in base and round_one.made.get(first[0]):
                only[config] = _differ(compiler_of(repo), first[1], list(usual), round_one.made[first[0]][1], repo, tracked, own)
                same[config] = own - only[config]
                owned = {r: o & only[config] for r, o in owned.items()}
        plan += [(config, rel, flags, sight[rel], owned[rel]) for rel, flags in sorted(units) if owned[rel]]
    round_one.run(plan, log)
    late = _left_over(round_one.results, own, plan, included, only)
    round_one.run(late, log)
    out, errors = Facts(), []
    for key in sorted(round_one.results):
        facts, errs, _, _ = round_one.results[key]
        out.merge(facts)
        errors.extend(errs)
    for config, files in sorted(same.items()):
        out.units += [(*u[:8], {**u[8], "config": config}) for u in list(out.units) if u[3] in files and u[8].get("config") == first[0]]
    _unique(out)
    seen = {d for r in round_one.results.values() for d in r[2]}
    info = {"translation_units": units_total, "errors": sorted(set(errors)), "no_flags": missing_flags, "headers_alone": sorted(p for p in own if HEADER.search(p) and p not in seen)}
    return out, info


def _left_over(results, own, plan, included, only):
    """What the text promised and the compiler did not see, or the reverse: a guarded include. Such a file goes to a unit that really saw it, and a
    header no unit includes is parsed by itself, so nothing goes unwalked."""
    walked = collections.defaultdict(set)
    seen_by = collections.defaultdict(dict)
    flags_of = {}
    for (config, rel, _), result in sorted(results.items()):
        walked[config].update(result[3])
        for dep in result[2]:
            seen_by[config].setdefault(dep, rel)
    for config, rel, flags, _, _ in plan:
        flags_of[(config, rel)] = flags
    late = []
    for config in sorted(seen_by):
        again = collections.defaultdict(set)
        for dep, rel in seen_by[config].items():
            if dep not in walked[config] and (config not in only or dep in only[config]):
                again[rel].add(dep)
        late += [(config, rel, flags_of[(config, rel)], _seen_from(rel, included) | deps, deps) for rel, deps in sorted(again.items())]
    anyone = {d for per in seen_by.values() for d in per}
    alone = sorted(p for p in own if HEADER.search(p) and p not in anyone)
    if alone and plan:
        config, _, flags, _, _ = plan[0]
        late += [(config, h, ["-x", "c++-header", *flags], _seen_from(h, included), {h}) for h in alone]
    return late
