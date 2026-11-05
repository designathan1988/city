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
- **Referência do 4.0 trocada pelo usuário (2026-11-05):** agora é `C:\Users\Jonathan\Documents\Projects\city\roadcraft` (commit de 29/10). A `roadcraft-4.0-main` (23/10) é antiga e não vale: 345 arquivos de `src` diferem e 144 existem só numa das duas. Exemplo: só a nova tem os postes urbano e rural. O `CLAUDE.md` do worktree já diz isso; o do `main` muda com o próximo commit.
- **Scratchpad desta sessão** (logs e cópias):
  - pasta: `C:\Users\Jonathan\AppData\Local\Temp\claude\26c8cd2f84403c71\6266e3d7-e77a-4554-af1a-13c6b872f74c\scratchpad`;
  - o que há nela: `conv-areas.sh`, `conv-areas.log`, `conv-areas/<área>/conversion`, `converter-snap/`, `pole-before/`, `pole-after/`, `pole-diff/`.

## Feito (commitado e no `origin/main`)

- **Greide das vias** (porte do `elevation.ts`):
  - `native/src/sim/roads/road_grade*` e `road_shape`;
  - os números batem com o 4.0 (`road_grade_test`: morro 0,628608, nó 4,82363, rampas 99/99 e 98/100, tabuleiro 0,6456);
  - regras das placas: junção, ponta sem saída, transição, emenda reta.
- **O raio acerta o que o jogador vê:** `RoadNetwork::ray_deck` (o tabuleiro antes do chão). A câmera usa a mesma regra (`city_camera.gd _ground_height`).
- **Ferramentas:**
  - `tools/build/runq.sh` (fila vigiada, ciclo `fast`), `check_scripts.gd`, `gates_changed.sh`, `photodiff.py`, `rename_english.py`;
  - `project/tests/visual/look.gd`, `look_measure.gd` (o `look.mjs` do 4.0 no jogo).
- **Nomes de pasta em inglês:** `tools/process`, `tools/ports`, `coverage.md`, `constants.json`.

## Feito nesta rodada (commitado no `main` no fechamento)

1. **`structure_spec`:** `StructureSpec structure_spec(Structure, const Tuning &)` em `sim/roads/road_grade.h` é o dono único da folga, da espessura e dos apoios de cada estrutura.
   - O `GradeTune` guarda as quatro estruturas, e saíram `structure_clearance`/`structure_deck`.
   - Chaves novas: `grade.deck_ground_m` 0,22 e `grade.deck_tunnel_m` 0,24 (`tuning_fields.h` e `tuning.json`).
   - Na library do tradutor (`tools/ports/road-elevation/conversion-library.json`): o tipo `RoadStructureSpec`, a função `roadStructure` e o padrão `ROAD_STRUCTURES[$s]`.
   - Testes `road_grade`, `road_commands` e `street_works` passaram.
2. **Poste urbano e rural** (do 4.0 novo: `src/world/utilities.ts` e `src/render/utilities.ts placePole`, CELESC E-313.0002, estruturas B1, N1 e S1):
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
- [x] Foto `rua_04` (`scratchpad/q-rua/shots`): a forma urbana confere com o código do 4.0 novo.
- [ ] Ficam para depois (em "Próximos passos"): o par de perto com uma foto tirada no 4.0 novo, e a foto de um vão urbano-rural.

## "Postes no asfalto": medido, não é defeito

- A `capture_roads` agora imprime os pontos das luminárias das duas avenidas (`_spots`, consulta `furniture_spots`).
  - Avenida vertical (eixo em x = 900): fileira 0 em u = −7,80 (x = 892,2) e fileira 1 em u = +7,80. A horizontal é igual. Todos na calçada.
  - Na vista de cima (`cross_avenue_top`), o pé de cada poste fica na calçada, junto à guia.
- Na vista baixa (`pair_avenue_low`), o topo do poste de 8,5 m se projeta sobre a pista pela perspectiva; daí parecer "no asfalto".
- A pendência saiu do CLAUDE.md.
- **Regra do 4.0 para a linha de uma rua** (`editor/roads/powerLine.ts`):
  - um trecho por segmento, na calçada esquerda, a meia largura da via + 0,4 m;
  - de 4 m depois da boca até 4 m antes da outra; mínimo de 24 m;
  - vãos emendados só em nó de 2 segmentos (um chega, outro sai).
  - O `join_rows` do jogo já segue a regra do nó.

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
  - a troca da referência (o `out/rc40/program.json` e o `out/jelly` dela são da cópia antiga e precisam ser refeitos da nova);
  - `structure_spec` na library;
  - `buildMode.ts` → dono `sim/economy/road_economy.cpp`.
- **Política:** unidades marcadas "metodologia errada" (alocação no laço quente, proximidade quadrática) são escritas no dono pelo método indicado, nunca coladas.

## Depois (pendências maiores, ver também `CLAUDE.md` › Pendências)

- **Elevação:** editor (camada 3), render por estação (tabuleiro, pilares, encontros, guarda-corpos, emboques) e consumidores.
- **Laboratório F7:** lógica pelo tradutor em `native/src/api/lab`, interface à mão.
- **Portão `verify`:** goldens por tick, foto com relógio congelado, recusa de medida com a máquina ocupada.
- **Cantos das junções:** `world/junction/corners.ts`, raio 4,5 contra 5.
- **Chave de tuning ausente derruba o jogo (0xC0000005):** tem que recusar no boot.
- **Portar tudo do 4.0 novo:** construções, pessoas, o resto, pela passada por área do tradutor, integrando o que compila e escrevendo o aberto nos donos.

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
  - o tradutor em `tools/converter`, o processo em `tools/processo`, os portes em `tools/portes`, o oráculo em `tools/oracle-4.0`;
  - `godot-cpp` e DLLs versionados;
  - analisador de causas.

### 2026-11-05 — esta sessão (elevação, ferramentas, tradutor, postes)

- 14:20 `be4f19db` Elevação, camadas 1 e 2:
  - o greide das vias (`road_grade*`), porte do `elevation.ts`, e o chão moldado a elas (`road_shape`).
  - Medido contra o 4.0 por vitest em `tools/oracle-4.0/tests/world/`. Os números só bateram depois de duas correções: a classe certa (`avenue`, não `avenue4Lanes`) e as regras das placas (a ponta sem saída leva só a margem de 2 m; uma junta onde só a estrutura muda é transição).
- 14:26 `a37880e2` Ferramentas: ciclo rápido `runq.sh fast` (6–15 s; eram 3 min), gates só dos arquivos mudados, `photodiff.py`, as medidas do `look` (`look_measure.gd`). CLAUDE.md ganhou a seção "Ferramentas".
- 14:36 `c78fdb32` (sessão de causa-raiz) Ferramentas de análise em inglês e custo do quadro em tempo constante.
- 14:43 `ad200882` Nomes de pasta em inglês:
  - `tools/processo` → `tools/process`, `tools/portes` → `tools/ports` (8673 renomeações, 518 arquivos com referência trocada, por `tools/build/rename_english.py`);
  - as medidas do `look`;
  - a primeira passada do tradutor no porte.
- 14:59 `317ce792` O ponteiro e a câmera pousam no que o jogador vê: `RoadNetwork::ray_deck`, o tabuleiro antes do chão, e a câmera pela mesma regra.
  - O `camera_input_test` ("olho acima do chão") falhava antes de a câmera usar a mesma superfície.
- ~15:05 Passada do tradutor no programa inteiro (`src/main.ts`, 17.485 declarações, 776 arquivos).
  - O usuário teve de parar o chat: a sessão ficou presa esperando e não recebia mensagens.
  - Lição: todo processo longo vai para segundo plano.
- 15:05–15:15 A sessão do conversor corrigiu as lacunas pedidas: `has()`, includes, literais, `reduce`/`startsWith`/regex, cópia em `[...xs].sort()`, tuplas.
  - Pediu a decisão sobre `ROAD_STRUCTURES`.
  - **Decisão:** uma função dona no jogo, `structure_spec(Structure, const Tuning &)` (feito, ver acima).
- 15:09–15:15 A passada inteira foi trocada por uma passada por área: 4 por vez, 12 GB de heap cada, de uma cópia do `HEAD` do conversor.
  - Primeiro problema: a outra sessão deixou um erro de sintaxe momentâneo em `command.ts` (`'port's'`), e as áreas lançadas naquele instante caíram. Daí a cópia do `HEAD`.
  - Segundo problema: `render`, `world` e `editor` caem em arquivos que só reexportam. O script tira o arquivo e repete; o conversor vai aceitar e pular.
- 15:16 Postes, primeira tentativa: as medidas da cópia antiga (8,5 m, cruzeta ±0,85, fios ±0,68, mastro de 12 lados). Testes OK, fotos antes e depois tiradas (`scratchpad/pole-before|after`).
- 15:20 **O usuário corrigiu:** o poste do Roadcraft tem a cruzeta deslocada para o lado da rua, e há dois tipos, rural e urbano.
  - Achado: só a cópia `Documents/Projects/city/roadcraft` (29/10) tem isso; a `roadcraft-4.0-main` (23/10) é antiga.
  - **O usuário decidiu:** a referência passa a ser `city\roadcraft`. CLAUDE.md e `convert.sh` atualizados; a sessão do conversor foi avisada e já refez o grafo dela (`out/city40/program.json`, 863 arquivos, 20.318 unidades; Jelly em `out/city40/whole/jelly`).
- 15:25–15:45 Poste urbano e rural portados (detalhes em "Feito nesta rodada", item 2). Testes ajustados ao 5º argumento (tipo):
  - `street_works_test` ganhou `_rural`: os postes novos de uma linha rural saem rurais, o poste reaproveitado continua urbano, e `street_counts.rural_poles` conta;
  - a ida e volta do save compara todas as contagens, então cobre o tipo.
- 15:40 O usuário pediu este registro em `log/` na raiz, atualizado a cada tarefa e informação nova.
  - A sessão do conversor também mantém o dela: `log/conversor.md`.
- 15:45 A sessão do conversor publicou o runtime C++ dos built-ins do ECMAScript (`cpp/js/js_runtime.h`, 6.449 casos iguais ao Node, `js::Map` na ordem de inserção).
  - A tradução leva o runtime como `native/src/core/js_runtime.h`. Ele ainda tem números nus (`NUMERO_MAGICO` vai pegar), que ela vai nomear.
  - Passada por área relançada sobre a referência nova (`scratchpad/conv-areas40`, log `scratchpad/conv-areas.log`).
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

- 15:31 Passada do tradutor sobre o 4.0 novo (`scratchpad/conv-areas40/<área>/conversion`; `area-top` é o programa inteiro a partir dos arquivos da raiz de `src`). Tudo "in the game 0", porque o porte rodou sem `coverage.md`.
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
- 16:25 `tools/converter` no jogo sincronizado com o master `9499eac`. Passada por área refeita sobre a referência nova: mesmos totais da rodada anterior (por exemplo render 755 de 8.510, world 562 de 8.697), peopleLab ainda cai no `convert`. O tradutor deixa a maioria aberta (`method`), então o porte segue pelos donos, com o livro-razão como guia.
- 16:28 Norma CELESC E-313.0002 baixada e lida nas figuras: Figura 16 (B1 55/70/60 cm, N1 95/35/60 cm) e Figura 43 (15 cm do topo à cruzeta, 50 cm à mão-francesa) confirmam as medidas do poste.
- 16:30 Foto do poste no 4.0 novo (servidor vite em 5300, `scripts/look.mjs`, `press` em "Começar a construir"): a forma confere com a nossa (cruzeta sobre a rua, 3 isoladores, carretéis, luminária abaixo).
- 16:33 Par aéreo no mesmo enquadramento (semente 7): as ruas são as mesmas; as diferenças eram os jardins dos lotes (nenhuma árvore de lote desenhada), arbustos em blocos, canteiros em caixas e a falta de variedade entre vizinhos.
- 16:37–16:46 Jardins dos lotes portados (`render/trees/garden_place`):
  - causa: `tree_place` só lia corpos editados; o mapa 0 tinha 2.646 árvores de lote nunca desenhadas;
  - árvores, arbustos, sebes e canteiros viram plantas, com as medidas do `gardenPlace.ts`; as plantas de cada corpo ficam num cache por `b40::body_key` (o corpo custa <134 µs);
  - espécies novas `hedge`, `shrub_flowering_cream/red`, `flower_bed*`; flor de arbusto é salpico de 30% do lado alto (regra do 4.0; o schema não aceita chave nova, então é constante nomeada em `tree_kit.cpp`);
  - medido: as touceiras do 4.0 (24 modelos por canteiro) triplicaram a GPU, de 6,2 para 21 ms, quase tudo em sombra. Um modelo por trecho de canteiro, sem sombra: 6,8 ms na vista aérea e 9,1 na rua (base 6,2 e 8,2), 60 fps, zero engasgos;
  - `render/buildings/civic.h` (repasse) removido; e2e confere `plants_near`.
- 16:48 DLLs refeitas, gates dos mudados OK, suíte 25/25 sem instáveis.

- 16:57 Variedade dos prédios (`sim/buildings/likeness`): o `record_for` testa 4 sementes em paralelo no `JobPool` e fica com o corpo menos parecido com o que está em 100 m (forma, cor, telhado, andares, pesos do 4.0); `BuildingStore::looks` guarda a aparência por `body_key`. `city_mass_test`: 1 de 1.350 pares de vizinhos parecidos (sonda `look_twins`), cidade abre em 0,14 s. Suíte 25/25, gates dos mudados OK.

- 17:30 Elevação, editor e estruturas (ainda sem commit no início desta linha):
  - faixa escura gigante: causa confirmada em foto. O `wire_min_px` do fio engordava sob a projeção do sol; no passe de sombra o fio fica em repouso e na largura real (`IN_SHADOW_PASS`);
  - tabuleiro: só projetava uma linha fina, porque a pista não lança sombra e o sol vê a face de baixo pelas costas. O tabuleiro ganhou a face de cima, 4 cm sob a pista; a sombra larga apareceu;
  - editor: PageUp/PageDown e a linha Altura (um metro por vez, pousa em metro inteiro), a via sai da altura em que foi começada, a prévia sobe a rampa, o ponteiro segura a via na altura, o encaixe só pega o que está na mesma altura (`anchorForHeight`), a linha Corte (talude ou muro);
  - `ToolController._unhandled_key_input` vem antes de todo `_unhandled_input`: é assim que a tecla da ferramenta não chega à câmera (`keeps_key`);
  - muros de arrimo (`road_walls.cpp`) com aterro, e o talude agora é por lado;
  - só a estrutura túnel poupa o prédio de cima (`touchesRoad`);
  - `road_height_cases.gd` e as teclas no `camera_input_test`: passam.
- 17:30 Par com o 4.0: **não foi possível fotografar a via nova.** Pelo `look.mjs`, na cópia `city\roadcraft`, a via entra no documento e os carros andam nela, mas nada é desenhado (cidade e mapa vazio; 25 s de espera). O console diz `[roads] a road tile worker has been silent ... still waiting`. Causa não investigada a fundo: parei por custo. Nesse mapa vazio só a primeira via foi feita, porque o Escape do plano fecha a ferramenta. As medidas das estruturas vêm do `render/structures.ts`, não de foto.

## Próximos passos (em ordem)

1. **Integrar a tradução do 4.0 novo:**
   - rodar a passada de novo com o conversor `9499eac`, que já não cai nos arquivos que só reexportam: `scratchpad/conv-areas.sh <áreas>`, com `--graph out/city40/program.json --analysis out/city40/whole/jelly`;
   - levar ao jogo o que compila, área por área, conferindo com `differential --hand` contra o 4.0;
   - escrever as unidades abertas nos donos pelo método indicado;
   - o registro do conversor está em `log/conversor.md`.
2. **Par de um vão urbano-rural** (a foto ainda não foi tirada).
3. **Elevação:** falta o que está em Pendências no `CLAUDE.md`; o par do viaduto depende de o 4.0 desenhar a via nova no `look.mjs`.
4. **Laboratório F7, portão `verify`, cantos das junções** (`corners.ts`, raio 4,5) e a chave de tuning ausente que derruba o jogo.
5. **Nomes ainda em português:** as chaves `sementes` do `features.json`, as mensagens dos testes e capturas.

## Regras de convivência (não esquecer)

- **Comandos longos sempre em segundo plano:** o usuário teve de parar o chat porque a sessão travou num comando e não recebia mensagens.
- **Processos:** parar só os próprios, pelo PID. O jogo roda só por `runq`/`hidden_run.ps1`, uma instância por vez.
- **Arquivos do usuário:** gates, hooks, schemas e `.claude/settings*`. Um comando que cite `tools/gates` é recusado pelo hook: use as ferramentas Read/Grep ou um script.
- **Commit:** `git add` e `git commit` separados, com logs de `dist/tests` mais novos que o código. Identidade Coordenadora `<coordenadora@roadcraft.local>`, trailer `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- **Nomes de arquivos, pastas e código em inglês.**
