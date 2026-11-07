# Registro da linha de arquitetura (tools/analysis/arch)

Ao retomar: leia este arquivo e rode `python tools/analysis/arch.py status`. Metodologia: os 7 passos do `CLAUDE.md`, registrados abaixo.
Item fechado só com prova (saída, diff ou arquivo e linha) na seção "Provas". `[ ]` aberto, `[x]` feito.
Contagem: `grep -c '^- \[x\]' log/arquitetura.md` e `grep -c '^- \[ \]' log/arquitetura.md`.

Meta da sessão (o `/goal` do usuário, 2026-11-06): só está cumprido quando `python tools/analysis/arch.py selfcheck`, rodado no turno e sem
cache, mostrar todas as verificações passando; `status` mostrar a tabela de cobertura sem categoria não coberta e o resíduo com seus itens;
o gate completo mostrar Gates OK; a tag `arch-baseline` existir; e o push estiver feito. Nenhuma correção de causa nesta fase.

Correções de leitura feitas ao gravar (2026-11-06):
- `dist/arch-proto/meta.py` tem 115 linhas; o `".import"` fixo está em `meta.py:75`. O `meta.py:681` da confirmação era a linha na saída concatenada dos protótipos: erro meu.
- A resposta R1 diz que o `build.sh` não usa `git archive`. Conferido: usa, em `tools/build/build.sh:72` (`build.sh package`). Não muda a decisão de R1 (sem `.gitattributes`), e está avisado ao usuário.

## Parte a: o PEDIDO, com as palavras do usuário

- [ ] **A0.** A causa dos problemas de desempenho deste jogo é a arquitetura. Sombra, polígono e tempo de GPU são sintomas e mudam com a placa de vídeo. Crie o mecanismo que rastreia tudo, sem amarra nem limitação, e encontra na causa os problemas de arquitetura, otimização e desempenho.
- [ ] **A1. O PROGRAMA.** tools/analysis/arch/ com entrada python tools/analysis/arch.py e comandos scan, explain <id>, next, predict <id>, done <id>, status e selfcheck. Recebe só o caminho de um repositório git. Não contém nome de funcionalidade, pasta, arquivo, classe, função ou API de nenhum projeto, nem lista de decisões, fluxo descrito à mão ou limite numérico escolhido à mão. Corte, peso e resolução saem do repositório analisado (distribuição do corpus ou previsão do histórico). Adaptador é por linguagem ou formato, nunca por projeto. features.cjs é exemplo do que não pode: regex com nomes de pasta. Se um resultado pedir um nome no código, o método está errado: troque o método.
- [ ] **A2. UNIDADE E EVIDÊNCIA.** A unidade não é o arquivo. É declaração, campo, ramo de função, chave de JSON, linha de CSV, função e uniform de shader, nó de cena. Uma funcionalidade pode ser um ramo dentro de uma função compartilhada. Toda aresta é tipada e traz origem da evidência e grau de confiança. Fontes: AST do compilador, tipos, servidores de linguagem, identificadores raros, commits, execução gravada. Pesquise o melhor para cada linguagem. Duas fontes independentes que concordam valem mais que uma.
- [ ] **A3. RELAÇÃO DE POSSIBILIDADES.** Cubra todas as classes abaixo e amplie (a lista inteira está na parte b, item a item):
  Chamada: direta, virtual, ponteiro de função e de membro, std::function, lambda guardada, template instanciado, macro e X-macro expandidas, operador, construtor e destrutor (RAII, travas), conversão implícita, herança, sobrecarga, inline em header.
  Por nome em tempo de execução: despacho por string (dispatch, query, set_tool, tabelas constantes), Callable, bind, call, callv, call_deferred, has_method, get e set por nome, Object::call do C++ ao script, ClassDB::bind, match e switch sobre enum, valor de dado que escolhe código.
  Eventos: sinais, conexões em código e em cena, timers, callbacks do motor (extension_api traz is_virtual), entrada, autoloads, grupos, caminhos de nó.
  Recursos: preload, load com caminho montado, cena, script, shader, include de shader, material por nome, uniform e global e instance uniform escritos por nome, chave de JSON, Dictionary lido por texto, chave de tr() montada, tabela em partes, chave de save e bloco etiquetado.
  Dados e mutação: quem lê e quem escreve cada campo, com sensibilidade a campo; mutação por referência, ponteiro, alias, const_cast; estado compartilhado entre threads; atômicos, travas, buffers triplos; canais indiretos em que A escreve e B lê depois (ChangeLog, diário, undo e seu inverso, save e load); função como valor que muda em tempo de execução.
  Tempo: boot, por quadro, por tick, por evento, adiado, em thread ou job, ao carregar save antigo, só em teste, só em debug, só em exportação, só num renderizador ou nível gráfico, só por dado ou opção, só por tecla ou argumento de linha de comando.
  Fronteira: motor, Terrain3D, bibliotecas, processos externos, ferramentas que geram arquivos (tools/assets), código gerado pelo tradutor, build, export_presets, PGO.
  Papel: funcionalidade própria, dependência de outras, dependente de outras, compartilhada, infraestrutura, código de teste, código morto, dado sem leitor, opção sem efeito.
  Evolução: a mesma funcionalidade ao longo dos commits, por renames, movimentos e divisões, com identidade estável.
  Se achar classe nova, ela entra. A relação nunca é considerada fechada por decisão sua: é a seção 5 que mede o que falta.
- [ ] **A4. SEM EXPLOSÃO.** Seguir todas as arestas alcançou 722 de 726 arquivos no 4.0: rastreio que devolve o programa inteiro reprova. Use dominância, sensibilidade a campo e fecho em camadas: núcleo (só ela usa), executado, dados (quem escreve o que ela lê e quem lê o que ela escreve), ligação (da raiz até ela) e alcançável (só listado). Aresta genérica por identificador nunca mantém um alvo vivo sozinha. Pesquise fatiamento de programas, inclusive thin slicing.
- [ ] **A5. TUDO É CONTADO.** Uma enumeração independente da que resolve lista todo ponto de ligação de cada arquivo. Cada um termina resolvido, com candidatos e confiança, ou no resíduo. O resíduo é um número em status e cada item tem arquivo e linha. O que a análise estática não decidir não some: fica com confiança e a IA lê o código.
- [ ] **A6. EXECUÇÃO GRAVADA, SEM RODAR.** native/pgo/citycore.profdata tem contagens e alvos de chamada indireta: use para resolver chamadas indiretas, achar o que é quente e medir o que a análise estática perdeu. Cuidado: nomes internos carregam o caminho de uma worktree antiga, o perfil é anterior a commits recentes e "não rodou no treino" não significa morto. Se o lado GDScript não fechar, use um registrador leve na suíte existente, cerca de um minuto, nunca sessão longa.
- [ ] **A7.1.** O QUE DESCOBRE. Tudo a partir das seções acima: funcionalidades, em níveis, nomeadas pelas próprias palavras, atravessando linguagens e dados.
- [ ] **A7.2.** espalhamento: mesma responsabilidade em vários lugares da mesma camada. Camada é descoberta pelo grafo condensado, nunca por nome de pasta. Passar por camadas é a arquitetura, não é defeito.
- [ ] **A7.3.** deuses por responsabilidade, nunca por linhas: alcance, participação, grupos sem estado comum, fan-in, fan-out, rotatividade, inclusive em arquivo de dados. Coesão por campos sozinha falha, porque um campo comum faz fachada parecer coesa.
- [ ] **A7.4.** dono que falta: classes respondendo às mesmas mensagens sem contrato comum, regra repetida, decisão tomada em vários lugares.
- [ ] **A7.5.** custos, por análise estática, a partir do que repete (quadro, tick, thread): trabalho que cresce com o estado, laços aninhados, alocação, trava e contenção, chamada à fronteira em laço, trabalho refeito sem mudança de entrada, cópia, busca linear em caminho quente, consulta por quadro, GPU (o que é enviado, passes, sombra, material, variantes, leitura de tela, custo de shader por análise offline sobre SPIR-V), memória, streaming, carregamento, configuração, opção cara ou sem efeito. Monte a tabela completa de categorias pela pesquisa. Categoria sem detector aparece como não coberta e impede concluir.
- [ ] **A7.6.** raiz: achados formam um grafo. O achado explicado por outro vira filho dele. A causa é o nó que, removido, faz os outros sumirem. Só a raiz entra na fila. Todo achado traz o caminho de prova no grafo; achado sem caminho não existe.
- [ ] **A8. FILA.** dist/arch/queue.json, regenerável pelo scan. Cada unidade: a causa, o caminho de prova, todos os lugares (arquivo e linha), o dono candidato (dominador comum), o que deve sumir, os filhos que somem junto, quantos commits passados ela custou. Ordem: alcance causal vezes custo histórico. Esforço não é dado e não entra em nenhuma conta.
- [ ] **A9.0. SELFCHECK.** Cada verificação tem de poder reprovar, e cada uma tem um teste mostrando a reprovação. Conjunto vazio é reprovação.
- [ ] **A9.1.** Invariância: trocar todos os nomes e caminhos dá mapa, fila e raízes idênticos.
- [ ] **A9.2.** Anti-maquiagem: dividir um arquivo sem separar responsabilidade, renomear, mover código ou criar função que só repassa não muda nenhuma nota.
- [ ] **A9.3.** Injeção: numa cópia, com semente sorteada, injete casos de cada classe da seção 3 e de cada categoria de custo: funcionalidade nova, deus, regra repetida, chamada só por nome, trabalho por quadro. Todos têm de ser achados.
- [ ] **A9.4.** Ablação: desligar qualquer adaptador reprova um teste. A aresta genérica não pode esconder a falha.
- [ ] **A9.5.** Estabilidade: tirar uma funcionalidade ou alguns commits mantém o resto. O protótipo deu 0,85 a 0,88: ache a causa e corrija.
- [ ] **A9.6.** Previsão: prevê commits seguintes melhor que a pasta e o acaso.
- [ ] **A9.7.** Portabilidade: roda sem edição em C:\Users\Jonathan\Documents\Projects\city\roadcraft e em tools/converter, e o resultado faz sentido em cada um.
- [ ] **A9.8.** Limpeza: nenhum identificador raro do repositório analisado existe no código do programa, e o programa não abre nenhum arquivo de gabarito.
- [ ] **A9.9.** Determinismo: duas rodadas em processos separados dão bytes iguais.
- [ ] **A10. GABARITO DO PROJETO.** Use o projeto como referência sem pôr nada dele no programa. Os gabaritos ficam em tools/analysis/arch_eval/, que só o selfcheck lê: log/mapa.md (arquivos de mesma responsabilidade), cpp_layers de rules.json (camadas), owners.json (donos), tools/process/records/map40 (funcionalidades do 4.0), os defeitos com causa registrada em log/*.md e nos commits (rode o programa no commit anterior à correção: ele deve apontar o lugar). Separe uma parte para ajustar e outra guardada para a nota final, para não decorar a prova. O protótipo deu 0,69 de pares do mapa contra 0,12 ao acaso. Baixa concordância é investigada: pode ser erro do programa ou defeito real do mapa. Diga qual, com prova.
- [ ] **A11.1.** CENÁRIOS JÁ PREVISTOS. compile_commands não está nas pastas de build: use ninja -t compdb em native/build-win-debug e filtre native/src. O índice do clangd leva cerca de 1 minuto; percorrer token por token é o lento: troque o método. tools/converter/out/game/cppgraph.json é velho, com 202 arquivos.
- [ ] **A11.2.** Git Bash reescreve argumentos que parecem caminho e a saída pode vir com fim de linha do Windows.
- [ ] **A11.3.** Reconheça sozinho arquivos gerados, vendorizados, cópias (tools/converter-slice, tools/oracle-4.0) e arquivos sem vocabulário próprio (.import, .uid).
- [ ] **A11.4.** Commits gigantes e renames distorcem o histórico: trate pelo comportamento, sem número fixo.
- [ ] **A11.5.** tools/converter tem git próprio: mudança nele vai no repositório dele.
- [ ] **A11.6.** Limites de tamanho, cabeçalho e clones valem para o programa. Arquivo novo entra em log/mapa.md.
- [ ] **A11.7.** Pode instalar dependências de desenvolvimento se a pesquisa mostrar que são o melhor caminho, e registre.
- [ ] **A12. FALHAS DO GATE.** Corrija as 15 falhas do gate completo (schemas e rules.json), alinhando com o que o jogo lê. Tem minha autorização para alterar tools/gates e os schemas.
- [ ] **A13. COMO TRABALHAR.** Siga os 7 passos do CLAUDE.md em log/arquitetura.md. Sem tentativa e erro: leia e pesquise antes. Sem medições longas e sem abrir o jogo para investigar. Não mexa em mudanças de outra sessão. O estado fica em disco, para continuar depois de uma compactação: ao retomar, leia log/arquitetura.md e rode status.
- [ ] **A14. ENTREGA DESTA FASE.** selfcheck passando, regeneração completa do grafo C++, tabela de cobertura, resíduo, as 20 primeiras raízes com caminho de prova. Commit e push. Crie a tag arch-baseline no commit da ferramenta e dos gabaritos. Pare aí: não corrija nenhuma causa nesta fase.

## Parte b: cada classe da seção 3, com o exemplo real (cada uma coberta por adaptador e injeção)

Chamada
- [ ] **B1.** direta: `native/src/sim/simulation/simulation.cpp:101`
- [ ] **B2.** virtual: `native/src/render/mesh_queue.h:76`
- [ ] **B3.** ponteiro de função: `native/src/sim/traffic/reeds_shepp.cpp:24`
- [ ] **B4.** ponteiro de membro: `native/src/render/chunk_jobs.h:105`
- [ ] **B5.** std::function: `native/src/core/job_pool.cpp:31`
- [ ] **B6.** lambda guardada: `native/src/render/buildings/building_renderer.cpp:198`, `native/src/render/buildings/facade.h:29`
- [ ] **B7.** template instanciado: `native/src/sim/traffic/astar.h:28`
- [ ] **B8.** macro e X-macro: `native/src/api/catalog_loader.cpp:59`
- [ ] **B9.** operador: `native/src/core/math.h:25`
- [ ] **B10.** construtor e destrutor (RAII): `native/src/sim/terrain/heightfield.h:157`
- [ ] **B11.** trava por RAII: `native/src/api/city_core_query.cpp:118`
- [ ] **B12.** conversão implícita: `native/src/api/city_core_query.cpp:132`
- [ ] **B13.** herança: `native/src/api/city_core.h:29`
- [ ] **B14.** sobrecarga: `native/src/sim/roads/road_network.h:137`
- [ ] **B15.** inline em header: `native/src/core/grid.h:20`

Por nome em tempo de execução
- [ ] **B16.** dispatch: `project/game/flow/game_keys.gd:97` → `project/game/store/store_api.gd:8` → `project/game/store/city_store.gd:83`
- [ ] **B17.** query: `project/game/camera/camera_pull.gd:29` → `project/game/store/store_api.gd:47` → `project/game/store/city_store.gd:93`
- [ ] **B18.** set_tool: `project/ui/roadcraft_hud.gd:226`
- [ ] **B19.** tabela constante: `project/game/tools/tool_controller.gd:26`, `project/game/flow/game_flow.gd:11`
- [ ] **B20.** Callable: `project/game/store/city_store.gd:44`
- [ ] **B21.** bind: `project/ui/effects_panel.gd:158`
- [ ] **B22.** call: `project/game/health/health_log.gd:35`
- [ ] **B23.** callv: `project/game/store/city_store.gd:83`
- [ ] **B24.** call_deferred: `project/render/model_studio.gd:65`
- [ ] **B25.** has_method: `project/ui/builder_strip.gd:32`
- [ ] **B26.** get e set por nome: `project/game/settings/quality.gd:69`
- [ ] **B27.** Object::call do C++: `native/src/render/terrain_bridge.cpp:36`
- [ ] **B28.** ClassDB::bind: `native/src/api/city_core_bind.cpp:17`, `native/src/api/city_core_models.cpp:210`
- [ ] **B29.** match e switch: `project/game/flow/game_keys.gd:72`, `native/src/render/buildings/facade_bay.cpp:46`
- [ ] **B30.** dado que escolhe código: `project/data/ui/land_panels.json:140`, `project/game/tools/body_edits.gd:11`

Eventos
- [ ] **B31.** sinal: `project/game/flow/game_flow.gd:7`
- [ ] **B32.** conexão em código: `project/game/lab/lab_recorder.gd:32`
- [ ] **B33.** conexão em cena: nenhuma no projeto; a única é vendorizada, `native/godot-cpp/test/project/main.tscn:29` (prova por injeção, P1)
- [ ] **B34.** timer: `project/render/thunder.gd:62`
- [ ] **B35.** callback do motor: `project/game/camera/city_camera.gd:99`, `native/src/api/city_core.h:58`
- [ ] **B36.** entrada: `project/game/tools/tool_controller.gd:144`
- [ ] **B37.** autoload: `project/project.godot:12`
- [ ] **B38.** grupo: `project/game/tools/tool_controller.gd:68`
- [ ] **B39.** caminho de nó: `project/ui/model_shelf.gd:77`

Recursos
- [ ] **B40.** preload: `project/game/tools/tool_controller.gd:26`
- [ ] **B41.** load com caminho montado: `native/src/render/agents/people_renderer.cpp:109`, `project/render/city_water.gd:30`
- [ ] **B42.** cena: `project/tests/visual/capture_clock.gd:25`
- [ ] **B43.** script: `project/game/play.tscn:3`
- [ ] **B44.** shader: `project/render/city_water.gd:6`, `native/src/render/buildings/signature_bodies.cpp:27`
- [ ] **B45.** include de shader: `project/render/shaders/building.gdshader:3`
- [ ] **B46.** material por nome: `native/src/render/agents/agent_renderer.cpp:42`
- [ ] **B47.** uniform por nome: `native/src/render/agents/agent_looks.cpp:53`
- [ ] **B48.** global uniform: `project/render/shaders/agents.gdshader:12`, `project/project.godot:32`
- [ ] **B49.** instance uniform: `project/render/shaders/building_level.gdshaderinc:11`
- [ ] **B50.** chave de JSON: `project/data/catalog/tuning.json:46` → `native/src/render/buildings/building_renderer_reach.cpp:42`
- [ ] **B51.** Dictionary lido por texto: `native/src/api/traffic_loader.cpp:38`
- [ ] **B52.** chave de tr() montada: `project/game/tools/build_hands.gd:76`, `project/ui/approach_panel.gd:19`
- [ ] **B53.** tabela em partes: `project/game/data/data_tables.gd:19`
- [ ] **B54.** chave de save: `project/game/settings/settings.gd:41`
- [ ] **B55.** bloco etiquetado: `native/src/sim/persistence/save_blocks.cpp:28`

Dados e mutação
- [ ] **B56.** quem escreve e quem lê um campo: `native/src/sim/citygen/city_gen.cpp:84`, `native/src/render/buildings/building_renderer_reach.cpp:42`
- [ ] **B57.** por referência: `native/src/sim/commands/commands.h:117`
- [ ] **B58.** por ponteiro: `native/src/render/chunk_jobs.h:107`
- [ ] **B59.** alias: `native/src/sim/traffic/ped_furniture.cpp:98`
- [ ] **B60.** const_cast: `native/src/sim/lots/lot_book.cpp:76`, `native/src/sim/roads/flow_stats_query.cpp:45`
- [ ] **B61.** estado entre threads e atômicos: `native/src/sim/state/city_state.h:71`
- [ ] **B62.** travas: `native/src/sim/state/city_state.h:89`
- [ ] **B63.** buffer triplo: `native/src/sim/state/pose_buffer.h:113`
- [ ] **B64.** ChangeLog: `native/src/sim/state/change_log.h:17`
- [ ] **B65.** diário: `native/src/sim/commands/edit_journal.h:126`
- [ ] **B66.** undo e inverso: `native/src/sim/commands/edit_history.cpp:2`
- [ ] **B67.** save e load: `native/src/sim/persistence/save_format.h:32`
- [ ] **B68.** função como valor que muda: `project/game/store/city_store.gd:34`, `project/game/tools/tool_controller.gd:91`

Tempo
- [ ] **B69.** boot: `project/game/play.gd:16`
- [ ] **B70.** por quadro: `project/game/store/city_store.gd:131`, `native/src/render/render_world.cpp:178`
- [ ] **B71.** por tick: `native/src/sim/simulation/simulation.cpp:64`
- [ ] **B72.** por evento: `project/game/tools/tool_controller.gd:144`
- [ ] **B73.** adiado: `project/ui/effects_panel.gd:43`
- [ ] **B74.** em thread: `native/src/api/city_core_persist.cpp:79`
- [ ] **B75.** em job: `native/src/sim/lots/lot_cache.cpp:90`
- [ ] **B76.** save antigo: `native/src/sim/persistence/body_codec.cpp:164`
- [ ] **B77.** só em teste: `project/game/flow/game_flow.gd:22`
- [ ] **B78.** só em debug: `native/src/sim/traffic/lane_graph_edit.cpp:213`, `native/CMakeLists.txt:104`
- [ ] **B79.** só em exportação: `project/export_presets.cfg:10`, `project/data/i18n/strings_ui.csv.import:3`
- [ ] **B80.** só num renderizador: `project/render/shaders/atmosphere.gdshader:188`
- [ ] **B81.** só num nível gráfico: `project/game/settings/quality.gd:119`
- [ ] **B82.** por dado ou opção: `project/game/settings/quality.gd:117`
- [ ] **B83.** por tecla: `project/data/ui/keys.json:27`
- [ ] **B84.** por argumento: `project/game/play.gd:38`

Fronteira
- [ ] **B85.** motor: `native/src/render/rs_util.h:65`
- [ ] **B86.** Terrain3D: `native/src/render/terrain_bridge.cpp:43`
- [ ] **B87.** bibliotecas: `native/src/core/polygon_ops.cpp:6`, `native/src/core/memory.cpp:4`, `native/CMakeLists.txt:40`
- [ ] **B88.** processo externo: `tools/analysis/sampler.py:226`
- [ ] **B89.** ferramenta que gera arquivo: `tools/assets/make_pole_parts.py:13`
- [ ] **B90.** código gerado pelo tradutor: `native/src/sim/roads/flow_stats.h:2`
- [ ] **B91.** build: `native/CMakeLists.txt:16`
- [ ] **B92.** export_presets: `project/export_presets.cfg:9`
- [ ] **B93.** PGO: `tools/build/pgo.sh:50`, `native/CMakeLists.txt:28`

Papel
- [ ] **B94.** própria: `native/src/render/roads/road_stays.cpp:1`
- [ ] **B95.** dependência de outras (as outras usam ela, R8): `native/src/sim/roads/road_network.h:137`
- [ ] **B96.** dependente de outras (ela usa as outras, R8): `native/src/sim/traffic/ped_crossing.cpp:54`
- [ ] **B97.** compartilhada: `native/src/sim/traffic/day_table.h:1`
- [ ] **B98.** infraestrutura: `native/src/core/job_pool.cpp:31`
- [ ] **B99.** código de teste: `project/game/store/store_api.gd:67`
- [ ] **B100.** código morto (candidato): `native/src/api/city_core_body.cpp:227`, `project/game/store/store_api.gd:34`
- [ ] **B101.** dado sem leitor: `project/data/render/streets.json:318`; carregado sem uso: `project/data/render/buildings.json:130`, `native/src/api/buildings_loader.cpp:189`
- [ ] **B102.** opção sem efeito (candidato): `project/data/render/streets.json:321`, `log/pendencias.md:40`

Evolução
- [ ] **B103.** rename: `tools/analise/causas.py` → `tools/analysis/causes.py` em `c78fdb32`
- [ ] **B104.** movimento: `sim/lots/lot_land.cpp` → `native/src/sim/citygen/lot_land.cpp` em `c4f6848b`
- [ ] **B105.** divisão: `project/game/camera/camera_aim.gd` de `city_camera.gd` em `32e0bb55`; `native/src/render/instance_rows.h` de `rs_util.h` em `013e2bee`

## Parte c: cenários além dos listados (P2: fazem parte do pedido)

Gabarito e contaminação
- [ ] **C1.** Os originais do gabarito continuam dentro do repositório analisado (`log/mapa.md`, `rules.json`, `owners.json`, `records/map40`). A prova precisa rodar também sem esses arquivos e dar a mesma nota.
- [ ] **C2.** O programa e `arch_eval` ficam dentro do repositório que ele analisa: rodar nele mesmo muda o corpus, e a Limpeza precisa saber o que é "ele".
- [ ] **C3.** Toda mudança entra no mapa e no log no mesmo commit: essa regra fabrica mudança conjunta. As DLLs versionadas fazem o mesmo em todo commit de C++.
- [ ] **C4.** `features.json` nasceu do grafo antigo, 16.771 unidades contra 20.318 (`log/conversor.md:95`), e as sementes dele são por pasta: usá-lo como gabarito premia o método proibido.
- [ ] **C5.** O gabarito tem defeitos próprios, e a seção 10 pede dizer qual lado errou: `tools/gates/owners.json:46` usa `"\b"` com uma só barra (backspace em JSON, a regra de `street_furniture` nunca casa); `rules.json` e `owners.json` citam `sim/zones`, que o mapa não lista (a conferir).

Histórico
- [ ] **C6.** Histórico curto: 647 commits em 3 dias, 80 merges, mediana 8 arquivos, p90 33, máximo 8.713; o 4.0 tem 2.788 e o conversor 74. A previsão pede teste de significância, e sessões em paralelo misturam temas no mesmo commit.
- [ ] **C7.** O autor não informa nada: quase tudo vem de duas identidades de sessão.
- [ ] **C8.** O produto "alcance × custo histórico" zera a causa recente, porque código novo não tem custo passado e sumiria da fila. A suavização tem de sair do corpus.
- [ ] **C9.** Custo histórico sem lista de palavras: as mensagens misturam português e inglês, então "correção" tem de ser definida por comportamento.
- [ ] **C10.** Rodar em commit anterior à correção: não há base de compilação nem `godot-cpp` gerado naquele estado, e as bibliotecas vêm por `FetchContent`.

C++
- [ ] **C11.** O clangd vê uma só configuração. `DEBUG_ENABLED`, release e Linux dão grafos diferentes: o certo é a união com a origem marcada.
- [ ] **C12.** A X-macro cola nomes: `render` + `building_bay_end_m` vira `render_building_bay_end_m`, que não existe como texto. `#name` em `native/src/api/city_core_nature.cpp:44` liga campo a chave de dado.
- [ ] **C13.** Header sem unidade de compilação própria e template instanciado só em alguns lugares precisam do grafo pós-instanciação, não do texto.
- [ ] **C14.** Classe repartida: `CityCore` e as áreas ocupam dezenas de `.cpp`. Fachada, deus repartido e arquivo são três perguntas diferentes.
- [ ] **C15.** Trabalho limitado por orçamento (`commit_budget_ms`, `lorry_plan_work`) ou guardado por revisão parece laço crescente e não é. Sem ver a guarda, o detector dá falso positivo, e `bounded:` em comentário é dica à mão, que não se pode usar.
- [ ] **C16.** Closure de job captura `this` e `&s`: aresta de thread e de tempo de vida.

Por nome e GDScript
- [ ] **C17.** Uma rota atravessa quatro formatos: JSON `action` → tabela GDScript → `"area/metodo"` → `D_METHOD` → método C++. Três dos sete nomes sem chamador literal (`clear_gullies` e outros) só existem em JSON de interface.
- [ ] **C18.** Há chamadores fora do git: `bay_census` era pedido por planos do `look` (`log/work-log.md:245`). "Sem chamador" nunca pode virar "morto" sozinho.
- [ ] **C19.** `await` parte a função em dois tempos: o que vem depois é evento, não a mesma chamada.
- [ ] **C20.** Atribuir pode ser chamada: setter de propriedade em `project/render/daylight.gd:20`.
- [ ] **C21.** `&"x"` e `"x"` são tipos diferentes com o mesmo texto, e o tipado dinâmico depende de duck typing.
- [ ] **C22.** Classe sem nenhum caso real, como conexão em cena: prova-se por injeção; vazio real não reprova (P1).

Dados, shader e fronteira
- [ ] **C23.** Um uniform pode existir só em JSON e no shader, quando um laço de `params` o copia por nome.
- [ ] **C24.** Layout binário é contrato sem nome: `INSTANCE_CUSTOM.y` guarda `fase + 8·(kind_a + 2·kind_b)` (`log/work-log.md:55`), e `people.bin` é escrito em Python e lido em C++.
- [ ] **C25.** Dados gerados: a correção vai no gerador, não na saída (`streets.json` por `make_pole_parts.py`, `flow_stats*` pelo tradutor). O dono candidato da fila deve apontar o gerador.
- [ ] **C26.** O Terrain3D só tem binário e seu API não está no `extension_api`: a fronteira dele vem do uso por nome.
- [ ] **C27.** `.gdshader` não é GLSL comum, e sair em SPIR-V pede o compilador do motor, com variantes e `#if CURRENT_RENDERER`. Não há glslang nem spirv-tools na máquina (R5 autoriza instalar).
- [ ] **C28.** Custo por nível gráfico: nesta placa o `auto` escolhe `ultra`, e os testes rodam em `medium` (`log/causa-raiz.md:257`). O custo estático tem de ser calculado por nível.

Perfil de execução
- [ ] **C29.** O perfil é nível IR, 17.825 funções, 4.647 com bloco de alvos indiretos (muitos com zero sites); os nomes têm `city-worktrees/porte-total/...` e incluem `godot-cpp` gerado.
- [ ] **C30.** A função no topo por contagem de bloco é `CityProbes::state_hash`, 1,2 bilhão: "quente no treino" não é "quente para o jogador", o treino está contaminado por sonda.
- [ ] **C31.** O perfil é binário e não acompanha renomeação: a Invariância não o alcança sem uma transformação própria.

Selfcheck e ambiente
- [ ] **C32.** A renomeação precisa ser coerente entre C++, JSON, CSV, `res://`, `#include`, strings de `D_METHOD`, `.uid` e `.tscn`, e a cópia renomeada ainda tem de compilar.
- [ ] **C33.** Muitos índices somam tempo longo: cache por conteúdo, e o cache não pode esconder a falha que a Ablação procura (R7).
- [ ] **C34.** Semente sorteada e bytes iguais: a semente vai no relatório, e o Determinismo vale para o scan com semente fixa.
- [ ] **C35.** Determinismo também exige `PYTHONHASHSEED`, ordem de `os.walk`, ordem de threads, sem data na saída, `core.quotepath` (`log/work-log.md:548`) e o nome reservado `con` (`log/work-log.md:650`).
- [ ] **C36.** Portabilidade sem `node_modules`: o conversor não tem TypeScript próprio, o adaptador degrada com confiança menor. Aqui faltam `tree_sitter`, `scipy` e o binding de `clang`, e o `clangd` não está no PATH.
- [ ] **C37.** HEAD ou disco: há mudança não commitada e `log/mapa.md` já cita `look_aim.gd`, fora do git. O scan lê o commit. Em `tools/converter` há 18 mudanças pendentes.
- [ ] **C38.** Gates sobre o próprio programa: 300 linhas por `.py`, 700 por `.json`, `DOC_UNICO` em `arch_eval` (`.md` e `.txt` vetados ali), marcadores proibidos em `tools/`.
- [ ] **C39.** O gate pode esconder falhas atrás das 15: ele acusa `moto.helmet_r`, e `log/base.md:102` também cita `bicycle.helmet_r`.
- [ ] **C40.** A camada pelo grafo condensado pode colapsar: `CityState` guarda tudo, e sem sensibilidade a campo vira um único componente. Pela arquitetura ele é fonte única, não deus, e o programa distingue isso sem nome.
- [ ] **C41.** Achados que explicam uns aos outros formam ciclo: a raiz sai do componente condensado, não de um nó.
- [ ] **C42.** Instabilidade de 0,85 a 0,88: hipóteses (normalizações globais `sqrt(n)`, rank por `n`, IDF; Louvain não local). Os protótipos também têm números à mão (`0.7`, `40`, `k=10`, grades de `w` e `res`, `".import"` em `dist/arch-proto/meta.py:75`), que a seção 1 proíbe.

## Parte d: classes novas (P2: fazem parte do pedido)

Ligação
- [ ] **D1.** rota composta `"area/metodo"`: `project/game/store/store_api.gd:34`
- [ ] **D2.** enum indexado por texto vindo de dado: `project/ui/roadcraft_hud.gd:226`
- [ ] **D3.** stringificação e colagem de nomes no pré-processador: `native/src/api/city_core_nature.cpp:44`
- [ ] **D4.** setter de propriedade como chamada: `project/render/daylight.gd:20`
- [ ] **D5.** `await` como corte de tempo: `project/render/thunder.gd:62`
- [ ] **D6.** captura de closure entre threads: `native/src/render/buildings/building_renderer.cpp:198`
- [ ] **D7.** `notify` e `wait` de `condition_variable`, e ordem de memória de atômicos
- [ ] **D8.** layout binário entre C++, shader e cozinha (`INSTANCE_CUSTOM`, `people.bin`)
- [ ] **D9.** varying de vértice a fragmento
- [ ] **D10.** máscaras e bits: camada de render, `shadow_caster_mask`, chaves de efeitos
- [ ] **D11.** `uid://` e `.uid`
- [ ] **D12.** importação e filtros de exportação como aresta (o CSV que sumia do pacote, `log/work-log.md:515`)
- [ ] **D13.** ordem de inicialização: autoloads e globais estáticas (`__cxx_global_var_init`)
- [ ] **D14.** arquivo em `user://` entre sessões: `settings.cfg`, saves, `health.json`
- [ ] **D15.** variável de ambiente: `tools/build/pgo.sh:39`
- [ ] **D16.** texto de log como contrato: `FAILURES: 0` casava com `^FAIL` (`log/lab.md:93`)
- [ ] **D17.** gates e `quick.sh` como leitores de dados e de caminhos
- [ ] **D18.** mesmo significado em duas linguagens: constante de shader e de C++
- [ ] **D19.** comentário e `_doc` como aresta fraca

Tempo
- [ ] **D20.** por distância da câmera: faixa de visibilidade decidida pelo motor, `native/src/render/rs_util.h:65`
- [ ] **D21.** por hora do dia e por clima
- [ ] **D22.** por tamanho do mapa
- [ ] **D23.** na primeira compilação de pipeline
- [ ] **D24.** ao perder o foco e ao fechar

Custo
- [ ] **D25.** sobrecusto de quadras 2x2 em triângulos pequenos (`log/work-log.md:370`)
- [ ] **D26.** caixa de recorte larga demais, que mantém tudo desenhado (`log/causa-raiz.md:241`) ou um nível vivo (`log/work-log.md:453`)
- [ ] **D27.** compilação de pipeline no meio do jogo (`log/lab.md:23`)
- [ ] **D28.** medida que causa o custo que mede: `health_log` → `render_cost` (`log/causa-raiz.md:63`)
- [ ] **D29.** refazer interface inteira por temporizador (`log/lab.md:112`)
- [ ] **D30.** conversão Variant e Dictionary na fronteira a cada quadro
- [ ] **D31.** cópias que o motor guarda sozinho: 3 por instância (`log/mapa.md:272`)
- [ ] **D32.** troca de nível sem histerese
- [ ] **D33.** cadeia serial de carregamento e decodificação dupla
- [ ] **D34.** trava longa durante o save
- [ ] **D35.** excesso de threads e rotatividade de jobs
- [ ] **D36.** falso compartilhamento e layout de dados
- [ ] **D37.** consumo de banda de textura e formato
- [ ] **D38.** nível padrão caro pela placa
- [ ] **D39.** sobrecusto de código só de teste dentro da DLL entregue

## Respostas do usuário às dúvidas (fazem parte da confirmação)

- [ ] **R1.** Não use .gitattributes. O build.sh não usa git archive, confira. O selfcheck avalia numa cópia temporária sem os gabaritos, sem arch_eval e sem o próprio programa, e o scan normal tem de dar o mesmo resultado. Assim a contaminação vira prova, e os arquivos de índice são reconhecidos pelo comportamento. (Conferido: `tools/build/build.sh:72` usa `git archive`; a decisão não muda.)
- [ ] **R2.** Aceito: nome é o vocabulário do projeto, e a API do motor e a gramática são lidas de fora em tempo de execução.
- [ ] **R3.** Aceito: nenhum número que mude o resultado. O que sobrar de parâmetro de algoritmo é provado inerte pelo selfcheck. Hold-out por origem rolante.
- [ ] **R4.** Sim: exportação temporária de commit antigo em dist/arch é permitida. Não é worktree.
- [ ] **R5.** Sim: pode instalar glslang, spirv-tools e compiladores offline. Godot sem janela só para compilar shader não conta como abrir o jogo. Use hidden_run.ps1 e tempo-limite.
- [ ] **R6.** Aceito a significância por permutação, que é critério da prova, não do programa. Na Portabilidade, além disso, mostre os grupos achados em cada repositório, para eu ler.
- [ ] **R7.** Aceito o cache por conteúdo e a rodada completa em segundo plano com tempo-limite. A Ablação e a rodada final da entrega rodam sem cache.
- [ ] **R8.** Está invertido. "Dependência de outras": as outras usam ela. "Dependente de outras": ela usa as outras. Cubra os dois sentidos.
- [ ] **R9.** Sim: tools/process/CLAUDE.md é velho. Vale o CLAUDE.md da raiz.
- [ ] **R10.** Sem executável novo nesta fase, salvo se algo em project/ mudar. Se os schemas exigirem mudar dado do jogo, gere o executável. Nesta fase não há nada mais a entregar além da seção 14.
- [ ] **P1.** Classe sem caso real no projeto, como conexão em cena, se prova por injeção. Vazio real não reprova essa classe.
- [ ] **P2.** Tudo o que você listou em c e d faz parte do pedido, não são sugestões.
- [ ] **P3.** Confira as citações antes de usar: meta.py:681 não existe, o arquivo tem 115 linhas.

## Passo 1: identificar
Ciclo 1, 2026-11-06. Problemas (cada um liga a itens acima):
- I1. Não existe mecanismo genérico: `causes.py` usa regex sobre forma, `graph.cjs` e `features.cjs` têm nomes, `cppgraph` é token a token por clangd (lento) e sem macro, template e lambda; o protótipo vê arquivo, não unidade.
- I2. `dist/arch-proto` tem números à mão e Louvain global (C42).
- I3. Medido: `ninja -t compdb` em `native/build-win-debug` dá 1.579 entradas, 378 do próprio código; libclang analisa uma unidade em 0,5 s e anda 90 mil nós por segundo; sem o triplo e os caminhos do driver falta `cstdint`.
- I4. `extension_api-4-7.json`: 1.437 virtuais; 152 argumentos `Callable`; `Object.call` e irmãos têm `method`/`property`/`signal` em `StringName`: papéis saem da forma, sem nome no programa. `_process` e `_physics_process` são os virtuais de um só `float` chamado `delta` em `Node`.
- I5. Gate: 15 falhas fechadas em `ad7ea03d`; achei uma chave morta (`agent.params.cloth_roughness`, sem uniform em shader de `agent`), retirada: `project/` mudou, a entrega leva executável novo (R10).

## Passo 2: pesquisar
| Fonte | Liga a | O que disse |
|---|---|---|
| [libclang-ng](https://pypi.org/project/libclang-ng/) | I1, I3 | `clang.cindex` com libclang embutida, LLVM 20 a 22; `CompilationDatabase` |
| [Sridharan, Fink, Bodík, Thin slicing, PLDI 2007](https://homes.cs.washington.edu/~bodik/ucb/research/pldi07-thin-slicing.pdf) | A4 | a fatia fina guarda só os produtores (quem computa e copia o valor), sem os que explicam; ignora o fluxo para ponteiros-base do heap |
| [tree-sitter-gdscript](https://www.npmjs.com/package/tree-sitter-gdscript), [tree-sitter-gdshader](https://rust-digger.code-maven.com/crates/tree-sitter-gdshader) | A2 | gramática de GDScript existe no PyPI (6.1.0, instalada); GDShader não tem especificação oficial, é GLSL ES 3.0 com restrições |
| [value-prof.proftext](https://llvm.googlesource.com/llvm/+/master/test/tools/llvm-profdata/value-prof.proftext) | A6 | texto do perfil: nome, `# Func Hash`, contadores, `IPVK_IndirectCallTarget`, sites com `nome:contagem` |
| [RGA](https://gpuopen.com/manuals/rga_manual/quickstart), [Mali Offline Compiler](https://documentation-service.arm.com/static/5fd35c414f183d1bab003f95) | A7.5, C27 | análise estática de SPIR-V: instruções, registradores; o resultado depende da placa, a contagem de SPIR-V não |
| [FastEnsemble](https://arxiv.org/pdf/2409.02077) | A9.5, C42 | ordem dos nós e semente mudam muito o resultado do Leiden; consenso reduz, não elimina |
| [Agrahari e Chimalakonda, catálogo de antipadrões de jogos](https://arxiv.org/abs/2006.13129) | A7.5 | 15 antipadrões por mineração; não cobre CPU e GPU fino, a tabela de custos vem também dos registros do projeto (parte d) |
| [godot-docs shader_reference](https://github.com/godotengine/godot-docs/tree/master/tutorials/shaders/shader_reference) | C27, R2 | tabelas de built-ins (`in vec3 **VERTEX**`): lidas de fora, em `dist/arch/external` |
| [Godot, pipeline compilations](https://docs.godotengine.org/uk/4.4/_sources/tutorials/performance/pipeline_compilations.rst.txt) | D23, D27 | shader vai a SPIR-V e o pipeline compila depois; 4.4 pré-compila ao carregar |

Dependências instaladas (A11.7): `libclang-ng`, `tree-sitter`, gramáticas `gdscript`, `typescript`, `bash`, `cmake`, `cpp`, `glsl`, `scipy` (pip); Vulkan SDK 1.4.363 (`winget`, glslang, spirv-opt, spirv-cross).

## Passo 3: compreender
- Unidade e aresta precisam vir do compilador onde há compilador: libclang por unidade de compilação em paralelo, em vez de token a token no clangd (I3). As configurações de build são a união (C11).
- O que liga linguagens é texto. A cadeia `action` → tabela → `area/metodo` → `D_METHOD` → método (C17) é resolvida por tabela de nomes por espaço (método, sinal, uniform, chave, caminho), com a posição do argumento dita pela API lida de fora: nunca por nome no programa.
- A instabilidade do protótipo vem de duas coisas globais: normalização que depende do tamanho do corpus e partição que otimiza um valor global. Remédio de causa: similaridade local (só dos vizinhos) e árvore por pares recíprocos, com seleção por estabilidade da própria árvore (excesso de massa), sem resolução global.
- Anti-maquiagem pede que nada dependa de arquivo: linhagem por corpo normalizado sem nomes, repasse contraído antes de tudo, notas por unidade.
- Invariância pede que nenhum empate seja decidido por nome: ordem por estrutura, e renomear por subpalavra, para colagem de macro seguir compilando (C12, C32).

## Passo 4: planejar
Dono único: `tools/analysis/arch/`. Etapas do scan: instantâneo do commit → papéis de arquivo pelo comportamento → adaptadores (fatos por unidade) → enumeração independente → tabela de nomes e ligação → linhagem e perfil → contração → grupos, camadas, fatias → detectores → grafo de achados e raízes → fila. Selfcheck em cópias, gabaritos em `tools/analysis/arch_eval/` só lidos por ele. Saídas em `dist/arch/`, inclusive `queue.json`, `coverage.json`, `residual.json`.

## Passo 5: codificar
Ciclo 1 (2026-11-06). Criados: `tools/analysis/arch.py`, `tools/analysis/arch/**` (núcleo, adaptadores, ligação, histórico, perfil, grupos, camadas, custos, desenho, causas, fila, selfcheck em `check/`), `tools/analysis/arch_eval/**`. Alterados: `tools/gates/rules.json`, `tools/gates/schemas/{agents,drawers,materials}.schema.json`, `project/data/render/materials.json` (`ad7ea03d`), `CLAUDE.md`, `log/mapa.md`.
Causas achadas codificando, registradas para não repetir:
- Pasta estrangeira: taxa por arquivo cai quando chegam arquivos novos numa pasta viva; o certo é commits absolutos depois da chegada, mais o que a pasta conhece de fora (arquivo ou tipo declarado) e a chegada concentrada.
- `natural_break` em cauda pesada isola só o extremo: para "o que quase todos usam" serve a quebra da cauda (`design.tail_cut`).
- `.gdshader`: o comentário `//` pegava `res://`; derivadas só valem em fragmento; condição `#if X == 1` não define `1`.
- Operador `a = b` no libclang traz o objeto como primeiro argumento; sem isso escrita virava leitura.
- `npm install typescript` trouxe a 7, sem API do compilador: fixada a 5.
- Heredoc do Bash neste ambiente desfaz `\`: edições com barra invertida vão por arquivo.

Ciclo 2 (2026-11-06, pedido do usuário: tempo de execução). Causas da demora, lidas no código, e o que as removeu:
- `selfcheck` relia o repositório inteiro em cada verificação (cerca de 15 leituras) e rodava uma verificação depois da outra. Agora: uma leitura do zero, um depósito criado para a rodada e apagado com ela, verificações lado a lado (`check/run.py`), cada uma refaz só o que a sua cópia mudou. A nova verificação `incremental` prova que o depósito dá o mesmo que o zero, e `from_nothing` é um segundo scan sem depósito nenhum.
- `Repo.on_disk` rodava `git ls-files --ignored` por chamada e varria a própria saída em `dist/arch` (14 GB de sobras): uma listagem por processo, sem a saída do programa.
- `natural_break` em Python puro sobre amostras grandes: uma ordenação e somas acumuladas (`numpy`).
- `cost_in_commits` intersectava cada achado com cada commit (17,6 milhões de interseções): índice de commits por linhagem.
- `Costs._boundary_with` recalculava palavras a cada detector: índice palavra → chamada.
- `canonical_ranks` usava hash em número fixo de rodadas: refino de partição até o ponto fixo, só nas classes vizinhas de uma mudança. Um parâmetro a menos.
- `cpp_walk` lia o nome do arquivo de cada cursor de macro e de `std`/`godot`, centenas de milhares por unidade: pergunta por handle, e blocos de fora não são percorridos (salvo se um arquivo de fora inclui um nosso).
- `cpp.py` usava um `Manager` com ida e volta por arquivo para decidir quem percorre cada header, com disputa entre processos: dono escolhido antes pelo texto dos `#include`, com carga equilibrada e sem disputa; unidade só é refeita se algo que ela enxerga mudou.
- Histórico e palavras de cada arquivo liam blob a blob: leitura em todos os núcleos, cada processo com seu fluxo do git. Pontos e classes só são calculados para o relatório, não em cada cópia.
Defeitos achados nessa leitura: `git cat-file` compartilhado entre duas threads travava a leitura (trava por pergunta); `gd_body` cortava o id em `#b`, então função com nome iniciado em "b" perdia o nome e colidia (achado pela semente sorteada); `Renamer` trocava letras dentro de `0xff00ffffu` e quebrava a compilação da cópia (causa do único grupo diferente na Invariância); achado de dois lugares escolhia a cabeça por nome.
Causa de método, minha: rodar para descobrir em vez de ler o código, e corrigir uma falha por rodada. O usuário cobrou isso três vezes nesta sessão.

Ciclo 3 (2026-11-06, execução sob supervisor). O que a execução paralela mostrou, com a causa de cada um:
- Fila: reservar núcleos segurava 12 tarefas atrás de 2. O sistema divide os processadores; só memória que não existe segura uma tarefa, e nunca as menores (`check/supervisor.py`).
- `WinError 1455`: cada processo do pool carregava `numpy`, e o OpenBLAS reserva memória por núcleo. `numpy` só entra em quem corta amostras, com uma thread.
- Crash `0xC0000005`: `faulthandler.dump_traceback_later` lê frames sem segurar o interpretador e derrubou a tarefa que vigiava. Pilhas agora por thread que segura o interpretador (`job_kinds._keep_stacks`).
- Falso travamento: quando o pool fecha, o tempo de CPU somado cai; "parado" é não mudar, não deixar de subir.
- Sem prazo fixo: travada é a tarefa parada por mais tempo do que já trabalhou; atraso é avisado (`LATE`) e nada espera por ela. Painel `NOW` em tempo real.
- `scattered` guardava todos os pares: agora um achado por unidade, com contagem de pares esquecida unidade a unidade.
- `compile_units` dava a arquivo novo as opções da maior biblioteca; agora as da vizinhança dele. Variável errada numa compreensão zerou o C++ em uma rodada: a contagem de unidades denunciou.
- Exportação extraía `arch_eval`: o programa e os gabaritos nunca saem para o disco.
- Dono candidato vinha vazio quando só a entrada dominava: agora o lugar mais usado, marcado `most_used`.

## Passo 6: testar

## Passo 7: verificar

## Provas
