# Fila do porte

Base: `src/` do 4.0, Pendências de `fonte/CLAUDE.md` e `converter/out/features/`. Ordem por dependência; no máximo 3 filhas ao mesmo tempo, cada uma no seu clone; os lotes são só ordem, e conflito se resolve na integração. As filhas são sessões do app, uma por clone (o `claude -p` lançado do app responde "Not logged in").

Estados: rastreada, em porte, entregue, verificada, integrada.

| Lote | Funcionalidade | Estado | Observação |
|---|---|---|---|
| 1 | roads | em porte | continua o trabalho sem commit do clone; donos: `sim/roads`, `sim/commands`, `sim/persistence` |
| 1 | build | em porte | continua o trabalho sem commit do clone; donos: `sim/buildings`, `render/buildings` |
| 1 | landscape | em porte | continua o trabalho do clone; donos: `render/trees`, shaders de vegetação; absorve `clouds`; faltam fotos de referência |
| 2 | interface | rastreada | absorve `start-menu` e `camera` |
| 2 | demolition | rastreada | absorve `bulldozer` |
| 3 | traffic | rastreada | estacionar, docas, embarque |
| 3 | people | rastreada | refazer pessoas a partir do master; branch antigo foi apagado |
| 3 | street-furniture | rastreada | absorve `poles` e `barriers` |
| 4 | transit | rastreada | ônibus, linhas, metrô; `ports-arquivo/transit-parado-2026-11-04` tem trabalho antigo |
| 4 | road-elevation | rastreada | pontes, túneis, viadutos, greide |
| 4 | economy | rastreada | |
| 5 | destruction | rastreada | bomba, cratera, efeitos; `interior-bomba` foi apagado |
| 5 | marcos | a rastrear | 38 projetos de `data/lots-roadcraft` e o estádio |
