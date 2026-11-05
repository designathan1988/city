# Registro do objetivo: implementar e corrigir todas as pendências

Worktree: `C:\Users\Jonathan\Desktop\city\tools\converter-slice`. O jogo (original e cópia) e o main não são alterados.

Estado de partida (medido antes deste registro, frame real `out/frame2`, 177 draws, referência WebGL no ANGLE/Vulkan):
51 iguais (todo pixel ≤ 2/255), 60 próximos (≤ 1 % dos pixels diferem), 6 diferentes, 60 vazios (fora da tela);
porte C++ 177/177 byte-idêntico ao GDScript; paridade sintética 21/21 exata (MRT incluído).

---

## Passo 1 — Identificar os problemas (testes, medições, investigação no código)

Ferramentas de medição: `scripts/frame-replay.mjs --probe` (o mesmo valor intermediário nos dois motores),
`scripts/probe-battery.mjs` (vários probes em paralelo), `out/inv-defines.mjs` (defines e estado GL dos programas),
comparação pixel a pixel por coordenada, contagem de vizinhos, medição de faixas de dados capturados.

| # | Problema (efeito) | Medição / investigação | Causa identificada |
|---|---|---|---|
| P1 | Programas 291, 356, 516, 144 (desenhos “próximos/diferentes”), o OutputShader 663 (draw 175, 3,7 % dos pixels) e os passes GPGPU 1 e 5 (draws 0/1: Godot desenha preto) | Probe battery: em 95/112/141 todas as etapas de iluminação e `outgoingLight` são iguais; em 175 `clamp(c,0,1)` (a cor final) é igual; só o valor final com alfa difere. `inv-defines.mjs`: os 7 programas **não têm `OPAQUE`** e o draw tem **blend desligado**. Draws 0/1: alfa de saída 0 (dados) | O tradutor escreve `ALPHA` quando o material não é opaco; no Godot, escrever `ALPHA` põe o material no pipeline transparente com `blend_mix` por padrão — mistura que o three não fazia (blend desligado). RGB sai multiplicado pelo alfa |
| P2 | Draw 44 (programa 101, a estrada): `diffuseColor` diferente | Probes: `map`, bias, `detailW`, `fwidth`, desgaste, faixas, `pow` de base negativa, leitura em fluxo não uniforme — todos iguais; **`vColor.rgb` diferente**, `vColor.a` igual. Dados capturados do atributo `color`: valores de 0,84 a **2,70**, 33 849 de 41 898 não exatos em 8 bits | O atributo `color` do three é float e HDR; o tradutor o leva a `ARRAY_COLOR`/`COLOR`, que o Godot armazena com precisão de 8 bits normalizada (a confirmar na pesquisa): clampa em 1 e quantiza |
| P3 | Diferenças de 1–20 pixels em muitos draws (7, 19, 129, 131, 46, 168…) | Coordenadas dos pixels: em 7/19 um pixel de silhueta coberto só de um lado; em 129/131/46/168 pixels internos trocando entre dois valores (empates entre triângulos). Teste: Godot sem depth prepass (`driver/depth_prepass/enable=false`) → **nenhuma mudança** (prepass descartado como causa) | O modo `matrices: 'three'` espelha y (`POSITION.y = -y`): o Godot rasteriza no framebuffer espelhado em relação ao ANGLE (y para cima). O *snapping* de vértices em ponto fixo e a regra de preenchimento (top-left) desempatam em coordenadas espelhadas → bordas e empates diferentes |
| P4 | Sequências curtas de pixels com ~3/255 (53, 55, 105) nas bordas de sombra | Pixels contíguos, vizinhos todos acesos, diferença pequena e constante | A comparação de sombra manual usa pesos bilineares em float; o hardware calcula os pesos com precisão de sub-texel limitada (`subTexelPrecisionBits`) |
| P5 | Saídas de render targets float (bone matrices dos draws 0/1, cena HDR) comparadas em RGBA8 | O replay WebGL desenha sempre num RGBA8; a captura não registra o formato do anexo | Comparação fraca: valores fora de [0,1] e a precisão float são perdidos; o formato do alvo não é capturado |
| P6 | 60 draws vazios; programas 9, 15, 284, 608, 631, 664 nunca visíveis em `frame2` | `draw-coverage.mjs`: 0 vértices dentro do volume de recorte | Câmera única: esses programas não são exercitados. Capturas extras `frame3` (90 draws) e `frame4` (364 draws) já feitas, ainda não reproduzidas |
| P7 | Caminho de texturas inteiras pouco exercitado | Todas as texturas uint das 3 capturas são zero (confirmado por duas leituras independentes, `crossChecked`) | A cena não usa lotes: nenhum dado real não nulo; falta um caso com dados inteiros reais nas duas rotas (16 bits e bits em float) |
| P9 | Probes (e a codificação de profundidade) não valem num `main` com `return` antecipado | Draw 0/1: `main` retorna cedo (`outColor = vec4(0.0); return;`) | O probe é acrescentado no fim de `main`; um `return` antes dele o pula |
| P8 | Em lista de programas diferente, traduções do cache saíam como “hung” | Replay `--only` de 11 draws: 7 “translation hung” | `shaders.ts` gravava a entrada do cache com o `id` e o nome de arquivo da lista em que foi criada (corrigido no código antes deste registro; listado para o teste final) |

---

## Passo 2 — Pesquisa (documentação oficial e fontes relevantes)

| Site consultado | O que diz | Problemas |
|---|---|---|
| https://docs.godotengine.org/en/stable/tutorials/shaders/shader_reference/spatial_shader.html | `ALPHA`: “If read from or written to, the material will go to the transparent pipeline”; blend: `blend_mix` (padrão), `blend_add`, `blend_sub`, `blend_mul`, `blend_premul_alpha` — **não existe** modo sem blend; `FRAGCOORD`: origem no canto superior esquerdo; `POSITION` substitui a posição de clip | P1, P3 |
| https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/scene_shader_forward_clustered.cpp | Compare de profundidade `COMPARE_OP_GREATER_OR_EQUAL` (= GEQUAL do three); passe de cor opaco com prepass usa `EQUAL`; pipeline transparente usa `blend_state_color_blend` | P1, P3 (empates de profundidade descartados como causa, como a medição sem prepass) |
| https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/rendering_server.cpp | `ARRAY_COLOR`: `uint8_t(CLAMP(c * 255.0, 0.0, 255.0))` — 8 bits, truncado, clampado em [0,1]; `ARRAY_NORMAL`: octaédrico 16 bits; `ARRAY_CUSTOM*` RGBA_FLOAT: float32 sem compressão | P2 |
| https://forum.godotengine.org/t/is-array-color-stored-as-floats-or-bytes-in-godot-4-x/142528 | `ARRAY_COLOR` é RGBA 8 bits normalizado; sem cor float no 4.x | P2 |
| https://forum.godotengine.org/t/cant-pack-32-bit-int-into-a-color-channel-for-custom-vertex-data/136818 | Usar atributo custom para dados de precisão | P2 |
| https://docs.vulkan.org/refpages/latest/refpages/source/VkViewport.html · https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkViewport.html | Altura negativa inverte y no transform clip → framebuffer; origem do framebuffer Vulkan no topo | P3 |
| https://saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport | Vulkan y para baixo, GL y para cima; espelhar troca o enrolamento | P3 |
| https://chromium.googlesource.com/angle/angle/+/5a0f64b6ae2900670f9ae326f056d0ad5eb4788c/include/platform/FeaturesVk.h | ANGLE `flipViewportY`: render invertido “igual ao OpenGL”, ligado ao backbuffer | P3 |
| https://chromium.googlesource.com/angle/angle/+/refs/heads/main/src/libANGLE/renderer/vulkan/ContextVk.cpp | Rotação de `gl_Position` só no framebuffer padrão de janela; FBOs offscreen não | P3 |
| https://microsoft.github.io/DirectX-Specs/d3d/VulkanOn12.html | Viewport negativo existe para compatibilidade com OpenGL | P3 |
| https://projects.blender.org/blender/blender/pulls/107743/files | Blender/Vulkan: flip só no framebuffer padrão | P3 |
| https://pub.dev/documentation/vulkan/latest/vulkan/VkPhysicalDeviceLimits/subTexelPrecisionBits.html | `subTexelPrecisionBits`: precisão de subdivisão dos filtros | P4 |
| https://swiftshader.googlesource.com/SwiftShader/+/678ffefb1b3b677de66e2050b2d3bd04f7300d2f%5E%21 | Implementações usam 8 bits de sub-texel; diferenças em texels vizinhos muito distintos (bordas) | P4 |
| `vulkaninfo` neste PC (RTX 3060) | `subTexelPrecisionBits = 8`, `subPixelPrecisionBits = 8`, `mipmapPrecisionBits = 8`, `maxSamplerAnisotropy = 16` | P3, P4 |
| https://docs.godotengine.org/en/4.3/classes/class_viewport.html · https://docs.godotengine.org/uk/4.x/classes/class_viewporttexture.html | `use_hdr_2d`: textura HDR linear. Medido no 4.7.2: imagem de viewport HDR = formato 14 (`RGBH`, meio float) | P5 |
| https://docs.godotengine.org/en/4.2/classes/class_rdsamplerstate.html · https://engineer-game.org/devlog/2025/05/09/godot-anisotropic-filtering.html · https://docs.godotengine.org/en/latest/classes/class_renderscenebuffersconfiguration.html | Anisotropia: hint por uniform, nível por projeto/render buffers; medido no 4.7.2: `Viewport.anisotropic_filtering_level` existe | anisotropia (já resolvida por viewport) |

## Passo 3 — Causa, não efeito (raciocínio)

- **P1 ← ALPHA ⇒ pipeline transparente.** Medido: iluminação e cor final iguais quando o probe escreve alfa 1; só a saída com o alfa do programa difere, e só nos programas sem `OPAQUE` com blend desligado. Documentado: escrever `ALPHA` leva ao pipeline transparente, blend padrão `blend_mix`, sem modo “sem blend”. Logo o RGB sai multiplicado pelo alfa (preto quando o alfa é dado, como nos passes GPGPU). Exato para um draw sobre alvo limpo (o replay, e o three com blend desligado): `blend_premul_alpha` (cor `src + dst·(1−a)`, alfa `a + dst.a·(1−a)`) grava RGB e alfa exatos quando o destino é (0,0,0,0). Sem precisar do alfa: não escrever `ALPHA` (pipeline opaco, RGB exato).
- **P2 ← ARRAY_COLOR 8 bits.** Medido: `vColor` diferente com cores HDR até 2,70. Fonte: `uint8(CLAMP(c*255))`. A cor por vértice não pode passar por `COLOR`; CUSTOM RGBA_FLOAT é float32 exato. Normais (octaédricas 16 bits) e tangentes também perdem precisão: CUSTOM. A cor de instância segue em `COLOR` (MultiMesh guarda float; medido: draw 54 igual com cor > 1).
- **P3 ← framebuffer espelhado.** Medido: pixels de silhueta e empates internos; prepass descartado (fonte: GEQUAL igual). Fontes: Vulkan com origem no topo; ANGLE só inverte o framebuffer padrão, então num FBO rasteriza com y_fb = y_GL. O modo `three` nega y no `POSITION`: o Godot rasteriza com y_fb = H − y_GL, e o *snapping* de 8 bits de sub-pixel e a regra de preenchimento desempatam em coordenadas espelhadas. Solução: orientação GL (sem negar y; sem inverter índices, as duas inversões se cancelam; `gl_FragCoord.y = FRAGCOORD.y`; `dFdy` sem troca de sinal). A imagem do Godot fica com a linha 0 = base GL, como o `readPixels`, e um render target assim é amostrado pelo passe seguinte com o v do three (correto também num porte).
- **P4 ← PCF manual em float.** O hardware filtra com 8 bits de sub-texel (medido): pesos bilineares do PCF manual quantizados a 1/256.
- **P5 ← alvo float lido em RGBA8.** O viewport HDR do Godot guarda meio float (medido); nenhum viewport float32. Comparação exata: o shader grava os 32 bits de cada componente como 4 bytes (bytes k/255 sobrevivem exatamente ao meio float), uma passada por componente; o WebGL renderiza em RGBA32F e lê floats. A captura precisa registrar o formato do anexo.
- **P6 ← uma câmera.** Capturas `frame3`/`frame4` com outras vistas: reproduzi-las no teste final.
- **P7 ← cena sem inteiros não nulos.** Caso sintético de paridade com textura inteira real nas duas rotas.
- **P9 ← probe no fim de `main`.** Renomear `main` e chamar o original antes do probe (probe, profundidade, empacotamento).

## Passo 4 — Plano (ligado aos passos 1–3)

1. **P1** — tradutor: opção `alphaOutput` (`'blend'`: modo de blend do three; `'replace'`: `ALPHA` + `blend_premul_alpha`, exato sobre alvo limpo; `'none'`: sem `ALPHA`). Replay: `blend ? 'blend' : 'replace'`. Comparação inclui o alfa.
2. **P2** — tradutor: `color`, `normal`, `tangent` do three em slots CUSTOM float (ou textura de dados); `instanceColor` em `COLOR.rgb`. Replay e parity seguem o mapa de atributos.
3. **P3** — tradutor: `orientation: 'gl'` (com `matrices: 'three'`). Replay: sem inversão de índices; comparação sem inverter linhas (caso marca `orientation`); `replay-compare`/`replay-view` respeitam.
4. **P4** — tradutor: pesos do PCF quantizados (`shadowSubTexelBits`, padrão 8).
5. **P5** — captura: formato interno do anexo de cor. Replay: alvo float → WebGL RGBA32F lido em float; Godot em 4 passadas de bits (caso `id` = `<draw>c<k>`); comparação float por ULP. replay.gd e runtime C++ usam `id`.
6. **P9** — probe, profundidade e empacotamento via renomeação de `main`.
7. **P7** — parity: `uint-texture-16` e `uint-texture-32` com dados reais; parity.gd com rotas RD `R16G16B16A16_UINT` e bits em `RGBAF`.
8. **P6, P8** — teste final: replay de `frame2` (recapturado com formatos de alvo), `frame3`, `frame4`; C++; paridade.

## Passo 5 — Codificação (sem testes)

Nenhum teste, medição ou probe foi executado neste passo; só edição de código.

| Arquivo | Ação | O que mudou | Plano / problema |
|---|---|---|---|
| `src/render/godot-shader.ts` | alterado | Atributos `color`, `normal`, `tangent`, `skinWeight` → slots CUSTOM float (ou textura de dados); `instanceColor` → `COLOR.rgb` (sem o caso `vec3(1.0)`); comentário com a fonte (`rendering_server.cpp`) | 2 / P2 |
| `src/render/godot-shader.ts` | alterado | Opção `alphaOutput` (`blend`/`replace`/`none`): `replace` = `ALPHA` + `blend_premul_alpha` (remove outros `blend_*`); `none` = sem `ALPHA`; notas | 1 / P1 |
| `src/render/godot-shader.ts` | alterado | Opção `orientation: 'gl'` (com `matrices: 'three'`): `POSITION.y` mantido, `gl_FragCoord.y = FRAGCOORD.y`, `dFdy` sem troca de sinal; nota `orientation gl` | 3 / P3 |
| `src/render/godot-shader.ts` | alterado | Opção `shadowSubTexelBits` (padrão 8): coordenada do texel em ponto fixo, `floor((uv·s − 0,5)·256)/256`, antes de escolher texels e pesos do PCF | 4 / P4 |
| `scripts/frame-capture.mjs` | alterado | Formato interno do anexo de cor (`target.format`, de textura ou renderbuffer) e `target.componentType` | 5 / P5 |
| `scripts/frame-replay.mjs` | alterado | `outputOf` (saída do fragment) e `afterMain` (renomeia `main`, roda o código depois de tudo, retornos antecipados incluídos) para profundidade e empacotamento; probe mantido dentro de `main` (lê locais); opções `orientation: 'gl'` e `alphaOutput` por draw; alvo float → 4 variantes (`packed`: bits do componente k em 4 bytes) com casos `<draw>c<k>` (`float`, `bits`); índices sem inversão em orientação GL; caso com `id` e `orientation`; referência WebGL em RGBA32F lida como float (`.f32`) para alvos float; lista de draws da referência sem duplicatas; comparação pelo módulo comum; `judge.json` para o C++ | 1, 3, 5, 6 / P1, P3, P5, P9 |
| `scripts/replay-judge.mjs` | criado | Veredito comum: RGBA com alfa, linhas por orientação, alvos float pelas palavras de 32 bits (≤ 16 ULP), profundidade (código 24 bits), níveis igual/próximo/diferente/vazio/vazio-de-referência | 1, 3, 5 |
| `scripts/replay-compare.mjs` | reescrito | Usa o veredito comum e `judge.json`; byte a byte com outro replay, por `id` e extensão | 5, 8 |
| `scripts/replay-view.mjs` | alterado | Linhas do Godot conforme a orientação | 3 |
| `scripts/replay.gd` | alterado | Nome e marcadores por `id`; passadas de bits gravadas em `RGBAF` (`.f32`); pesos zero quando o atributo vai para CUSTOM | 5, 2 |
| `cpp/runtime/three_runtime.h` / `.cpp` | alterados | `Draw.id`, `Draw.bits`; nome e formato da saída por `id`/`bits`; pesos zero com ossos | 5, 2 |
| `scripts/cpp-scene.mjs` | alterado | Função por `id` (`fid`), `d.id`/`d.bits` gerados | 5 |
| `scripts/parity.mjs` | alterado | Casos `uint-texture-16` e `uint-texture-32` (DataTexture RGBA32UI com dados reais); rotas UINT16/BITS32 e `floatBitsSamplers`; cor por vértice fora de `ARRAY_COLOR` | 7, 2 / P7 |
| `scripts/parity.gd` | alterado | Rota RD `R16G16B16A16_UINT` para `UINT16`; pesos zero com ossos | 7, 2 |
| `test/godot-shader.test.ts` | alterado | Testes atualizados (compare de sombra com precisão do hardware; pesos em CUSTOM) e novos (atributos em CUSTOM e `instanceColor`; `alphaOutput`; orientação) | 1–4 |
| `scripts/shaders.ts` | alterado (antes deste registro) | Entrada do cache com o `id` e o nome de arquivo da lista atual | P8 |

## Passo 6 — Testes e medições (uma única execução: `out/final-test.sh`, saída em `out/final-test.log`)

| Etapa | Teste / medição | Resultado |
|---|---|---|
| 1 | Typecheck (92 arquivos) | **0 problemas** |
| 2 | Testes unitários (`test/godot-shader.test.ts`, `test/platform.test.ts`) | **Resultado não registrado**: o filtro do script procurava linhas TAP (`# pass`) e o Node 24 imprime no formato spec (`ℹ pass`). Os testes rodaram, mas a saída foi filtrada; não foram reexecutados (regra do passo 6) |
| 3 | Os 277 programas reais: tradução (live-run) e compilação + desenho no Godot | Tradução **277/277** em 16,2 s, 0 falhas, 0 travamentos; Godot **compilou e desenhou 277/277** (223 compilados agora, 54 do cache) |
| 4 | Paridade sintética three.js × Godot, critério estrito | **23/23 iguais** (todo pixel ≤ 2/255): MRT (2 anexos), `uint-texture-16` (rota RGBA16 uint) e `uint-texture-32` (rota de bits em float) com dados inteiros reais, sombra, skinning, instâncias, materiais padrão |
| 5 | Três capturas novas (formato dos alvos registrado) | Vista padrão: 314 draws, 56 programas; vista alta: 78 draws, 30 programas; vista de rua: 147 draws, 36 programas; 0 erros de página |
| 6 | Replay de todos os draws de cada vista | Vista 5: 6 iguais, 190 próximos, 58 diferentes, 60 vazios. Vista 6: 4 / 14 / 26 / 34. Vista 7: 4 / 75 / 51 / 17. **Regressão** em relação ao estado de partida (51 iguais) |
| 7 | Porte C++ da vista 5 | 154 arquivos compilados em 10,1 s, DLL de 31 MB; executado no Godot; **1123/1208 imagens byte-idênticas** à versão GDScript; mesmos níveis contra o WebGL (5 / 190 / 58 / 61) |
| 8–9 | Diferenças por programa; cobertura somando as três vistas | 58 programas distintos: 6 com todo draw visível igual, 48 com algum draw diferente, 4 nunca na tela |

**O que a execução revelou (lido dos logs da própria execução, sem novos testes):**

- **Causa da regressão: o caminho de alvos float (P5) não compila no Godot.** O log do Godot tem 1203 vezes `SHADER ERROR: Unknown identifier in expression: '_three_out'`. O `afterMain` move o `main` original para `_three_replay_main()`, e a saída do fragment (`pc_fragColor` → `_three_out`) passa a ser escrita dentro dessa função auxiliar. O tradutor só entrega `_three_out` ao `main` do fragment (`_three_fragment(inout vec4 _three_out)`), não às auxiliares. Essas passadas não desenham, e a decodificação lê palavras zero: as diferenças de **1065353216 ULP** são exatamente os bits de 1,0 contra 0.
- **Por que isso pegou quase tudo:** na vista 5, 284 de 314 draws desenham no alvo RGBA16F da cena (`componentType` FLOAT), e 14 num alvo RGBA32F. Só 15 draws usam alvo de 8 bits. A regressão de 51 → 6 iguais vem dessa falha do caminho de empacotamento, não das correções P1–P4. Estas não puderam ser avaliadas nesta execução, porque os mesmos draws passaram pelo caminho quebrado.
- **Problema adicional observado:** o viewport HDR do Godot sem fundo transparente é `RGBH` (sem canal alfa, medido no passo 2). A passada de bits grava 4 bytes por pixel e depende do canal alfa; com `transparent_bg = true` (o que o replay usa) o formato não foi medido.
- **C++:** 85 das 1208 imagens não são byte-idênticas ao GDScript, e 261 vezes `Parameter "RenderingServer::get_singleton()" is null` no encerramento do processo (liberação de texturas RD depois do servidor). Causa não investigada.
- **P6 (cobertura):** com três vistas, 54 de 58 programas distintos aparecem na tela; 4 continuam fora de todas as vistas.

## Passo 7 — Parada para avaliação

Execução parada aqui, como exige a regra. Pendências que o passo 6 mostrou e que **não** foram corrigidas (a regra proíbe nova codificação depois dos testes):

1. O empacotamento de alvos float precisa que a saída do fragment fique disponível fora do `main`. Por exemplo, o tradutor passar `_three_out` (como `inout`) às auxiliares que o escrevem, ou o `afterMain` copiar a saída para uma global de fragment. Também é preciso confirmar se há canal alfa no viewport do replay; se não houver, usar 3 bytes por passada.
2. Reavaliar P1–P4 (alfa, cores por vértice, orientação GL, PCF) com o caminho float funcionando.
3. Testes unitários: corrigir o filtro do script (`ℹ pass`/`ℹ fail`) e registrar o resultado.
4. C++: as 85 imagens diferentes e a ordem de liberação das texturas RD no encerramento.
5. Quatro programas que nenhuma das três vistas mostra.

**Avaliação do ciclo 1: DEU ERRO** (regressão no replay real, C++ divergente, testes unitários sem resultado). Recomeço no passo 1.

---

# Ciclo 2

## Passo 1 — Identificar os problemas

| # | Problema | Medição / investigação | Causa |
|---|---|---|---|
| Q1 | Passadas de empacotamento de alvos float não compilam (`Unknown identifier '_three_out'`, 1203×) | Shader gerado `p1_0`: `_three_replay_main(in vec4 _g_FRAGCOORD)` escreve `_three_out`, mas não o recebe; só `_three_fragment(inout vec4 _three_out, …)` recebe. Código do tradutor: o cabeçalho do main é fixo `void _three_fragment(inout vec4 _three_out)`, e a chamada `_three_fragment(_three_out)`. `gl_FragDepth` (`_three_depth`) e saídas MRT estão no `contextType` (entregues a qualquer auxiliar); `_three_out` não | O tradutor trata a saída do fragment como parâmetro só do `main`, não como contexto do estágio: qualquer função que escreva a saída (o three permite: a saída é global) quebra |
| Q2 | Programas `OPAQUE` (pós-processos 646–661) com alfa diferente | Replay de `frame2` (sem caminho float), por canal: RGB 0 diferenças, alfa 36 140 / 921 600 / 38 218 pixels; WebGL guarda alfa 35, 129, 0…; Godot 255 | O tradutor decide pelo define `OPAQUE` não escrever `ALPHA`; mas o shader escreve seu próprio alfa (`gl_FragColor = vec4(rgb, a)`), e com blend desligado o WebGL o guarda. `OPAQUE` não diz o que o shader escreve no alfa |
| Q3 | Programas não opacos com blend desligado (291, 356, 516, 144) ainda diferem | Por canal: RGB e alfa maiores no Godot (W 3,5,4,55 × G 5,8,6,255; W 25,13,17,151 × G 48,23,33,255); são instanciados ou de dois lados (fragmentos sobrepostos) | `replace` = `blend_premul_alpha`: exato só sem sobreposição. Com fragmentos sobrepostos o Godot compõe (`src + dst·(1−a)`); o three, com blend desligado, sobrescreve. O pipeline transparente do Godot não tem blend desligado |
| Q4 | Porte C++: 85 imagens diferentes do GDScript | Rodada GDScript repetida nessas 85: 85/85 idênticas à 1ª rodada GDScript (determinístico), 0/85 ao C++. As 85 são **todas** de draws com ossos (`bones`) | `cpp-scene.mjs` gera `d.weights = ${str(c.weights)}`: com `weights` nulo grava a string `"null"`; o runtime lê o arquivo inexistente e passa pesos vazios ao lado dos ossos |
| Q5 | Porte C++: 261× `RenderingServer::get_singleton() is null` e 581 RIDs vazados ao sair | Log: erros em `~Texture2DRD` depois de `REPLAY DONE` | O cache estático `texture_cache` do runtime (std::map de `Ref<Texture>`) é destruído no descarregamento da DLL, depois do servidor de renderização; os RIDs nunca são liberados |
| Q6 | Testes unitários: 2 de 30 falham | `node --test`: falham “orientation gl…” e “a second color output…”. Saída real do tradutor para o caso de orientação: `vec4(_g_FRAGCOORD.x, _g_FRAGCOORD.y, …)` dentro do auxiliar | Os testes estão errados: (a) a regex espera `FRAGCOORD.` sem o prefixo `_g_` dos auxiliares; (b) o teste de MRT exige `unsupported`, mas desde a divisão MRT a segunda saída é uma *nota* (cada local vira uma passada) |
| Q7 | Script do ciclo 1 não mostrou o resultado dos testes unitários | O Node 24 imprime `ℹ pass` / `ℹ fail` (reporter spec), não `# pass` | Filtro errado no `final-test.sh` |
| Q8 | Diferenças de até 8/255 nas sombras/normais do terreno (565) e pixels isolados de cobertura (42, 185, 348, 460, 127, 201) | Teste PCF: truncar × arredondar a coordenada de sub-texel: **nenhuma mudança** (descartado). Probe battery em 151/152/155: `diffuseColor` igual, `normal` já difere (normal-map por `textureGrad` em arrays + derivadas); pixels isolados: um lado aceso e o outro vazio, ou outra superfície vencendo | Os dois motores executam o mesmo GLSL compilado por compiladores diferentes (ANGLE → SPIR-V; Godot → glslang → SPIR-V): hipótese de contração FMA / reordenação nas contas de `gl_Position` e da normal (a confirmar na pesquisa) |
| Q9 | 4 programas nunca na tela nas três vistas | Contagem do ciclo 1 (etapa 9) | Cobertura de câmera (não é erro de tradução) |
| — | Efeito real de P1–P4 (medido sem o caminho float: `frame2`) | 51 → **66 iguais**; 16 próximos→iguais, 4 diferentes→iguais; 5 iguais→diferentes (são os de Q2, só alfa) | P1–P4 corretos; a regressão do ciclo 1 vinha de Q1 |
| — | Formato do viewport HDR com fundo transparente | Medido: formato 15 (`RGBAH`, com alfa) | 4 bytes por passada cabem |

## Passo 2 — Pesquisa (ciclo 2)

| Site consultado | O que diz | Problemas |
|---|---|---|
| https://docs.godotengine.org/en/stable/tutorials/shaders/shader_reference/spatial_shader.html · https://godot.readthedocs.io/uk/4.x/tutorials/shaders/shader_reference/spatial_shader.html · https://docs.godotengine.org/de/4.4/_sources/tutorials/shaders/shader_reference/spatial_shader.rst.txt | Escrever `ALPHA` leva ao pipeline transparente; modos de blend só `mix/add/sub/mul/premul_alpha`; nenhum modo sem blend; `ALPHA_SCISSOR_THRESHOLD` descarta (alfa binário, não guarda valores) | Q2, Q3 |
| https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/scene_shader_forward_clustered.cpp (ciclo 1) | Pipeline opaco: `blend_state_color_opaque` (sobrescreve), compare GEQUAL, prepass + EQUAL — o mesmo resultado do three com blend desligado | Q2, Q3 |
| https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/shader_language.cpp | A tabela de palavras-chave tem `lowp/mediump/highp` e **não tem `precise` nem `invariant`** | Q8 |
| https://chromium.googlesource.com/angle/angle.git/+/main/src/tests/compiler_tests/Precise_test.cpp | ANGLE gera `NoContraction` no SPIR-V **só** quando o shader usa `precise`; sem `precise`, o driver pode fundir mul+add (FMA) | Q8 |
| https://chromium.googlesource.com/angle/angle.git/+/refs/heads/chromium/4595%5E (commit “precise” na geração direta de SPIR-V) | A geração direta de SPIR-V do ANGLE aplica `precise` às expressões do lado direito, apenas se o shader usar a palavra | Q8 |
| https://rust-digger.code-maven.com/src/glsl-to-spirv-0.1.7/glslang/Test/precise.tesc | glslang: `precise` → `NoContraction`; `fma()` explícito não é decorado | Q8 |
| https://arai.searchfox.org/firefox-main/source/gfx/angle/checkout/src/compiler/translator/TranslatorVulkan.h | O ANGLE (tradutor Vulkan) tem pipeline de compilação próprio, distinto do do Godot | Q8 |
| https://docs.godotengine.org/uk/stable/tutorials/shaders/converting_glsl_to_godot_shaders.html | Guia de conversão GLSL→Godot: qualificadores de precisão sim; nada sobre `precise`/`invariant` | Q8 |
| https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/rendering_server.cpp (ciclo 1) | Formatos dos arrays de mesh (`ARRAY_WEIGHTS` obrigatório com `ARRAY_BONES`) | Q4 |
| https://docs.godotengine.org/en/stable/classes/class_texture2drd.html (ciclo 1) e Godot `texture_rd.cpp` (citado no log: `~Texture2DRD` usa o RenderingServer) | Um `Texture2DRD` libera seu wrapper pelo RenderingServer: precisa ser destruído antes do servidor; o RID do RD é de quem o criou | Q5 |
| https://nodejs.org/api/test.html (reporter padrão `spec`, linhas `ℹ tests/pass/fail`) | Formato da saída de `node --test` | Q7 |

## Passo 3 — Causa, não efeito (ciclo 2)

- **Q1 ← a saída do fragment não é contexto.** Em GLSL a saída é uma variável global do estágio: qualquer função pode escrevê-la. O tradutor a dá só ao `main` traduzido (parâmetro fixo). A solução está no tradutor: tratar `_three_out` como o resto do contexto do estágio (como `_three_depth` e as saídas MRT já são). Assim, toda função que a lê ou escreve a recebe (`inout` quando escreve), e o `main` também, pelo mesmo mecanismo. Isso conserta o empacotamento e qualquer programa do three que escreva a saída numa função.
- **Q2 + Q3 ← o Godot não tem pipeline transparente sem blend.** Fontes: escrever `ALPHA` ⇒ pipeline transparente, sempre misturado; o pipeline opaco sobrescreve (o mesmo que o three com blend desligado), mas não guarda o alfa do shader. Medido: RGB exato nos opacos, alfa errado; RGB e alfa errados onde há sobreposição com `premul`. Conclusão: com blend desligado, o RGB vai pelo pipeline opaco (exato, com sobreposições e profundidade como o three), e o alfa por **uma segunda passada opaca** que grava o alfa nos canais de cor (`ALBEDO = vec3(a)`). O par (RGB, A) reconstrói exatamente o que o three guardou. O define `OPAQUE` deixa de decidir: o que decide é o blend do draw.
- **Alvos float**: pelo mesmo motivo, as passadas de bits usam só RGB no pipeline opaco: 3 bytes por passada, 16 bytes por pixel ⇒ 6 passadas.
- **Q4 ← gerador grava `"null"`.** Emitir pesos só quando existem; senão o runtime cria pesos zero (como o GDScript).
- **Q5 ← destruição estática fora de ordem.** Liberar o cache (wrappers) e os RIDs do RD antes de `quit()`, nos dois runtimes.
- **Q6 ← testes errados**: corrigir as expectativas (prefixo `_g_` nos auxiliares; MRT é nota). **Q7**: filtro `ℹ`.
- **Q8 ← compiladores diferentes sem `precise`.** O mesmo GLSL vira SPIR-V por dois compiladores (ANGLE e glslang/Godot). Sem `NoContraction`, o driver pode fundir mul+add de modos diferentes conforme a sequência de instruções. O ANGLE só emite `NoContraction` com `precise`, e o Godot não tem `precise`. Não há como fixar o arredondamento a partir da linguagem de shader do Godot: **limite do motor**, registrado como tal (diferenças ≤ 8/255 em poucos pixels e trocas isoladas de cobertura).
- **Q9 ← cobertura de câmera**: medir no teste quais programas faltam (não é tradução).

## Passo 4 — Plano (ciclo 2)

1. Q1 — tradutor: `_three_out` no `contextType`; cabeçalho do main `void _three_fragment()`; chamada `_three_fragment()` com o contexto acrescentado pelo mecanismo comum.
2. Q2/Q3 — tradutor: opção `channels: 'rgb' | 'alpha'` (`alpha`: `ALBEDO = vec3(_three_out.a)`); `alphaOutput` dado pelo chamador vale mesmo com `OPAQUE`. Replay: blend desligado ⇒ caso RGB (`alphaOutput: 'none'`) + caso alfa (`<draw>a`, `channels: 'alpha'`); blend ligado ⇒ `ALPHA` com o modo de blend. Juiz: RGB de um, A do outro.
3. Alvos float — replay: 6 passadas de bytes (`<draw>b0..b5`) no pipeline opaco; juiz monta as palavras dos 16 bytes.
4. Q4 — `cpp-scene.mjs`: pesos só quando existem. Q5 — runtime C++ e `replay.gd`: liberar cache e RIDs antes de sair.
5. Q6/Q7 — corrigir os dois testes e o filtro do script de teste.
6. Q9 — teste final lista os programas nunca vistos.

## Passo 5 — Codificação (ciclo 2; sem testes)

| Arquivo | Ação | O que mudou | Plano / problema |
|---|---|---|---|
| `src/render/godot-shader.ts` | alterado | `_three_out` entra no `contextType`; cabeçalho do main `void _three_fragment()`, chamada `_three_fragment()` completada pelo mecanismo de contexto: toda função que lê/escreve a saída a recebe (`inout` se escreve) | 1 / Q1 |
| `src/render/godot-shader.ts` | alterado | Opção `channels: 'rgb' \| 'alpha'` (`alpha`: `ALBEDO = vec3(_three_out.a)`, sem `ALPHA`); `alphaOutput` dado pelo chamador vale mesmo com `OPAQUE` | 2 / Q2, Q3 |
| `scripts/frame-replay.mjs` | alterado | Tipo do alvo (`kindOf`: `color8`, `half`, `bits32`); passadas por draw: blend ligado → 1 (`ALPHA` + modo); blend desligado → cor (pipeline opaco) + alfa (`<draw>a`); float32 → 6 passadas de bytes (`bytePass`, `<draw>b0..b5`, opaco); casos com `part`/`kind`/`bits`; referência WebGL em RGBA16F para alvos half e RGBA32F para float32 | 2, 3 / Q2, Q3, Q1 |
| `scripts/replay-judge.mjs` | reescrito | Junta as passadas por draw: cor+alfa; half por ULP de meio float (≤ 2); bits32 remontando as palavras de 16 bytes (≤ 16 ULP); profundidade | 2, 3 |
| `scripts/cpp-scene.mjs` | alterado | `d.weights` só quando existe | 4 / Q4 |
| `cpp/runtime/three_runtime.cpp` / `.h` | alterados | `texture_rids` e `three::release()` (cache → RIDs); liberação dois quadros depois do último viewport, antes de `quit()` (`releasing`) | 4 / Q5 |
| `scripts/replay.gd` | alterado | Lista de RIDs; ao fim, dois quadros, cache limpo e RIDs liberados | 4 / Q5 |
| `test/godot-shader.test.ts` | alterado | Expectativas corrigidas (prefixo `_g_` nos auxiliares; MRT como nota; ordem do contexto na chamada de `_three_fragment`); novos: saída escrita por função auxiliar; `channels: 'alpha'` e `alphaOutput` acima de `OPAQUE` | 5 / Q6, Q1, Q2 |
| `out/c2-test.sh` | criado | Teste do ciclo 2: filtro `ℹ`/`✖` dos testes; reusa as capturas `frame5–7`; etapa 10 lista os programas que nenhuma vista mostra | 5, 6 / Q7, Q9 |
