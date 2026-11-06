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

- 17:35 Cantos das junções: medidos no oráculo (`tests/world`, teste descartado). Avenida com boulevard: raio 4,5, recuos 13,65 e 11,65 no nível da guia; rua com rua: 3 e 7,15; 60°: 15,85 e 14,12. A fórmula do `junction.cpp` dá os mesmos valores (o jogo mede no nível do asfalto, 0,15 m a menos, como o nível asfalto do 4.0). A pendência "raio 5 m" estava errada e saiu. Faltava só a cunha comprida: agora é arredondada pelo raio que o recuo pede, como no `corners.ts`, antes de virar chanfro.
- 17:35 Chave de tuning ausente: no caminho do jogo ela é recusada pelo nome e o jogo fica de pé (`boot_refusal_test`). O crash vinha do `grade_probe`, que lia `profiles[0]` num catálogo vazio quando um teste seguia depois do boot recusado. Corrigido.

- 17:50 Inventário e modernização (ordem do usuário):
  - duas sessões consultadas. Roteiro do conversor em `log/conversor.md` ("Roteiro: portar com o conversor"); ferramentas de diagnóstico em `log/causa-raiz.md`. Nada do tradutor foi integrado ao jogo ainda;
  - o relatório do 4.0 "Verificação rápida sem suítes" diz: trocar o oráculo, não só acelerar. Portado o hash de estado (`state_hash`, os bytes do save; dead slots fora) e o `edit_fuzz_test` (3 sementes, ~67 edições, desfazer, refazer, salvar e abrir, 9 s);
  - `tools/build/quick.sh`: um comando, tudo em paralelo; as duas DLLs agora compilam ao mesmo tempo;
  - par do viaduto com o 4.0: a via segue sem aparecer no `look.mjs`, mesmo com ferramenta e PageUp reais; a causa não é o pool de workers (a regra de pista sozinha só vale no desktop). Sem resposta ainda.

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

- **Passo 1.** Problema P10: além de 500 m a baia é retângulo plano; janela sem recuo. O par do viaduto com o 4.0 foi fechado com a foto do arquivo (`reference40.mjs configs/plans/viaduct.json`): tabuleiro, pilares e rampas equivalem.
- **Passo 2.** Krecklau e Kobbelt: a fachada é avaliada por pixel sobre o polígono-base, por ray casting (fontes no ciclo 1).
- **Passo 3.** Depois do ciclo 2, a geometria deixa de ser a resposta nessa faixa: 1 quad por baia ainda pagaria 2,5 execuções por pixel. A profundidade tem de vir do fragmento.
- **Passo 4.** Recuo analítico em `far_window`, sol por `sun_direction`, peitoril e sombra dele; desligado no vidro das baias com geometria (`v_recess`).
- **Previsão:** algumas dezenas de operações por pixel de vidro, sem textura nova: menos de 1 ms em `low3`.
- **Passo 5.** `project.godot`, `daylight.gd`, `building.gdshader`, `building_bay.gdshaderinc`, `building_far_window.gdshaderinc`.
- **Passo 6.** A/B intercalado na mesma execução (`look` passo `global`, `building_recess`), seis janelas, vista z1,5, com a GPU a 100 % pelas outras sessões: desligado 25,85 ms, ligado 25,98 ms. Custo: 0,13 ms. A medida solta de 24 ms e 56 engasgos era carga externa (`nvidia-smi` 100 %) e foi descartada. Fotos lado a lado: 7,6 % dos pixels mudaram, fachadas mais definidas.
- **Passo 7.** Aprovado. Método novo: em máquina dividida, só vale A/B intercalado.

### Ciclo 4 (de perto)

- **Passo 1.** Fotos de perto, oito vistas. P11: gramado do lote chapado, com facetas. Causa: `lot_emit.cpp` desenha cada gramado como um quad com cor de vértice e ruído (`SK_LAWN`).
- **Passo 2.** O 4.0, `buildingMesh.ts` linha 2517: o gramado é o próprio terreno, com o material dele. O jogo tem o mesmo recurso no talude (`verge.gdshader`).
- **Passo 3.** A causa é um segundo desenho de grama. Corrigir a cor seria tratar o efeito.
- **Passo 4.** O gramado vira superfície da malha do chunk com o material `verge`, no perto e no médio; sem draw novo por prédio.
- **Passo 5.** `shell.h`, `lot_emit.cpp`, `building_renderer*`.
- **Passo 6.** Fotos `mid_c` e `lot_a`: grama igual à do terreno, sem facetas. **Passo 7.** Aprovado.
- **P12, achado na foto:** faixas de vaga sobre a grama. Causa: `LotEmit::surface` usa um quad pelos quatro cantos; em declive, superfícies sobrepostas dobram em diagonais diferentes e se cruzam. Correção: em chão não plano, a superfície segue o chão em células de 3 m; em chão plano continua um quad.
  Foto `lot_a` depois: estacionamento limpo, grama não atravessa mais. Aprovado.

### Ciclo 5 (o par com os mesmos prédios)

- **Passo 1.** O nível real: as regras dão `ultra` numa RTX 3060 (com oclusão). Em `ultra`, o bairro `low3` mede GPU 9,2 ms, 0 engasgos, 0 % de carga externa. Fotos: parecia haver asfalto escuro demais, sombra escura demais e cerca solta. Para julgar, um par com o 4.0 sem ele ficar aberto (`reference40.mjs`, ~50 s por plano novo).
  - Asfalto, grama e calçada ao sol, mesma cena: iguais (urbana 52,56,66 contra 54,58,68; grama 73,105,47 contra 76,103,49). Não há defeito. As amostras que indicavam calçada mais clara caíram fora do alvo: recortes lado a lado desmentiram.
  - Parede na sombra, mesmo prédio: igual ao 4.0. O escuro do chão entre prédios não é defeito de luz.
  - `predios-40.json` estava velho (outra cidade): relidos do 4.0 de agora (`predios.js`, 441 documentos). `place_model` funde prédios cujos lotes encostam: **o caixilho escuro e a aleta cinza do par vinham da fusão, não do render**. Cada foto de par precisa de prédios que não se fundam.
  - P13: janelas ~40 % menores que o documento pede. Prédio teal: 0,749 da baia e 0,567 do andar; a foto do jogo mede 0,63 e 0,41.
  - P14: peça na cor da guarnição metálica fica escura até no sol (brise `fin 7`, medido com uma linha temporária; vista `unshaded` 85-105).
- **Passo 2.** Fontes: `buildingMesh.ts` `openingOf` (sem teto para janela dimensionada), `kit.ts` (frame metalness 0,05; fin e brise 0, sem mapa), `architecture.ts` (peitoris 0,8 e 0,85). Godot, Standard Material 3D: quanto mais refletivo, menos a luz difusa e ambiente afeta o material. Regra do usuário (Steam): nada de padrão só brasileiro.
- **Passo 3.** P13: o commit `2414d2ef` (sessão anterior, sem pedido do usuário nos registros) pôs tetos SINAPI em `opening_of` e na tabela. Previsão pela conta: teal 2,00 m de 3,39 (0,59) e verga 2,10 (1,30 m de altura): bate com a foto, então a causa é essa. P14: o jogo passa o acabamento da guarnição às peças; no 4.0 a guarnição só colore, e a peça tem material próprio. A cor sai multiplicada pelo mapa do metal (média 0,61) e com metalness 0,55, que tira difuso: fica escura.
- **Passo 4.** Tirar os tetos da janela dimensionada, voltar a tabela e os peitoris do 4.0, `PART_PAINT` para as peças. Moldura da janela com veneziana passa à cor da guarnição, como `tc` no 4.0. Previsão: mesmo número de triângulos; mais pixels de vidro, GPU sobe pouco.
- **Passo 5.** `facade_bay.cpp`, `facade_bay.h`, `facade_parts.cpp`, `buildings.json`; `capture_pair.gd` ganhou `draw` (vista de depuração por foto).

- **Passo 6.** Uma rodada: `quick.sh --all --bless` 30/30, 0 instáveis, em 56 s; fotos do par sem fusão (`a1`, `a2`). `low3` em `ultra`: GPU 9,15 ms (antes 9,22), 0 engasgos.
- **Passo 7.** Aprovado. Aletas brancas como no 4.0; janela do prédio teal na proporção dele; cidade gerada com janelas mais altas, como o estilo pede.
- **P15, achado, para o próximo ciclo:** cerca do lote é faixa opaca flutuante. Causa em `lot_emit.cpp` `fence`: um painel inventado entre os trilhos; no 4.0 (`emitFence`) cerca é só postes e dois trilhos. É a pendência das "colunas": conferir depois.
- A foto `cmp_b` (prédio bege) não vale de par: o 4.0 mostra um vizinho na frente. Nada se conclui dela.
- **Ordem do usuário (19:5x):** não usar a ferramenta de porte e tradução, incluindo `reference40.mjs` e `call40.mjs`, até a sessão dona resolver. Cobrada por mensagem. Sem ela não há foto nova do 4.0: valem as do arquivo já tiradas e a leitura da fonte.

### Ciclo 6 (cercas)

- **Passo 1.** P15: cerca do lote é faixa opaca flutuando a 0,9 m (`mid_b`, gramados do par). P16: a faixa dos frades briga com o poste (z-fighting, faixa rasgada). P17: cerca e guarda-corpo somem no nível médio.
- **Passo 2.** `buildingMesh.ts` `emitFence`: postes e dois trilhos, nada entre eles; trilhos 4 cm de espessura, 7 e 6 cm de altura; o poste passa 5 cm do topo; postes de cerca só no perto, os de guarda-corpo ficam; trilhos também longe (`lotRail`). `case 'bollard'`: a faixa 1 cm para fora do poste.
- **Passo 3.** P15: o painel foi invenção do porte ("One panel instead of a box"), não otimização de algo que o 4.0 tem. P16: faixa e poste com as mesmas faces. P17: `emit_lot` sai na hora para tudo que não é chão, muro ou sebe no nível médio.
- **Passo 4.** Tirar o painel; medidas do 4.0; faixa 1 cm para fora; cerca e guarda-corpo entram no nível médio pelas caixas dele, sem os postes de cerca. Previsão: menos área opaca perto; caixas a mais no médio, trilho abaixo de um pixel pode cintilar longe: olhar na foto `low3` e medir GPU.
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
- **Fotos** `z15`: fundo e frente combinam. Fundo mais cinza que antes, porque agora mostra a parcela real de vidro; sem par com o 4.0 nessa distância (ferramenta proibida).
- **Método.** `quick.sh --ready`: as duas DLLs e o teste curto, deixa `dist/quick/passed`. O hook aceita essa prova (mudança aplicada pelo usuário).

### Ciclo 9 (nível perto, P6 e P7)

- **Causa, pela leitura do 4.0.** O jogo gravava 4 retângulos de parede por janela, e toda caixa de baia (peitoril, laje, marquise, moldura, brise) ia na malha do chunk, cerca de 20 vértices cada. O 4.0 desenha a parede por fileira (`emitFacadeRuns`, `byRun`) e põe o vidro sobre ela; as caixas são instâncias. E as peças em caixa do jogo não lançavam sombra nenhuma.
- **Correção.** `WallRun`: bays lisas e com vidro embutido viram uma face de até 12 m (a escala do tom, escrito por vértice); o vidro sobe 2 cm da parede. Toda caixa de baia é instância em `parts`. Bandas de caixas, menos a mais fina, lançam sombra por `ShadowTwin` até `lod.cast_props_m`.
- **Fotos.** `res_a`, `mid_a`, `res_c`, `lot_a`: varandas, peitoris e marquises iguais, sem briga de profundidade. `low3`: GPU 9,23 ms (8,93 sem as sombras novas), primitivas 4,81 M (5,00 M). A primeira leitura de 14,4 ms foi carga de outra sessão: repetida com a GPU livre.
- **Falta medir:** a memória do nível perto depois disso.

### Ciclo 10 (os gates de verdade)

- **Causa da lentidão.** `tools/converter-slice` (91 GB, no `.gitignore`) e as outras ferramentas entraram na árvore sem entrar em `exclude_dirs`: o gate anda pelo disco, não pelo git. Sem elas, o gate completo roda em 15 a 26 s. O conserto em `rules.json` é do usuário.
- **O que isso escondia.** 82 problemas reais, de várias sessões. Corrigidos aqui: código duplicado (linha de instância, baias, ids, arredondamento do greide), números mágicos, `build_cpu` longo, estado na interface, mapa. Restam 55.
- **Cena instável.** O nome sempre esteve em `dist/quick/suite.txt` (`UNSTABLE`); `quick.sh` só mostrava a última linha, e meu `grep` não casava. Agora mostra. Na rodada com as 31 cenas: 0 instáveis.
- `quick.sh`: arquivo sem linha no mapa não chama mais todas as cenas. Aquela rodada de 31 cenas foi mais uma contra a ordem do usuário, por `native/src/api` não ter linha.

- **Cidade distante contra o 4.0.** Sem a ferramenta proibida: foto `par1/ref/p1-cidade` do arquivo, mesma câmera, prédios `predios-40.json` da mesma data. Antes: jogo mais claro e azulado (mediana 111,120,124 contra 93,101,99). `MEAN_BOOST` inflava a janela real; sem ele: mediana 101,113,117, p90 174,169,161 contra 171,165,153. Limite: o jogo não tem as ruas nesse par, o chão entra na mediana. As faces na sombra ainda ficam um pouco mais azuis que as do 4.0.

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
- **Causa 1, a lanterna preta:** `part_lamp` punha o corpo de metal na frente do vidro (`p.depth - p.sz / 2` anda para fora, porque a profundidade cresce para dentro), e o vidro ficava dentro da caixa. No 4.0 a caixa inteira é a lâmpada (material `lamp`): agora as cinco faces são `SK_LAMP`.
- **Causa 2, a cor da lâmpada:** `kind == 4` emitia o albedo, então a lâmpada não podia ser clara de dia e quente de noite. O corpo é `#f1ebdd` e a luz é `lamp_light` `#ffd49a` x `lamp_glow` 3,4 (2,4 / luminância da cor, `kit.ts`).
- **Luz na parede:** `emitPorchLight` portado pelo comportamento: superfície `building_wash` do nível perto, aditiva, mesma regra de acesa da lanterna. O 4.0 soma depois do tom, na cor da tela (`#e4d3b3` é `#ffd49a` pelo ACES do three); o jogo soma antes do ACES, e a cor crua dava laranja saturado. Por isso o shader soma `display_to_radiance` dessa cor.
- **Diferença de propósito:** o raio vai até o fim da parede, não até o fim da baia (`wall_before`/`wall_after`); no 4.0 a baia cortava a luz numa elipse estreita.
- Foto de perto às 22h: lanterna acesa, com brilho, e poça suave na parede. Não há foto de perto de lanterna de parede no 4.0 arquivado: a referência foi o código e os balizadores de `35-noite-torre-perto`.
- `gates_user.py` (scratch) ganhou `building_wash` e `lamp_light`; o schema de `materials` ainda depende de o usuário rodar o script.

### Export do jogo (2026-11-05)
- `tools/build/build.sh export` a pedido do usuário: `dist/windows/Roadcraft.exe` do commit `c8a63a4e` mais esta correção.
- **Defeito achado ao abrir o exe:** `strings_ui`, `strings_d` e `strings_e` estavam com o importador `csv_translation`, que tira o CSV do pacote, e `DataTables.load_translations` lê o CSV. Os três agora são `keep`, como os outros. Toda parte nova de `strings*.csv` precisa do mesmo `.import`.
- Aberto no desktop oculto por 600 quadros: 0 erros no log. A tela do exe não foi olhada.

### Ciclo 13: a câmera que puxava e a cidade de longe (2026-11-05)
- **Câmera:** `pull_full_half`/`pull_none_half` estavam em 60/240; no 4.0 são 4,8/19,2 (`pullWeight`), então o olho era puxado na vista normal da cidade. Faltavam duas regras do `isoViewport`: nada de puxão quando o ponto olhado está num prédio, e peso pela profundidade (`PULL_SOFT` 1,5 m). Portadas em `city_core_sight.cpp` (`sight_pull`), com `eye_free`: em qualquer distância o olho não fica dentro de um prédio.
- **Ruas sumindo:** as vias iam só até `road_end_m` (2,2 km da câmera). Novo `FarRoadRenderer`: o mapa inteiro, uma malha por superchunk, só as faces de cima, no mesmo material `road`, dobrada por `chunk_mask` onde o chunk detalhado está de pé (o `networkFarRoads` do 4.0). Sem marcações, talude nem pontes nesse nível.
- **Cidade cortada a 4,5 km:** o alcance dos prédios agora cresce com a altura do olho (`lod.far_per_eye`).
- **Faces pretas:** era buraco. `walls()` do nível distante testava um ponto só, no térreo e no meio do lado, e tirava a parede inteira. `lean_walls` desenha por vão e por andar, juntando o que é igual; o térreo com outra fachada ganha faixa própria.
- **Telhados de longe:** voltaram coroamento, casas de máquinas e o equipamento grande (`emit_roof_plant` com `coarse`), iguais aos de perto.
- Fotos: mapa `km16`, semente 7, vistas de subúrbio e de longe em `ultra`, comparadas antes e depois. A paridade da cor da via na troca e a câmera em movimento não foram olhadas em foto.
- Pendente: o `gates_user.py` (scratch) ganhou `pull_soft_m`, `far_step_m`, `far_pull_m` e `far_per_eye`.

### Ciclo 14: terraço e venezianas (2026-11-05)
- **Terraço sem deck:** `b.cores.empty()`, uma condição que o 4.0 não tem, tirava o terraço de todo prédio com núcleo de escada. Agora vale a regra dele, outro bloco no topo (`built_on`), e o nível médio também desenha.
- `emit_roof_terrace` refeito pelo `emitTerrace`: vasos e caixa d'água redondos (`cylinder`, agora de todos os telhados), espreguiçadeiras na direção e no passo certos, pérgola independente delas, mesa e banquinhos na altura dele; a semente usa o id do bloco (`vol + 1`).
- **Venezianas escuras:** o `FIN_WOOD` escurecia a cor. No 4.0 é tinta lisa (`louvreMaterial`), agora `PART_PAINT`; a terceira cor é `#55785c`.
- Par com `20-dia-apartamento` no prédio 513: mesma cor de veneziana, deck, vasos e espreguiçadeiras. A pérgola difere pelo id do prédio, que aqui é outro.

### Ciclo 15: pontes e pintura de longe, câmera em movimento (2026-11-05)
- **Pontes e pilares:** os chunks com ponte, viaduto ou túnel têm stream própria (`raised_stream_`) com o alcance dos prédios. Foto de um viaduto de 800 m: de perto inteiro; a cerca de 4 km os pilares ainda aparecem.
- **Pintura de longe:** as linhas entre faixas no fragmento de `road.gdshader`, por filtro de caixa da largura do pixel sobre a da linha (uma linha fina em geometria cintila; a pesquisa dá o mesmo caminho: filtrar pelo `fwidth` ou passar à média). A zebra é a média das barras. Sem linha de parada e sem pintura de vagas nesse nível. Ganho discreto nas fotos.
- **Câmera:** `_walk_test` no `camera_input_test`: 90 passos sobre a cidade a 250 m, zoom variou 0,00 m nos dois renderizadores. O teste não foi rodado contra o código antigo.
- Documentação: uniforms por instância até 16 por shader, nos valores por instância, com a nota do Compatibility só sobre índice de textura. `CUSTOM0` ausente na malha: a busca não achou resposta oficial; os testes no Compatibility e no Forward+ passaram com as vias detalhadas sem esse array.
- Brilho do cômodo mais claro: a fórmula do cômodo é a mesma do 4.0 (`glassRoom.ts`, `pane_lamp`); o par não prova diferença, porque o id do prédio muda a sorte de cada cômodo. O limiar do glow não foi mexido.

### Ciclo 16: provas, demolir e o canteiro à noite (2026-11-05)
- **Teste da câmera contra o antigo:** com `pull_full_half`/`pull_none_half` em 60/240 o `_walk_test` falha nas duas checagens (o olho puxado, zoom variando 1,24 m); com 4,8/19,2 passa.
- **Código do Godot:** atributo ausente na malha liga o buffer padrão, zeros em `CUSTOM0` (`mesh_storage.cpp`), então a mesma `road.gdshader` serve aos chunks sem o array. O glow recebe `environment_get_exposure` (`renderer_scene_render_rd.cpp`).
- **Demolir:** o clique leva a rua de cruzamento a cruzamento (`street_of`), um passo de desfazer. O corte em `road_max_seg_m` fica, porque decide por pedaço o que é ponte.
- **Canteiro preto:** o disco da poça estava a 0,164 m (calçada 0,15 + `pool_lift_m` 0,014) e o canteiro tem 0,17 m. `pool_lift_m` 0,03.
- Aberto: entre as poças o canteiro fica mais escuro que no 4.0 (5,12,1 contra 22,26,10); a luz pelas contas é a mesma, causa não provada.

### O zip (2026-11-05)
- `build.sh package` copiava a fonte arquivo por arquivo: quebrava em caminho com acento (`git ls-files` devolve entre aspas) e, corrigido isso, passava de 9 minutos com 16 mil arquivos. Agora é `git archive` da HEAD: 17 s, `dist/Roadcraft.zip` com o exe do commit `700ced28`.

## Próximos passos (em ordem)

1. **Integrar a tradução do 4.0 novo:**
   - rodar a passada de novo com o conversor `9499eac`, que já não cai nos arquivos que só reexportam: `scratchpad/conv-areas.sh <áreas>`, com `--graph out/city40/program.json --analysis out/city40/whole/jelly`;
   - levar ao jogo o que compila, área por área, conferindo com `differential --hand` contra o 4.0;
   - escrever as unidades abertas nos donos pelo método indicado;
   - o registro do conversor está em `log/conversor.md`.
2. **Par de um vão urbano-rural** (a foto ainda não foi tirada).
3. **Elevação:** falta o que está em Pendências no `CLAUDE.md`; o par do viaduto depende de o 4.0 desenhar a via nova no `look.mjs`.
4. **Laboratório F7 e portão `verify`.**
5. **Nomes ainda em português:** as chaves `sementes` do `features.json`, as mensagens dos testes e capturas.

## Regras de convivência (não esquecer)

- **Comandos longos sempre em segundo plano:** o usuário teve de parar o chat porque a sessão travou num comando e não recebia mensagens.
- **Processos:** parar só os próprios, pelo PID. O jogo roda só por `runq`/`hidden_run.ps1`, uma instância por vez.
- **Arquivos do usuário:** gates, hooks, schemas e `.claude/settings*`. Um comando que cite `tools/gates` é recusado pelo hook: use as ferramentas Read/Grep ou um script.
- **Commit:** `git add` e `git commit` separados, com logs de `dist/tests` mais novos que o código. Identidade Coordenadora `<coordenadora@roadcraft.local>`, trailer `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- **Nomes de arquivos, pastas e código em inglês.**
