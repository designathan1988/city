# Porte da elevação das vias (pontes, viadutos, túneis, greide) — plano

Sessão de integração, `ports/integracao/workspace` (convert/integracao, base 7d4ab48). Referência de comportamento: Roadcraft 4.0,
commit 22357c92 (mesmo em `Documents/Labs` e `Documents/Projects`). Rastreio: `converter/out/features/road-elevation` (382 declarações
próprias, 66 arquivos, entradas: elevation.ts, structures.ts, render/structures.ts, structureCells.ts, junction/levels.ts,
roads/tuning.ts, roads/buildMode.ts, roadField.ts, ui/roads/cutWalls.ts). Cobertura: `ports/road-elevation/cobertura.md`.

## 1. O que li

- Rastreio inteiro: `elevation.ts` (1540), `roadField.ts` (751, 4.0 completo para `blend`), `render/structures.ts` (715),
  `structureCells.ts`, `world/structures.ts`, `levels.ts` (344), `commit.ts` (recorte e 4.0 linhas 1-160, 214-300, 369-520),
  `editRules`, `snap`, `buildMode.ts` (125, completo), `economy.ts` (1-80), `tuning.ts` (86), `main.ts` (recorte 187: prévia, modos,
  `setNodeHeightMetres`), `roadSurfaces/Reads/Pass/Inputs/Entries`, `renderer.ts` (recorte 705), `groundShape.ts`, `networkBuild`,
  `doc.ts`, `walkways`, `markings`, `walkingLines`, `surfaces`, `paving`, `lanelets`, `poles`, `poleRoutes`, `roadTileMesh`,
  `roadTool` (recorte), `ui/roads/cutWalls.ts`, `ui/v2/shell.ts` (recorte), `money.ts`, `editor/roads/economy.ts`.
- Teste do 4.0 `tests/world/elevation.spec.ts` (559): é a especificação de comportamento, portada como teste aqui.
- Jogo atual: `road_network.{h,cpp}`, `heightfield.{h,cpp}`, `road_edit.{h,cpp}`, `road_commands.cpp`, `road_plan.cpp`,
  `city_gen.cpp`, `edit_history.cpp`, `save_write.cpp`, `save_format.h`, `change_log.h`, `road_segment.cpp`, `road_verge.cpp`,
  `road_junction.cpp`, `road_look.h`, `road_mesh.h`, `verge_renderer.h`, `road_chunks.h`, `road_tool.gd`,
  `city_core_commands.cpp` (100-210), `rules.json` (camadas).

## 2. O que o 4.0 faz (comportamento a atingir)

1. **Campo único de altura por via**: estações a cada ≤1,6 m (máx. 400). Chão lido em 7 pontos laterais por estação:
   - `ceil`: o máximo, dilatado 2 estações;
   - `base`: a média, suavizada em 36 m e limitada a 12%.
2. **Placas planas nas junções**, do tamanho do recuo + casing + margem, limitadas a 45% do trecho. Nó articulado (joint) tem placa 0.
3. **Altura do nó**: média da `base` das vias de chão na borda das placas + `heightOffset` (a altura que o jogador deu). Sem via de
   chão, a média do chão num disco. Nó com todas as vias elevadas fica no ar (`aloft`), com o teto que uma rampa alcança.
4. **Resolvedores por perfil**:
   - **chão**: segue a `base`, puxada até o nó por uma transição dimensionada pela correção, rampa ≤12%, tangente da linha
     preservada na placa, curvas verticais;
   - **altura autoral**: `base` + offsets suavizados, rampa ≤16%;
   - **elevada/ponte**: tabuleiro reto acima do teto + folga (5,6/3 m), rampas de 16% com curva de 8 m, o tabuleiro baixa em vão
     curto em vez de inclinar a rampa;
   - **túnel**: piso plano abaixo do chão (folga −7 m), rampas de 13%, mais raso em vão curto.
   - Rodadas até os nós pararem (máx. 64).
5. **Curvas verticais** = média móvel de raio 4,8 m: sobre uma quebra de greide é exatamente uma parábola de tangentes iguais de
   9,6 m (provado em teste).
6. **O chão segue a via** (`roadShapeAt`): vence a maior autoridade, não a menor distância.
   - Sob a via, o chão fica 0,6 m abaixo da superfície; ao lado, 0,1 m.
   - Talude de |natural − via| × 2,5, entre 18 e 36 m; corte com muro de arrimo: ombro de 6,4 m.
   - Some sob o tabuleiro (0,6 → 2,8 m de altura) e sobre o furo do túnel (cobertura 5,24 → 5,56 m).
7. **Estruturas (render)**:
   - tabuleiro: laje até a face inferior, espessura 1,04/0,96 m;
   - pórticos de 2 colunas, espaçados 29,6/38,4 m, esbeltez 8, travessa 0,72 m, só fora de outra via;
   - encontros onde a rampa sai do chão (0,48 m);
   - guarda-corpo de concreto 0,88 m, crescendo de 0,64 a 1,28 m de altura sobre o chão;
   - muros de arrimo com reaterro;
   - emboques de túnel onde a cobertura cruza 0,6 m: alas que descem com o morro, verga e capeamento.
8. **Edição**:
   - altura do rascunho (PageUp/PageDown, metros inteiros), limitada a 12% × comprimento, com recusa se a ponta presa não alcança;
   - rótulo "+5,6 m · 8%" com perfil;
   - cruzamento por altura autoral (offset linear + folga da estrutura): >5,6 m passa por cima sem nó; 0,3–5,6 m é recusado
     (`clearance`); ≤0,3 m vira junção;
   - snap só em via na mesma altura (±0,3 m);
   - túnel automático quando a cobertura passa de 18 m (relevo esculpido);
   - muros de arrimo (opção da ferramenta);
   - altura do nó selecionado ±1 m, limitada pelo greide das vias dele (aviso `hint.road.gradeLimited`);
   - recusa acima de 35% de greide construído;
   - prévia com modos (chão, aterro, corte, ponte, túnel) desenhados e resumo "Ponte 120 m · Aterro 40 m";
   - preço por modo: aterro 1,4, corte 1,8, ponte 2,5, túnel 6.
9. **Calçadas, pavimento e tráfego** leem a altura do campo; calçadas de estruturas diferentes não se ligam.

## 3. O que o jogo tem hoje (causa dos defeitos)

1. **Altura linear entre nós** (`RoadNetwork::seg_y`); `RoadNode.y` vem do terreno na criação.
   - Por isso a via é cortada em pedaços para seguir o chão (`follow_ground` da cidade gerada) e o terreno é moldado à reta
     (modificadores).
   - É a causa de: rampa sem limite, quebra de greide sem curva, junção que não é plana (o render interpola nó → boca), nenhuma
     estrutura, via "no ar" com cortina de terra.
2. **Modificadores de relevo das vias gravados no diário e no save** (`conform_under`, `city_gen`): ficam velhos quando a altura de
   uma via muda por causa de outra.
3. **Cruzamento decidido pela diferença terreno × via** (`road_level_dy_m`), sem folga de estrutura nem recusa.
4. **A ajuda já anuncia "Page Up/Down → Altura da via"** na ferramenta de via; não existe.

## 4. Pesquisa (fontes primárias abertas)

| Fonte | O que diz | Uso |
|---|---|---|
| FHWA, *Technical Manual for Design and Construction of Road Tunnels — Civil Elements* (FHWA-NHI-10-034, 2009), §2.2.1 | "Maximum effective grades in main roadway tunnels preferably should not exceed 4%; although grades up to 6% have been used" | A rampa de 13% do 4.0 é escolha de jogo, registrada como tal |
| idem, §2.4 (gabarito) | AASHTO 2004: mínimo de 16 ft (4,9 m) em rodovias e 14 ft (4,3 m) em outras vias | Vão livre de 4,4 m no túnel ≥ 4,3 m |
| idem, §1.3.5 | "in a cut and cover tunnel, the portal can be as close to the surface as the roof of the tunnel can be placed with sufficient clearance for traffic" | Emboque onde a cobertura passa do teto (`TUNNEL_CLOSE`) |
| DNIT/IPR, *Manual de Projeto de Obras-de-Arte Especiais* (IPR-698, 1996), Quadro I e §3.2.5 | "Gabarito Vertical (m) Desejável/Mínimo 5,50/4,50"; a altura livre deve ter "folga mínima de um metro em relação ao gabarito vertical"; vigas pré-moldadas, "nas travessias de rios largos e sem exigência de gabarito de navegação", são "praticamente, uma solução padrão" | Os 5,6 m do 4.0 são até a superfície: sem a laje de 1,04 m sobram ~4,4 m livres, entre a AASHTO urbana (4,3) e o mínimo do DNIT (4,5). É escolha do 4.0, mantida pela paridade. Ponte da cidade gerada sobre rio: vigas, como a solução padrão do DNIT |
| PCI Journal, mar./abr. 1994, *Short to Medium Span Precast Prestressed Concrete Bridges in Japan* | vigas pré-moldadas pós-tensionadas "with span capabilities of 20 to 40 m"; pré-tensionadas até 21 m | Vãos de 29,6 e 38,4 m dentro da faixa |
| AASHTO (via exam guides; a forma em uso) | curva vertical de tangentes iguais: y = y0 + g1·x + (A/2L)·x² | A média móvel do 4.0 é exatamente essa parábola: prova por teste |

## 5. Arquitetura (decisões)

**Camadas** (`rules.json` é do usuário): `sim/roads` não inclui `sim/terrain`, nem o contrário.

- **Resolvedor** (`sim/roads/road_grade*.{h,cpp}`):
  - recebe o chão natural como função, como o `terrainAt` do 4.0;
  - os perfis moram na rede (`RoadNetwork::grade`), e `seg_y` lê o perfil, então os ~15 leitores de altura seguem sem mudar;
  - `RoadNode.y` passa a ser a altura resolvida; `RoadNode.lift` é a altura autoral (gravada);
  - a estrutura e os muros ficam nos bits livres de `RoadSegment.flags`.
- **Rastreio de mudanças na origem**: a rede guarda os trechos e nós cujas entradas mudaram (adicionar, remover, perfil, `lift`,
  estrutura); o heightfield guarda os retângulos do chão natural alterados (pincel, desfazer, carga). O resolvedor drena os dois.
- **Relevo derivado**:
  - `Heightfield` ganha um conjunto próprio de moldagens das vias (`road_shapes`, fora do diário e do save), aplicado em `recompose`
    com a regra `roadShapeAt` (autoridade);
  - a ligação perfil → moldagem fica em `sim/commands/road_grade_sync.cpp` (camada que inclui os dois lados), chamada por
    `finish_edit` (edições, desfazer, refazer), pela cidade gerada e pela carga;
  - `conform_under` e `conform_terrain` saem;
  - o disco da rotatória continua no diário.
- **Save v6**: o nó grava `lift` no lugar de `y`. Saves antigos: `lift = 0`; os modificadores gravados de via (polilinhas, e
  discos sobre nós) são descartados e derivados de novo; os demais (disco da rotatória) ficam.
- **Propagação**: um perfil que mudou anuncia chunks de via, `net_seg` (o tráfego refaz as faixas na altura nova),
  `furniture_segs`, e o retângulo do terreno.
- **Render**:
  - o trecho tem vértice em cada estação não colinear;
  - a saia vira a laje (até a face inferior) quando a via está no ar, e a beirada é omitida;
  - estruturas novas em `render/roads/road_structure*.cpp`, por chunk, instâncias para pilares, travessas, encontros e peças de
    emboque; extrusões para guarda-corpo e muro.

## 6. Camadas de trabalho (um commit por tema, cada uma com o ciclo completo)

1. **Greide (núcleo)**:
   - dados, resolvedor e `seg_y`;
   - constantes em `data/catalog/tuning.json` (`grade_*`, com fonte em `_doc`);
   - save v6;
   - teste `tests/core/road_grade_test` com os casos do `elevation.spec.ts`: plano, morro, rampa, tabuleiro, vão curto, vale,
     ponta solta, junções nos 18 arranjos, túnel; curva vertical = parábola.
2. **Relevo derivado**: moldagens das vias, migração dos saves, fim de `conform_under`/`conform_terrain`; teste da regra
   (sob o tabuleiro, sobre o furo, emboque) e de desfazer/refazer.
3. **Edição**:
   - altura na ferramenta (PageUp/PageDown na ferramenta de via; a câmera mantém as teclas fora dela) e rótulo;
   - nós com `lift` suavizado;
   - cruzamento por altura (passa, recusa, junta) e snap por altura;
   - altura do nó no inspetor; túnel automático; muros de arrimo; greide máximo; modos e preço na prévia.
4. **Estruturas no render**: estações, laje, beirada, pórticos, encontros, guarda-corpos, muros, emboques; fotos pareadas com o
   4.0 (`port.ts reference` + `hidden_run.ps1`, mesmas poses).
5. **Consumidores**: tráfego, pedestres, mobiliário, postes e transporte sobre o tabuleiro e no túnel (leem `seg_y`); conferir
   com fotos e contagens.

## 7. Rastreio verificado pelo oráculo

O teste definitivo do tradutor roda a cópia instrumentada (`tools/oracle-4.0`, `ORACLE_FEATURES` = a minha lista) com o meu roteiro
(`scratchpad/oracle-elev/scenario.mjs`):

- via a +5,6 m;
- via de chão por baixo;
- via a +3 m recusada;
- corte a −8 m com muro de arrimo;
- vistas.

Veredito R1–R4 e os dois controles, que devem falhar. O túnel automático exige relevo esculpido, fora do roteiro: o veredito só
cobra o que rodou.

**Resultado (12:31, 2026-11-05):**

- Execução: 22099 arestas, 0 erros de página, pilhas de volta à raiz. Rodaram 153 unidades das sementes, com extensão de 430 e 93
  entradas.
- **R1 = 0, R2 = 0, R4 = 0**: tudo o que a funcionalidade rodou está no rastreio.
- **R3 = 3**, todos o mesmo caso: o getter `LeanJunction.rings` (`levels.ts:72`), lido via a interface `Junction` por
  `sidewalk.ts#clearOfJunction`, `surfaceParts` e `surfaces.ts#levelRingsOf`.
  - É lacuna do grafo (leitura de propriedade de interface → get accessor da classe), relatada à sessão do tradutor.
  - O cache `LeanJunction` é infraestrutura do 4.0: não se aplica ao porte.
- **Controles**:
  - `--drop-own 0.2` discrimina: 38 de 38 removidas que rodaram foram pegas.
  - `--strip-platform` falha só pelos mesmos 3 R3, então não discrimina enquanto eles existirem.
- **Contagens do roteiro: resolvidas por medição** (`diag.mjs`, `oracle-diag.log`, fotos `diag-*.png`).
  - No 4.0 atual a unidade do mundo é o metro: o viaduto tem `heightOffset` de 5,6.
  - A via de chão que cruza o viaduto foi **recusada** com o aviso: "cruzaria uma via com diferença de altura pequena demais para
    se unir a ela ou passar por cima. Suba ou desça a via (Page Up/Page Down)".
  - A folga de exatamente 5,6 m não basta; o código exige `gap > CROSSING_CLEARANCE`. Para passar por baixo é preciso descer a via.
  - As vias de +3 m e −0,4 m foram construídas porque, com a câmera nesse azimute, os cliques "horizontais" viraram vias paralelas
    ao viaduto (x = 620 e 740 contra 682): não cruzaram nada.
  - A regra a portar: folga > 5,6 m passa por cima; folga entre 0,3 e 5,6 m é recusada; folga ≤ 0,3 m vira junção.
- Logs: `scratchpad/oracle-elev/` (`veredito_*.log`, `oracle/calls.json`).

**Com o grafo e o rastreio refeitos pelo converter do master** (`tools/converter` 0d666d3, que liga a leitura de propriedade de
interface aos accessors das classes; `rerun.sh`). O rastreio novo tem 373 declarações próprias.

- **REAL: PASS**, R1 = R2 = R3 = R4 = 0 (153 unidades das sementes, extensão 430, 93 entradas).
- **`--drop-own 0.2 --seed 1`: FAIL**. As 26 removidas que rodaram foram pegas (R1 42, R2 50): o teste discrimina.
- **`--strip-platform`: PASS**.
  - Pelo `verdict.mjs`, a lista `platform` só é consultada numa entrada vinda do navegador, do three.js ou de código compartilhado
    sem aresta no grafo.
  - Com ela vazia, as 93 entradas continuaram conhecidas: nenhuma dependeu da plataforma nesta execução.
  - **Provado no grafo do master** (`graph-master.json`, arestas executáveis): as 10 entradas de `platform` têm todas
    chamadores no próprio programa:
    - os métodos de `ReadonlyMap` de `JunctionLevels` têm de 3 a 35 chamadores, inclusive o `buildRoadElevation`;
    - o `made` é chamado pelo `RecentCache.get`.
  - Toda entrada nelas é explicada por aresta antes de a lista ser lida. Nesta funcionalidade, o `--strip-platform` não tem
    como falhar: é propriedade da funcionalidade, não fraqueza do teste. O controle que discrimina aqui é o `--drop-own`.
- O `cobertura.md` é gerado do rastreio novo.
