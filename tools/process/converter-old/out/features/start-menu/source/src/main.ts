// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 71 lines
  mountStart(document.querySelector<HTMLElement>('.v2') ?? document.body, { saved: hadSavedMap, newMap, openMap: () => void openMap(), graphicsOptions: () => graphics.openPanel(), about: openAbout });
// … 14 lines
}
