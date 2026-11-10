// The code of src/lab/tabs.ts that is the feature's (the rest of the file is shared).

// src/lab/tabs.ts:12-32 slice of src/lab/tabs.ts#createLabTabs (shared): the feature's statements; "…" marks code left out
export function createLabTabs(ctx: LabContext): LabTab[] {
  const switches: LabTab = {
    // … 2 lines
    mount: (host) => ctx.effects.panel.embed(host),
    unmount: () => ctx.effects.panel.release(),
  };
// … 13 lines
}
