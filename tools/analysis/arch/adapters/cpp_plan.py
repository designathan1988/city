"""What a scan of C and C++ decides before it parses: what each unit can see, who walks each header, what is compiled once for all, and which of our files
can read differently in another configuration."""
import collections
import concurrent.futures
import os
import re

from ..core import IDENT, digest, natural_break
from .cpp_build import HEADER, macros

INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*["<]([^">]+)[">]', re.M)
DEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+(\w+)', re.M)
ANGLED = re.compile(r'^[ \t]*#[ \t]*include[ \t]*<([^>]+)>', re.M)


def angled(repo, tracked, own):
    """Per file, the outside headers it names."""
    return {p: {n for n in ANGLED.findall(repo.text(tracked[p]) or "") if n.rsplit(".", 1)[-1] in ("hpp", "h") or "." not in n} for p in own}


def levels(seen):
    """Nested sets of outside headers, from what nearly every unit includes down: each level adds what the sample's own break says comes next."""
    rest = collections.Counter(n for names in seen.values() for n in names)
    taken, out = set(), []
    while rest:
        cut = natural_break(list(rest.values()))
        step = {n for n, c in rest.items() if c >= cut} if cut != float("inf") else set(rest)
        taken |= step
        out.append(frozenset(taken))
        rest = collections.Counter({n: c for n, c in rest.items() if n not in step})
    return out


def _compiled(job):
    from clang import cindex as ci
    flags, source, names = job
    with open(source, "w", encoding="utf8", newline="\n") as f:
        f.write("".join(f"#include <{n}>\n" for n in sorted(names)))
    tu = ci.Index.create().parse(source, args=["-x", "c++-header", *flags], options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD | ci.TranslationUnit.PARSE_INCOMPLETE)
    if any(d.severity >= 3 for d in tu.diagnostics):
        return None
    tu.save(source + ".pch")
    return source + ".pch"


def shared(seen, flags, export, config):
    """Outside headers compiled once and read by many units, so the compiler stops parsing them hundreds of times. A unit reads only the largest level made
    of headers it includes itself: it never sees a declaration it would not have seen. Each file ends with this process's number, for tasks side by side."""
    steps = levels(seen)
    pick = {}
    for rel, names in seen.items():
        fits = [s for s in steps if s <= names]
        if fits:
            pick[rel] = fits[-1]
    used = sorted(set(pick.values()), key=len)
    jobs = [(flags, os.path.join(export, f"shared_{digest(config)}_{i}_{os.getpid()}.hpp"), names) for i, names in enumerate(used)]
    with concurrent.futures.ThreadPoolExecutor(max(1, len(jobs))) as pool:
        made = dict(zip(used, pool.map(_compiled, jobs)))
    return {rel: made[names] for rel, names in pick.items() if made[names]}


def everything(names, export, config):
    """A file naming every outside header: what the compiler is asked about macros."""
    source = os.path.join(export, f"every_{digest(config)}_{os.getpid()}.hpp")
    with open(source, "w", encoding="utf8", newline="\n") as f:
        f.write("".join(f"#include <{n}>\n" for n in sorted(names)))
    return source


def differ(compiler, first, second, source, repo, tracked, own):
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


def included(repo, tracked, own):
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


def seen_from(start, included):
    seen, stack = {start}, [start]
    while stack:
        for nxt in included.get(stack.pop(), ()):
            if nxt not in seen:
                seen.add(nxt)
                stack.append(nxt)
    return seen


def owners(rels, sight, weight):
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
