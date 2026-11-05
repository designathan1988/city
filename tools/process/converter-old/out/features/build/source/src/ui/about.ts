// The code of src/ui/about.ts that is the feature's (the rest of the file is shared).

// src/ui/about.ts:5-14 slice of src/ui/about.ts#mountAbout (shared): the feature's statements; "…" marks code left out
export function mountAbout(): void {
  // … 6 lines
  onBuildStamp(paint);
// … 1 lines
}

// src/ui/about.ts:8-10 slice of src/ui/about.ts#mountAbout.paint (shared): the feature's statements; "…" marks code left out
  const paint = (): void => {
    build.textContent = formatBuildStamp();
  };

// src/ui/about.ts:17-24 slice of src/ui/about.ts#openAbout (shared): the feature's statements; "…" marks code left out
export function openAbout(): void {
  // … 3 lines
  if (build) build.textContent = formatBuildStamp();
  // … 1 lines
  void refreshBuildStamp();
}
