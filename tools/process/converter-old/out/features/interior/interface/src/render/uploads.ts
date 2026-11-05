// What the feature uses of src/render/uploads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/uploads.ts:75-98 function shadowStandIn
/** A caster as the shadow pass draws it: on its own depth material (or three's plain one) */
export function shadowStandIn(mesh: Mesh): Object3D | null;

// src/render/uploads.ts:410-410 variable queuedBytes
/** The bytes of the loose meshes waiting to go up: their arrays live on the page until they are on the GPU (`gpuOnly`). */
let queuedBytes = 0;

// src/render/uploads.ts:412-412 variable WARM_CEILING
/** Past this the producers ask no new mesh with its arrays (`warmFull`): the queue drains `UPLOAD_BYTES` a frame, and its arrays live on the CPU until sent (the page's process reached 7.3 GB, ~3.7 GB native, at a re-bake: mu1, conf77). */
export const WARM_CEILING = 64 * 1024 * 1024;

// src/render/uploads.ts:422-422 variable reservedBytes
/** The bytes asked of the workers and not landed yet, as their askers reckon them (`reserveWarm`): counted with the queue. DO NOT let only what already waits hold the producers back: every job in flight landed into the queue at once, past its ceiling (the page holding the answers until they went up). */
let reservedBytes = 0;

// src/render/uploads.ts:439-439 variable warmer
let warmer = false;

// src/render/uploads.ts:445-453 function warmLooseAhead
/** Meshes not in the scene yet (a batch of buildings put together to replace the one drawn); `priority`: the pixels of error their absence leaves on the screen, the largest sent first. Settles once their buffers are on the GPU and their programs, lit and in the shadow, are built (`programsReady`). */
export function warmLooseAhead(meshes: readonly Mesh[], priority: () => number = () => 0): Promise<void> | null;

// src/render/uploads.ts:455-466 function cancelWarm
/** DO NOT dispose a mesh still waiting here without this */
export function cancelWarm(meshes: Iterable<Mesh>): void;

// src/render/uploads.ts:468-490 function meshBytes
/** The bytes a mesh's buffers take on the GPU: every attribute (an interleaved buffer once), the index, an instanced mesh's matrices and colours. */
export function meshBytes(mesh: Mesh, geometries: readonly BufferGeometry[] = [mesh.geometry]): number;

// src/render/uploads.ts:564-585 function gateUntilBuilt
/** What comes into the scene (`renderer.ts` childadded): every mesh, line and points under it whose programs are not built held off the drawn layers until they are (`programsOf`). Nothing is held where no renderer answers (tests, the opening before the first frame: the catalogue builds those). */
export function gateUntilBuilt(root: Object3D, opened?: () => void): void;
