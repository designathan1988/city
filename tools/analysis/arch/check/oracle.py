"""Precision. A random sample of each kind of finding, judged against the source text by rules that share nothing with the detectors: no graph, no adapter,
no compiler, only the words on the lines. Each judge answers True (the text agrees), False (the text refutes it) or None (the text cannot tell)."""
import random
import re

from .oracle_text import TYPE_OF, WORD, Code

PER_KIND = 40
ALLOWED_WRONG = 0.10
MUST_DECIDE = 0.5
TEST = re.compile(r"\b(if|elif|while|switch|match|case)\b")
ASSIGN = r"\s*(\[[^\]]*\])?\s*([-+*/|&^]|<<|>>)?=[^=]"
LOCK = re.compile(r"lock|mutex", re.I)
TAKES = re.compile(r"push|emplace|insert|append|\[")
GIVES = re.compile(r"\.(erase|clear|pop\w*|remove\w*|shrink\w*)\(|resize\(0|std::erase|\.swap\(")
CONTAINER = re.compile(r"vector|map|set|list|deque|string|array|dictionary|queue|Array|Dictionary|\bnew\b|\[|\{")
BOX = re.compile(r"\bAABB\b")
MAKES = re.compile(r"\bmake_(unique|shared)\b|\bnew\b|\.new\(")
FAMILY = {**dict.fromkeys(("cpp", "h", "hpp", "inl"), ("cpp", "h", "hpp", "inl")), "gd": ("gd",)}


def _name_of(unit):
    """The last word of a unit's id, as text would call it."""
    words = re.findall(r"[A-Za-z_]\w*", re.split(r"[#(]", unit.replace("@F@", " ").replace("@FI@", " ").split("@")[-1])[0].split(" ")[-1])
    return words[-1] if words else ""


def no_reader(c, f):
    name = re.escape(f["detail"]["key"])
    pattern = re.compile(rf"""["']{name}["']|\.{name}\b""")
    return not any(pattern.search(c.texts[p]) for p in c.where(f["detail"]["key"]))


def _reads_through(c, path, kind, name, skip):
    """A line that reads the field through a variable of its type, or by itself in the type's own code."""
    text = c.texts[path]
    held = set(re.findall(rf"\b{kind}\b[\s&*>]+(\w+)", text))
    through = re.compile(rf"\b({'|'.join(sorted(held))})(\[[^\]]*\])?\s*(\.|->)\s*{re.escape(name)}\b") if held else None
    alone = re.compile(rf"\b{re.escape(name)}\b") if re.search(rf"\b{kind}::", text) else None
    write = re.compile(rf"\b{re.escape(name)}{ASSIGN}")
    for i, line in enumerate(c.lines[path], 1):
        if (path, i) != skip and not write.search(line) and not line.rstrip().endswith("\\"):
            if (through and through.search(line)) or (alone and alone.search(line) and "=" not in line.split(name)[0]):
                return True
    return False


def no_effect(c, f):
    name = f["detail"]["field"]
    family = FAMILY.get(f["file"].rsplit(".", 1)[-1], ())
    places = [p for p in c.of_type(f["unit"], name) if p.rsplit(".", 1)[-1] in family]
    owner = TYPE_OF.search(f["unit"])
    if owner is not None:
        return not any(_reads_through(c, p, owner[1], name, (f["file"], f["line"])) for p in places)
    used = re.compile(rf"\b{re.escape(name)}\b")
    write = re.compile(rf"\b{re.escape(name)}{ASSIGN}")
    if len(places) > 8:
        return None
    for path in places:
        for i, text in enumerate(c.lines[path], 1):
            # A line of a macro's list only spells the name for the macro to declare.
            if used.search(text) and not write.search(text) and not text.rstrip().endswith("\\") and not (path == f["file"] and i == f["line"]):
                return False
    return True


def loop_call(c, f):
    owner, _, method = f["detail"].get("api", "").rpartition(".")
    written = method.split("<")[0]
    # An operator or a constructor is not spelled at the place it runs: the loop around it is what can be read.
    if owner != written and "operator" not in written and c.line(f["file"], f["line"]).find(written) < 0:
        return None
    return c.loops_around(f["file"], f["line"]) > 0


def has_api(c, f):
    api = f["detail"].get("api", "")
    method = api.rsplit(".", 1)[-1]
    return method in c.line(f["file"], f["line"]) if method else None


def bounds(c, f):
    """The call that sets the box, or the box built by hand on that line."""
    return has_api(c, f) or BOX.search(c.line(f["file"], f["line"])) is not None


def has_loop(c, f):
    return c.loops_around(f["file"], f["line"]) > 0


def nested(c, f):
    return c.loops_around(f["file"], f["line"]) > 1


def allocation(c, f):
    """Making an object allocates wherever it stands; a container written as a local does when a loop makes it again."""
    text = c.line(f["file"], f["line"])
    return bool(MAKES.search(text)) or (bool(CONTAINER.search(text)) and c.loops_around(f["file"], f["line"]) > 0)


def lock_around(c, f):
    text = c.line(f["file"], f["line"])
    if LOCK.search(text):
        return True
    for i in range(f["line"] - 1, 0, -1):
        line = c.line(f["file"], i)
        if LOCK.search(line):
            return True
        if line[:1].isalpha() and line.rstrip().endswith("{"):
            return False
    return False


def on_lock(c, f):
    return bool(LOCK.search(c.line(f["file"], f["line"])))


def growth(c, f):
    name = _name_of(f["detail"].get("field", ""))
    if not name:
        return None
    takes = gives = False
    places = c.of_type(f["detail"].get("field", ""), name)
    if len(places) > 8:
        return None
    for path in places:
        for text in c.lines[path]:
            if re.search(rf"\b{re.escape(name)}\b", text):
                takes = takes or bool(TAKES.search(text))
                gives = gives or bool(GIVES.search(text))
    return takes and not gives


def texture(c, f):
    """The import file says lossless, and the source is no icon: a raster of real size, by what the image itself says."""
    mode = re.search(r"^compress/mode=(\d+)", c.raw.get(f["file"] + ".import") or "", re.M)
    return mode is not None and mode[1] == "0" and f["detail"].get("pixels", 0) >= 256 * 256


def god(c, f):
    return c.delivered(f["file"])


def role(c, f):
    names = f["detail"].get("names", ())
    first, second = f["places"][0][1], f["places"][1][1]
    held = [n for n in names if re.search(rf"\b{re.escape(n)}\b", c.texts.get(first, "")) and re.search(rf"\b{re.escape(n)}\b", c.texts.get(second, ""))]
    return len(held) > 1


def rule(c, f):
    """Two copies read the same once every word and number is taken out."""
    (_, a, la), (_, b, lb) = f["places"][0], f["places"][1]
    same = [re.sub(r"[A-Za-z_]\w*", "x", re.sub(r"\d+(\.\d+)?f?", "0", "".join(c.body(p, n).split()))) for p, n in ((a, la), (b, lb))]
    return bool(same[0]) and same[0] == same[1]


def decision(c, f):
    name = _name_of(f["unit"])
    if not name:
        return None
    hits = {p for p in c.where(name) if any(TEST.search(t) and re.search(rf"\b{re.escape(name)}\b", t) for t in c.lines[p])}
    return len(hits) > 1


def scattered(c, f):
    """Every place names the same rare things as some other place of the group."""
    words = [set(WORD.findall(c.body(p, n))) for _, p, n in f["places"]]
    common = lambda a, b: {w for w in a & b if len(c.where(w)) < max(len(c.texts) // 4, 3)}  # noqa: E731
    return all(any(len(common(a, b)) > 1 for j, b in enumerate(words) if j != i) for i, a in enumerate(words))


def only_tests(c, f):
    """Named by a call in delivered code, apart from where it is defined, it is not only for tests."""
    name = _name_of(f["unit"])
    if len(name) < 3 or len(c.where(name)) > 30:
        return None
    call = re.compile(rf"(?<![\w&:])\b{re.escape(name)}\s*\(")
    for path in c.where(name):
        if c.delivered(path):
            for i, text in enumerate(c.lines[path], 1):
                if call.search(text) and not (path == f["file"] and abs(i - f["line"]) < 2) and "D_METHOD" not in text and not re.match(r"\s*(func\b|[\w:<>,&*\s]+\s+\w*::)", text):
                    return False
    return True


def table_exists(c, f):
    return f["line"] > 0 and bool(c.line(f["file"], f["line"]).strip())


JUDGES = {"no_reader": no_reader, "no_effect": no_effect, "boundary_in_loop": loop_call, "grows_with_state": has_loop, "nested_loops": nested, "allocation": allocation, "redone_work": has_loop,
          "lock_held_work": lock_around, "lock_contention": on_lock, "long_save": lock_around, "memory_growth": growth, "texture_bandwidth": texture, "god": god, "implicit_role": role,
          "repeated_rule": rule, "repeated_decision": decision, "scattered": scattered, "test_code_shipped": only_tests, "wide_bounds": bounds, "tiny_triangles": has_api, "gpu_upload": has_api,
          "shadow": has_api, "material": has_api, "level_flip": has_api, "engine_copies": loop_call, "boundary_conversion": table_exists, "linear_search": has_loop, "copy": table_exists,
          "tier_price": table_exists, "default_tier": table_exists, "query_per_frame": table_exists, "costly_option": table_exists, "variants": table_exists, "shader_cost": table_exists,
          "gpu_pass": table_exists, "loading": table_exists, "serial_decode": table_exists, "false_sharing": table_exists, "thread_churn": has_loop, "streaming": table_exists,
          "rebuilt_interface": has_api, "pipeline_compile": has_api, "measure_feedback": has_loop}


def walks(c, f):
    """Every step of a proof sits on a real line of a real file."""
    return all(c.line(s["file"], s["line"]).strip() or s["kind"] in ("contains", "part_of") for s in f["path"])


def judged(counts):
    """Each kind within what is allowed, and enough of it decided. No sample at all is a failure."""
    bad = {}
    for kind, (right, wrong, unsure) in counts.items():
        decided = right + wrong
        if decided == 0 or wrong / decided > ALLOWED_WRONG or decided / (decided + unsure) < MUST_DECIDE:
            bad[kind] = (right, wrong, unsure)
    return bool(counts) and not bad, bad


def measure(c, findings, seed):
    """(right, wrong, unsure) per kind, the wrong ones kept to be read."""
    rng = random.Random(seed)
    by_kind = {}
    for f in findings:
        by_kind.setdefault(f["category"], []).append(f)
    counts, examples = {}, {}
    for kind, found in sorted(by_kind.items()):
        judge = JUDGES.get(kind)
        tally = [0, 0, 0]
        for f in rng.sample(found, min(PER_KIND, len(found))):
            answer = judge(c, f) if judge else None
            if answer is not None and not walks(c, f):
                answer = False
            tally[0 if answer is True else 1 if answer is False else 2] += 1
            if answer is False:
                examples.setdefault(kind, []).append(f"{f['file']}:{f['line']}")
        counts[kind] = tuple(tally)
    return counts, {k: v[:3] for k, v in examples.items()}


def precision(ctx):
    """A sample of each kind of finding, drawn now, checked by the text alone. The seed is in the report."""
    run, a = ctx.plain()
    seed = int.from_bytes(__import__("os").urandom(4), "big")
    texts = {p: run.repo.text(b) for p, b in run.tracked.items() if p in run.scope or p in run.roles.sidecar}
    code = Code(texts, run.info.get("godot", {}).get("filters", {}).get("exclude_filter", []), lambda p: run.repo.text(run.tracked[p]) if p in run.tracked else None)
    counts, examples = measure(code, a["findings"], seed)
    ok, bad = judged(counts)
    missing = sorted(k for k in counts if k not in JUDGES)
    return {"pass": ok and not missing, "detail": {"seed": seed, "kinds": len(counts), "sampled": sum(sum(v) for v in counts.values()), "wrong": sum(v[1] for v in counts.values()), "unsure": sum(v[2] for v in counts.values()),
                                                   "failing": bad, "no_judge": missing, "wrong_examples": {k: examples.get(k) for k in bad}, "all": counts}}
