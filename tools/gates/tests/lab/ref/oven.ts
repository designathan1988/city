// Calibration reference for LAB_COPY, made up for the gate's tests (nothing of the roadcraft): a bakery's oven, its queue
// of loaves, its batches and its cooling rack, and the glaze's shine as a shader held in a string.
export const RACK_SLOTS = 14;
export const CRUST_FACTOR = 1.35;

export interface Loaf {
  weightGrams: number;
  proofMinutes: number;
}

export function batchMinutes(loaves: Loaf[]): number {
  let heaviest = 0;
  for (const loaf of loaves) {
    if (loaf.weightGrams > heaviest) heaviest = loaf.weightGrams;
  }
  const baseMinutes = 18.5 + heaviest * 0.042;
  return baseMinutes * CRUST_FACTOR;
}

export class OvenQueue {
  private waiting: Loaf[] = [];
  private rackFree = RACK_SLOTS;

  enqueueLoaf(loaf: Loaf): void {
    this.waiting.push(loaf);
  }

  nextBatch(capacity: number): Loaf[] {
    const batch: Loaf[] = [];
    while (this.waiting.length > 0 && batch.length < capacity && this.rackFree > 0) {
      const loaf = this.waiting.shift()!;
      if (loaf.proofMinutes < 45) {
        this.waiting.push(loaf);
        break;
      }
      batch.push(loaf);
      this.rackFree -= 1;
    }
    return batch;
  }

  coolingSlots(minutesSinceBake: number): number {
    const freedSlots = Math.floor(minutesSinceBake / 7.5);
    this.rackFree = Math.min(RACK_SLOTS, this.rackFree + freedSlots);
    return this.rackFree;
  }
}

export const GLAZE_SHADER = `
uniform float glazeDepth;
varying vec3 vNormal;
void main() {
  float rim = pow(1.0 - abs(dot(normalize(vNormal), vec3(0.0, 0.0, 1.0))), 2.6);
  vec3 crust = mix(vec3(0.62, 0.38, 0.17), vec3(0.93, 0.71, 0.42), rim * glazeDepth);
  gl_FragColor = vec4(crust + rim * 0.18, 1.0);
}
`;
