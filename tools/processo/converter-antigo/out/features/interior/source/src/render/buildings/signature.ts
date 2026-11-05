// The code of src/render/buildings/signature.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/signature.ts:199-358 function makeMaterials: the feature's own
function makeMaterials(): Record<Mat, Material> {
  // The materials' noise from one seeded stream, made in this fixed order: the same pictures in every run.
  const rng = new Rng(seedOf('signature-materials'));
  // Stone base: large grey slabs, thin joints (a 1.2 m x 0.6 m course).
  const stone = canvasTexture(256, (g, s) => {
    g.fillStyle = '#77736e'; g.fillRect(0, 0, s, s);
    for (let i = 0; i < 400; i++) { g.fillStyle = `rgba(${90 + rng.float() * 40},${88 + rng.float() * 36},${84 + rng.float() * 30},.35)`; g.fillRect(rng.float() * s, rng.float() * s, 3, 3); }
    g.strokeStyle = '#4f4c49'; g.lineWidth = 3;
    for (let r = 0; r < 4; r++) {
      g.beginPath(); g.moveTo(0, r * s / 4); g.lineTo(s, r * s / 4); g.stroke();
      const off = r % 2 ? s / 4 : 0;
      for (let c = 0; c < 3; c++) { g.beginPath(); g.moveTo(off + c * s / 2, r * s / 4); g.lineTo(off + c * s / 2, (r + 1) * s / 4); g.stroke(); }
    }
  }, 1 / 2.4, 1 / 2.4);
  // Render: warm light grey, a faint trowel texture.
  const render = canvasTexture(128, (g, s) => {
    g.fillStyle = '#d9d4cb'; g.fillRect(0, 0, s, s);
    for (let i = 0; i < 900; i++) { const v = 200 + rng.float() * 30; g.fillStyle = `rgba(${v},${v - 4},${v - 10},.25)`; g.fillRect(rng.float() * s, rng.float() * s, 2, 2); }
  }, 1 / 3, 1 / 3);
  // Timber cladding: horizontal boards of a brown-orange wood, 0.15 m each.
  const wood = canvasTexture(256, (g, s) => {
    for (let r = 0; r < 16; r++) {
      const v = rng.float() * 18;
      g.fillStyle = `rgb(${150 + v},${86 + v * 0.6},${52 + v * 0.4})`;
      g.fillRect(0, r * s / 16, s, s / 16);
      g.fillStyle = 'rgba(60,30,15,.55)'; g.fillRect(0, r * s / 16, s, 2);
      for (let k = 0; k < 6; k++) { g.fillStyle = 'rgba(90,45,20,.18)'; g.fillRect(rng.float() * s, r * s / 16 + 3 + rng.float() * 8, 30 + rng.float() * 60, 1); }
    }
  }, 1 / 2.4, 1 / 2.4);
  // Vertical timber slats with dark gaps.
  const slats = canvasTexture(128, (g, s) => {
    g.fillStyle = '#2a1a10'; g.fillRect(0, 0, s, s);
    for (let c = 0; c < 8; c++) {
      const v = rng.float() * 16;
      g.fillStyle = `rgb(${140 + v},${82 + v * 0.6},${48 + v * 0.4})`;
      g.fillRect(c * s / 8 + 2, 0, s / 8 - 5, s);
    }
  }, 1 / 0.8, 1);
  // Railing: black frame, top and bottom rails, thin bars 0.11 m apart (alpha).
  const railing = canvasTexture(128, (g, s) => {
    g.clearRect(0, 0, s, s);
    g.fillStyle = '#ffffff';
    g.fillRect(0, 0, s, 9); g.fillRect(0, s - 7, s, 7); g.fillRect(0, s * 0.16, s, 3);
    for (let c = 0; c < 9; c++) g.fillRect(c * s / 9, 0, 3, s);
  }, 1, 1 / 1.05);
  const std = (p: ConstructorParameters<typeof MeshStandardMaterial>[0]): MeshStandardMaterial => new MeshStandardMaterial(p);
  // Surfaces that read as stone, concrete, roofing, tiles: never flat colour (flat light planes blow out to white).
  const ashlar = (base: string, joint: string): CanvasTexture => canvasTexture(256, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    for (let i = 0; i < 700; i++) { g.fillStyle = `rgba(${90 + rng.float() * 80},${85 + rng.float() * 75},${75 + rng.float() * 70},.12)`; g.fillRect(rng.float() * s, rng.float() * s, 3, 3); }
    g.strokeStyle = joint; g.lineWidth = 2;
    for (let r = 0; r < 8; r++) { g.beginPath(); g.moveTo(0, (r * s) / 8); g.lineTo(s, (r * s) / 8); g.stroke(); for (let c = 0; c < 4; c++) { const x = (c + (r % 2) * 0.5) * (s / 4); g.beginPath(); g.moveTo(x, (r * s) / 8); g.lineTo(x, ((r + 1) * s) / 8); g.stroke(); } }
  }, 1 / 2.4, 1 / 2.4);
  const speckle = (base: string, dot: number, lines = 0): CanvasTexture => canvasTexture(256, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    for (let i = 0; i < 2500; i++) { const v = rng.float() > 0.5 ? 255 : 0; g.fillStyle = `rgba(${v},${v},${v},${dot})`; g.fillRect(rng.float() * s, rng.float() * s, 2, 2); }
    g.fillStyle = 'rgba(0,0,0,.18)'; for (let k = 1; k <= lines; k++) { g.fillRect(0, (k * s) / (lines + 1), s, 2); g.fillRect((k * s) / (lines + 1), 0, 2, s); }
  }, 1 / 4, 1 / 4);
  // Rusticated stone: 0.6 m courses with deep shadowed joints, long blocks.
  const rusticated = (base: string, joint: string): CanvasTexture => canvasTexture(256, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    for (let i = 0; i < 600; i++) { g.fillStyle = `rgba(${120 + rng.float() * 60},${110 + rng.float() * 50},${90 + rng.float() * 40},.12)`; g.fillRect(rng.float() * s, rng.float() * s, 3, 3); }
    for (let r = 0; r < 4; r++) {
      const y = (r * s) / 4;
      g.fillStyle = joint; g.fillRect(0, y, s, 7);
      g.fillStyle = 'rgba(255,255,255,.25)'; g.fillRect(0, y + 7, s, 2);
      for (let c = 0; c < 2; c++) { g.fillStyle = joint; g.fillRect((c + (r % 2) * 0.5) * (s / 2), y, 5, s / 4); }
    }
  }, 1 / 2.4, 1 / 2.4);
  // An Art Deco gilded panel: a stepped sunburst and zigzags.
  const decoPanel = (base: string, gold: string): CanvasTexture => canvasTexture(128, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    g.strokeStyle = gold; g.lineWidth = 4;
    for (let k = 0; k < 4; k++) g.strokeRect(8 + k * 10, 8 + k * 10, s - 16 - k * 20, s - 16 - k * 20);
    g.fillStyle = gold;
    for (let k = 0; k < 7; k++) { g.beginPath(); g.moveTo(s / 2, s * 0.62); g.lineTo(s * (0.2 + k * 0.1), s * 0.25); g.lineTo(s * (0.24 + k * 0.1), s * 0.25); g.fill(); }
  }, 1 / 1.6, 1 / 1.1);
  // A standing-seam copper roof: vertical seams 0.5 m apart.
  const seams = (base: string, seam: string): CanvasTexture => canvasTexture(64, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    g.fillStyle = seam; for (let k = 0; k < 4; k++) g.fillRect((k * s) / 4, 0, 3, s);
    g.fillStyle = 'rgba(255,255,255,.08)'; for (let k = 0; k < 4; k++) g.fillRect((k * s) / 4 + 3, 0, 2, s);
  }, 1 / 2, 1 / 2);
  // Art Deco spandrel: gilded chevrons on dark bronze.
  const chevrons = (base: string, gold: string): CanvasTexture => canvasTexture(128, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    g.strokeStyle = gold; g.lineWidth = 5;
    for (let k = 0; k < 3; k++) { const y = s * (0.25 + k * 0.25); g.beginPath(); g.moveTo(0, y + s * 0.12); g.lineTo(s / 2, y - s * 0.08); g.lineTo(s, y + s * 0.12); g.stroke(); }
    g.fillStyle = gold; g.fillRect(0, 0, s, 5); g.fillRect(0, s - 5, s, 5);
  }, 1 / 1.5, 1 / 1);
  const courses = (base: string, line: string, rows: number): CanvasTexture => canvasTexture(128, (g, s) => {
    g.fillStyle = base; g.fillRect(0, 0, s, s);
    for (let r = 0; r < rows; r++) { g.fillStyle = line; g.fillRect(0, (r * s) / rows, s, 2); for (let c = 0; c < 6; c++) g.fillRect((c + (r % 2) * 0.5) * (s / 6), (r * s) / rows, 1, s / rows); g.fillStyle = `rgba(0,0,0,${rng.float() * 0.08})`; g.fillRect(0, (r * s) / rows + 2, s, s / rows - 2); }
  }, 1 / 1.5, 1 / 1.5);
  return {
    stone: std({ map: stone, roughness: 0.85 }),
    render: std({ map: render, roughness: 0.92 }),
    trim: std({ color: 0xe6dfd3, roughness: 0.85 }),
    wood: std({ map: wood, roughness: 0.7 }),
    slats: std({ map: slats, roughness: 0.75 }),
    slab: std({ color: 0xece8e0, roughness: 0.8 }),
    frame: std({ color: 0x26282b, roughness: 0.45, metalness: 0.5 }),
    // Daylight glass: dark, reflective, a faint warmth from the rooms behind.
    glass: std({ color: 0x3a4550, emissive: 0x6b5236, emissiveIntensity: 0.25, roughness: 0.08, metalness: 0.7 }),
    railing: std({ color: 0x1c1d1f, alphaMap: railing, alphaTest: 0.5, transparent: false, side: DoubleSide, roughness: 0.5, metalness: 0.4 }),
    pot: std({ color: 0x4a4d51, roughness: 0.75 }),
    leaf: std({ color: 0x4d7a32, roughness: 0.9, flatShading: true }),
    leafDark: std({ color: 0x35612a, roughness: 0.9, flatShading: true }),
    metal: std({ color: 0x3b3e42, roughness: 0.45, metalness: 0.6 }),
    plant: std({ color: 0xa9a59e, roughness: 0.7 }),
    sconce: std({ color: 0xfff1d8, emissive: 0xffd49a, emissiveIntensity: 1.2 }),
    orange: std({ color: 0xe07b2c, roughness: 0.6 }),
    woodDark: std({ color: 0x5a3b24, roughness: 0.8 }),
    door: std({ map: wood, color: 0x8a5a36, roughness: 0.6 }),
    slate: std({ map: courses('#454c56', '#2c3138', 8), roughness: 0.7 }),
    roofing: std({ map: speckle('#6b6d70', 0.1, 3), roughness: 0.95 }),
    patina: std({ color: 0x6f9a8a, roughness: 0.5, metalness: 0.4 }),
    redPaint: std({ color: 0xb3262a, roughness: 0.55 }),
    brickRed: std({ color: 0xa04a34, roughness: 0.9 }),
    ivory: std({ map: ashlar('#d3cbbb', 'rgba(120,108,90,.45)'), roughness: 0.8 }),
    asphalt: std({ color: 0x3c3f43, roughness: 0.95 }),
    concreteLight: std({ map: speckle('#a9a6a0', 0.08, 1), roughness: 0.9 }),
    gravel: std({ map: speckle('#a3967f', 0.16), roughness: 1 }),
    lawn: std({ map: speckle('#4f7c33', 0.12), roughness: 1 }),
    blackSteel: std({ color: 0x1f2124, roughness: 0.5, metalness: 0.5 }),
    blueSteel: std({ color: 0x2f5fa8, roughness: 0.5, metalness: 0.3 }),
    yellowPaint: std({ color: 0xe8c22a, roughness: 0.6 }),
    gold: std({ color: 0xc9a54a, roughness: 0.35, metalness: 0.8 }),
    copper: std({ color: 0xb06d3c, roughness: 0.35, metalness: 0.75 }),
    roofTerracotta: std({ map: courses('#a6503a', '#6e2f22', 10), roughness: 0.8 }),
    glassRail: std({ color: 0xcfe3ec, transparent: true, opacity: 0.38, roughness: 0.05, metalness: 0.2, depthWrite: false }),
    white: std({ map: speckle('#dddcd6', 0.05, 1), roughness: 0.8 }),
    stoneLight: std({ map: ashlar('#c4bdaf', 'rgba(110,100,85,.5)'), roughness: 0.8 }),
    beige: std({ color: 0xd9c49c, roughness: 0.85 }),
    brickPier: std({ color: 0x8f4433, roughness: 0.9 }),
    darkPanel: std({ color: 0x2f3439, roughness: 0.6, metalness: 0.3 }),
    lightGrid: std({ color: 0xc4c9cd, roughness: 0.6 }),
    podiumGlass: std({ color: 0x40505c, emissive: 0x5a4630, emissiveIntensity: 0.3, roughness: 0.08, metalness: 0.7 }),
    // The landmarks' (`landmarks.ts`): warm limestone, rusticated courses, a copper roof gone green.
    limestone: std({ map: ashlar('#cbbb9c', 'rgba(110,92,64,.35)'), roughness: 0.82 }),
    rustic: std({ map: rusticated('#c8b898', 'rgba(70,58,40,.75)'), roughness: 0.85 }),
    copperRoof: std({ map: seams('#4a645a', '#2c3d36'), roughness: 0.65, metalness: 0.1 }),
    awning: std({ color: 0x2c5a44, roughness: 0.8, side: DoubleSide }),
    decoStone: std({ map: ashlar('#d8c6a0', 'rgba(120,100,70,.3)'), roughness: 0.78 }),
    decoSpandrel: std({ map: chevrons('#5a4527', '#d8b157'), roughness: 0.4, metalness: 0.6 }),
    glassDeco: std({ color: 0x33403f, emissive: 0x6b5236, emissiveIntensity: 0.32, roughness: 0.08, metalness: 0.7 }),
    glassBlue: std({ color: 0x4a6f93, emissive: 0x4a4a3a, emissiveIntensity: 0.18, roughness: 0.05, metalness: 0.75 }),
    lightStrip: std({ color: 0xfff0d2, emissive: 0xffd49a, emissiveIntensity: 1.3 }),
    terracotta: std({ map: courses('#b0603a', '#7d3c22', 14), roughness: 0.75 }),
    alu: std({ color: 0xb9c0c6, roughness: 0.35, metalness: 0.6 }),
    // The landmarks' glass: glass, reflecting the sky and the street (the scene's environment), no picture behind it.
    glassLit: std({ color: 0x8a9cab, roughness: 0.06, metalness: 0.75 }),
    glassBlueLit: std({ color: 0x5b8cc0, roughness: 0.05, metalness: 0.7 }),
    frameWhite: std({ color: 0xeee7da, roughness: 0.55 }),
    frameBronze: std({ color: 0x5a4632, roughness: 0.4, metalness: 0.5 }),
    soil: std({ color: 0x4a3a2a, roughness: 1 }),
    decoGold: std({ map: decoPanel('#2a2420', '#d7ad55'), roughness: 0.35, metalness: 0.6 }),
    paving: std({ map: courses('#cfc6b4', 'rgba(120,108,90,.55)', 5), roughness: 0.9 }),
  };
}

// src/render/buildings/signature.ts:1755-1799 function buildSignature: the feature's own
export function buildSignature(b: Building, floor: number): Group | null {
  const kind = signatureKind(b);
  const design = kind ? DESIGNS[kind] : undefined;
  if (!design) return null;
  materials ??= makeMaterials();
  const U = m(1);
  const closed = b.volumes.filter((v) => !v.open);
  if (!closed.length) return null;
  const base = closed.filter((v) => v.base === 0).sort((p, q) => q.w * q.d - p.w * p.d)[0] ?? closed[0]!;
  const levels = Math.max(...closed.map((v) => v.base + v.storeys.length));
  const parts = new Parts();
  design(parts, base.w / U, base.d / U, b.groundHeight / U, b.storeyHeight / U, Math.max(1, levels - 1));
  const group = new Group();
  group.name = `signature-${b.id}`;
  // The faces another part covers, or that lie on the ground facing down, taken out before the parts are merged (`hiddenFaces.ts`): a step inside
  // the next, a block's foot on the plinth, a cap's underside on its post.
  // Only parts of an opaque material hide or are hidden: a railing's alpha-cut screen, a decal, a pane seen through cover nothing.
  const solid = (mat: Mat): boolean => {
    const look = materials![mat] ?? facades.get(mat);
    return look !== undefined && !look.transparent && look.alphaTest === 0;
  };
  dropHiddenFaces([...parts.byMat].filter(([mat]) => solid(mat)).flatMap(([, list]) => list), true);
  for (const [mat, list] of parts.byMat) {
    for (const g of list) g.clearGroups();
    const merged = mergeGeometries(list, false);
    for (const g of list) g.dispose();
    if (!merged) continue;
    merged.scale(U, U, U);
    merged.computeBoundingSphere();
    const mesh = new Mesh(merged, materials[mat] ?? facades.get(mat)!);
    mesh.castShadow = mat !== 'glass' && mat !== 'sconce';
    mesh.receiveShadow = true;
    mesh.name = `signature-${mat}`;
    group.add(mesh);
  }
  // The body's frame: the base block's corner, in the building's frame, in the world.
  group.position.set(b.x, floor, -b.y);
  group.rotation.y = b.rotation;
  const inner = new Group();
  inner.position.set(base.x, 0, -base.y);
  inner.add(...group.children);
  group.add(inner);
  group.updateMatrixWorld(true);
  return group;
}

// src/render/buildings/signature.ts:1801-1803 function disposeSignature: the feature's own
export function disposeSignature(group: Group): void {
  group.traverse((o) => { if ((o as Mesh).isMesh) (o as Mesh).geometry.dispose(); });
}
