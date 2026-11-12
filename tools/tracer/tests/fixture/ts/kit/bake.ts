// A function taking one object apart in its parameter, and another in a declaration.
import { Oven } from "./oven";

export function bake({ oven, onDone: done }: { oven: Oven; onDone: () => void }): void {
  const { heat } = oven;
  heat.read();
  done();
}
