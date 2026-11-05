// What the feature uses of src/render/riderPoses.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/riderPoses.ts:52-57 interface WheelSpec
/** A steering wheel, from the hip point, metres: its centre ahead and above, the tilt of its rim from vertical */
export interface WheelSpec {
  readonly forward: number;
  readonly up: number;
  readonly tilt: number;
  readonly radius: number;
}

// src/render/riderPoses.ts:76-96 interface TwoWheelerFit
/** Where a rider sits on a two-wheeler and what they hold and stand on, metres. */
export interface TwoWheelerFit {
  /** Height of the pelvis bone over the road. */
  readonly pelvisY: number;
  /** How far the pelvis bone sits above the saddle's top: the seat of the trousers. */
  readonly saddleDrop: number;
  /** The centre of each grip. */
  readonly grip: { readonly side: number; readonly up: number; readonly forward: number };
  /** The steering axis, ahead of the pelvis: the bars turn about it. */
  readonly steerAxis: number;
  /** A motorcycle's footpegs. */
  readonly peg?: { readonly side: number; readonly up: number; readonly forward: number };
  /** A bicycle's bottom bracket, and its cranks. */
  readonly bracket?: { readonly up: number; readonly forward: number };
  readonly crank?: number;
  /** Pedal centres either side of the frame. */
  readonly pedalSide?: number;
  /** Stopped, the machine tilts this far towards the rider's left foot, which is down. */
  readonly stopTilt: number;
  /** Where that foot is put down: out from the centre line, and a little ahead of the pelvis. */
  readonly footDown: { readonly side: number; readonly forward: number };
}
