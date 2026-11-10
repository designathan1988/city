# JavaScript / TypeScript / three.js in the feature tracer: implementation plan (2026-11-10)

**Goal:** the generic tracer (`tools/analysis/trace_feature.py`) traces a feature through JavaScript, TypeScript, JSX/TSX
and anything built on them (three.js included) by the languages' rules, the same way it traces GDScript and C++.

**Architecture:** one more reader family on the same IR and solver: `js_read` (definitions, imports, exports), `js_body`
(statements and references), `ir_js` (expression trees), `js_modules` (specifier resolution, export lookup, tsconfig and
package.json). The index, resolver, typer and linker get a `js` branch only where the language's rule differs (lookup
order, module namespaces, constructors, events). three.js is not named anywhere: it is code when its source is read,
a library otherwise (J11).

**Spec:** `log/trace-rules.md`, sections "JavaScript and TypeScript" (sources J1-J12, the general rules applied, every
construct of the TSX grammar, 184 kinds). The owner's order: research first, code without trial and error, test at the
end; generic, no project named.

## Global constraints
- English in the repository; every file starts with a header comment; files under 300 lines; `log/mapa.md` lists
  new files; gates pass (`python tools/gates/run_gates.py --files-from`).
- Grammars: `tree_sitter_typescript.language_typescript()` for `.ts .mts .cts`, `language_tsx()` for `.tsx .js .jsx
  .mjs .cjs`; `.d.ts` files read as TypeScript with every function `decl_only`.
- Skipped folders added: `node_modules` is read only when a root points inside it (a library is code when asked for).
- Keys: module key = the file's display path; class key = `<module key>#<Name>` (nested: `#Outer.Inner`, object
  literal: `#<obj:line>`, namespace: `#<Ns>`); unit qual = `<module key>:<Name>[.<member>]`.

## Tasks (code all, review all, then test)
1. **Rules data** (`rules.py`): `JS_KEYWORDS` (MDN reserved words + TS contextual keywords, each with its role);
   `EVENT_SUBSCRIBE = {"addEventListener", "on", "once", "addListener", "prependListener"}` (type arg 0, handler arg 1);
   `EVENT_FIRE = {"dispatchEvent": event object/constructor, "emit": arg 0}`, `MESSAGE_FIRE = {"postMessage"}` ->
   "message"; `HANDLER_PREFIX = "on"`; `HIGHER_ORDER` = callback parameter values per array/promise method (`forEach
   map filter find findIndex findLast findLastIndex some every flatMap sort` -> elements; `reduce reduceRight` ->
   (accumulator, element); `then` -> awaited value; `catch finally` -> engine value); `CALL_FORMS = {"call": 1,
   "apply": 1}` (arguments after thisArg), `BIND_FORMS = {"bind"}`; HANDLERS rows for the new modules.
2. **Sources** (`source.py`, `program.py`): extensions and grammars above; `tsconfig*.json`/`jsconfig.json` read as
   lang `tsconfig`, `package.json` as lang `package` (JSONC parser: comments and trailing commas removed); `reader_for`
   dispatches `js`, `tsconfig`, `package`.
3. **Facts** (`model.py`): `FileFacts.imports` {local: (specifier, imported | "default" | "*")}, `exports` {name: local
   | ("from", specifier, name)}, `star_exports` [specifier], `config` dict (tsconfig paths/baseUrl, package name/
   exports/module/main/types), `script` bool (no import/export: top-level names are global).
4. **Reader** (`js_read.py`, `js_body.py`, `ir_js.py`): units per the table: `<module>` function per file holding the
   top-level code; classes (ClassFact lang `js`, bases from `extends` and `implements`), methods, getters/setters as
   accessor units of a field unit, fields (instance initializers read into the constructor unit; a class without a
   constructor gets the default one with an implicit `super` call), `<static>` unit for static fields and blocks;
   functions and arrows (named after their variable, J9; nested ones are lambdas with `enclose`); object literals as
   classes; interfaces, enums, namespaces, type aliases (`aliases`); references: call, invoke, new, read, write,
   value, lambda, dynamic (computed member with a non-literal key: the key tree in `handler`), connect/emit for events,
   implicit calls (getter/setter via accessors, `[Symbol.iterator]` on for-of/spread, `then` on await, `toString` on
   template substitution, `valueOf` on arithmetic, decorators, tagged templates, JSX components).
5. **Modules** (`js_modules.py`): `resolve_specifier(from_file, spec) -> str | None` (relative with extension
   substitution `.js->.ts/.tsx`, extensionless, `index.*`, tsconfig `paths`/`baseUrl`, `node_modules/<pkg>` with
   package.json `exports` (conditions import, module, browser, default, types) / `module` / `main` / `types`);
   `export_targets(module, name) -> list[int]` following `export ... from` and `export *`; `binding(file, local) ->
   (gids, module key | None)`.
6. **Index / resolver / typer / linker**: Index builds `JsModules`; `Index.lookup` on a module key answers by its
   exports; `js_class(text, file)`; resolver `bare` js branch (lambdas, locals and captures, module names, imports,
   globals); typer `named`/`type_class` js branch, `as`/`satisfies` keep the value; linker: js `constructors`, events
   (subscribe/fire by type on non-program receivers -> engine signal of that type), `dynamic` members by text values,
   callables written to members of non-program values are handed, higher-order callbacks get their values, `call`/
   `apply` shift arguments; walk entry reasons for `<module>`, `<static>` and library-called methods (a class with a
   base the program does not define).
7. **Review** every changed file against the table (each row's rule has its handler).
8. **Test at the end:** a fixture `tests/fixture/web/` (TS + JS + JSX + a stub `node_modules/three` package with
   EventDispatcher/Object3D/WebGLRenderer in JS) and `tests/test_js.py`; then the rule-table test for the TSX grammar;
   then a real run on a JS/TS + three.js project; gates; log; commit and push.

## Review focus
- A barrel file chain (`export * from` through two files) must still reach the definition.
- A method called only by a library base class (three.js `raycast`, `onBeforeRender` written as a method) must be an
  entry, not dead code.
- `this` inside an arrow inside a method is the class; inside an object literal's method the object.
- A `.js` specifier in a `.ts` file resolves to the `.ts` file.
- An event fired with a type held in a variable reaches the handlers of each text value it holds.
