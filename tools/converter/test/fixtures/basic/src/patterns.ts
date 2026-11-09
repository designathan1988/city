export function drain(queue: number[]): number {
  let sum = 0;
  while (queue.length > 0) {
    sum += queue.shift()!;
  }
  return sum;
}

export function forget(record: { a?: number }): void {
  delete record.a;
}

export function sized(n: number): number[] {
  return new Array(n);
}

export function nearest(points: { x: number; y: number }[]): number {
  let best = Infinity;
  for (const a of points) {
    for (const b of points) {
      best = Math.min(best, Math.hypot(a.x - b.x, a.y - b.y));
    }
  }
  return best;
}

export function send(worker: Worker, data: Float32Array): void {
  worker.postMessage({ data });
}
