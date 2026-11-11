"""TYPE (log/trace-rules.md): the class a type text names, seen from where it is read, and a value's one canonical
form (a type naming a class of the program is that class). A class template's member has its parameters replaced by
the arguments written. The typer (`typer.Typer`) is built on this.
"""
from __future__ import annotations

from .cpp_names import strip_templates
from .library import clean, substitute
from .solver import CLASS, ELEM, ENGINE, FN, TEXT


class TypeNames:
    def canon(self, text: str, ctx) -> tuple:
        """A value in its one form: a type that names a class of the program (and carries no template argument) is
        that class, whatever unit it was read in."""
        if text.startswith(ELEM):
            inner, where = self.canon(text[len(ELEM):], ctx)
            return ELEM + inner, where
        if text == ENGINE or text.startswith((CLASS, FN, TEXT)) or "<" in text or "[" in text:
            return text, ctx
        key = self.type_class(text, ctx)
        return (CLASS + key, ctx) if key else (text, ctx)

    def type_class(self, text: str, ctx) -> str | None:
        if not text or text == ENGINE or text.startswith((FN, ELEM, TEXT)):
            return None
        if text.startswith(CLASS):
            return text[1:]
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

    def substitute(self, key: str, outer: str, member: str) -> tuple[str, bool]:
        if outer.startswith(CLASS) or member.startswith((CLASS, FN, ELEM, ENGINE)):
            return member, False
        return substitute(self.ix.classes[key].params, outer, member)
