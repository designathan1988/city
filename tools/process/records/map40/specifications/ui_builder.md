# ui:builder

Grupo: interface. Entrada: painel. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/builder/catalog.ts`, `src/ui/builder/icons.ts`, `src/ui/builder/workspace.ts`
- Unidades próprias (só esta funcionalidade alcança): 5 em 2 arquivo(s), 282 linhas
- Alcance total: 99 unidades; usa 67 compartilhadas de outras; é dona principal de 27 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `builder.field.material`
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/buildingsWiring.ts#READY@1232` (2), `src/buildingsWiring.ts#createBuildingWiring@81` (2), `src/buildingsWiring.ts#BuildingWiring@49` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `ui:i18n` (17)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/ui/builderCatalog.spec.ts`, `tests/ui/builderWorkspaceDom.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- porte `interface`: outra sessão 8
- porte `interior`: portado 6, outro porte 10

## Código próprio (linhas — declaração — comentário)

### src/ui/builder/catalog.ts
- 113-115 `tabSpec` (function)
- 118-119 `BUILDER_CATALOG` (variable): Every tab with its tools, flat: the shape the tests and the wiring read.
- 151-166 `OPENING_COMPONENTS` (variable): The BayComponent an opening tool places.

### src/ui/builder/workspace.ts
- 110-115 `el` (function)
- 117-371 `initBuilderWorkspace` (function)
