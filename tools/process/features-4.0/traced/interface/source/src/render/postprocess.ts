// The code of src/render/postprocess.ts that is the feature's (the rest of the file is shared).

// src/render/postprocess.ts:28-57 slice of src/render/postprocess.ts#PostChain (shared): the feature's statements; "…" marks code left out
export interface PostChain {
  // … 26 lines
  setSwitches(switches: PostSwitches): void;
// … 1 lines
}

// src/render/postprocess.ts:75-567 slice of src/render/postprocess.ts#createPostChain (shared): the feature's statements; "…" marks code left out
export function createPostChain(
  // … 3 lines
  quality: QualitySettings,
  level: QualityLevel,
): PostChain {
  if (!quality.postProcessing) {
    // A canvas made for the chain has no multisampling of its own (`renderer.ts`): the scene multisampled in a target, then tone mapped onto it, as the canvas did.
    const plain = renderer.getContext().getContextAttributes()?.antialias === false ? plainChain(renderer, scene, camera) : null;
    return {
      enabled: false,
      target: plain?.target ?? null,
      ready: Promise.resolve(),
      targets: () => (plain ? [plain.target] : []),
      render() {
        if (plain) plain.render();
        else renderer.render(scene, camera);
      },
      setSize(width, height, pixelRatio) {
        // Otherwise the renderer's own resize is enough.
        plain?.setSize(width, height, pixelRatio);
      },
      setNight() {
        /* no bloom without the chain */
      },
      setDirectShare() {
        /* no cloud shadows without the chain */
      },
      followTown() {
        /* no street light without the chain */
      },
      setAtmosphere() {
        /* no clouds or mist without the chain */
      },
      setGroundFog() {
        /* no painted fog without the chain */
      },
      setCloudDrift() {},
      setSwitches() {
        /* no passes without the chain */
      },
      setPlacedClouds() {
        /* no clouds without the chain */
      },
      dispose() {
        plain?.dispose();
      },
    };
  }
  // … 4 lines
  const sceneTarget = new WebGLRenderTarget(Math.max(1, size.x * ratio), Math.max(1, size.y * ratio), {
    // … 4 lines
    samples: Math.min(quality.msaa, renderer.capabilities.maxSamples),
  // … 3 lines
  });
  // … 21 lines
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
  // … 2 lines
  if (quality.ambientOcclusion) {
    gtao = new GTAOPass(scene, camera, size.x, size.y);
    // DO NOT let it copy the picture and blend into it (`OUTPUT.Default`, two full passes): its denoised occlusion is multiplied in by the canyon pass that reads the picture next (`canyonLight.ts` tAO).
    gtao.output = GTAOPass.OUTPUT.Off;
    gtao.needsSwap = false;
    // Tuned for world units where a kerb is 0.4 high and a pier is 15 tall.
    gtao.updateGtaoMaterial({
      radius: level === 'ultra' ? 1.3 : 1.0,
      distanceExponent: 1.6,
      thickness: 0.8,
      scale: 1.05,
      // Eight samples on every tier: the Poisson denoise takes the noise out, and sixteen at the scene's half resolution
      // (the AO's default) cost the pass about a third more for an edge no player can tell apart (the lab's passes, 2026-10-20).
      samples: 8,
      screenSpaceRadius: false,
    });
    gtao.blendIntensity = 0.85;
    // The denoise: half the Poisson taps of three's default (16), the noise it takes out is under an edge no player can tell apart (the lab's passes, 2026-10-20).
    gtao.updatePdMaterial({ samples: 8 });
    // An occlusion factor is a smooth 0..1, not an HDR colour: half floats held 12.4 MB of it at the half resolution. An 8-bit channel is enough for both the denoise and the multiply (`canyonLight.ts` reads the denoised target).
    {
      const width = gtao.gtaoRenderTarget.width, height = gtao.gtaoRenderTarget.height;
      gtao.gtaoRenderTarget.dispose();
      gtao.pdRenderTarget.dispose();
      gtao.gtaoRenderTarget = new WebGLRenderTarget(width, height, { depthBuffer: false });
      gtao.pdRenderTarget = new WebGLRenderTarget(width, height, { depthBuffer: false });
      gtao.pdMaterial.uniforms['tDiffuse']!.value = gtao.gtaoRenderTarget.texture;
    }
    // The occlusion is worked out from the depth the scene was just drawn with (normals rebuilt from it).
    const pass = gtao;
    const draw = pass.render.bind(pass);
    pass.render = (...args: Parameters<GTAOPass['render']>) => {
      const depth = sceneDepth;
      if (depth && pass.depthTexture !== depth) pass.setGBuffer(depth);
      draw(...args);
    };
    // At the scene's resolution, or half of it (`QualitySettings.aoFull`): scaled up, the half one blurs every edge the occlusion lies along
    const resize = pass.setSize.bind(pass);
    const share = quality.aoFull ? 1 : 2;
    pass.setSize = (width: number, height: number) => resize(Math.max(1, Math.ceil(width / share)), Math.max(1, Math.ceil(height / share)));
    composer.addPass(gtao);
    // GTAO's denoise shader is the slowest program to link on a cold start.
    gtao.setGBuffer(sceneTarget.depthTexture!);
    const quad = fullScreenTriangle();
    const warm = new Scene();
    warm.add(new Mesh(quad, gtao.gtaoMaterial), new Mesh(quad, gtao.pdMaterial));
    const previous = renderer.getRenderTarget();
    renderer.setRenderTarget(gtao.gtaoRenderTarget);
    warming.push(renderer.compileAsync(warm, camera).catch((error: unknown) => {
      console.error('Ambient occlusion shader precompile failed', error);
    }).finally(() => quad.dispose()));
    renderer.setRenderTarget(previous);
  }
  // … 3 lines
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
  // … 2 lines
  const bloomStrength = level === 'ultra' ? 0.22 : 0.18;
  const bloom = new UnrealBloomPass(new Vector2(size.x, size.y), bloomStrength, 0.32, 1.05);
  // … 1 lines
  {
    const resize = bloom.setSize.bind(bloom);
    bloom.setSize = (width: number, height: number) => resize(Math.max(1, Math.ceil(width / 2)), Math.max(1, Math.ceil(height / 2)));
  }
  shareBloomBlur(bloom.separableBlurMaterials as ShaderMaterial[]);
  bloom.enabled = false;
  composer.addPass(bloom);
  // … 1 lines
  {
    // … 3 lines
    for (const material of [bloom.materialHighPassFilter, ...bloom.separableBlurMaterials, bloom.compositeMaterial, bloom.blendMaterial]) {
      warm.add(new Mesh(quad, material));
    }
    // … 1 lines
    renderer.setRenderTarget(bloom.renderTargetsHorizontal[0]!);
  // … 2 lines
  }
  // … 1 lines
  const clouds = quality.cloudShadows || quality.skyClouds ? new ShaderPass(CLOUD_SHADOWS) : null;
  // … 28 lines
  if (clouds) {
    const pass = clouds;
    const fragment = CLOUD_SHADOWS.fragmentShader;
    const functions = fragment.slice(0, fragment.indexOf('void main()'));
    bodiesQuad = new FullScreenQuad(new ShaderMaterial({
      uniforms: pass.uniforms, vertexShader: CLOUD_SHADOWS.vertexShader,
      fragmentShader: functions + CLOUD_BODIES_MAIN,
      blending: NoBlending, depthTest: false, depthWrite: false,
    }));
    shadowMapQuad = new FullScreenQuad(new ShaderMaterial({
      uniforms: pass.uniforms, vertexShader: CLOUD_SHADOWS.vertexShader,
      fragmentShader: functions + CLOUD_SHADOW_MAP_MAIN,
      blending: NoBlending, depthTest: false, depthWrite: false,
    }));
    pass.uniforms['tCloudShadow']!.value = shadowMapTarget.texture;
    const quad = bodiesQuad;
    const mapQuad = shadowMapQuad;
    const shade = pass.render.bind(pass);
    pass.render = (...args: Parameters<ShaderPass['render']>) => {
      pass.uniforms['tDepth']!.value = sceneDepth;
      const on = (pass.uniforms['uCloudCount']!.value as number) > 0;
      pass.uniforms['uCloudsOn']!.value = on ? 1 : 0;
      const gl = args[0];
      const u = pass.uniforms as Record<string, { value: unknown }>;
      if (on && (u['uShadowMapOn']!.value as number) > 0.5 && (u['uStrength']!.value as number) > 0
        && mapStamp.changed(u['uCloudCount']!.value as number, CLOUD_PUFFS, u['uCloud']!.value as Vector4[], u['uPuff']!.value as Vector4[],
          u['uLife']!.value as number[], u['uBase']!.value as number[], u['uShow']!.value as number[], u['uSunDir']!.value as Vector3,
          u['uShadowRect']!.value as Vector4, u['uShadowPlane']!.value as number)) {
        const previous = gl.getRenderTarget();
        gl.setRenderTarget(shadowMapTarget);
        mapQuad.render(gl);
        gl.setRenderTarget(previous);
      }
      if (on) {
        // With the view still, fewer steps and the last frame blended in (Playdead's TAA feedback)
        const still = historyValid;
        u['uSteps']!.value = still ? CLOUD_STEPS_STILL : CLOUD_STEPS_MOVING;
        u['uHistory']!.value = still ? CLOUD_HISTORY : 0;
        u['uFrame']!.value = still ? cloudFrame++ % 4096 : 0;
        const write = bodiesTargets[bodiesWrite]!, read = bodiesTargets[1 - bodiesWrite]!;
        u['tCloudHistory']!.value = read.texture;
        const previous = gl.getRenderTarget();
        gl.setRenderTarget(write);
        quad.render(gl);
        gl.setRenderTarget(previous);
        u['tClouds']!.value = write.texture;
        bodiesWrite = 1 - bodiesWrite;
      }
      shade(...args);
    };
    composer.addPass(clouds);
    // Its program built now, in parallel
    {
      const quad = fullScreenTriangle();
      const previous = renderer.getRenderTarget();
      const builds = ([[clouds.material, composer.renderTarget1], [bodiesQuad!.material, bodiesTargets[0]!], [shadowMapQuad!.material, shadowMapTarget]] as const).map(([material, target]) => {
        const warm = new Scene();
        warm.add(new Mesh(quad, material));
        renderer.setRenderTarget(target);
        return renderer.compileAsync(warm, camera).catch(() => {});
      });
      renderer.setRenderTarget(previous);
      // Not drawn until they are built: drawn while the driver still builds them, the frame waits all the same
      warming.push(Promise.all(builds).finally(() => { quad.dispose(); cloudsReady = true; }));
    }
  }
  const smaa = quality.smaa ? new SMAAPass() : null;
  if (smaa) {
    // three ships SMAA at its *Medium* preset
    const tune = (material: ShaderMaterial, defines: Record<string, string>): void => {
      Object.assign(material.defines as Record<string, string>, defines);
      material.needsUpdate = true;
    };
    const pass = smaa as unknown as {
      _materialEdges: ShaderMaterial;
      _materialWeights: ShaderMaterial;
    };
    tune(pass._materialEdges, { SMAA_THRESHOLD: '0.04' });
    tune(pass._materialWeights, { SMAA_MAX_SEARCH_STEPS: '16' });
    // DO NOT keep three's half-float edges and weights: SMAA's own are RG8 and RGBA8 (Jimenez et al., SMAA.hlsl: edgesTex R8G8, blendTex R8G8B8A8), four and eight bytes a pixel less.
    const targets = smaa as unknown as { _edgesRT: WebGLRenderTarget; _weightsRT: WebGLRenderTarget; _uniformsWeights: Record<string, { value: unknown }>; _uniformsBlend: Record<string, { value: unknown }> };
    targets._edgesRT.dispose();
    targets._weightsRT.dispose();
    targets._edgesRT = new WebGLRenderTarget(1, 1, { depthBuffer: false, type: UnsignedByteType, format: RGFormat });
    targets._edgesRT.texture.name = 'SMAAPass.edges';
    targets._weightsRT = new WebGLRenderTarget(1, 1, { depthBuffer: false, type: UnsignedByteType });
    targets._weightsRT.texture.name = 'SMAAPass.weights';
    targets._uniformsWeights['tDiffuse']!.value = targets._edgesRT.texture;
    targets._uniformsBlend['tDiffuse']!.value = targets._weightsRT.texture;
    composer.addPass(smaa);
  }
  // … 4 lines
  output.uniforms['uSharpen'] = { value: quality.sharpen };
  // … 18 lines
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
  // … 5 lines
  let switches: PostSwitches = ALL_EFFECTS_ON;
// … 124 lines
}

// src/render/postprocess.ts:454-488 slice of src/render/postprocess.ts#createPostChain.render (shared): the feature's statements; "…" marks code left out
    render(delta) {
      // … 31 lines
      (output.uniforms['uSharpen'] as { value: number }).value = switches.sharpen ? quality.sharpen : 0;
    // … 1 lines
    },
