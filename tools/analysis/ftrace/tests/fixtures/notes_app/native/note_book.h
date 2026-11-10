// Ordered collection of notes; every change goes to the journal.
#pragma once
#include <string>
#include <vector>

struct Note {
    std::string text;
    long created_at;
};

Note make_note(const std::string& text);

class NoteBook {
public:
    void add(const Note& note);
    void clear();
    int count() const;
    std::vector<Note> notes;
};
