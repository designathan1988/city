# Plano do porte `road-elevation`

Estado: passou para a coordenadora às 20h de 2026-11-04, por ordem do usuário. Há código no branch, sem prova visual. Cobertura em `cobertura.md`, ainda com todas as linhas em `a portar`: não foi atualizada.

## Onde parei

O código abaixo foi escrito antes de chegar a ordem de parar. Está no commit de trabalho de `convert/road-elevation`, em `ports/road-elevation/workspace`, sem push. A base é `edc8cc4`: o merge do master ainda não foi feito.

**Provado:** `tests/core/road_structure_test`, 11 verificações, 0 falhas (`scratchpad/e1.log` da sessão, resumo aqui): tabuleiro a 5,60 m, pé da rampa no chão, rampa em 0,160, via do chão passa por baixo sem nó, desfazer volta a 438 trechos, refazer na mesma altura, ponte a 3,00 m. DLL de release compilada. Gates OK antes do último build.

**Não provado:**
- **O viaduto não aparece na foto.** `evidence/fotos-e1/structure_side.png` mostra uma via no chão acabando na grama e, ao fundo, uma via comprida, sem pilares visíveis. A sonda do log dá `roads.chunks 0`. Não sei se é a câmera, a espera da foto ou o desenho: não investiguei, parei aqui. É a primeira coisa a resolver.
- Nenhuma das 5 fotos de `fotos-e1` foi lida contra o 4.0, além dessa.
- Testes completos, DLL de debug e custo não rodaram neste branch.
- `capture_structures.gd` usa `SPAN_M 360` e `CROSS_M 120`. Para casar com `look-par16.json` são cerca de 254 e 104.

**Referência do 4.0 pronta:** `reference/37-viaduto-low.png`, `40-viaduto-top.png`, `43-viaduto-side.png`, por `ports/roads/look-par16.json` com `setRoadHeight(5.6)`. O relatório teve 1 erro ou aviso no console, não lido. Na lateral: tabuleiro claro de cerca de 1 m com mureta nos dois lados, pilares redondos em pares, postes sobre o tabuleiro. Os pilares do 4.0 são redondos: conferir contra `emit_structure`.

## O que existe no branch

- **Estado:** `RoadStructure{GROUND,ELEVATED,BRIDGE}` em `sim/roads/road_grade.{h,cpp}`, guardada em `flags` (`SEG_STRUCTURE_MASK 0x300`). `RoadSegment.lift`: metros sobre a corda, em passos iguais. `seg_y` soma `height_on(lift, share)`, então todos os 10 leitores de altura seguem sem mudar.
- **Save:** `SAVE_VERSION 4`, `LIFT_VERSION`; saves antigos abrem como chão.
- **Comandos:** `structure` em `plan_road`, `build_road`, `build_path`, `plan_path`; `RoadPlan.heights`; `find_crossings` e `overlaps_road` por nível; preço por `bridge/ground`; `conform_under` pula onde a via está acima de `terrain.lift_on_m`; dividir mantém o perfil; `road_tabs` inverte o `lift`.
- **Render:** `render/roads/road_structure.cpp` (`emit_structure`: fundo, lados, mureta, pilares), `structure_renderer.{h,cpp}`, `JobLane::STRUCTURES`. O talude pula a via no ar.
- **Dados:** `road.clearance_elevated_m 5.6`, `clearance_bridge_m 3`, `ramp_grade .16`, `deck_grade .05`, `ramp_curve_m 8`, `station_m 1.6`, `terrain.lift_on_m .6`; `streets.json` `structures`, com schema.
- **Interface:** linha `level` nas opções da via (`road_options.gd`, `drawers.json`, `palette.level*`).
- **Testes:** `tests/core/road_structure_test`, `tests/visual/capture_structures`.

## Não feito

Túnel (precisa de furo no Terrain3D e portais), muros, encontro da ponte, altura escolhida à mão, greide no chão com linha suavizada, solução incremental (`SolveScope`), `roadShapeAt` com ombro de 18 a 36 m, cidade gerada.

## O que aprendi

- `seg_y` é o dono único da altura: `road_emit`, `lane_geometry`, `overlay_build`, `road_plan`, `road_edit` e as sondas leem dele. Por isso o `lift` entrou ali, sem campo novo nos leitores. As Portes 5 e 6 recebem a altura sem mudar nada, mas vale conferir pedestres e mobiliário no tabuleiro.
- `flags` tinha bits livres: estrutura não precisou de campo novo, só o `lift`.
- `lift` sobre a corda, e não altura absoluta, faz o perfil acompanhar quando um nó se move.
- `setRoadHeight` do 4.0 dá viaduto direto, o que basta para o par de fotos.
- `JobLane::STRUCTURES` é lane nova: soma com `VERGES`, `CANYON` e `SHORE` no merge.
- `city_gen.cpp` é da Porte 2: não mexi.
- Vão estaiado: a busca em `feature.md` não achou nada, e a busca no `src/` também não achou uso real de ponte estaiada. Os arquivos que deram resultado eram outro assunto.

## Base

`convert/road-elevation` avançou até `edc8cc4`, a ponta de `convert/roads`, sem merge: era avanço direto de 86 commits. O motivo é que os dois portes mexem nos mesmos arquivos: `road_segment.cpp`, `road_mesh.h`, `road_edit.cpp`, talude e terreno. Quando `roads` entrar no master, os commits são os mesmos e o merge é trivial.

## O que existe hoje

- A altura de uma via é uma reta entre dois nós: `RoadNetwork::seg_y`, `road_network.cpp:173`.
- `RoadSegment` não tem estrutura. Não há ponte, túnel nem viaduto em `sim/roads`. Em `road_profiles.json` só há os preços `bridge` e `tunnel`.
- O terreno se ajusta por `TerrainModifier`: `conform_under` em `road_edit.cpp`, e `city_gen.cpp` da Porte 2 com `half + pad`.
- A linha de `CLAUDE.md` que diz "Pontes têm tabuleiro e pilares" não bate com o código de `sim/roads`. Conferir no `render` antes de corrigir a pendência.

## O que o 4.0 faz

Fonte: `src/world/elevation.ts` (1.540 linhas), `structures.ts`, `junction/levels.ts`, `render/structures.ts`, `render/structureCells.ts`.

- **Um campo de altura para toda via.** O perfil é amostrado a cada 1,6 m, no máximo 400 estações. O chão é lido em 7 pontos por estação, a linha de greide é suavizada em 36 m e amarrada aos nós.
- **Quatro estruturas:** `ground`, `elevated`, `bridge`, `tunnel`, com folga, espessura de tabuleiro e pilares próprios.
- **Rampas:** 16% na parte reta, curva vertical de 8 m no pé, 12% no chão, 13% no túnel, 5% no tabuleiro.
- **Solução incremental:** o diário da rede diz o que mudou, e só o que mudou é resolvido de novo (`SolveScope`, `pendingOrder`).
- **Terreno pela via:** segurado 1,2 m além do casing, 0,1 m abaixo da superfície ao lado e 0,6 m por baixo, com ombro de 18 a 36 m (`roadShapeAt`).
- **Níveis de cruzamento:** vias em alturas diferentes se cruzam sem nó (`junction/levels.ts`).

## Plano, por camadas

Cada etapa compila, passa nos gates e nos testes, e fecha com commit próprio.

1. **Dados e estado.** `RoadStructure` e a altura escolhida em `RoadSegment`, com dono em `sim/roads`. Valores de `ROAD_TUNING` em `data/catalog/tuning.json`, nomes em `tuning_fields.h`. `SAVE_VERSION` novo, lendo saves antigos como `ground`.
2. **Perfil da via.** `sim/roads/road_profile.{h,cpp}`: estações de altura por via, e `seg_y` passa a ler o perfil. É aqui que entra todo leitor de altura: render, tráfego, pedestres, mobiliário. Determinístico, recalculado ao abrir, sem gravar estações.
3. **Solução.** Nós, rampas, tabuleiros e curvas verticais em `sim/roads/road_grade.{h,cpp}`, dividido por responsabilidade, sem arquivo-deus: o `elevation.ts` tem 1.540 linhas e o limite aqui é 350 por arquivo. Incremental pelo `ChangeLog`.
4. **Terreno.** `conform_under` e `city_gen` passam a ler a mesma regra (`roadShapeAt`), com ombro de 18 a 36 m. `city_gen.cpp` é da Porte 2: combinar a mudança pela coordenadora, sem editar.
5. **Níveis.** Cruzamento sem nó quando a diferença de altura passa do limite, no plano da via e na prévia. Depende do `road_plan.cpp`.
6. **Render.** Tabuleiro, pilares, muros e portal de túnel em `render/roads/`, por chunk, com instâncias e dentro dos orçamentos. O talude deixa de existir onde a via está no ar.
7. **Ferramenta.** Altura e Corte nas opções da via, com prévia. Dados em `data/ui`, textos em `data/i18n`.
8. **Tráfego e pedestres.** Altura pelo perfil. Pedestre não entra em túnel sem calçada, conforme o 4.0.

Pendências do `CLAUDE.md` cobertas: vão estaiado e rodovia elevada sobre terra (viadutos). O rastreamento não tem vão estaiado: a busca por `stay`, `cable`, `pylon` e `suspension` em `feature.md` não achou nada. Falta procurar no `src/` do 4.0 antes de dizer que ele não existe lá.

## Provas

- Fotos do 4.0 por `port.ts reference`, com pose exata e preset `high`: rampa, viaduto, ponte, túnel, cruzamento em níveis. Lista escrita antes de cada rodada.
- Contagens por `road_mesh_counts`, estendida a tabuleiro e pilares.
- Teste novo em `tests/core`: greide máximo, nós, ida e volta do save, desfazer.
- Tempo de quadro com a máquina quieta, com comando registrado.

## Riscos

- Mexer em `seg_y` alcança tráfego, pedestres e mobiliário, que são de outras portes: a Porte 5 e a Porte 6 precisam saber antes da etapa 2.
- `sim/citygen` é da Porte 2.
- O custo de recalcular perfis numa cidade grande precisa de medida antes da etapa 3.
