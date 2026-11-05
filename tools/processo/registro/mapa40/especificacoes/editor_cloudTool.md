# editor:cloudTool

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/cloudTool.ts`
- Unidades próprias (só esta funcionalidade alcança): 10 em 1 arquivo(s), 142 linhas
- Alcance total: 121 unidades; usa 110 compartilhadas de outras; é dona principal de 1 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#pointerMove@1404` (2), `src/main.ts#cloudTool@1137` (1), `src/main.ts#currentGesture@1056` (1), `src/main.ts#pointerDown@1217` (1), `src/main.ts#endPointer@1484` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:clouds` (19), `worker:conflictTable` (8), `world:doc` (7)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.clouds`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: portado 22

## Código próprio (linhas — declaração — comentário)

### src/editor/cloudTool.ts
- 7-22 `CloudToolHost` (interface): What the cloud tool reads of the game and does to it.
- 25-92 `CloudTool` (class): THE CLOUD TOOL (Landscape > Terrain > Clouds)
- 26-26 `CloudTool.drag` (field)
- 28-28 `CloudTool.constructor` (method)
- 30-32 `CloudTool.pointer` (method)
- 34-36 `CloudTool.gesture` (method)
- 38-71 `CloudTool.down` (method)
- 74-83 `CloudTool.move` (method): @perInput The pointer moved to (px, py): the cloud being dragged follows it.
- 85-87 `CloudTool.up` (method)
- 89-91 `CloudTool.cancel` (method)
