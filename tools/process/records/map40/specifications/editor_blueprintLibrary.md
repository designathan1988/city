# editor:blueprintLibrary

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/blueprintLibrary.ts`
- Unidades próprias (só esta funcionalidade alcança): 11 em 1 arquivo(s), 73 linhas
- Alcance total: 159 unidades; usa 140 compartilhadas de outras; é dona principal de 8 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/buildingsWiring.ts#createBuildingWiring@81` (6)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:buildings` (7), `world:rescale` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/blueprintLibrary.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interior`: outro porte 2

## Código próprio (linhas — declaração — comentário)

### src/editor/blueprintLibrary.ts
- 8-8 `KEY` (variable): DO NOT drop the oldest past a count: the 49th save erased the first without a word
- 10-16 `StoredBlueprint` (interface)
- 18-19 `isStored` (function)
- 21-49 `BlueprintLibrary` (class)
- 22-22 `BlueprintLibrary.kept` (field)
- 24-26 `BlueprintLibrary.constructor` (method)
- 28-30 `BlueprintLibrary.list` (method)
- 33-43 `BlueprintLibrary.save` (method): Saves a body under a name (the one of that name replaced); returns the stored blueprint, or null when it was not saved.
- 46-48 `BlueprintLibrary.remove` (method): Removes one blueprint, the rest as they were; false when nothing could be written.
- 52-56 `readEntry` (function): One stored entry as a blueprint, or null when this build cannot read it (it stays stored).
- 59-66 `readBody` (function): A stored body, repaired through `migrateBuilding` (placement fields are dummies).
