# Root-cause detectors by code shape, with no names nor versions of any project: each looks for the form of a defect (who
# locks what, who allocates per frame, who never prunes, who wakes for nothing, who calls the engine per element) in any copy.
# Every finding says the cause, where it is and what proves it; the effect it produces goes in `effect`.
import os
import re

CPP_EXT = (".cpp", ".cc", ".cxx", ".h", ".hpp")
GD_EXT = (".gd",)
# Folders that hold no code of the project's own: hidden ones, builds, dependencies brought in whole.
SKIPPED = re.compile(r"^(\..*|build.*|dist|bin|out|obj|__pycache__|node_modules|third_party|thirdparty|vendor|addons|godot-cpp)$", re.I)
FUNC_HEAD = re.compile(r"^\s*[A-Za-z_][\w:<>,\s\*&~]*?\b([A-Za-z_]\w*(?:::~?\w+)*)\s*\(([^;{}]*)\)\s*(const)?[^;{}=]*\{\s*$")
SIGNATURE_LINES = 4  # the longest signature that wraps over lines
# Maths and format constants: repeating them repeats no rule.
TRIVIAL = re.compile(r"^(HALF\w*|BYTE\w*|PI\w*|TAU\w*|RGBA\w*|CHANNELS|CORNERS|DEG_TO_RAD|RAD_TO_DEG|GOLDEN|NS_PER_\w+|QUARTER|\w*AXES|"
                     r"\w*EPS\w*|DEGENERATE|FULL_TURN|\w*_SHIFT|\w*_BITS|\w*_MASK)$")
ENGINE_TYPE = r"(?:godot::)?(?:Packed\w+Array|TypedArray<[^>]+>|Array|Dictionary|String)"
FRAME_ROOTS = re.compile(r"^(_process|_physics_process|update|sync|tick|step)$")  # what a frame or a step enters by
BOUNDED = "bounded:"  # on a member's own line: the code states what ends it (the owner's life, a reset)


def files(root, ext):
    for base, dirs, names in os.walk(root):
        dirs[:] = [d for d in dirs if not SKIPPED.match(d)]
        for n in names:
            if n.endswith(ext):
                yield os.path.join(base, n)


def read(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        return f.read()


def functions(text):
    """(name, first line, body, const) of each function defined with its brace on the signature's last line."""
    lines = text.splitlines()
    i = 0
    while i < len(lines):
        head, k = lines[i], i  # a signature may run over several lines: joined until its parentheses close
        while head.count("(") > head.count(")") and k + 1 < len(lines) and k - i < SIGNATURE_LINES:
            k += 1
            head += " " + lines[k].strip()
        m = FUNC_HEAD.match(head)
        if not m or m.group(1) in ("if", "for", "while", "switch", "catch"):
            i += 1
            continue
        depth, j = 0, i
        while j < len(lines):
            depth += lines[j].count("{") - lines[j].count("}")
            if depth <= 0 and j > i or (depth == 0 and j == i and "}" in lines[j]):
                break
            j += 1
        yield m.group(1), i + 1, "\n".join(lines[i:j + 1]), bool(m.group(3))
        i = j + 1


def finding(rule, cause, effect, where, proof):
    return {"rule": rule, "cause": cause, "effect": effect, "where": where, "proof": proof}


def rel(root, p):
    return os.path.relpath(p, root).replace("\\", "/")


def names_in_scripts(root):
    """Every name the shipped scripts say (tests left out): who the running program can reach from script."""
    words = set()
    for p in files(root, GD_EXT):
        if not re.search(r"[\\/]tests?[\\/]", p):
            words |= set(re.findall(r"\w+", re.sub(r"(?s)const\s+(QUERIES|COMMANDS)\s*:?=\s*\{.*?\n\}", "", read(p))))
    return words


def per_frame(sources):
    """The functions a frame or a simulation step reaches, by the short names their bodies call."""
    calls = {}
    for t in sources.values():
        for name, _, body, _ in functions(t):
            short = name.split("::")[-1]
            calls.setdefault(short, set()).update(re.findall(r"\b([A-Za-z_]\w*)\s*\(", body))
    todo = [n for n in calls if FRAME_ROOTS.match(n)]
    seen = set(todo)
    while todo:
        for c in calls.get(todo.pop(), ()):
            if c in calls and c not in seen:
                seen.add(c)
                todo.append(c)
    return seen


def worker_locks_in_readers(root, sources):
    """A function outside a worker thread's loop, that the running program calls, takes a mutex the loop holds: the caller
    waits for a whole step. What only tests call waits for nobody who plays."""
    out, held, reached = [], {}, names_in_scripts(root)
    loops = {p for p, t in sources.items() if re.search(r"std::thread|while\s*\(\s*running", t)}
    for p in loops:
        for m in re.finditer(r"lock_guard<std::mutex>\s+\w+\(\s*([\w.>-]+)\s*\)", sources[p]):
            held.setdefault(m.group(1).split(".")[-1].split(">")[-1], set()).add(rel(root, p))
    for p, t in sources.items():
        if p in loops:
            continue
        for name, line, body, _ in functions(t):
            if name.split("::")[-1] not in reached and "api" in rel(root, p).split("/"):
                continue
            for m in re.finditer(r"lock_guard<std::mutex>\s+\w+\(\s*([\w.>-]*)", body):
                mutex = m.group(1).split(".")[-1].split(">")[-1]
                if mutex in held:
                    out.append(finding("WORKER_LOCK_IN_READER", f"{name} takes '{mutex}', which {', '.join(sorted(held[mutex]))} holds in its loop",
                                       "the caller waits for the other thread's whole step (frame spikes, slow clicks)",
                                       f"{rel(root, p)}:{line}", "publish what the reader needs (atomics, an immutable snapshot) instead of reading under the lock"))
                    break
    return out


def realloc_per_frame(root, sources):
    """Whoever allocates its instance buffer every time the count changes, with no room."""
    out = []
    for p, t in sources.items():
        for name, line, body, _ in functions(t):
            if "allocate_data" not in body:
                continue
            exact = re.search(r"\b(\w*count\w*)\s*!=\s*(\w*capacity\w*)|\b(\w*capacity\w*)\s*!=\s*(\w*count\w*)", body)
            room = re.search(r">\s*\w*capacity|capacity\w*\s*<|ROOM|room", body)
            if exact and not room:
                out.append(finding("REALLOC_PER_FRAME", f"{name} reallocates the buffer whenever the count differs from the capacity",
                                   "a GPU buffer destroyed and made again on every frame the count moves (GPU spikes)",
                                   f"{rel(root, p)}:{line}", f"'{exact.group(0)}' with no room rule in the buffer's owner"))
    return out


def unpruned_maps(root, sources):
    """A map or set member that takes entries and never lets any go: it grows with the session."""
    out = []
    for p, t in sources.items():
        if not p.endswith((".h", ".hpp")):
            continue
        body = t + sources.get(os.path.splitext(p)[0] + ".cpp", "")
        for m in re.finditer(r"std::(?:unordered_map|map|unordered_set|set)<[^;]*>\s+(\w+_)\s*;([^\n]*)", t):
            n = m.group(1)
            if BOUNDED in m.group(2):
                continue
            grows = re.search(rf"\b{n}\s*(\[|\.emplace|\.insert|\.try_emplace)", body)
            pruned = re.search(rf"\b{n}\s*\.(erase|clear)\s*\(|\b{n}\s*=\s*", body)
            if grows and not pruned:
                out.append(finding("UNPRUNED_MAP", f"'{n}' takes entries and is never pruned", "memory that grows without bound over a session",
                                   f"{rel(root, p)}:{t[:m.start()].count(chr(10)) + 1}", "inserted into, never erased, cleared nor reassigned"))
    return out


def idle_wakeups(root, sources):
    """A thread loop that sleeps by time even with nothing to do (wakes for nothing while paused)."""
    out = []
    for p, t in sources.items():
        for name, line, body, _ in functions(t):
            if re.search(r"while\s*\(", body) and re.search(r"sleep_for|sleep_until", body) and not re.search(r"\.wait\s*\(|wait_for|wait_until", body):
                out.append(finding("IDLE_WAKEUPS", f"{name} sleeps by time in a loop instead of waiting for an event",
                                   "periodic wake-ups with nothing to do (CPU and power while idle)", f"{rel(root, p)}:{line}",
                                   "sleep_for/sleep_until in a while, no condition variable in the loop"))
    return out


def duplicated_constants(root, sources):
    """The same named constant in several files: the rule has more than one owner."""
    owners = {}
    for p, t in sources.items():
        for m in re.finditer(r"(?<!static )constexpr\s+[\w:<>]+\s+([A-Z][A-Z0-9_]{3,})\s*=\s*([^,;]+)", t):
            owners.setdefault(m.group(1), set()).add((rel(root, p), m.group(2).strip()))
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


def engine_call_in_loop(root, sources):
    """A loop a frame reaches whose condition asks the engine (size() of an engine type, a call across the DLL boundary) on
    every turn; at a load it costs nothing that shows."""
    out, hot = [], per_frame(sources)
    for p, t in sources.items():
        for name, line, body, _ in functions(t):
            if name.split("::")[-1] not in hot:
                continue
            typed = set(re.findall(ENGINE_TYPE + r"\s*&?\s*(\w+)\s*[;=,)(:]", body))
            for m in re.finditer(r"for\s*\([^;]*;\s*[^;]*<\s*(\w+)\.size\(\)\s*;", body):
                if m.group(1) in typed:
                    out.append(finding("ENGINE_CALL_IN_LOOP", f"{name}: the loop calls {m.group(1)}.size() on every turn",
                                       "one call across the DLL boundary per element: long loops turn into hitches",
                                       f"{rel(root, p)}:{line + body[:m.start()].count(chr(10))}", m.group(0)))
    return out
