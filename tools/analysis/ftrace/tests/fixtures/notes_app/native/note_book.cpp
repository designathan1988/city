// NoteBook implementation.
#include "note_book.h"
#include "journal.h"

Note make_note(const std::string& text) {
    return Note{text, current_time()};
}

void NoteBook::add(const Note& note) {
    notes.push_back(note);
    journal_record("add");
}

void NoteBook::clear() {
    notes.clear();
    journal_record("clear");
}

int NoteBook::count() const {
    return static_cast<int>(notes.size());
}
