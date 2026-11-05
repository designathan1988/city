# Cobertura do porte economy

Escrita antes de codificar e atualizada ao fim do porte. `buildLengths` está portado, mas toda via vale como ao nível do chão até o greide existir (`built` em `road_economy.cpp`).

Rastreamento próprio: `node scripts/port.ts extract economy src/world/economy.ts src/editor/roads/economy.ts src/ui/roads/money.ts src/world/roads/buildMode.ts`. As duas últimas entradas faltavam no rastreamento da coordenadora: sem elas saíam a formatação do dinheiro, a barra, o rótulo na ponta da via e `fillFrom`. Resultado: 34 declarações próprias, 32 fatias, 14 arquivos.

Comportamento medido no 4.0 (cidade pequena, semente 7, `reference/report.json`):

| Passo | Saldo |
|---|---|
| Início | 20.000.000 |
| Via urbana de 500 m, prévia | R$ 760.960 |
| Depois de construir | 19.239.040 (cobrou a prévia exata) |
| Demolir essa via | 19.429.280 (+190.240, 25 % exatos) |
| Desfazer | 19.239.040 |
| Demolir via gerada pela cidade | 19.292.320 (+53.280) |

O último passo contraria o jogo: `SEG_FREE` faz a via gerada não valer nada.

## Arquivos

| Arquivo | O que faz para o jogador | Estado | Dono no jogo |
|---|---|---|---|
| `src/world/economy.ts` | preço, valor das vias, cobrança | portado | `native/src/sim/economy/road_economy.cpp` |
| `src/editor/roads/economy.ts` | paga a edição ou recusa | portado | `road_economy.cpp` |
| `src/ui/roads/money.ts` | dinheiro como o jogador lê | portado | `project/game/store/formatters.gd` |
| `src/world/roads/buildMode.ts` | como cada trecho é construído | não se aplica: `road-elevation` | depende do greide, que não existe aqui |
| `src/editor/bulldozer.ts` | demolir devolve parte | portado | `demolish_road` em `road_commands.cpp` |
| `src/editor/commit.ts` | prévia, recusa e cobrança | portado | `road_commands.cpp`, `road_plan.cpp` |
| `src/editor/editRules.ts` | recusa `funds` nas edições | portado | `road_tabs.cpp`, `roundabout.cpp` |
| `src/editor/persistence.ts` | recusa saldo inválido | portado | `save_read.cpp` |
| `src/editor/roadTool.ts` | custo da via na mão | portado | `project/game/tools/road_tool.gd` |
| `src/main.ts` | rótulo na ponta, saldo na barra | portado: rótulo; já existia: barra | `roadcraft_hud.gd` |
| `src/render/structures.ts` | muros de arrimo | não se aplica: `road-elevation` | entrou pelo `buildMode.ts` |
| `src/ui/roads/catalogPanel.ts` | preço por metro no cartão | já existia | `road_catalog.gd` |
| `src/ui/v2/shell.ts` | saldo e dica na barra | já existia | `roadcraft_hud.gd` |
| `src/world/doc.ts` | saldo no mapa, desfazer, save | já existia | `CityState::money`, `j_money`, `save_write.cpp` |

## Declarações próprias

| Declaração | O que faz | Estado | Dono no jogo |
|---|---|---|---|
| `RoadsBefore` | valor e saldo antes da edição | não se aplica: o diário da edição já diz o que mudou | `road_economy.cpp` |
| `roadsBefore` | idem | não se aplica: idem | `road_economy.cpp` |
| `settleRoadEdit` | cobra ou recusa a edição | portado | `settle_road_edit` |
| `balanceTip` | dica do saldo com a parte devolvida | já existia | `roadcraft_hud.gd`, `ui.money.tip` |
| `formatCost` | preço, ou "devolve" quando negativo | portado | `Formatters.format_cost` |
| `formatMoney` | dinheiro pelo idioma | portado: hoje "R$" e o ponto são fixos | `Formatters.format_money` |
| `RoadDoc.forkOrigin` | cópia de trabalho do mapa | não se aplica: aqui a edição é desfeita pelo diário | `EditHistory` |
| `RoadDoc.setBalance` | muda o saldo no diário | já existia | `j_money` |
| `BUILD_KINDS` | os cinco modos de construir | portado | `road_economy.h` |
| `BuildKind` | idem | portado | `road_economy.h` |
| `DEFAULT_ECONOMY` | saldo inicial | já existia | `tuning.json` `economy.start_money` |
| `E` | atalho para a tabela | não se aplica: é apelido | — |
| `Economy` | o saldo | já existia | `CityState::money` |
| `Ledger` | soma dos preços pelo diário | não se aplica: a soma do mapa não precisa ser guardada | `edit_value_change` lê só a edição |
| `PER_SQUARE_UNIT` | preço por unidade do mundo | não se aplica: aqui tudo é em metros | `road_profiles.json` |
| `RoadCharge` | valor, saldo depois, se pode pagar | portado | `road_economy.h` |
| `buildLengths` | quanto da via é construído de cada modo | portado | `build_lengths` |
| `buildLengths.share` | parte da via numa faixa de altura | portado | `build_lengths` |
| `chargeFor` | a regra da cobrança | portado: hoje repetida em três arquivos | `charge_for` |
| `ledgers` | ver `Ledger` | não se aplica | — |
| `priceAll` | ver `Ledger` | não se aplica | — |
| `priceOf` | ver `Ledger` | não se aplica | — |
| `pricePerUnit` | preço por metro da seção | já existia; confere pelo catálogo | `derive_profile` |
| `readEconomy` | recusa saldo que não é número | portado | `save_read.cpp` |
| `roadValue` | valor de todas as vias | não se aplica: ver `Ledger` | `edit_value_change` |
| `segmentCost` | preço de uma via | portado | `build_value` |
| `segmentPrice` | idem, com o comprimento | portado | `recorded_value` |
| `BuildMode` | modo num ponto | não se aplica: `road-elevation` | — |
| `BuildRun` | trecho de um modo | não se aplica: `road-elevation` | — |
| `RoadStation` | ponto da via em construção | não se aplica: `road-elevation` | — |
| `buildModeAt` | modo num ponto | não se aplica: `road-elevation` | — |
| `buildRuns` | trechos por modo | não se aplica: `road-elevation` | — |
| `orderAlong` | ordem dos pedaços | não se aplica: `road-elevation` | — |
| `stationsAlong` | pontos da via | não se aplica: `road-elevation` | — |

## Fatias de código compartilhado

| Fatia | Estado | Dono no jogo |
|---|---|---|
| `removeRoads` | portado | `demolish_road` |
| `DraftResult`, `commitRoadPath`, `layCopy` | portado | `plan_road`, `build_road` |
| `moveNodeChecked` | não se aplica aqui: Mover está em `convert/roads`; `road_move.cpp` deve passar a usar `settle_road_edit` | relatório |
| `draftStations`, `drawBuildModes`, `drawTunnelStretches`, `BUILD_FILL`, `buildSummary`, `retainingWalls` | não se aplica: `road-elevation` | — |
| `guardRoadEdit` | portado | `settle_road_edit` |
| `isSerializedDoc` | portado | `save_read.cpp` |
| `RoadTool.cost`, `judge`, `judged`, `stations`, `verdict` | portado: custo; o resto já existia em `preview_road` | `road_tool.gd` |
| `RoadTool.layPart` | não se aplica: perfil próprio na via não existe no jogo | — |
| `currentStatus`, `syncMoney` | já existia | `snapshot`, `roadcraft_hud.gd` |
| `drawOverlayScreen` | portado: rótulo na ponta da via | `roadcraft_hud.gd` |
| `catalogStrip` | já existia | `road_catalog.gd` |
| `RoadDoc.economy`, `valueOf`, `restoreValue`, `savedGlobals`, `worldPart`, `fromJSON`, `layOver`, `takeFork`, `replaceContents` | já existia | `CityState::money`, `save_write.cpp`, `edit_history.cpp` |

## Recursos

| Recurso | Estado | Dono no jogo |
|---|---|---|
| `catalog.meta` | já existia | `strings.csv` |
| `hint.rule.funds` | portado: substitui `ui.insp.noMoney` | `strings` |
| `rule.short.funds` | portado | `strings` |
| `rule.short.*` (outros motivos) | portado pelo texto curto que o jogo já tem (`road.reason.*`, `refuse.road.*`) | `road_tool.gd` |
| `economy.money` | já existia, mas sem uso: `Formatters` ignorava | `strings.csv` |
| `economy.refund` | portado | `strings` |
| `economy.balance` | já existia como `ui.money.tip`; texto a conferir | `strings_c.csv` |
| `buildMode.*` | não se aplica: `road-elevation` | — |
| `ground`, `embankment`, `trench`, `bridge`, `tunnel` | já existia | `road_profiles.json`, agora por estilo |
| `fillFrom`, `pierFrom` | portado | `tuning.json`: `fill_from_m`, `on_piers_from_m` |

## Receitas

O 4.0 não tem imposto, tarifa nem manutenção. A única entrada de dinheiro é a devolução ao demolir. Nada portado além disso.
