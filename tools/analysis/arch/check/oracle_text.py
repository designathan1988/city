"""The text of a repository as the precision judges read it: lines, the blocks around a line, who holds a word. No graph, no adapter, no compiler."""
import fnmatch
import re

LOOP = re.compile(r"\b(for|while|do)\b")
WORD = re.compile(r"[A-Za-z_]\w*")
CODE = (".cpp", ".h", ".hpp", ".inl", ".gd", ".gdshader", ".gdshaderinc", ".py", ".ts")
TYPE_OF = re.compile(r"@S@(\w+)@FI@")


class Code:
    """The text of a repository at a commit, and nothing else."""

    def __init__(self, texts, left_out=(), read=lambda path: None):
        self.texts = {p: t for p, t in texts.items() if p.endswith(CODE)}
        self.raw = texts
        self.lines = {p: t.split("\n") for p, t in texts.items() if t is not None}
        self.left_out = left_out
        self.read = read
        self.holding = {}
        for path, text in self.texts.items():
            for w in set(WORD.findall(text)):
                self.holding.setdefault(w, set()).add(path)

    def line(self, path, n):
        if path not in self.lines:
            self.lines[path] = (self.read(path) or "").split("\n")
        lines = self.lines[path]
        return lines[n - 1] if 0 < n <= len(lines) else ""

    def delivered(self, path):
        return not any(fnmatch.fnmatch(path.split("/", 1)[-1], p) for p in self.left_out)

    def where(self, word):
        return sorted(self.holding.get(word, ()))

    def of_type(self, unit, word):
        """Files that hold a word and, when the unit says what type it belongs to, name that type too: a field is reached through its type."""
        owner = TYPE_OF.search(unit)
        return [p for p in self.where(word) if owner is None or p in self.holding.get(owner[1], ())]

    def _braces(self, path, n):
        """Lines that open the blocks around a line, nearest first, by counting braces upwards."""
        depth = 0
        out = []
        for i in range(n - 1, 0, -1):
            text = self.line(path, i)
            depth += text.count("}") - text.count("{")
            if depth < 0:
                out.append(f"{self.line(path, i - 1)} {text}")
                depth = 0
        return out

    def _indents(self, path, n):
        """Lines that hold a line in an indented language."""
        out = []
        width = len(self.line(path, n)) - len(self.line(path, n).lstrip())
        for i in range(n - 1, 0, -1):
            text = self.line(path, i)
            if text.strip() and len(text) - len(text.lstrip()) < width:
                out.append(text)
                width = len(text) - len(text.lstrip())
        return out

    def around(self, path, n):
        return self._indents(path, n) if path.endswith((".gd", ".py")) else self._braces(path, n)

    def loops_around(self, path, n):
        text = self.line(path, n)
        head = self.line(path, n - 1)
        return sum(1 for x in self.around(path, n) if LOOP.search(x)) + bool(LOOP.search(text)) + bool(LOOP.search(head) and "{" not in head)

    def body(self, path, n):
        """The text of the function that starts near a line."""
        out = []
        depth = 0
        began = False
        width = None
        for i in range(n, len(self.lines.get(path, ())) + 1):
            text = self.line(path, i)
            if path.endswith((".gd", ".py")):
                here = len(text) - len(text.lstrip())
                if width is None:
                    width = here
                elif text.strip() and here <= width:
                    break
            else:
                depth += text.count("{") - text.count("}")
                began = began or "{" in text
                if began and depth <= 0:
                    out.append(text)
                    break
            out.append(text)
        return "\n".join(out)
