// The units of a C++ file, from clangd's hierarchical document symbols: functions, methods and constructors with a
// body, class and struct definitions, their fields, enums, and namespace-scope variables. A declaration without a body
// (a method declared in its class, a forward-declared `struct X;`) is not a unit: its definition is.
import type { UnitKind } from '../graph/ir.ts';

export interface LspPosition {
  readonly line: number;
  readonly character: number;
}

export interface LspRange {
  readonly start: LspPosition;
  readonly end: LspPosition;
}

export interface DocumentSymbol {
  readonly name: string;
  readonly detail?: string;
  readonly kind: number;
  readonly range: LspRange;
  readonly selectionRange: LspRange;
  readonly children?: DocumentSymbol[];
}

/** LSP SymbolKind values clangd uses for C++. */
const KIND = { namespace: 3, class: 5, method: 6, property: 7, field: 8, constructor: 9, enum: 10, interface: 11, function: 12, variable: 13, constant: 14, struct: 23 };

export interface CppUnit {
  readonly name: string;
  readonly kind: UnitKind;
  readonly range: LspRange;
  readonly selection: LspRange;
  readonly detail: string;
  /** The qualified name of the class it belongs to, or null. */
  readonly owner: string | null;
  /** For a member written inside its class: where that class's name is, in the same file (its definition exactly). */
  readonly ownerAt: LspPosition | null;
  /** A namespace-scope variable that is neither const, constexpr nor thread_local: state shared by every caller. */
  readonly mutable: boolean;
}

export class FileText {
  readonly text: string;
  readonly #lineStarts: number[] = [0];

  constructor(text: string) {
    this.text = text;
    for (let i = 0; i < text.length; i++) if (text.charCodeAt(i) === 10) this.#lineStarts.push(i + 1);
  }

  offset(p: LspPosition): number {
    return (this.#lineStarts[p.line] ?? this.text.length) + p.character;
  }

  slice(range: LspRange): string {
    return this.text.slice(this.offset(range.start), this.offset(range.end));
  }

  get lines(): number {
    return this.#lineStarts.length;
  }
}

function qualify(scope: readonly string[], name: string): string {
  return name.includes('::') ? name : [...scope, name].join('::');
}

export function unitsOf(file: FileText, symbols: readonly DocumentSymbol[]): CppUnit[] {
  const out: CppUnit[] = [];
  // `classAt`: the name position of the class whose body is being walked, null outside a class body.
  const walk = (list: readonly DocumentSymbol[], scope: readonly string[], classAt: LspPosition | null): void => {
    const inClass = classAt !== null;
    for (const s of list) {
      // `using namespace x;` and `using x::y;` are listed as symbols; they declare no state.
      if (/^using\b/.test(s.name)) continue;
      const base = { range: s.range, selection: s.selectionRange, detail: s.detail ?? '', mutable: false, ownerAt: classAt };
      const owner = inClass ? scope.join('::') : s.name.includes('::') ? s.name.slice(0, s.name.lastIndexOf('::')) : null;
      switch (s.kind) {
        case KIND.namespace:
          walk(s.children ?? [], scope, null);
          break;
        case KIND.class: case KIND.struct: case KIND.interface: {
          if (!file.slice(s.range).includes('{')) break; // a forward declaration: the definition is the unit
          const name = qualify(scope, s.name);
          out.push({ ...base, name, kind: 'class', owner: inClass ? scope.join('::') : null });
          walk(s.children ?? [], name.split('::'), s.selectionRange.start);
          break;
        }
        case KIND.function: case KIND.method: case KIND.constructor: {
          if (!file.slice(s.range).includes('{')) break; // a declaration: its definition is the unit
          const kind: UnitKind = s.kind === KIND.constructor ? 'constructor' : owner ? 'method' : 'function';
          out.push({ ...base, name: qualify(scope, s.name), kind, owner });
          break;
        }
        case KIND.field: case KIND.property:
          out.push({ ...base, name: qualify(scope, s.name), kind: 'field', owner: scope.join('::') });
          break;
        case KIND.variable: case KIND.constant: {
          if (inClass) {
            out.push({ ...base, name: qualify(scope, s.name), kind: 'field', owner: scope.join('::') });
            break;
          }
          const declaration = file.slice({ start: s.range.start, end: s.selectionRange.start });
          // thread_local gives each thread its own copy (a re-entrancy guard): not state shared by every caller.
          out.push({ ...base, name: qualify(scope, s.name), kind: 'variable', owner: null, mutable: !/\b(const|constexpr|thread_local)\b/.test(declaration) });
          break;
        }
        case KIND.enum:
          out.push({ ...base, name: qualify(scope, s.name), kind: 'enum', owner: inClass ? scope.join('::') : null });
          break;
        default:
          break;
      }
    }
  };
  walk(symbols, [], null);
  return out;
}
