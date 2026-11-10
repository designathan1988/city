// Draws the visible notes straight from the NoteBook.
#include "note_book.h"

void draw_notes(const NoteBook& book) {
    for (const Note& note : book.notes) {
        paint_text(note.text);
    }
}
