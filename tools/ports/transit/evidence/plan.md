# Plano do porte: transit

Clone `ports/transit/workspace`, branch `convert/transit`, a partir do master a11384a. Este clone NÃO tem o trabalho do `convert/traffic`
(paradas na guia, `v.stop_at`, carroceria nova): a integração dos dois fica para a coordenadora, e o ônibus aqui para na faixa com
regra própria, no mesmo ponto em que `kerb_stops.cpp` do traffic entra depois.

## Fonte do comportamento
`src/world/transit.ts` (dados, edições, grafo de trilhos, busca), `src/sim/transit/transit.ts` (ônibus, trens, passagens de nível),
`src/editor/transitTools.ts` (ferramentas), `src/render/transit.ts` (trilho, estações, paradas, trens), `src/ui/v2/transitLine.ts`.

## Arquitetura
| Camada | Dono | O quê |
|---|---|---|
| sim/transit/transit_data.h + .cpp | dados | paradas, trilhos, linhas; edições puras; índice por célula; grafo de trilhos, Dijkstra, trilho mais perto, trecho sobre a via |
| sim/commands/transit_commands.cpp | comandos | parada, terminal, trilho, estação, linha, veículos, remoção: um passo de desfazer com o antes e o depois inteiros (OpKind::TRANSIT) |
| sim/persistence | save | versão 2 com a seção de transporte; lê a versão 1 |
| sim/transit/transit_sim.cpp | simulação | ônibus de cada linha (veículos do tráfego), trens numa volta de ida e volta, intervalo entre trens, passagens de nível fechando faixas |
| render/transit | render | trilho (lastro, dormentes, trilhos), plataforma e cobertura, abrigo do ônibus, entrada do metrô, vagões em MultiMesh |
| game/tools/transit_tool.gd, ui/transit_panel.gd | jogador | parada, terminal, trilho, estação, linha; lista de linhas com veículos |

## Fases, cada uma um commit que compila
1. Dados, comandos, save, API e teste de ida e volta.
2. Simulação: ônibus e trens, passagem de nível, sonda e teste.
3. Render.
4. Ferramentas e painel.
5. Rodada do 4.0, fotos, relatório.
