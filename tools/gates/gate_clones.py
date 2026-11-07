"""Copy/paste detector (PMD CPD style): every run of `min_tokens` code tokens is hashed (Karp-Rabin over stable token
ids, per file and cached by content); a run found in two places is a re-implemented rule."""


def applies(ctx, path):
    cfg = ctx.rules["clones"]
    return any(path.startswith(p) for p in cfg["paths"]) and path.endswith(tuple(cfg["extensions"]))


def check_clones(ctx, err):
    width = ctx.rules["clones"]["min_tokens"]
    seen, reported = {}, set()
    for f in ctx.files:
        if not applies(ctx, f):
            continue
        facts = ctx.facts(f)
        if facts.get("lex_error"):
            err("CODIGO_DUPLICADO", f, f"cannot be read as code: {facts['lex_error']}")
            continue
        for h, line in facts.get("windows", []):
            prev = seen.get(h)
            if prev is None:
                seen[h] = (f, line)
                continue
            if prev[0] == f and abs(prev[1] - line) < 3:
                continue
            if (prev[0], f) in reported:
                continue
            reported.add((prev[0], f))
            err("CODIGO_DUPLICADO", f"{f}:{line}", f"{width}+ tokens identical to {prev[0]}:{prev[1]}: extract a single owner")
