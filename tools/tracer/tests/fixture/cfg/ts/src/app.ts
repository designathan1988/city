// Names imported through the aliases the configuration declares.
import { temper, Anvil } from "~kit/forge";
import { hone } from "#tool";

export function work(): number {
  const a = new Anvil();
  return temper(a.strike()) + hone();
}
