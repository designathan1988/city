// The role of each unit a feature owns, from its facts (never from its folder): drawing (render), interface (ui),
// tunable values (data), state, code that changes state (tick), pure code (compute), types; and `mixed` when one unit
// does two of these, which the conversion must split.
import type { EdgeRecord, UnitRecord } from '../graph/ir.ts';

export type Role = 'render' | 'ui' | 'data' | 'state' | 'tick' | 'compute' | 'type' | 'mixed';

export interface UnitRole {
  readonly id: string;
  readonly role: Role;
  /** For `mixed`, the roles found together; otherwise the role alone. */
  readonly parts: readonly Role[];
  readonly why: string;
}

const RENDER_API = /^(three:|dom:(WebGL2?RenderingContext|HTMLCanvasElement|CanvasRenderingContext2D|OffscreenCanvas)\b)/;
const UI_API = /^dom:(Document|HTML\w*Element|Element|Node|EventTarget|\w*Event|CSSStyleDeclaration|DOMTokenList|DOMRect)\b/;
const STATE_KINDS = new Set(['field']);
const TYPE_KINDS = new Set(['interface', 'type', 'enum', 'namespace']);

function apiRole(unit: UnitRecord): Role | null {
  const render = unit.facts.external.some((e) => RENDER_API.test(e.key));
  const ui = unit.facts.external.some((e) => UI_API.test(e.key));
  return render && ui ? 'mixed' : render ? 'render' : ui ? 'ui' : null;
}

export function classifyRoles(units: readonly UnitRecord[], byId: Map<string, UnitRecord>, outgoing: Map<string, EdgeRecord[]>): UnitRole[] {
  // First the roles that do not depend on other units, then what each callable writes decides the rest.
  const first = new Map<string, Role>();
  for (const unit of units) {
    const api = apiRole(unit);
    if (TYPE_KINDS.has(unit.kind)) first.set(unit.id, 'type');
    else if (unit.kind === 'class') first.set(unit.id, api ?? 'state');
    else if (STATE_KINDS.has(unit.kind)) first.set(unit.id, api ?? 'state');
    else if (unit.kind === 'variable') first.set(unit.id, unit.facts.mutableModuleState ? 'state' : api ?? 'data');
    else first.set(unit.id, api ?? 'compute');
  }
  // State belongs to the drawing when the class (or factory) that holds it has a member that draws.
  const drawingOwners = new Set(units.filter((u) => u.parent !== null && apiRole(u) === 'render').map((u) => u.parent!));
  const renderState = (id: string): boolean => {
    const target = byId.get(id);
    return target !== undefined && (first.get(id) === 'render' || (target.parent !== null && drawingOwners.has(target.parent)));
  };
  return units.map((unit): UnitRole => {
    const role = first.get(unit.id)!;
    if (!unit.facts.callable) return { id: unit.id, role, parts: [role], why: `${unit.kind}${role === 'data' ? ' holding constant values' : ''}` };
    const writes = (outgoing.get(unit.id) ?? []).filter((e) => e.kind === 'write').map((e) => e.to);
    const gameWrites = writes.filter((id) => !renderState(id));
    if ((role === 'render' || role === 'ui') && gameWrites.length > 0) {
      return { id: unit.id, role: 'mixed', parts: [role, 'tick'], why: `draws or shows and also changes ${gameWrites.slice(0, 3).join(', ')}` };
    }
    if (role === 'mixed') return { id: unit.id, role, parts: ['render', 'ui'], why: 'draws and drives the interface' };
    if (role === 'render' || role === 'ui') return { id: unit.id, role, parts: [role], why: role === 'render' ? 'uses the renderer' : 'uses the page' };
    if (writes.length > 0) return { id: unit.id, role: 'tick', parts: ['tick'], why: `changes ${writes.slice(0, 3).join(', ')}` };
    return { id: unit.id, role: 'compute', parts: ['compute'], why: 'reads and computes, changes nothing' };
  });
}
