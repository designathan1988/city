# editor:mapExport

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/mapExport.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 35 linhas
- Alcance total: 311 unidades; usa 307 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#saveMap@2257` (1), `src/main.ts#IfStatement@4036` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 1
- Usa: `editor:saveBinary` (5), `world:doc` (3), `editor:persistence` (3)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/binarySave.spec.ts`, `tests/editor/exportMap.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/editor/mapExport.ts
- 6-6 `MAP_FILE_NAME` (variable): The map file's name: a binary `.roadcraft` file (`persistence.ts` `importFromFile` reads it back, and the JSON files before it).
- 9-15 `mapFileBlob` (function): A map file (Save map) as the pieces of one Blob: the frame's header, the settings' JSON (with the unit its camera is in, as the autosave stamps it), the map's binary sections (`RoadDoc.toBinary`) - never one text of the map, nor one buffer 
- 18-34 `exportToFile` (function): DO NOT go back to one `JSON.stringify` of the whole map: the file is the map's binary sections.
- 37-46 `offerFile` (function): A file offered to the player to save (a link to it clicked): whether it was.
