// The code of src/ui/toolChoices.ts that is the feature's (the rest of the file is shared).

// src/ui/toolChoices.ts:40-40 slice of src/ui/toolChoices.ts#lamps (shared): the feature's statements; "…" marks code left out
let lamps: PoleLampMode = stored(LAMP_KEY, POLE_LAMP_MODES, 'alternate');

// src/ui/toolChoices.ts:42-44 slice of src/ui/toolChoices.ts#poleLampMode (shared): the feature's statements; "…" marks code left out
export function poleLampMode(): PoleLampMode {
// … 1 lines
}

// src/ui/toolChoices.ts:46-49 slice of src/ui/toolChoices.ts#setPoleLampMode (shared): the feature's statements; "…" marks code left out
export function setPoleLampMode(next: PoleLampMode): void {
  lamps = next;
  keep(LAMP_KEY, next);
}
