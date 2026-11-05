// The code of src/peopleLab/widgets.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/widgets.ts:53-56 slice of src/peopleLab/widgets.ts#widgets.thumbOf (shared): the feature's statements; "…" marks code left out
  function thumbOf(name: string): string | null {
    // … 1 lines
    return hasThumb(name) ? proxyUrl(`${name}-thumb.webp`) : null;
  }

// src/peopleLab/widgets.ts:58-59 slice of src/peopleLab/widgets.ts#widgets.pretty (shared): the feature's statements; "…" marks code left out
  const pretty = (name: string): string => name.replace(/^(female|male)_/, '').replace(/^[a-z0-9]+_(?=[a-z])/i, (m) => (communityItem(name) ? '' : m))
    .replace(/_/g, ' ').replace(/\b\w/g, (c) => c.toUpperCase()).slice(0, 26);
