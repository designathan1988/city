// The code of src/ui/roads/catalogPanel.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/catalogPanel.ts:65-153 slice of src/ui/roads/catalogPanel.ts#catalogStrip (shared): the feature's statements; "…" marks code left out
export function catalogStrip(host: CatalogHost): HTMLElement {
  // … 49 lines
  for (const road of shown) {
    // … 19 lines
    main.querySelector('.rc-meta')!.textContent = t('catalog.meta', {
      // … 1 lines
      price: formatMoney(Math.round(pricePerUnit(rt) * UNITS_PER_METER)),
    });
  // … 8 lines
  }
// … 5 lines
}
