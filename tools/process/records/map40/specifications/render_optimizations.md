# render:optimizations

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/optimizations.ts`, `src/render/optimizationsApply.ts`, `src/render/optimizationsQuality.ts`
- Unidades próprias (só esta funcionalidade alcança): 7 em 2 arquivo(s), 45 linhas
- Alcance total: 58 unidades; usa 22 compartilhadas de outras; é dona principal de 29 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/effectsWiring.ts#createEffectsWiring@108` (2), `src/effectsWiring.ts#readOptimizations@49` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `render:optimization` (9)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: não se aplica 19, portado 63

## Código próprio (linhas — declaração — comentário)

### src/render/optimizations.ts
- 293-293 `optimizationSpec` (function)
- 296-302 `optimizationChoice` (function): The value `value` (or its text, as an input gives it) when `id` may take it: one of its choices, or a number in its range; undefined otherwise.
- 304-306 `isDefaultOptimizations` (function)
- 346-350 `OptTarget` (interface): A thing of the scene as the lab's inspector knows it: its layer (`LAYER_SWITCHES`), its post pass, the names of it and its parents.
- 353-365 `globMatch` (function): `*` any run of characters, everything else itself.
- 368-376 `affectsTarget` (function): Whether `spec` changes `target` (`OptimizationSpec.affects`); `all` only when `withAll`.

### src/render/optimizationsQuality.ts
- 96-102 `qualityFieldValue` (function): Whether `value` is one `id` (a `q.<field>`) may take: a choice, or a number in its range.
