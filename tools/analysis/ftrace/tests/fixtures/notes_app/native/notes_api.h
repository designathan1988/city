// Native notes API that the UI reaches by name.
#pragma once
#include <string>
#include "note_book.h"

class NotesApi {
public:
    void store_note(const std::string& text);
    void clear_notes();
    int note_count() const;
private:
    NoteBook book;
};

void* call_native(const char* name, void* args);
