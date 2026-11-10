// The code of src/render/citizenCasting.ts that is the feature's (the rest of the file is shared).

// src/render/citizenCasting.ts:87-118 function codesFor: the feature's own
function codesFor(company: Company, hash: number, hasChild: boolean): DressCode[] {
  const rotate = (list: DressCode[]): DressCode[] => {
    const k = hash % list.length;
    return [...list.slice(k), ...list.slice(0, k)];
  };
  // One adult party in twelve, and one lone walker in twenty, in traditional dress.
  const traditional = !hasChild && (hash >>> 8) % 12 === 0;
  const withTradition = (list: DressCode[]): DressCode[] => (traditional ? [CODES.traditional, ...list] : list);
  switch (company) {
    case 'colleagues': return [CODES.office];
    case 'family': return rotate([CODES.everyday, CODES.sporty]);
    case 'couple': return withTradition(hasChild ? rotate([CODES.everyday, CODES.sporty]) : rotate([CODES.everyday, CODES.office, CODES.sporty]));
    case 'friends': return withTradition(rotate([CODES.sporty, CODES.everyday]));
    case 'elders': return withTradition([CODES.everyday]);
    case 'tourists': return withTradition([CODES.sporty]);
    case 'car': {
      // A car in five is on a work trip; the rest everyday or weekend clothes.
      const base: DressCode[] = [CODES.everyday, CODES.sporty];
      if (!hasChild && hash % 5 === 0) return [CODES.office, ...base];
      return withTradition(rotate(base));
    }
    case 'bus': case 'solo': {
      // Strangers: each dressed on their own, in one wardrobe.
      const k = hash % 40;
      const first: DressCode = k < 17 ? CODES.casual : k < 25 ? CODES.smart : k < 32 ? CODES.business : k < 38 ? CODES.sport
        : hasChild ? CODES.casual : CODES.traditional;
      return [first, CODES.casual, CODES.smart];
    }
    case 'truck': return [CODES.casual];
    case 'rider': return rotate([CODES.casual, CODES.sport, CODES.smart]);
  }
}

// src/render/citizenCasting.ts:138-142 interface Casting: the feature's own
export interface Casting {
  readonly index: number;
  readonly size: number;
  readonly code: DressCode;
}

// src/render/citizenCasting.ts:147-152 function hashOf: the feature's own
const hashOf = (n: number): number => {
  let h = (n | 0) + 0x7f4a7c15;
  h = Math.imul(h ^ (h >>> 15), 0x2c1b3c6d);
  h = Math.imul(h ^ (h >>> 12), 0x297a2d39);
  return (h ^ (h >>> 15)) >>> 0;
};

// src/render/citizenCasting.ts:159-159 field CastingRegistry.wearers: the feature's own
  private readonly wearers = new Map<number, Set<number>>();

// src/render/citizenCasting.ts:161-161 field CastingRegistry.members: the feature's own
  private readonly members = new Map<string, Set<number>>();

// src/render/citizenCasting.ts:163-163 field CastingRegistry.codes: the feature's own
  private readonly codes = new Map<string, DressCode>();

// src/render/citizenCasting.ts:165-165 field CastingRegistry.drawn: the feature's own
  private drawn: { seed: number; index: number; company: Company; companyId: number; code: DressCode }[] = [];

// src/render/citizenCasting.ts:166-166 field CastingRegistry.recordCensus: the feature's own
  recordCensus = false;

// src/render/citizenCasting.ts:169-169 constructor CastingRegistry.constructor: the feature's own
  constructor(private readonly roster: readonly CitizenModel[] = CROWD) {}

// src/render/citizenCasting.ts:172-178 method CastingRegistry.forgetCompany: the feature's own
  forgetCompany(company: Company, companyId: number): void {
    const group = `${company}:${companyId}`;
    const seeds = this.members.get(group);
    if (seeds) for (const seed of seeds) this.drop(seed);
    this.members.delete(group);
    this.codes.delete(group);
  }

// src/render/citizenCasting.ts:181-191 method CastingRegistry.drop: the feature's own
  private drop(seed: number): void {
    const entry = this.cast.get(seed);
    if (!entry) return;
    this.cast.delete(seed);
    const worn = this.wearers.get(entry.index);
    worn?.delete(seed);
    if (worn?.size === 0) this.wearers.delete(entry.index);
    const group = this.members.get(entry.group);
    group?.delete(seed);
    if (group?.size === 0) this.members.delete(entry.group);
  }

// src/render/citizenCasting.ts:194-197 method CastingRegistry.join: the feature's own
  private join(seed: number, group: string): void {
    const set = this.members.get(group);
    if (set) set.add(seed); else this.members.set(group, new Set([seed]));
  }

// src/render/citizenCasting.ts:205-207 method CastingRegistry.beginFrame: the feature's own
  beginFrame(): void {
    this.drawn.length = 0;
  }

// src/render/citizenCasting.ts:210-228 method CastingRegistry.candidates: the feature's own
  candidates(ctx: CastingContext, code: DressCode, helmetFits: (id: string) => boolean = () => true):
    { pool: number[]; size: number; spare: number[]; spareSize: number } {
    const of = (band: AgeBand): number[] => {
      const out: number[] = [];
      this.roster.forEach((m, i) => {
        if (m.gender === ctx.gender && m.ageBand === band && code.includes(m.wardrobe) && (!ctx.helmet || helmetFits(m.id))
          && (ctx.company !== 'rider' || m.rides !== false)) out.push(i);
      });
      return out;
    };
    // An adult is any grown body; an elder a senior's first, a grown one when every senior's is worn.
    if (ctx.ageClass === 'adult') return { pool: [...of('young'), ...of('adult')], size: 1, spare: [], spareSize: 1 };
    if (ctx.ageClass === 'child') return { pool: of('child'), size: 1, spare: [], spareSize: 1 };
    const [first, second] = [of('senior'), of('adult')];
    const spareSize = 1;
    if (first.length) return { pool: first, size: 1, spare: second, spareSize };
    if (second.length) return { pool: second, size: spareSize, spare: [], spareSize };
    return { pool: [], size: 1, spare: [], spareSize: 1 };
  }

// src/render/citizenCasting.ts:231-319 method CastingRegistry.pickCitizenModel: the feature's own
  pickCitizenModel(ctx: CastingContext, helmetFits: (id: string) => boolean = () => true): Casting | null {
    const known = this.cast.get(ctx.seed);
    if (known) {
      // The two fields, not the string built from them
      if (known.company !== ctx.company || known.companyId !== ctx.companyId) {
        const group = `${ctx.company}:${ctx.companyId}`;
        // A walker boarding a car keeps their body.
        const wardrobe = this.roster[known.index]!.wardrobe;
        const options = codesFor(ctx.company, hashOf(ctx.companyId ^ 0x51ce),
          ctx.hasChild ?? ctx.ageClass === 'child');
        const code: DressCode = options.find((candidate) => candidate.includes(wardrobe)) ?? [wardrobe];
        this.codes.set(group, code);
        // The generated occupants of a vehicle are re-dressed to match (they are barely seen inside it)
        const vehicle = ctx.company === 'car' || ctx.company === 'bus' || ctx.company === 'truck';
        for (const seed of [...(this.members.get(group) ?? [])]) {
          if (seed === ctx.seed) continue;
          const entry = this.cast.get(seed)!;
          if (!vehicle || code.includes(this.roster[entry.index]!.wardrobe)) { entry.code = code; continue; }
          this.drop(seed);
        }
        const left = this.members.get(known.group);
        left?.delete(ctx.seed);
        if (left?.size === 0) this.members.delete(known.group);
        this.join(ctx.seed, group);
        known.group = group;
        known.code = code;
        known.company = ctx.company;
        known.companyId = ctx.companyId;
      }
      known.x = ctx.x;
      known.y = ctx.y;
      this.note(ctx, known);
      return known;
    }
    const group = `${ctx.company}:${ctx.companyId}`;
    const hash = hashOf(ctx.seed);
    const groupHash = hashOf(ctx.companyId ^ 0x51ce);
    // The company's code: its first member settles it, taking the first code of its kind that dresses them
    const shared = ctx.company !== 'solo' && ctx.company !== 'bus' && ctx.company !== 'rider';
    let options = codesFor(ctx.company, shared ? groupHash : hash, ctx.hasChild ?? ctx.ageClass === 'child');
    const settled = shared ? this.codes.get(group) : undefined;
    if (settled) options = [settled, ...options.filter((c) => c !== settled)];
    let code: DressCode | undefined;
    let choice = { pool: [] as number[], size: 1, spare: [] as number[], spareSize: 1 };
    for (const candidate of options) {
      const c = this.candidates(ctx, candidate, helmetFits);
      if (!c.pool.length) continue;
      if (settled && candidate !== settled && !this.dressesMembers(group, candidate)) continue;
      code = candidate;
      choice = c;
      break;
    }
    if (!code || !choice.pool.length) return null;
    if (shared && code !== settled) this.recode(group, code);
    const { pool, spare } = choice;
    let size = choice.size;
    let index = -1;
    let fallback = pool[hash % pool.length]!;
    let farthest = -1;
    const deck = pool.length + spare.length;
    for (let k = 0; k < deck; k++) {
      const fromSpare = k >= pool.length;
      const candidate = fromSpare ? spare[(hash + k) % spare.length]! : pool[(hash + k) % pool.length]!;
      let nearest = Infinity;
      let taken = false;
      for (const other of this.wearers.get(candidate) ?? []) {
        const worn = this.cast.get(other);
        if (!worn) continue;
        if (shared && worn.group === group) { taken = true; break; }
        nearest = Math.min(nearest, Math.hypot(worn.x - ctx.x, worn.y - ctx.y));
      }
      if (taken) continue;
      if (nearest >= CAST_NEAR) {
        index = candidate;
        if (fromSpare) size = choice.spareSize;
        break;
      }
      if (!fromSpare && nearest > farthest) { farthest = nearest; fallback = candidate; }
    }
    if (index < 0) index = fallback;
    const entry: Entry = { index, size, x: ctx.x, y: ctx.y, group, code, company: ctx.company, companyId: ctx.companyId };
    this.cast.set(ctx.seed, entry);
    this.join(ctx.seed, group);
    const list = this.wearers.get(index);
    if (list) list.add(ctx.seed);
    else this.wearers.set(index, new Set([ctx.seed]));
    this.note(ctx, entry);
    return entry;
  }

// src/render/citizenCasting.ts:322-327 method CastingRegistry.dressesMembers: the feature's own
  private dressesMembers(group: string, code: DressCode): boolean {
    for (const seed of this.members.get(group) ?? []) {
      if (!code.includes(this.roster[this.cast.get(seed)!.index]!.wardrobe)) return false;
    }
    return true;
  }

// src/render/citizenCasting.ts:329-332 method CastingRegistry.recode: the feature's own
  private recode(group: string, code: DressCode): void {
    this.codes.set(group, code);
    for (const seed of this.members.get(group) ?? []) this.cast.get(seed)!.code = code;
  }

// src/render/citizenCasting.ts:334-336 method CastingRegistry.note: the feature's own
  private note(ctx: CastingContext, entry: Entry): void {
    if (this.recordCensus) this.drawn.push({ seed: ctx.seed, index: entry.index, company: ctx.company, companyId: ctx.companyId, code: entry.code });
  }

// src/render/citizenCasting.ts:339-342 method CastingRegistry.census: the feature's own
  census(): { seed: number; model: string; wardrobe: Wardrobe; company: Company; companyId: number; code: DressCode }[] {
    return this.drawn.map((d) => ({ seed: d.seed, model: this.roster[d.index]!.id, wardrobe: this.roster[d.index]!.wardrobe,
      company: d.company, companyId: d.companyId, code: d.code }));
  }
