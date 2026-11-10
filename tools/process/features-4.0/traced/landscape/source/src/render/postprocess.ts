// The code of src/render/postprocess.ts that is the feature's (the rest of the file is shared).

// src/render/postprocess.ts:28-57 slice of src/render/postprocess.ts#PostChain (shared): the feature's statements; "…" marks code left out
export interface PostChain {
  // … 20 lines
  setPlacedClouds(clouds: readonly PlacedCloud[]): void;
// … 7 lines
}

// src/render/postprocess.ts:75-567 slice of src/render/postprocess.ts#createPostChain (shared): the feature's statements; "…" marks code left out
export function createPostChain(
// … 5 lines
): PostChain {
  // … 78 lines
  {
    // … 1 lines
    scenePass.render = (...args: Parameters<RenderPass['render']>) => {
      // … 3 lines
      if (!sceneRead()) {
        const gl = args[0];
        (copyQuad.material as ShaderMaterial).uniforms['tDiffuse']!.value = sceneTarget.texture;
        const previous = gl.getRenderTarget();
        gl.setRenderTarget(readBuffer);
        copyQuad.render(gl);
        gl.setRenderTarget(previous);
      }
    };
  }
  // … 58 lines
  const canyon = quality.ambientOcclusion
    ? createCanyonLight(camera, () => sceneDepth, () => (sunLights ??= ['sun', 'sun-cascade-1'].map((name) => scene.getObjectByName(name) as DirectionalLight | undefined).filter((light): light is DirectionalLight => light !== undefined)))
    : null;
  if (canyon) {
    const pass = canyon.pass;
    const shade = pass.render.bind(pass);
    // The scene's own resolved picture read directly (no copy), with the occlusion multiplied in as GTAO's blend did (dst * mix(1, ao, intensity)).
    pass.render = (gl, writeBuffer, readBuffer, delta, mask) => {
      canyon.setOcclusion(gtao?.enabled ? gtao.pdRenderTarget.texture : null, gtao?.blendIntensity ?? 0);
      shade(gl, writeBuffer, pass.enabled ? sceneTarget : readBuffer, delta, mask);
    };
    sceneRead = () => pass.enabled;
    composer.addPass(pass);
    const quad = fullScreenTriangle();
    const warm = new Scene();
    warm.add(new Mesh(quad, canyon.pass.material));
    // For the target it draws into (the composer's: no tone mapping, its colour space): compiled for another, its program was built again at its first draw (9 s, wprof7).
    const previous = renderer.getRenderTarget();
    renderer.setRenderTarget(composer.renderTarget1);
    warming.push(renderer.compileAsync(warm, camera).catch((error: unknown) => {
      console.error('Street light shader precompile failed', error);
    }).finally(() => quad.dispose()));
    renderer.setRenderTarget(previous);
  }
  // … 30 lines
  let placedClouds: readonly PlacedCloud[] = [];
  // … 19 lines
  const mapStamp = new CloudShadowStamp();
  // … 4 lines
  if (clouds) {
    // … 17 lines
    pass.render = (...args: Parameters<ShaderPass['render']>) => {
      // … 5 lines
      if (on && (u['uShadowMapOn']!.value as number) > 0.5 && (u['uStrength']!.value as number) > 0
        && mapStamp.changed(u['uCloudCount']!.value as number, CLOUD_PUFFS, u['uCloud']!.value as Vector4[], u['uPuff']!.value as Vector4[],
          u['uLife']!.value as number[], u['uBase']!.value as number[], u['uShow']!.value as number[], u['uSunDir']!.value as Vector3,
          u['uShadowRect']!.value as Vector4, u['uShadowPlane']!.value as number)) {
        const previous = gl.getRenderTarget();
        gl.setRenderTarget(shadowMapTarget);
        mapQuad.render(gl);
        gl.setRenderTarget(previous);
      }
    // … 16 lines
    };
  // … 15 lines
  }
  // … 48 lines
  for (const [pass, name] of [[scenePass, 'scene'], [gtao, 'gtao'], [canyon?.pass, 'canyon'], [bloom, 'bloom'], [clouds, 'clouds'], [smaa, 'smaa'], [output, 'output']] as [Pass | null | undefined, string][]) {
    if (!pass) continue;
    const draw = pass.render.bind(pass);
    pass.render = (...args: Parameters<Pass['render']>) => {
      const hook = scene.userData[POST_PASS_HOOK] as PostPassHook | undefined;
      if (!hook) { draw(...args); return; }
      hook.before(name);
      try { draw(...args); } finally { hook.after(name); }
    };
  }
// … 130 lines
}

// src/render/postprocess.ts:310-310 slice of src/render/postprocess.ts#createPostChain.mapLost (shared): the feature's statements; "…" marks code left out
  const mapLost = (): void => mapStamp.invalidate();

// src/render/postprocess.ts:454-488 slice of src/render/postprocess.ts#createPostChain.render (shared): the feature's statements; "…" marks code left out
    render(delta) {
      // … 28 lines
      if (canyon) canyon.pass.enabled = switches.canyonLight;
    // … 4 lines
    },

// src/render/postprocess.ts:520-522 slice of src/render/postprocess.ts#createPostChain.followTown (shared): the feature's statements; "…" marks code left out
    followTown(cx, cy, solids, changed) {
      canyon?.follow(cx, cy, solids, changed);
    },

// src/render/postprocess.ts:523-534 slice of src/render/postprocess.ts#createPostChain.setAtmosphere (shared): the feature's statements; "…" marks code left out
    setAtmosphere(atmosphere, sun, skyColor, sunLight, eyeShift = 0) {
      canyon?.setSun(sun, directShare);
    // … 9 lines
    },

// src/render/postprocess.ts:549-566 slice of src/render/postprocess.ts#createPostChain.disposeAll (shared): the feature's statements; "…" marks code left out
  function disposeAll(): void {
    canyon?.dispose();
  // … 15 lines
  }

// src/render/postprocess.ts:642-657 slice of src/render/postprocess.ts#layClouds (shared): the feature's statements; "…" marks code left out
function layClouds(bounds: Vector4[], puffs: Vector4[], lives: number[], bases: number[], shows: number[], placed: readonly PlacedCloud[], drift: { x: number; y: number },
  about: { x: number; y: number }): number {
  // … 1 lines
  for (const cloud of placed) {
    if (slot >= MAX_CLOUDS) break;
    // Where the wind has carried it, its repeat round the view, thinning away near the edge it wraps round.
    const at = driftedCloud(cloud, drift, about);
    const height = cloud.height;
    layOne(slot, 1_000_003 + cloud.id * 7919, at.x, -at.y, height, cloud.size, cloud.yaw, cloud.density, 1, bounds, puffs, lives);
    shows[slot] = at.show;
    // Each its own flat base, at its own height
    bases[slot] = height;
    slot++;
  }
// … 1 lines
}
