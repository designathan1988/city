# Notes

A small app to write a note and save it. Notes live in memory while the app runs, and every change
is written to a journal in case the app stops before the next autosave.

## Building

If you are on Linux, run the build script; for Windows, use the same script in a Git shell:

```bash
./scripts/build.sh release
```

Do not edit the journal by hand: it is rewritten in full on every flush, so anything added in between is lost.

## How saving works

The notes are kept in a list in memory. When you click save, the window asks the native side to store the note,
which appends it to the list and records the change in the journal.

- `store_note` is the native entry point for saving.
- `draw_notes` repaints the list after every change.

There is no undo yet; see the issue tracker for the plan.
