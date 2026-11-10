# ui:effectsPanel

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/effectsPanel.ts`
- Unidades próprias (só esta funcionalidade alcança): 7 em 1 arquivo(s), 170 linhas
- Alcance total: 17 unidades; usa 10 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `fx.allOff`, `fx.allOn`, `fx.note`, `fx.reset`, `fx.resolution`, `fx.title`
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/effectsWiring.ts#createEffectsWiring@108` (2), `src/effectsWiring.ts#EffectsWiring@83` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `ui:i18n` (7)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 24

## Código próprio (linhas — declaração — comentário)

### src/ui/effectsPanel.ts
- 5-12 `EffectsPanelItem` (interface): THE EFFECTS PANEL: every effect of the drawing and every heavy system of the frame switched on or off live, with what the GPU, the CPU and the memory spend beside them (`effectsWiring.ts`). Not modal: the game stays in view while it is open
- 14-18 `EffectsPanelGroup` (interface)
- 20-34 `EffectsPanelDeps` (interface)
- 36-43 `EffectsPanel` (interface)
- 45-45 `RESOLUTIONS` (variable)
- 47-47 `STATS_MS` (variable): The readings redrawn this often while the panel is open, ms.
- 49-180 `createEffectsPanel` (function)
