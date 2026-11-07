"""The compiler's view of one C or C++ translation unit: declarations become units, and the walk of each body its edges and costs."""
import ctypes
import sys

from clang import cindex as ci

from ..core import words_of
from ..model import Facts
from ..shape import shape
from . import cpp_body

K = ci.CursorKind
FUNCTIONS = {K.FUNCTION_DECL, K.CXX_METHOD, K.CONSTRUCTOR, K.DESTRUCTOR, K.CONVERSION_FUNCTION, K.FUNCTION_TEMPLATE}
CLASSES = {K.CLASS_DECL, K.STRUCT_DECL, K.UNION_DECL, K.CLASS_TEMPLATE, K.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION}
SCOPES = {K.NAMESPACE, K.LINKAGE_SPEC, K.UNEXPOSED_DECL}


class Unit:
    """Reads one translation unit into Facts; `rel` says which absolute paths are the project's and how they are called."""

    def __init__(self, tu, rel, config, claim, nested):
        self.tu, self.rel, self.config, self.claim, self.nested = tu, rel, config, claim, nested
        self.handles = {}
        self.spot = (ci.c_object_p(), ctypes.c_uint(), ctypes.c_uint(), ctypes.c_uint())
        self.mine = {}
        self.facts = Facts()
        self.texts = {}
        self.seen = set()
        self.ids = {}

    def text(self, rel, path):
        if rel not in self.texts:
            with open(path, "rb") as f:
                self.texts[rel] = f.read()
        return self.texts[rel]

    def where(self, c):
        loc = c.location
        return self.rel(loc.file.name if loc.file else None), loc.line

    def foreign(self, c):
        """Is this cursor in a file outside the project? Asked of the library by file handle: a translation unit has hundreds of thousands of such
        cursors, its macros and those of the headers it includes, and a name read for each is what made a walk slow."""
        lib = ci.conf.lib
        f, line, column, offset = self.spot
        lib.clang_getInstantiationLocation(lib.clang_getCursorLocation(c), ctypes.byref(f), ctypes.byref(line), ctypes.byref(column), ctypes.byref(offset))
        handle = ctypes.cast(f, ctypes.c_void_p).value
        if handle not in self.handles:
            self.handles[handle] = self.rel(c.location.file.name if c.location.file else None) is None
        return self.handles[handle]

    def walks(self, file):
        """A file is walked once per configuration, by the first translation unit that reaches it."""
        if file not in self.mine:
            self.mine[file] = self.claim(file)
        return self.mine[file]

    def target(self, ref):
        """A declaration as an edge end: a unit of the project, or the outside by library class and name."""
        usr = ref.get_usr() or f"{ref.spelling}@{ref.location.line}"
        known = self.ids.get(usr)
        if known is None:
            file = ref.location.file
            mine = self.rel(file.name if file else None)
            if mine is not None:
                known = "cpp:" + usr
            else:
                parent = ref.semantic_parent
                owner = parent.spelling if parent is not None and parent.kind in CLASSES else ""
                known = f"ext:{owner}.{ref.spelling}"
            self.ids[usr] = known
        return known

    def run(self):
        for top in self.tu.cursor.get_children():
            if self.foreign(top) and not (self.nested and top.kind in SCOPES):
                continue
            kind = top.kind
            if kind == K.MACRO_INSTANTIATION or kind == K.MACRO_DEFINITION or kind == K.INCLUSION_DIRECTIVE:
                self.preprocessor(top)
            else:
                self.decl(top, None)
        return self.facts

    def preprocessor(self, c):
        file, line = self.where(c)
        if file is None or not self.walks(file):
            return
        if c.kind == K.INCLUSION_DIRECTIVE:
            try:
                included = c.get_included_file()
            except AssertionError:  # a header the compiler could not find
                included = None
            mine = self.rel(included.name) if included else None
            self.facts.edge("file:" + file, "include", "file:" + mine if mine else "ext:" + c.spelling, file, line)
            self.facts.site(file, line, c.location.column, "include")
        elif c.kind == K.MACRO_DEFINITION:
            text = " ".join(t.spelling for t in c.get_tokens())
            uid = "macro:" + c.spelling
            self.facts.unit(uid, "macro", c.spelling, file, line, c.extent.end.line, shape=shape(text))
            cpp_body.macro_names(self.facts, uid, file, line, list(c.get_tokens()))
        else:
            self.facts.edge("file:" + file, "macro", "macro:" + c.spelling, file, line, at=line)
            self.facts.site(file, line, c.location.column, "macro")

    def decl(self, c, parent):
        kind = c.kind
        if kind in SCOPES:
            for k in c.get_children():
                self.decl(k, parent)
            return
        file, line = self.where(c)
        if file is None or not self.walks(file):
            return
        spot = (file, c.extent.start.offset, c.spelling, kind)
        if spot in self.seen:
            return
        self.seen.add(spot)
        if kind in CLASSES:
            self.record(c, file, line, parent)
        elif kind in FUNCTIONS:
            self.function(c, file, line, parent)
        elif kind == K.FIELD_DECL or kind == K.VAR_DECL:
            self.variable(c, file, line, parent)
        elif kind == K.ENUM_DECL:
            self.enum(c, file, line, parent)
        elif kind == K.TYPEDEF_DECL or kind == K.TYPE_ALIAS_DECL:
            self.facts.unit(self.target(c), "alias", c.spelling, file, line, c.extent.end.line, parent)

    def record(self, c, file, line, parent):
        if not c.is_definition():
            return
        uid = self.target(c)
        self.facts.unit(uid, "class", c.spelling or "anonymous", file, line, c.extent.end.line, parent, abstract=c.is_abstract_record())
        for k in c.get_children():
            if k.kind == K.CXX_BASE_SPECIFIER:
                base = k.referenced
                if base is not None:
                    self.facts.edge(uid, "inherit", self.target(base), file, k.location.line)
            else:
                self.decl(k, uid)

    def variable(self, c, file, line, parent):
        uid = self.target(c)
        type_name = c.type.spelling
        self.facts.unit(uid, "field" if c.kind == K.FIELD_DECL or parent else "global", c.spelling, file, line, c.extent.end.line, parent,
                        type=type_name, constant=c.type.is_const_qualified(), words=words_of(type_name.split("<")[0].split("::")[-1]))
        for k in c.get_children():
            if k.kind == K.TYPE_REF or k.kind == K.TEMPLATE_REF:
                ref = k.referenced
                if ref is not None:
                    self.facts.edge(uid, "type", self.target(ref), file, k.location.line)
        body = cpp_body.Body(self, uid, file)
        for k in c.get_children():
            if k.kind not in (K.TYPE_REF, K.TEMPLATE_REF, K.NAMESPACE_REF):
                body.expr(k, cpp_body.State(uid), cpp_body.READ)

    def enum(self, c, file, line, parent):
        if not c.is_definition():
            return
        uid = self.target(c)
        self.facts.unit(uid, "enum", c.spelling or "anonymous", file, line, c.extent.end.line, parent)
        for k in c.get_children():
            if k.kind == K.ENUM_CONSTANT_DECL:
                self.facts.unit(self.target(k), "enumerator", k.spelling, file, k.location.line, k.location.line, uid)

    def function(self, c, file, line, parent):
        owner = c.semantic_parent
        if owner is not None and owner.kind in CLASSES:
            parent = self.target(owner)
        if not c.is_definition():
            if c.is_virtual_method():
                self.facts.unit(self.target(c), "method", c.spelling, file, line, c.extent.end.line, parent, shape="", virtual=True,
                                pure=c.is_pure_virtual_method(), signature=c.type.spelling, declared=True)
            return
        uid = self.target(c)
        start, end = c.extent.start.offset, c.extent.end.offset
        path = c.location.file.name
        text = self.text(file, path)[start:end].decode("utf8", "replace")
        self.facts.unit(uid, "function" if parent is None else "method", c.spelling, file, line, c.extent.end.line, parent, shape=shape(text),
                        virtual=c.is_virtual_method(), const=c.is_const_method(), signature=c.type.spelling, config=self.config,
                        ctor=c.kind == K.CONSTRUCTOR, dtor=c.kind == K.DESTRUCTOR, inline=file.endswith((".h", ".hpp", ".hh", ".inl")),
                        template=c.kind == K.FUNCTION_TEMPLATE, statements=text.count(";"), overrides=" override" in text.split("{")[0], mangled=c.mangled_name)
        body = cpp_body.Body(self, uid, file)
        for k in c.get_children():
            if k.kind == K.COMPOUND_STMT or k.kind == K.MEMBER_REF or k.kind == K.CALL_EXPR:
                body.stmt(k, cpp_body.State(uid))


def walk(tu, rel, config, claim, nested):
    """`nested`: an outside file includes one of ours, so a block of an outside file may hold our code and must be entered."""
    sys.setrecursionlimit(max(sys.getrecursionlimit(), 20000))
    return Unit(tu, rel, config, claim, nested).run()
