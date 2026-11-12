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
  *Done in the second step, below; what is left of it:* a signal's name given as a text (`connect("sig", f)`,
  `emit_signal("sig")`), a class named by a text (`ClassDB.instantiate("CityCore")`), a name cut out of a text by
  the code itself (`spec.substr(cut + 1)`: only the name a text ends with is read).
- TypeScript path aliases (`tsconfig` `paths`, `baseUrl`) are not read: an import written with an alias resolves only
  where the file's path ends as written. CommonJS `require` and `module.exports` are not read.
- C++: macros are not expanded; the overloads of one name are not told apart; operators are not calls; what a
  template or an outside function gives back is unknown (found by name where it is called on).
- GDScript: property `get`/`set` blocks, `match` bindings, calls by a text (`call("x")`); an autoload is known only by
  name.
- A member read and not called on an object nothing is known of is not looked up by name (it is most often data).
- It was run on this repository's game code only; not on the lab (the owner's order).

## Second step (2026-11-12): links by text, and a trace that follows what each call gave

The first trace of a tool's click stopped at the store: the scripts reach the C++ core through names written as
texts (`CityStore.dispatch(&"build_road_path", ...)` on one side, `ClassDB::bind_method(D_METHOD("build_road_path",
...), &CityCore::build_road_path)` on the other, a `Callable(core, name)` kept in a table between them).

- **A text that names a function is a value** (`TXT`): the text itself where it is one name, the name it ends with
  where it is names joined by marks (the way a name is written under its scopes); only names of the code's functions.
- **What the result of a call no definition answers is built with, it keeps** (`AS_BUILT`): the functions and the
  names it was given. A container keeps the functions it is handed, never the texts.
- **By text** (`calls.by_text`, a third way a link is found, said in the link): a function handed over together
  with the text of its own name is known by that text; a call no definition answers, on an object that keeps that
  name, may run it. A text given to such a call says nothing alone; with an object whose class has a method of that
  name, it names the method (`call_deferred("lift")`, `Callable(self, "lift")`).
- **The trace follows what each call gave** (`walk.py`; the links stay as the flow found them, no object or call is
  told from another there): a function entered by a call that gave a function's name is followed by text to the
  function of that name, where there is one; one entered with functions given, not to the functions only its other
  callers give it; and back the same way. The click's trace went from 5,800 lines to 472 at depth 7.

Each with a test that failed first (`test_shapes.Text`, `test_command`). What the game's code showed on the way:
a file path split into names linked `DataTables._read` to `CityCore.overlays` (now only the name a text ends with);
a text kept in a vector linked `items_.size()` to `CityCore.strike` (a container keeps functions, not texts); a call
answered by name kept its arguments in its result (only where no definition answers).

Whole run: 9.5 s; 21,751 links: 14,861 `call` exact, 4,001 `call` by name, 691 `call` by text, 888 `new`, 1,117
`handed` exact, 192 `handed` by name. Read in the game: `road_tool._lay` -> `city_store.dispatch` (by name) ->
`city.CityCore.build_road_path` (by text) -> `city.run_edit` -> that command's own lambda -> `road_ends`,
`find_anchor`, `build_path`, `place_road`. 71 tests, about 4 s.

Not covered, beyond the list above: a function reached through a function it is given keeps no more than one call
of context (the walk looks one call back); `std::move(f)` and the like give back what they are handed only as kept
(run where called, said by name).

## Files

Created: `tools/tracer/trace.py`, `tools/tracer/tracer/**` (`__init__`, `vocab`, `languages`, `model`, `read`, `program`,
`sources`, `scopes`, `solver`, `flow`, `reads`, `stores`, `calls`, `walk`, `report`, `patterns/{gdscript,cpp,typescript,
typescript_only,jsx}.scm`), `tools/tracer/tests/**`, this record. Changed: `tools/gates/source.py` (a lexer for `.scm`),
`log/mapa.md`, `CLAUDE.md` (the tool's line). Deleted: none.
