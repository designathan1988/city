// The code of src/lab/recorder/report.ts that is the feature's (the rest of the file is shared).

// src/lab/recorder/report.ts:102-262 slice of src/lab/recorder/report.ts#buildReport (shared): the feature's statements; "…" marks code left out
export function buildReport(ctx: LabContext, rec: Recording): LabReport {
  // … 5 lines
  const build = formatBuildStamp();
  // … 2 lines
  const meta = {
    // … 12 lines
    build,
  // … 2 lines
  };
  // … 24 lines
  md.push(`- Build: ${build} · ${location.href}`);
// … 109 lines
}
