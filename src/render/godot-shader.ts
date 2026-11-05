// A three.js program (the vertex and fragment GLSL WebGL ran, chunks and onBeforeCompile edits already in) translated
// into one Godot spatial shader, statement for statement, by the rules of Godot's own documentation and engine source:
//  - "Converting GLSL to Godot shaders": one file, `main` → `vertex()`/`fragment()`, `gl_Position` → `POSITION`
//    (clip space; projection skipped), `gl_FragColor` → the output, `gl_FragCoord` → `FRAGCOORD`, varyings → `varying`.
//  - The spatial shader reference: three's matrices are Godot's built-ins (modelMatrix → MODEL_MATRIX, viewMatrix →
//    VIEW_MATRIX, projectionMatrix → PROJECTION_MATRIX, modelViewMatrix → MODELVIEW_MATRIX, normalMatrix →
//    MODELVIEW_NORMAL_MATRIX, cameraPosition → CAMERA_POSITION_WORLD); attributes are VERTEX, NORMAL, UV, UV2, COLOR,
//    TANGENT and CUSTOM0–3; `render_mode unshaded` because three's program computes the final color itself.
//  - Godot's engine source (scene_forward_clustered.glsl): a MultiMesh instance's transform is multiplied into the
//    model matrix before the user's vertex code and its color into COLOR, so `instanceMatrix` is the identity there and
//    `instanceColor` is COLOR; shader_types.cpp: custom functions see only TIME, EXPOSURE and IN_SHADOW_PASS, so a
//    helper that reads another built-in receives it as a parameter; shader_language.cpp: there is no shadow sampler,
//    so `sampler2DShadow` is a `sampler2D` compared by hand (three's depth compare is LEQUAL: step(ref, depth)).
//  - Color: three's working space is linear and its output transfer (sRGB) and tone mapping run in the shader; Godot
//    applies both after the fragment, so three's `linearToOutputTexel` and `toneMapping` become the identity.
//  - Godot has no struct uniforms: a uniform array of structs is one uniform array per field.
// What a rule cannot carry is said in `unsupported`, never dropped silently. The uniforms the engine must still feed
// (everything not a Godot built-in) are listed with their three names.
import { createRequire } from 'node:module';
import { functionScope, isScalar, isVector, referenceTypes, typeOf, type TypeEnv } from './glsl-types.ts';
import path from 'node:path';

/* eslint-disable @typescript-eslint/no-explicit-any */
type Node = { type: string; [key: string]: any };
interface Path { node: Node; parent: Node | undefined; replaceWith(n: Node): void; skip(): void }

export interface Glsl {
  parse(source: string, options?: object): Node;
  generate(node: Node): string;
  preprocess(source: string, options?: object): string;
  visit(node: Node, visitors: Record<string, { enter?: (p: Path) => void; exit?: (p: Path) => void }>): void;
}

/** The ShaderFrog GLSL parser (`@shaderfrog/glsl-parser`) from the directory holding its package (or its node_modules). */
export function loadGlsl(dir: string): Glsl {
  const base = path.resolve(dir);
  const req = createRequire(path.join(base, 'package.json'));
  const name = '@shaderfrog/glsl-parser';
  const main = req(name) as { parse: Glsl['parse']; generate: Glsl['generate'] };
  const pre = req(`${name}/preprocessor`) as { default?: Glsl['preprocess']; preprocess?: Glsl['preprocess'] };
  const ast = req(`${name}/ast`) as { visit: Glsl['visit'] };
  const preprocess = pre.default ?? pre.preprocess;
  if (!preprocess) throw new Error(`${name}: no preprocess export`);
  return { parse: main.parse, generate: main.generate, preprocess, visit: ast.visit };
}

export interface ThreeProgram {
  readonly vertex: string;
  readonly fragment: string;
}

export interface UniformInput {
  /** The uniform as three names it (a struct array's field: `directionalLights[].direction`). */
  readonly three: string;
  /** The uniform in the Godot shader. */
  readonly godot: string;
  readonly type: string;
  readonly array: string;
  readonly note?: string;
}

export interface GodotShader {
  readonly code: string;
  /** three's SHADER_TYPE (MeshStandardMaterial, ShaderMaterial...) and SHADER_NAME. */
  readonly material: string;
  readonly name: string;
  /** What the engine feeds: every uniform that is not a Godot built-in. */
  readonly uniforms: UniformInput[];
  /** Each vertex attribute and where it is in Godot (a mesh array or a built-in). */
  readonly attributes: { readonly three: string; readonly godot: string; readonly type: string }[];
  readonly notes: string[];
  readonly unsupported: string[];
}

export interface TranslateOptions {
  /** Godot render modes beyond `unshaded` (cull, blend, depth: the three material's side, blending, depthWrite). */
  readonly renderModes?: readonly string[];
  /** A sampler's state, which three keeps on the texture and Godot in the uniform's hints (`source_color` for an sRGB
   *  texture, `filter_nearest` for NearestFilter, `repeat_disable` for ClampToEdgeWrapping...), by uniform name. */
  readonly samplerHints?: Readonly<Record<string, readonly string[]>>;
  /** Attributes three binds per instance (vertexAttribDivisor 1: InstancedBufferAttribute). */
  readonly instancedAttributes?: readonly string[];
  /** Where three's matrices come from. 'godot' (the default): Godot's built-ins (MODEL_MATRIX, VIEW_MATRIX,
   *  PROJECTION_MATRIX...), so the Godot camera and node must equal three's. 'three': uniforms holding three's own
   *  values (`_three_projectionMatrix`...), and three's clip position converted to Godot's at the end of vertex() —
   *  every value the program computes is then three's, whatever its camera (a full-screen pass, a shadow camera). The
   *  node then has the identity transform (a MultiMesh's MODEL_MATRIX is the instance's own). */
  readonly matrices?: 'godot' | 'three';
  /** three's depth: 'standard' (WebGL: near at -1, window depth 0 near) or 'reversed' (reversedDepthBuffer:
   *  EXT_clip_control zero-to-one, near at 1, like Godot's own). */
  readonly depth?: 'standard' | 'reversed';
  /** A shadow sampler's compare function (TEXTURE_COMPARE_FUNC: 'lequal' for three's default depth, 'gequal' with
   *  a reversed depth buffer) and filter ('linear': the hardware's 2×2 compare then bilinear weights, 'nearest'). */
  readonly shadowCompare?: 'lequal' | 'gequal' | 'less' | 'greater';
  readonly shadowFilter?: 'linear' | 'nearest';
  /** 'display' (the default): three's canvas, Godot encoding ALBEDO to sRGB after the fragment. 'raw': the values
   *  the program wrote, as stored by its target (a render target, or a Godot viewport with HDR 2D: no encoding). */
  readonly output?: 'display' | 'raw';
  /** The draw buffer (fragment output location) this pass writes, for a program with several outputs (MRT). */
  readonly fragmentOutput?: number;
  /** Integer samplers (usampler2D, isampler2D...) whose texture is given as a 32-bit float texture holding the
   *  integers' bits (Godot 4.7 takes no 32-bit integer texture: its RenderingDevice formats map to Image formats,
   *  which stop at RGBA16I). Declared as float samplers, read back exactly with floatBitsToUint/floatBitsToInt (an
   *  integer texture has no filtering: nearest returns the stored bits). By three's uniform name. */
  readonly floatBitsSamplers?: readonly string[];
  /** How a non-opaque program's alpha reaches the target. Writing ALPHA puts a Godot material in the transparent
   *  pipeline, whose blending is never off (spatial_shader: blend_mix is the default; there is no disabled mode).
   *  'blend' (the default): ALPHA with the render mode's blending (three's blending on). 'replace': ALPHA with
   *  blend_premul_alpha and the colour as written, so colour and alpha are stored exactly over a cleared target
   *  (three's blending off; a draw alone, or the first into a target). 'none': no ALPHA (the opaque pipeline: the
   *  colour exact, the stored alpha 1). */
  readonly alphaOutput?: 'blend' | 'replace' | 'none';
  /** What the pass stores in Godot's colour: 'rgb' (the default: the program's colour) or 'alpha' (its alpha, in all
   *  three channels, through the opaque pipeline). With three's blending off, Godot's opaque pipeline overwrites like
   *  three (its transparent one always blends) but stores no alpha of the shader's: an 'rgb' pass with alphaOutput
   *  'none' and an 'alpha' pass together carry exactly what three stored. */
  readonly channels?: 'rgb' | 'alpha';
  /** With matrices 'three': 'gl' leaves three's clip y as it is, so Godot rasterizes in GL's framebuffer orientation
   *  (row 0 = the bottom, as WebGL's offscreen framebuffers: vertex snapping and the fill rule then break ties the
   *  same way; the image's rows are readPixels' rows, and a render target is sampled with three's v). Triangles then
   *  keep three's index order. 'godot' (the default): y negated (an upright Godot image; indices reversed). */
  readonly orientation?: 'gl' | 'godot';
  /** Sub-texel precision of the hardware's filtering (VkPhysicalDeviceLimits.subTexelPrecisionBits; 8 on the GPUs
   *  measured): the hand-written shadow compare quantizes its bilinear weights to it, as the hardware's PCF does. */
  readonly shadowSubTexelBits?: number;
}

// ---- tables ---------------------------------------------------------------------------------------------------

/** Godot spatial built-ins a helper may need as a parameter, with their types (spatial shader reference). */
const BUILTIN_TYPES: Readonly<Record<string, string>> = {
  VERTEX: 'vec3', NORMAL: 'vec3', TANGENT: 'vec3', BINORMAL: 'vec3', UV: 'vec2', UV2: 'vec2', COLOR: 'vec4',
  CUSTOM0: 'vec4', CUSTOM1: 'vec4', CUSTOM2: 'vec4', CUSTOM3: 'vec4', POSITION: 'vec4', POINT_SIZE: 'float',
  INSTANCE_ID: 'int', VERTEX_ID: 'int', INSTANCE_CUSTOM: 'vec4',
  MODEL_MATRIX: 'mat4', VIEW_MATRIX: 'mat4', INV_VIEW_MATRIX: 'mat4', PROJECTION_MATRIX: 'mat4', INV_PROJECTION_MATRIX: 'mat4',
  MODELVIEW_MATRIX: 'mat4', MODELVIEW_NORMAL_MATRIX: 'mat3', MODEL_NORMAL_MATRIX: 'mat3',
  CAMERA_POSITION_WORLD: 'vec3', CAMERA_DIRECTION_WORLD: 'vec3', NODE_POSITION_WORLD: 'vec3', VIEWPORT_SIZE: 'vec2',
  FRAGCOORD: 'vec4', FRONT_FACING: 'bool', POINT_COORD: 'vec2', SCREEN_UV: 'vec2', DEPTH: 'float',
};

/** Names Godot reserves that GLSL does not: a user identifier with one of them is renamed. */
const RESERVED = new Set([
  'instance', 'global', 'group_uniforms', 'shader_type', 'render_mode', 'stencil_mode', 'source_color',
  'color_conversion_disabled', 'hint_range', 'hint_enum', 'instance_index', 'hint_normal', 'hint_default_white',
  'hint_default_black', 'hint_default_transparent', 'hint_anisotropy', 'hint_roughness_r', 'hint_roughness_g',
  'hint_roughness_b', 'hint_roughness_a', 'hint_roughness_normal', 'hint_roughness_gray', 'hint_screen_texture',
  'hint_normal_roughness_texture', 'hint_depth_texture', 'hint_blit_source0', 'hint_blit_source1', 'hint_blit_source2',
  'hint_blit_source3', 'filter_nearest', 'filter_linear', 'filter_nearest_mipmap', 'filter_linear_mipmap',
  'filter_nearest_mipmap_anisotropic', 'filter_linear_mipmap_anisotropic', 'repeat_enable', 'repeat_disable',
  'no_storage', 'no_editor', 'TIME', 'PI', 'TAU', 'E', 'EXPOSURE', 'IN_SHADOW_PASS', 'OUTPUT_IS_SRGB', 'CLIP_SPACE_FAR',
  'IS_MULTIVIEW', 'ALBEDO', 'ALPHA', 'EMISSION', 'METALLIC', 'ROUGHNESS', 'SPECULAR', 'LIGHT', 'VIEW',
  ...Object.keys(BUILTIN_TYPES),
]);

/** three's built-in uniforms, per stage, as Godot expressions. */
const THREE_UNIFORMS: Readonly<Record<'vertex' | 'fragment', Readonly<Record<string, string>>>> = {
  vertex: {
    modelMatrix: 'MODEL_MATRIX', viewMatrix: 'VIEW_MATRIX', projectionMatrix: 'PROJECTION_MATRIX',
    modelViewMatrix: 'MODELVIEW_MATRIX', normalMatrix: 'MODELVIEW_NORMAL_MATRIX', cameraPosition: 'CAMERA_POSITION_WORLD',
    isOrthographic: '(PROJECTION_MATRIX[3][3] == 1.0)',
  },
  fragment: {
    modelMatrix: 'MODEL_MATRIX', viewMatrix: 'VIEW_MATRIX', projectionMatrix: 'PROJECTION_MATRIX',
    modelViewMatrix: '(VIEW_MATRIX * MODEL_MATRIX)', normalMatrix: '(mat3(VIEW_MATRIX) * MODEL_NORMAL_MATRIX)',
    cameraPosition: 'CAMERA_POSITION_WORLD', isOrthographic: '(PROJECTION_MATRIX[3][3] == 1.0)',
  },
};

/** The types of three's built-in uniforms. */
const THREE_UNIFORM_TYPES: Readonly<Record<string, string>> = {
  modelMatrix: 'mat4', viewMatrix: 'mat4', projectionMatrix: 'mat4', modelViewMatrix: 'mat4', normalMatrix: 'mat3',
  cameraPosition: 'vec3', isOrthographic: 'bool',
};

/** GLSL built-in variables, as Godot built-ins. */
const GL_BUILTINS: Readonly<Record<string, string>> = {
  gl_Position: 'POSITION', gl_FragCoord: 'vec4(FRAGCOORD.x, VIEWPORT_SIZE.y - FRAGCOORD.y, 1.0 - FRAGCOORD.z, FRAGCOORD.w)', gl_FrontFacing: 'FRONT_FACING', gl_PointSize: 'POINT_SIZE',
  gl_PointCoord: 'POINT_COORD', gl_FragDepth: '_three_depth', gl_VertexID: 'VERTEX_ID', gl_InstanceID: 'INSTANCE_ID',
};

/** Samplers three fills with color data: sRGB textures, decoded to linear on sampling (Godot's `source_color`). */
const COLOR_MAPS = new Set(['map', 'emissiveMap', 'specularColorMap', 'sheenColorMap', 'matcap', 'lightMap']);

const FLOATS: Readonly<Record<string, number>> = { float: 1, vec2: 2, vec3: 3, vec4: 4 };

// ---- one stage ----------------------------------------------------------------------------------------------------

interface Decl {
  readonly name: string;
  readonly type: string;
  /** `[N]` (or ``): the array the declarator has. */
  readonly array: string;
  readonly qualifiers: string[];
  readonly node: Node;
}

interface Stage {
  readonly kind: 'vertex' | 'fragment';
  readonly ast: Node;
  readonly uniforms: Decl[];
  readonly ins: Decl[];
  readonly outs: Decl[];
  readonly consts: Node[];
  readonly structs: Map<string, { node: Node; fields: { name: string; type: string; array: string }[] }>;
  readonly functions: Node[];
  readonly globals: Decl[];
  readonly defines: Map<string, string>;
}

/** Text the translation wrote (an identifier node the generator prints as is); the visitors leave it alone. */
const raw = (text: string, gtype?: string | null): Node => ({ type: 'identifier', identifier: text, whitespace: '', translated: true, ...(gtype ? { gtype } : {}) });
const trim = (s: string): string => s.trim().replace(/\s+/g, ' ');
/** A node turned, in place, into translated text (the visitor applies a replacement only on enter). */
const become = (n: Node, text: string, gtype?: string | null): void => { for (const k of Object.keys(n)) delete n[k]; Object.assign(n, raw(text, gtype)); };

function prepare(glsl: Glsl, source: string, kind: 'vertex' | 'fragment', unsupported: string[]): Stage {
  const defines = new Map<string, string>();
  for (const m of source.matchAll(/^[ \t]*#define[ \t]+(\w+)(?:[ \t]+(.*))?$/gm)) defines.set(m[1]!, (m[2] ?? '').trim());
  const clean = source.replace(/^[ \t]*#version[^\n]*$/m, '').replace(/^[ \t]*#(extension|pragma)[^\n]*$/gm, '');
  const pre = glsl.preprocess(clean, { preserve: {} }).replace(/^[ \t]*#(extension|pragma)[^\n]*$/gm, '');
  const ast = glsl.parse(pre, { quiet: true, stage: kind });
  const stage: Stage = { kind, ast, uniforms: [], ins: [], outs: [], consts: [], structs: new Map(), functions: [], globals: [], defines };
  const typeOf = (spec: Node): string => trim(glsl.generate(spec.specifier.type === 'struct' ? raw(spec.specifier.typeName?.identifier ?? '?') : spec));
  for (const item of ast.program as Node[]) {
    if (item.type === 'function') { stage.functions.push(item); continue; }
    if (item.type !== 'declaration_statement') continue;
    const d = item.declaration as Node;
    if (d.type === 'precision' || d.type === 'qualifier_declarator') continue;
    if (d.type === 'function_header') { unsupported.push(`${kind}: a function prototype (${trim(glsl.generate(d))}): Godot has none; definitions must come before use`); continue; }
    if (d.type === 'interface_declarator') { unsupported.push(`${kind}: an interface block (${trim(glsl.generate(d)).slice(0, 80)}): Godot has no uniform blocks`); continue; }
    if (d.type !== 'declarator_list') continue;
    const spec = d.specified_type.specifier as Node;
    if (spec.specifier.type === 'struct') {
      const s = spec.specifier as Node;
      const name = s.typeName?.identifier as string;
      const fields: { name: string; type: string; array: string }[] = [];
      for (const sd of s.declarations as Node[]) {
        const type = trim(glsl.generate(sd.declaration.specified_type));
        for (const q of sd.declaration.declarations as Node[]) fields.push({ name: q.identifier.identifier, type, array: q.quantifier ? trim(q.quantifier.map((x: Node) => glsl.generate(x)).join('')) : '' });
      }
      stage.structs.set(name, { node: item, fields });
      if (d.declarations.length === 0) continue;
    }
    const qualifiers = ((d.specified_type.qualifiers ?? []) as Node[]).map((q) => (q.type === 'keyword' ? q.token as string : q.type === 'layout_qualifier' ? 'layout' : q.type));
    // an output's location (layout(location = N)): which draw buffer it writes
    for (const q of (d.specified_type.qualifiers ?? []) as Node[]) {
      if (q.type !== 'layout_qualifier') continue;
      const loc = /\blocation\s*=\s*(\d+)/.exec(glsl.generate(q));
      if (loc) qualifiers.push(`location=${loc[1]}`);
    }
    const type = typeOf(spec);
    for (const decl of d.declarations as Node[]) {
      const entry: Decl = { name: decl.identifier.identifier, type, array: decl.quantifier ? trim((decl.quantifier as Node[]).map((x) => glsl.generate(x)).join('')) : '', qualifiers, node: decl };
      if (qualifiers.includes('uniform')) stage.uniforms.push(entry);
      else if (qualifiers.includes('in') || qualifiers.includes('attribute') || (kind === 'fragment' && qualifiers.includes('varying'))) stage.ins.push(entry);
      else if (qualifiers.includes('out') || (kind === 'vertex' && qualifiers.includes('varying'))) stage.outs.push(entry);
      else if (qualifiers.includes('const')) { if (!stage.consts.includes(item)) stage.consts.push(item); }
      else stage.globals.push(entry);
    }
  }
  return stage;
}

/** The global references of a name in a stage: the nodes the parser's scope attributes to the global declaration. */
function globalRefs(stage: Stage, name: string): Set<Node> {
  const binding = stage.ast.scopes?.[0]?.bindings?.[name];
  return new Set<Node>((binding?.references ?? []) as Node[]);
}

/** A postfix chain `base[i].f[j].g` as its base and its operations. */
function chain(n: Node): { base: Node; ops: Node[] } {
  const ops: Node[] = [];
  const flat = (x: Node): void => { if (x.type === 'postfix') { flat(x.expression); flat(x.postfix); } else ops.push(x); };
  flat(n.postfix);
  return { base: n.expression, ops };
}

// ---- the program ------------------------------------------------------------------------------------------------

export function translateProgram(glsl: Glsl, program: ThreeProgram, options: TranslateOptions = {}): GodotShader {
  const notes: string[] = [];
  const unsupported: string[] = [];
  const v = prepare(glsl, program.vertex, 'vertex', unsupported);
  const f = prepare(glsl, program.fragment, 'fragment', unsupported);
  const material = v.defines.get('SHADER_TYPE') ?? f.defines.get('SHADER_TYPE') ?? 'RawShaderMaterial';
  const name = v.defines.get('SHADER_NAME') ?? f.defines.get('SHADER_NAME') ?? '';
  const gen = (n: Node): string => glsl.generate(n);

  // -- attributes: three's names to Godot's arrays and built-ins; the others packed into CUSTOM0..3
  const attributes: { three: string; godot: string; type: string }[] = [];
  const attrExpr = new Map<string, string>();
  const attrType = new Map<string, string>();
  // Godot's places for custom data: per vertex CUSTOM0–3 (16 floats), per instance INSTANCE_CUSTOM (4 floats); what
  // does not fit is a data texture (RGBA32F, one texel per vertex or instance) read with texelFetch by VERTEX_ID or
  // INSTANCE_ID, which has no limit. Which attributes are per instance is the program's GL state (the divisor).
  const instanced = new Set(options.instancedAttributes ?? []);
  const attrTextures: { name: string; uniform: string; by: string }[] = [];
  let slot = 0;
  let instanceSlot = 0;
  const swizzle = (n: number, at: number): string => 'xyzw'.slice(at, at + n);
  const dataTexture = (a: Decl, n: number, by: 'VERTEX_ID' | 'INSTANCE_ID'): string => {
    const uniform = `_three_attr_${a.name}`;
    attrTextures.push({ name: a.name, uniform, by });
    return `texelFetch(${uniform}, ivec2(${by} % 4096, ${by} / 4096), 0).${swizzle(n, 0)}`;
  };
  for (const a of v.ins) {
    let expr: string | null = null;
    // Godot's mesh arrays keep only position and UVs as float32 (rendering_server.cpp): ARRAY_COLOR is 8 bits clamped to
    // [0, 1] (`uint8(CLAMP(c * 255))`), ARRAY_NORMAL and ARRAY_TANGENT octahedral 16 bits, ARRAY_WEIGHTS 16 bits. three's
    // color (often HDR), normal, tangent and skinWeight are float attributes: they go to CUSTOM slots (RGBA_FLOAT,
    // exact) like any custom attribute. The instance colour stays COLOR (a MultiMesh keeps it in float; with no
    // ARRAY_COLOR, COLOR is exactly the instance colour).
    if (a.name === 'position') expr = 'VERTEX';
    else if (a.name === 'uv') expr = 'UV';
    else if (a.name === 'uv1') expr = 'UV2';
    else if (a.name === 'instanceMatrix') expr = '_three_instanceMatrix';
    else if (a.name === 'instanceColor') expr = 'COLOR.rgb';
    // three's skin indices are Godot's bone array (ARRAY_BONES, 16-bit integers: exact); three's skinning code stays
    // in the shader, so the Godot mesh carries no skeleton (else Godot would skin it a second time)
    else if (a.name === 'skinIndex') expr = 'vec4(BONE_INDICES)';
    else {
      const n = FLOATS[a.type];
      if (n === undefined || a.array) { unsupported.push(`attribute ${a.type} ${a.name}${a.array}: only float..vec4 attributes are carried`); continue; }
      if (instanced.has(a.name)) {
        if (instanceSlot + n <= 4) { expr = `INSTANCE_CUSTOM.${swizzle(n, instanceSlot)}`; instanceSlot += n; }
        else expr = dataTexture(a, n, 'INSTANCE_ID');
      } else {
        const start = slot % 4 + n > 4 ? Math.ceil(slot / 4) * 4 : slot;
        if (start + n <= 16) { expr = `CUSTOM${Math.floor(start / 4)}.${swizzle(n, start % 4)}`; slot = start + n; }
        else expr = dataTexture(a, n, 'VERTEX_ID');
      }
    }
    attrExpr.set(a.name, expr);
    attrType.set(a.name, a.type);
    attributes.push({ three: a.name, godot: expr, type: a.type });
  }
  if (attrTextures.length > 0) notes.push(`attributes beyond Godot's slots are data textures (RGBA32F, 4096 texels a row, read by VERTEX_ID or INSTANCE_ID): ${attrTextures.map((t) => `${t.name} → ${t.uniform}`).join(', ')}`);
  if (attributes.some((a) => a.three === 'instanceMatrix')) notes.push('instanceMatrix: a MultiMesh instance transform, already in MODEL_MATRIX/MODELVIEW_MATRIX (Godot multiplies it in before vertex())');
  if (attributes.some((a) => a.godot.startsWith('CUSTOM'))) notes.push('custom attributes are Mesh ARRAY_CUSTOMn arrays (RGBA_FLOAT); an instanced one (InstancedBufferAttribute) is MultiMesh custom data instead (INSTANCE_CUSTOM, 4 floats)');

  // -- uniforms of both stages: three's built-ins become Godot's; struct arrays become one array per field
  const uniforms: UniformInput[] = [];
  const uniformDecls = new Map<string, string>();
  for (const t of attrTextures) {
    uniformDecls.set(t.uniform, `uniform sampler2D ${t.uniform} : filter_nearest, repeat_disable;`);
    uniforms.push({ three: `attribute ${t.name}`, godot: t.uniform, type: 'sampler2D', array: '', note: `RGBA32F texture, texel i = ${t.by === 'VERTEX_ID' ? 'vertex' : 'instance'} i at (i % 4096, i / 4096)` });
  }
  const structUniforms = new Map<string, { struct: string; fields: { name: string; type: string; array: string }[]; array: string }>();
  const shadowSamplers = new Set<string>();
  const bitsSamplers = new Map<string, string>();
  const threeMatrices = options.matrices === 'three';
  const reversed = options.depth === 'reversed';
  // GL orientation (matrices 'three' only): Godot's framebuffer rows are GL's (row 0 the bottom), so FRAGCOORD.y is
  // already gl_FragCoord.y and derivatives along y keep their sign
  const glOrientation = threeMatrices && options.orientation === 'gl';
  const subTexel = 2 ** (options.shadowSubTexelBits ?? 8);
  const builtins: Record<string, string> = { ...GL_BUILTINS, gl_FragCoord: `vec4(FRAGCOORD.x, ${glOrientation ? 'FRAGCOORD.y' : 'VIEWPORT_SIZE.y - FRAGCOORD.y'}, ${reversed ? 'FRAGCOORD.z' : '1.0 - FRAGCOORD.z'}, FRAGCOORD.w)` };
  const threeUniform = (kind: 'vertex' | 'fragment', name: string): string | undefined => {
    const u = THREE_UNIFORMS[kind][name];
    return u === undefined ? undefined : threeMatrices ? `_three_${name}` : u;
  };
  const shadowFns = new Set<string>();
  const vecIndexFns = new Map<string, { vector: string; scalar: string; n: number }>();
  const structs = new Map([...v.structs, ...f.structs]);
  for (const stage of [v, f]) {
    for (const u of stage.uniforms) {
      if (THREE_UNIFORMS[stage.kind][u.name] !== undefined) {
        if (threeMatrices && !uniformDecls.has(u.name)) {
          const type = THREE_UNIFORM_TYPES[u.name] ?? u.type;
          uniformDecls.set(u.name, `uniform ${type} _three_${u.name};`);
          uniforms.push({ three: u.name, godot: `_three_${u.name}`, type, array: '', note: 'three\'s own value (matrices: three)' });
        }
        continue;
      }
      if (uniformDecls.has(u.name)) continue;
      const struct = structs.get(u.type);
      if (struct) {
        if (struct.fields.some((x) => structs.has(x.type))) { unsupported.push(`uniform ${u.type} ${u.name}: a struct holding a struct`); continue; }
        structUniforms.set(u.name, { struct: u.type, fields: struct.fields, array: u.array });
        const lines: string[] = [];
        for (const x of struct.fields) {
          if (x.array) { unsupported.push(`uniform ${u.name}.${x.name}: an array field of a struct array`); continue; }
          const g = `${u.name}__${x.name}`;
          lines.push(`uniform ${x.type} ${g}${u.array};`);
          uniforms.push({ three: `${u.name}${u.array ? '[]' : ''}.${x.name}`, godot: g, type: x.type, array: u.array });
        }
        uniformDecls.set(u.name, lines.join('\n'));
        continue;
      }
      let type = u.type;
      let hint = '';
      let note: string | undefined;
      const bits = /^([ui])sampler(2D|2DArray|3D)$/.exec(type);
      if (bits && options.floatBitsSamplers?.includes(u.name)) {
        type = `sampler${bits[2]}`;
        bitsSamplers.set(u.name, bits[1] === 'u' ? 'floatBitsToUint' : 'floatBitsToInt');
        const g = RESERVED.has(u.name) ? `${u.name}_` : u.name;
        uniformDecls.set(u.name, `uniform ${type} ${g}${u.array}${u.array ? '' : ' : filter_nearest, repeat_disable'};`);
        uniforms.push({ three: u.name, godot: g, type, array: u.array, note: `the integers' 32-bit words in a float texture (RGBA32F), read back with ${bitsSamplers.get(u.name)}` });
        continue;
      }
      if (/\bsampler2DShadow\b/.test(type)) { type = type.replace('sampler2DShadow', 'sampler2D'); shadowSamplers.add(u.name); hint = ' : filter_linear'; note = 'a depth texture compared by hand (Godot has no shadow sampler)'; }
      else if (/\bsampler2D\b/.test(type) && COLOR_MAPS.has(u.name)) { hint = ' : source_color'; note = 'sRGB in three (texture.colorSpace = SRGBColorSpace): decoded on sampling; drop the hint for a linear texture'; }
      const g = RESERVED.has(u.name) ? `${u.name}_` : u.name;
      const given = options.samplerHints?.[u.name];
      if (given) { hint = given.length > 0 ? ` : ${given.join(', ')}` : ''; note = 'sampler state from the three texture'; }
      if (u.array && hint) hint = '';
      uniformDecls.set(u.name, `uniform ${type} ${g}${u.array}${hint};`);
      uniforms.push({ three: u.name, godot: g, type, array: u.array, ...(note ? { note } : {}) });
    }
  }
  if (structUniforms.size > 0) notes.push(`struct uniforms flattened, one array per field: ${[...structUniforms.keys()].join(', ')}`);
  if (shadowSamplers.size > 0) notes.push(`shadow samplers as depth textures compared by hand like the hardware (${options.shadowCompare ?? 'lequal'}; ${options.shadowFilter ?? 'linear'}: four texels compared, then weighted bilinearly): ${[...shadowSamplers].join(', ')}`);

  // -- varyings: the vertex stage's outputs, the fragment stage's inputs
  const varyings = new Map<string, string>();
  for (const o of [...v.outs, ...f.ins]) {
    if (varyings.has(o.name)) continue;
    const interp = o.qualifiers.find((q) => q === 'flat' || q === 'smooth');
    const precision = o.qualifiers.find((q) => q === 'highp' || q === 'mediump' || q === 'lowp');
    varyings.set(o.name, `varying ${interp ? `${interp} ` : ''}${precision ? `${precision} ` : ''}${o.type} ${o.name}${o.array};`);
  }
  // -- the fragment's color output
  // Multiple render targets: a spatial shader has one colour output, so an MRT program is one Godot pass per draw
  // buffer — options.fragmentOutput picks the location this one writes (the others are locals, their values unused:
  // every pass computes the same, so each attachment gets exactly what three's single draw wrote to it).
  const locationOf = (o: Decl, i: number): number => { const q = o.qualifiers.find((x) => x.startsWith('location=')); return q ? Number(q.slice(9)) : i; };
  const byLocation = f.outs.map((o, i) => ({ o, at: locationOf(o, i) })).sort((a, b) => a.at - b.at);
  const wanted = options.fragmentOutput ?? byLocation[0]?.at ?? 0;
  const chosen = byLocation.find((x) => x.at === wanted)?.o ?? null;
  if (options.fragmentOutput !== undefined && !chosen) unsupported.push(`fragment output at location ${options.fragmentOutput}: the program has none`);
  const outputs = chosen ? [chosen, ...f.outs.filter((o) => o !== chosen)] : f.outs;
  const extraOutputs = outputs.slice(1);
  if (extraOutputs.length > 0) notes.push(`multiple render targets: this pass writes location ${wanted} (${chosen?.name}); ${extraOutputs.map((o) => `${o.name} (location ${locationOf(o, f.outs.indexOf(o))})`).join(', ')} are locals here — translate once per location (options.fragmentOutput) for one pass per attachment`);
  const outName = outputs[0]?.name ?? null;
  const opaque = f.defines.has('OPAQUE');

  // -- rewrite both stages' functions
  for (const stage of [v, f]) {
    const refs = new Map<string, Set<Node>>();
    const ref = (name: string): Set<Node> => refs.get(name) ?? refs.set(name, globalRefs(stage, name)).get(name)!;
    const isGlobal = (n: Node, name: string): boolean => ref(name).has(n);
    const shadowParams = new Set<string>();
    for (const fn of stage.functions) {
      for (const p of (fn.prototype.parameters ?? []) as Node[]) {
        if (p.specifier && /sampler2DShadow/.test(gen(p.specifier))) {
          shadowParams.add(p.identifier?.identifier);
          p.specifier.specifier.token = 'sampler2D';
        }
      }
    }
    const allBits = bitsSamplers.size > 0 && stage.uniforms.every((u) => !/^[ui]sampler/.test(u.type) || bitsSamplers.has(u.name));
    const bitsParams = new Map<string, string>();
    for (const fn of stage.functions) {
      for (const p of (fn.prototype.parameters ?? []) as Node[]) {
        const m = p.specifier ? /^([ui])sampler(2D|2DArray|3D)$/.exec(trim(gen(p.specifier))) : null;
        if (!m) continue;
        if (!allBits) { if (bitsSamplers.size > 0) unsupported.push(`function parameter ${p.identifier?.identifier}: an integer sampler, while only some integer textures are float bits`); continue; }
        bitsParams.set(p.identifier?.identifier, m[1] === 'u' ? 'floatBitsToUint' : 'floatBitsToInt');
        p.specifier.specifier.token = `sampler${m[2]}`;
      }
    }
    const bitsOf = (x: Node): string | undefined => {
      const base = x.type === 'postfix' ? chain(x).base : x;
      const id = base.type === 'identifier' ? base.identifier as string : null;
      return id ? bitsSamplers.get(id) ?? bitsParams.get(id) : undefined;
    };
    const isShadow = (x: Node): boolean => {
      const base = x.type === 'postfix' ? chain(x).base : x;
      const id = base.type === 'identifier' ? base.identifier as string : base.type === 'type_specifier' ? base.specifier?.identifier : null;
      return !!id && (shadowSamplers.has(id) || shadowParams.has(id));
    };
    const args = (call: Node): Node[] => (call.args as Node[]).filter((a) => !(a.type === 'literal' && a.literal === ','));
    // Godot forbids a local or a parameter named as a global (GLSL lets it shadow): such a local is renamed
    const globalNames = new Set<string>([...uniformDecls.keys(), ...structUniforms.keys(), ...varyings.keys(),
      ...stage.functions.map((x) => x.prototype.header.name.identifier as string), ...stage.globals.map((g) => g.name)]);
    for (const scope of (stage.ast.scopes ?? []).slice(1) as Node[]) {
      for (const [bname, binding] of Object.entries(scope.bindings ?? {}) as [string, Node][]) {
        if (!globalNames.has(bname)) continue;
        for (const r of (binding.references ?? []) as Node[]) if (r.type === 'identifier') { r.identifier = `${bname}_l`; r.translated = true; }
        const d = binding.declaration as Node | undefined;
        if (d?.identifier?.type === 'identifier') { d.identifier.identifier = `${bname}_l`; d.identifier.translated = true; }
      }
    }
    for (const fn of stage.functions) {
      // a `const` parameter is a constant to Godot, which then refuses to pass it on to an `in` parameter
      for (const prm of (fn.prototype.parameters ?? []) as Node[]) {
        if (Array.isArray(prm.qualifier)) prm.qualifier = prm.qualifier.filter((q: Node) => q.token !== 'const');
      }
    }
    const GLSL1_TEXTURE: Readonly<Record<string, string>> = {
      texture2D: 'texture', textureCube: 'texture', texture2DProj: 'textureProj', texture2DLodEXT: 'textureLod',
      textureCubeLodEXT: 'textureLod', texture2DProjLodEXT: 'textureProjLod', texture2DGradEXT: 'textureGrad',
      textureCubeGradEXT: 'textureGrad', texture2DLod: 'textureLod', textureCubeLod: 'textureLod',
    };
    // types for Godot's integer operators (no vector–scalar broadcast there): globals, then each function's scope
    const globalTypes = new Map<string, string>(Object.entries(BUILTIN_TYPES));
    for (const st of [v, f]) for (const u of st.uniforms) globalTypes.set(u.name, `${u.type.replace('sampler2DShadow', 'sampler2D')}${u.array.replace(/\s+/g, '')}`);
    for (const st of [v, f]) for (const o of [...st.outs, ...st.ins, ...st.globals]) globalTypes.set(o.name, `${o.type}${o.array.replace(/\s+/g, '')}`);
    for (const [aname, atype] of attrType) globalTypes.set(aname, atype);
    for (const c of stage.consts) for (const d of c.declaration.declarations as Node[]) globalTypes.set(d.identifier.identifier, trim(gen(c.declaration.specified_type.specifier)));
    const fnTypes = new Map<string, string>();
    for (const st of [v, f]) for (const fn of st.functions) fnTypes.set(fn.prototype.header.name.identifier, trim(gen(fn.prototype.header.returnType)));
    const structTypes = new Map<string, Map<string, string>>([...structs].map(([k, sd]) => [k, new Map(sd.fields.map((x) => [x.name, x.type]))]));
    const INTEGER_OPS = new Set(['&', '|', '^', '%', '<<', '>>']);
    const stageRefs = referenceTypes(stage.ast, gen);
    let env: TypeEnv = { names: globalTypes, functions: fnTypes, structs: structTypes, refs: stageRefs };
    /** `a op b` with one vector side and one scalar side: the scalar made the vector's type. */
    const broadcast = (node: Node, leftKey: string, rightKey: string): void => {
      const l = typeOf(node[leftKey], env);
      const r = typeOf(node[rightKey], env);
      if (isVector(l) && isScalar(r)) node[rightKey] = raw(`${l}(${trim(gen(node[rightKey]))})`, l);
      else if (isScalar(l) && isVector(r) && leftKey !== 'left_assign') node[leftKey] = raw(`${r}(${trim(gen(node[leftKey]))})`, r);
    };
    for (const fn of [...stage.functions, ...stage.consts]) { // global consts too (a mat2 of four scalars)
      env = { names: fn.type === 'function' ? functionScope(fn, gen, globalTypes) : globalTypes, functions: fnTypes, structs: structTypes, refs: stageRefs };
      glsl.visit(fn, {
        binary: { exit: (p) => { if (INTEGER_OPS.has(p.node.operator?.literal)) broadcast(p.node, 'left', 'right'); } },
        // Godot refuses a swizzled varying write (`v.xyz = ...`) inside a bare block, not inside an if's block
        // (out/repro: g1 fails, f passes): a bare block of the vertex stage is the block of an `if (true)`
        compound_statement: {
          exit: (p) => {
            if (stage.kind !== 'vertex' || p.parent?.type !== 'compound_statement' || p.node.translated) return;
            become(p.node, `if (true) ${trim(gen(p.node))}`);
          },
        },
        assignment: {
          exit: (p) => {
            const op = (p.node.operator?.literal ?? '') as string;
            if (!['&=', '|=', '^=', '%=', '<<=', '>>='].includes(op)) return;
            const l = typeOf(p.node.left, env);
            const r = typeOf(p.node.right, env);
            if (isVector(l) && isScalar(r)) p.node.right = raw(`${l}(${trim(gen(p.node.right))})`, l);
          },
        },
        // on exit: the arguments are already translated when their text is taken
        function_call: {
          exit: (p) => {
            const id = p.node.identifier;
            if (id.type === 'identifier' && GLSL1_TEXTURE[id.identifier]) { id.identifier = GLSL1_TEXTURE[id.identifier]; id.translated = true; }
            const callee = id.type === 'identifier' ? id.identifier : id.type === 'type_specifier' ? id.specifier?.identifier ?? id.specifier?.token : null;
            // float(v), int(v)... of a vector: GLSL takes the first component; Godot only converts scalars
            if (/^(float|int|uint|bool)$/.test(callee ?? '') && args(p.node).length === 1 && isVector(typeOf(args(p.node)[0], env))) {
              become(p.node, `${callee}((${trim(gen(args(p.node)[0]!))}).x)`, callee);
              return;
            }
            // a vector built from components of another kind (ivec4(float), vec3(int, ...)): GLSL converts each,
            // Godot converts none implicitly: each such argument cast to the vector's kind, scalar or vector
            const vm = /^([biu]?)vec([234])$/.exec(callee ?? '');
            if (vm) {
              const kind = vm[1] === 'b' ? 'bool' : vm[1] === 'i' ? 'int' : vm[1] === 'u' ? 'uint' : 'float';
              const prefix = vm[1] ?? '';
              let changed = false;
              const parts = args(p.node).map((a) => {
                const t = typeOf(a, env);
                const text = trim(gen(a));
                if (isScalar(t) && t !== kind) { changed = true; return `${kind}(${text})`; }
                const av = t ? /^([biu]?)vec([234])$/.exec(t) : null;
                if (av && (av[1] ?? '') !== prefix) { changed = true; return `${prefix}vec${av[2]}(${text})`; }
                return text;
              });
              if (changed) { become(p.node, `${callee}(${parts.join(', ')})`, callee); return; }
            }
            // matN of N×N arguments: all scalars (each argument gives one component at least); Godot takes columns
            const m = /^mat([234])$/.exec(callee ?? '');
            if (m) {
              const n = Number(m[1]);
              const a = args(p.node);
              if (a.length === n * n) {
                const cols = Array.from({ length: n }, (_, c) => `vec${n}(${a.slice(c * n, c * n + n).map((x) => trim(gen(x))).join(', ')})`);
                become(p.node, (`mat${n}(${cols.join(', ')})`));
                return;
              }
            }
            // screen-space derivatives along y: GL's window y points up, Godot's down (FRAGCOORD's origin is the top
            // left), so dFdy changes sign (three's flat shading, bump maps and normal maps without tangents use it;
            // the frame replay's flat-shaded draws measured it). dFdx and fwidth are the same in both.
            if (callee && /^dFdy(Coarse|Fine)?$/.test(callee) && !p.node.translated && !glOrientation) {
              become(p.node, `(-${trim(gen(p.node))})`);
              return;
            }
            if (callee && /^(texture|textureLod|textureProj|texelFetch|textureGrad|textureOffset|texelFetchOffset|textureLodOffset)$/.test(callee) && args(p.node).length >= 2 && bitsOf(args(p.node)[0]!)) {
              become(p.node, `${bitsOf(args(p.node)[0]!)}(${trim(gen(p.node))})`);
              return;
            }
            if ((callee === 'texture' || callee === 'textureLod' || callee === 'textureProj') && args(p.node).length >= 2 && isShadow(args(p.node)[0]!)) {
              const [s, c] = args(p.node);
              const st = trim(gen(s!));
              const ct = `(${trim(gen(c!))})`;
              const fn = `_three_shadow_${options.shadowCompare ?? 'lequal'}_${options.shadowFilter ?? 'linear'}`;
              shadowFns.add(fn);
              become(p.node, (callee === 'textureProj'
                ? `${fn}(${st}, ${ct}.xy / ${ct}.w, ${ct}.z / ${ct}.w)`
                : `${fn}(${st}, ${ct}.xy, ${ct}.z)`));
            }
          },
        },
        postfix: {
          exit: (p) => {
            if (p.node.translated) return;
            const { base, ops } = chain(p.node);
            // a vector indexed by a value known only at run time (v[i]): glslang makes it OpVectorExtractDynamic,
            // which Godot 4.7's SPIR-V pass refuses (and the process ends). Read: a helper choosing the component by
            // comparisons (exact for every value, NaN and Inf included; the index evaluated once). Written: refused.
            const q0 = ops[0];
            if (ops.length === 1 && q0 && q0.type === 'quantifier') {
              const bt = typeOf(base, env);
              const vm = bt ? /^([biu]?)vec([234])$/.exec(bt) : null;
              const idx = trim(gen(q0.expression));
              if (bt && vm && !/^\d+u?$/.test(idx)) {
                const parent = p.parent as Node | undefined;
                const written = !!parent && ((parent.type === 'assignment' && parent.left === p.node) || ((parent.type === 'postfix' || parent.type === 'unary') && /\+\+|--/.test(trim(gen(parent)))));
                if (written) { unsupported.push(`a write to a vector component chosen at run time (${trim(gen(p.node))}): Godot's SPIR-V pass refuses OpVectorInsertDynamic`); return; }
                const scalar = ({ '': 'float', i: 'int', u: 'uint', b: 'bool' } as Record<string, string>)[vm[1] ?? '']!;
                const fn = `_three_at_${bt}`;
                vecIndexFns.set(fn, { vector: bt, scalar, n: Number(vm[2]) });
                become(p.node, `${fn}(${trim(gen(base))}, int(${idx}))`, scalar);
                return;
              }
            }
            if (base.type !== 'identifier' || !structUniforms.has(base.identifier) || !isGlobal(base, base.identifier)) return;
            const s = structUniforms.get(base.identifier)!;
            const index = ops[0]?.type === 'quantifier' ? `[${trim(gen(ops[0].expression))}]` : '';
            const rest = ops.slice(index ? 1 : 0);
            const field = rest[0]?.type === 'field_selection' ? rest[0].selection.identifier ?? rest[0].selection.literal : null;
            if (field) {
              const after = rest.slice(1).map((o) => gen(o)).join('');
              const ftype = s.fields.find((x) => x.name === field)?.type ?? null;
              become(p.node, `${base.identifier}__${field}${index}${after}`, after ? null : ftype);
            } else {
              become(p.node, (`${s.struct}(${s.fields.map((x) => `${base.identifier}__${x.name}${index}`).join(', ')})${rest.map((o) => gen(o)).join('')}`));
            }
          },
        },
        // a user function's name at a call is a type name: the same rename as its definition
        type_name: { enter: (p) => { if (RESERVED.has(p.node.identifier)) p.node.identifier = `${p.node.identifier}_`; } },
        identifier: {
          enter: (p) => {
            const n = p.node;
            if (n.translated) return;
            const name = n.identifier as string;
            // a GLSL 1 fragment writes gl_FragColor itself (three's prefix makes it an output only for GLSL 3)
            if (stage.kind === 'fragment' && name === 'gl_FragColor') { n.identifier = '_three_out'; n.translated = true; return; }
            if (builtins[name]) { n.identifier = builtins[name]; n.translated = true; return; }
            if (stage.kind === 'vertex' && attrExpr.has(name) && isGlobal(n, name)) { p.replaceWith(raw(attrExpr.get(name)!, attrType.get(name))); return; }
            const u = threeUniform(stage.kind, name);
            if (u !== undefined && isGlobal(n, name)) { p.replaceWith(raw(u, THREE_UNIFORM_TYPES[name] ?? null)); return; }
            if (stage.kind === 'fragment' && name === outName && isGlobal(n, name)) { n.identifier = '_three_out'; n.translated = true; return; }
            // a struct uniform inside a member chain is the chain's (postfix, on exit)
            if (structUniforms.has(name) && isGlobal(n, name) && p.parent?.type === 'postfix' && p.parent.expression === n) return;
            if (structUniforms.has(name) && isGlobal(n, name)) {
              const s = structUniforms.get(name)!;
              p.replaceWith(raw(`${s.struct}(${s.fields.map((x) => `${name}__${x.name}`).join(', ')})`));
              return;
            }
            if (RESERVED.has(name)) n.identifier = `${name}_`;
          },
        },
      });
    }
    // three's tone mapping stays three's code (Godot's ACES/AgX are other curves, and a pass may call them itself:
    // the frame replay of the 4.0's OutputPass measured it); only the sRGB output transfer is Godot's own after the
    // fragment in 'display' output (the identity here). In 'raw' output nothing is Godot's.
    if (options.output !== 'raw') {
      for (const fn of stage.functions) {
        const fname = fn.prototype.header.name.identifier as string;
        if (fname === 'linearToOutputTexel') {
          const param = fn.prototype.parameters?.[0]?.identifier?.identifier;
          if (param) { fn.body.statements = [raw(`return ${param};`)]; }
        }
      }
    }
  }
  notes.push(options.output === 'raw'
    ? 'three\'s tone mapping and output transfer are its own code here: the Godot viewport stores the values unencoded (HDR 2D), Environment.tonemap_mode linear'
    : 'three\'s tone mapping is its own code here (Environment.tonemap_mode linear, exposure 1); its sRGB output transfer is Godot\'s encoding after the fragment');

  // -- helpers: every function but the vertex main, in text. What a helper reads of the processor's world (a Godot
  // built-in, or a mutable global of three's, which Godot does not have) becomes a parameter: `in`, or `inout` when
  // it is written. The fragment main is a helper too (`_three_fragment`): Godot allows no `return` in fragment().
  interface Fn { name: string; header: string; body: string; text: string; stage: 'vertex' | 'fragment' }
  const fns: Fn[] = [];
  let vertexMain = '';
  // Godot lets vertex() alone assign a varying, and takes none as a function argument: the vertex main stays inline,
  // and a vertex helper that touches a varying is written inline at its calls (its parameters copied in and out)
  const varyingNames = [...varyings.keys()];
  const touchesVarying = (text: string): boolean => varyingNames.some((n) => new RegExp(`\\b${n}\\b`).test(text));
  const inline = new Map<string, { params: { qual: string; type: string; name: string }[]; body: string }>();
  for (const stage of [v, f]) {
    for (const fn of stage.functions) {
      const fname = fn.prototype.header.name.identifier as string;
      const text = gen(fn).trim();
      const brace = text.indexOf('{');
      if (fname === 'main' && stage.kind === 'vertex') { vertexMain = text.slice(brace); continue; }
      if (stage.kind === 'vertex' && fname !== 'main' && touchesVarying(text.slice(brace))) {
        const params = ((fn.prototype.parameters ?? []) as Node[]).filter((p) => p.identifier).map((p) => ({
          qual: ((p.qualifier ?? []) as Node[]).map((q) => q.token).find((q: string) => q === 'out' || q === 'inout') ?? 'in',
          type: trim(gen(p.specifier)), name: p.identifier.identifier as string,
        }));
        if (trim(gen(fn.prototype.header.returnType)) !== 'void' || /\breturn\b/.test(text.slice(brace))) {
          unsupported.push(`vertex function ${fname} touches a varying and returns: it cannot be written inline`);
        } else { inline.set(fname, { params, body: text.slice(brace + 1, text.lastIndexOf('}')) }); continue; }
      }
      // the fragment main is a helper; the output (_three_out) reaches it, like every function that reads or writes it, as
      // part of the stage's context (GLSL's output is a global of the stage: any function may write it)
      const header = fname === 'main' ? 'void _three_fragment()' : text.slice(0, brace).trim();
      fns.push({ name: fname === 'main' ? '_three_fragment' : fname, header, body: text.slice(brace), text: fname === 'main' ? `${header} ${text.slice(brace)}` : text, stage: stage.kind });
    }
  }
  /** Each statement call `f(a, b);` of an inline helper replaced by its body in a block, parameters as copies. */
  let inlineCount = 0;
  const writeInline = (text: string): string => {
    let out = text;
    for (let pass = 0; pass < 8; pass++) {
      let changed = false;
      for (const [fname, h] of inline) {
        const re = new RegExp(`\\b${fname}\\s*\\(`, 'g');
        let result = '';
        let i = 0;
        for (let m = re.exec(out); m; m = re.exec(out)) {
          let depth = 1;
          let j = m.index + m[0].length;
          for (; j < out.length && depth > 0; j++) { if (out[j] === '(') depth++; else if (out[j] === ')') depth--; }
          const semi = /^\s*;/.exec(out.slice(j));
          if (!semi) continue; // not a statement call
          const argText = out.slice(m.index + m[0].length, j - 1);
          const argsList: string[] = [];
          let d = 0; let start = 0;
          for (let k = 0; k < argText.length; k++) { const ch = argText[k]; if (ch === '(' || ch === '[') d++; else if (ch === ')' || ch === ']') d--; else if (ch === ',' && d === 0) { argsList.push(argText.slice(start, k).trim()); start = k + 1; } }
          if (argText.trim()) argsList.push(argText.slice(start).trim());
          const k = inlineCount++;
          let body = h.body;
          for (const p of h.params) body = body.replace(new RegExp(`\\b${p.name}\\b`, 'g'), `${p.name}_i${k}`);
          const copyIn = h.params.map((p, x) => `${p.type} ${p.name}_i${k}${p.qual === 'out' ? '' : ` = ${argsList[x]}`};`).join(' ');
          const copyOut = h.params.map((p, x) => (p.qual === 'in' ? '' : `${argsList[x]} = ${p.name}_i${k};`)).join(' ');
          result += out.slice(i, m.index) + `if (true) { ${copyIn} ${body} ${copyOut} }`;
          i = j + semi[0].length;
          re.lastIndex = i;
          changed = true;
        }
        out = result + out.slice(i);
      }
      if (!changed) break;
    }
    return out;
  };
  if (inline.size > 0) notes.push(`vertex functions that assign varyings written inline at their calls: ${[...inline.keys()].join(', ')}`);
  // the same helper in both stages is written once; a different one under the same signature is the fragment's own
  const byHeader = new Map<string, Fn>();
  const kept: Fn[] = [];
  const renamedInFragment = new Map<string, string>();
  for (const fn of fns) {
    const known = byHeader.get(fn.header);
    if (!known) { byHeader.set(fn.header, fn); kept.push(fn); continue; }
    if (known.text === fn.text) continue;
    const renamed = `${fn.name}_fs`;
    renamedInFragment.set(fn.name, renamed);
    kept.push({ ...fn, name: renamed, header: fn.header.replace(new RegExp(`\\b${fn.name}\\s*\\(`), `${renamed}(`), text: fn.text.replace(new RegExp(`\\b${fn.name}\\s*\\(`), `${renamed}(`) });
  }
  const renameCalls = (text: string, stage: 'vertex' | 'fragment'): string => {
    if (stage !== 'fragment') return text;
    let t = text;
    for (const [from, to] of renamedInFragment) t = t.replace(new RegExp(`\\b${from}\\s*\\(`, 'g'), `${to}(`);
    return t;
  };
  for (const fn of kept) fn.body = renameCalls(fn.body, fn.stage);
  // the processor's world: Godot built-ins, and each stage's mutable globals (declared in its processor function)
  const contextType = new Map<string, string>(Object.entries(BUILTIN_TYPES));
  const globalsOf = { vertex: v.globals, fragment: f.globals };
  for (const g of [...v.globals, ...f.globals]) contextType.set(g.name, `${g.type}`);
  // a fragment helper that reads a varying receives it as a parameter (fragment() passes it)
  const fragmentVaryings = new Set<string>();
  for (const o of [...v.outs, ...f.ins]) if (!contextType.has(o.name)) { contextType.set(o.name, `${o.type}`); fragmentVaryings.add(o.name); }
  contextType.set('_three_depth', 'float');
  contextType.set('_three_out', 'vec4');
  for (const o of extraOutputs) contextType.set(o.name, o.type);
  contextType.set('_three_instanceMatrix', 'mat4');
  const contextIn = (text: string, stage: 'vertex' | 'fragment'): string[] => [...contextType.keys()]
    .filter((b) => (stage === 'fragment' || !fragmentVaryings.has(b)) && new RegExp(`\\b${b}\\b`).test(text));
  const names = new Set(kept.map((x) => x.name));
  const callees = (text: string): string[] => [...names].filter((n) => new RegExp(`\\b${n}\\s*\\(`).test(text));
  const needs = new Map<string, Set<string>>(kept.map((x) => [x.name, new Set(contextIn(x.body, x.stage))]));
  for (let changed = true; changed;) {
    changed = false;
    for (const x of kept) {
      const mine = needs.get(x.name)!;
      for (const c of callees(x.body)) for (const b of needs.get(c) ?? []) if (!mine.has(b)) { mine.add(b); changed = true; }
    }
  }
  const writes = (body: string, name: string): boolean => new RegExp(`\\b${name}\\b(\\s*\\[[^\\]]*\\])?(\\.[a-z]+)?\\s*(=[^=]|\\+=|-=|\\*=|/=|\\+\\+|--)`).test(body);
  /** Every call of a helper that needs its world gets it as trailing arguments (`prefix` + name). */
  const passContext = (text: string, prefix: string): string => {
    let out = text;
    for (const [fname, set] of needs) {
      if (set.size === 0) continue;
      const extra = [...set].sort().map((b) => `${prefix}${b}`).join(', ');
      let i = 0;
      const re = new RegExp(`\\b${fname}\\s*\\(`, 'g');
      let result = '';
      for (let m = re.exec(out); m; m = re.exec(out)) {
        // not the helper's own header
        if (/\b(void|float|int|uint|bool|vec[234]|ivec[234]|uvec[234]|bvec[234]|mat[234]|\w+)\s+$/.test(out.slice(Math.max(0, m.index - 40), m.index)) && out.slice(0, m.index).trim().length === 0) continue;
        let depth = 1;
        let j = m.index + m[0].length;
        for (; j < out.length && depth > 0; j++) { if (out[j] === '(') depth++; else if (out[j] === ')') depth--; }
        const inner = out.slice(m.index + m[0].length, j - 1);
        result += out.slice(i, m.index) + `${m[0]}${inner.trim() ? `${inner}, ` : ''}${extra})`;
        i = j;
        re.lastIndex = j;
      }
      out = result + out.slice(i);
    }
    return out;
  };
  const helperText = kept.map((x) => {
    const set = [...(needs.get(x.name) ?? [])].sort();
    let body = passContext(x.body, '');
    let header = x.header;
    if (set.length > 0) {
      for (const b of set) body = body.replace(new RegExp(`\\b${b}\\b`, 'g'), `_g_${b}`);
      const params = set.map((b) => {
        const g = [...v.globals, ...f.globals, ...v.outs, ...f.ins].find((x2) => x2.name === b);
        return `${writes(body, `_g_${b}`) ? 'inout' : 'in'} ${contextType.get(b)} _g_${b}${g?.array ?? ''}`;
      }).join(', ');
      header = header.replace(/\)\s*$/, () => (/\(\s*(void\s*)?\)\s*$/.test(header) ? `${params})` : `, ${params})`));
      header = header.replace(/\(\s*void\s*,/, '(');
    }
    return `${header} ${body}`;
  });

  // -- the processor functions: each stage's globals as locals; the vertex main inline (it assigns the varyings)
  const localsOf = (stage: 'vertex' | 'fragment'): string => globalsOf[stage].map((g) => {
    const init = g.node.initializer ? ` = ${trim(gen(g.node.initializer))}` : '';
    return `\n\t${g.type} ${g.name}${g.array}${init};`;
  }).join('');
  let vbody = vertexMain ? passContext(writeInline(vertexMain), '').slice(1, -1) : '';
  // an early return of the vertex main: Godot's vertex() has none; outside loops it is the break of a one-pass loop
  if (/\breturn\b/.test(vbody)) {
    if (/\b(for|while|do|switch)\b/.test(vbody)) unsupported.push('the vertex main returns early inside a loop: Godot\'s vertex() has no return');
    else { vbody = `\n\tdo {${vbody.replace(/\breturn\s*;/g, 'break;')}\n\t} while (false);`; notes.push('the vertex main\'s early return is the break of a one-pass loop'); }
  }
  vbody = localsOf('vertex') + (attrExpr.get('instanceMatrix') ? (threeMatrices ? '\n\tmat4 _three_instanceMatrix = MODEL_MATRIX;' : '\n\tmat4 _three_instanceMatrix = mat4(1.0);') : '') + vbody;
  // three's clip position, Godot's: Vulkan's y points down (Godot's own projection flips it), and its depth is zero to
  // one, reversed (near at 1) — three's reversed depth already; its standard depth (near at -1) remapped
  if (threeMatrices) {
    vbody += `\n\tPOSITION = vec4(POSITION.x, ${glOrientation ? 'POSITION.y' : '-POSITION.y'}, ${reversed ? 'POSITION.z' : '(POSITION.w - POSITION.z) * 0.5'}, POSITION.w);`;
    if (glOrientation) notes.push('orientation gl: three\'s clip y kept (Godot\'s framebuffer rows are GL\'s: row 0 the bottom); triangles keep three\'s index order');
    notes.push(`matrices: three's own (uniforms _three_*); its clip position converted to Godot's (y down, depth ${reversed ? 'reversed already' : 'remapped to reversed zero to one'})`);
  }
  // three's canvas shows what the program wrote; Godot's ALBEDO is linear and encoded to sRGB after the fragment.
  // A program that encodes its output (linearToOutputTexel, now the identity) wrote linear light: ALBEDO as is. One
  // that writes its color raw (a ShaderMaterial without <colorspace_fragment>) wrote display values: decoded first
  // (the sRGB EOTF three itself uses), so that Godot's encoding gives back the same value.
  const fragMain = f.functions.find((x) => x.prototype.header.name.identifier === 'main');
  const encodes = fragMain ? /\blinearToOutputTexel\s*\(/.test(gen(fragMain)) : false;
  const color = encodes || options.output === 'raw' ? '_three_out.rgb'
    : 'mix(pow(_three_out.rgb * 0.9478672986 + vec3(0.0521327014), vec3(2.4)), _three_out.rgb * 0.0773993808, vec3(lessThanEqual(_three_out.rgb, vec3(0.04045))))';
  if (options.output === 'raw') notes.push('output raw: ALBEDO is what the program wrote (the target stores it unencoded)');
  else if (!encodes) notes.push('the program writes display values (no linearToOutputTexel): ALBEDO is their sRGB decoding, Godot encodes them back');
  // the caller's choice wins: three's OPAQUE define says what three's chunks do with the alpha, not what a pass
  // writes (a post-processing pass may write its own alpha with blending off: measured in the frame replay)
  const alphaOutput = options.alphaOutput ?? (opaque ? 'none' : 'blend');
  const alphaPass = options.channels === 'alpha';
  if (alphaPass) notes.push('channels alpha: the program\'s alpha in ALBEDO (opaque pipeline), the companion of an rgb pass with alphaOutput none');
  const emit = alphaPass ? 'ALBEDO = vec3(_three_out.a);' : `ALBEDO = ${color};${alphaOutput === 'none' ? '' : ' ALPHA = _three_out.a;'}`;
  if (alphaOutput === 'replace') notes.push('alpha replace: ALPHA with blend_premul_alpha and the colour as written — colour and alpha stored exactly over a cleared target (three\'s blending off)');
  if (alphaOutput === 'none' && !opaque) notes.push('alpha none: ALPHA not written (Godot\'s opaque pipeline: the colour exact, the stored alpha 1)');
  const fcall = passContext('_three_fragment()', '');
  const writesDepth = kept.some((x) => x.stage === 'fragment' && /\b_three_depth\b/.test(x.body));
  const depthLocal = writesDepth ? `\n\tfloat _three_depth = ${reversed ? 'FRAGCOORD.z' : '1.0 - FRAGCOORD.z'};` : '';
  const depthEmit = writesDepth ? (reversed ? ' DEPTH = _three_depth;' : ' DEPTH = 1.0 - _three_depth;') : '';
  if (writesDepth) notes.push('gl_FragDepth is three\'s depth (0 near); Godot\'s DEPTH is reversed Z (1 near): DEPTH = 1 - depth');
  if (!opaque) notes.push('ALPHA is written (three\'s material is not opaque): Godot draws it in the transparent pipeline');
  if (v.globals.length + f.globals.length > 0) notes.push(`three's mutable globals are locals of the processor, passed to the helpers that use them: ${[...v.globals, ...f.globals].map((g) => g.name).join(', ')}`);

  // 'replace' takes the blend mode itself (premultiplied: src + dst·(1 − a), the colour as written)
  const modes = alphaOutput === 'replace' ? [...(options.renderModes ?? []).filter((m) => !m.startsWith('blend_')), 'blend_premul_alpha'] : [...(options.renderModes ?? [])];
  const header = ['shader_type spatial;', `render_mode ${['unshaded', ...modes].join(', ')};`];
  const structText: string[] = [];
  const seen = new Set<string>();
  for (const [sname, s] of structs) {
    if (seen.has(sname)) continue;
    seen.add(sname);
    structText.push(`struct ${sname} { ${s.fields.map((x) => `${x.type} ${x.name}${x.array};`).join(' ')} };`);
  }
  const constText = [...new Set([...v.consts, ...f.consts].map((c) => trim(gen(c))))];
  // Godot allows 32 varyings: beyond that, float/vec2/vec3 varyings of the same interpolation share vec4s
  let varyingDecls = [...varyings.values()];
  // (after packing, below) each varying assigned whole at the top of vertex(): Godot sets a varying's stage only on
  // an assignment to the name itself (`v = ...`), not to a swizzle (`v.xyz = ...`), and refuses a stageless varying
  // once vertex() calls any function (shader_language.cpp _validate_varying_assign/_validate_restricted_func). The
  // components three writes are written after; the others (undefined in GLSL) are zero.
  let varyingInit = '';
  const packed = new Map<string, string>();
  // Godot counts its own varyings in the same 32 locations: small ones are packed whenever there are many (exact:
  // each component interpolates on its own)
  if (varyings.size > 8) {
    const all = [...v.outs, ...f.ins].filter((o, i, a) => a.findIndex((x) => x.name === o.name) === i);
    const small = all.filter((o) => !o.array && FLOATS[o.type] !== undefined && FLOATS[o.type]! < 4 && !o.qualifiers.includes('flat'));
    small.sort((a, b) => FLOATS[b.type]! - FLOATS[a.type]!);
    const bins: { used: number; members: { name: string; n: number }[] }[] = [];
    for (const o of small) {
      const n = FLOATS[o.type]!;
      let bin = bins.find((b) => b.used + n <= 4);
      if (!bin) { bin = { used: 0, members: [] }; bins.push(bin); }
      bin.members.push({ name: o.name, n });
      bin.used += n;
    }
    const merged = bins.filter((b) => b.members.length > 1);
    merged.forEach((b, k) => {
      let at = 0;
      for (const m of b.members) { packed.set(m.name, `_three_pack${k}.${'xyzw'.slice(at, at + m.n)}`); at += m.n; }
    });
    varyingDecls = [...varyings].filter(([n]) => !packed.has(n)).map(([, d]) => d).concat(merged.map((_, k) => `varying vec4 _three_pack${k};`));
    if (packed.size > 0) notes.push(`${varyings.size} varyings (Godot has 32 locations, its own included): ${packed.size} small ones packed into ${merged.length} vec4s`);
    if (varyings.size - packed.size + merged.length > 32) unsupported.push(`${varyings.size} varyings: packing leaves ${varyings.size - packed.size + merged.length}, more than Godot's 32`);
  }
  const zeroOf = (type: string): string | null => {
    if (/^(float|vec[234]|mat[234])$/.test(type)) return `${type}(0.0)`;
    if (/^(int|ivec[234])$/.test(type)) return `${type}(0)`;
    if (/^(uint|uvec[234])$/.test(type)) return `${type}(0u)`;
    return null;
  };
  for (const d of varyingDecls) {
    const m = /^varying\s+(?:(?:flat|smooth)\s+)?(?:(?:highp|mediump|lowp)\s+)?(\w+)\s+(\w+)\s*;$/.exec(d.trim());
    const z = m ? zeroOf(m[1]!) : null;
    if (m && z) varyingInit += `\n\t${m[2]} = ${z};`;
  }
  const pack = (text: string): string => { let t = text; for (const [n, e] of packed) t = t.replace(new RegExp(`\\b${n}\\b`, 'g'), e); return t; };
  const code = [
    `// three.js ${material}${name ? ` "${name}"` : ''}, translated by the converter (src/render/godot-shader.ts)`,
    ...header, '',
    ...structText,
    ...constText,
    ...uniformDecls.values(),
    ...varyingDecls, '',
    ...[...vecIndexFns].sort().map(([fn, x]) => `${x.scalar} ${fn}(${x.vector} v, int i) {\n\treturn ${['x', 'y', 'z'].slice(0, x.n - 1).map((c, k) => `i == ${k} ? v.${c} : `).join('')}v.${'xyzw'[x.n - 1]};\n}`),
    ...[...shadowFns].sort().map((fn) => {
      const [, , , cmp, filter] = fn.split('_');
      const test = { lequal: 'step(r, d)', gequal: 'step(d, r)', less: '(1.0 - step(d, r))', greater: '(1.0 - step(r, d))' }[cmp!]!;
      const one = (e: string): string => test.replace(/\bd\b/g, `(${e})`);
      const body = filter === 'nearest'
        ? `ivec2 s = textureSize(t, 0); ivec2 i = clamp(ivec2(floor(uv * vec2(s))), ivec2(0), s - 1); return ${one('texelFetch(t, i, 0).r')};`
        // the texel coordinate in the hardware's fixed point (subTexelPrecisionBits): the texels and the bilinear
        // weights the hardware's PCF uses
        : `ivec2 s = textureSize(t, 0); vec2 p = floor((uv * vec2(s) - 0.5) * ${subTexel.toFixed(1)}) / ${subTexel.toFixed(1)}; vec2 w = fract(p); ivec2 i = ivec2(floor(p)); ivec2 m = s - 1;\n\tfloat a = ${one('texelFetch(t, clamp(i, ivec2(0), m), 0).r')}; float b = ${one('texelFetch(t, clamp(i + ivec2(1, 0), ivec2(0), m), 0).r')};\n\tfloat c = ${one('texelFetch(t, clamp(i + ivec2(0, 1), ivec2(0), m), 0).r')}; float e = ${one('texelFetch(t, clamp(i + ivec2(1, 1), ivec2(0), m), 0).r')};\n\treturn mix(mix(a, b, w.x), mix(c, e, w.x), w.y);`;
      return `float ${fn}(sampler2D t, vec2 uv, float r) {\n\t${body}\n}`;
    }),
    ...helperText, '',
    `void vertex() {${varyingInit}${vbody}\n}`, '',
    `void fragment() {\n\tvec4 _three_out = vec4(0.0);${depthLocal}${extraOutputs.map((o) => `\n\t${o.type} ${o.name} = ${o.type}(0.0);`).join('')}${localsOf('fragment')}\n\t${fcall};\n\t${emit}${depthEmit}\n}`, '',
  ].join('\n');
  const marker = code.indexOf('\n\n', code.indexOf('varying') >= 0 ? code.lastIndexOf('varying ') : 0);
  const fragmentAt = code.indexOf('void fragment()');
  const packRead = (text: string): string => { let x = text; for (const [n, e] of packed) { const k = e.split('.')[1]!.length; x = x.replace(new RegExp(`\\b${n}\\b`, 'g'), `${k === 1 ? 'float' : `vec${k}`}(${e})`); } return x; };
  const finalCode = packed.size > 0 ? code.slice(0, marker) + pack(code.slice(marker, fragmentAt)) + packRead(code.slice(fragmentAt)) : code;
  return { code: finalCode, material, name, uniforms, attributes, notes, unsupported };
}
