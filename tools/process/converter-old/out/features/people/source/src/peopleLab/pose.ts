// The code of src/peopleLab/pose.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/pose.ts:101-101 slice of src/peopleLab/pose.ts#ClipPlayer.transfer (shared): the feature's statements; "…" marks code left out
  private transfer: NeutralWalk | null = null;

// src/peopleLab/pose.ts:102-102 slice of src/peopleLab/pose.ts#ClipPlayer.clip (shared): the feature's statements; "…" marks code left out
  private clip: LibraryClip | null = null;

// src/peopleLab/pose.ts:105-105 slice of src/peopleLab/pose.ts#ClipPlayer.constructor (shared): the feature's statements; "…" marks code left out
  constructor(private readonly root: Object3D, private readonly mesh: SkinnedMesh, private readonly clips: RocketboxClips) {}

// src/peopleLab/pose.ts:108-116 slice of src/peopleLab/pose.ts#ClipPlayer.play (shared): the feature's statements; "…" marks code left out
  play(name: LibraryClipName | null, now: number): boolean {
    const clip = name ? this.clips[name] : undefined;
    if (!clip) { this.transfer = null; this.clip = null; return false; }
    if (clip === this.clip) return true;
    this.clip = clip;
    this.transfer = clipTransferFor(this.root, this.mesh, clip);
  // … 2 lines
  }

// src/peopleLab/pose.ts:118-118 slice of src/peopleLab/pose.ts#ClipPlayer.playing (shared): the feature's statements; "…" marks code left out
  get playing(): boolean { return !!this.transfer; }

// src/peopleLab/pose.ts:124-128 slice of src/peopleLab/pose.ts#ClipPlayer.apply (shared): the feature's statements; "…" marks code left out
  apply(now: number): void {
    if (!this.transfer || !this.clip) return;
    // … 1 lines
    this.transfer.pose(this.clip.loop ? t % this.clip.duration : Math.min(t, this.clip.duration - 1e-3));
  }
