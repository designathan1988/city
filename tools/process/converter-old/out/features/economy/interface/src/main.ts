// What the feature uses of src/main.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/main.ts:189-189 variable doc
const doc = new RoadDoc();

// src/main.ts:197-197 variable camera
const camera = new Camera();

// src/main.ts:608-608 variable view
let view: Viewport = flatViewport(camera);

// src/main.ts:2942-3410 function drawOverlayScreen
function drawOverlayScreen(): void;

// src/main.ts:3413-3419 variable BUILD_FILL
/** How each stretch of the road in hand will be built, drawn between its deck and the natural ground */
const BUILD_FILL: Readonly<Record<BuildMode, string | null>> = {
  ground: null,
  embankment: 'rgba(164, 120, 74, 0.62)',
  cutting: 'rgba(132, 128, 120, 0.62)',
  bridge: null,
  tunnel: 'rgba(40, 34, 30, 0.55)',
};

// src/main.ts:3420-3420 variable PIER_EVERY
const PIER_EVERY = 14;

// src/main.ts:3421-3459 function drawBuildModes
function drawBuildModes(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], ground: readonly Vec2[], modes: readonly BuildMode[]): void;

// src/main.ts:3461-3475 function drawTunnelStretches
function drawTunnelStretches(ctx: CanvasRenderingContext2D, deck: readonly Vec2[], modes: readonly BuildMode[], width: number): void;

// src/main.ts:3478-3486 function buildSummary
/** The ways the road in hand is built, beside its length: "ponte 120 m · aterro 40 m". */
function buildSummary(stations: Parameters<typeof buildRuns>[0]): string;

// src/main.ts:3837-3856 function currentStatus
/** The city's numbers now: the clock, the traffic, the money, the junctions that cannot be built. */
function currentStatus(): ShellStatus;
