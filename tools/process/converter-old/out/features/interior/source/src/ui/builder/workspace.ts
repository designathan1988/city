// The code of src/ui/builder/workspace.ts that is the feature's (the rest of the file is shared).

// src/ui/builder/workspace.ts:337-340 method initBuilderWorkspace.showInside: the feature's own
    showInside(s) {
      insideNow = { on: s.on, level: s.level };
      for (const listen of insideListeners) listen(insideNow);
    },

// src/ui/builder/workspace.ts:341-341 method initBuilderWorkspace.inside: the feature's own
    inside: () => insideNow,

// src/ui/builder/workspace.ts:342-344 method initBuilderWorkspace.onInside: the feature's own
    onInside(listen) {
      insideListeners.push(listen);
    },

// src/ui/builder/workspace.ts:352-354 method initBuilderWorkspace.showGallery: the feature's own
    showGallery(category) {
      actions.setCategory(category);
    },

// src/ui/builder/workspace.ts:355-358 method initBuilderWorkspace.relabel: the feature's own
    relabel() {
      inspectorBody.dataset['signature'] = '';
      if (lastState) refresh(lastState);
    },

// src/ui/builder/workspace.ts:360-363 method initBuilderWorkspace.setPresetThumbnails: the feature's own
    setPresetThumbnails(images) {
      for (const [key, url] of images) thumbnails.set(key, url);
      if (lastState) for (const listen of listeners) listen(lastState);
    },

// src/ui/builder/workspace.ts:365-368 method initBuilderWorkspace.subscribe: the feature's own
    subscribe(listen) {
      listeners.push(listen);
      if (lastState) listen(lastState);
    },

// src/ui/builder/workspace.ts:369-369 method initBuilderWorkspace.thumbnail: the feature's own
    thumbnail: (id) => thumbnails.get(id),
