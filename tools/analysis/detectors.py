# Root-cause detectors by code shape, with no names nor versions of any project: each looks for the form of a defect (who
# locks what, who allocates per frame, who never prunes, who wakes for nothing, who calls the engine per element).
# Every finding says the cause, where it is and what proves it; the effect it produces goes in `effect`. They read the
# Sources of code_parse.py: the project's files, each parsed once.
import os
import re

from code_parse import CLASS_HEAD, KEYWORDS

# Maths and format constants: repeating them repeats no rule.
TRIVIAL = re.compile(r"^(HALF\w*|BYTE\w*|PI\w*|TAU\w*|RGBA\w*|CHANNELS|CORNERS|DEG_TO_RAD|RAD_TO_DEG|GOLDEN|NS_PER_\w+|QUARTER|\w*AXES|"
                     r"\w*EPS\w*|DEGENERATE|FULL_TURN|\w*_SHIFT|\w*_BITS|\w*_MASK)$")
ENGINE_TYPE = r"(?:godot::)?(?:Packed\w+Array|TypedArray<[^>]+>|Array|Dictionary|String)"
# What a frame or a simulation step enters by: the engine's frame callbacks and the owners' per-frame entry points, matched
# only as member definitions (Class::sync), never as any function that happens to share a generic word.
FRAME_ROOTS = frozenset({"_process", "_physics_process", "sync", "tick"})
BOUNDED = "bounded:"  # on a member's own line: the code states what ends it (the owner's life, a reset)


def finding(rule, cause, effect, where, proof):
    return {"rule": rule, "cause": cause, "effect": effect, "where": where, "proof": proof}


def shipped(path):
    """A script the running program can reach (tests left out)."""
    return not re.search(r"(^|/)tests?/", path)


def names_in_scripts(scripts):
    """Every name the shipped scripts say: who the running program can reach from script."""
    words = set()
    for p, s in scripts.items():
        if shipped(p):
            words |= set(re.findall(r"\w+", re.sub(r"(?s)const\s+(QUERIES|COMMANDS)\s*:?=\s*\{.*?\n\}", "", s.text)))
    return words


MEMBER_CALL = re.compile(r"\b(\w+)\s*(?:\.|->)\s*(~?\w+)\s*\(")
QUALIFIED_CALL = re.compile(r"\b(\w+)::(~?\w+)\s*\(")
PLAIN_CALL = re.compile(r"(?<![\w.>:])(\w+)\s*\(")


def _declared_type(name, texts):
    """The class a variable or member is declared with (its template argument for a smart pointer), or None."""
    rx = re.compile(r"\b(?:std::(?:unique_ptr|shared_ptr|optional)<\s*)?([A-Z]\w*)\s*>?\s*[&*]?\s*\b" + re.escape(name)
                    + r"\s*[;={(,)\[]")
    for t in texts:
        m = rx.search(t)
        if m:
            return m.group(1)
    return None


def ident(path, f):
    """A definition's identity: a member by its qualified name, a free function by its name and its file (file-local
    helpers of the same name are different functions)."""
    return f.name if f.owner else f"{f.name}@{path}"


def per_frame(sources, parents=None):
    """The idents of the functions a frame or a simulation step reaches: from the member definitions of FRAME_ROOTS, through
    calls resolved to their definitions: a member of the caller's class, of a receiver's declared class, of a qualifier;
    a free function of the caller's file or one a header declares; a member call on an unknown receiver only when one
    function has that name. An unresolved generic name reaches nothing. `parents` (a dict) receives callee -> caller."""
    defs, members, free = {}, {}, {}
    for p, s in sources.items():
        for f in s.funcs:
            key = ident(p, f)
            defs.setdefault(key, (p, f))
            if f.owner:
                members.setdefault(f.short, set()).add(key)
            else:
                free.setdefault(f.short, []).append((p, key))
    headers = [s.text for p, s in sources.items() if p.endswith((".h", ".hpp"))]
    declared = {}  # free name -> whether a header says it is callable

    def in_header(short):
        """A header declares a free function of that name: a declaration at the start of a line (members are indented)."""
        if short not in declared:
            rx = re.compile(r"^(?:inline\s+|static\s+|constexpr\s+)*[A-Za-z_][\w:<>,\*& ]*?\b" + re.escape(short) + r"\s*\(", re.M)
            declared[short] = any(rx.search(t) for t in headers)
        return declared[short]
    class_text = {}
    for s in sources.values():
        if s.path.endswith((".h", ".hpp")):
            for m in CLASS_HEAD.finditer(s.text):
                class_text.setdefault(m.group(1), []).append(s.text)

    def in_class(cls, short):
        return {n for n in members.get(short, ()) if n == f"{cls}::{short}" or n.endswith(f"::{cls}::{short}")}

    def free_fn(path, short):
        local = {k for p, k in free.get(short, ()) if p == path}
        if local:
            return local
        defined_in_header = {k for p, k in free.get(short, ()) if p.endswith((".h", ".hpp"))}
        if defined_in_header or not in_header(short):
            return defined_in_header
        return {k for p, k in free.get(short, ())}

    def callees(path, f):
        out = set()
        cls = f.owner.split("::")[-1] if f.owner else ""
        for recv, short in MEMBER_CALL.findall(f.body):
            if recv == "this" and cls:
                out |= in_class(cls, short)
                continue
            t = _declared_type(recv, [f.body] + class_text.get(cls, []))
            if t:
                out |= in_class(t, short)
            elif len(members.get(short, ())) == 1:
                out |= members[short]
        for q, short in QUALIFIED_CALL.findall(f.body):
            out |= in_class(q, short) or free_fn(path, short)  # Class::member, or namespace::function
        for short in PLAIN_CALL.findall(f.body):
            if short in KEYWORDS or short == f.short:
                continue
            out |= (in_class(cls, short) if cls else set()) or free_fn(path, short)
        return out
    todo = [k for k, (p, f) in defs.items() if f.owner and f.short in FRAME_ROOTS]
    seen = set(todo)
    while todo:
        caller = todo.pop()
        for c in callees(*defs[caller]):
            if c not in seen:
                seen.add(c)
                todo.append(c)
                if parents is not None:
                    parents[c] = caller
    return seen


def chain(parents, key):
    """root -> ... -> key, by the parents per_frame recorded."""
    out = [key]
    while out[-1] in parents:
        out.append(parents[out[-1]])
    return " -> ".join(reversed(out))


def worker_locks_in_readers(root, sources, scripts):
    """A function outside a worker thread's loop, that the running program calls, takes a mutex the loop holds: the caller
    waits for a whole step. What only tests call waits for nobody who plays."""
    out, held, reached = [], {}, names_in_scripts(scripts)
    loops = {p for p, s in sources.items() if re.search(r"std::thread|while\s*\(\s*running", s.text)}
    for p in loops:
        for m in re.finditer(r"lock_guard<std::mutex>\s+\w+\(\s*([\w.>-]+)\s*\)", sources[p].text):
            held.setdefault(m.group(1).split(".")[-1].split(">")[-1], set()).add(p)
    for p, s in sources.items():
        if p in loops:
            continue
        for f in s.funcs:
            if f.short not in reached and "api" in p.split("/"):
                continue
            for m in re.finditer(r"lock_guard<std::mutex>\s+\w+\(\s*([\w.>-]*)", f.body):
                mutex = m.group(1).split(".")[-1].split(">")[-1]
                if mutex in held:
                    out.append(finding("WORKER_LOCK_IN_READER", f"{f.name} takes '{mutex}', which {', '.join(sorted(held[mutex]))} holds in its loop",
                                       "the caller waits for the other thread's whole step (frame spikes, slow clicks)",
                                       f"{p}:{f.line}", "publish what the reader needs (atomics, an immutable snapshot) instead of reading under the lock"))
                    break
    return out


def realloc_per_frame(root, sources, scripts):
    """Whoever allocates its instance buffer every time the count changes, with no room."""
    out = []
    for p, s in sources.items():
        for f in s.funcs:
            if "allocate_data" not in f.body:
                continue
            exact = re.search(r"\b(\w*count\w*)\s*!=\s*(\w*capacity\w*)|\b(\w*capacity\w*)\s*!=\s*(\w*count\w*)", f.body)
            room = re.search(r">\s*\w*capacity|capacity\w*\s*<|ROOM|room", f.body)
            if exact and not room:
                out.append(finding("REALLOC_PER_FRAME", f"{f.name} reallocates the buffer whenever the count differs from the capacity",
                                   "a GPU buffer destroyed and made again on every frame the count moves (GPU spikes)",
                                   f"{p}:{f.line}", f"'{exact.group(0)}' with no room rule in the buffer's owner"))
    return out


def unpruned_maps(root, sources, scripts):
    """A map or set member that takes entries and never lets any go: it grows with the session."""
    out = []
    for p, s in sources.items():
        if not p.endswith((".h", ".hpp")):
            continue
        pair = sources.get(os.path.splitext(p)[0] + ".cpp")
        body = s.text + (pair.text if pair else "")
        for m in re.finditer(r"std::(?:unordered_map|map|unordered_set|set)<[^;]*>\s+(\w+_)\s*;([^\n]*)", s.text):
            n = m.group(1)
            if BOUNDED in m.group(2):
                continue
            grows = re.search(rf"\b{n}\s*(\[|\.emplace|\.insert|\.try_emplace)", body)
            pruned = re.search(rf"\b{n}\s*\.(erase|clear)\s*\(|\b{n}\s*=\s*", body)
            if grows and not pruned:
                out.append(finding("UNPRUNED_MAP", f"'{n}' takes entries and is never pruned", "memory that grows without bound over a session",
                                   f"{p}:{s.text[:m.start()].count(chr(10)) + 1}", "inserted into, never erased, cleared nor reassigned"))
    return out


def idle_wakeups(root, sources, scripts):
    """A thread loop that sleeps by time even with nothing to do (wakes for nothing while paused)."""
    out = []
    for p, s in sources.items():
        for f in s.funcs:
            if re.search(r"while\s*\(", f.body) and re.search(r"sleep_for|sleep_until", f.body) and not re.search(r"\.wait\s*\(|wait_for|wait_until", f.body):
                out.append(finding("IDLE_WAKEUPS", f"{f.name} sleeps by time in a loop instead of waiting for an event",
                                   "periodic wake-ups with nothing to do (CPU and power while idle)", f"{p}:{f.line}",
                                   "sleep_for/sleep_until in a while, no condition variable in the loop"))
    return out


def duplicated_constants(root, sources, scripts):
    """The same named constant in several files: the rule has more than one owner."""
    owners = {}
    for p, s in sources.items():
        for m in re.finditer(r"(?<!static )constexpr\s+[\w:<>]+\s+([A-Z][A-Z0-9_]{3,})\s*=\s*([^,;]+)", s.text):
            owners.setdefault(m.group(1), set()).add((p, m.group(2).strip()))
    out = []
    for name, places in sorted(owners.items()):
        where = sorted({a for a, _ in places})
        if len(where) < 2 or TRIVIAL.match(name):
            continue
        values = sorted({v for _, v in places})
        if len(values) > 1 and not any(a.endswith((".h", ".hpp")) for a in where):
            continue  # each private to its file, with a value of its own: two rules that happen to share a name
        rule = "DIVERGENT_RULE" if len(values) > 1 else "RULE_WITHOUT_SINGLE_OWNER"
        effect = ("the same name means different things in each file" if len(values) > 1
                  else "a copied rule: changing one copy leaves the others behind")
        out.append(finding(rule, f"'{name}' defined in {len(where)} files ({', '.join(values)})", effect, ", ".join(where), "repeated constexpr"))
    return out


def engine_call_in_loop(root, sources, scripts):
    """A loop a frame reaches whose condition asks the engine (size() of an engine type, a call across the DLL boundary) on
    every turn; at a load it costs nothing that shows."""
    out, parents = [], {}
    hot = per_frame(sources, parents)
    for p, s in sources.items():
        for f in s.funcs:
            if ident(p, f) not in hot:
                continue
            typed = set(re.findall(ENGINE_TYPE + r"\s*&?\s*(\w+)\s*[;=,)(:]", f.body))
            for m in re.finditer(r"for\s*\([^;]*;\s*[^;]*<\s*(\w+)\.size\(\)\s*;", f.body):
                if m.group(1) in typed:
                    out.append(finding("ENGINE_CALL_IN_LOOP", f"{f.name}: the loop calls {m.group(1)}.size() on every turn",
                                       "one call across the DLL boundary per element: long loops turn into hitches",
                                       f"{p}:{f.line + f.body[:m.start()].count(chr(10))}",
                                       f"{m.group(0)}; reached by {chain(parents, ident(p, f))}"))
    return out
