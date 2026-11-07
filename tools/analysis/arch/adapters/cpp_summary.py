"""The compiler's view of one C or C++ translation unit as summaries: classes with their members, every function and lambda with the events of its
body, which declarations of the outside each method overrides, and the names calls bind to functions. Units and names also go to the graph that joins
languages."""
import ctypes
import os
import sys

from clang import cindex as ci

from ..model import Facts
from .cpp_flow import SKIP, Flow, new_summary

K = ci.CursorKind
FUNCTIONS = {K.FUNCTION_DECL, K.CXX_METHOD, K.CONSTRUCTOR, K.DESTRUCTOR, K.CONVERSION_FUNCTION, K.FUNCTION_TEMPLATE}
CLASSES = {K.CLASS_DECL, K.STRUCT_DECL, K.UNION_DECL, K.CLASS_TEMPLATE, K.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION}
SCOPES = {K.NAMESPACE, K.LINKAGE_SPEC, K.UNEXPOSED_DECL}
SCALARS = {"BOOL", "CHAR_U", "UCHAR", "CHAR16", "CHAR32", "USHORT", "UINT", "ULONG", "ULONGLONG", "UINT128", "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT",
           "LONG", "LONGLONG", "INT128", "ENUM"}
_LIB = []


def _clang_calls():
    if not _LIB:
        lib = ci.conf.lib
        lib.clang_getOverriddenCursors.argtypes = [ci.Cursor, ctypes.POINTER(ctypes.POINTER(ci.Cursor)), ctypes.POINTER(ctypes.c_uint)]
        lib.clang_getOverriddenCursors.restype = None
        lib.clang_disposeOverriddenCursors.argtypes = [ctypes.POINTER(ci.Cursor)]
        lib.clang_disposeOverriddenCursors.restype = None
        _LIB.append(lib)
    return _LIB[0]


def overridden(c):
    """The declarations a method overrides, as the compiler resolved them."""
    lib = _clang_calls()
    found, count = ctypes.POINTER(ci.Cursor)(), ctypes.c_uint()
    lib.clang_getOverriddenCursors(c, ctypes.byref(found), ctypes.byref(count))
    out = []
    for i in range(count.value):
        copy = ci.Cursor()
        ctypes.memmove(ctypes.byref(copy), ctypes.byref(found[i]), ctypes.sizeof(ci.Cursor))
        copy._tu = c._tu
        out.append(copy)
    if count.value:
        lib.clang_disposeOverriddenCursors(found)
    return out


def _role(t):
    """What a parameter carries: a number that selects, a handle on state, or a value of data."""
    c = t.get_canonical()
    kind = c.kind.name
    if kind in ("LVALUEREFERENCE", "RVALUEREFERENCE", "POINTER"):
        inner = c.get_pointee()
        if inner.get_canonical().kind.name in ("RECORD", "ELABORATED", "UNEXPOSED") and not inner.is_const_qualified():
            return "handle"
        return "data" if inner.get_canonical().kind.name in ("FLOAT", "DOUBLE", "LONGDOUBLE", "RECORD") else "handle" if kind == "POINTER" else "data"
    if kind in SCALARS and kind != "BOOL":
        return "selector"
    return "data"


class Unit:
    """Reads one translation unit; `rel` says which absolute paths are the project's and how they are called."""

    def __init__(self, tu, rel, config, claim, nested, language):
        self.tu, self.rel, self.config, self.claim, self.nested, self.language = tu, rel, config, claim, nested, language
        self.handles, self.mine, self.texts, self.seen, self.ids = {}, {}, {}, set(), {}
        self.spot = (ci.c_object_p(), ctypes.c_uint(), ctypes.c_uint(), ctypes.c_uint())
        self.facts = Facts()
        self.summaries = self.facts.summaries
        self.lambda_ids = {}
        self.lang_memo = {}

    def text(self, rel, path):
        if rel not in self.texts:
            with open(path, "rb") as f:
                self.texts[rel] = f.read()
        return self.texts[rel]

    def where(self, c):
        loc = c.location
        return self.rel(loc.file.name if loc.file else None), loc.line

    def foreign(self, c):
        """Is this cursor in a file outside the project? Asked by file handle: a unit has hundreds of thousands of cursors."""
        lib = ci.conf.lib
        f, line, column, offset = self.spot
        lib.clang_getInstantiationLocation(lib.clang_getCursorLocation(c), ctypes.byref(f), ctypes.byref(line), ctypes.byref(column), ctypes.byref(offset))
        handle = ctypes.cast(f, ctypes.c_void_p).value
        if handle not in self.handles:
            self.handles[handle] = self.rel(c.location.file.name if c.location.file else None) is None
        return self.handles[handle]

    def of_the_language(self, ref):
        """Declared by the compiler's own library: libc and the standard headers come with the language."""
        if ref is None:
            return False
        file = ref.location.file
        if file is None:
            return True
        name = file.name
        known = self.lang_memo.get(name)
        if known is None:
            known = self.lang_memo[name] = os.path.normpath(name).replace("\\", "/").lower().startswith(self.language)
        return known

    def walks(self, file):
        if file not in self.mine:
            self.mine[file] = self.claim(file)
        return self.mine[file]

    def target(self, ref):
        """A declaration as an id: the project's by its compiler id, the outside by owner and name."""
        usr = ref.get_usr() or f"{ref.spelling}@{ref.location.line}"
        known = self.ids.get(usr)
        if known is None:
            file = ref.location.file
            if self.rel(file.name if file else None) is not None:
                known = "cpp:" + usr
            else:
                parent = ref.semantic_parent
                owner = parent.spelling if parent is not None and parent.kind in CLASSES else ""
                known = f"{'lang' if self.of_the_language(ref) else 'ext'}:{owner}.{ref.spelling}"
            self.ids[usr] = known
        return known

    def run(self):
        for top in self.tu.cursor.get_children():
            if self.foreign(top) and not (self.nested and top.kind in SCOPES):
                continue
            if top.kind not in (K.MACRO_INSTANTIATION, K.MACRO_DEFINITION, K.INCLUSION_DIRECTIVE):
                self.decl(top, None)
        return self.facts

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
        elif kind == K.VAR_DECL and parent is None:
            self.facts.summaries.append({"kind": "global", "id": self.target(c), "name": c.spelling, "file": file, "line": line, "type": c.type.spelling,
                                         "storage": "thread" if c.tls_kind.name != "NONE" else "static"})

    def record(self, c, file, line, parent):
        if not c.is_definition():
            return
        uid = self.target(c)
        name = "anonymous" if c.is_anonymous() else c.spelling
        self.facts.unit(uid, "class", name, file, line, c.extent.end.line, parent)
        out = {"kind": "class", "id": uid, "name": name, "file": file, "line": line, "end": c.extent.end.line, "parent": parent, "bases": [],
               "foreign_bases": [], "fields": []}
        src = self.text(file, c.location.file.name)
        for k in c.get_children():
            if k.kind == K.CXX_BASE_SPECIFIER:
                base = k.referenced
                if base is not None:
                    (out["foreign_bases"] if self.foreign(base) else out["bases"]).append(self.target(base))
            elif k.kind == K.FIELD_DECL or k.kind == K.VAR_DECL:
                decl = k.type.get_declaration()
                has = decl is not None and decl.kind != K.NO_DECL_FOUND
                init = [x for x in k.get_children() if x.kind not in SKIP]
                tree = Flow(self, src, new_summary("init", uid, "", file, line, line, uid, None), {}, {}, False).tree(init[-1]) if init else None
                out["fields"].append([self.target(k), k.spelling, k.type.spelling, self.target(decl) if has else "", decl.spelling if has else "",
                                      bool(has and self.of_the_language(decl)), tree, k.kind == K.VAR_DECL, k.location.line])
            else:
                self.decl(k, uid)
        self.facts.summaries.append(out)

    def function(self, c, file, line, parent):
        owner = c.semantic_parent
        if owner is not None and owner.kind in CLASSES:
            parent = self.target(owner)
        if not c.is_definition():
            return
        uid = self.target(c)
        kind = "ctor" if c.kind == K.CONSTRUCTOR else "method" if parent else "function"
        s = new_summary(kind, uid, c.spelling, file, line, c.extent.end.line, parent, None)
        s["virtual"] = bool(c.kind == K.CXX_METHOD and c.is_virtual_method())
        s["const"] = bool(c.kind == K.CXX_METHOD and c.is_const_method())
        s["mangled"] = c.mangled_name or ""
        result = c.result_type
        s["ret"] = result.spelling
        scalar = result.get_canonical().kind.name in SCALARS
        for o in overridden(c) if c.kind == K.CXX_METHOD else ():
            if self.foreign(o) or self.rel(o.location.file.name if o.location.file else None) is None:
                holder = o.semantic_parent
                s["foreign_over"].append([holder.spelling if holder is not None else "", o.spelling])
            else:
                s["over"].append(self.target(o))
        params = {}
        for i, p in enumerate(k for k in c.get_children() if k.kind == K.PARM_DECL):
            params[p.get_usr()] = (uid, i)
            s["params"].append([p.spelling, p.type.spelling, _role(p.type)])
        self.facts.unit(uid, "method" if parent else "function", c.spelling, file, line, c.extent.end.line, parent, signature=c.type.spelling,
                        virtual=s["virtual"], config=self.config)
        Flow(self, self.text(file, c.location.file.name), s, params, {}, scalar).body(c)
        self.summaries.append(s)


def walk(tu, rel, config, claim, nested, language):
    """`nested`: an outside file includes one of ours, so a block of an outside file may hold our code and must be entered."""
    sys.setrecursionlimit(max(sys.getrecursionlimit(), 20000))
    return Unit(tu, rel, config, claim, nested, language).run()
