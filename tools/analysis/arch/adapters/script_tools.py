"""Shell and CMake by their grammars: the commands they run, the files they name, the variables they set: what a build and a tool do."""
import re

import tree_sitter
import tree_sitter_bash
import tree_sitter_cmake

from ..model import GRAMMAR, Facts

BASH = tree_sitter.Parser(tree_sitter.Language(tree_sitter_bash.language()))
CMAKE = tree_sitter.Parser(tree_sitter.Language(tree_sitter_cmake.language()))
PATHLIKE = re.compile(r"^[\w.$/{}-]*[\w}]/[\w./${}*-]+$|^[\w-]+\.\w+$")


def _walk(n):
    stack = [n]
    while stack:
        x = stack.pop()
        yield x
        stack.extend(reversed(x.children))


def _text(n, src):
    return src[n.start_byte:n.end_byte].decode("utf8", "replace")


def shell(file, src, facts):
    root = "sh-script:" + file
    facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, src.count(b"\n") + 1)
    facts.define("file", file, root)
    for n in _walk(BASH.parse(src).root_node):
        line = n.start_point[0] + 1
        if n.type == "command":
            name = n.child_by_field_name("name")
            if name is not None:
                word = _text(name, src).strip("\"'")
                facts.site(file, line, n.start_point[1] + 1, "call")
                facts.edge(root, "process", "ext:process." + word.rsplit("/", 1)[-1], file, line, GRAMMAR)
                if PATHLIKE.match(word):
                    facts.ref("file", word, root, file, line, "run")
        elif n.type in ("word", "string_content", "raw_string"):
            word = _text(n, src).strip("'\"")
            if PATHLIKE.match(word):
                facts.ref("file", word, root, file, line, "shell")
                facts.site(file, line, n.start_point[1] + 1, "string")
        elif n.type == "variable_assignment":
            name = n.child_by_field_name("name")
            if name is not None:
                facts.edge(root, "environment", "ext:env." + _text(name, src), file, line, GRAMMAR)


def cmake(file, src, facts):
    root = "cmake:" + file
    facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, src.count(b"\n") + 1)
    facts.define("file", file, root)
    for n in _walk(CMAKE.parse(src).root_node):
        if n.type == "normal_command":
            head = n.named_children[0] if n.named_children else None
            if head is None:
                continue
            word = _text(head, src).lower()
            line = n.start_point[0] + 1
            facts.edge(root, "build", "ext:build." + word, file, line, GRAMMAR)
            facts.site(file, line, 1, "call")
            for a in n.named_children[1:]:
                text = _text(a, src)
                for w in re.findall(r"[\w./-]+", text):
                    if PATHLIKE.match(w):
                        facts.ref("file", w, root, file, line, "build")


def scan(repo, tracked, scope):
    facts = Facts()
    for file in sorted(scope):
        name = file.rsplit("/", 1)[-1].lower()
        if name.endswith(".sh"):
            text = repo.text(tracked[file])
            if text is not None:
                shell(file, text.encode("utf8"), facts)
        elif name == "cmakelists.txt" or name.endswith(".cmake"):
            text = repo.text(tracked[file])
            if text is not None:
                cmake(file, text.encode("utf8"), facts)
    return facts, {}
