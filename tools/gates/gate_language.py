"""New text is English (CLAUDE.md, the language rule), judged by its words, not its letters: the lines a file changed against HEAD (all of a new
file, every line in a whole run) are split into words (identifiers on their case and underscores, strings and comments alike) and
each word is looked up in the Portuguese the project writes: the words of the pt_BR column of the player's translations
(project/data/i18n, the one place Portuguese belongs), less those its en column uses, plus tools/gates/lexicon.json `portuguese`
(the words no translation holds), less lexicon.json `english` (the words both languages write: final, via, interior). A name the
lexicon lists (a source's title, a place, a person) is taken out of the line first; an accented letter left is a finding too. The
translations themselves are exempt."""
import csv
import io
import json
import os
import re
import unicodedata

TRANSLATIONS = "project/data/i18n/"
LEXICON = "tools/gates/lexicon.json"
EXEMPT = (TRANSLATIONS, LEXICON)  # the player's translations, and the lexicon that lists the Portuguese words
SHORTEST = 3  # two-letter words (de, da, no) are as often English abbreviations; the longer function words carry the language
WORD = re.compile(r"[^\W\d_]+")
CASE = re.compile(r"(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])")  # where an identifier's words meet: camelCase, HTTPServer


def _base(ch):
    """A letter without its accent, lower case; anything else as it is."""
    return unicodedata.normalize("NFD", ch)[0].lower() if ch.isalpha() else ch


def _plain(text):
    return "".join(_base(ch) for ch in text)


def _accented(ch):
    if ch.isascii() or not ch.isalpha():
        return False
    base = unicodedata.normalize("NFD", ch)
    return len(base) > 1 and base[0].isascii()


def _words(text):
    return {w for w in (_plain(m) for m in WORD.findall(text)) if len(w) >= SHORTEST}


def inputs(ctx):
    """What the verdicts depend on besides the file judged: the translations and the lexicon."""
    return sorted(f for f in ctx.files if f.startswith(TRANSLATIONS) and f.endswith(".csv")) + [LEXICON]


def lexicon(ctx):
    """(the Portuguese words, the names as plain lower-case phrases longest first, what is wrong with the lexicon or None)."""
    def build():
        pt, en = set(), set()
        for rel in inputs(ctx)[:-1]:
            with open(os.path.join(ctx.root, rel), encoding="utf-8") as fh:
                rows = list(csv.reader(io.StringIO(fh.read())))
            if not rows or "pt_BR" not in rows[0] or "en" not in rows[0]:
                continue
            ip, ie = rows[0].index("pt_BR"), rows[0].index("en")
            for row in rows[1:]:
                if len(row) > max(ip, ie):
                    pt |= _words(row[ip])
                    en |= _words(row[ie])
        try:
            with open(os.path.join(ctx.root, LEXICON), encoding="utf-8") as fh:
                lex = json.load(fh)
        except (OSError, json.JSONDecodeError) as e:
            return set(), [], f"{LEXICON}: {e}"
        portuguese = (pt - en - {_plain(w) for w in lex["english"]}) | {_plain(w) for w in lex["portuguese"]}
        if not portuguese:  # a lexicon with no word cannot find one: the rule could not fail
            return set(), [], "no Portuguese word: the translations have no pt_BR and en columns and the lexicon lists none"
        names = sorted({_plain(n) for n in lex["names"]}, key=len, reverse=True)
        return portuguese, names, None
    return ctx.lazy("lexicon", build)


def applies(ctx, path):
    return path in ctx.changed and not path.startswith(EXEMPT)


def _without_names(line, names):
    """The line with every name the lexicon lists blanked out (matched on whole words, accents and case aside)."""
    plain = _plain(line)  # one character for one: a precomposed letter becomes its base letter
    out = list(line)
    for name in names:
        for m in re.finditer(r"(?<![^\W\d_])" + re.escape(name) + r"(?![^\W\d_])", plain):
            out[m.start():m.end()] = " " * (m.end() - m.start())
    return "".join(out)


def _portuguese(line, words):
    """The Portuguese words of a line: every identifier, string and comment word split on its case and underscores."""
    found = set()
    for token in WORD.findall(line):
        for part in CASE.sub(" ", token).split():
            w = _plain(part)
            if len(w) >= SHORTEST and w in words:
                found.add(w)
    return sorted(found)


def check_english(ctx, src, err):
    if src.binary:
        return
    words, names, wrong = lexicon(ctx)
    if wrong:
        err("NON_ENGLISH_TEXT", LEXICON, wrong)
        return
    changed = ctx.changed.get(src.path)
    for n, line in enumerate(src.lines, 1):
        if changed is not None and n not in changed:
            continue
        text = _without_names(line, names)
        accents = sorted({ch for ch in text if _accented(ch)})
        found = _portuguese(text, words)
        if accents or found:
            what = ", ".join(found + ([f"accented {''.join(accents)}"] if accents else []))
            err("NON_ENGLISH_TEXT", f"{src.path}:{n}", f"new or changed text must be English: {what}")
