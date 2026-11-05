// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:268-276 slice of src/ui/v2/shell.ts#ShellTransitTool (shared): the feature's statements; "…" marks code left out
interface ShellTransitTool {
  // … 4 lines
  lines(): readonly TransitLine[];
// … 2 lines
}

// src/ui/v2/shell.ts:1602-1637 slice of src/ui/v2/shell.ts#mountShell.renderTransit (shared): the feature's statements; "…" marks code left out
  function renderTransit(): void {
    // … 21 lines
    for (const line of lines) {
      const row = transitLineRow(line);
      const less = button('v2-icon', t('transit.fewer'), () => { tool2.setVehicles(line.id, line.vehicles - 1); render(); }, '<svg viewBox="0 0 24 24" width="14" height="14" fill="none" stroke="currentColor" stroke-width="2.4"><path d="M6 12h12"/></svg>');
      // … 1 lines
      count.textContent = String(line.vehicles);
      // … 1 lines
      const more = button('v2-icon', t('transit.more'), () => { tool2.setVehicles(line.id, line.vehicles + 1); render(); }, '<svg viewBox="0 0 24 24" width="14" height="14" fill="none" stroke="currentColor" stroke-width="2.4"><path d="M6 12h12M12 6v12"/></svg>');
      const drop = button('v2-icon danger', t('transit.remove'), () => { tool2.removeLine(line.id); render(); }, svg('demolish', 14));
      row.append(less, count, more, drop);
      items.appendChild(row);
    }
  // … 2 lines
  }
