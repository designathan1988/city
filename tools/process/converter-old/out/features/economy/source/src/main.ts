// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 267 lines
  if (roadPreview) {
    // … 41 lines
    const modes: BuildMode[] = [];
    if (exact && exact.length > 1) {
      // … 1 lines
      for (const st of exact) {
        const p = { x: st.x, y: st.y };
        // … 1 lines
        offsets.push(st.deck - groundUnder(p));
        projected.push(view.toScreen(p, w, h, st.deck));
        groundProjected.push(view.toScreen(p, w, h, st.ground));
        modes.push(st.mode);
      }
    }
    // … 22 lines
    if (modes.length > 1) drawBuildModes(ctx, projected, groundProjected, modes);
    else if (points.length > 1 && offsets.some((offset) => Math.abs(offset) > UNITS_PER_METER * 0.5)) {
      strokeScreen(points, 'rgba(6, 19, 21, 0.52)', casingWidth + 5, [7, 7], groundProjected);
      ctx.beginPath();
      ctx.moveTo(projected[0]!.x, projected[0]!.y);
      for (const p of projected.slice(1)) ctx.lineTo(p.x, p.y);
      for (const p of [...groundProjected].reverse()) ctx.lineTo(p.x, p.y);
      ctx.closePath();
      ctx.fillStyle = previewHeight >= 0 ? 'rgba(101, 229, 195, 0.16)' : 'rgba(244, 184, 103, 0.20)';
      ctx.fill();
    }
    // … 28 lines
    if (modes.includes('tunnel')) drawTunnelStretches(ctx, projected, modes, asphaltWidth);
    // … 5 lines
    if (points.length && !settling) {
      // … 5 lines
      {
        // … 4 lines
        const priced = (cost === null ? `${tens} m` : `${tens} m · ${formatCost(cost)}`) + (built ? ` · ${built}` : '');
        const text = refusal ? `${priced} · ${t(`rule.short.${refusal}`)}` : priced;
        // … 2 lines
        const tw = ctx.measureText(text).width;
        const bx = clamp(end.x + 16, 8, Math.max(8, w - tw - 28)), by = clamp(end.y - 40, 60, h - 40);
        // … 2 lines
        ctx.roundRect(bx, by, tw + 18, 24, 6);
        // … 3 lines
        ctx.fillText(text, bx + 9, by + 12.5);
      // … 1 lines
      }
    // … 51 lines
    }
  }
}

// src/main.ts:3413-3419 slice of src/main.ts#BUILD_FILL (shared): the feature's statements; "…" marks code left out
const BUILD_FILL: Readonly<Record<BuildMode, string | null>> = {
  ground: null,
  embankment: 'rgba(164, 120, 74, 0.62)',
  cutting: 'rgba(132, 128, 120, 0.62)',
  bridge: null,
  tunnel: 'rgba(40, 34, 30, 0.55)',
};

// src/main.ts:3421-3459 slice of src/main.ts#drawBuildModes (shared): the feature's statements; "…" marks code left out
function drawBuildModes(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], ground: readonly Vec2[], modes: readonly BuildMode[]): void {
  // … 1 lines
  while (i < modes.length) {
    const mode = modes[i]!;
    let j = i;
    while (j + 1 < modes.length && modes[j + 1] === mode) j++;
    const a = Math.max(0, i - 1), b = Math.min(modes.length - 1, j + 1);
    const fill = BUILD_FILL[mode];
    if (fill) {
      ctx.beginPath();
      ctx.moveTo(deck[a]!.x, deck[a]!.y);
      for (let k = a + 1; k <= b; k++) ctx.lineTo(deck[k]!.x, deck[k]!.y);
      for (let k = b; k >= a; k--) ctx.lineTo(ground[k]!.x, ground[k]!.y);
      ctx.closePath();
      ctx.fillStyle = fill;
      ctx.fill();
      if (mode === 'cutting') {
        // The held side of the cut: its top line on the ground.
        ctx.strokeStyle = 'rgba(214, 208, 196, 0.9)';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(ground[a]!.x, ground[a]!.y);
        for (let k = a + 1; k <= b; k++) ctx.lineTo(ground[k]!.x, ground[k]!.y);
        ctx.stroke();
      }
    }
    if (mode === 'bridge') {
      ctx.strokeStyle = 'rgba(206, 200, 188, 0.95)';
      ctx.lineWidth = 3;
      ctx.beginPath();
      for (let k = i; k <= j; k += PIER_EVERY) {
        ctx.moveTo(deck[k]!.x, deck[k]!.y);
        ctx.lineTo(ground[k]!.x, ground[k]!.y);
      }
      ctx.stroke();
    }
    i = j + 1;
  }
}

// src/main.ts:3461-3475 slice of src/main.ts#drawTunnelStretches (shared): the feature's statements; "…" marks code left out
function drawTunnelStretches(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], modes: readonly BuildMode[], width: number): void {
  // … 5 lines
  for (let k = 0; k < modes.length; k++) {
    if (modes[k] !== 'tunnel') continue;
    const start = modes[k - 1] !== 'tunnel';
    if (start) ctx.moveTo(deck[k]!.x, deck[k]!.y);
    else ctx.lineTo(deck[k]!.x, deck[k]!.y);
  }
// … 2 lines
}

// src/main.ts:3478-3486 slice of src/main.ts#buildSummary (shared): the feature's statements; "…" marks code left out
function buildSummary(stations: Parameters<typeof buildRuns>[0]): string {
  const totals = new Map<BuildMode, number>();
  for (const run of buildRuns(stations)) totals.set(run.mode, (totals.get(run.mode) ?? 0) + run.to - run.from);
  return [...totals]
    .filter(([mode, length]) => mode !== 'ground' && length * METERS_PER_UNIT >= 5)
    .sort((x, y) => y[1] - x[1])
    .map(([mode, length]) => `${t(mode === 'cutting' && cutWallsChosen() ? 'buildMode.cuttingWalls' : `buildMode.${mode}`)} ${Math.max(10, Math.round((length * METERS_PER_UNIT) / 10) * 10)} m`)
    .join(' · ');
}

// src/main.ts:3837-3856 slice of src/main.ts#currentStatus (shared): the feature's statements; "…" marks code left out
function currentStatus(): ShellStatus {
  // … 5 lines
  return {
    // … 10 lines
    balance: doc.economy.balance,
  };
}
