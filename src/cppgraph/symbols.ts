// The units of a C++ file, from clangd's hierarchical document symbols: functions, methods and constructors with a
// body, class and struct heads, their fields, enums, and namespace-scope variables. A declaration without a body is
// part of its class head; its definition elsewhere is the unit.
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
  /** A namespace-scope variable that is neither const nor constexpr: state shared by every caller. */
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
  const walk = (list: readonly DocumentSymbol[], scope: readonly string[], inClass: boolean): void => {
    for (const s of list) {
      const base = { range: s.range, selection: s.selectionRange, detail: s.detail ?? '', mutable: false };
      const owner = inClass ? scope.join('::') : s.name.includes('::') ? s.name.slice(0, s.name.lastIndexOf('::')) : null;
      switch (s.kind) {
        case KIND.namespace:
          walk(s.children ?? [], scope, false);
          break;
        case KIND.class: case KIND.struct: case KIND.interface: {
          const name = qualify(scope, s.name);
          out.push({ ...base, name, kind: 'class', owner: inClass ? scope.join('::') : null });
          walk(s.children ?? [], name.split('::'), true);
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
          out.push({ ...base, name: qualify(scope, s.name), kind: 'variable', owner: null, mutable: !/\b(const|constexpr)\b/.test(declaration) });
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
  walk(symbols, [], false);
  return out;
}
