// The code of src/render/signs.ts that is the feature's (the rest of the file is shared).

// src/render/signs.ts:20-31 slice of src/render/signs.ts#PLATE (shared): the feature's statements; "…" marks code left out
export const PLATE: Readonly<Record<SignType, { w: number; h: number; z: number }>> = {
  stop: { w: 0.75, h: 0.75, z: 2.2 },
  yield: { w: 0.8, h: 0.7, z: 2.2 },
  speed: { w: 0.6, h: 0.6, z: 2.2 },
  noParking: { w: 0.6, h: 0.6, z: 2.2 },
  noEntry: { w: 0.6, h: 0.6, z: 2.2 },
  pedestrian: { w: 0.7, h: 0.7, z: 2.2 },
  school: { w: 0.7, h: 0.7, z: 2.2 },
  direction: { w: 1.6, h: 0.5, z: 2.4 },
  street: { w: 1.0, h: 0.28, z: 2.6 },
  info: { w: 1.0, h: 0.6, z: 2.2 },
};

// src/render/signs.ts:41-41 slice of src/render/signs.ts#onTopOfPost (shared): the feature's statements; "…" marks code left out
export const onTopOfPost = (type: SignType): boolean => type === 'street';

// src/render/signs.ts:44-48 slice of src/render/signs.ts#plateFaces (shared): the feature's statements; "…" marks code left out
export function plateFaces(type: SignType, x: number, y: number, yaw: number): { x: number; y: number; yaw: number }[] {
  const out = onTopOfPost(type) ? BLADE_HALF : FACE_OUT;
  // … 1 lines
  return type === 'street' ? [face(yaw), face(yaw + Math.PI)] : [face(yaw)];
}

// src/render/signs.ts:62-148 slice of src/render/signs.ts#plateCanvas (shared): the feature's statements; "…" marks code left out
function plateCanvas(type: SignType, text: string): HTMLCanvasElement {
  const plate = PLATE[type];
  const W = 256, H = Math.max(64, Math.round((256 * plate.h) / plate.w));
  // … 1 lines
  c.width = W; c.height = H;
  // … 1 lines
  const cx = W / 2, cy = H / 2, r = Math.min(W, H) / 2 - 4;
  // … 9 lines
  switch (type) {
    case 'stop': {
      g.beginPath();
      for (let i = 0; i < 8; i++) { const a = Math.PI / 8 + (i * Math.PI) / 4; g.lineTo(cx + Math.cos(a) * r, cy + Math.sin(a) * r); }
      g.closePath(); g.fillStyle = '#ffffff'; g.fill();
      g.beginPath();
      for (let i = 0; i < 8; i++) { const a = Math.PI / 8 + (i * Math.PI) / 4; g.lineTo(cx + Math.cos(a) * r * 0.9, cy + Math.sin(a) * r * 0.9); }
      g.closePath(); g.fillStyle = '#c8102e'; g.fill();
      g.fillStyle = '#fff'; fit('PARE', r * 1.5, 72); g.fillText('PARE', cx, cy + 4);
      break;
    }
    case 'yield': {
      const tri = (k: number, fill: string): void => {
        g.beginPath(); g.moveTo(cx - r * k, cy - r * 0.8 * k); g.lineTo(cx + r * k, cy - r * 0.8 * k); g.lineTo(cx, cy + r * 0.95 * k); g.closePath(); g.fillStyle = fill; g.fill();
      };
      tri(1, '#c8102e'); tri(0.72, '#ffffff');
      break;
    }
    case 'speed': {
      circle('#ffffff', '#c8102e');
      g.fillStyle = '#111'; fit(text || '40', r * 1.2, 96); g.fillText(text || '40', cx, cy + 4);
      break;
    }
    case 'noParking': {
      circle('#1f4fa0', '#c8102e');
      g.fillStyle = '#fff'; g.font = `700 ${Math.round(r * 1.1)}px system-ui, sans-serif`; g.fillText('E', cx, cy + 4);
      g.strokeStyle = '#c8102e'; g.lineWidth = r * 0.16; g.beginPath(); g.moveTo(cx - r * 0.6, cy - r * 0.6); g.lineTo(cx + r * 0.6, cy + r * 0.6); g.stroke();
      break;
    }
    case 'noEntry': {
      circle('#c8102e', null);
      g.fillStyle = '#fff'; g.fillRect(cx - r * 0.65, cy - r * 0.16, r * 1.3, r * 0.32);
      break;
    }
    case 'pedestrian':
    case 'school': {
      g.save(); g.translate(cx, cy); g.rotate(Math.PI / 4);
      g.fillStyle = '#111'; g.fillRect(-r * 0.7, -r * 0.7, r * 1.4, r * 1.4);
      g.fillStyle = '#f2c500'; g.fillRect(-r * 0.66, -r * 0.66, r * 1.32, r * 1.32);
      g.restore();
      g.fillStyle = '#111';
      if (type === 'school') { fit('ESCOLA', r * 1.1, 40); g.fillText('ESCOLA', cx, cy + 2); }
      else {
        // A walking figure.
        g.beginPath(); g.arc(cx, cy - r * 0.35, r * 0.11, 0, Math.PI * 2); g.fill();
        g.lineWidth = r * 0.09; g.strokeStyle = '#111'; g.lineCap = 'round';
        g.beginPath(); g.moveTo(cx, cy - r * 0.2); g.lineTo(cx - r * 0.05, cy + r * 0.12);
        g.lineTo(cx - r * 0.22, cy + r * 0.4); g.moveTo(cx - r * 0.05, cy + r * 0.12); g.lineTo(cx + r * 0.15, cy + r * 0.4);
        g.moveTo(cx - r * 0.2, cy - r * 0.02); g.lineTo(cx, cy - r * 0.15); g.lineTo(cx + r * 0.2, cy); g.stroke();
      }
      break;
    }
    case 'direction':
    case 'street':
    case 'info': {
      const bg = type === 'direction' ? '#1b6e3a' : '#1f4fa0';
      g.fillStyle = '#ffffff'; g.fillRect(0, 0, W, H);
      g.fillStyle = bg; g.fillRect(4, 4, W - 8, H - 8);
      g.fillStyle = '#fff';
      const words = text || (type === 'street' ? 'Rua' : type === 'direction' ? 'Centro' : 'Informação');
      if (type === 'direction') {
        fit(words, W - 70, Math.round(H * 0.5)); g.fillText(words, (W - 40) / 2, cy + 2);
        g.beginPath(); g.moveTo(W - 14, cy); g.lineTo(W - 44, cy - H * 0.3); g.lineTo(W - 44, cy + H * 0.3); g.closePath(); g.fill();
      } else {
        fit(words, W - 20, Math.round(H * 0.55)); g.fillText(words, cx, cy + 2);
      }
      break;
    }
  }
// … 1 lines
}

// src/render/signs.ts:208-228 slice of src/render/signs.ts#Sheet (shared): the feature's statements; "…" marks code left out
export interface Sheet {
  // … 12 lines
  readonly plates: Map<number, { readonly type: SignType; readonly text: string }>;
// … 6 lines
}

// src/render/signs.ts:281-286 slice of src/render/signs.ts#SignSheets.take (shared): the feature's statements; "…" marks code left out
  take(type: SignType, text: string): Placed {
    const key = `${type}|${text}`;
    const placed = this.plateCell(type, text);
    this.counts.set(key, (this.counts.get(key) ?? 0) + 1);
    return placed;
  }

// src/render/signs.ts:289-309 slice of src/render/signs.ts#SignSheets.drop (shared): the feature's statements; "…" marks code left out
  drop(type: SignType, text: string): void {
    const key = `${type}|${text}`;
    const left = (this.counts.get(key) ?? 0) - 1;
    if (left > 0) { this.counts.set(key, left); return; }
    this.counts.delete(key);
    const kept = this.placed.get(key);
    if (!kept) return;
    this.placed.delete(key);
    this.pending.delete(key);
    // … 1 lines
    kept.sheet.plates.delete(kept.cell);
    kept.sheet.free.push(kept.cell);
    kept.sheet.blank.push(kept.cell);
    kept.sheet.used--;
    if (kept.sheet.used > 0) return;
    const at = this.sheets.indexOf(kept.sheet);
    if (at >= 0) this.sheets.splice(at, 1);
    this.watch?.gone?.(kept.sheet);
    kept.sheet.texture.dispose();
    kept.sheet.material.dispose();
  }

// src/render/signs.ts:317-333 slice of src/render/signs.ts#SignSheets.plateCell (shared): the feature's statements; "…" marks code left out
  plateCell(type: SignType, text: string): Placed {
    const key = `${type}|${text}`;
    let known = this.placed.get(key);
    if (!known) {
      const picture = plateCanvas(type, text);
      const sheet = this.sheetWithRoom(CELL_HEIGHTS.find((c) => c >= picture.height) ?? CELL);
      const cell = sheet.free.shift()!;
      sheet.plates.set(cell, { type, text });
      sheet.dirty = true;
      sheet.used++;
      this.pending.set(key, picture);
      known = { sheet, cell, h: Math.min(picture.height, sheet.cellHeight), build: this.builds };
      this.placed.set(key, known);
    }
    known.build = this.builds;
    return known;
  }
