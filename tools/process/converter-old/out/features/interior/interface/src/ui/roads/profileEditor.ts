// What the feature uses of src/ui/roads/profileEditor.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/roads/profileEditor.ts:18-31 interface ProfileEditorOptions
/** THE PROFILE EDITOR: the road's cross-section drawn as a street section */
export interface ProfileEditorOptions {
  /** What is being edited ("this road", "new roads"). */
  readonly title: string;
  readonly profile: RoadProfileSpec;
  readonly type: number;
  /** Applies the profile to the road being edited; absent when there is none. */
  readonly apply?: (profile: RoadProfileSpec, type: number) => void;
  /** Makes the profile the one new roads are drawn with. */
  readonly drawWith?: (name: string, profile: RoadProfileSpec, type: number) => void;
  /** Docked above the road tool's drawer (the catalogue's "Customise..."), never over it: compact, and folding to its head */
  readonly docked?: boolean;
  /** Told when a template was saved (the catalogue shows it under "My roads"). */
  readonly onSaved?: () => void;
}

// src/ui/roads/profileEditor.ts:148-148 variable openEditor
let openEditor: { root: HTMLElement; dispose: () => void } | null = null;

// src/ui/roads/profileEditor.ts:151-155 function closeProfileEditor
/** Closes the editor if it is open. */
export function closeProfileEditor(): void;

// src/ui/roads/profileEditor.ts:184-668 function openProfileEditor
export function openProfileEditor(options: ProfileEditorOptions): void;
