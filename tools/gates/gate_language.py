"""New text is English (CLAUDE.md, Idioma): the lines a file changed against HEAD (all of a new file) carry no accented
letters, except the player-facing translations in project/data/i18n. Old Portuguese lines are judged when touched."""
import unicodedata

EXEMPT = ("project/data/i18n/",)


def applies(ctx, path):
    return path in ctx.changed and not path.startswith(EXEMPT)


def _accented(ch):
    if ch.isascii() or not ch.isalpha():
        return False
    base = unicodedata.normalize("NFD", ch)
    return len(base) > 1 and base[0].isascii()


def check_english(ctx, src, err):
    if src.binary:
        return
    changed = ctx.changed.get(src.path)
    for n, line in enumerate(src.lines, 1):
        if changed is not None and n not in changed:
            continue
        found = sorted({ch for ch in line if _accented(ch)})
        if found:
            err("NON_ENGLISH_TEXT", f"{src.path}:{n}", f"new or changed text must be English (found {''.join(found)})")
