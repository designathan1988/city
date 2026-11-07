"""What every mutation shares: a site the analysis did not already flag, the text of a line or a span of the analysed commit, one edit applied
to a copy, and where the new finding must appear."""


class NoSite(Exception):
    """The analysed code has no place this mutation applies to: the check fails, it is never skipped."""


def mutation(file, old, new, expect, note):
    if old == new:
        raise NoSite(f"edit changes nothing in {file}")
    return {"file": file, "old": old, "new": new, "expect": list(expect), "note": note}


def line_span(text, line):
    """Byte offsets of a 1-based line of a file."""
    start = 0
    for _ in range(line - 1):
        start = text.index(b"\n", start) + 1
    end = text.find(b"\n", start)
    return start, len(text) if end < 0 else end


def replace_in_line(text, line, pattern, repl, count=1):
    """The file with one regular-expression replacement made inside one line only."""
    start, end = line_span(text, line)
    piece = text[start:end]
    changed, n = pattern.subn(repl, piece, count=count)
    if n == 0:
        raise NoSite(f"line {line} has no {pattern.pattern!r}")
    return text[:start] + changed + text[end:]


def flagged(findings, cls):
    """Places the baseline already reports for a class: a mutation must make a new one, never land on these."""
    return {(f["file"], f["line"]) for f in findings if f["class"] == cls}


def inside(findings, cls, file, lo, hi):
    return any(f["class"] == cls and f["file"] == file and lo <= f["line"] <= hi for f in findings)
