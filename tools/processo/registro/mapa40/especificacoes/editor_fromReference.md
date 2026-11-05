# editor:fromReference

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/fromReference.ts`
- Unidades próprias (só esta funcionalidade alcança): 10 em 1 arquivo(s), 228 linhas
- Alcance total: 97 unidades; usa 87 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/buildingsWiring.ts#createBuildingWiring@81` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:buildings` (73), `editor:barriers` (11)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/editor/fromReference.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/editor/fromReference.ts
- 14-20 `ReferenceBlock` (interface)
- 22-26 `ReferenceOptions` (interface)
- 28-28 `TOUCH` (variable)
- 31-74 `blocksFromTriangles` (function): The model's blocks, from its upward faces.
- 76-76 `luminance` (function)
- 77-78 `toHex` (function)
- 79-79 `distance` (function)
- 82-220 `buildFromReference` (function): The building the reference shows; null when the model has no block big enough to build.
- 223-233 `normalisedOutline` (function): A block's hull in its own box, 0..1, without points closer than a few centimetres.
- 235-251 `convexHull` (function)
