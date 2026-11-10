// Reads a layout file and wires each button to the controller method it names.
import { MainWindow } from "./main_window";

export function load_layout(path: string): Window {
  const spec = read_text(path);
  const controller = new MainWindow();
  for (const line of spec.split("\n")) {
    const handler = pick_attribute(line, "on_click");
    if (handler) {
      connect_click(line, (controller as any)[handler]);
    }
  }
  return new Window(spec);
}

function pick_attribute(line: string, key: string): string {
  const at = line.indexOf(key + "=\"");
  return at < 0 ? "" : line.slice(at + key.length + 2, line.indexOf("\"", at + key.length + 2));
}
