// What the feature uses of src/view/cameraMotion.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/view/cameraMotion.ts:7-17 variable KEYS
/** THE CAMERA'S MOTION OVER TIME: the keys held and the wheel's notches turned into a glide, frame by frame */

/** The camera's own keys, by `KeyboardEvent.code`. */
const KEYS: Readonly<Record<string, readonly [Axis, 1 | -1]>> = {
  KeyW: ['forward', 1], ArrowUp: ['forward', 1],
  KeyS: ['forward', -1], ArrowDown: ['forward', -1],
  KeyA: ['right', -1], ArrowLeft: ['right', -1],
  KeyD: ['right', 1], ArrowRight: ['right', 1],
  KeyQ: ['turn', -1], KeyE: ['turn', 1],
  // Tilt on Page Up/Down: R and F (Cities: Skylines II's zoom keys) are the building's rotation and the walls tool here.
  PageUp: ['tilt', 1], PageDown: ['tilt', -1],
  Equal: ['zoom', 1], NumpadAdd: ['zoom', 1],
  Minus: ['zoom', -1], NumpadSubtract: ['zoom', -1],
};

// src/view/cameraMotion.ts:18-18 type Axis
type Axis = 'forward' | 'right' | 'turn' | 'tilt' | 'zoom';

// src/view/cameraMotion.ts:50-195 class CameraMotion
export class CameraMotion { … }
