// Shared code of the program: it keeps what it is given and calls it later.
const jobs: (() => unknown)[] = [];
export function later(job: () => unknown): void { jobs.push(job); }
export function drain(): void { for (const job of jobs.splice(0)) job(); }
export function later2(job: { run: () => number }): void { later(job.run); }
