// JSDoc types bound their places in a JavaScript file (log/trace-rules.md, S2'; TypeScript JSDoc Reference).
// `viaDoc` passes any value on, as code with no declared type does.
class DocVec {
  /**
   * @param {number} [x=0] - The x value.
   */
  constructor(x = 0) {
    /**
     * The x value.
     *
     * @type {number}
     */
    this.x = x;
  }

  /** @param {number} x */
  set(x) { this.x = x; return this; }
}

function viaDoc(v) { return v; }

new DocVec(viaDoc("s")).set(viaDoc({}));

export function docX() { return new DocVec(1).x; }

/** @param {?number} n */
export function docNullable(n) { return n; }
docNullable(null);
docNullable(viaDoc("s"));
docNullable(2);

/** @param {number=} n */
export function docOptional(n) { return n; }
docOptional();
docOptional(viaDoc("s"));

/** @param {*} v */
export function docAny(v) { return v; }
docAny(viaDoc("s"));

/**
 * @param {Object} v - Anything handed in.
 * @returns {DocVec}
 */
export function docMake(v) { return v; }
docMake(new DocVec(1));
docMake(viaDoc("s"));

/** @param {Array.<number>} a */
export function docArray(a) { return a; }
docArray([1]);
docArray(viaDoc("s"));

/** @param {function(number): string} f */
export function docFn(f) { return f; }
docFn((n) => "x");
docFn(viaDoc("s"));

/** @param {!DocVec} v */
export function docBang(v) { return v; }
docBang(new DocVec(2));
docBang(viaDoc("s"));

/** @type {number} */
let docLet = viaDoc("s");
docLet = 3;
export function docLetRead() { return docLet; }
