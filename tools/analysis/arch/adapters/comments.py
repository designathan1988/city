"""Comments as the weakest of links: a name written between backticks says the text knows it, and nothing more."""
from ..model import Facts
from .data import QUOTED

MARK = {"gd": "#", "py": "#", "sh": "#", "cmake": "#", "c": "//", "cc": "//", "cpp": "//", "cxx": "//", "h": "//", "hh": "//", "hpp": "//", "hxx": "//", "inl": "//",
        "gdshader": "//", "gdshaderinc": "//", "ts": "//", "tsx": "//", "js": "//", "mjs": "//", "cjs": "//"}


def scan(scope, texts):
    facts = Facts()
    for file in sorted(scope):
        mark = MARK.get(file.rsplit(".", 1)[-1].lower())
        text = texts.get(file)
        if mark is None or text is None:
            continue
        for n, line in enumerate(text.split("\n"), 1):
            if mark in line and "`" in line:
                for word in QUOTED.findall(line.split(mark, 1)[1]):
                    facts.ref(None, word, "file:" + file, file, n, "doc")
    return facts, {}
