// The code of src/desktopJobs.ts that is the feature's (the rest of the file is shared).

// src/desktopJobs.ts:15-46 slice of src/desktopJobs.ts#desktopJobWorkers (shared): the feature's statements; "…" marks code left out
export function desktopJobWorkers(): ((long: boolean) => WorkerLike | null) | null {
  // … 16 lines
  return (long) => {
    // … 2 lines
    made = new ProcessWorker(() => new Promise<MessagePort>((resolve) => {
      asked.push((port, process) => {
        let set = living.get(process);
        if (!set) living.set(process, set = new Set());
        if (made) set.add(made);
        resolve(port);
      });
      connect();
    }), isRecordTable);
    return made;
  };
}
