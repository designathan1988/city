// The code of src/render/occupants.ts that is the feature's (the rest of the file is shared).

// src/render/occupants.ts:9-9 slice of src/render/occupants.ts#OccupantClip (shared): the feature's statements; "…" marks code left out
type OccupantClip = RiderClipKey | 'walk' | 'walkSlow' | 'standUp' | 'idle';

// src/render/occupants.ts:20-24 slice of src/render/occupants.ts#SEAT_CLIPS (shared): the feature's statements; "…" marks code left out
const SEAT_CLIPS: Readonly<Record<SeatPose, { readonly drive: readonly [RiderClipKey, RiderClipKey, RiderClipKey]; readonly ride: readonly [RiderClipKey, RiderClipKey, RiderClipKey] }>> = {
  car: { drive: ['carDrive', 'carDriveMirror', 'carDriveRight'], ride: ['carRide', 'carRideLeft', 'carRideRight'] },
  cab: { drive: ['cabDrive', 'cabDriveMirror', 'cabDriveRight'], ride: ['cabRide', 'cabRideLeft', 'cabRideRight'] },
  chair: { drive: ['chairSit', 'chairSitLeft', 'chairSitRight'], ride: ['chairSit', 'chairSitPhone', 'chairSitRight'] },
};

// src/render/occupants.ts:27-29 slice of src/render/occupants.ts#clipsFor (shared): the feature's statements; "…" marks code left out
const clipsFor = (seat: Pick<SeatModel, 'pose' | 'driver' | 'row'>): readonly [RiderClipKey, RiderClipKey, RiderClipKey] =>
  seat.pose === 'car' && !seat.driver && seat.row > 0
    ? REAR_CAR_CLIPS : SEAT_CLIPS[seat.pose][seat.driver ? 'drive' : 'ride'];

// src/render/occupants.ts:32-34 slice of src/render/occupants.ts#seatBaseClip (shared): the feature's statements; "…" marks code left out
export function seatBaseClip(seat: Pick<SeatModel, 'pose' | 'driver' | 'row'>): RiderClipKey {
// … 1 lines
}

// src/render/occupants.ts:50-71 slice of src/render/occupants.ts#occupantPlays (shared): the feature's statements; "…" marks code left out
export function occupantPlays(seat: Pick<SeatModel, 'pose' | 'driver' | 'row' | 'z'>, seed: number, time: number, out: Play[], slain = false): Play[] {
  // … 13 lines
  const key: RiderClipKey = seat.pose === 'chair' && !seat.driver && variant === 2
    ? (seat.z > 0 ? 'chairSitRight' : 'chairSitLeft')
    : clips[variant]!;
  // … 2 lines
  const idle = time / SEATED_IDLE + (h & 0xff) / 256;
  return onePlay(out, glance > 0.5 ? key : clips[0]!, idle);
}
