// What the feature uses of src/peopleLab/picker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/picker.ts:8-8 interface PickerEntry
/** A GRID OF CHOICES THAT NEVER SCROLLS, as CharacterStudio and Create-a-Sim lay their catalogues out: the pictures fill the height the panel has left,
 *  a page at a time (arrows, the wheel over the grid), the current choice's page opened first, and a search box once there is more than a page.
 *  Only the page shown asks for its pictures. */

export interface PickerEntry { readonly id: string; readonly label: string }

// src/peopleLab/picker.ts:11-19 class PickerMemory
/** Where each catalogue was left (page, search), kept across the panel's redraws. */
export class PickerMemory { … }

// src/peopleLab/picker.ts:27-90 function picker
/** `picture(entry)` makes one tile's picture; `all` draws every entry (baking the pictures). */
export function picker(memory: PickerMemory, key: string, entries: readonly PickerEntry[], current: string | null | undefined,
  picture: (e: PickerEntry) => HTMLElement, pick: (id: string) => void, all = false): HTMLElement;
