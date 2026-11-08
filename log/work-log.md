# Registro de trabalho — para outra sessão tocar daqui

Atualizado em 2026-11-05 pela sessão de porte (elevação, postes, tradutor). Leia o `CLAUDE.md` da raiz antes; ele manda nas regras.

## Onde está o trabalho (fechamento, 2026-11-05)

- **Tudo desta sessão está no `main`** (commits e push no `origin/main`; ver o fim do histórico).
  - O worktree `C:\Users\Jonathan\Desktop\city-worktrees\elevacao` (branch `porte/elevacao`) ficou para trás e pode ser apagado.
  - Os commits não foram feitos nele porque o hook de commit lê o `dist/tests` da pasta do projeto da sessão (`CLAUDE_PROJECT_DIR`), não do worktree.
  - A mudança foi aplicada como patch na pasta do `main`, as DLLs refeitas e a suíte rodada lá.
- **Stash compartilhado:** a entrada `aa558b4d` ("porte postes e structure_spec") já foi aplicada e pode ser apagada; o `git stash drop` foi negado pelo modo de permissões.
- **Pendências do `main` que não são desta sessão:**
  - mudanças do conversor em `tools/converter` (a sessão dele diz que o repositório próprio está limpo; o que está na pasta do jogo é a cópia mais nova, a trazer por `git read-tree`);
  - o gate `DOC_UNICO` acusa os documentos de `log/`, `tools/ports`, `tools/oracle-4.0` e `tools/converter`. Liberar é uma exceção em `rules.json`, que só o usuário altera.
- **Scratchpad desta sessão** (logs e cópias):
  - pasta: `C:\Users\Jonathan\AppData\Local\Temp\claude\26c8cd2f84403c71\6266e3d7-e77a-4554-af1a-13c6b872f74c\scratchpad`;
  - o que há nela: `conv-areas.sh`, `conv-areas.log`, `conv-areas/<área>/conversion`, `converter-snap/`, `pole-before/`, `pole-after/`, `pole-diff/`.

## Feito (commitado e no `origin/main`)

- **Road grade:**
  - `native/src/sim/roads/road_grade*` e `road_shape`;
  - `road_grade_test` checks: hill 0.628608, node 4.82363, ramps 99/99 and 98/100, deck 0.6456;
  - regras das placas: junção, ponta sem saída, transição, emenda reta.
- **O raio acerta o que o jogador vê:** `RoadNetwork::ray_deck` (o tabuleiro antes do chão). A câmera usa a mesma regra (`city_camera.gd _ground_height`).
- **Ferramentas:**
  - `tools/build/runq.sh` (fila vigiada, ciclo `fast`), `check_scripts.gd`, `gates_changed.sh`, `photodiff.py`, `rename_english.py`;
  - `project/tests/visual/look.gd`, `look_measure.gd` (looking at the game by a plan).
- **Nomes de pasta em inglês:** `tools/process`, `tools/ports`, `coverage.md`, `constants.json`.

## Feito nesta rodada (commitado no `main` no fechamento)

1. **`structure_spec`:** `StructureSpec structure_spec(Structure, const Tuning &)` em `sim/roads/road_grade.h` é o dono único da folga, da espessura e dos apoios de cada estrutura.
   - O `GradeTune` guarda as quatro estruturas, e saíram `structure_clearance`/`structure_deck`.
   - Chaves novas: `grade.deck_ground_m` 0,22 e `grade.deck_tunnel_m` 0,24 (`tuning_fields.h` e `tuning.json`).
   - Na library do tradutor (`tools/ports/road-elevation/conversion-library.json`): o tipo `RoadStructureSpec`, a função `roadStructure` e o padrão `ROAD_STRUCTURES[$s]`.
   - Testes `road_grade`, `road_commands` e `street_works` passaram.
2. **Urban and rural pole** (CELESC E-313.0002, structures B1, N1 and S1):
   - **Dados:** `tools/assets/make_pole_parts.py` gera as peças dos estilos `roadcraft` (8,5 m) e `brasil` (10,2 m) em `streets.json`.
     - Urbano (bits `ARM` e `CAN`): cruzeta de 0 a 2 m sobre a rua, isoladores a 0,55/1,25/1,85, mão-francesa do lado da rua, rack S1 com dois carretéis a 0,22 m e 1,3/1,5 m abaixo da coroa, transformador do lado oposto.
     - Rural (bits `ARM_RURAL` 32 e `CAN_RURAL` 64): cruzeta de −1 a +1, isoladores a −0,95/0,35/0,95.
     - Luminária 2,0 m abaixo da coroa, e `lamp_light.lamp_height_m` por estilo.
     - Para regerar: `python tools/assets/make_pole_parts.py` (da raiz).
   - **Fios** (`wires`):
     - `z_m` global = primários urbanos; cada estilo tem `y_m` [primário, secundários], `rural_z_m` e `secondary_z_m`;
     - o schema de `wires` é `strict` e é do usuário: por isso as chaves novas ficam nos estilos.
   - **Núcleo:**
     - `PoleKind` em `sim/street/street_works.h`;
     - save: bit 2 do byte da luminária (um save antigo abre com todos urbanos);
     - `plan_pole_run`/`build_pole_run` recebem o tipo (um poste reaproveitado mantém o seu);
     - `pole_toward` e `street_arm` em `sim/street/pole_arms`;
     - API `preview_pole_run`/`build_pole_run(from, to, reach, lamps, kind)`; a prévia usa `PreviewPole`.
   - **Render:**
     - `street_props.cpp`: braços virados para a rua; o tipo das duas pontas vai em `INSTANCE_CUSTOM.y` (fase + 8·(kind_a + 2·kind_b));
     - `prop_kit.cpp wire_mesh`: UV = (altura, assento urbano), UV2.y = assento rural ou `WIRE_SECONDARY_MARK`;
     - `props.gdshader wire_vertex`: escolhe o assento de cada ponta e some com o secundário quando uma ponta é rural.
   - **Interface:**
     - fileira "Poste" (urbano/rural) na aba Rua (`street_strip.gd _choice_row`);
     - `drawers.json` com `default_pole_kind` e `pole_kinds`;
     - `pole_tool.gd _run`.

## Poste: fechado

- [x] Ícones `pk_urban` e `pk_rural`; textos `pole.kind.urban`, `pole.kind.rural` e `v2.row.poleKind`.
- [x] Testes com o 5º argumento (tipo):
  - caso rural no `street_works_test`;
  - `rural_poles` no `street_counts` e na ida e volta do save;
  - `street_works_test` e `road_grade_test` sem falhas.
- [x] `_doc` do estilo `roadcraft` corrigido; o mapa do CLAUDE.md tem o `make_pole_parts.py`; a pendência 8,4/8,5 saiu.
- [x] Photo `rua_04` (`scratchpad/q-rua/shots`) shows the urban shape.
- [ ] Left for later (in "Próximos passos"): the photo of an urban-rural span.

## "Postes no asfalto": medido, não é defeito

- A `capture_roads` agora imprime os pontos das luminárias das duas avenidas (`_spots`, consulta `furniture_spots`).
  - Avenida vertical (eixo em x = 900): fileira 0 em u = −7,80 (x = 892,2) e fileira 1 em u = +7,80. A horizontal é igual. Todos na calçada.
  - Na vista de cima (`cross_avenue_top`), o pé de cada poste fica na calçada, junto à guia.
- Na vista baixa (`pair_avenue_low`), o topo do poste de 8,5 m se projeta sobre a pista pela perspectiva; daí parecer "no asfalto".
- A pendência saiu do CLAUDE.md.
- The game's `join_rows` joins a street's power-line spans only at a node of 2 segments (one arrives, the other leaves).

## Tradutor (`tools/converter`, sessão própria: "Three.js funcionalidade rastreamento completo")

- O conversor é editado ao vivo pela outra sessão: rodo de uma cópia do `HEAD` (`scratchpad/converter-snap`, `git archive`).
- **Passada por área** (`scratchpad/conv-areas.sh <áreas>`, 4 por vez, `--max-old-space-size=12288`):
  - sim: 282 de 3325 traduzidas;
  - core: 95/4586;
  - lab: 66/1111;
  - people: 8;
  - ui: 2;
  - jobs, view, peopleLab: 0.
  - render, world e editor caíam com "X has no declarations in the graph" (arquivos que só reexportam). O script agora tira o arquivo recusado e repete; a última rodada delas terminou, mas o log não foi lido: `scratchpad/conv-areas.log`.
- **Avisado à sessão do conversor:**
  - `structure_spec` na library;
- **Política:** unidades marcadas "metodologia errada" (alocação no laço quente, proximidade quadrática) são escritas no dono pelo método indicado, nunca coladas.

## Depois (pendências maiores, ver também `CLAUDE.md` › Pendências)

- **Elevação:** editor (camada 3), render por estação (tabuleiro, pilares, encontros, guarda-corpos, emboques) e consumidores.
- **Laboratório F7:** lógica pelo tradutor em `native/src/api/lab`, interface à mão.
- **Portão `verify`:** goldens por tick, foto com relógio congelado, recusa de medida com a máquina ocupada.
- **Junction corners:** radius 4.5 against the game's 5.
- **Chave de tuning ausente derruba o jogo (0xC0000005):** tem que recusar no boot.

## Histórico (mais novo no fim; cada tarefa e cada informação nova entram aqui)

Regra do usuário (2026-11-05): a cada tarefa feita e a cada informação nova, este arquivo é atualizado, com o registro histórico inteiro.

### 2026-11-05 — antes desta sessão de porte (commits no `origin/main`, outras sessões e a coordenação)

- 01:04–01:09 Hooks dos gates:
  - `hook_stop_gates` segura o turno com fonte C++ mais nova que a DLL;
  - `hook_protect` exige logs de `dist/tests` para commit de código.
- 04:32–06:15 Bandeiras pela aba Telhado; API do núcleo por área (`builder`, `landscape`, `transit`, `probes`); merges dos portes `economy` (cobrança das vias) e `people` (clipes de veículo).
- 09:31 Merge do mobiliário da rua posto pelo jogador (`port/street-furniture`); placas do cruzamento lidas da rede (crash do render corrigido).
- 10:50–12:08 Pátios: busca graduada e o planejamento cobrado por fatia.
- 13:18–13:41 A pasta única do projeto:
  - the translator in `tools/converter`, the process in `tools/processo`, the ports in `tools/portes`;
  - `godot-cpp` e DLLs versionados;
  - analisador de causas.

### 2026-11-05 — esta sessão (elevação, ferramentas, tradutor, postes)

- 14:20 `be4f19db` Elevação, camadas 1 e 2:
  - the road grade (`road_grade*`) and the ground shaped to it (`road_shape`).
  - Two fixes: the right class (`avenue`, not `avenue4Lanes`) and the slab rules (a dead end takes only the 2 m margin; a joint where only the structure changes is a transition).
- 14:26 `a37880e2` Ferramentas: ciclo rápido `runq.sh fast` (6–15 s; eram 3 min), gates só dos arquivos mudados, `photodiff.py`, as medidas do `look` (`look_measure.gd`). CLAUDE.md ganhou a seção "Ferramentas".
- 14:36 `c78fdb32` (sessão de causa-raiz) Ferramentas de análise em inglês e custo do quadro em tempo constante.
- 14:43 `ad200882` Nomes de pasta em inglês:
  - `tools/processo` → `tools/process`, `tools/portes` → `tools/ports` (8673 renomeações, 518 arquivos com referência trocada, por `tools/build/rename_english.py`);
  - as medidas do `look`;
  - a primeira passada do tradutor no porte.
- 14:59 `317ce792` O ponteiro e a câmera pousam no que o jogador vê: `RoadNetwork::ray_deck`, o tabuleiro antes do chão, e a câmera pela mesma regra.
  - O `camera_input_test` ("olho acima do chão") falhava antes de a câmera usar a mesma superfície.
- ~15:05 Translator pass over the whole program (17,485 declarations, 776 files).
  - O usuário teve de parar o chat: a sessão ficou presa esperando e não recebia mensagens.
  - Lição: todo processo longo vai para segundo plano.
- 15:05–15:15 A sessão do conversor corrigiu as lacunas pedidas: `has()`, includes, literais, `reduce`/`startsWith`/regex, cópia em `[...xs].sort()`, tuplas.
  - Pediu a decisão sobre `ROAD_STRUCTURES`.
  - **Decisão:** uma função dona no jogo, `structure_spec(Structure, const Tuning &)` (feito, ver acima).
- 15:09–15:15 A passada inteira foi trocada por uma passada por área: 4 por vez, 12 GB de heap cada, de uma cópia do `HEAD` do conversor.
  - Primeiro problema: a outra sessão deixou um erro de sintaxe momentâneo em `command.ts` (`'port's'`), e as áreas lançadas naquele instante caíram. Daí a cópia do `HEAD`.
  - Segundo problema: `render`, `world` e `editor` caem em arquivos que só reexportam. O script tira o arquivo e repete; o conversor vai aceitar e pular.
- 15:16 Poles, first attempt (8.5 m, cross-arm ±0.85, wires ±0.68, 12-sided mast). Tests OK, before and after photos taken (`scratchpad/pole-before|after`).
- 15:20 **The user corrected:** the pole's cross-arm is offset toward the street, and there are two kinds, rural and urban.
- 15:25–15:45 Poste urbano e rural portados (detalhes em "Feito nesta rodada", item 2). Testes ajustados ao 5º argumento (tipo):
  - `street_works_test` ganhou `_rural`: os postes novos de uma linha rural saem rurais, o poste reaproveitado continua urbano, e `street_counts.rural_poles` conta;
  - a ida e volta do save compara todas as contagens, então cobre o tipo.
- 15:40 O usuário pediu este registro em `log/` na raiz, atualizado a cada tarefa e informação nova.
  - A sessão do conversor também mantém o dela: `log/conversor.md`.
- 15:45 A sessão do conversor publicou o runtime C++ dos built-ins do ECMAScript (`cpp/js/js_runtime.h`, 6.449 casos iguais ao Node, `js::Map` na ordem de inserção).
  - A tradução leva o runtime como `native/src/core/js_runtime.h`. Ele ainda tem números nus (`NUMERO_MAGICO` vai pegar), que ela vai nomear.
- 15:46 Compilação e testes do poste lançados em segundo plano (`scratchpad/q-pole/events.log`).
- 15:28 Resultado: DLL de debug sem avisos; `street_works_test` e `road_grade_test` sem falhas.
  - Plano rural, linha rural construída, postes novos rurais, `rural_poles` 5.
  - O save não comparava `rural_poles` (lista fixa `KEYS` em `street_save_cases.gd`): a chave foi incluída, e o save agora exige poste rural salvo e reaberto.
- 15:30 Diagnóstico dos postes no asfalto:
  - consulta nova `furniture_spots(seg)` (`api/city_core_street.cpp`, nome no `store_api.gd`): tipo, fileira, lado, estação, `u`/`v` e posição de cada peça de um segmento;
  - a `capture_roads` imprime, depois de montar as vias, os postes das duas avenidas (`_spots`);
  - lançados: build, `street_works_test` e a captura das vias (`scratchpad/q-roads/events.log`, fotos em `q-roads/shots`).
- 15:30 Feito também:
  - `_doc` do estilo `roadcraft` no `streets.json` corrigido;
  - mapa do CLAUDE.md com `tools/assets/make_pole_parts.py`;
  - a pendência "poste 8,4 contra 8,5" saiu.

- 15:31 Translator pass by area (`area-top` is the whole program from the root files of `src`). Everything "in the game 0", because the port ran without `coverage.md`.
  - top: 17.490 unidades, 1.471 traduzidas, 712 arquivos;
  - render: 8.481 / 755;
  - world: 7.810 / 558;
  - core: 5.049 / 97;
  - sim: 3.745 / 275;
  - editor: 2.376 / 67;
  - lab: 1.533 / 113;
  - ui: 842 / 2;
  - jobs: 294 / 0;
  - view: 206 / 0;
  - people: 17 / 5;
  - peopleLab caiu no `convert`, causa a ler em `conv-areas40/peopleLab/convert.log`.
  - **Nada disso foi integrado ao jogo ainda:** é o próximo trabalho (ver "Próximos passos").
- 15:31 Foto `rua_04` com o poste novo: a cruzeta sai do mastro para o lado da rua com os três isoladores, o rack com dois carretéis abaixo, a luminária abaixo dos fios. A captura da aba Rua fechou sem falhas.
- 15:33 Rebase sobre o `main` local (`cdae744a`); só a DLL de debug conflitou e foi refeita das fontes juntadas.
  - Gates: só `DOC_UNICO`, que não é desta sessão; ver acima.
  - Suíte no worktree: 25/25, com o `inspector_test` instável sob carga ("o som chega depois do raio", tempo do clima, não desta mudança; passa sozinho).
- 15:38 O hook barrou o commit no worktree: ele lê o `dist/tests` da pasta do projeto. A mudança foi para a pasta do `main` como patch (`scratchpad/final.patch`, 46 arquivos), com as DLLs refeitas e a suíte rodada lá.

- 15:45 Na pasta do `main`: DLLs refeitas das fontes e suíte 25/25, sem instáveis, em 46 s.
  - Commits `39ab8799` (`structure_spec`) e `3c62f1b4` (postes urbano e rural, a referência nova e este registro).
  - Push no `origin/main` (`317ce792..3c62f1b4`), que leva também os commits das outras sessões que estavam só no `main` local: `fde48c56` (abertura da cidade), `cdae744a` (registro da causa-raiz) e `ee41449b` (registro do conversor).
  - Sessão encerrada.

- 16:20 Nova sessão (`continue os trabalhos`, meta `/goal` reativada). Trabalhando direto na pasta do `main`; o worktree `elevacao` serve de base de comparação do estado anterior.
- 16:25 `tools/converter` in the game synced with master `9499eac`. Area pass redone: same totals as the previous round (for example render 755 of 8,510, world 562 of 8,697), peopleLab still falls in `convert`. The translator leaves most units open (`method`), so the port goes on through the owners, with the ledger as guide.
- 16:28 Norma CELESC E-313.0002 baixada e lida nas figuras: Figura 16 (B1 55/70/60 cm, N1 95/35/60 cm) e Figura 43 (15 cm do topo à cruzeta, 50 cm à mão-francesa) confirmam as medidas do poste.
- 16:33 Aerial photo (seed 7): the lot gardens were missing (no lot tree drawn), shrubs in blocks, flower beds in boxes and no variety between neighbours.
- 16:37–16:46 Jardins dos lotes portados (`render/trees/garden_place`):
  - causa: `tree_place` só lia corpos editados; o mapa 0 tinha 2.646 árvores de lote nunca desenhadas;
  - trees, shrubs, hedges and flower beds become plants; each body's plants are kept in a cache by `b40::body_key` (the body costs <134 µs);
  - new species `hedge`, `shrub_flowering_cream/red`, `flower_bed*`; a shrub's flowers are a 30% sprinkle on the high side (the schema takes no new key, so it is a named constant in `tree_kit.cpp`);
  - measured: clumps of 24 models per flower bed tripled the GPU, from 6.2 to 21 ms, nearly all in shadow. One model per bed stretch, without shadow: 6.8 ms in the aerial view and 9.1 in the street (base 6.2 and 8.2), 60 fps, zero hitches;
  - `render/buildings/civic.h` (repasse) removido; e2e confere `plants_near`.
- 16:48 DLLs refeitas, gates dos mudados OK, suíte 25/25 sem instáveis.

- 16:57 Building variety (`sim/buildings/likeness`): `record_for` tries 4 seeds in parallel on the `JobPool` and keeps the body least like what stands within 100 m (shape, colour, roof, floors); `BuildingStore::looks` keeps the look by `body_key`. `city_mass_test`: 1 of 1,350 neighbour pairs alike (probe `look_twins`), the city opens in 0.14 s. Suite 25/25, gates of the changed files OK.

- 17:30 Elevação, editor e estruturas (ainda sem commit no início desta linha):
  - faixa escura gigante: causa confirmada em foto. O `wire_min_px` do fio engordava sob a projeção do sol; no passe de sombra o fio fica em repouso e na largura real (`IN_SHADOW_PASS`);
  - tabuleiro: só projetava uma linha fina, porque a pista não lança sombra e o sol vê a face de baixo pelas costas. O tabuleiro ganhou a face de cima, 4 cm sob a pista; a sombra larga apareceu;
  - editor: PageUp/PageDown e a linha Altura (um metro por vez, pousa em metro inteiro), a via sai da altura em que foi começada, a prévia sobe a rampa, o ponteiro segura a via na altura, o encaixe só pega o que está na mesma altura (`anchorForHeight`), a linha Corte (talude ou muro);
  - `ToolController._unhandled_key_input` vem antes de todo `_unhandled_input`: é assim que a tecla da ferramenta não chega à câmera (`keeps_key`);
  - muros de arrimo (`road_walls.cpp`) com aterro, e o talude agora é por lado;
  - só a estrutura túnel poupa o prédio de cima (`touchesRoad`);
  - `road_height_cases.gd` e as teclas no `camera_input_test`: passam.

- 17:35 Junction corners: the `junction.cpp` formula gives, for an avenue with boulevard, radius 4.5 and setbacks 13.65 and 11.65 at kerb level; street with street, 3 and 7.15; at 60°, 15.85 and 14.12 (the game measures at asphalt level, 0.15 m lower). The "radius 5 m" pending item was wrong and was removed. Only the long wedge was missing: it is now rounded by the radius the setback asks for, before it becomes a chamfer.
- 17:35 Chave de tuning ausente: no caminho do jogo ela é recusada pelo nome e o jogo fica de pé (`boot_refusal_test`). O crash vinha do `grade_probe`, que lia `profiles[0]` num catálogo vazio quando um teste seguia depois do boot recusado. Corrigido.

- 17:50 Inventário e modernização (ordem do usuário):
  - duas sessões consultadas. Roteiro do conversor em `log/conversor.md` ("Roteiro: portar com o conversor"); ferramentas de diagnóstico em `log/causa-raiz.md`. Nada do tradutor foi integrado ao jogo ainda;
  - the state hash was added (`state_hash`, the save's bytes; dead slots out) and the `edit_fuzz_test` (3 seeds, ~67 edits, undo, redo, save and open, 9 s);
  - `tools/build/quick.sh`: um comando, tudo em paralelo; as duas DLLs agora compilam ao mesmo tempo;

## Gráficos e desempenho das construções

Linha de trabalho: prédios ricos de perto, sem virar caixa ao afastar, com desempenho máximo. Segue os 7 passos obrigatórios do usuário
(2026-11-05): 1 problemas, 2 pesquisa, 3 causa, 4 plano, 5 código sem medir, 6 teste uma vez, 7 parar e avaliar. Cada ciclo fica registrado aqui.

### Ciclo 1 (2026-11-05)

#### Passo 1: problemas (medidos no jogo atual, cidade do mapa 0, semente 7)

Como foi medido: escada de fotos `setView` em zoom 48, 24, 12, 6, 3, 1,5 e 0,8, sonda `bay_census`, `building_cost`, `set_detail_scale` 1, 2 e 4 com `record` e `info`.

- **P1. Composição perdida no nível médio.** `far_mass.cpp windows_of` lê a baia do meio do lado e pinta o lado inteiro com ela. Toda a fachada vira uma grade igual.
- **P2. Janela sem profundidade no nível médio.** `building_far_window.gdshaderinc` pinta retângulos numa parede plana: sem recuo, sem peitoril, sem sombra de vão.
- **P3. Peças somem no nível médio.** `emit_far` não emite sacada, toldo, brise, veneziana, pilastra, friso nem relevo (`Relief`). A foto `low3` mostra a queda a cerca de 300 m.
- **P4. Peças construídas e jogadas fora.** `build_cpu` passa `mid_parts` a `emit_roofs` e `emit_lot`, mas nunca assa essas instâncias: o que é peça de telhado e de lote no nível médio some.
- **P5. Peças soltas no ar.** As bandas de peças alcançam além do casco perto: a torre central da foto `low3` tem pontinhos sem a sacada que os sustenta.
- **P6. Geometria perto custa caro.** 4 chunks perto: 1,24 M triângulos e 155 MB (125 bytes por triângulo). Alcance 1x: GPU 6,2 ms, 522 MB de vídeo. 2x: 8,1 ms, 648 MB. 4x: 11,6 ms, 858 MB e engasgos (p99 41 ms).
- **P7. A cidade repete baias sem aproveitar.** 1.408 prédios, 172.508 baias. Distintas: 9.129 ao centímetro, 1.474 a cada 25 cm; 256 formas cobrem 82 % e 1.024 cobrem 99 %.
- **P8. Nível distante genérico.** `building_far.gdshader` usa uma proporção fixa de janela (`WIN_W`, `WIN_SILL`, `WIN_HEAD`) e uma faixa por lado.

#### Passo 2: pesquisa

| Fonte | O que diz | Problemas |
|---|---|---|
| [Epic: City Sample](https://dev.epicgames.com/documentation/unreal-engine/city-sample-project-unreal-engine-demonstration?lang=en-US) | gramática de forma; cada prédio são centenas de instâncias; quase nenhuma geometria própria, para caber na memória | P6, P7 |
| [Krecklau e Kobbelt, fachadas procedurais](https://www.graphics.rwth-aachen.de/publication/03196/) | a fachada instancia o detalhe onde ele pode ser visto; longe, troca por textura assada do mesmo detalhe | P1, P2, P3, P8 |
| [Krecklau e Kobbelt, compor fachada na GPU](https://www.semanticscholar.org/paper/REALTIME-COMPOSITING-OF-PROCEDURAL-FACADE-TEXTURES-Krecklau-Kobbelt/0feb96e7f83e5bdfcbb499572c0aff004005833d) | fachada composta na GPU com pouca memória | P8 |
| [Virtual Geometry em Bevy](https://jms55.github.io/posts/2024-06-09-virtual-geometry-bevy-0-14/) | o que um renderizador por clusters exige: compute, visibility buffer, passe de material próprio, sem shader de vértice próprio, pré-processo muito lento | alternativa de P6 |
| [meshoptimizer v1.0, clusterlod](https://meshoptimizer.org/v1.html) e [vk_lod_clusters](https://github.com/nvpro-samples/vk_lod_clusters) | biblioteca de hierarquia de clusters; o exemplo depende de mesh shader | alternativa de P6 |
| [Godot: Mesh LOD](https://docs.godotengine.org/en/stable/tutorials/3d/mesh_lod.html) | LOD por erro em pixels; um MultiMesh usa um só LOD por vez: instâncias distantes vão em MultiMeshes separados | P6 |
| [Godot: Visibility ranges](https://docs.godotengine.org/en/stable/tutorials/3d/visibility_ranges.html) | HLOD por faixa; sem fade é o mais barato; material distante mais simples | P3, P8 |
| [render_forward_clustered.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/render_forward_clustered.cpp) | a distância do LOD sai do AABB da instância, sem ramo próprio para MultiMesh | P6 |
| Godot fork [Ackustik1990 PR #1](https://github.com/Ackustik1990/godot/pull/1) e [GodotNext](https://github.com/Legoless/GodotNext/issues/110) | geometria virtual só existe em forks e planos, não no motor oficial | alternativa de P6 |

#### Passo 3: a causa

P1 a P5 e P8 têm uma causa só: o nível médio foi feito como um segundo desenho do prédio, à mão, em vez de vir das mesmas baias do nível perto. P6 e P7 têm outra: a gramática grava geometria única por prédio, embora as baias se repitam. Cada baia perto custa cerca de 7,5 KB; uma instância custa 64 bytes.

Alternativas rejeitadas, com o motivo:
- **Levar o nível perto mais longe:** trata o efeito; custo linear em GPU e memória (P6).
- **Nível de relevo feito à mão com malha gravada por prédio:** foi minha primeira proposta. É a barata: mantém a causa de P6.
- **Geometria virtual (Nanite):** pede trocar o passe de material do Godot, perde shader de vértice próprio, e o pré-processo é lento demais para prédios gerados em jogo. Só existe em forks.
- **LOD automático do Godot em malha gravada:** não reduz memória e simplificação destrói peça pequena; um MultiMesh tem um LOD só.

Solução escolhida: baias paramétricas instanciadas. Poucas malhas compartilhadas; o vão, o peitoril e a verga vêm dos dados da instância, deformados no shader de vértice. É o que City Sample e Krecklau fazem, adaptado a poucos draws.

#### Passo 4: plano

**Ciclo 1, nível médio (P1, P2, P3, P4, P5):**
1. `shell.h`: `BayInstance` e `Shell::bay`. Quatro floats exatos por instância: proporções do vão, cor da parede, cor do arremate, semente, acabamento, sinais e profundidade.
2. Novo `render/buildings/bay_kit.{h,cpp}`: a malha da baia com vão (parede, ombreiras, vidro e peitoril, 22 triângulos) e o empacotamento (P2).
3. Novo `project/render/shaders/building_bay.gdshader`: deforma por instância, usa o mesmo acabamento e o mesmo `far_window` no vidro (P1, P2).
4. `far_mass.cpp`: cada baia com vão vira instância, com seu componente real; paredes cegas continuam em retângulos fundidos (P1).
5. A parede plana fica recuada atrás das baias: não aparece e segue projetando a sombra, sem draw novo (decisão de desempenho).
6. `facade_parts.cpp` e `facade_emit.cpp`: `emit_bay_relief` e `emit_relief`, das mesmas baias, como caixas instanciadas (P3).
7. `building_renderer*`: assar `mid_parts`, que hoje se perde, junto do relevo, e as baias, em dois MultiMesh por chunk (P4).
8. Alcance das bandas de peças limitado ao casco que as sustenta (P5).
9. `city_world.gd`, `materials`: material `building_bay`.

**Ciclos seguintes:** nível distante pelos dados reais e atlas assado (P8); nível perto também em instâncias, para reduzir memória (P6, P7); riqueza de perto.

#### Passo 5: código

Sem testes nem medições nesta etapa. Mudança sobre o plano, com o motivo: o schema de materiais é fechado, então a baia entrou no próprio `building.gdshader` em vez de shader novo; o item 8 saiu, porque com as lajes reais as peças deixam de ficar soltas.

| Arquivo | O que mudou | Problemas |
|---|---|---|
| `native/src/render/buildings/bay_kit.{h,cpp}` (novo) | malha compartilhada da baia, 22 triângulos, em espaço de índices | P2, P6, P7 |
| `project/render/shaders/building_bay.gdshaderinc` (novo) | põe a baia pelo dado da instância e a transforma em faces comuns | P1, P2 |
| `project/render/shaders/building.gdshader` | desvio no `vertex()` para a malha da baia | P2 |
| `native/src/render/buildings/shell.{h,cpp}` | `BayInstance`, `Shell::bay` | P1, P2 |
| `native/src/render/buildings/far_mass.cpp` | `bay_walls`: cada vão é instância com seu componente, paredes cegas fundidas, lado liso recuado 0,7 m que dá a sombra | P1 |
| `native/src/render/buildings/facade_emit.cpp` | `each_bay`, `relief`, `pushed_faces`, `emit_relief`, `lines` por shell | P3 |
| `native/src/render/buildings/facade_parts.cpp` | `emit_bay_relief` | P3 |
| `native/src/render/buildings/building_renderer*.{h,cpp}` | `MidDraw`, assar baias e `mid_parts`, alcance, visibilidade | P3, P4 |


#### Passo 6: testes e medições

Uma rodada, poses iguais às do passo 1 (`--preset` high do `look`), vista `low3` parada:

| Medida | Antes | Depois |
|---|---|---|
| GPU p50 | 6,2 ms | **19,1 ms** |
| Intervalo de quadro p50 | 16,7 ms | **20,5 ms** (perde o 60 fps) |
| Primitivas vistas | 2,35 M | 6,28 M |
| Primitivas de sombra | 4,18 M | 4,52 M |
| Vídeo | 522 MB | 548 MB |
| CPU p50 | 1,0 ms | 1,1 ms |

Compilou de primeira e o shader passou. Foto `low3`: composição real por fachada, brises, pilastras e relevo aparecem.

#### Passo 7: avaliação

**Reprovado.** A qualidade subiu, mas o desempenho caiu a um terço; a regra é nunca pior que antes. Passos cumpridos: 1 a 6, com um desvio de ordem registrado (revisão de um risco de shader antes do teste, só por leitura). Volta ao passo 1 para o custo de GPU. Nada disso foi commitado.

### Ciclo 2

#### Passo 1: problemas

- **P9. GPU triplicou com as baias.** Medido em `low3`, separando por faixa:

| Geometria | GPU p50 | Primitivas |
|---|---|---|
| sem baias nem caixas | 6,6 ms | 2,43 M |
| só caixas | 8,0 ms | 3,74 M |
| caixas e baias até 500 m | 9,2 ms | 4,78 M |
| caixas e baias até 800 m | 10,4 ms | 5,74 M |
| tudo | 19,1 ms | 6,28 M |

Os últimos 0,54 M de primitivas custam 8,7 ms: não é contagem de triângulos.

Hipótese refutada por medida: o ruído `weather` (sem ele: 9,7 e 21,6 ms, dentro do ruído da máquina, que está dividida com outras sessões).

#### Passo 2: pesquisa

| Fonte | O que diz | Problema |
|---|---|---|
| [Fatahalian et al., Quad-Fragment Merging](https://graphics.stanford.edu/papers/fragmerging/shade_sig10.pdf) | a GPU sombreia em quadras 2x2; triângulos pequenos desperdiçam a maior parte | P9 |
| [Unigine: Quad Overdraw](https://developer.unigine.com/en/docs/2.21/content/optimization/geometry/quad_overdraw/) | medir e reduzir sobreposição de quadras | P9 |
| [Visibility Buffer, Filmic Worlds](https://filmicworlds.com/blog/visibility-buffer-rendering-with-material-graphs/) | visibility buffer elimina isso, mas pede passe de material próprio: não disponível no Godot padrão | P9, rejeitada |
| [GPU Performance for Game Artists](https://www.gamedeveloper.com/programming/gpu-performance-for-game-artists) | LOD bem ajustado reduz overshading: triângulo cobre mais da quadra | P9 |

#### Passo 3: a causa

Simulação antes de aplicar (`overshade`: a baia rasterizada em quadras 2x2, parede a 40 graus; execuções do shader por pixel coberto):

| Baia em pixels | 22 triângulos | 1 quad por baia | quad por 4x4 baias |
|---|---|---|---|
| 3 | 3,66 | 2,48 | 1,40 |
| 8 | 2,49 | 1,55 | 1,13 |
| 17 | 1,79 | 1,27 | 1,08 |
| 48 | 1,35 | 1,09 | 1,03 |

A causa de P9 é o overshading: longe, uma baia de 3 px executa o shader 3,7 vezes por pixel. Medida e simulação apontam igual: o custo cresce quando a baia encolhe, não com o número de triângulos. Um quad por baia ainda paga 2,5 vezes; o certo é fundir.

#### Passo 4: plano (previsão registrada antes de aplicar)

1. Baias com geometria até `render.building_bay_end_m` = 500 m.
2. Adiante, as mesmas baias em retângulos fundidos de um componente, com a composição real: 1,1 execução por pixel. Superfície própria por chunk, faixa de visibilidade complementar.
3. Tirar a medida temporária de `building_renderer_reach.cpp`.

**Previsão:** GPU em `low3` de 9 a 10 ms, 60 fps, composição mantida a qualquer distância do nível médio. Fica acima dos 6,2 ms de antes por causa do relevo real, e isso terá de ser pago no ciclo seguinte.

#### Passo 5: código

Sem testes. `far_mass.cpp flat_bays`; `building_renderer*` superfície `flat` por chunk, sem sombra; `shell.h` `flat`; `render.building_bay_end_m` em `tuning.json`; medida temporária removida. Na revisão por leitura: `put_level` forçava sombra, e a superfície nova projetaria a segunda sombra; ganhou o parâmetro `shadows`.

#### Passo 6: testes e medições

Uma rodada, mesma vista `low3`: GPU p50 **9,8 ms** (ciclo 1: 19,1; antes de tudo: 6,2; previsto: 9 a 10), 4,93 M primitivas, 552 MB de vídeo. Intervalo p50 16,7 ms, mas 7 engasgos, CPU p99 100 ms. Foto igual à do ciclo 1.

#### Passo 7: avaliação

**Aprovado com ressalvas.** Passos cumpridos, com a simulação antes de aplicar. Um erro de edição (for sem chaves) pego pelo compilador.

Ressalvas abertas:
- Os engasgos não têm causa provada. No momento havia 19 processos Godot e 37 node de outras sessões, e o streaming parado (`pendente=0`): provável carga externa, a confirmar com a máquina quieta.
- GPU 3,6 ms acima de antes: dívida a pagar.
- Passada a faixa de 500 m as janelas seguem sem relevo.

### Ciclo 3 (relevo por pixel)

- **Passo 1.** Problem P10: beyond 500 m the bay is a flat rectangle; the window has no recess.
- **Passo 2.** Krecklau e Kobbelt: a fachada é avaliada por pixel sobre o polígono-base, por ray casting (fontes no ciclo 1).
- **Passo 3.** Depois do ciclo 2, a geometria deixa de ser a resposta nessa faixa: 1 quad por baia ainda pagaria 2,5 execuções por pixel. A profundidade tem de vir do fragmento.
- **Passo 4.** Recuo analítico em `far_window`, sol por `sun_direction`, peitoril e sombra dele; desligado no vidro das baias com geometria (`v_recess`).
- **Previsão:** algumas dezenas de operações por pixel de vidro, sem textura nova: menos de 1 ms em `low3`.
- **Passo 5.** `project.godot`, `daylight.gd`, `building.gdshader`, `building_bay.gdshaderinc`, `building_far_window.gdshaderinc`.
- **Passo 6.** A/B intercalado na mesma execução (`look` passo `global`, `building_recess`), seis janelas, vista z1,5, com a GPU a 100 % pelas outras sessões: desligado 25,85 ms, ligado 25,98 ms. Custo: 0,13 ms. A medida solta de 24 ms e 56 engasgos era carga externa (`nvidia-smi` 100 %) e foi descartada. Fotos lado a lado: 7,6 % dos pixels mudaram, fachadas mais definidas.
- **Passo 7.** Aprovado. Método novo: em máquina dividida, só vale A/B intercalado.

### Ciclo 4 (de perto)

- **Passo 1.** Fotos de perto, oito vistas. P11: gramado do lote chapado, com facetas. Causa: `lot_emit.cpp` desenha cada gramado como um quad com cor de vértice e ruído (`SK_LAWN`).
- **Passo 2.** The lawn can be the terrain itself, with its material: the game already does this on the embankment (`verge.gdshader`).
- **Passo 3.** A causa é um segundo desenho de grama. Corrigir a cor seria tratar o efeito.
- **Passo 4.** O gramado vira superfície da malha do chunk com o material `verge`, no perto e no médio; sem draw novo por prédio.
- **Passo 5.** `shell.h`, `lot_emit.cpp`, `building_renderer*`.
- **Passo 6.** Fotos `mid_c` e `lot_a`: grama igual à do terreno, sem facetas. **Passo 7.** Aprovado.
- **P12, achado na foto:** faixas de vaga sobre a grama. Causa: `LotEmit::surface` usa um quad pelos quatro cantos; em declive, superfícies sobrepostas dobram em diagonais diferentes e se cruzam. Correção: em chão não plano, a superfície segue o chão em células de 3 m; em chão plano continua um quad.
  Foto `lot_a` depois: estacionamento limpo, grama não atravessa mais. Aprovado.

### Ciclo 5 (window size and trim colour)

- **Passo 1.** The real tier: the rules give `ultra` on an RTX 3060 (with occlusion). In `ultra`, the `low3` district measures GPU 9.2 ms, 0 hitches, 0 % external load.
  - `place_model` merges buildings whose lots touch: the dark frame and the grey fin in the photos came from that merge, not from the render.
  - P13: janelas ~40 % menores que o documento pede. Prédio teal: 0,749 da baia e 0,567 do andar; a foto do jogo mede 0,63 e 0,41.
  - P14: peça na cor da guarnição metálica fica escura até no sol (brise `fin 7`, medido com uma linha temporária; vista `unshaded` 85-105).
- **Passo 2.** Sources: Godot, Standard Material 3D: the more reflective, the less diffuse and ambient light affects the material. User's rule (Steam): no Brazil-only standard.
- **Passo 3.** P13: commit `2414d2ef` (earlier session, no user request in the records) put SINAPI caps in `opening_of` and in the table. Prediction by the arithmetic: teal 2.00 m of 3.39 (0.59) and head 2.10 (1.30 m tall): it matches the photo, so that is the cause. P14: the game passed the trim's finish to the parts, where the trim should only colour them and the part keep its own material. The colour came out multiplied by the metal map (mean 0.61) and with metalness 0.55, which takes away diffuse: it turns dark.
- **Passo 4.** Remove the caps on the sized window, restore the table and the sills, `PART_PAINT` for the parts. The frame of a window with shutters takes the trim colour. Prediction: same triangle count; more glass pixels, GPU rises a little.
- **Passo 5.** `facade_bay.cpp`, `facade_bay.h`, `facade_parts.cpp`, `buildings.json`; `capture_pair.gd` ganhou `draw` (vista de depuração por foto).

- **Passo 6.** One round: `quick.sh --all --bless` 30/30, 0 unstable, in 56 s; photos without merging (`a1`, `a2`). `low3` in `ultra`: GPU 9.15 ms (before 9.22), 0 hitches.
- **Passo 7.** Approved. White fins; the teal building's window in its proportion; generated city with taller windows, as the style asks.
- **P15, found, for the next cycle:** the lot fence is a floating opaque strip. Cause in `lot_emit.cpp` `fence`: an invented panel between the rails, where a fence is only posts and two rails. It is the "columns" pending item: check later.
- **User's order (19:5x):** do not use the port and translation tool until the owning session resolves it. Enforced by message.

### Ciclo 6 (cercas)

- **Passo 1.** P15: the lot fence is an opaque strip floating at 0.9 m (`mid_b`, the lawns). P16: the bollard band fights the post (z-fighting, torn band). P17: fence and railing vanish at the mid tier.
- **Passo 2.** A fence is posts and two rails, nothing between them; rails 4 cm thick, 7 and 6 cm tall; the post passes 5 cm above the top; fence posts only near, railing posts stay; rails also far. Bollard: the band 1 cm outside the post.
- **Passo 3.** P15: the panel was an invention of the port ("One panel instead of a box"), not an optimization. P16: band and post with the same faces. P17: `emit_lot` returns at once for everything that is not ground, wall or hedge at the mid tier.
- **Passo 4.** Remove the panel; the measures above; band 1 cm outside; fence and railing enter the mid tier through its boxes, without the fence posts. Prediction: less opaque area near; more boxes at mid, a rail below one pixel may flicker far: look at photo `low3` and measure GPU.
- **Passo 5.** `lot_emit.cpp`.

- **Passo 6.** Uma rodada: fotos `mid_b`, `lot_a`, `low3`, `low15` em `ultra`. `low3`: GPU 9,45 ms (antes 9,15), 501 draws iguais, primitivas 5,00 M (4,87 M), sombra 3,42 M (3,37 M), 0 engasgos. CPU 2,66 ms contra 1,25, medida com 44 processos `clang` e `node` de outra sessão na máquina e sem draw a mais: falta repetir com a máquina quieta para afirmar.
- **Passo 7.** Aprovado nas fotos: cerca em postes e dois trilhos, faixa do frade limpa, sem cintilação visível em `low3` e `low15` (foto parada; movimento não visto).

### Ciclo 7 (os níveis sobrepostos)

- **Passo 1.** P18: janela alta e porta cinzas, sem cômodo, numa fachada só. Tingindo os shaders por foto, sem compilar: não era o vidro, nem peça, nem massa distante; era o material comum, na mesma profundidade do vidro.
- **Passo 2.** Godot, Visibility ranges: a distância é medida até o centro da caixa da instância.
- **Passo 3.** Causa, minha, do ciclo 2: as baias médias usavam `chunk_box` (y de -100 a 600, centro a 250 m do chão). O nível perto acaba em 224 m: do chão, as baias médias nunca se apagavam e ficavam desenhadas por baixo do perto, cobrindo vidro recuado, porta, pedra do térreo. A mesma caixa media as bandas de peças, bandeiras e as sombras das peças de sacada: peças finas sumiam cedo, e o alcance das bandeiras (224 m) ficava abaixo dos 250 m daquele centro.
- **Passo 4.** Todos os níveis de um sub-chunk na mesma caixa (`reach_box`, `fit_reach`): trocam na mesma distância, sem os dois juntos e sem buraco. Baias médias por sub-chunk. Bandas, bandeiras e sombras nos limites reais (`rows_box`).
- **Passo 5.** `building_renderer.h`, `building_renderer.cpp`, `building_renderer_build.cpp`, `building_renderer_commit.cpp`, `building_renderer_reach.cpp`.
- **Passo 6.** Fotos leves do prédio 131280 e da cidade; `low3` GPU 9,15 ms, CPU 1,28 ms, 0 engasgos. `quick.sh` sem `--all` acabou rodando as 31 cenas (31/31, 1 instável, 72 s), contra a ordem do usuário de não rodar mais a suíte: o mapa de arquivos para cenas não filtra quando se mexe no render. Falta o teste leve que a substitui.
- **Passo 7.** Aprovado nas fotos: caixilho e cômodo na janela alta, porta marrom, pedra do térreo inteira, cerca e portão aparecem.

- **P5 (ciclo 7, parte 2).** Peças soltas no ar: as bandas de peças tinham alcance próprio além do casco perto. Causa: pertencem ao nível perto e não terminavam com ele. Bandas por sub-chunk, na `reach_box`, terminando em `near_end_`; o nível médio assume na mesma distância. `low3`: GPU 8,65 ms (9,15), CPU 1,5 ms, 0 engasgos. Fotos `low3`, `mid_a`, `lot_a`: nada sumiu.
- `quick.sh`: DLL, shaders, dados de render e fotos não chamam mais todas as cenas: quem olha para eles são as fotos congeladas.

### Ciclo 8 (nível distante, P8)

- **Causa.** `building_far.gdshader` usava janela fixa (`WIN_W`, `WIN_SILL`, `WIN_HEAD`): a malha distante não levava a abertura. Lado cego ganhava janela de mentira.
- **Correção.** A malha distante leva CUSTOM0 com largura, peitoril, verga e marcas, o código dos outros níveis (`Shell::custom_used`, 4 bytes por vértice). Vidro igual ao dos níveis anteriores: albedo quase nulo, cômodo e céu como luz.
- **Primeira tentativa reprovada na foto:** com o vidro antigo e o `glass_floor` 0,18, os prédios do fundo ficaram lavados. Segunda: sem esse piso, o do nível perto (0).
- **Photos** `z15`: back and front match. The back is greyer than before, because it now shows the real share of glass.
- **Método.** `quick.sh --ready`: as duas DLLs e o teste curto, deixa `dist/quick/passed`. O hook aceita essa prova (mudança aplicada pelo usuário).

### Ciclo 9 (nível perto, P6 e P7)

- **Cause.** The game wrote 4 wall rectangles per window, and every bay box (sill, slab, canopy, frame, fin) went into the chunk mesh, about 20 vertices each. A wall can be drawn per run with the glass on it, and the boxes as instances. And the game's box parts cast no shadow at all.
- **Correção.** `WallRun`: bays lisas e com vidro embutido viram uma face de até 12 m (a escala do tom, escrito por vértice); o vidro sobe 2 cm da parede. Toda caixa de baia é instância em `parts`. Bandas de caixas, menos a mais fina, lançam sombra por `ShadowTwin` até `lod.cast_props_m`.
- **Fotos.** `res_a`, `mid_a`, `res_c`, `lot_a`: varandas, peitoris e marquises iguais, sem briga de profundidade. `low3`: GPU 9,23 ms (8,93 sem as sombras novas), primitivas 4,81 M (5,00 M). A primeira leitura de 14,4 ms foi carga de outra sessão: repetida com a GPU livre.
- **Falta medir:** a memória do nível perto depois disso.

### Ciclo 10 (os gates de verdade)

- **Causa da lentidão.** `tools/converter-slice` (91 GB, no `.gitignore`) e as outras ferramentas entraram na árvore sem entrar em `exclude_dirs`: o gate anda pelo disco, não pelo git. Sem elas, o gate completo roda em 15 a 26 s. O conserto em `rules.json` é do usuário.
- **O que isso escondia.** 82 problemas reais, de várias sessões. Corrigidos aqui: código duplicado (linha de instância, baias, ids, arredondamento do greide), números mágicos, `build_cpu` longo, estado na interface, mapa. Restam 55.
- **Cena instável.** O nome sempre esteve em `dist/quick/suite.txt` (`UNSTABLE`); `quick.sh` só mostrava a última linha, e meu `grep` não casava. Agora mostra. Na rodada com as 31 cenas: 0 instáveis.
- `quick.sh`: arquivo sem linha no mapa não chama mais todas as cenas. Aquela rodada de 31 cenas foi mais uma contra a ordem do usuário, por `native/src/api` não ter linha.

- **Segunda leva.** Arquivos longos divididos por responsabilidade (`road_prefs.gd`, `camera_aim.gd`, `road_network_deck.cpp`, `runq_jobs.sh`, casos de teste à parte), nomes coincidentes com chaves de dados trocados pelo que são. Hash e fotos congeladas iguais: nada mudou no jogo. `capture_roads` compila, mas não foi rodada depois da divisão.
- `quick.sh` apaga `dist/quick/passed` logo no início: nenhuma aprovação sobrevive a uma verificação que falha.
- O restante do gate é schema e `rules.json` (do usuário), mais o que é da sessão do conversor.

### Ciclo 11 (memória do nível médio)

- **Medida.** Por prédio, o nível médio tinha 309 triângulos de telhado, 265 de lote, 141 de relevo e 71 de parede: as caixas gravadas na malha pesavam mais que a fachada. `mid_walls` 1,08 M triângulos e 139 MB.
- **Causa.** Toda caixa de telhado e de lote ia na malha (12 triângulos, ~1,5 KB), onde uma instância custa 64 bytes.
- **Correção.** `Shell::box_sink`: as caixas das malhas perto e média viram instâncias, num lugar só. As do médio com mais de meio metro (`MID_SOLIDS`) lançam sombra; as finas não: ali um texel de sombra já tem um palmo. As do perto lançam até o fim do nível.
- **Resultado** (`low3`, `ultra`): `mid_walls` 502 mil triângulos e 68 MB; `near_walls` de 29 para 16 MB; vídeo 565 para 534 MB; buffers 176 para 145 MB. GPU 8,63 ms (8,18 antes), porque as caixas do perto agora lançam sombra até 224 m. Primeira tentativa, com toda caixa do médio lançando, deu 9,62 ms: reprovada.
- **Fotos** `mid_b`, `lot_a`, `low3`, `low15`: iguais a olho.

### Próximo ciclo

2. Nível distante pelos dados reais (P8).
3. Nível perto também em instâncias (P6, P7): paga a dívida de GPU e memória.
4. Riqueza de perto.
5. Decisão do usuário: `visibility buffer` pediria modificar o motor Godot.

### Ciclo 12: a lanterna da porta à noite (2026-11-05)
- **Cause 1, the black lantern:** `part_lamp` put the metal body in front of the glass (`p.depth - p.sz / 2` moves outward, because depth grows inward), and the glass sat inside the box. Now the whole box is the lamp: the five faces are `SK_LAMP`.
- **Cause 2, the lamp colour:** `kind == 4` emitted the albedo, so the lamp could not be light by day and warm at night. The body is `#f1ebdd` and the light is `lamp_light` `#ffd49a` x `lamp_glow` 3.4 (2.4 / the colour's luminance).
- **Light on the wall:** a `building_wash` surface of the near tier, additive, with the same lit rule as the lantern. The target colour is the one on screen (`#e4d3b3` is `#ffd49a` through ACES); the game adds before ACES, and the raw colour gave saturated orange. So the shader adds `display_to_radiance` of that colour.
- **Radius:** it reaches the end of the wall, not the end of the bay (`wall_before`/`wall_after`), so the bay does not cut the light into a narrow ellipse.
- Close photo at 22h: lantern lit, with glow, and a soft pool on the wall.
- `gates_user.py` (scratch) ganhou `building_wash` e `lamp_light`; o schema de `materials` ainda depende de o usuário rodar o script.

### Export do jogo (2026-11-05)
- `tools/build/build.sh export` a pedido do usuário: `dist/windows/Roadcraft.exe` do commit `c8a63a4e` mais esta correção.
- **Defeito achado ao abrir o exe:** `strings_ui`, `strings_d` e `strings_e` estavam com o importador `csv_translation`, que tira o CSV do pacote, e `DataTables.load_translations` lê o CSV. Os três agora são `keep`, como os outros. Toda parte nova de `strings*.csv` precisa do mesmo `.import`.
- Aberto no desktop oculto por 600 quadros: 0 erros no log. A tela do exe não foi olhada.

### Ciclo 13: a câmera que puxava e a cidade de longe (2026-11-05)
- **Camera:** `pull_full_half`/`pull_none_half` were 60/240, now 4.8/19.2: the eye was pulled in the normal city view. Two rules were missing: no pull when the looked-at point is on a building, and weight by depth (`PULL_SOFT` 1.5 m). Added in `city_core_sight.cpp` (`sight_pull`), with `eye_free`: at any distance the eye does not stay inside a building.
- **Roads vanishing:** roads went only to `road_end_m` (2.2 km from the camera). New `FarRoadRenderer`: the whole map, one mesh per superchunk, top faces only, in the same `road` material, folded by `chunk_mask` where the detailed chunk stands. No markings, embankment or bridges at this tier.
- **Cidade cortada a 4,5 km:** o alcance dos prédios agora cresce com a altura do olho (`lod.far_per_eye`).
- **Faces pretas:** era buraco. `walls()` do nível distante testava um ponto só, no térreo e no meio do lado, e tirava a parede inteira. `lean_walls` desenha por vão e por andar, juntando o que é igual; o térreo com outra fachada ganha faixa própria.
- **Telhados de longe:** voltaram coroamento, casas de máquinas e o equipamento grande (`emit_roof_plant` com `coarse`), iguais aos de perto.
- Fotos: mapa `km16`, semente 7, vistas de subúrbio e de longe em `ultra`, comparadas antes e depois. A paridade da cor da via na troca e a câmera em movimento não foram olhadas em foto.
- Pendente: o `gates_user.py` (scratch) ganhou `pull_soft_m`, `far_step_m`, `far_pull_m` e `far_per_eye`.

### Ciclo 14: terraço e venezianas (2026-11-05)
- **Terrace without deck:** `b.cores.empty()` removed the terrace from every building with a stair core. Now the rule is another block on top (`built_on`), and the mid tier draws it too.
- `emit_roof_terrace` redone: round planters and water tank (`cylinder`, now for all roofs), loungers in the right direction and step, a pergola independent of them, table and stools at the right height; the seed uses the block id (`vol + 1`).
- **Dark shutters:** `FIN_WOOD` darkened the colour. Now plain paint, `PART_PAINT`; the third colour is `#55785c`.

### Ciclo 15: pontes e pintura de longe, câmera em movimento (2026-11-05)
- **Pontes e pilares:** os chunks com ponte, viaduto ou túnel têm stream própria (`raised_stream_`) com o alcance dos prédios. Foto de um viaduto de 800 m: de perto inteiro; a cerca de 4 km os pilares ainda aparecem.
- **Pintura de longe:** as linhas entre faixas no fragmento de `road.gdshader`, por filtro de caixa da largura do pixel sobre a da linha (uma linha fina em geometria cintila; a pesquisa dá o mesmo caminho: filtrar pelo `fwidth` ou passar à média). A zebra é a média das barras. Sem linha de parada e sem pintura de vagas nesse nível. Ganho discreto nas fotos.
- **Câmera:** `_walk_test` no `camera_input_test`: 90 passos sobre a cidade a 250 m, zoom variou 0,00 m nos dois renderizadores. O teste não foi rodado contra o código antigo.
- Documentação: uniforms por instância até 16 por shader, nos valores por instância, com a nota do Compatibility só sobre índice de textura. `CUSTOM0` ausente na malha: a busca não achou resposta oficial; os testes no Compatibility e no Forward+ passaram com as vias detalhadas sem esse array.
- Brighter room glow: the room formula (`pane_lamp`) was not changed, and neither was the glow threshold.

### Ciclo 16: provas, demolir e o canteiro à noite (2026-11-05)
- **Teste da câmera contra o antigo:** com `pull_full_half`/`pull_none_half` em 60/240 o `_walk_test` falha nas duas checagens (o olho puxado, zoom variando 1,24 m); com 4,8/19,2 passa.
- **Código do Godot:** atributo ausente na malha liga o buffer padrão, zeros em `CUSTOM0` (`mesh_storage.cpp`), então a mesma `road.gdshader` serve aos chunks sem o array. O glow recebe `environment_get_exposure` (`renderer_scene_render_rd.cpp`).
- **Demolir:** o clique leva a rua de cruzamento a cruzamento (`street_of`), um passo de desfazer. O corte em `road_max_seg_m` fica, porque decide por pedaço o que é ponte.
- **Canteiro preto:** o disco da poça estava a 0,164 m (calçada 0,15 + `pool_lift_m` 0,014) e o canteiro tem 0,17 m. `pool_lift_m` 0,03.

### O zip (2026-11-05)
- `build.sh package` copiava a fonte arquivo por arquivo: quebrava em caminho com acento (`git ls-files` devolve entre aspas) e, corrigido isso, passava de 9 minutos com 16 mil arquivos. Agora é `git archive` da HEAD. O arquivo que saiu desse jeito não era zip: o `tar.exe` do Git Bash ignora o `.zip` e grava tar sem compressão (954 MB, múltiplo de 10.240). O zip agora é do `shutil.make_archive`: 452 MB, formato conferido com `zipfile`.

### Ciclo 17: o Pare e as lentes apagadas (2026-11-05)
- **Stop:** `DirSeg::must_stop` by a `RULE_STOP` sign or by all-way control; `may_enter` only lets in a vehicle that stopped at the line (`Vehicle::stood_at`, `traffic.stop_sign_ms` 0.16 m/s). The first version, by speed only, locked the junction: the car pulled away, passed 0.16 and lost its turn; the test caught it.
- **Controle por perna:** parada, preferência e minirrotatória não têm mais via principal (`leg_rule`); `approach_rule` levanta a placa em toda perna; o automático aceita Pare (`AUTO_ALL_STOP`).
- Teste `flow_control_test`: Pare nos 156 cruzamentos da cidade, 679 veículos passaram, nenhum sem parar antes.
- **Dark lens:** its own colour and emission (`props.lens_dark`, `dark_radiance`), in place of `lens_dim` 0.1; the ratio 16 of EN 12368 no longer applies. Close photo, by day and at night.
- O teste de minirrotatória e o de preferência em toda perna não existem ainda; a regra deles só foi lida no código.

### Ciclo 18: a via é uma só para o jogador (2026-11-05)
- **Flower bed:** the "darker" look came from samples in different places; the pending item was removed.
- **Por que o corte fica:** a malha de um trecho mora no chunk do meio dele, com a caixa do chunk; sem o limite de 112 m uma via longa sumiria da tela. Quem manda é o render, não a altura.
- **Defeito meu do ciclo 16:** `street_of` atravessava o nó do Dividir. Agora o corte é marcado nas duas pontas (`SEG_RUNS_ON_A/B`, nas flags, que o save, o desfazer e o inverter carregam) e `RoadNetwork::pieces_of` só anda pelas marcas. Saves antigos não têm as marcas: cada pedaço segue sendo um.
- **Inteira agora:** demolir, Melhorar sem Shift, Informação (comprimento, faixa desenhada, tráfego somado), inverter mão única e duplicar (a curva refeita com `joined_quad`).
- Teste novo em `road_money_cases`: via de 5 pedaços, Dividir, um clique leva um lado, o outro fica.

### Ciclo 19: vias em relevo, ao vivo (2026-11-06)
- **Janela ao vivo:** `play.gd -- --live=<pasta>` carrega `tests/visual/live_driver.gd`: o jogo aberto na tela do usuário toma passos de fora (câmera, comandos, fotos). A ordem é não fechar a janela: `tools/build/free_dll.sh` tira a DLL em uso do caminho e a compilação segue com o jogo aberto.
- **Road breaking on the hill:** the base line was made and limited to 12 % piece by piece; on a slope the pieces reached the node at different heights, the node took the average and the road dived 14 m in the last 8 m. `grade_chains` makes the line of the whole road as one profile. Measured after: maximum ramp 0.120.
- **Grama sobre o asfalto:** empate de peso entre dois pedaços em volta do corte; o primeiro ganhava e deixava a amostra 0,7 m acima da pista. Desempate pelo mais perto (`best_d`).
- **Veículos:** a pose não tinha inclinação. `Pose::rise` vem da frente e da traseira na via, e a carroceria gira por ela.
- **Rocha em mosaico:** a normal do terreno era uma por célula, e ela decide grama, terra e rocha. Agora vem dos vértices, interpolada.
- **Borda do mapa:** os vértices de fora do mapa andavam com a câmera. Ficam presos em `map_half`, com rocha no plano da parede. A foto ainda não enquadrou a parede.

## Próximos passos (em ordem)

1. **Photo of an urban-rural span** (not taken yet).
2. **Elevation:** what is in the Pendências of `CLAUDE.md`.
3. **F7 lab and the `verify` gate.**
4. **Names still in Portuguese:** the `sementes` keys of `features.json`, the messages of tests and captures.

## Regras de convivência (não esquecer)

- **Comandos longos sempre em segundo plano:** o usuário teve de parar o chat porque a sessão travou num comando e não recebia mensagens.
- **Processos:** parar só os próprios, pelo PID. O jogo roda só por `runq`/`hidden_run.ps1`, uma instância por vez.
- **Arquivos do usuário:** gates, hooks, schemas e `.claude/settings*`. Um comando que cite `tools/gates` é recusado pelo hook: use as ferramentas Read/Grep ou um script.
- **Commit:** `git add` e `git commit` separados, com logs de `dist/tests` mais novos que o código. Identidade Coordenadora `<coordenadora@roadcraft.local>`, trailer `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- **Nomes de arquivos, pastas e código em inglês.**

## 2026-11-06: as plantas do Construir
- Desenhar e Massa › Forma do bloco: retângulo, L, U, círculo, hexágono, octógono, chanfrado, cruz e cruz escalonada (`body_plan.cpp`). `MAX_FACES` foi a 24, porque o círculo tem 24 lados e `faces[side]` é lido sem conferir; o corpo gravado foi à versão 3 e abre os de 16 faces.
- Changing the shape carries what was on a wall to the new wall facing the same side, and divides the openings by the ratio of the lengths. With relief, it refuses and asks to align. Reapplying the same shape changes nothing: on a concave plan several walls face the same side and the map swapped the rhythms, found by the test.
- Pitched roof over a plan: it used to cover the whole box and pass over the gap of an L. Now each plane is a convex plate cut by the plan's triangles and the walls rise to it, with the exact top on each piece (`roof_outline.cpp`).
- `bays_of` contava os vãos de uma parede de planta pela largura da caixa: agora pelo comprimento da parede. Recuo e ala recusavam bloco com forma: agora valem (`offset_ring`, `simple_ring`, ala saindo do meio da parede).
- Fotos analisadas em `project/dist/rq/shapes`: as 9 formas com telhado plano, duas águas e quatro águas. A linha preta num prédio branco era o meu plano de foto, que pôs duas águas no bloco de baixo de um corpo de dois volumes.
- Falta do Desenhar: primitivas e os gestos de ala, empilhar e cortar por arrasto.
- O sorteio do `edit_fuzz_test` mudou com a forma do prédio desenhado e achou um defeito antigo: pôr a mesma faixa onde ela já estava deixava um passo de desfazer vazio (`j_set_crossing`). O hash do estado mudou só pela versão do corpo gravado (grades, graph, heights e lots iguais).

## 2026-11-06: os blocos do corpo
- Cutting is a boolean: an empty block removes its space from the solids without changing any block. `Volume::mode`, `b40/blocks.cpp` resolves the body into solids, and `record_mass` is what the render, the walls of fallen bodies, the line of sight, the doors and the Interior read; editors read the blocks. Empty, intersection and exclusion with floor bands.
- `core/polygon_ops`: as booleanas em milímetros inteiros pelo Clipper2, com o furo cortado em dois porque uma planta não tem furo. O cabeçalho entra como `<clipper.h>`, igual a `<meshoptimizer.h>`, com a pasta dele em `CMakeLists.txt`: o gate `CAMADA` só aceita biblioteca sem barra no caminho. A regra não foi tocada.
- Ala, Empilhar e Recortar: a carta arma, o arrasto faz o retângulo no referencial do lote e a grade o assenta. A ala se desenha no chão; o bloco no telhado e o recorte, na altura do telhado, onde o raio do ponteiro é lido (o chão sob o ponteiro fica muito atrás de um prédio alto). Uma caixa não ganha contorno: o retângulo fica um bloco comum.
- Fotos em `project/dist/rq/blocks`: canto recortado, pátio fechado (dois sólidos), bloco no telhado, ala, corte de lado a lado, interseção, exclusão e corte só no bloco de cima. As fotos acharam um defeito meu: faltava a parede sob uma das águas do dente de serra, porque um quadrilátero com dois cantos no mesmo ponto vira pelo primeiro triângulo; agora é um triângulo.

## 2026-11-06: a aba Interior
- `Core` gained a type (lift, stair and both) and the render a 12-step stair along the back of the shaft; the stair alone takes two modules. `Body::furnishing` keeps the floors furnished by the player: the first change on a floor keeps what was there, and a furnished floor equals no other (`Interior::type_of`).
- Cartões de núcleo, de arrumar e as 22 peças; o clique lê o raio na altura do andar cortado, porque o chão do lote pode estar noutra altura. R gira. Abrir a aba com um prédio escolhido abre o térreo cortado.
- Corpo gravado na versão 5, com leitura dos antigos. O hash muda só por isso.
- Fotos em `project/dist/rq/inside`: escada, elevador, os dois e as peças postas no térreo cortado. Falta o fantasma da peça sob o ponteiro.

## 2026-11-06: traçados e pincel
- `LotElement::angle`: uma peça em ângulo é a mesma peça num referencial girado em volta do meio dela, porque tudo que um elemento desenha passa por `pl.lot`; nenhum tipo de peça mudou. Corpo gravado na versão 6.
- Traces (wall, fence, footway, stair): each click a corner, Enter sets it in one undo step, Backspace goes back a corner; one piece per stretch, and the stair climbs the chosen floor along the whole trace. Tree in the Parts tab. Roof equipment is not repeated in the Parts tab: it is in the Roof tab.
- Cores: Pintar liga o pincel, um acabamento o carrega, Copiar estilo carrega o que o escolhido usa.

## 2026-11-06: modelar blocos e formas básicas
- `body_model.cpp`: extrudar a face, tijolo contra a face, recuar a face numa moldura, crescer a planta, chanfrar, mover e criar pontos com o que estava nas paredes indo junto, copiar, centrar, girar, deslizar levando o que está em cima e encaixando nos outros blocos, e soltar num prédio à parte no mesmo lugar, um passo de desfazer.
- Basic shapes: the first version asked for a lot and was refused (`refuse.lot.small`): a basic shape is a body, not a lot. Now it is a one-block body through `place_body`, with the card's plan and roof. On the roof and on the wall it is one more block.
- Copy and rotate carry what is on the roof along.
- Fotos em `project/dist/rq/model`: as oito formas, bloco girado 15°, chanfrado, tijolo com face recuada, forma sobre recuo e na parede.

## 2026-11-06: as alças do bloco
- `body_handles` diz onde ficam (faces, pontos, andares, mover, girar, escalar; os três últimos no canto mais perto do olho), `body_draft` faz o corpo numa cópia para o fantasma, `body_apply` edita ao soltar, `building_place` leva o prédio inteiro e funde se pousar em outro (`place_body`). `HandleMarks` desenha sobre a imagem pela projeção da câmera.
- Provado com entrada real (`look.gd`: `tap`, `press`, `moveTo`, `letGo`): face puxada com o fantasma e a etiqueta (10,3 m), andares, mover e girar. Fotos em `project/dist/rq/hand*`.
- As fotos acharam: girar trocava a cara do prédio, porque `place_body` tira semente da posição. Um prédio movido guarda semente, estilo e idade.

## 2026-11-06: o meio-nível
- `Volume::lift`: os emissores desenham um bloco pelo referencial dele, então um bloco erguido é o mesmo bloco num referencial mais alto (`block_frame`). O que mede altura fora deles passou a `Body::block_z`. Uma parede só some atrás de outro bloco se ele a cobre inteira na altura (`covered`).
- No corte, cada ponto do piso, parede, núcleo e móvel fica na cota do bloco onde está (`lift_at`) e os blocos que se encontram em cotas diferentes ganham o lance entre eles (`level_flights`). Clicar onde um bloco vazio tirou a parede escolhe o vazio. Corpo gravado na versão 7.
- Alças da cota e do relevo do vão escolhido.

## 2026-11-06: a referência 3D
- `ReferenceModel` abre .glb e .gltf em tempo de execução pelo `GLTFDocument`, translúcido, e alinha, gira, espelha, esmaece, esconde e tira. As fotos do levantamento saem de um SubViewport próprio com `DEBUG_DRAW_UNSHADED`; a profundidade não é um shader (a saída de 8 bits passa pela conversão sRGB): os triângulos são rasterizados em C++, o que também dá a mesma resposta em qualquer placa.
- `body_survey.cpp`: blocos pelas faces de cima, base de outra cor, cor das paredes pela mediana do que é claro, cada vão pelo escuro que mostra, telhado de cima, lanterna no que passa do topo. `reference_test` escreve um .glb, lê de volta e confere: dois blocos, a torre sobre a base, parede #cc804d (0,8/0,5/0,3), um passo de desfazer.
- Limite: o teste usa caixas; um modelo real de prédio não foi lido, porque não há um na máquina.

## 2026-11-06: pontes e viadutos
- A pendência "sem viaduto" estava velha: `Structure::ELEVATED` tem tabuleiro, pilares, encontros e guarda-corpos; provado em foto no mapa km10 (via a 9 m). Tirada do `CLAUDE.md`.
- Cable-stayed span, made as an improvement: a bridge street of 320 m or more becomes spans of ~220 m, an H tower in each and a fan of cables to both edges of the deck, counted along the street because the road comes in pieces (`road_stays.cpp`). Photo: a 900 m bridge over water, four towers.
- The photos showed trees planted on the deck. A tree needs soil: I removed them from roads on structures.
- The far road painting pending item was removed: the game's far tier still filters the lines, and past `road_end_m` (2,200 m) a pixel of 1080 rows covers ~1.5 m, so a 0.4 m stop line is 0.27 px. The embankment is painting on the already shaped ground, which is still there.

## 2026-11-06: sombra de contato
- First version rejected in the photo: inside the atmosphere pass, with a march over the building heights to find where the sun reaches. The heights do not know what is on the roof, and at the foot of the box a hard-edged band came out inside the shadow that already existed (darkened twice).
- A causa é o passe de tela não saber onde o sol chega; o atlas de sombra não é exposto. A nativa (GH-118045) só existe no Godot 4.8 dev 6; a 4.7 não tem nada disso na API. A saída é `LIGHT_VERTEX` (docs; na fonte, `vertex = light_vertex` antes da luz, e o `shadow` chega ao `light_compute` no lugar do `ATTENUATION`): um passe próprio com luz (`contact.gdshader`) põe o pixel no lugar dele e lê a sombra real do sol ali. Multiplica a imagem, e só quem achou obstáculo chega à luz; o resto sai por `discard`. O `max` entre as direcionais dá a do sol sem identificá-la, porque as luzes planas não têm sombra.
- Liga em `tier.post` com o sol com sombra e a chave `canyonLight`, então só em `high` e `ultra`.
- Armadilhas da medida: sem `--frozen`, duas rodadas iguais diferem 1,55%, mais que o efeito (1,49%), e o mapa de diferença é todo ruído; em `--frozen` o `time_scale` é 0 e `wait` nunca termina, use `frames`. O `look` abre sempre em `high` (`PairCamera.PRESET`) e ignora `--preset=`: a suspeita de passe desligado por `medium` foi engano meu. E `con` é dispositivo reservado no Windows: `con.json` não abre.
- Medido em `high`, congelado (A/A 0,00 a 0,03%): 0,19% da foto muda numa vista de torres, menos nas outras, em linhas finas na junção de friso e parede; a sombra da caixa fica intacta. GPU, duas rodadas ligado contra uma desligado: perto +0,1 a +0,25 ms, rua +0,05, de longe nada, da ordem do ruído.
- `screen_depth.gdshaderinc` é agora o dono do que um passe lê da profundidade, antes repetido na oclusão e na atmosfera; `screen_pass.gd`, o quad que multiplica.
- Testes: 833 frases em português (838 trocas) passadas para o inglês por tabela de texto exato (uma frase, uma tradução, em todo arquivo).
- `FALHOU:` virou `FAIL:` e `FALHAS:` virou `FAILURES:`. Antes a suíte só via `^FAIL`, então um `FALHOU:` com saída 0 passava calado; agora todo leitor usa `\bFAIL\b` (o hook já usava), para o resumo `FAILURES: 0` não contar como falha.

## 2026-11-06: carros e pendências velhas
- Mirror: a box in the game. Now `agent_pod`: a shell of superelliptic section that closes in a dome toward the front, flat bottom and glass set in the same shape; ~100 triangles, only at the cabin tier. The "dark thread" of the arch was the door seam, which exists on a real car.
- Old pending items removed: `noise2d` no longer exists; the game's roundabout cut roads and did not check what was left, and now refuses like the other edits (`squeezed_worse`).
- Stone facade looking like camouflage: the recipe (`cellHash * 0.2`) gives almost twice the light between neighbouring blocks. Dressed stone varies a few percent: `tools/assets/calm_stone.py` brings the blocks' deviation to 4/255 without touching joints and grain.

## 2026-11-06: a noite
- Contact-sheet audit (`capture`, 69 photos): the biggest defect of the whole game was the night. The pool was a `blend_add, unshaded` disc: it added the same light over any colour, so the asphalt turned beige, lighter than the footway, and nothing standing received light.
- Pesquisa: Forward+ aceita 512 elementos por vista; `light_projector` pede sombra (fora); na prática de iluminação pública os postes ficam a até ~5 alturas e pede-se U0 >= 0,4, então preto entre lâmpadas não é real. Godot 4.8 traz sombra de contato própria, a 4.7 não.
- Plano revisto antes de codificar: disco multiplicando daria (1+a)(1+b) onde duas poças se cruzam. O shader da via já desenhava as poças de longe, pelos mesmos `span_m` e `head_u` dos postes, então os discos saíram: menos malha, memória e overdraw.
- `LampLights`: spots nas lâmpadas perto do olho que a vista alcança (48/96/160 por nível), cada luz fica no seu poste, fade pelo raio suavizado e fade complementar na via (`lamp_lit`). Depois delas a poça da via multiplica o albedo. Sem luzes (nível baixo, Compatibility) a via desenha tudo.
- Sombra: as 6 mais perto (12 no ultra). Primeiro custou +4,6 ms perto e +1,9 ms de alto, redesenhando prédios em cada mapa. Com `light_set_shadow_caster_mask` (veículos, pessoas, mobiliário) e 80 m pelo ar: +2,1 ms perto, zero nas outras.
- A lua: à noite o passe de sombra do sol rodava para a lua, invisível sob os postes. `Daylight` agora é o dono do interruptor (eram dois lugares) e tira o passe da noite fechada em diante.
- Medido em `high`, à noite, RTX 3060: perto 10,06 -> 9,15 ms, rua 7,43 -> 5,56, bairro 10,86 -> 8,78: mais luz, mais barata que antes.
- Armadilha: `capture` roda no `tests_preset` (`medium`, sem sombra de poste); `look` em `high`. Para julgar a sombra, `look`.
- `inspector_test` instável sob carga (`the sound comes after the lightning`): só apareceu agora porque `FAIL` passou a ser lido. O timer conta quadros e tem a precisão de um; o teste comparava com o relógio de parede. Descontar o quadro não bastou sob carga; o teste agora confere o atraso em si (`Thunder.delay_s`), sem relógio de parede.
- Os dados: `lamp_light` de `streets.json` é estrito (li só o começo do schema e supus aberto). Os números foram para `tuning.json render.lamp_*`, a cor é o `glow_color` do `lamp_glow`, e `pool_along`, `pool_sides` e `pool_lift_m` ficam sem uso enquanto o schema pedir.

## 2026-11-06: the open ground
- Fog: `fog_density` 1.5e-4 with dark blue `air_high` left the field turquoise at 1 km and the distant woods black. User's order: density 0, and the engine fog turns off with it.
- Sparse woods: I suspected the arithmetic (`scale * share`) and replaced it by `tier_share`, but nothing changed, because the plan exceeds the budget. The cause was only the `high` tier cut (0.54). At 1 the patches are full, and the cost in the city views is zero (10.16 against 10.10 ms near), because the woods are outside it: `high` went to 1.

## 2026-11-06: torres pretas
- At a medium zoom, whole buildings without windows (black where the finish is glass). Sweeping the zoom in from far, only at z4 and z5. Hypotheses dropped: different AABB (`fit_reach` already equalizes), missing `flat` (temporary log: every subchunk with bays has `flat`).
- Causa: `MultiMeshDraw::range` passa margem de histerese (3%) e o `range(MeshDraw)` do `flat` passava 0. Com `Fade Mode: Disabled` a margem desloca a borda pelo sentido da câmera: vindo de longe o `flat` saía em 350 m e as baias só voltavam em 339,5, e nessa faixa ficava a parede lisa de trás. Depende da direção, por isso nenhum teste via. O `flat` agora tem a mesma margem no `begin`, e a troca é complementar nos dois sentidos. Perto para médio não tem o problema: 44 m de sobreposição contra 5,4 m de margem.

## 2026-11-06: faróis
- Da auditoria: o quintal "vazio" foi erro meu pela miniatura; de perto há canteiros, piscina, varal, arbustos e árvore. Sem mudança.
- Faróis acesos não iluminavam nada. O miolo de `LampLights` virou `LightPool` (render/), e `LampLights` e `HeadLights` são duas fontes finas sobre ele: o mesmo fade, a mesma troca estável, sem duplicar. Cor do `head_glow` do material `agent`.

## 2026-11-06: the mirrored map
- `look.gd` passed the map's `y` as `z`. The map's north is `-z`, so every `setView` showed mirrored places, including views a-f of the previous round that I reported as "same view". The tool's comment claimed the opposite. `setView` is fixed, and `frozen_plan.json` was negated so the frozen photos stay where they were. The woods photo was redone after this.
- With the view in the right place, my fog conclusion fell: the air (β = 3.912/30000 per metre, 1200 m scale, ceiling 0.55, `#3d5a82`) is what `air_high` and `air_height_m` carry, and removing it was my mistake. The real defect was the strength: 1.5e-4 where it should be 0.55 × β = 7.17e-5, hence the turquoise field and the black woods. It came back with the right value.
- Mirrored city: `zone_at` read the map's `y` in the world's `z`. Centres, industrial sector and noise came out mirrored for the seed, though the streets (symmetric grid) did not show it. Now the arithmetic is done in the map's `y`.
- `sky_clouds` and `cloud_shadows` were dead options: only `quality_test` read them, and it passed. Now `SkyView.clouds_drawn` obeys them (either one builds the pass), and the test checks the effect.
- A cidade certa expôs três coisas. Câmera: um solavanco de 1,35% em `half 6`, com o alvo de `over` saltando ao passar sob um prédio alto. Uma mola de 2ª ordem dá toda a aceleração no primeiro quadro de um degrau; `CameraDamp.ease_to` põe uma segunda mola à frente, cada uma com metade do tempo, e sai e chega do repouso. `building_edit_test` pegava o primeiro prédio alto, e o novo já estava no limite de blocos e com os vãos pedidos: o teste agora escolhe um em que toda edição muda o corpo, e "mudou o corpo" segue exigido. `building_light_test` cruzava 1 passo de 8 bits por arredondamento da névoa na referência: a névoa sai desse teste, que compara a superfície sob a mesma luz.
- O mapa 0 tem 97 docas: a pendência "sem indústria" era o espelho. Saiu do `CLAUDE.md`. A foto `park_sample` segue achando docas; `capture --only=trafego` avisa que não acha guia nem ciclofaixa sem prédio alto perto, com a cidade nova.
- White crowns against the sun: without the specular the white goes, so it is the specular. The crown's normal is a volume normal, and the Fresnel grazing the edge lights as a smooth surface what are hundreds of leaves. `leaf_sheen` removes the sheen where the view grazes the normal and keeps the frontal one.

## 2026-11-06: the 20 km map
- `town` and `metropolis` went from 10 to 20 km a side; the river and the lakes, which depend on the half side, move with it.
- Custo medido com a mesma cidade: GPU igual (perto 10,04 contra 10,09 ms, cidade 5,70 contra 5,66), abertura 96 contra 69 ms, e +250 MiB de memória e +260 MiB de vídeo. Isso passa da regra de memória; são 300 tiles de chão a 0,78 MiB, os três mapas do Terrain3D em 256², e é o preço da área pedida.
- Save 8: o header guarda o lado da terra. Sem isso um arquivo antigo abriria noutro chão, porque a terra vem da semente e do lado. O arquivo antigo abre em 10 km, e o teste corta um save real de volta ao formato 7 para provar.
- End of the map: the game ends in the sea.
- Pedido do usuário: o `dist\\windows\\Roadcraft.exe` da pasta `city` era de 05/11 23:53 e a pasta estava 77 commits atrás. "Enviado para main" não chega ao jogo que ele abre: cada entrega precisa de um executável novo, e isso deve ser dito.

## 2026-11-06: os gates de camada
- `lot_land` lia vias e calçadas de dentro de `sim/lots`, camada que não pode. Em vez de pedir regra nova, foi para `sim/citygen`, ao lado do `lot_gen`: é o mesmo assunto, lotes contra as ruas, e a camada permite. Três gates a menos, sem tocar em `rules.json`.

## 2026-11-06: a main é o lugar de trabalho
- Ordem do usuário: nada de worktree; tudo em `Desktop\city`, na `main`. O código já era o mesmo (`c4f6848b`); vieram as referências de `dist/quick` (digest e fotos congeladas), que não estão no git. Nenhuma pasta fora do projeto foi tocada: a limpeza do disco é do usuário.
- `hidden_run.ps1` e `tools/process/bin/gate*` ainda apontavam para `C:/Projects/Roadcraft-windows`: agora saem da própria pasta, e o projeto não depende mais dela.

## 2026-11-06: the bomb in the city, by real click
- `look.gd` gained `{"control": "what it says"}`: a plan presses buttons by their text.
- Found by real click: initial force 1, bomb only on the ground under the pointer, and a slider in place of the step. Now it falls on the first building in the line of sight (`body_pick`), opens at 5 and steps 1 by 1 up to 10 and 5 by 5 after (`StrikeStrip`, values in `tuning.strike`).
- O primeiro passo que escrevi parava em 6: lambda de GDScript guarda cópia do local. `shell_test` agora aperta o botão sete vezes.
- Missing, seen at force 45: the explosion cloud on a building is small, and the neighbours fall whole (21 ruins) instead of losing only the part hit and staying up.

## 2026-11-06: o prédio perde só o que o golpe alcança
- At force 45, 21 buildings were on the ground, where neighbours should lose only the part hit. Now the simulation keeps the mark of the blow (`DamageState::scars`, in the save and in undo) and the render uses the same fractured building: what the blow reaches and what was left without support falls, the rest stands. In the same scene: 7 ruins and 7 broken buildings standing.
- A causa do resto era o limite único de colapso (5 psi, de casa) para toda torre. Prédio de 5 andares ou mais agora é estrutura de concreto, 20 psi.
- `FORCE_SCALE` was removed: the force goes from 1 to 50 directly.
- The smaller cloud is not a defect: in the city the click hit a low building between towers; on open land it is full size.
- Os 15 gates restantes estão em `tools/gates/rules.json` e nos schemas, que só o usuário altera; a ordem "você muda o hook" foi sobre a prova do commit.

## 2026-11-07 22:20: the Android APK
- User's order: an Android version, the APK now, download what is needed. Delivered `dist/android/Roadcraft.apk` (80 MB, arm64-v8a,
  targetSdk 36, signed v2 and v3). Not run on a device: none was connected.
- Toolchain in `C:/Projects/toolchains`: JDK 17 (Adoptium 17.0.21+9), the SDK's platform-tools, build-tools 35.0.1, platform 35 and
  NDK r28b, the versions the Godot 4.7 Android export page lists. The new `sdkmanager` (cmdline-tools 16111833) is a shim over the
  "android" CLI and refused the package names, so the packages came straight from Google's `repository2-3.xml`. Godot's editor
  settings point `export/android/java_sdk_path` and `android_sdk_path` there.
- Causes, each with its fix:
  - `native/CMakeLists.txt` coded for x86-64-v3 on every target: `-march=x86-64-v3` now only when the target processor is x86_64;
    arm64 keeps armv8-a. `-ffp-contract=off` stays on both (arm64 compilers fuse by default), so a city is the same number on both.
  - `way_body.cpp` read the clearance only with AVX2 gathers: the gather path is under `__AVX2__`, and a machine without it goes
    through the scalar loops that already finished the quads (the same `Field::clear_at` sum, floor and bounds, the same count of
    looks). The PC is untouched: the x86 objects built with both build folders' exact flags, before and after, are the same (the
    release bitcode byte for byte, the debug object but for the COFF timestamp byte); the Windows DLLs were not rebuilt.
  - `CITY_PROFILE` had no "off": any value but `generate` read the profile. `none` is now none (the profile is of the PC build, and
    the NDK's clang is older than the one that wrote it).
  - `LINK_SEARCH_START/END_STATIC` would ask for a static libc, libm and libdl on Android: set for the other targets only. The
    library needs only the system's `libc.so`, `libm.so` and `libdl.so`, exports `citycore_library_init` and has 16 KB segments
    (Android 15+). A scratch configure of both Windows folders gives the same compile line for all 458 files and the same link.
  - Terrain3D's `.gdextension` header, written with `#` (uncommitted work of the audit session), made Godot's ConfigFile lose
    `[configuration]`: `entry_symbol` read as missing and Terrain3D loaded on no platform. ConfigFile.load of HEAD, of the header
    stripped and of a `;` header keeps the key, only `#` loses it: the header is `;` now. The Android arm64 libraries came back
    from HEAD (upstream 1.1.0).
  - Android needs ETC2/ASTC: `import_etc2_astc` is on, and the 23 VRAM textures import both (21 ETC2 and 2 ASTC in the APK); the
    PC's BPTC is unchanged.
- `tools/build/android.sh` (`core`, `export`): its own script, since `build.sh` sits at the 90-line limit and the Android toolchain
  is its own concern. The release keystore (keytool, RSA 2048) and its env file are in `C:/Projects/toolchains/keystores`, never in
  the repository.
- Verified: both `android.sh` targets end to end, `apksigner verify`, `aapt2 dump badging`, the APK's libraries and data. Gates on
  the changed files: only `MAPA_DESATUALIZADO` for files the audit session moved out or added (its batch updates the map). Open:
  the run on a phone, touch, the renderer on mobile (`log/pendencias.md`).
- Files: created `tools/build/android.sh` and `project/bin/libcitycore.android.template_release.arm64.so` (ignored by git);
  changed `native/CMakeLists.txt`, `native/src/sim/traffic/way_body.cpp`, `tools/build/build.sh` (header),
  `project/bin/citycore.gdextension`, `project/addons/terrain_3d/terrain.gdextension`, `project/export_presets.cfg`,
  `project/project.godot`, 23 `.import` files, `CLAUDE.md`, `log/mapa.md`, `log/pendencias.md`; restored
  `project/addons/terrain_3d/bin/libterrain.android.{debug,release}.arm64.so`.

## 2026-11-07 22:50: the scenic Sao Paulo
- User's order: a beautiful scenic city like Sao Paulo, about 5 x 5 km, in the game, opened and shown. Delivered the map
  `sao_paulo` ("Sao Paulo (cenografica)" on the start screen): a 5 km square laid out by hand on a 10-tile map.
- Problems found: the only cities were drawn from a seed (one straight grid round a centre, a straight canal, lakes by chance);
  the graph builder hard-wired the generated city's ranks, zones and profile variety (`road_gen_graph.cpp`); the land came only
  from the seed (`TerrainGen`); nothing could say where a river, an avenue or a park goes.
- Plan and causes fixed, each with one owner:
  - `sim/catalog/scenic.h` + `api/scenic_loader.cpp`: a scenic city as data (land, districts, roads, parks, works, heat,
    zoning, park trees), strict, linked from `maps.json` by `scenic`. Road names are i18n keys (`scenic.road.*`,
    `strings_d.csv`), translated by the loader: no player text outside data/i18n.
  - `sim/terrain/scenic_land.*`: the authored land (plateau, ridges, rivers in channels with floodplains, lakes under their
    lowest shore). `TerrainGen` takes it for a scenic map; `clear_relief` restores the same land.
  - `road_gen.h GraphRules`: what the graph builder asks of a city (rank, which street yields, the zone beside a street, its
    final section). The generated city's rules moved unchanged into `PlanRules` (road_gen.cpp): the digest of the frozen town
    is equal, bit for bit. The density-to-zone table has one owner (`city_plan.cpp zone_of`).
  - `sim/citygen/scenic_rules.*` (rank by profile priority, only locals yield, zoning by parks, works, district and heat lines),
    `scenic_roads.*` (each district's grid inside its Voronoi cell, seams between cells, the plan's roads, district streets kept
    off water, parks and the highways' band), `scenic_finish.*` (park trees, the named roads' plates before the style's names).
- The city: 21,251 buildings (half of them 10 storeys or more), 9,638 street segments, 22,585 lots, 5,849 park trees, made in
  about 1 s; the Tiete (north) and the Pinheiros (west) meeting at the north-west corner with their marginals, the Tamanduatei
  and Avenida do Estado, Paulista on its ridge (50 m over the Tiete's banks) with the Trianon, Centro, Jardins (houses), Pinheiros
  and Faria Lima, Ibirapuera with its lake, 34 named roads, bridges over the rivers, a Mata Atlantica biome.
- Verified: both DLLs; `scenic_city_test` (new: streets, buildings, trees, every named road on its street, Paulista's section and
  height, the Marginal Tiete a highway, Ibirapuera empty of buildings, a city of towers); `quick.sh --ready`: 43 of 45, the two
  failures are `gates_changed` (only the old backlog and the audit session's files) and `photos_check` (see below); captures of
  the plan `sao_paulo` (look group `sao_paulo`), looked at one by one: the first lake shore was a cliff where the ridge tilts the
  park (fixed by a wider `shore_m`), the street shot stood in a building (moved onto Paulista's median).
- `photos_check`: a few scattered pixels of `00-cidade` (once `03-rua_noite`) differ from the goldens blessed at 21:25, the box
  moving from run to run while the digest is equal. HEAD without this change (9a056673, the audit session's own worktree) fails
  the same way: the known residual at the utility wires' joints, depth ties broken by job order (log/review.md 6.6, finding C12,
  fixed in its batch B2). Against HEAD's photos this run differs in 2 pixels of one level. Not blessed.
- Files: created `native/src/core/polyline.h`, `native/src/sim/catalog/scenic.h`, `native/src/api/scenic_loader.cpp`,
  `native/src/sim/terrain/scenic_land.{h,cpp}`, `native/src/sim/citygen/scenic_{rules,roads,finish}.{h,cpp}`,
  `project/data/catalog/scenic.json`, `tools/gates/schemas/scenic.schema.json`, `project/tests/core/scenic_city_test.{gd,tscn}`,
  `project/tests/visual/sao_paulo_plan.json`; changed `catalog.h`, `catalog_loader.{h,cpp}`, `city_core.cpp`, `city_gen.{h,cpp}`,
  `city_plan.{h,cpp}`, `road_gen.{h,cpp}`, `road_gen_graph.cpp`, `road_gen_tidy.cpp`, `nature_commands.cpp`,
  `terrain_gen.{h,cpp}`, `maps.json`, `start.json`, `strings_ui.csv`, `strings_d.csv`, `data_tables.gd`, `maps.schema.json`,
  `rules.json` (the schema line), `suites.json`, `CLAUDE.md`, `log/mapa.md`, `log/pendencias.md`.
- The DLLs are not in this commit: a DLL built in another folder carries that folder in godot-cpp's `__FILE__` strings, so the
  audit session rebuilds both from HEAD with its folder-independent build (`-ffile-prefix-map`, the retrained profile) in its
  next commit. The checks above ran on DLLs built from this commit's sources in the worktree `city-scenic` (HEAD + this change
  only); the user's `dist/windows/Roadcraft.exe` was exported from it.

## 2026-11-08 05:10: zoning by whole lots, as the source game does (English)
**Problems (photos `dist/captures/zoning`, before):** the brush was a 40 m disc that zoned whole road sides: one short stroke
zoned both sides of the street and the cross streets' sides; no lot the streets propose was shown with the Zones tool in hand;
the disc was drawn over the buildings; the Lots tab drew no lot, no front; lots stood 0.6 m apart (`lot_place.cpp`
`WIDTH_TRIM`), so every seam showed two lines with grass between them (the user's photo); the zone colour was cut on an 8 m grid
of its own while the terrain's mesh has a vertex every 4 m (`terrain_bridge.cpp` vertex_spacing = the heightfield's spacing),
so the ground rose through it between its samples.
**Source read:** the converter's extraction `zoning` (60 files, `editor/lotTool.ts`, `world/lots.ts`, `world/openLots.ts`,
`world/growth/zoning.ts`, `render/lotOverlay.ts`) and the whole-program ledger (`ports/all/conversion`). What the player sees
there: the street land cut into proposed lots in every mode of the tool; the brush zones the lots it passes (a proposed one
becomes a lot), the lot under the pointer lit; zoned lots coloured (alpha 0.45, 0.22 built, 0.25 with another tool), outlined,
their front marked with an arrow; the Lots tab's gestures take a proposed lot first; the delete stroke takes buildings off
lots too; changing a lot's zone keeps what stands, taking it off takes what grew.
**Design in this game:** the proposal is `LotCache::proposal` cut once per cell and per cut (works or not: `lot_gen` cuts works
deeper and wider) and kept in `CityState::proposed` (`sim/lots/proposed_lots.h`), dropped only by the new ChangeLog list
`street_cells` (streets or ground changed), never by the player's lot edits; the brush (`zone_commands.cpp` `paint_zone_path`)
zones the player's lot under each dab, else takes the land lot there (`land_lot_at`: a zoned street lot, else the proposal
for the zone painted) as the player's; the eraser makes no lot of unzoned land. The zone layer (`overlay_build.cpp`) draws every
lot whole from the chunk its middle stands in, cut on the relief's own grid (`lot_rings` `lot_cover`), with outline, front
and arrow while the tool (either tab) is in hand; the render learns of lot and building changes through the new ChangeLog list
`lot_chunks` (it was never rebuilt after a lot edit before). The Lots tab asks the core near a point (`lot_corner_near`,
`lot_side_near`, `lot_ring`) instead of reading the whole book each edit, and the book files a lot added or changed alone
(`LotBook::file_one`: the brush grows it to the thousands). The side-painting machinery went (`paint_zone`, `j_set_zone`,
`OpKind::ZONE`, `SEG_ZONED_*`, `RoadNetwork::set_zone`/`player_sides`, `zone.side_offset_m`/`sample_m`, the brush radius).
**Checks:** `zoning_plan.json` and `zones_town_plan.json` photographed before and after (lots touching, the proposal outlined,
the lot under the pointer lit on the relief, two lots zoned by a short stroke with colour, outline, front and arrow, one
building grown on each, a proposed lot split, the eraser taking the zone and what grew; the generated town's lots coloured
by zone with fronts); `road_commands_test`, `save_undo_test`, `camera_input_test`, `edit_fuzz_test` pass (tests and captures
now zone through `tests/zone_stroke.gd`, the player's path). Gates on the changed files: what remains failed at HEAD already
(listed in log/pendencias.md).
**Files:** created `native/src/sim/lots/proposed_lots.h`, `native/src/sim/commands/lot_commands.h`,
`native/src/sim/commands/nature_commands.h` (commands.h split by responsibility), `project/game/tools/lot_erase_stroke.gd`,
`project/tests/zone_stroke.gd`, `project/tests/visual/{zoning,zones_town,bomb}_plan.json`; deleted
`project/game/tools/lot_outlines.gd`; changed the lot cache and book, lot and zone commands, journal and history, road
network, overlay renderer, city state and take, city gen, lot placement and squares, the API (lots, edit, commands, query,
bind), `zone_tool.gd`, `lots_tool.gd`, `lot_proposals.gd`, `preview_line.gd`, `settings.gd`, `roadcraft_hud.gd`,
`zone_strip.gd`, `theme_builder.gd`, `store_api.gd`, `overlays.json` and its schema, `tokens.json`, `drawers.json`, `hud.json`
and their schemas, `tuning.json`, the strings, eight tests and captures, `suites.json`, `log/mapa.md`.

## 2026-11-08 05:35: the bomb, what a blow leaves on the street (English)
**Problems (photos `dist/captures/bomb`, `dist/captures/bomb_street`, before):** a charge on a street dug no crater: the road's
deck stands 0.6 m over the terrain under it, and `strike_at` called any burst more than 0.5 m over the terrain a burst on a
building (which digs none); a crater laid on a road would have lain under the asphalt too (`Marks` read the natural ground), and
so did every piece of debris, body, pool of blood and blot of soot that came down on a street (they all read the terrain). The
crater left no broken ground round it (only the decal); the shock ring went to the collapse reach, not the fireball's; LEAK and
SPARK events existed and were never sent nor drawn; the player's utility poles stood through any blow; a lamp pole knocked out
took its wires away with nothing left hanging; a lighting column, a post box, a phone, a sign or a shrub knocked out was drawn
as a tree (`kind_of` sent every kind it did not name to TREE, written twice, in `strike_ground.cpp` and `ped_furniture.cpp`); the
"ruins close" photo looked at the back of another tower.
**Source read:** the converter's extraction `bomb` (29 files: `actionsWiring.ts` explodeAt, `render/blast.ts`, `playEffects.ts`,
`destruction.ts`, `gore.ts`) and its ledger slice (wrong methods flagged there and not carried: the queue shift, the god
function, the hot allocations, the quadratic proximity).
**Design in this game:** the sim decides and the render draws from events and state. `strike_at` calls a burst on a building only
when a building stands under it (`building_under`). The player's poles in reach go through the journal (one undo step with the
blow), a broken pole's wires pull the next pole over now and then (`strike.pole_pull_*`), and every span left with one end
standing is a WIRE event with that pole's seat line (`pole_stand`, one owner of a pole's frame, now also what
`street_props.cpp` draws from); a lamp row cut does the same from the stations left. The first blow a building takes and stands
shorts it: SPARK or LEAK points along the wall of its blocks nearest the burst (`short_out`). The render: `GroundCells` is the
one owner of the ground the effects lie on (the street's surface, else the terrain, kept by cells per map revision);
`Heave` lays the plates and lip slabs of each crater from its seed, so a save, an undo or a redo lays the same, and only a
blow's own are added; `Ropes` steps the torn wires (Verlet, pinned, slack, on the ground, a live crackling end); arcs flash
the one light blue (`flash_at`, the brighter flash of another colour keeps it); a leak is a weak geyser; poles snap, splinter or
fall turning away from the burst with their arm and lamp thrown; `gone_kind` is one owner of what a knocked piece goes as
(new saved kinds COLUMN, SHRUB, BOX at the end). The tints of what is thrown are data (`effects.json` tints, read by the blast,
the things and the heave); what a blow leaves is its own table, `aftermath.json` (heave, rope, arc, leak, pole).
**Checks:** `bomb_street_plan.json` (new) and `bomb_plan.json` photographed: the crater on the raised road with its plates and
cracks, poles snapped and fallen, the wires hanging from the poles left to the ground, columns falling as metal; the tower's
broken face from its own side. `strike_test` (with `strike_pole_cases.gd`: the crater on the raised road, poles down, wires
and heave drawn, undo and redo by the state hash), `street_works_test`, `save_undo_test`, `edit_fuzz_test`, `shoot_test` pass.
Gates on the changed files: what remains failed at HEAD already (log/pendencias.md). `quick.sh --ready`: 45 of 46 (the gates
of the changed files fail on those findings only). The digest and the frozen photos, held to the branch's base (12e753b1, built
and photographed in a scratch worktree, equal to the main checkout's goldens): the digest moves in `lots` and the fleet only, and
the photos show other buildings on the lots, both from the zoning round's lots that now touch (`lot_place.cpp` WIDTH_TRIM gone);
held to the zoning commit (27aab1be) this round changes two photos by one pixel that also flickers between two runs of the same
commit, and six pixels of a wire's edge by one level of one channel (first put down to float contraction in the new `WireSeats`
caller; the gore round found those six pixels, and 55 of a tree's crown in 02-rua, differing between two runs of the same code:
render noise, log/pendencias.md). The goldens of this worktree were blessed from it. `building_edit_test`
aimed at the middle of a facade, which on a facade of an even number of bays is the line between two (the zoning round's lots
gave its building six): it now aims at the middle of the middle bay (`building_pick_cases.gd`).
The heave's and the wires' first reset wrote their layers before init (three `instance_set_visible` errors at every start, in
the import and test logs; the commit after this round's): a reset now clears their state and the next frame writes it.
**Files:** created `native/src/render/effects/{heave,ropes,ground_cells}.{h,cpp}`, `project/data/render/aftermath.json`,
`tools/gates/schemas/aftermath.schema.json`, `project/tests/core/strike_pole_cases.gd`, `project/tests/visual/bomb_street_plan.json`;
changed the strike commands (buildings, ground, commands), `damage_event.h`, `damage_state.{h,cpp}`, `pole_arms.{h,cpp}`,
`ped_furniture.cpp`, `ped_system.h`, `tuning_fields.h`, the effects (params, world, blast, things, bodies, marks), `prop_kit`,
`street_props.cpp`, `render_world.h`, `city_core_strike.cpp`, `effects.json` and its schema, `tuning.json`, `data_tables.gd`,
`rules.json`, `strike_test.gd`, `building_pick_cases.gd`, `bomb_plan.json`, `suites.json`, `log/mapa.md`.

## 2026-11-08 06:32: the bomb, what a body opened or cut shows (English)
**Problems:** a limb or the head cut off flew as a cylinder tinted by the skin or clothes, not as the person's own; a body opened
spilled nothing (no guts, no bone out of a stump) and the organs were boxes and cylinders of one flesh tint; with the game paused,
the walkers and the vehicles a bomb or a round took stayed drawn where they stood (the poses are published only by a step), the
dead standing beside their bodies; the `walker` step of the look plans aimed the camera at infinity when nobody walked yet (NaN in
every frame, 455 000 warnings, the task killed at its deadline) instead of failing; the damage events had two writers
(`wounds.cpp` pushed onto the list itself).
**Source read:** the converter's `bomb` extraction, `render/gore.ts` (spill, scatter, bone), `render/ragdoll.ts` (detach, sever, STUMP,
openBelly), `render/playEffects.ts` (onGore).
**Design in this game:** a piece cut off is a body of its own (`BodySpec::piece`, `Ragdoll::cut_to`: only the piece's particles keep
their sticks, its hinges go, its frames keep their roll), drawn with the person's own skin; `BodyGore` (render/agents) owns what
hangs off a body, the guts held to the wound by the body's particle (some torn loose after a while) and the bones out of the stumps,
stepped by `BodyRenderer` with the bodies and gone with them; the organs are flattened rounds among the debris (`Debris` gains a
round layer); `TakenAgents` hides what a blow took until a publication newer than the one standing when it was taken
(`DamageEvent::published`, stamped by `emit_damage_event`, now the one writer of the list, in sim/demolition); the gore's look is its
own table, `gore.json` (the organs of a body torn apart too, out of effect_things' constants); the agents.json skin and clothes tints
the stand-in pieces read are no longer read.
**Checks:** `bomb_gore_plan.json` (new: the camera on a walker, the charge at their feet, the bodies where they came to rest; the damage
probe gives the chains, the bones, the bodies and where each lies); `shoot_test`, `strike_test`, `save_undo_test`, `edit_fuzz_test`,
the three bomb plans pass; `quick.sh --ready` 44 of 46: the gates of the changed files fail on findings there before, and the frozen
photos on render noise (two runs of the same code differ in the same pixels, log/pendencias.md).
**Files:** created `native/src/render/agents/body_gore.{h,cpp}`, `native/src/render/agents/taken_agents.h`, `project/data/render/gore.json`,
`tools/gates/schemas/gore.schema.json`, `project/tests/visual/bomb_gore_plan.json`; changed the ragdoll, the body, people and agent
renderers, the debris, the effects (params, world, bodies, things, heave, ropes), `render_world`, `damage_event.h`, `damage_state`,
`strike_commands.cpp`, `strike_internal.h`, `wounds.cpp`, `city_core_strike.cpp`, `effects.json`, `aftermath.json` and their schemas,
`data_tables.gd`, `rules.json`, `look.gd`, `look_aim.gd`, `suites.json`, `log/mapa.md`.

## 2026-11-08 10:40: the bomb brings trees down (English)
**Problem:** no tree fell to a blow but a street tree: the player's planted trees and the woods stood through any charge.
**Design:** a tree falls to the blow that topples a pole (`strike_ground.cpp` `fell_trees`, the same reach): the planted trees in it are
thrown each from where it stood (TREE_FELL, its own height), then `cut_trees_at` (the landscape's own cut, now giving the clearing's
id) takes them through the journal and leaves a clearing that hides the woods' trees there (TREES_FELLED). The render lists those trees
as they stood before that clearing (`PlantedTrees::cleared(x, z, but)`, `felled_trees` in tree_place, `TreeRenderer::felled` over the
near chunks) and throws each at its size: a charred stump, the trunk turning away from the burst and burning, chips, the crown in clumps
(`EffectWorld::fell`, one owner, the street trees through it too; aftermath.json "tree"). `effect_params.cpp` was split by
responsibility (the blast; what it leaves and the gore, `effect_params_aftermath.cpp`; the reader's helpers, `effect_params_read.h`).
**Checks:** `strike_pole_cases.gd` plants a grove by the street: the charge brings the trees in its reach down with a clearing, undo
stands every one again (and the state hash); `bomb_street_plan.json` plants the same grove and shows stumps, burning trunks and leaves
inside the reach and the grove standing past it; `strike_test`, `bomb_street`, `bomb_gore`, `shoot_test`, `edit_fuzz_test`,
`save_undo_test` pass.

## 2026-11-08 11:05: a body knocked down gets up by the simulation's clock (English)
**Problem:** paused, a body knocked down got up and went, and its walker came back only some time after the game ran again.
**Cause:** the body's seconds ran on the effects' clock (`body_renderer.cpp` `update`: `b.left -= dt`, the wall clock), while the
walker stays down by the simulation's (`down_until`, `traffic_system.cpp` leaves it out of the publication): two clocks for one
state; `effects.json` `body_lead_s` covered the gap at normal speed only (a patch, removed).
**Design:** the pose frame carries the simulated second of its publication (`PoseFrame::sim_s`) and every damage event the second
it happened (`DamageEvent::sim_s`, stamped by `emit_damage_event`, its one owner). A knocked body gets up when the simulation stands
its walker again (`BodySpec::up_s` = the event's second + its lie): its seconds left are `up_s` less the newest publication's
second, so paused it lies on, at any speed it gets up with the walker, and it is gone the frame the walker is drawn again. Its fall
stays an effect on the effects' clock, as the debris'.
**Checks:** `shoot_test` `_rises`: 60 frames paused leave the knocked body's seconds unchanged (`damage_probe.bodies_rising_left`;
the old clock lost them every frame), then the ticks of the longest lie bring the walker back and the body goes; `strike_test`,
`edit_fuzz_test` pass.

## 2026-11-08 12:10: a design of its own broken and brought down in its own pieces (English)
**Problem:** a charge on the lot lab's church did nothing; the strongest brought it down only as its crater took it, as a heap of
plain boxes; a design of its own never broke in part.
**Causes, each with its fix:**
- `strike_buildings.cpp` measured a blow to the *lot* (the land round the building: the church's lot is 137 x 125 m, its blocks
  89 x 71 m inside a garden) and took the load at half the lot's depth, so a blow on the church's roof was 62 m away; and the local
  break (the scar) was skipped whenever the whole building felt less than the light pressure. Now a blow is measured to the walls of
  the body's blocks (`nearest_wall`, also the short circuits' owner): how far, the point hit, the depth of the block it reaches; a scar
  is made where the blow breaks a wall even when the building as a whole feels little; `standing_share` takes the blocks' height and
  room (`strike.storey_m`, a storey guessed for every building, goes). Houses set back in their gardens are measured right too.
- `ruin_body.cpp` cut only the facade kit's walls: a design of its own (`pl.b->signature`) had none, the cut came back empty and the
  fall was a plain heap. `SignatureBodies::ruin_faces` (`signature_ruin.cpp`) hands its cooked faces, placed as drawn, each corner
  painted by its look (the colour times the mean of its picture round the corner, from an 8 x 8 copy), the glass apart, what a picture
  cuts (leaves, railings) left out. The cooked file's layout has one owner (`signature_file.h`), read by the drawing and the cut.
- The fracture gives a wall face a back 0.28 m behind it (the kit draws one face to a wall); the church's roof and walls are cooked
  with both sides in one plane, so that back stood out through the roof (grey patches) or, in the plane, fought the twin face. A face
  whose twin is on the same three corners goes to `sided` (no back); a face alone gets a back in its plane
  (`fracture_shell(..., sided)`).
- `RuinSim::begin` held a piece as standing on the ground by the terrain at the lot's middle; a design of its own stands on its lot's
  floor (the highest ground under it plus the plinth) with no foundation down to the ground, so on a slope of half a metre nothing
  was held and the whole church fell to the smallest scar. A piece starting on the floor its building stood on rests on it
  (`RuinMesh::floor`, from the placement, for the kit too).
- A piece was laid to rest 12 s after it was let go wherever it was: thrown up from inside the nave (some 60 m/s at the strongest
  charge, a flight of about 16 s) it slept in the air and others landed on it. The clock now runs from its first touch
  (`RigidPiece::touching`, `Piece::landed`).
- A piece laid to rest raised the heap's map under its whole box by twice its smallest half side: the church's pieces are boxes of
  corners and sloping roof, so each raised ~2.5 m, lifted the pieces still rocking there, which raised it again: a heap 59 m high
  (measured). A piece adds a wall's depth where it lies (`RUIN_WALL_DEPTH_M`, one owner with the fracture's), and the map keeps its
  0.5 m cell for a big building (it covers what 256 cells reach round the middle; a coarser cell was raised whole by any piece).
**Checks:** `strike_landmark_cases.gd` (in `strike_test`): a small charge on the church's roof scars it, it stands drawn in pieces of
its own faces (`effects_ruin_pieces_of` its seed: another broken building may make way for it under the vertex budget), undo gives it
back whole; the strongest charge brings it down in 2519 pieces of its own faces. `bomb_landmark_plan.json`: the church before, broken
at the top with its red roof and white walls, the blast, the heap lying on the ground at 32 s of the effects' clock (hidden runs draw
frames faster than that clock: a photo by frames came before the pieces landed). `strike_test`, `shoot_test`, `model_test`,
`building_edit_test`, `save_undo_test`, `edit_fuzz_test`, `bomb`, `bomb_street`, `bomb_gore` pass; the tower's broken face in `bomb`
looks as before.

## 2026-11-08 12:10: the lane lines of a signalled approach run solid (English)
**Problem:** the lines between the lanes of one way were dashed up to the stop line of a signalled junction.
**Source:** MBST vol. IV (CONTRAN, 2022) 5.2.1: the LMS-1 (solid white line between lanes of one way) is used on the approach of a
signalled junction, 15 to 30 m from the stop line (the PDF read in full in that section).
**Design:** `road_markings.cpp` `lines()` lays a dashed line between lanes of one way solid for `markings.base.signal_solid_m` (30 m,
the most: a red's queue) back from where the lines end, on the side whose traffic arrives at a signalled end (+u arrives at a; a one
way road at the end its lanes run to); the control is the network's (`traffic::control_of`, as the signal posts read it). A
junction's control changed by the player or by undo now draws its arms again (`mark_control`: the signal posts did not refresh
either, since only the lane graph was told).
**Checks:** `signal_approach_plan.json`: two avenues under signals, each arm's arriving lines solid for the last 30 m and dashed
before, the leaving lines dashed; `road_commands_test`, `edit_fuzz_test` pass.

## 2026-11-08 12:20: a clearing takes the gardens' and the parks' trees too (English)
**Problem:** a blow brought the woods and the player's planted trees down, but the trees of a lot's garden and a park's grove stood in
its crater's reach; the player's own cut-trees brush left them standing too.
**Cause:** a clearing (`PlantedTrees::cleared`) was read only by the woods' placement (`tree_place.cpp` `wild_tree`); the parks' groves
and the gardens' plants (`parks`, `garden_place.cpp` `place_garden`) never asked it.
**Design:** one rule: a clearing takes every tree in it. The park's grove skips a tree in a clearing (keeping its count, so the trees
past the clearing stand where they stood) and a garden's trees and shrubs in one are not placed; a new GardenView::FELLED lists only
what a clearing takes (never a hedge or a bed). `felled_trees` takes the woods', the groves' and the gardens' trees as they stood
before the blow's clearing and keeps those it hides now; they are thrown as the woods' are (`EffectWorld::fell`).
`damage_probe.trees_thrown` gives where the last blow's thrown trees stood.
**Checks:** `bomb_street_plan.json` draws a park across the street (draw_building, FN_PARK): after the blow its trees lie thrown and
burning, its paths and hedge stay; in the town's strikes of `strike_test` the gardens' trees in reach are thrown (`trees_thrown` in
its log); `strike_test` (the park held by undo and redo with the rest), `edit_fuzz_test`, `save_undo_test` pass.
