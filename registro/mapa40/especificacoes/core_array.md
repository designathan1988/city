# core:array

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/arrayPack.ts`
- Unidades próprias (só esta funcionalidade alcança): 13 em 1 arquivo(s), 78 linhas
- Alcance total: 13 unidades; usa 0 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/arrayPack.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/arrayPack.ts
- 4-6 `KINDS` (variable): The typed arrays a pack holds, by their tag in the head.
- 7-7 `Kind` (type)
- 10-10 `PackableArray` (type): Any typed array a pack can hold.
- 13-16 `ArrayPack` (interface): What a pack holds: its numbers and its arrays by name (the arrays views on the pack's buffer).
- 19-22 `PackHead` (interface): The head: the numbers, and each array's name, kind, element offset past the head (in bytes) and length.
- 24-24 `align8` (function)
- 26-29 `kindOf` (function)
- 32-42 `layoutOf` (function): A pack's layout: its head's text (a u32 length before it), where the arrays start, and the bytes in all.
- 45-55 `packArrays` (function): The numbers and arrays laid in one buffer: a u32 head length, the head's JSON, then each array at an offset of 8.
- 58-73 `packArraysBlob` (function): `packArrays`'s very bytes as a Blob made straight from the arrays (MDN "Blob()": its parts copied once, into the Blob): DO NOT pack into a buffer to put it in a Blob, two copies of every array where one is kept (a tile's link table, written
- 76-87 `unpackArrays` (function): A pack read back: its arrays views on `buffer` (the buffer's own, from its start).
- 90-94 `packedArray` (function): One array of a pack, of the kind its reader expects (a pack of another layout throws, never read as garbage).
- 97-101 `packedNumber` (function): One number of a pack.
