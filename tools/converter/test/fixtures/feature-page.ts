// The feature fixture's page and style sheet (test/fixtures/feature/index.html and src/shared/app.css), written there by
// test/feature.test.ts before it reads the fixture: the repository's gates read no HTML or CSS, so the two files are made
// at test time from these texts, byte for byte, and kept out of git (tools/converter/.gitignore).

/** The page: its style sheet and the module script the program starts from. */
export const FEATURE_PAGE = `<!doctype html>
<html><head><link rel="stylesheet" href="/src/shared/app.css" /></head><body><script type="module" src="/src/main.ts"></script></body></html>
`;

/** The style sheet: ferry-row only the feature names (its rules, also inside a media query, and a rule that needs it);
 *  panel the program names too (shared); other the feature never names. */
export const FEATURE_STYLE = `/* Fixture: the page's style sheet. ferry-row only the feature names (its rules, also inside a media query, and a rule
   that needs it); panel the program names too (shared); other the feature never names. */
.ferry-row { color: red; }
.other { color: blue; }
@media (max-width: 600px) {
  .ferry-row { color: green; }
}
.panel { padding: 1px; }
.ferry-row.panel { margin: 0; }
.panel .other { margin: 1px; }
`;
