// Names read through a folder's file; a property read and written; a component given a handler, which gives it to an
// element the code read does not define; a module loaded where it is used.
import { Oven, cook, Tools } from "./kit";

function Panel({ onPress }: { onPress: () => void }) {
  return <button onClick={onPress} />;
}

function pressed(): void { Tools.wipe(); }

export function Kitchen() {
  const oven = Oven.make();
  oven.heat = oven.heat;
  cook({ oven, onDone: pressed });
  return <Panel onPress={pressed} />;
}

export async function late(): Promise<number> {
  const loaded = await import("./kit/oven");
  return loaded.Tools.wipe();
}
