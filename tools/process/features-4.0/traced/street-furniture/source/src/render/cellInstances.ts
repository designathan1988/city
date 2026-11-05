// The code of src/render/cellInstances.ts that is the feature's (the rest of the file is shared).

// src/render/cellInstances.ts:262-267 function warmMesh: the feature's own
export function warmMesh(spec: KindMesh, tinted: boolean, format: SlotFormat): InstancedMesh {
  const mesh = dressedMesh(spec, posedGeometry(spec.geometry, new InstancedInterleavedBuffer(new Float32Array(format.stride), format.stride)), format);
  if (tinted) mesh.instanceColor = new InstancedBufferAttribute(new Float32Array(3).fill(1), 3);
  mesh.count = 1;
  return mesh;
}

// src/render/cellInstances.ts:307-307 slice of src/render/cellInstances.ts#screenHalf (shared): the feature's statements; "…" marks code left out
export const screenHalf = (): number => screen.half;

// src/render/cellInstances.ts:310-317 function watchScreen: the feature's own
export function watchScreen(mesh: InstancedMesh): void {
  const before = mesh.onBeforeRender.bind(mesh);
  mesh.onBeforeRender = (renderer, ...rest) => {
    const half = renderer.getDrawingBufferSize(bufferSize).y / 2;
    if (half > 0) screen.half = half;
    before(renderer, ...rest);
  };
}

// src/render/cellInstances.ts:323-333 function batchMesh: the feature's own
export function batchMesh(spec: KindMesh, model: BufferGeometry, poses: InstancedInterleavedBuffer, tints: InstancedBufferAttribute | null,
  count: number, sphere: Sphere, format: SlotFormat): InstancedMesh {
  const mesh = dressedMesh(spec, posedGeometry(model, poses), format);
  mesh.instanceColor = tints;
  mesh.count = count;
  mesh.boundingSphere = sphere;
  mesh.frustumCulled = true;
  watchScreen(mesh);
  spec.made?.(mesh);
  return mesh;
}

// src/render/cellInstances.ts:336-339 function releaseBatchMesh: the feature's own
export function releaseBatchMesh(mesh: InstancedMesh): void {
  mesh.dispose();
  letGo(mesh.geometry);
}

// src/render/cellInstances.ts:447-1131 slice of src/render/cellInstances.ts#createCellInstances (shared): the feature's statements; "…" marks code left out
export function createCellInstances(kindSpecs: readonly KindSpec[], format: SlotFormat, margin: number): CellInstances {
  // … 547 lines
  return {
    // … 1 lines
    get triangles() {
      // … 1 lines
      for (const k of kinds) for (const s of k.meshes) if (s.mesh && !s.spec.far) n += trianglesOf(s.spec.geometry) * (k.used - k.dead);
      return n;
    },
  // … 128 lines
  };
}

// src/render/cellInstances.ts:604-632 slice of src/render/cellInstances.ts#createCellInstances.makeMesh (shared): the feature's statements; "…" marks code left out
  const makeMesh = (k: Kind, s: MeshState): InstancedMesh => {
    // … 12 lines
    mesh.onBeforeRender = (renderer, ...rest) => {
      // … 1 lines
      if (half > 0) screen.half = half;
    // … 1 lines
    };
  // … 10 lines
  };

// src/render/cellInstances.ts:1052-1066 method createCellInstances.setMap: the feature's own
    setMap(map) {
      if (mapMode === map) return;
      mapMode = map;
      for (const k of kinds) {
        if (k.spec.levels?.far === undefined && !k.spec.meshes.some((s) => s.cards)) continue;
        k.current = false;
        // DO NOT leave the cards shown with no instance at the map zoom: a draw call each for nothing.
        for (const s of k.meshes) {
          if (!s.spec.cards || !s.mesh) continue;
          const left = map && !k.pictured;
          s.mesh.visible = !left;
          if (left) s.mesh.count = 0;
        }
      }
    },

// src/render/cellInstances.ts:1067-1122 slice of src/render/cellInstances.ts#createCellInstances.cull (shared): the feature's statements; "…" marks code left out
    cull(frustum, view) {
      // … 2 lines
      if (!culledFor || !culledFor.equals(view) || culledHalf !== screen.half || (sunMoved && margin > 0)) {
        culledFor = (culledFor ?? new Matrix4()).copy(view);
        culledHalf = screen.half;
        Object.assign(culledSun, shadowSun);
        for (const k of kinds) k.current = false;
      }
    // … 46 lines
    },
