// Periodic autosave: flushes the journal on every interval tick.
#include "journal.h"

void autosave_tick(int elapsed_seconds) {
    if (elapsed_seconds % 30 == 0) {
        journal_flush();
    }
}
