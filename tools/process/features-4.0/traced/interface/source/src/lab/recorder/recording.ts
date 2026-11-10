// The code of src/lab/recorder/recording.ts that is the feature's (the rest of the file is shared).

// src/lab/recorder/recording.ts:128-171 slice of src/lab/recorder/recording.ts#startRecording.onFrame (shared): the feature's statements; "…" marks code left out
  const onFrame = (sample: LabSample): void => {
    // … 20 lines
    if (t - lastView >= VIEW_EVERY_MS) {
      // … 3 lines
      const moved = !prev || prev.kind !== v.kind || Math.abs(v.zoom - prev.zoom) / Math.max(1e-6, prev.zoom) > 0.01
        || Math.hypot(v.centre.x - prev.x, v.centre.y - prev.y) * v.zoom > 20 || Math.abs(v.azimuth - prev.azimuth) > 0.01 || Math.abs(v.elevation - prev.elevation) > 0.01;
      if (moved) {
        const view: ViewSample = { t: round(t), kind: v.kind, x: round(v.centre.x), y: round(v.centre.y), zoom: round(v.zoom, 10000), azimuth: round(v.azimuth, 1000), elevation: round(v.elevation, 1000) };
        if (prev && t - viewMovedAt > VIEW_STILL_MS) {
          events.push({ t: round(t), kind: 'view.move', detail: `camera moved: zoom ${prev.zoom} → ${view.zoom}, centre (${Math.round(view.x)}, ${Math.round(view.y)})`, split: false });
        }
        viewMovedAt = t;
        views.push(view);
      }
    }
  // … 7 lines
  };
