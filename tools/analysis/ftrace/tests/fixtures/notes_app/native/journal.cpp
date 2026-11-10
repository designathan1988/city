// Append-only change journal, flushed to disk by the autosave timer.
#include <string>
#include <vector>
#include "journal.h"

static std::vector<std::string> g_entries;

void journal_record(const char* what) {
    g_entries.push_back(what);
}

void journal_flush() {
    write_lines("journal.txt", g_entries);
    g_entries.clear();
}

long current_time() {
    return 0;
}
