// The code of src/render/people/gpuRows.ts that is the feature's (the rest of the file is shared).

// src/render/people/gpuRows.ts:9-11 type Step: the feature's own
type Step =
  | { readonly kind: 'write'; readonly row: number; readonly data: Float32Array }
  | { readonly kind: 'copy'; readonly from: number; readonly to: number; readonly count: number };

// src/render/people/gpuRows.ts:36-66 function copyPass: the feature's own
function copyPass(): NonNullable<typeof copier> {
  if (copier) return copier;
  const material = new ShaderMaterial({
    glslVersion: GLSL3,
    uniforms: { uFrom: { value: null }, uShift: { value: 0 } },
    vertexShader: /* glsl */`
      void main() { gl_Position = vec4(position.xy, 0.0, 1.0); }`,
    fragmentShader: /* glsl */`
      precision highp float;
      precision highp int;
      precision highp sampler2D;
      uniform sampler2D uFrom;
      uniform int uShift;
      out highp vec4 outColor;
      void main() {
        ivec2 at = ivec2(gl_FragCoord.xy);
        outColor = texelFetch(uFrom, ivec2(at.x, at.y - uShift), 0);
      }`,
    blending: NoBlending,
    depthTest: false,
    depthWrite: false,
  });
  const quad = new BufferGeometry();
  quad.setAttribute('position', new Float32BufferAttribute([-1, -1, 0, 3, -1, 0, -1, 3, 0], 3));
  const mesh = new Mesh(quad, material);
  mesh.frustumCulled = false;
  const scene = new Scene();
  scene.add(mesh);
  copier = { scene, camera: new OrthographicCamera(-1, 1, 1, -1, 0, 1), material };
  return copier;
}

// src/render/people/gpuRows.ts:68-73 function makeTarget: the feature's own
function makeTarget(width: number, rows: number): WebGLRenderTarget {
  return new WebGLRenderTarget(width, Math.max(1, rows), {
    type: FloatType, format: RGBAFormat, minFilter: NearestFilter, magFilter: NearestFilter,
    depthBuffer: false, stencilBuffer: false, generateMipmaps: false,
  });
}

// src/render/people/gpuRows.ts:76-83 function drawRows: the feature's own
function drawRows(renderer: WebGLRenderer, from: Texture, into: WebGLRenderTarget, fromRow: number, at: number, count: number): void {
  const pass = copyPass();
  pass.material.uniforms['uFrom']!.value = from;
  pass.material.uniforms['uShift']!.value = at - fromRow;
  into.viewport.set(0, at, into.width, count);
  renderer.setRenderTarget(into);
  renderer.render(pass.scene, pass.camera);
}

// src/render/people/gpuRows.ts:86-92 function wholeRows: the feature's own
export function wholeRows(data: Float32Array, width: number): Float32Array {
  const row = width * 4;
  if (data.length % row === 0 && data.length > 0) return data;
  const out = new Float32Array(Math.max(1, Math.ceil(data.length / row)) * row);
  out.set(data);
  return out;
}

// src/render/people/gpuRows.ts:94-172 function createGpuRows: the feature's own
export function createGpuRows(width: number, rows: number): GpuRows {
  let target = makeTarget(width, rows);
  let height = rows;
  /** Whether `target` holds rows on the GPU (flushed into since it was made). */
  let filled = false;
  /** The target grown from, its rows still to be carried over, and how many. */
  let carry: { target: WebGLRenderTarget; rows: number } | null = null;
  /** A target for a copy inside the texture (one cannot be read and drawn into at once). */
  let scratch: WebGLRenderTarget | null = null;
  const steps: Step[] = [];
  const at = new Vector2();

  return {
    get texture() { return target.texture; },
    get rows() { return height; },
    width,
    get bytes() { return width * height * 16; },
    write(row, data) {
      steps.push({ kind: 'write', row, data: wholeRows(data, width) });
    },
    copy(from, to, count) {
      if (from !== to && count > 0) steps.push({ kind: 'copy', from, to, count });
    },
    grow(next) {
      if (next <= height) return;
      // The first target grown from is the one on the GPU; one made since and never flushed holds nothing.
      if (filled && !carry) carry = { target, rows: height };
      else if (target !== carry?.target) target.dispose();
      target = makeTarget(width, next);
      height = next;
      filled = false;
    },
    flush(renderer) {
      if (!carry && !steps.length) return;
      const previous = renderer.getRenderTarget();
      const autoClear = renderer.autoClear;
      renderer.autoClear = false;
      // The copy's program built with the first rows sent (the crowd's opening), not at the first growth or twin.
      renderer.initRenderTarget(target);
      // Compiled with a float target set: three keys a program by its target (no tone mapping, linear output).
      if (!warmed) { const pass = copyPass(); renderer.setRenderTarget(target); renderer.compile(pass.scene, pass.camera); warmed = true; }
      if (carry) {
        drawRows(renderer, carry.target.texture, target, 0, 0, carry.rows);
        carry.target.dispose();
        carry = null;
      }
      for (const step of steps) {
        if (step.kind === 'write') {
          // A texture never on the GPU is sent from its own data (WebGLRenderer.copyTextureToTexture, the texSubImage2D branch).
          const rowsOf = step.data.length / (width * 4);
          renderer.copyTextureToTexture(new DataTexture(step.data, width, rowsOf, RGBAFormat, FloatType), target.texture, null, at.set(0, step.row));
        } else {
          if (!scratch || scratch.height < step.count) { scratch?.dispose(); scratch = makeTarget(width, step.count); }
          renderer.initRenderTarget(scratch);
          drawRows(renderer, target.texture, scratch, step.from, 0, step.count);
          drawRows(renderer, scratch.texture, target, 0, step.to, step.count);
        }
      }
      steps.length = 0;
      filled = true;
      renderer.setRenderTarget(previous);
      renderer.autoClear = autoClear;
    },
    reset() {
      carry?.target.dispose();
      carry = null;
      steps.length = 0;
      filled = false;
    },
    dispose() {
      carry?.target.dispose();
      carry = null;
      scratch?.dispose();
      scratch = null;
      target.dispose();
      steps.length = 0;
    },
  };
}
