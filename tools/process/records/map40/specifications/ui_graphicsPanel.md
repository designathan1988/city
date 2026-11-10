# ui:graphicsPanel

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/graphicsPanel.ts`
- Unidades próprias (só esta funcionalidade alcança): 7 em 1 arquivo(s), 103 linhas
- Alcance total: 17 unidades; usa 10 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `gfx.aa`, `gfx.ao`, `gfx.close`, `gfx.dynamicResolution`, `gfx.dynamicResolution.note`, `gfx.fpsLimit`, `gfx.fpsLimit.none`, `gfx.fullscreen`, `gfx.gpu`, `gfx.off`, `gfx.on`, `gfx.preset`, `gfx.shadows`, `gfx.shadows.…`, `gfx.title`, `gfx.vsync`, `gfx.vsync.note`, `quality.…`
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/graphicsWiring.ts#createGraphicsWiring@118` (24), `src/graphicsWiring.ts#pick@284` (7)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `ui:i18n` (19)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/ui/graphicsSameChoice.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: portado 22

## Código próprio (linhas — declaração — comentário)

### src/ui/graphicsPanel.ts
- 5-5 `PresetName` (type): THE GRAPHICS OPTIONS: a dialog of choices, each applied to the running game as it is picked (`graphicsWiring.ts`).
- 7-19 `GraphicsChoices` (interface)
- 21-29 `GraphicsPanelDeps` (interface)
- 31-31 `PRESETS` (variable)
- 32-32 `SHADOWS` (variable)
- 33-33 `FPS` (variable)
- 35-111 `createGraphicsPanel` (function)
