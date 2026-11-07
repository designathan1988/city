"""JSON and CSV as code: every key path and every CSV row is a unit, and a string value that looks like a name is a possible link."""
import csv
import io
import re

from ..model import GRAMMAR, Facts

NAMELIKE = re.compile(r"^[A-Za-z_][\w./:#-]*$")
QUOTED = re.compile(r"`([A-Za-z_][\w./:-]*)`")
STRING = re.compile(r'"(?:\\.|[^"\\])*"')


class _Reader:
    """A JSON reader that remembers lines, which `json` throws away."""

    def __init__(self, text, facts, file):
        self.t, self.i, self.line, self.f, self.file = text, 0, 1, facts, file
        self.seen = set()

    def space(self):
        t = self.t
        while self.i < len(t) and t[self.i] in " \t\r\n":
            self.line += t[self.i] == "\n"
            self.i += 1

    def string(self):
        m = STRING.match(self.t, self.i)
        if m is None:
            raise ValueError("string")
        self.i = m.end()
        self.line += m.group(0).count("\n")
        return m.group(0)[1:-1]

    def value(self, path, parent):
        self.space()
        c = self.t[self.i:self.i + 1]
        if c == "{":
            self.object(path, parent)
        elif c == "[":
            self.array(path, parent)
        elif c == '"':
            line = self.line
            text = self.string()
            if NAMELIKE.match(text):
                self.f.ref(None, text, parent, self.file, line, "data")
                self.f.site(self.file, line, 1, "string")
            for word in QUOTED.findall(text):
                self.f.ref(None, word, parent, self.file, line, "doc")
        else:
            start = self.i
            while self.i < len(self.t) and self.t[self.i] not in ",]}\n":
                self.i += 1
            word = self.t[start:self.i].strip()
            if word == "":
                raise ValueError("value")
            self.f.cost(parent, "data_value", self.file, self.line, value=word)

    def object(self, path, parent):
        self.i += 1
        while True:
            self.space()
            if self.t[self.i:self.i + 1] == "}":
                self.i += 1
                return
            line = self.line
            key = self.string()
            self.space()
            self.i += 1
            child = f"{path}/{key}"
            uid = self.key(child, key, line, parent)
            self.value(child, uid)
            self.space()
            if self.t[self.i:self.i + 1] == ",":
                self.i += 1

    def key(self, child, key, line, parent):
        uid = f"json:{self.file}#{child}"
        if uid not in self.seen:
            self.seen.add(uid)
            self.f.unit(uid, "key", key, self.file, line, line, parent)
            self.f.define("key", key, uid)
            if "/" in child[1:]:
                pieces = child.strip("/").split("/")
                self.f.define("key", "_".join(pieces[-2:]), uid)
                self.f.define("key", ".".join(pieces[-2:]), uid)
        return uid

    def array(self, path, parent):
        self.i += 1
        while True:
            self.space()
            if self.t[self.i:self.i + 1] == "]":
                self.i += 1
                return
            self.value(path + "/*", parent)
            self.space()
            if self.t[self.i:self.i + 1] == ",":
                self.i += 1


def json_file(file, text, facts):
    root = "file:" + file
    facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, text.count("\n") + 1)
    facts.define("file", file, root)
    try:
        _Reader(text, facts, file).value("", root)
    except (ValueError, IndexError):
        return False
    return True


def csv_file(file, text, facts):
    """The first column of a table is its key: a text id, a name; every other cell is data."""
    root = "file:" + file
    facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, text.count("\n") + 1)
    facts.define("file", file, root)
    rows = csv.reader(io.StringIO(text))
    for n, row in enumerate(rows, 1):
        if n > 1 and row and row[0].strip():
            uid = f"csv:{file}#{row[0]}"
            facts.unit(uid, "row", row[0], file, n, n, root)
            facts.define("key", row[0], uid)


def scan(repo, tracked, scope, mentioned):
    """A table no code names is a file and no more: its keys only count when something can read them."""
    facts = Facts()
    broken = []
    for file in sorted(scope):
        if file.endswith(".json"):
            text = repo.text(tracked[file])
            if text is None:
                continue
            if file.rsplit("/", 1)[-1] not in mentioned:
                root = "file:" + file
                facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, text.count("\n") + 1)
                facts.define("file", file, root)
            elif not json_file(file, text, facts):
                broken.append(file)
        elif file.endswith(".csv"):
            text = repo.text(tracked[file])
            if text is not None:
                csv_file(file, text, facts)
    return facts, {"broken": broken}


__all__ = ["scan", "GRAMMAR"]
