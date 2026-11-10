// Dispatches named calls from the UI to NotesApi methods.
#include <cstring>
#include "notes_api.h"
#include "log.h"

static NotesApi g_api;

struct Binding {
    const char* name;
    void (*fn)(void*);
};

static void bind_store(void* args) { g_api.store_note(*static_cast<std::string*>(args)); }
static void bind_clear(void*) { g_api.clear_notes(); }

static const Binding kBindings[] = {
    {"store_note", bind_store},
    {"clear_notes", bind_clear},
};

void* call_native(const char* name, void* args) {
    for (const Binding& binding : kBindings) {
        if (std::strcmp(binding.name, name) == 0) {
            binding.fn(args);
            return nullptr;
        }
    }
    log_line("unknown native call");
    return nullptr;
}

void NotesApi::store_note(const std::string& text) {
    Note note = make_note(text);
    book.add(note);
    log_line("stored");
}

void NotesApi::clear_notes() {
    book.clear();
}

int NotesApi::note_count() const {
    return book.count();
}
