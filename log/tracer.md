# The generic feature tracer (`tools/tracer`)

Record of the standalone tracer the owner ordered on 2026-11-11: a new folder, a new tool, tied to no other project,
generic, tracing features in the established languages (GDScript, C++, TypeScript/JavaScript). The older tracer
(`tools/analysis/featuretrace`, records `trace-*.md`) stays as it is: nothing of it is imported, nothing was removed.

## What it is (2026-11-12)

`python tools/tracer/trace.py <start> --root <folder>...` reads the code under the roots and shows, from a start (a
label such as `Shop.open`, the end of one, or `path:line`), what runs after it and what reaches it. Each link says its
kind (`call`, `new`, `handed`: a function given to code the tracer did not read, which may run it) and how it was
found (`exact`: by the language's scopes and declared types; `by name`). `--list`, `--exact`, `--depth`, `--back`,
`--json`, `--out`. Exit 1 on a start naming nothing or roots holding no code.

## Design, and where each rule comes from

- **One engine, a row and a pattern file per language.** `languages.py` names the grammar (tree-sitter, installed with
  Python) and the rules of the language no pattern can say, each from the language's reference (a file is a class and
  a member is looked up in the object as it runs: GDScript reference; a method named as its class makes its objects:
  C++ [class.ctor]; a declaration that reads as a function's or an object's: C++ [dcl.ambig.res]). `patterns/*.scm`
  say where each fact is in the grammar, in the tracer's own vocabulary (`vocab.py`). A pattern that is one kind of
  node (`(identifier) @ident`) is kept as a table by kind, never matched node by node.
- **Names by the language's scopes** (`scopes.py`): the function, the class and its bases where a plain name reads a
  member, the namespace (one table for every place writing it, seen from the files that reach the one writing a name:
  itself and what it includes), the file with its imports and includes. A name no scope gives is none of the code read.
- **Values flow by variable, parameter, result** (`flow.py`, `solver.py`); no object is told from another of its class.
  A declared type gives an object of each class it names; a type naming none of the code's gives an object of a type
  not defined here, whose members are not the code's where the language is statically typed.
- **By name, said so in every link, and never the ground for another find by name** (`reads.py`, `solver.py`): a member
  *called* on an object nothing is known of is a function of that name of some class of that language (a script also
  reaches another language's classes below a type not defined here). Functions written under a property name by a
  write whose object no class declares that member for are read back under the name (field-based flow: Feldthaus,
  Schäfer, Sridharan, Dolby, Tip, "Efficient Construction of Approximate Call Graphs for JavaScript IDE Services",
  ICSE 2013), by code of the same language.
- **Handed and kept** (`calls.py`): where no definition with a body answers a call, the functions among its arguments
  are handed over and kept by the object called on (kept, not the object's value). A later call on that object that is
  given no function may run them, unless the code hands functions through that same member (it keeps, it does not run).
- **What an expression holds is what a pattern says** (`pass`, `cast`): a choice, brackets, a cast; a sum holds nothing.

## What the game's own code showed (this repository, `native/src` + `project/game|ui|render`: 911 files)

Each was a design decision of the tracer, found by reading the links it produced, fixed at the decision, with a test
that failed first (`tests/test_shapes.py`):

| Found | Cause in the tracer | After |
|---|---|---|
| 4.3 GB and no end | every identifier and literal kept as a fact per node | roles by node kind; the read takes 2 s |
| 285,683 links for 51,435 calls | a C++ name no scope gave was looked up by name everywhere; a member of a `std::` or engine type was looked up in every class with an outside base, scripts included | a plain name is the scopes' or outside; an outside type's members are outside where types are static |
| the function `deck()` read as the local `deck` | `const bool deck = ..., low = ...;` defined one name: the fact was anchored on the declaration | one fact per declarator |
| a lambda stored in `ground.height` reaching 31,164 nodes | a node with no role held its children's values (a sum carried functions); every member write fed the property-name bucket | values pass only where a pattern says; a declared member is written in the member, not under its name |
| 57 handlers "handed" at every `.connect(...)` | a function kept by `x.pressed` was treated as the value of `pressed`, read back by any `d.pressed` | kept is not the value; a member the code hands functions through keeps, it does not run |
| `for r in rows` reading the comparator of `rows.sort_custom(cmp)` | an element read turned what the object keeps into a function | it runs only where it is called, by name |

Whole run now: 8.6 s; 20,671 links: 14,842 `call` exact, 3,983 `call` by name, 888 `new`, 821 `handed` exact, 137
`handed` by name. Example checked by reading the game: `road_tool.click` reaches `RoadBlocks.lay`, the store's
`dispatch` (by name: an autoload) and back from `ToolController._unhandled_input`.

## Tests (`python -m unittest discover -s tests`, from `tools/tracer`; 61, about 2 s)

`test_languages.py` and `test_shapes.py` (the links expected in the fixtures `gd`, `ts`, `cpp`, `mix`), `test_solver.py`,
`test_command.py`, and `test_generic.py`: every text the Python writes that reads as a name is a word of `vocab.py` or
one of the listed words of the command; the only imports are Python's, the grammar library and its own; a pattern
compares a node's text only with words of its language.

## Not covered (each stops a trace where it applies)

- **Links by text.** A function handed over together with a text and run by a call given that text
  (`bind_method("x", &C::x)`, `Callable(obj, "x")`, `connect("sig", f)`/`emit_signal("sig")`). In the game this is
  where a trace from GDScript stops before the C++ core: `city_store.dispatch` ends at `_routes[command].callv`.
- TypeScript path aliases (`tsconfig` `paths`, `baseUrl`) are not read: an import written with an alias resolves only
  where the file's path ends as written. CommonJS `require` and `module.exports` are not read.
- C++: macros are not expanded; the overloads of one name are not told apart; operators are not calls; what a
  template or an outside function gives back is unknown (found by name where it is called on).
- GDScript: property `get`/`set` blocks, `match` bindings, calls by a text (`call("x")`); an autoload is known only by
  name.
- A member read and not called on an object nothing is known of is not looked up by name (it is most often data).
- It was run on this repository's game code only; not on the lab (the owner's order).

## Files

Created: `tools/tracer/trace.py`, `tools/tracer/tracer/**` (`__init__`, `vocab`, `languages`, `model`, `read`, `program`,
`sources`, `scopes`, `solver`, `flow`, `reads`, `stores`, `calls`, `walk`, `report`, `patterns/{gdscript,cpp,typescript,
typescript_only,jsx}.scm`), `tools/tracer/tests/**`, this record. Changed: `tools/gates/source.py` (a lexer for `.scm`),
`log/mapa.md`, `CLAUDE.md` (the tool's line). Deleted: none.
