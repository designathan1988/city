"""A JSX element in the new core (log/trace-core-plan.md, Task 6; TypeScript handbook: JSX; tsconfig `jsx`,
`jsxFactory`, `jsxImportSource`): it is a call of the factory the program's TypeScript configuration chooses — the
classic transform's factory name, or the automatic transform's runtime module's function — with the tag and the props
object; where no transform is configured the element is handed to a runtime nothing declares (an open call: the tag
is called back). The props' children are named as the runtime's declarations name them (`JSX.ElementChildrenAttribute`).
"""
from __future__ import annotations

from ..languages import (JSX_AUTOMATIC, JSX_CHILDREN_ATTRIBUTE, JSX_CLASSIC, JSX_FACTORY_DEFAULT, JSX_FACTORY_OPTION,
                         JSX_MODE_OPTION, JSX_NAMESPACE, JSX_SOURCE_DEFAULT, JSX_SOURCE_OPTION)
from ..model import STATE_KINDS
from ..vocab import CALL, JSX, JSX_CHILDREN, LITERAL, MEMBER, NAME
from . import declared, members, names, summaries

JSX_CALL, FACTORY, CHILDREN = 70, 71, 72          # node tags


def value(an, node, tree, unit, ctx) -> None:
    """The value of a JSX element's tree `(JSX, tag, props)`: what its factory call gives."""
    tag, props = an.node(tree[1], unit, ctx), an.node(tree[2], unit, ctx)
    config = options(an, unit.file)
    mode = config.get(JSX_MODE_OPTION)
    call = call_node(an, node, tree, unit, ctx)
    module = runtime(an, unit.file, config, mode)
    if mode in JSX_AUTOMATIC and module is not None:
        factories = (FACTORY, module, JSX_AUTOMATIC[mode][1])
        for gid in an.ix.js.export(module, JSX_AUTOMATIC[mode][1])[0]:
            names.value_of(an, gid, factories)
        an.solver.subscribe(factories, call, (FACTORY,), lambda t, fns: [
            an.call(t, f, None, [tag, props], CALL) for f in fns])
    elif mode not in JSX_CLASSIC:
        summaries.open_call(an, call, [tag, props], declared.call_site(an, call))
    an.solver.follow(call, node)
    name = children_name(an, unit.file, config, mode)
    if name:
        an.solver.subscribe(props, (CHILDREN, props), (CHILDREN,), lambda _, objs: [
            named_children(an, o, name) for o in objs])


def call_node(an, node, tree, unit, ctx):
    """The factory call a JSX element's node makes: the classic factory's call tree, else the element's own call."""
    config = options(an, unit.file)
    if config.get(JSX_MODE_OPTION) in JSX_CLASSIC:
        factory = dotted(config.get(JSX_FACTORY_OPTION) or JSX_FACTORY_DEFAULT)
        return an.node((CALL, factory, (tree[1], tree[2])), unit, ctx)
    return (JSX_CALL,) + node[1:]


def written(an, unit, ref, ctx) -> None:
    """A JSX element's reference: its line on the factory call its node makes."""
    tree = (JSX, ref.base, ref.args[0] if ref.args else (LITERAL, ""))
    node = an.node(tree, unit, ctx)
    an.written(call_node(an, node, tree, unit, ctx), unit.gid, ref.line)


def options(an, file: str) -> dict:
    """The JSX options of the nearest TypeScript configuration above a file."""
    folder = file.rsplit("/", 1)[0] if "/" in file else ""
    return next((config for _, config in an.ix.js.tsconfigs_of(folder)), {})


def runtime(an, file: str, config: dict, mode) -> str | None:
    """The automatic transform's runtime module, as the file resolves it."""
    if mode not in JSX_AUTOMATIC:
        return None
    return an.ix.js.resolve(file, (config.get(JSX_SOURCE_OPTION) or JSX_SOURCE_DEFAULT) + JSX_AUTOMATIC[mode][0])


def dotted(text: str) -> tuple:
    """A dotted name as the tree of its reads."""
    head, *rest = text.split(".")
    tree = (NAME, head)
    for part in rest:
        tree = (MEMBER, tree, part, False)
    return tree


def children_name(an, file: str, config: dict, mode) -> str | None:
    """The props' member the declarations give an element's children: the one property of
    `JSX.ElementChildrenAttribute`, read in the runtime module or the scope of the file."""
    js = an.ix.js
    module = runtime(an, file, config, mode)
    keys = []
    if module is not None:
        for space in js.export(module, JSX_NAMESPACE)[0]:
            keys.append(an.ix.inner[an.ix.units[space].qual].get(JSX_CHILDREN_ATTRIBUTE))
    keys.append(js.class_of(JSX_NAMESPACE + "." + JSX_CHILDREN_ATTRIBUTE, file))
    for key in keys:
        found = [n for n, gids in an.ix.members[key].items() if any(an.ix.units[g].kind in STATE_KINDS for g in gids)] \
            if key else []
        if found:
            return found[0]
    return None


def named_children(an, obj: int, name: str) -> None:
    key = an.objects.class_of(obj)
    for gid in an.ix.members[key].get(JSX_CHILDREN, []) if key else []:
        an.solver.follow(an.initial(gid), members.field(an, obj, name))
