"""TYPE (log/trace-rules.md): the class a type text names, seen from where it is read, and a value's one canonical
form (a type naming a class of the program is that class). A class template's member has its parameters replaced by
the arguments written. A JavaScript text is resolved when the value is made (T1-T3, T1'): a class, a type alias's
marker (the values it admits, `Typer.expand`), a container of the language with its arguments resolved
(`js_library`), or nothing. The typer (`typer.Typer`) is built on this.
"""
from __future__ import annotations

from . import js_library
from .cpp_names import strip_templates, template_args
from .library import clean, substitute
from .solver import ALIAS, CLASS, ELEM, ENGINE, FN, LANG, TEXT, is_language


class TypeNames:
    def canon(self, text: str, ctx) -> tuple:
        """A value in its one form: a type that names a class of the program (and carries no template argument) is
        that class, whatever unit it was read in; a JavaScript type with no runtime value is no value ("")."""
        if text.startswith(ELEM):
            inner, where = self.canon(text[len(ELEM):], ctx)
            return (ELEM + inner if inner else ""), where
        if text == ENGINE or text.startswith((CLASS, FN, TEXT, LANG)):
            return text, ctx
        if ctx.lang == "js":
            return js_library.canonical(text, lambda name: self.type_class(name, ctx),
                                        lambda name: self.ix.js.declares_type(name, ctx.file, ctx.owner),
                                        lambda name: self.alias_marker(name, ctx)), ctx
        if "<" in text or "[" in text:
            return text, ctx
        key = self.type_class(text, ctx)
        return (CLASS + key, ctx) if key else (text, ctx)

    def alias_marker(self, name: str, ctx) -> str | None:
        """The marker of the type alias a JS name names where it is read, or of a reader's marker of its own file (a
        type written inside a container's arguments, `js_types.written`): the values it admits (T1')."""
        if name.startswith(js_library.ALIAS_LOCAL):
            return ALIAS + str(self.ix.offset[ctx.file] + int(name[len(js_library.ALIAS_LOCAL):]))
        gid = self.ix.js.alias_of(name, ctx.file, ctx.owner)
        return ALIAS + str(gid) if gid is not None else None

    def type_class(self, text: str, ctx) -> str | None:
        if not text or is_language(text) or text.startswith((FN, ELEM, TEXT)):
            return None
        if text.startswith(CLASS):
            return text[1:]
        if ctx.lang == "js" and js_library.is_container(text):
            return None                                     # a container of the language, never a class of the program
        if ctx.lang == "cpp" and ctx.local_types:          # a class of the function it is read in ([class.local])
            local = ctx.local_types.get(strip_templates(clean(text)))
            if local:
                return local
        key = (text, ctx.lang, ctx.owner, ctx.file)       # a type names the same class from the same scope
        if key not in self._classes:
            self._classes[key] = self.ix.gd_class(text, ctx.owner) if ctx.lang == "gd" else \
                self.ix.js.class_of(text, ctx.file, ctx.owner) if ctx.lang == "js" else \
                self.ix.cpp_class(clean(text), ctx.owner, ctx.file)
        return self._classes[key]

    def receiver(self, tree, unit) -> tuple[list[str], bool, list[int]]:
        """(classes of the program the receiver can be, whether it can be an engine value, callables it can be)."""
        keys, engine, functions = [], False, []
        for text, ctx in self.evaluate(tree, unit):
            if text.startswith(ELEM):
                engine = True              # a container's elements: the container is the language's or the library's
                continue
            if text.startswith(FN):
                functions.append(int(text[len(FN):]))
                continue
            key = self.type_class(text, ctx)
            if key:
                keys.append(key)
            else:
                engine = True
        return list(dict.fromkeys(keys)), engine, list(dict.fromkeys(functions))

    def enclosing(self, unit, name: str):
        """JS (J7): the function around `unit` that declares `name`, whatever the order it is written in (a `var` is
        hoisted; a closure runs after its enclosing function's later declarations). None elsewhere: C++ and GDScript
        lambdas see only what is declared before them."""
        while unit.lang == "js" and unit.outer >= 0:
            unit = self.ix.units[self.ix.offset[unit.file] + unit.outer]
            if name in unit.types:
                return unit
        return None

    def replaces(self, key: str, outer: str) -> bool:
        """Whether a member read on `outer`, a value of class `key`, can have the class template's parameters
        replaced: a class template written with its arguments. Else a member's values are its own, as they are."""
        return not outer.startswith(CLASS) and bool(self.ix.classes[key].params) and bool(template_args(clean(outer)))

    def substitute(self, key: str, outer: str, member: str) -> tuple[str, bool]:
        if not self.replaces(key, outer) or member.startswith((CLASS, FN, ELEM, ENGINE, LANG)):
            return member, False
        return substitute(self.ix.classes[key].params, outer, member)
