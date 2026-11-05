// What the feature uses of src/lab/inspector/entity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/inspector/entity.ts:46-64 function agentsNear
/** The vehicle and the person nearest the line of sight, within their size, in front of what the ray met first (`before`). */
function agentsNear(ctx: LabContext, ray: Ray, before: number): EntityRef[];

// src/lab/inspector/entity.ts:105-111 interface EntityInfo
/** An entity's title, its main fields and its whole record, and where it stands on the map. */
export interface EntityInfo {
  readonly title: string;
  readonly rows: readonly (readonly [string, string | number | boolean | null | undefined])[];
  readonly record: unknown;
  readonly extra?: Readonly<Record<string, unknown>>;
  readonly at: { readonly x: number; readonly y: number } | null;
}

// src/lab/inspector/entity.ts:113-113 function fixed
const fixed = (v: number | undefined, d = 2): string | null => …;

// src/lab/inspector/entity.ts:115-207 function entityInfo
export function entityInfo(ctx: LabContext, ref: EntityRef): EntityInfo;
