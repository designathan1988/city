// What the feature uses of src/ui/builder/catalog.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/builder/catalog.ts:3-3 type BuilderCategoryId
/** The Builder's tool catalogue: seven tabs, each showing all it offers at once, in short sections. */

export type BuilderCategoryId = 'models' | 'draw' | 'mass' | 'facade' | 'parts' | 'roof' | 'paint' | 'interior';

// src/ui/builder/catalog.ts:175-188 interface BuilderField
/** One precise, editable number of the current selection, for the inspector. */
export interface BuilderField {
  readonly id: string;
  readonly labelKey: string;
  /** In metres, degrees or a count, depending on the unit. */
  readonly value: number;
  readonly unit?: 'm' | 'deg' | 'count' | 'percent';
  readonly min?: number;
  readonly max?: number;
  readonly step?: number;
  /** A read-only row (a name, a material, a computed figure). */
  readonly text?: string;
  /** A choice among named values instead of a number. */
  readonly options?: readonly { readonly value: number; readonly labelKey: string }[];
}

// src/ui/builder/catalog.ts:191-198 interface BuilderSelectionInfo
/** What the inspector shows for the current selection. */
export interface BuilderSelectionInfo {
  readonly titleKey: string;
  /** e.g. "Volume 02" */
  readonly name: string;
  readonly fields: readonly BuilderField[];
  /** The material line under the fields, when the selection has one. */
  readonly material?: string | undefined;
}
