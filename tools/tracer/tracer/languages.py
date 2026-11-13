"""The languages the tracer reads: for each, its grammar (a tree-sitter grammar installed with Python), the pattern
files that say where each fact is in that grammar (`patterns/*.scm`), and the rules of the language no pattern can
say, each from the language's own reference. Adding a language is a row here and its pattern file.

A pattern that is one kind of node and nothing else (`(identifier) @ident`) says what every node of that kind is: it
is kept as a table by kind and never matched node by node.
"""
from __future__ import annotations

import importlib
import re
from dataclasses import dataclass
from functools import cached_property
from pathlib import Path

from tree_sitter import Language as Grammar
from tree_sitter import Parser, Query

from .vocab import (CROSS, DISTANCE, DOT, LENGTH, NORMALIZE)
from .vocab import (ABS, ADD, ALL, ANY, ATAN, ATAN2, BY_KIND, CEIL, CLAMP, COS, DOUBLES, DYNAMIC, EQ, EXP,
                    FLOOR, FRACT, LOG, MAX, MIN, MIX, NE, NOT, POW, REAL32, ROLES, ROUND, SHADER, SIGN, SIN,
                    SMOOTH, SQRT, STEP, SUB, TAN, TRUNC, TYPED)

PATTERNS = Path(__file__).resolve().parent / "patterns"
COMMENT = re.compile(r"^\s*;.*$", re.MULTILINE)
ALONE = re.compile(r"^(?:\(([\w.]+)\)|\[((?:\s*\([\w.]+\))+\s*)\])\s*@([\w.]+)$")    # `(kind) @role`, `[(kind) ...] @role`
KIND = re.compile(r"\(([\w.]+)\)")


@dataclass(frozen=True)
class Language:
    name: str
    package: str                       # the Python package holding the grammar
    entry: str                         # the function of it that gives the grammar
    patterns: tuple                    # its pattern files
    implicit_members: bool = False     # a plain name reads a member of the class around it (GDScript, C++)
    file_is_class: bool = False        # a file is a class: its functions are its methods (GDScript reference)
    implicit_base: bool = False        # a class that extends nothing extends one the language defines (GDScript)
    duck_typed: bool = False           # a member is looked up in the object as it runs, whatever type is declared
    #                                    for it (GDScript reference: duck typing); elsewhere it is the declared type's
    class_named_constructor: bool = False   # a method named as its class makes its objects (C++ [class.ctor])
    super_names_method: bool = False   # the base called alone runs the base's function of this one's name (GDScript)
    ambiguous_declarations: bool = False    # a declaration in a body that reads as a function's or an object's is an
    #                                         object's unless its parameters name types (C++ [dcl.ambig.res])
    beside: bool = False               # a source not written as relative is still looked for beside the file
    folder_file: tuple = ()            # the file a folder stands for in an import (TypeScript module resolution)
    project: str = ""                  # the file the language reads to resolve names (`configs.READERS`)
    operators: str = ""                # what an operator applied to an object of a class calls: the function of
    #                                    this name followed by the operator (C++ [over.oper])
    called: str = ""                   # the operator an object of a class called runs (C++ [over.call]: `()`)
    invokes: tuple = ()                # the members of a function value that run it: (name, where its own arguments
    #                                    start among the call's; None: they come as one list)
    makers: tuple = ()                 # the calls that make a function value of an object's method a text names, as
    #                                    written (they run nothing: the value runs where it is invoked)
    text_ends: tuple = ()              # the members of a text that give its end: a name written under its scopes
    #                                    (`area/method`) names its last part (model.named), and so does that end
    aggregate: bool = False            # a class with no constructor made with values takes them as its fields, in
    #                                    the order they are declared (C++ [dcl.init.aggr])
    node_marks: str = ""               # what starts a node's path written in place, and a unique name ($, %)
    node_lookup: tuple = ()            # the functions that find a node by its path written as a text
    spelled: tuple = ()                # the operators written otherwise than the tracer writes them: (as written, as
    #                                    the tracer writes it)
    truths: tuple = (("true", True), ("false", False))   # the words of the two truth values
    steps: tuple = ()                  # the operators that add one to a variable or take one from it
    numbers: str = DOUBLES             # how its numbers behave (`vocab`: DOUBLES, TYPED, DYNAMIC)
    whole_types: tuple = ()            # the declared types holding whole numbers
    real32_types: tuple = ()           # the declared types holding reals in 32 bits (the rest: 64)
    divides_whole: bool = False        # a whole number divided by a whole number gives the whole part (toward zero)
    math: tuple = ()                   # the functions of numbers the language defines: (as written, `vocab` head)
    counts: bool = False               # a loop over a number goes over 0 .. number - 1 (GDScript `for i in 3`)
    keys_word: str = ""                # the word of a loop over an object's keys (the other loop goes over values)
    picks_operand: bool = False        # `a || b`, `a && b` give one of the operands, not a truth (ECMA-262)
    falls_through: bool = False        # a case of a choice runs on into the next until a break (C++, ECMA-262)
    prepare: str = ""                  # how a file is prepared before it is read (`vocab.SHADER`)
    entries: tuple = ()                # the functions a shader's pipeline runs (its entry points)
    outputs: tuple = ()                # what a shader writes that leaves it (the pipeline reads it)
    blanked: tuple = ()                # the words a shader writes that C does not, read as nothing (qualifiers)
    uniform_word: str = ""             # the word declaring a value the code outside sets (a uniform)
    leaves: tuple = ()                 # the statements that leave the shader there (an output: the pipeline sees it)
    placed: str = ""                   # the qualifier written with its arguments in brackets (`layout(...)`)
    declarations: tuple = ()           # the statements of the file itself, read as nothing (`shader_type x;`)
    nothing: tuple = ()                # the words of no value (null)
    real64_types: tuple = ()           # the declared types holding reals in 64 bits, where the language names them
    real32_marks: str = ""             # the letters after a number written that make it a 32-bit real (`1.5f`)
    size_words: tuple = ()             # the member a collection says its size by (`length`)
    compiler: str = ""                 # the compiler that answers what each call runs (`compilers.py`), if any
    vectors: tuple = ()                # the vector types and their sizes (`vec3`: 3), each made by its name
    components: tuple = ()             # the letters naming a vector's components, each set in order (`xyzw`)
    vector_math: tuple = ()            # the functions of vectors: (as written, `vocab` head)
    mix_weights_both: bool = False     # mix(x, y, a) is x·(1−a) + y·a (GLSL 4.60 §8.3), not x + (y − x)·a

    @cached_property
    def grammar(self) -> Grammar:
        return Grammar(getattr(importlib.import_module(self.package), self.entry)())

    @cached_property
    def compiled(self) -> tuple:
        """The pattern files as queries, and what a node's kind alone says it is: {kind: (role, no parts)}."""
        queries, by_type = [], {}
        for name in self.patterns:
            source = (PATTERNS / name).read_bytes()
            query = Query(self.grammar, source.decode("utf-8"))
            for i in range(query.pattern_count):
                written = source[query.start_byte_for_pattern(i):query.end_byte_for_pattern(i)].decode("utf-8")
                alone = ALONE.match(COMMENT.sub("", written).strip())
                if not alone or alone.group(3) not in BY_KIND:
                    continue
                query.disable_pattern(i)
                for kind in ([alone.group(1)] if alone.group(1) else KIND.findall(alone.group(2))):
                    known = by_type.get(kind)
                    if known is None or ROLES.index(alone.group(3)) < ROLES.index(known[0]):
                        by_type[kind] = (alone.group(3), {})
            queries.append(query)
        return tuple(queries), by_type

    @cached_property
    def math_table(self) -> dict:
        """`math` as a table, made once."""
        return dict(self.math)

    @cached_property
    def vectors_table(self) -> dict:
        """`vectors` as a table, made once."""
        return dict(self.vectors)

    @cached_property
    def vector_math_table(self) -> dict:
        """`vector_math` as a table, made once."""
        return dict(self.vector_math)

    @cached_property
    def invokes_table(self) -> dict:
        """`invokes` as a table, made once."""
        return dict(self.invokes)

    @cached_property
    def spelled_table(self) -> dict:
        """`spelled` as a table, made once."""
        return dict(self.spelled)

    @cached_property
    def truths_table(self) -> dict:
        """`truths` as a table, made once."""
        return dict(self.truths)

    @property
    def queries(self) -> tuple:
        return self.compiled[0]

    @property
    def by_type(self) -> dict:
        return self.compiled[1]

    def parser(self) -> Parser:
        return Parser(self.grammar)

    def __hash__(self) -> int:
        return hash(self.name)


# The functions of numbers each language defines, as written, and what each is (`vocab`).
JS_MATH = tuple((f"Math.{name}", head) for name, head in (
    ("sqrt", SQRT), ("pow", POW), ("sin", SIN), ("cos", COS), ("tan", TAN), ("abs", ABS), ("min", MIN), ("max", MAX),
    ("floor", FLOOR), ("ceil", CEIL), ("round", ROUND), ("atan", ATAN), ("atan2", ATAN2), ("exp", EXP), ("log", LOG),
    ("trunc", TRUNC), ("sign", SIGN), ("fround", REAL32)))
#   ECMA-262, The Math Object (21.3): Math.sqrt .. Math.fround (fround: the nearest 32-bit real)
GD_MATH = (("sqrt", SQRT), ("pow", POW), ("sin", SIN), ("cos", COS), ("tan", TAN), ("abs", ABS), ("absf", ABS),
           ("absi", ABS), ("min", MIN), ("minf", MIN), ("mini", MIN), ("max", MAX), ("maxf", MAX), ("maxi", MAX),
           ("floor", FLOOR), ("floorf", FLOOR), ("ceil", CEIL), ("ceilf", CEIL), ("round", ROUND), ("roundf", ROUND),
           ("atan", ATAN), ("atan2", ATAN2), ("exp", EXP), ("log", LOG), ("clamp", CLAMP), ("clampf", CLAMP),
           ("clampi", CLAMP), ("lerp", MIX), ("sign", SIGN), ("signf", SIGN))
#   GDScript reference, @GlobalScope: sqrt, pow, sin, cos, tan, abs*, min*, max*, floor*, ceil*, round*, atan, atan2,
#   exp, log, clamp*, lerp (a + (b - a) * t), sign*
CPP_MATH = tuple((f"{scope}{name}", head) for scope in ("", "std::") for name, head in (
    ("sqrt", SQRT), ("sqrtf", SQRT), ("pow", POW), ("powf", POW), ("sin", SIN), ("cos", COS), ("tan", TAN),
    ("abs", ABS), ("fabs", ABS), ("fabsf", ABS), ("min", MIN), ("max", MAX), ("fmin", MIN), ("fmax", MAX),
    ("floor", FLOOR), ("ceil", CEIL), ("round", ROUND), ("atan", ATAN), ("atan2", ATAN2), ("exp", EXP), ("log", LOG),
    ("trunc", TRUNC), ("clamp", CLAMP)))
#   ISO C++ [c.math]: <cmath> sqrt .. trunc; [alg.min.max] std::min, std::max, std::clamp
SHADER_MATH = (("sqrt", SQRT), ("pow", POW), ("sin", SIN), ("cos", COS), ("tan", TAN), ("abs", ABS), ("min", MIN),
               ("max", MAX), ("floor", FLOOR), ("ceil", CEIL), ("round", ROUND), ("atan", ATAN), ("exp", EXP),
               ("log", LOG), ("trunc", TRUNC), ("clamp", CLAMP), ("mix", MIX), ("step", STEP), ("smoothstep", SMOOTH),
               ("fract", FRACT), ("sign", SIGN))
SHADER_VECTORS = (("vec2", 2), ("vec3", 3), ("vec4", 4))           # GLSL 4.60 §4.1.5; the Godot shading language's
SHADER_COMPONENTS = ("xyzw", "rgba", "stpq")                      # GLSL 4.60 §5.5
SHADER_VECTOR_MATH = (("dot", DOT), ("length", LENGTH), ("distance", DISTANCE), ("normalize", NORMALIZE),
                      ("cross", CROSS))                           # GLSL 4.60 §8.5
#   OpenGL Shading Language 4.60, 8.1-8.3 (Angle and Trigonometry, Exponential, Common Functions); Godot "Shading
#   language", Built-in functions (the same names)

GDSCRIPT = Language("gdscript", "tree_sitter_gdscript", "language", ("gdscript.scm",), implicit_members=True,
                    file_is_class=True, implicit_base=True, duck_typed=True, super_names_method=True, beside=True,
                    project="godot", invokes=(("call", 0), ("callv", None), ("call_deferred", 0)), makers=("Callable",),
                    text_ends=("substr", "right"),
                    node_marks="$%", node_lookup=("get_node", "get_node_or_null"),
                    spelled=(("and", ALL), ("or", ANY), ("not", NOT), ("**", POW)), numbers=DYNAMIC, divides_whole=True,
                    whole_types=("int",), math=GD_MATH, counts=True, nothing=("null",), real64_types=("float",))
#                   GDScript reference, Operators: `and`, `or`, `not` are `&&`, `||`, `!`; `**` is the power; int / int is
#                   an int (truncated); `for i in 3` goes over 0, 1, 2; float is 64 bits
#                   GDScript reference: `$path` is short for get_node("path"), `%name` for a node unique in its scene
#                   GDScript reference, Callable: call, callv, call_deferred run it (bind, bindv, unbind make another);
#                   Callable(object, method) makes one of the object's method named by the text; String: substr(from)
#                   and right(length) give the text's end
CPP = Language("cpp", "tree_sitter_cpp", "language", ("cpp.scm",), implicit_members=True,
               class_named_constructor=True, ambiguous_declarations=True, beside=True, project="database",
               operators="operator", called="()", aggregate=True,
               spelled=(("and", ALL), ("or", ANY), ("not", NOT)), steps=(("++", ADD), ("--", SUB)),
               numbers=TYPED, divides_whole=True, math=CPP_MATH, falls_through=True, compiler="clang",
               whole_types=("int", "long", "short", "char", "unsigned", "size_t", "int32_t", "int64_t", "uint32_t",
                            "uint64_t", "auto"),
               real32_types=("float",), nothing=("nullptr", "NULL"), real64_types=("double",), real32_marks="fF")
#              C++ [lex.digraph]: `and`, `or`, `not` are `&&`, `||`, `!`; [expr.pre.incr]; [expr.mul]: integral
#              division truncates toward zero; [basic.fundamental]: float, double; [stmt.switch]: cases fall through
TYPESCRIPT = Language("typescript", "tree_sitter_typescript", "language_typescript",
                      ("typescript.scm", "typescript_only.scm"), folder_file=("index",), project="typescript",
                      invokes=(("call", 1), ("apply", None)),
                      spelled=(("===", EQ), ("!==", NE), ("**", POW)), steps=(("++", ADD), ("--", SUB)), math=JS_MATH,
                      keys_word="in", picks_operand=True, falls_through=True, nothing=("null", "undefined"),
                      size_words=("length",), compiler="typescript")
#                     ECMA-262, Exponentiation Operator: `**` is Math.pow; Equality Operators: `===` and `!==` compare
#                     without conversion; UpdateExpression;
#                     Number: every number a 64-bit real; for-in goes over keys, for-of over values; || and && give
#                     an operand; switch cases fall through
TSX = Language("tsx", "tree_sitter_typescript", "language_tsx", ("typescript.scm", "jsx.scm"), folder_file=("index",),
               project="typescript", invokes=(("call", 1), ("apply", None)),
               spelled=(("===", EQ), ("!==", NE), ("**", POW)), steps=(("++", ADD), ("--", SUB)), math=JS_MATH,
               keys_word="in",
               picks_operand=True, falls_through=True, nothing=("null", "undefined"), size_words=("length",),
               compiler="typescript")
#   ECMA-262, Function.prototype: call (its first argument is `this`) and apply run it; bind makes another
GLSL = Language("glsl", "tree_sitter_cpp", "language", ("cpp.scm",), beside=True,
                spelled=(("and", ALL), ("or", ANY), ("not", NOT)), steps=(("++", ADD), ("--", SUB)),
                numbers=TYPED, divides_whole=True, math=SHADER_MATH, whole_types=("int", "uint", "bool"),
                real32_types=("float", "vec2", "vec3", "vec4"), prepare=SHADER, entries=("main",),
                outputs=("gl_Position", "gl_FragColor", "gl_FragDepth", "gl_PointSize"),
                blanked=("uniform", "in", "out", "inout", "attribute", "varying", "flat", "smooth", "noperspective",
                         "centroid", "highp", "mediump", "lowp", "precision", "invariant", "readonly", "writeonly",
                         "coherent", "volatile", "restrict", "buffer", "shared"),
                uniform_word="uniform", leaves=("discard",), placed="layout", vectors=SHADER_VECTORS,
                components=SHADER_COMPONENTS, vector_math=SHADER_VECTOR_MATH, mix_weights_both=True)
#   OpenGL Shading Language 4.60: C-like syntax (1.1); storage and precision qualifiers (4.3, 4.7); `main` is the
#   entry point (7.1); gl_Position, gl_FragColor (compatibility), gl_FragDepth, gl_PointSize are written to leave the
#   shader (7.1); float is 32 bits (4.1.4); `discard` leaves the fragment (6.4)
GODOT_SHADER = Language("godot shader", "tree_sitter_cpp", "language", ("cpp.scm",), beside=True,
                        spelled=(("and", ALL), ("or", ANY), ("not", NOT)), steps=(("++", ADD), ("--", SUB)),
                        numbers=TYPED, divides_whole=True, math=SHADER_MATH, whole_types=("int", "uint", "bool"),
                        real32_types=("float", "vec2", "vec3", "vec4"), prepare=SHADER,
                        entries=("vertex", "fragment", "light", "start", "process", "sky", "fog"),
                        outputs=("VERTEX", "POSITION", "NORMAL", "TANGENT", "BINORMAL", "UV", "UV2", "COLOR",
                                 "POINT_SIZE", "ALBEDO", "ALPHA", "ALPHA_SCISSOR_THRESHOLD", "METALLIC", "ROUGHNESS",
                                 "SPECULAR", "EMISSION", "NORMAL_MAP", "RIM", "CLEARCOAT", "AO", "DIFFUSE_LIGHT",
                                 "SPECULAR_LIGHT", "LIGHT", "VELOCITY", "CUSTOM", "ACTIVE", "TRANSFORM", "FOG",
                                 "DENSITY", "SCREEN_UV", "INSTANCE_CUSTOM"),
                        blanked=("uniform", "varying", "in", "out", "inout", "flat", "smooth", "highp", "mediump",
                                 "lowp", "instance", "global"),
                        uniform_word="uniform", leaves=("discard",), declarations=("shader_type", "render_mode"),
                        vectors=SHADER_VECTORS, components=SHADER_COMPONENTS, vector_math=SHADER_VECTOR_MATH,
                        mix_weights_both=True)
#   Godot "Shading language": C-like syntax; `shader_type`, `render_mode` are declarations of the file; uniforms take
#   hints after `:`; the processor functions vertex, fragment, light (spatial, canvas_item), start, process
#   (particles), sky, fog are run by the pipeline; the built-ins written to leave each one are listed per shader type
#   (VERTEX .. INSTANCE_CUSTOM); float is 32 bits; `discard` leaves the fragment
# The compilers that answer what each call runs (`compilers.py`), each resolving names, overloads, templates and
# types as its language does: clang for C++ (its Python library, `clang.cindex`, the C++ compiler's own front end), the
# TypeScript compiler for TypeScript and JavaScript (`typescript` installed in `tools/`, run by node with the script
# there). A C++ compiler of the system (the first found) is asked where its standard library's headers are and what it
# compiles for (`-v`, `-dumpmachine`): clang reads the program with them.
CLANG, TSC = "clang", "typescript"
NODE = "node"
TOOLS, TS_SCRIPT = "tools", "calls.cjs"
NODE_PACKAGES = "node_modules"           # where node keeps the packages that script uses (the compiler: not the tracer's)
TOOLCHAINS = ("g++", "clang++", "c++")
SEARCH_STARTS, SEARCH_ENDS = "#include <...> search starts here:", "End of search list."
PATH_VARIABLE = "PATH"
SYSTEM_PATHS = (("HKEY_CURRENT_USER", "Environment"),       # where Windows keeps the PATH it gives a new process
                ("HKEY_LOCAL_MACHINE", r"SYSTEM\CurrentControlSet\Control\Session Manager\Environment"))
PATH_KEY = "Path"
EXACT_CALL, DISPATCHED_CALL, OUTSIDE_CALL = "exact", "dispatched", "outside"   # the TypeScript script's answers

import json  # noqa: E402

# The data formats whose files the tracer reads as values a program may load: the extension and how it is read
# (ECMA-404 / RFC 8259: JSON).
DATA_FORMATS = {".json": json.loads}

BY_EXTENSION = {
    ".gd": GDSCRIPT,
    ".glsl": GLSL, ".vert": GLSL, ".frag": GLSL, ".geom": GLSL, ".comp": GLSL, ".tesc": GLSL, ".tese": GLSL,
    ".gdshader": GODOT_SHADER, ".gdshaderinc": GODOT_SHADER,
    ".cpp": CPP, ".cc": CPP, ".cxx": CPP, ".h": CPP, ".hpp": CPP, ".hh": CPP, ".hxx": CPP, ".inl": CPP,
    ".ts": TYPESCRIPT, ".mts": TYPESCRIPT, ".cts": TYPESCRIPT,
    ".tsx": TSX, ".js": TSX, ".jsx": TSX, ".mjs": TSX, ".cjs": TSX,
}
