// Old storage path kept for reference; nothing calls it anymore.
void store_note_legacy(const char* text) {
    // store_note used to live here, before the NoteBook existed.
    write_lines("notes.txt", text);
}
