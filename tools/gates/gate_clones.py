"""Copy/paste detector (PMD CPD style): token windows hashed with Karp-Rabin; any window of at least
`min_tokens` identical tokens in two places is a re-implemented rule."""
import os
import re

TOKEN = re.compile(r"[A-Za-z_]\w*|\d+\.?\d*f?|==|!=|<=|>=|&&|\|\||->|::|[^\s\w]")
BASE, MOD = 1000003, (1 << 61) - 1


def _tokens(path):
    with open(path, encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    toks = []
    for n, line in enumerate(text.splitlines(), 1):
        line = re.sub(r"//.*", "", line)
        if path.endswith(".gd"):
            line = re.sub(r"#.*", "", line)
        if line.strip().startswith(("#include", "import ", "from ", "extends ", "class_name ")):
            continue
        for t in TOKEN.findall(line):
            toks.append((t, n))
    return toks


def check_clones(root, rules, files, err):
    cfg = rules["clones"]
    w = cfg["min_tokens"]
    seen = {}
    reported = set()
    for f in files:
        if not any(f.startswith(p) for p in cfg["paths"]) or not f.endswith(tuple(cfg["extensions"])):
            continue
        toks = _tokens(os.path.join(root, f))
        if len(toks) < w:
            continue
        ids = [hash(t) & 0xFFFFFFFF for t, _ in toks]
        h, power = 0, pow(BASE, w - 1, MOD)
        for i in range(len(ids)):
            if i >= w:
                h = (h - ids[i - w] * power) % MOD
            h = (h * BASE + ids[i]) % MOD
            if i < w - 1:
                continue
            start = i - w + 1
            loc = (f, toks[start][1])
            prev = seen.get(h)
            if prev is None:
                seen[h] = loc
                continue
            if prev[0] == f and abs(prev[1] - loc[1]) < 3:
                continue
            key = (prev[0], f)
            if key in reported:
                continue
            reported.add(key)
            err("CODIGO_DUPLICADO", f"{f}:{loc[1]}", f"{w}+ tokens idênticos a {prev[0]}:{prev[1]}: extraia para um único dono")
