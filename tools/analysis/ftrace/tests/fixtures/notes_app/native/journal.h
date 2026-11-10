// Append-only change journal.
#pragma once
void journal_record(const char* what);
void journal_flush();
long current_time();
