// Application start: builds the main window from its layout file and shows it.
import { load_layout } from "../ui/layout_loader";

export function main(): void {
  const window = load_layout("ui/main_window.layout");
  window.show();
}
