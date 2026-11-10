// Main window controller: turns button clicks into calls on the native notes API.
import { invoke } from "./bridge";
import { log_line } from "./log";

export class MainWindow {
  private editor_text = "";

  handle_save_clicked(): void {
    const text = this.read_editor();
    if (text.length > 0) {
      invoke("store_note", text);
    }
    log_line("save clicked");
  }

  handle_clear_clicked(): void {
    // Unlike handle_save_clicked, this one never calls store_note.
    invoke("clear_notes");
    log_line("clear clicked");
  }

  read_editor(): string {
    return this.editor_text.trim();
  }
}
