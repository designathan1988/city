// A callable written into, or passed to a method of, a receiver that can be the engine's object is run by the engine,
// whatever objects of the program the receiver can also be (log/trace-rules.md, J6, J11).
class OwnPort {
  onmessage: ((e: number) => void) | null = null;
  fire() { this.onmessage?.(1); }
}

class OwnThen {
  then(done: (n: number) => void) { done(1); }
}

export function mixedWrite(own: boolean) {
  const port = own ? new OwnPort() : new MessageChannel().port1;
  port.onmessage = (e) => {};
}

export function mixedCall(own: boolean) {
  const later = own ? new OwnThen() : fetch("x");
  later.then((r) => r);
}
