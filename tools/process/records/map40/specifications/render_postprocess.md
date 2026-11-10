# render:postprocess

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/postprocess.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 70 unidades; usa 45 compartilhadas de outras; é dona principal de 25 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: `ouve:webglcontextrestored`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`, `three/examples/jsm/postprocessing/EffectComposer.js`, `three/examples/jsm/postprocessing/GTAOPass.js`, `three/examples/jsm/postprocessing/OutputPass.js`, `three/examples/jsm/postprocessing/Pass.js`, `three/examples/jsm/postprocessing/RenderPass.js`, `three/examples/jsm/postprocessing/SMAAPass.js`, `three/examples/jsm/postprocessing/ShaderPass.js`, `three/examples/jsm/postprocessing/UnrealBloomPass.js`, `three/examples/jsm/shaders/CopyShader.js`

## Testes do 4.0 que a exercitam
- `tests/render/bloomBlur.spec.ts`, `tests/render/plainTarget.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `landscape`: portado 9

## Código próprio (linhas — declaração — comentário)
