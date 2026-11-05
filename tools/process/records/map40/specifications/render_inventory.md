# render:inventory

Grupo: render. Entrada: desenho. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/inventory.ts`
- Unidades próprias (só esta funcionalidade alcança): 10 em 1 arquivo(s), 159 linhas
- Alcance total: 10 unidades; usa 0 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#IfStatement@4036` (2)

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
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/render/inventory.ts
- 6-26 `InventoryRow` (interface): One row: a kind of mesh in a layer.
- 29-32 `InventoryFlag` (interface): A figure that says a row is out of proportion.
- 34-38 `Inventory` (interface)
- 40-40 `Attribute` (type)
- 41-41 `Holder` (type)
- 44-47 `kindOf` (function): A name without what tells one tile or one block from another: digits, hashes, `-ground`, `-alignment-12`.
- 49-49 `MB` (variable)
- 52-63 `FLAGS` (variable): The thresholds of the flags: a figure past one is where to look first.
- 66-160 `sceneInventory` (function): Counts the scene under `root`: a row per layer and kind of mesh, the flags, the totals.
- 163-177 `formatInventory` (function): The inventory as a table, the `top` largest rows first, then the flags.
