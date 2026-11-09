export class Clock {
  constructor(private readonly frame: (now: number) => void) {}

  request(): void {
    requestAnimationFrame(this.shown);
  }

  private readonly shown = (now: number): void => {
    this.frame(now);
  };
}

function step(now: number): number {
  return now * 2;
}

export function frame(now: number): void {
  step(now);
  clock.request();
}

export const clock = new Clock(frame);

export function deferOnce(): void {
  requestAnimationFrame(() => step(0));
}
