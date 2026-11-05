// The code of src/render/buildings/parts.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/parts.ts:25-25 variable SIZE: the feature's own
const SIZE = 224;

// src/render/buildings/parts.ts:27-27 variable PER_FRAME: the feature's own
const PER_FRAME = 3;

// src/render/buildings/parts.ts:29-29 variable FRAME_SHARE_MS: the feature's own
const FRAME_SHARE_MS = 6;

// src/render/buildings/parts.ts:31-31 variable IDLE_MS: the feature's own
const IDLE_MS = 4000;

// src/render/buildings/parts.ts:34-38 variable WALL_PARTS: the feature's own
const WALL_PARTS: Readonly<Record<string, BayComponent>> = {
  ...Object.fromEntries(BAY_COMPONENTS.map((component) => [component, component])),
  pillarBay: 'pillar',
  wallBay: 'wall',
};

// src/render/buildings/parts.ts:40-42 variable ROOF_PARTS: the feature's own
const ROOF_PARTS: Readonly<Record<string, RoofKind>> = Object.fromEntries(
  ROOF_KINDS.map((kind) => [kind === 'flat' ? 'roofFlat' : `roof${kind[0]?.toUpperCase()}${kind.slice(1)}`, kind]),
);

// src/render/buildings/parts.ts:44-44 variable ELEMENT_PARTS: the feature's own
const ELEMENT_PARTS = new Set<string>(Object.keys(ELEMENT_DEFAULTS));

// src/render/buildings/parts.ts:47-53 variable RUN_PARTS: the feature's own
const RUN_PARTS: Readonly<Record<string, ElementKind>> = {
  wallRun: 'wall',
  fenceRun: 'fence',
  pavementRun: 'pavement',
  stairRun: 'stair',
  railing: 'railing',
};

// src/render/buildings/parts.ts:55-55 variable MODEL_KEYS: the feature's own
const MODEL_KEYS = new Set(BLUEPRINTS.map((blueprint) => blueprint.key));

// src/render/buildings/parts.ts:57-61 function blank: the feature's own
function blank(): Building | null {
  const blueprint = blueprintByKey('block');
  if (!blueprint) return null;
  return { ...instantiate(blueprint.body, { x: 0, y: 0 }, 0), id: -1 } as Building;
}

// src/render/buildings/parts.ts:64-120 function partSample: the feature's own
function partSample(id: string): Building | null {
  // A lot of the lot lab, once read (`ThumbnailStudio.request` reads it first).
  if (id.startsWith('lot:')) {
    const lot = loadedLot(id.slice(4));
    return lot ? ({ ...instantiate(lot.body as BlueprintBody, { x: 0, y: 0 }, Math.PI / 4), id: -1 } as Building) : null;
  }
  if (id.startsWith('city:')) {
    const model = cityBuilding(id.slice(5));
    return model ? ({ ...instantiate(model.body, { x: 0, y: 0 }, Math.PI / 4), id: -1 } as Building) : null;
  }
  if (MODEL_KEYS.has(id)) {
    const blueprint = blueprintByKey(id);
    if (!blueprint) return null;
    return { ...instantiate(blueprint.body, { x: 0, y: 0 }, Math.PI / 4), id: -1 } as Building;
  }

  const sample = blank();
  const volume = sample?.volumes[0];
  if (!sample || !volume) return null;
  sample.elements = [];

  const component = WALL_PARTS[id];
  if (component) {
    // One storey, one wall, filled with the part: the picture is the part.
    if (!volume.storeys[0]) return null;
    volume.storeys = [{ facade: { fill: component } }];
    return sample;
  }

  // A composition is photographed on the wall it composes.
  if ((FACADE_PATTERNS as readonly string[]).includes(id)) {
    // Applied as the panel applies it - shopfronts, glazing, piers - or every pattern photographed as the same plain block.
    applyFacadePattern(sample, { scope: 'building' }, id as FacadePattern);
    return sample;
  }

  const roof = ROOF_PARTS[id];
  if (roof) {
    volume.roof = roof;
    return sample;
  }
  if (id === 'roofShape' || id === 'roofs') {
    volume.roof = 'gable';
    return sample;
  }

  const kind = RUN_PARTS[id] ?? (ELEMENT_PARTS.has(id) ? (id as ElementKind) : null);
  if (kind) {
    // No walls at all: the element alone, at the size it is placed at.
    sample.volumes = [];
    sample.cores = [];
    sample.elements = [{ ...elementAt(sample, kind, { x: 0, y: 0 }, 2), id: 1 }];
    return sample;
  }

  return null;
}

// src/render/buildings/parts.ts:122-131 interface ThumbnailStudio: the feature's own
export interface ThumbnailStudio {
  /** Photographs the ids that are worth a picture, a few per frame, calling back with each batch. */
  request(ids: readonly string[], onBatch: (images: ReadonlyMap<string, string>) => void): void;
  /** Releases the second GPU context at once. */
  dispose(): void;
  /** While held (the Builder in use) the studio keeps its context between galleries */
  hold(on: boolean): void;
  /** How long the studio holds its context after the last picture. */
  IDLE_MS?: number;
}

// src/render/buildings/parts.ts:133-133 variable __BUILDING_KIT_HASH__: the feature's own
declare const __BUILDING_KIT_HASH__: string | undefined;

// src/render/buildings/parts.ts:135-135 variable KIT_HASH: the feature's own
const KIT_HASH = typeof __BUILDING_KIT_HASH__ !== 'undefined' ? __BUILDING_KIT_HASH__ : null;

// src/render/buildings/parts.ts:137-322 function createThumbnailStudio: the feature's own
export function createThumbnailStudio(gl: WebGLRenderer): ThumbnailStudio {
  const done = new Set<string>();
  const queued = new Set<string>();
  const waiting: { id: string; onBatch: (images: ReadonlyMap<string, string>) => void }[] = [];
  let frame: number | null = null;
  let held = false;
  /** The pictures taken in earlier sessions (`derivedCache.ts`), read once */
  let kept: Promise<Map<string, Blob>> | null = null;
  const keptPictures = (): Promise<Map<string, Blob>> => {
    if (!kept) {
      if (KIT_HASH) forgetOtherDerived('thumb', KIT_HASH);
      kept = KIT_HASH ? readDerivedAll<Blob>(`thumb:${KIT_HASH}:`) : Promise.resolve(new Map());
    }
    return kept;
  };

  let idle: ReturnType<typeof setTimeout> | null = null;
  let canvas: HTMLCanvasElement | null = null;
  let renderer: WebGLRenderer | null = null;
  let kit: BuildingKit | null = null;
  let scene: Scene | null = null;
  let camera: PerspectiveCamera | null = null;

  const setUp = (): boolean => {
    if (renderer) return true;
    try {
      canvas = document.createElement('canvas');
      canvas.width = SIZE;
      canvas.height = SIZE;
      // The depth convention of the game's renderer (`depthConvention.ts`): the kit's offsets are written for it.
      renderer = new WebGLRenderer({ canvas, alpha: true, antialias: true, preserveDrawingBuffer: true, reversedDepthBuffer: true });
      renderer.setPixelRatio(1);
      renderer.setSize(SIZE, SIZE, false);
      renderer.setClearColor(0x000000, 0);
      void gl;
      kit = createBuildingKit();
      scene = new Scene();
      scene.add(new AmbientLight(0xffffff, 1.15));
      const sun = new DirectionalLight(0xffffff, 1.5);
      sun.position.set(-3, 6, -2);
      scene.add(sun);
      camera = new PerspectiveCamera(30, 1, 0.5, 4000);
      return true;
    } catch (error) {
      // No second context: the galleries keep their glyphs.
      console.warn('parts studio: no second context', error);
      return false;
    }
  };

  const shoot = (id: string, onBatch: (images: ReadonlyMap<string, string>) => void): void => {
    const sample = partSample(id);
    if (!sample || !renderer || !kit || !scene || !camera) return;
    try {
      const meshes = buildBuildingMeshes([sample], () => 0, kit);
      // A signature building's body is drawn with its own parts, as in the town (`layer.ts`); the shared kit drew only its lot
      const floor = drawsSignature(sample) ? emitChunk(sample, () => 0).signatureFloor : undefined;
      const signature = floor !== undefined ? buildSignature(sample, floor) : null;
      // A part that builds nothing keeps its glyph: an empty tile is a defect.
      if (meshes.triangles === 0 && !signature) {
        meshes.dispose();
        return;
      }
      scene.add(meshes.group);
      if (signature) meshes.group.add(signature);
      const box = buildingBounds(sample);
      const height = Math.max(buildingHeight(sample), 3);
      // An element on its own stands on nothing
      const alone = sample.volumes.length === 0;
      const span = Math.max(box.maxX - box.minX, box.maxY - box.minY, height * 0.9) + (alone ? 2 : 4);
      const focus = { x: (box.minX + box.maxX) / 2, y: (box.minY + box.maxY) / 2 };
      // One preset per kind of thing, as engines give each asset type its own thumbnail renderer
      const view = ROOF_PARTS[id] ? 'roof' : WALL_PARTS[id] ? 'wall' : 'model';
      const bounds = new Box3().setFromObject(meshes.group);
      const centre = bounds.getCenter(new Vector3());
      const size = bounds.getSize(new Vector3());
      // A wall part is framed on two bays of the ground storey, not the block.
      const radius = view === 'wall'
        ? 0.5 * Math.hypot(Math.min(size.x, 7.5), Math.min(size.y, 5))
        : 0.5 * Math.hypot(size.x, size.y, size.z);
      if (view === 'wall') centre.y = bounds.min.y + Math.min(size.y, 5) * 0.62;
      const dir = view === 'roof' ? new Vector3(0.5, 1.05, 0.62) : view === 'wall' ? new Vector3(0.28, 0.12, 1) : new Vector3(0.62, 0.6, 0.62);
      dir.normalize();
      const distance = (Math.max(radius, 0.8) / Math.sin((camera.fov * Math.PI) / 360)) * 1.08;
      if (view === 'wall') {
        // The front wall: the face nearest the camera's side of the box.
        centre.z = bounds.max.z;
      }
      camera.position.copy(centre).addScaledVector(dir, distance);
      void span;
      void focus;
      void alone;
      camera.lookAt(centre);
      camera.updateProjectionMatrix();
      renderer.render(scene, camera);
      // The picture is taken now and encoded off the frame (`toBlob`); it is kept for the next sessions (`derivedCache.ts`).
      renderer.domElement.toBlob((blob) => {
        if (!blob) return;
        if (KIT_HASH) writeDerived(`thumb:${KIT_HASH}:${id}`, blob);
        onBatch(new Map([[id, URL.createObjectURL(blob)]]));
      }, 'image/png');
      scene.remove(meshes.group);
      if (signature) { meshes.group.remove(signature); disposeSignature(signature); }
      meshes.dispose();
    } catch {
      // A part that will not build keeps its glyph: the gallery still works.
    }
  };

  const step = (): void => {
    frame = null;
    if (!setUp()) {
      waiting.length = 0;
      return;
    }
    // At least one a frame, and no more once the frame's share is spent
    const started = performance.now();
    for (let i = 0; i < PER_FRAME && waiting.length > 0 && (i === 0 || performance.now() - started < FRAME_SHARE_MS); i++) {
      const next = waiting.shift();
      if (!next) break;
      shoot(next.id, next.onBatch);
      done.add(next.id);
    }
    if (waiting.length > 0) {
      frame = requestAnimationFrame(step);
      return;
    }
    // Nothing left to shoot and the Builder put down
    if (!held) idle = setTimeout(release, IDLE_MS);
  };

  const release = (): void => {
    idle = null;
    kit?.dispose();
    renderer?.dispose();
    canvas?.remove();
    canvas = null;
    renderer = null;
    kit = null;
    scene = null;
    camera = null;
  };

  return {
    request(ids, onBatch) {
      if (idle !== null) {
        clearTimeout(idle);
        idle = null;
      }
      const fresh = ids.filter((id) => !done.has(id) && !queued.has(id));
      for (const id of fresh) queued.add(id);
      if (!fresh.length) return;
      // The pictures kept from earlier sessions go up at once, with no GPU; only the others are taken.
      const lots = fresh.filter((id) => id.startsWith('lot:')).map((id) => loadLot(id.slice(4)));
      void Promise.all(lots).then(() => keptPictures()).then((pictures) => {
        const ready = new Map<string, string>();
        for (const id of fresh) {
          const blob = pictures.get(id);
          if (blob) {
            ready.set(id, URL.createObjectURL(blob));
            done.add(id);
          } else waiting.push({ id, onBatch });
        }
        if (ready.size) onBatch(ready);
        if (frame === null && waiting.length > 0) frame = requestAnimationFrame(step);
      });
    },
    hold(on) {
      held = on;
      if (on && idle !== null) {
        clearTimeout(idle);
        idle = null;
      } else if (!on && renderer && frame === null && waiting.length === 0 && idle === null) {
        idle = setTimeout(release, IDLE_MS);
      }
    },
    dispose() {
      if (frame !== null) cancelAnimationFrame(frame);
      if (idle !== null) clearTimeout(idle);
      frame = null;
      idle = null;
      waiting.length = 0;
      release();
    },
  };
}
