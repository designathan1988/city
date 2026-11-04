# Fila do porte

Base: `src/` do 4.0, Pendências de `fonte/CLAUDE.md` e `converter/out/features/`. Ordem por dependência; até 6 filhas ao mesmo tempo (ordem do usuário, 2026-11-04), cada funcionalidade no seu clone; conflito se resolve na integração. As filhas são sessões do app (o `claude -p` lançado do app responde "Not logged in").

Estados: rastreada, em porte, entregue, verificada, integrada.

| Funcionalidade | Sessão | Estado | Observação |
|---|---|---|---|
| roads | porte-roads | em porte | continua o trabalho sem commit do clone; donos: `sim/roads`, `sim/commands`, `sim/persistence` |
| build | porte-build | em porte | continua o trabalho sem commit do clone; donos: `sim/buildings`, `render/buildings` |
| landscape | porte-landscape | em porte | absorve `clouds`; donos: `render/trees`, shaders de vegetação; fotos feitas |
| interface | porte-a-interface | em porte | 1ª da sessão A; absorve `start-menu`, `camera`, painel de gráficos e qualidade |
| demolition | porte-a-interface | na fila da sessão | 2ª da sessão A; absorve `bulldozer` |
| economy | porte-a-interface | na fila da sessão | 3ª da sessão A |
| traffic | porte-b-traffic | em porte | 1ª da sessão B; estacionar, docas, embarque, coordenação, ciclistas |
| street-furniture | porte-b-traffic | na fila da sessão | 2ª da sessão B; absorve `poles` e `barriers` |
| people | porte-b-traffic | na fila da sessão | 3ª da sessão B; refazer a partir do master |
| road-elevation | porte-c-elevacao | em porte | 1ª da sessão C; pontes, túneis, viadutos, greide |
| transit | porte-c-elevacao | na fila da sessão | 2ª da sessão C; ônibus, linhas, metrô |
| destruction | porte-c-elevacao | na fila da sessão | 3ª da sessão C; bomba, cratera, efeitos |
| marcos | porte-c-elevacao | na fila da sessão | 4ª da sessão C; 38 projetos de `lots/*.json` por `lotLibrary.ts`; estádio a conferir: `stadium` não aparece em `src/` |

## Cobertura medida (2026-11-04)
Fora `src/lab` e `src/peopleLab`: 728 arquivos, 195.006 linhas; 80,4 % tocadas por algum rastreamento; 102 arquivos (9.284 linhas) sem nenhum, em boa parte infraestrutura do motor web (perfilador, cache, WebGPU). A conferir, por serem possivelmente visíveis ao jogador: `src/render/wear.ts`, `src/world/cityGen/streets.ts`, `src/render/bake/detailRecipes.ts`.
