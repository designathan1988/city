# Cidade — city builder (Godot 4.7 + núcleo C++ GDExtension)

Único documento do projeto. Regras, comandos, mapa do código e pendências ficam aqui; não crie outros
documentos nem memórias (o gate `DOC_UNICO` confere). Atualize este arquivo na mesma
mudança que altera o que ele descreve: o gate `MAPA_DESATUALIZADO` falha quando o mapa diverge do disco.

## Processo de trabalho (inegociável, toda tarefa, nesta ordem; trabalho direto, sem agentes)
A skill `corrigir-na-causa` é a forma operacional deste processo; use-a em toda tarefa.
1. **Análise:** ler o código envolvido, reproduzir e medir (sonda `render_cost`, `tests/perf/camera_pan`, capturas, CPU,
   GPU, memória) até achar a causa raiz. Corrigir a causa, nunca o efeito. Medir é desta etapa.
2. **Pesquisa** na documentação oficial (Godot, godot-cpp, Terrain3D) até entender o que fazer e o método; nunca confiar
   só na memória.
3. **Compreensão e plano** antes de codificar: a solução completa e correta, nunca a mais barata; sem arquivo-deus nem
   funcionalidade espalhada; regras deste arquivo (dono único, sem número mágico, dados em `data/**`, mapa em dia).
4. **Codificação** sem tentativa e erro: se não deu certo, voltar à documentação até entender.
5. **Teste:** "Gates OK" em `python3 tools/gates/run_gates.py`; C++ mudou → `tools/build/build.sh
   core` / `core-windows` (DLL de release e de debug); visível mudou → `tools/build/build.sh e2e`. Medir de novo: quadros
   sem travar, GPU nunca a 100%, CPU e memória nunca acima de antes. Fotos analisadas com atenção (contra a referência e
   o estado anterior): nenhuma perda de qualidade visual; todo problema achado é corrigido.
6. Nunca esperar às cegas um processo lançado (tempo-limite e acompanhamento); parar só processos próprios, pelo PID.
7. Commit por tema e push regular em `origin/master`.
- **Referência — sempre acessível:** `C:\Users\Jonathan\Documents\Projects\roadcraft-4.0-main` (Roadcraft 4.0) é a referência
  de visual e de comportamento de tudo (gráficos, prédios, tráfego, semáforos, pedestres, carros, postes e fios): o jogo
  deve ter todos os gráficos e funcionalidades dele. A pasta pode ser lida, aberta, rodada e estudada a qualquer momento.
  O código dele é referência de comportamento, não de implementação: não se porta o código; portam-se as funcionalidades,
  cujo rastreamento passa pelo conversor (`converter/scripts/port.ts extract`) e cuja conversão é feita na arquitetura
  atual, otimizada; nada de colar código não otimizado sem a arquitetura e os gates.

## Regras de arquitetura (verificadas automaticamente)
- **Fonte única da verdade:** `CityState` (`native/src/sim/state/city_state.h`). Só `sim/commands` muda o
  estado a pedido do jogador; só `sim/simulation` avança o tempo. A UI nunca guarda números do jogo
  (`ESTADO_NA_UI`): lê `CityStore.snapshot` e envia `CityStore.dispatch(...)`.
- **Um dono por definição:** valores ajustáveis vivem só em `project/data/**` (tabelas JSON com schema).
  Nomes do tuning: X-macro `CITY_TUNING_FIELDS`; valores: `data/catalog/tuning.json` (`gate_tuning.py` exige
  as mesmas chaves e que todas sejam lidas). Regras com dono único estão em `tools/gates/owners.json`
  (altura do prédio, andar, regra do lote, "há trabalho a salvar", fachada, mobiliário da rua, dinheiro, estado do jogo);
  reimplementar o padrão em outro arquivo gera `DONO_UNICO`.
- **Sem números mágicos** em `native/src`, `project/game`, `project/ui` (`NUMERO_MAGICO`): dê nome
  (constante no dono) ou mova para dados. Valor distintivo de um dado redigitado no código → `VALOR_DUPLICADO`;
  constante com o nome de uma chave de dados → `SEGUNDO_DONO` (exceções justificadas em `rules.json`).
- **Camadas C++** (`CAMADA`): `core` → `sim/*` (C++ puro, sem Godot) → `render` (só lê) → `api`.
- **Sem arquivos-deus:** limites de linhas por arquivo e por função (`rules.json/max_lines`).
- **Sem código duplicado** (`CODIGO_DUPLICADO`, 80+ tokens iguais): extraia um dono.
- **Interface:** cores/tamanhos só de `data/ui/tokens.json` via `ui/theme/theme_builder.gd`; textos só via
  `tr("CHAVE")` com `data/i18n/strings*.csv`; botões, estatísticas e opções da HUD vêm de `data/ui/*.json`.
- **Fluxo:** troca de tela, pausa e saída só por `GameFlow`. JSON de dados só é lido por `DataTables`.
- **Testes conferem o que o jogador vê:** HUD, buffer de instâncias enviado à GPU, sondas do núcleo.
- **Testes e capturas (`--no-flow`) rodam nos dados:** não leem nem gravam `settings.cfg`, nem leem `health.json`; abrem no nível `tests_preset` (nunca `auto`, para não depender da placa de vídeo; outro nível é pedido pelo nome, `--preset=`); saves ficam em `user://saves_tests`. O `user://` é um só para todo clone com o mesmo nome de projeto; para isolar um clone, `project/override.cfg` com `application/config/use_custom_user_dir=true` e `custom_user_dir_name`.
- **Pendências** vão para a seção abaixo, nunca para comentários TODO/FIXME (`SEM_MARCADORES_SOLTOS`).

## Comandos
| O quê | Comando |
|---|---|
| Dependência do núcleo (uma vez) | `git clone https://github.com/godotengine/godot-cpp native/godot-cpp` (API 4.7) |
| Gates | `python3 tools/gates/run_gates.py` (`--file X` para um arquivo, `--summary` resumo) |
| Núcleo Linux / Windows | `tools/build/build.sh core` / `MINGW=... tools/build/build.sh core-windows` (DLL de release e de debug, a que o editor e os testes carregam) |
| Testes core + e2e (precisa de display) | `GODOT=/caminho/godot tools/build/build.sh e2e` |
| Capturas de tela do jogo real | `tools/build/build.sh capture /pasta` |
| Exportar + zip | `tools/build/build.sh export && tools/build/build.sh package` |
| Rodar o jogo | `godot --path project` (Forward+; sem Vulkan use `--rendering-method gl_compatibility`) |

## Fluxo de dados e threads
`DataTables` lê `data/**` (tabelas grandes divididas em partes com o mesmo schema) → `CityStore.boot` envia o
catálogo ao `CityCore` (rejeita chave ausente) → `Simulation` roda em thread própria com tick fixo
(`sim_tick_hz`), sob `map_mutex` compartilhado e `traffic_mutex`; comandos, crescimento e relógio tomam
`map_mutex` exclusivo → `ChangeLog` (único canal de "sujo"; drenado sob trava compartilhada + mutex do log) →
`RenderWorld.sync` (nunca toma o mapa exclusivo) manda as malhas ao `JobPool`: o job de prédio copia o que lê sob trava curta
(`HeightPatch`); a conversão da malha entra em `render.upload_budget_mb` por quadro (o envio à GPU roda na próxima chamada do
thread principal) e o commit só troca malhas, em `render.commit_budget_ms`. Poses por `PoseBuffer`; o save grava em thread própria.

## Luz e relógio
`Daylight` é o dono do sol, do preenchimento, do ambiente, da luz de cima e do céu, e encaixa as cascatas na vista (`_fit_cascades`). O ambiente é chapado e a luz de cima, sem sombra, só chega no que olha para o céu: medidos num cubo cinza nos dois jogos (`capture_landscape --only=cubo_a,cubo_b`). `CityWorld.set_sky_mode("day"|"night"|"cycle")`: dia e noite fixam as horas de `daylight.json` (16h e 22h, como a barra do 4.0), `cycle` segue a hora do snapshot passo a passo (`clock_step_h`). Testes e capturas usam `set_hour` e movem relógio e céu juntos, nunca um só. Par de foto com o 4.0: mesma hora (o modo dia do 4.0 é 16h, não o relógio), mesma câmera (`capture_landscape.gd` `_shot` recebe azimute, inclinação e zoom do `setView`) e 1920x1080.

## Máquinas de estado
- `GameFlow.State`: BOOT → MENU (tela inicial) → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram/saem de STOPPED.
- `ToolController.Tool` (de mãos vazias é INSPECT): NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT, MOVE, CONTROL, PAINT, TREES, CLOUD, FOG, WEATHER, ELEMENTS, STRIKE, SHOOT, BUILD, TRANSIT (só `set_tool`; botão segurado → `drag`/`tick`, solto → `release`); `RoadTool`: um arrasto desenha a via, um clique segura o começo (encadeia) e, no alinhamento CURVE, o clique no fim deixa a curva seguindo o ponteiro até o próximo clique (`Alignment`: STRAIGHT, CURVE, FREE).

## Mapa do código
<!-- MAPA:INICIO -->
**Raiz e ferramentas**
- `.claude/settings.json` — permissões das ferramentas, memória automática desligada e hook que entrega o Processo de trabalho em toda mensagem e antes de toda ferramenta (script na pasta de ferramentas da raiz do projeto, fora deste repositório; os gates rodam na etapa de teste)
- `native/CMakeLists.txt` — Godot 4.7 API, precisão simples, saída em `project/bin`
- `native/toolchain-mingw.cmake` — compilação cruzada Windows via llvm-mingw
- `project/export_presets.cfg` · `project/project.godot` — exportação Windows e projeto Godot (autoloads, cena inicial)

**API GDExtension — native/src/api**
- `native/src/api/body_doc.cpp` · `native/src/api/body_doc.h` — um prédio como documento (projeto de lote, prédio de uma cidade do Roadcraft) feito corpo
- `native/src/api/buildings_loader.cpp` · `native/src/api/buildings_loader.h` — buildings.json → tabelas do gerador de prédios da simulação
- `native/src/api/blast_loader.cpp` — blast.json → BlastParams estrito (curvas K-B, limiares, pessoas, cratera)
- `native/src/api/city_core_strike.cpp` — a bomba como API de script (`strike`, um passo de desfazer) e as sondas `damage_probe` (o que os golpes deixaram no mapa) e `blast_probe` (números do modelo)
- `native/src/api/catalog_loader.cpp` · `native/src/api/catalog_loader.h` · `native/src/api/furniture_loader.cpp` — Dictionary → Catalog estrito; as X-macros ditam as chaves (tuning, mobiliário da rua)
- `native/src/api/city_core.cpp` · `native/src/api/city_core.h` · `native/src/api/city_core_parts.h` — ciclo de vida: catálogo, render, mapa novo, velocidade, sincronização por quadro; o que o núcleo guarda para cada área (o save em thread, o cache do inspetor, as contagens do snapshot, as estatísticas das poses)
- `native/src/api/city_core_bind.cpp` — bindings de script do CityCore
- `native/src/api/city_core_commands.cpp` — comandos do jogador e prévias (sim/commands) como API de script
- `native/src/api/city_core_body.cpp` — o corpo de um prédio para o Construir: o que ele tem e as edições como comandos de script
- `native/src/api/city_core_edit.cpp` — desfazer/refazer, traços e edições das gavetas (dividir, melhorar, rotatória, relevo, prédio desenhado)
- `native/src/api/city_core_interior.cpp` — a camada Interior dos prédios como API de script: abrir um prédio num andar, ler o estado (andares e lâmpadas: leitura pesada, só depois de um comando), a revisão barata que avisa quando reler, e o custo do interior de um prédio (testes)
- `native/src/api/city_core_persist.cpp` — salvar/abrir: encode sob trava curta, ZSTD e gravação em thread própria
- `native/src/api/city_core_clearance_probe.cpp` — folga dos carros de lote ao longo do caminho e onde cada veículo está na faixa; a regra do âmbar para os testes
- `native/src/api/city_core_park_probe.cpp` — onde as capturas olham vagas, lotes, ciclofaixas e docas, e se o ponto da câmera está livre
- `native/src/api/city_core_query.cpp` · `native/src/api/city_core_probe.cpp` · `native/src/api/city_core_probe_render.cpp` · `native/src/api/city_core_people.cpp` · `native/src/api/city_core_jobs.cpp` — leituras para a UI e os testes (snapshot, altura, raycast); sondas: custo do quadro, chegada das poses, tráfego e a regra da fila de jobs
- `native/src/api/function_names.h` — nomes das funções do prédio, como os modelos e os documentos os escrevem
- `native/src/api/city_core_inspect.cpp` — o que a ferramenta Inspecionar, o aviso da barra e a câmera perguntam: tráfego ao vivo de via e cruzamento, nós impossíveis e a linha de visão livre de prédios
- `native/src/api/edit_result.h` — EditResult como Dictionary e `run_edit` (comando dentro de um passo de undo)
- `native/src/api/city_core_nature.cpp` · `native/src/api/nature_loader.cpp` — gaveta Paisagem como API de script (clima, nuvens, neblina, pintura, árvores, bioma); nature.json → NatureDef estrito
- `native/src/api/interiors_loader.cpp` · `native/src/api/interiors_loader.h` — interiors.json → tabelas dos interiores
- `native/src/api/city_core_models.cpp` · `native/src/api/models_loader.cpp` · `native/src/api/models_loader.h` — Construir › Modelos: catálogo, corpo em mãos como bytes, encaixe, colocar e desenhar à parte; models.json → catálogo
- `native/src/api/register_types.cpp` — entrada da GDExtension: registra CityCore
- `native/src/api/city_core_transit.cpp` — transporte público como API de script
- `native/src/api/traffic_loader.cpp` — Dictionary → TrafficParams estrito

**Núcleo — native/src/core**
- `native/src/core/bytes.h` — leitor/escritor binário do formato de save
- `native/src/core/color.h` — "#RRGGBB"/"#RRGGBBAA" → RGBA8 empacotado (único parser de cor)
- `native/src/core/grid.h` — chunks, chaves de célula, hash espacial e orçamento de build
- `native/src/core/job_pool.cpp` · `native/src/core/job_pool.h` — pool de jobs por prioridade e `parallel_for`; a chave do job é tipada (`JobKey`: uma `JobLane` por usuário e um id), então dois usuários não se substituem na fila
- `native/src/core/math.h` — vetores 2D no plano XZ
- `native/src/core/random.h` — hash, RNG e ruído determinísticos
- `native/src/core/seeded.h` — geradores com os números do Roadcraft bit a bit
- `native/src/core/units.h` · `native/src/core/laps.h` — unidades físicas e de calendário (único dono dos fatores); cronômetro das sondas

**Núcleo — native/src/sim**
- `native/src/sim/buildings/building_store.cpp` · `native/src/sim/buildings/building_store.h` — registros de prédio, ocupação, consultas espaciais e tabelas do gerador
- `native/src/sim/buildings/lot_access.cpp` · `native/src/sim/buildings/lot_access.h` · `native/src/sim/buildings/b40/record.cpp` · `native/src/sim/buildings/b40/record.h` — portões, vagas e docas do lote com o caminho até o portão (grade, Dijkstra) para o tráfego; corpo de um registro, referencial do lote e árvores do lote (dono único)
- `native/src/sim/buildings/lot_doors.cpp` · `native/src/sim/buildings/lot_doors.h` — a porta da frente do lote e o pé do caminho a pé na calçada
- `native/src/sim/buildings/b40/data.h` · `native/src/sim/buildings/b40/types.h` — tabelas do gerador (estilos, paletas, aberturas) e vocabulário do corpo (bays, telhados, acabamentos, lote)
- `native/src/sim/buildings/b40/gen.h` · `native/src/sim/buildings/b40/massing.h` · `native/src/sim/buildings/b40/paint.cpp` · `native/src/sim/buildings/b40/paint.h` — gerador do corpo de um registro num Xoshiro128, passos comuns do massing; ajuste HSL de telhado e segunda pele
- `native/src/sim/buildings/b40/model.h` — um modelo como models.json o monta, bloco por bloco
- `native/src/sim/buildings/b40/massing_core.cpp` · `native/src/sim/buildings/b40/massing_house.cpp` · `native/src/sim/buildings/b40/massing_tall.cpp` ·
  `native/src/sim/buildings/b40/massing_works.cpp` — corpo de um registro (blocos, telhados, segunda pele); casas; apartamentos, torres, lojas e escritórios; galpões
- `native/src/sim/buildings/b40/facade_compose.cpp` — gramática de fachada (dono da fachada)
- `native/src/sim/buildings/b40/lot_plan.cpp` — plano do lote crescido (frente, lados e fundo)
- `native/src/sim/buildings/b40/lot_layout.h` · `native/src/sim/buildings/b40/lot_space.cpp` · `native/src/sim/buildings/b40/lot_ground.cpp` · `native/src/sim/buildings/b40/lot_boundary.cpp` — o lote: zonas, portas, caminhos reservados e regra de encaixe; chão e caminhos do carro e da porta; divisa fechada por construção e portões
- `native/src/sim/buildings/b40/lot_cars.cpp` · `native/src/sim/buildings/b40/lot_front.cpp` · `native/src/sim/buildings/b40/lot_back.cpp` · `native/src/sim/buildings/b40/lot_yard.cpp` — vaga de toda casa (garagem,
  cobertura, vaga de frente), acessos e becos; frente; estacionamento, serviço, pátio e docas; quintal e pátios livres
- `native/src/sim/buildings/b40/body_table.h` · `native/src/sim/buildings/b40/plan_area.cpp` · `native/src/sim/buildings/b40/plan_area.h` · `native/src/sim/buildings/b40/roof_shape.h` — as versões do corpo que o jogador editou, cada uma inteira e imutável; áreas exatas de plantas (orelhas e corte convexo); forma do telhado (dono único)
- `native/src/sim/buildings/b40/civic.cpp` · `native/src/sim/buildings/b40/civic.h` · `native/src/sim/buildings/b40/civic_services.cpp` · `native/src/sim/buildings/b40/open_lots.cpp`
  — corpos cívicos (escola, igreja, hospital; posto, mercado, prefeitura, polícia, bombeiros) e lotes abertos (praça, parque, quadra, vazio)
- `native/src/sim/buildings/growth.h` — regras do que cresce num lote (pisos por zona/rua, altura orgânica)
- `native/src/sim/buildings/interior/interior.cpp` · `native/src/sim/buildings/interior/interior.h` · `native/src/sim/buildings/interior/tables.h` — o que há dentro de um prédio, andar por andar: tipo de andar (iguais mobiliados uma vez), uso do prédio, receitas de `project/data/render/interiors.json`, luz por cômodo, casas e lojas do andar
- `native/src/sim/buildings/interior/plan.cpp` · `native/src/sim/buildings/interior/plan.h` · `native/src/sim/buildings/interior/interior_homes.cpp` · `native/src/sim/buildings/interior/interior_rooms.cpp` — o andar sendo mobiliado (o que cabe onde) e a planta em apartamentos; apartamentos, casas e saguão; salas, aulas, enfermarias, hotel, lojas, restaurantes, celas e shopping
- `native/src/sim/buildings/interior/cut.cpp` · `native/src/sim/buildings/interior/cut.h` — o prédio cortado num andar (dono único do corte): volumes acima saem, os demais perdem os andares de cima; a direção da câmera em 8 passos
- `native/src/sim/buildings/interior/view.h` — o que a camada Interior mostra: quais prédios estão cortados, em que andar e para que direção da câmera (vista, não é salva)
- `native/src/sim/catalog/catalog.h` · `native/src/sim/catalog/furniture_types.h` — tipos do catálogo (sem valores); mobiliário da rua e travessia
- `native/src/sim/catalog/blast_params.h` — parâmetros da explosão (sem valores): curvas de sobrepressão, impulso e duração, limiares de dano, pessoas, cratera
- `native/src/sim/catalog/nature_types.h` — tipos do clima, nuvens, neblina, pintura, pincel de árvores e comunidades de plantas (X-macro do clima)
- `native/src/sim/catalog/profile_types.h` — formas da seção de via que road_profiles.json preenche
- `native/src/sim/catalog/traffic_params.h` — parâmetros de tráfego e pedestres
- `native/src/sim/catalog/tuning_fields.h` — nomes de todo tunable escalar (X-macro)
- `native/src/sim/citygen/block_lots.cpp` — quarteirão fechado cortado em lotes (blockLots.ts)
- `native/src/sim/citygen/city_plan.cpp` · `native/src/sim/citygen/city_plan.h` — o plano da cidade gerada: centros da semente e curva de calor (zona por densidade, comércio nas vias principais, indústria num setor da borda)
- `native/src/sim/citygen/squares.cpp` · `native/src/sim/citygen/squares.h` — as praças da cidade gerada: em cada bairro, a quadra em via principal mais perto do meio vira gramado com caminhos e árvores
- `native/src/sim/citygen/city_gen.cpp` · `native/src/sim/citygen/city_gen.h` — pipeline da cidade gerada
- `native/src/sim/citygen/lot_gen.cpp` · `native/src/sim/citygen/lot_gen.h` — lotes ao longo das ruas zonadas: grade de ocupação, recuos, testes
- `native/src/sim/citygen/lot_place.cpp` — colocação dos lotes: quarteirões fechados e testadas das ruas
- `native/src/sim/citygen/lot_strips.h` — cortador de lotes de uma célula (interno de sim/citygen)
- `native/src/sim/citygen/road_gen.cpp` · `native/src/sim/citygen/road_gen.h` — linhas da malha e marcha sobre o terreno
- `native/src/sim/citygen/road_gen_graph.cpp` — grafo planar das ruas geradas (cruzamentos, nós fundidos)
- `native/src/sim/citygen/road_gen_tidy.cpp` — limpeza do grafo gerado (nós e braços degenerados)
- `native/src/sim/demolition/blast_model.cpp` · `native/src/sim/demolition/blast_model.h` — dono único da física do sopro: escala de Hopkinson-Cranz e curvas de Kingery-Bulmash (Swisdak), pressão sentida pela estrutura, letalidade e tímpano, alcance, cratera
- `native/src/sim/demolition/damage_state.cpp` · `native/src/sim/demolition/damage_state.h` · `native/src/sim/demolition/damage_event.h` — o que sobra dos golpes no `CityState` (crateras, ruínas, mobiliário derrubado, dano dos prédios, fogos) e o relógio do dano; os eventos que o render toca uma vez
- `native/src/sim/demolition/wounds.cpp` — os feridos sangram e morrem do sangramento (pelo relógio do dano, sob a trava exclusiva)
- `native/src/sim/commands/body_edit.cpp` · `native/src/sim/commands/body_facade.cpp` · `native/src/sim/commands/body_finish.cpp` · `native/src/sim/commands/body_mass.cpp` · `native/src/sim/commands/body_ops.h` · `native/src/sim/commands/body_parts.cpp` — o único caminho de uma edição até o corpo (cópia, validação, diário) e o que um corpo precisa ser; abas Fachada, Telhado, Cores, Massa e Peças
- `native/src/sim/commands/body_pick.cpp` · `native/src/sim/commands/body_pick.h` — o que um raio da câmera acerta: registro, bloco, face, andar e vão
- `native/src/sim/commands/commands.h` — API de comandos do jogador
- `native/src/sim/commands/edit_history.cpp` — pilhas de desfazer/refazer e limites
- `native/src/sim/commands/edit_journal.h` — diário de edições (único mutador dos comandos)
- `native/src/sim/commands/journal_ops.cpp` — operações `j_*` do diário
- `native/src/sim/commands/journal_damage.cpp` — operações do diário do dano (cratera, ruína, mobiliário derrubado, dano acumulado do prédio) e o inverso de cada uma
- `native/src/sim/commands/land_commands.cpp` — relevo, prédio desenhado e edição de prédio
- `native/src/sim/commands/element_commands.cpp` — pincel de elementos com desfazer
- `native/src/sim/commands/nature_commands.cpp` — clima, nuvens, neblina, pintura, árvores plantadas, bioma e relevo de volta ao original
- `native/src/sim/commands/model_ops.h` · `native/src/sim/commands/model_body.cpp` · `native/src/sim/commands/model_place.cpp` · `native/src/sim/commands/place_snap.cpp` · `native/src/sim/commands/body_weld.cpp` — modelo feito corpo; colocar (recusas e fusão, um passo de desfazer); encaixe na rua, no vizinho e na grade; dois prédios feitos um
- `native/src/sim/commands/interior_commands.cpp` · `native/src/sim/commands/interior_commands.h` — a camada Interior: valida o prédio e o andar, grava a vista em CityState e marca os chunks a redesenhar (sem desfazer, sem salvar); `interior_focus`, dono único de onde o corpo fica (centro dos volumes, lado maior, andares: as fotos miram aí)
- `native/src/sim/commands/road_commands.cpp` · `native/src/sim/commands/signal_commands.cpp` — construir e demolir vias; tempo fixo, defasagem e onda verde de um semáforo
- `native/src/sim/commands/demolish_box.cpp` — demolir na caixa: prédios, entulho e vias com o meio dentro do retângulo, num passo de desfazer
- `native/src/sim/commands/road_copies.cpp` — inverter mão única, duplicar via e a pintura do mapa
- `native/src/sim/commands/road_edit.cpp` · `native/src/sim/commands/road_edit.h` — passos comuns das edições de via
- `native/src/sim/commands/road_plan.cpp` — encaixe e validação de vias (= prévia)
- `native/src/sim/commands/road_path.cpp` · `native/src/sim/commands/road_snap.cpp` — o gesto vira peças (reta, curva, traço à mão) e o plano e a obra delas; onde a via pousa (rede, ângulos, grade)
- `native/src/sim/commands/road_move.cpp` — Mover: o nó vai e as vias são assentadas de novo
- `native/src/sim/commands/road_rules.cpp` — o que uma edição de via não pode deixar: trecho apertado demais entre duas junções
- `native/src/sim/commands/road_tabs.cpp` — Dividir e Melhorar
- `native/src/sim/commands/roundabout.cpp` — rotatória
- `native/src/sim/commands/transit_commands.cpp` · `native/src/sim/commands/transit_commands.h` — comandos do transporte público
- `native/src/sim/commands/strike_commands.cpp` · `native/src/sim/commands/strike_internal.h` — a bomba: um golpe de força 1..50 aplicado à cidade pelo diário (prédios, chão, vias, mobiliário, veículos, pedestres) e avisado ao render por eventos
- `native/src/sim/commands/strike_buildings.cpp` · `native/src/sim/commands/strike_ground.cpp` · `native/src/sim/commands/strike_roads.cpp` · `native/src/sim/commands/strike_people.cpp` — prédios (dano que se soma, queda e ruína, fogo, desgaste); cratera, terreno e mobiliário derrubado; o trecho de via que a cratera corta; pedestres (mortos, derrubados, assustados) e veículos explodidos
- `native/src/sim/commands/shoot_commands.cpp` · `native/src/sim/commands/shoot_internal.h` · `native/src/sim/commands/shoot_people.cpp` · `native/src/sim/commands/shoot_vehicles.cpp` — a pistola: um tiro ao longo da linha de visão (para no terreno; pega o primeiro pedestre, em pé ou caído, ou veículo); ferida por parte do corpo (cabeça mata, tronco um terço da vida, membro um quinto; membro arrancado, barriga aberta, sangramento até morrer), motociclista derrubado, vidro de carro (para o carro), rajada que o faz explodir; avisa o render por eventos
- `native/src/sim/commands/zone_commands.cpp` — pincel de zona
- `native/src/sim/lots/lot_cache.cpp` · `native/src/sim/lots/lot_cache.h` — lotes por célula recalculados em paralelo
- `native/src/sim/nature/elements.cpp` · `native/src/sim/nature/elements.h` — elementos do pincel (objetos pequenos e efeitos) por chunk
- `native/src/sim/nature/ecology.cpp` · `native/src/sim/nature/ecology.h` — o que cresce onde por bioma (fitofisionomias), calculado por chunk onde é lido
- `native/src/sim/nature/ground_paint.cpp` · `native/src/sim/nature/ground_paint.h` — chão pintado em ladrilhos: superfície, bioma, coberturas e ravinas
- `native/src/sim/nature/nature_state.cpp` · `native/src/sim/nature/nature_state.h` — estado da paisagem no CityState e neblina num ponto
- `native/src/sim/nature/planted_trees.cpp` · `native/src/sim/nature/planted_trees.h` — árvores plantadas e clareiras
- `native/src/sim/nature/sky.cpp` · `native/src/sim/nature/sky.h` — clima do mapa e nuvens colocadas (deriva, escolha pelo raio, espalhar)
- `native/src/sim/persistence/body_codec.cpp` · `native/src/sim/persistence/body_codec.h` — o corpo em bytes, iguais para corpos iguais: seção do save e digest
- `native/src/sim/persistence/save_format.h` — formato do save da cidade
- `native/src/sim/persistence/save_demolition.cpp` · `native/src/sim/persistence/save_demolition.h` — o bloco do save com o que golpes deixaram (crateras, ruínas, mobiliário derrubado, prédios com dano, a espera do crescimento); etiqueta e tamanho antes de SAVE_END, um leitor sem a etiqueta pula o bloco; o registro de prédio compartilhado
- `native/src/sim/persistence/save_nature.cpp` — gravar e abrir a paisagem
- `native/src/sim/persistence/save_read.cpp` — abrir: terreno do seed, ladrilhos esculpidos, modificadores e grafo
- `native/src/sim/persistence/save_write.cpp` — gravar: cabeçalho, terreno, modificadores, vias e prédios
- `native/src/sim/roads/geometry.cpp` · `native/src/sim/roads/geometry.h` — geometria pura das vias (Bézier, tabelas, interseções)
- `native/src/sim/roads/junction.cpp` · `native/src/sim/roads/junction.h` — braços, recuos, esquinas e balões
- `native/src/sim/roads/profiles.cpp` · `native/src/sim/roads/profiles.h` — derivação do perfil (faixas, linhas, calçadas, preço)
- `native/src/sim/roads/road_network.cpp` · `native/src/sim/roads/road_network.h` — armazenamento e consultas do grafo viário
- `native/src/sim/roads/road_variants.cpp` · `native/src/sim/roads/road_variants.h` — via de classe com as faixas, a largura e as vagas das opções da ferramenta, um perfil a mais no catálogo
- `native/src/sim/roads/parking_layout.cpp` · `native/src/sim/roads/parking_layout.h` — dono único das vagas da guia e da pintura delas (CTB art. 181 I, MBST vol. IV: MER e LPP), lido pelo tráfego e pelo render
- `native/src/sim/roads/street_furniture.cpp` · `native/src/sim/roads/street_furniture.h` · `native/src/sim/roads/street_furniture_posts.cpp` · `native/src/sim/roads/street_furniture_internal.h` — dono único de onde fica o mobiliário da rua (hidrantes, lixeiras, bancos, árvores; sempre com passagem para quem anda), lido pelos pedestres e pelo render; postes e semáforos; zonas da calçada
- `native/src/sim/transit/transit_ground.cpp` · `native/src/sim/transit/transit_ground.h` — onde o transporte encontra a rua: pista, superfície da rua, referencial de uma parada, prédio perto
- `native/src/sim/transit/transit_data.cpp` · `native/src/sim/transit/transit_data.h` · `native/src/sim/transit/transit_sim.cpp` · `native/src/sim/transit/transit_sim.h` · `native/src/sim/transit/rail_run.cpp` · `native/src/sim/transit/rail_run.h` — transporte público: plano do jogador (paradas, trilhos, linhas, grafo de trilhos); ônibus de cada linha; trens numa volta, passagens de nível
- `native/src/sim/simulation/city_growth.cpp` · `native/src/sim/simulation/city_growth.h` — crescimento ao longo do tempo nas células da fila
- `native/src/sim/simulation/growth.cpp` — floorsFor/recordFor/fillAll e civicFor do Roadcraft
- `native/src/sim/simulation/simulation.cpp` · `native/src/sim/simulation/simulation.h` — thread do tick, velocidades e relógio
- `native/src/sim/state/change_log.h` — canal único de mudanças (campos líquidos e sujos)
- `native/src/sim/state/city_state.h` — fonte única da verdade, travas, poses, jobs e revisões
- `native/src/sim/state/pose_buffer.h` — buffer triplo sem trava das poses (PoseFrame)
- `native/src/sim/terrain/height_patch.h` — amostrador bilinear único do terreno e cópia das alturas para os jobs
- `native/src/sim/terrain/heightfield.cpp` · `native/src/sim/terrain/heightfield.h` — alturas, consultas, raycast, modificadores
- `native/src/sim/terrain/terrain_brush.cpp` — pincel de relevo e formas
- `native/src/sim/terrain/terrain_gen.cpp` · `native/src/sim/terrain/terrain_gen.h` — terreno gerado (mesmo seed do Roadcraft)
- `native/src/sim/traffic/astar.h` — A* único de carros e pedestres
- `native/src/sim/traffic/demand.cpp` — frota e viagens por zona
- `native/src/sim/traffic/junction_admission.cpp` — antitravamento: saída livre, caixa livre, reserva e governador
- `native/src/sim/traffic/junction_control.cpp` · `native/src/sim/traffic/signal_timing.cpp` · `native/src/sim/traffic/light_rule.cpp` — controle de cruzamento, estágios e preferência; plano no tempo pelo MBST V (entreverdes, luz, janela de pedestres, tempo fixo no relógio comum); regra única do semáforo para os veículos (âmbar: para quem ainda para na linha como o entreverdes supõe, mantido até o verde) e a parada que cada um vê pela rota
- `native/src/sim/traffic/cycling.cpp` · `native/src/sim/traffic/vehicle_speed.cpp` — bicicletas: no bordo da pista, ciclofaixa junto à faixa da guia e quem fica fora do caminho; velocidade de cada veículo (limite, conector, curva, deriva)
- `native/src/sim/traffic/green_wave.cpp` — onda verde: ciclo comum e defasagens pelo tempo de viagem na via principal
- `native/src/sim/traffic/lorry_traffic.cpp` · `native/src/sim/traffic/lane_closures.cpp` · `native/src/sim/traffic/lane_closures.h` · `native/src/sim/traffic/yard.cpp` · `native/src/sim/traffic/yard.h` · `native/src/sim/traffic/yard_input.cpp` — caminhões às docas: chamada, ré para a doca, descarga e volta à via, com a rua fechada enquanto passam; o pátio, planejado a partir de uma cópia em trabalho contado por tick, nunca por tempo
- `native/src/sim/traffic/way_search.cpp` · `native/src/sim/traffic/way_field.cpp` · `native/src/sim/traffic/way_search.h` · `native/src/sim/traffic/reeds_shepp.cpp` · `native/src/sim/traffic/reeds_shepp.h` — A* híbrido (Dolgov 2008) sobre a grade e a clearance do pátio; curvas de Reeds-Shepp
- `native/src/sim/traffic/kerb_stops.cpp` · `native/src/sim/traffic/kerb_stops.h` — recados na guia: quem desce, quem sobe, entrega do caminhão (porta, calçada, quem a pé vai ao carro)
- `native/src/sim/traffic/lane_change.cpp` — MOBIL
- `native/src/sim/traffic/lane_geometry.cpp` — pontos de faixa, conector e calçada
- `native/src/sim/traffic/lane_graph.cpp` — grafo de faixas e conectores com conflitos (build completo e por nó)
- `native/src/sim/traffic/lane_graph_edit.cpp` — rebuild incremental das faixas e nós sujos
- `native/src/sim/traffic/lane_graph_internal.h` — superfície interna do grafo de faixas
- `native/src/sim/traffic/lane_list.cpp` — listas por faixa/conector e lista micro de veículos
- `native/src/sim/traffic/meso.cpp` — modelo meso distante (uma fila por faixa)
- `native/src/sim/traffic/ped_crossing.cpp` · `native/src/sim/traffic/ped_crowd.cpp` — pedestres e carros nas travessias; multidão: quem vê quem (listas por calçada e por cruzamento, as "walking areas" do SUMO) e os corpos remapeados no trecho de cada um
- `native/src/sim/traffic/ped_furniture.cpp` · `native/src/sim/traffic/ped_kerb.cpp` · `native/src/sim/traffic/ped_stripes.cpp` · `native/src/sim/traffic/ped_stripes.h` — mobiliário como corpos nas calçadas; espera na guia; faixas do SUMO em calçada, esquina e faixa
- `native/src/sim/traffic/ped_routes.cpp` · `native/src/sim/traffic/pedestrians.cpp` — rotas a pé e caminho de esquina; pedestres com destino, da porta à porta
- `native/src/sim/traffic/ped_demand.cpp` · `native/src/sim/traffic/ped_doors.cpp` · `native/src/sim/traffic/ped_spawn.cpp` · `native/src/sim/traffic/ped_party.cpp` — quanta gente cada tipo de lugar põe na rua a cada hora e para que lado; portas da janela a pé, lidas aos poucos; quem sai, com que idade e ritmo; quem anda junto
- `native/src/sim/traffic/ped_system.h` · `native/src/sim/traffic/ped_door_walk.cpp` — o que os pedestres declaram (demanda, portas, nascimento, grupo, multidão, guia, mobiliário, rotas) e os primeiros e últimos passos numa porta de lote
- `native/src/sim/traffic/day_table.h` — uma tabela do dia (uma entrada por hora) lida numa hora, com a volta da meia-noite: dono único, lido pelos pedestres e pelos carros dos lotes
- `native/src/sim/traffic/parking_state.h` · `native/src/sim/traffic/parking_sources.cpp` · `native/src/sim/traffic/parking_lanes.cpp` — vagas perto da câmera (guias e lotes, lidas por orçamento) e carros parados de cenário; ponto da via, faixa de uma portaria e quem está a pé perto de um ponto
- `native/src/sim/traffic/lot_traffic.cpp` · `native/src/sim/traffic/lot_demand.cpp` · `native/src/sim/traffic/manoeuvre.cpp` · `native/src/sim/traffic/manoeuvre.h` — carros que entram e saem dos lotes pelas portarias, pelas horas de cada tipo de lugar; caminho fora da via (curvas de Hermite, ré e frente)
- `native/src/sim/traffic/ped_wounds.cpp` — quanto do passo próprio guarda um pedestre ferido (leve, na perna, grave)
- `native/src/sim/traffic/route_arena.cpp` — arena de rotas (RouteRef) reutilizada
- `native/src/sim/traffic/routing.cpp` — rotas de carro por segmento dirigido, desvio e reconsideração
- `native/src/sim/traffic/traffic_state.h` · `native/src/sim/traffic/lane_graph_types.h` · `native/src/sim/traffic/ped_state.h` — veículos, meso e pools; o vocabulário do grafo de faixas (faixas dirigidas, conectores, controle do cruzamento); os pedestres (caminho, multidão, mobiliário como corpos) e a fatia de rota
- `native/src/sim/traffic/traffic_system.cpp` · `native/src/sim/traffic/traffic_system.h` — passo do tráfego, publicação de poses e rebuild
- `native/src/sim/traffic/vehicle_motion.cpp` — IDM e conectores
- `native/src/sim/traffic/window.cpp` — janela micro em volta da câmera e materialização do meso

**Render (só lê) — native/src/render**
- `native/src/render/agents/agent_cabin.cpp` · `native/src/render/agents/agent_parts.cpp` — cabine de qualquer veículo (forro com vãos de porta, painel, fileiras de bancos nos estilos carro/cabine/poltrona, volante, pessoas com a ordem de ocupação) e vidro à parte; peças comuns: cores, caixas e barras marcadas, painéis de vidro, rodas e casco oco com arcos
- `native/src/render/agents/agent_meshes.h` · `native/src/render/agents/agent_renderer.cpp` · `native/src/render/agents/agent_renderer.h` — malhas dos agentes (agents.json) em dois níveis (perto: cabine e vidro transparente; longe: sem cabine, vidro opaco), paletas, lotação e ordem dos assentos por veículo e MultiMesh por quadro
- `native/src/render/agents/car_body.cpp` · `native/src/render/agents/car_parts.cpp` · `native/src/render/agents/car_shape.cpp` · `native/src/render/agents/car_shape.h` — carroceria loftada do carro (faixas por região, normais suaves); pontas, peças e cabine; forma (linha do teto monótona, anel)
- `native/src/render/agents/bus_body.cpp` · `native/src/render/agents/heavy_meshes.cpp` — ônibus de entrada baixa (deque atrás, lugar da cadeira de rodas) pela NBR 15570; caminhão (cabine, baú, chassi) pela ficha; moto; peças dos pesados (pneu pela designação, rodas duplas)
- `native/src/render/agents/bicycle_mesh.cpp` — bicicleta urbana com ciclista (quadro de tubos, rodas, garfo, guidão)
- `native/src/render/agents/people_asset.cpp` · `native/src/render/agents/people_asset.h` — pessoas cozidas (people.bin)
- `native/src/render/agents/people_renderer.cpp` · `native/src/render/agents/people_renderer.h` — pedestres com esqueleto no shader e 2 níveis
- `native/src/render/atmosphere/fog_field.cpp` · `native/src/render/atmosphere/fog_field.h` · `native/src/render/atmosphere/canyon_field.cpp` · `native/src/render/atmosphere/canyon_field.h` — janelas da neblina pintada e das alturas dos prédios, escritas em job e lidas pelo passe de atmosfera
- `native/src/render/buildings/body_stage.cpp` · `native/src/render/buildings/body_stage.h` — um corpo desenhado à parte da cidade: o fantasma sob o ponteiro e as fotos dos modelos
- `native/src/render/buildings/building_bodies.cpp` · `native/src/render/buildings/building_bodies.h` · `native/src/render/buildings/building_look.cpp` · `native/src/render/buildings/building_look.h`
  — cache de corpos e colocação no mundo; buildings.json → frações dos níveis, bandas de peças e medidas das peças finas
- `native/src/render/buildings/building_renderer.cpp` · `native/src/render/buildings/building_renderer.h` · `native/src/render/buildings/building_renderer_reach.cpp` — streaming dos níveis near/mid/far por chunk e superchunk; o alcance de cada nível (dados, nível gráfico) e as faixas de visibilidade dos chunks
- `native/src/render/buildings/building_renderer_build.cpp` — montagem CPU dos níveis por subchunk e bandas de peças
- `native/src/render/buildings/building_renderer_commit.cpp` — upload ao RenderingServer dos níveis, peças e bandas
- `native/src/render/buildings/building_renderer_probe.cpp` — o que os testes leem dos níveis: o que a vista ainda pede, como está um chunk e o que cada superfície guarda (`building_cost`)
- `native/src/render/buildings/civic.h` — repasse para `native/src/sim/buildings/b40/civic.h` enquanto `native/src/render/trees/tree_place.cpp` (outra área) não troca o include
- `native/src/render/buildings/facade.h` — corpo colocado no mundo e emissores que o desenham
- `native/src/render/buildings/facade_bay.cpp` · `native/src/render/buildings/facade_bay.h` — abertura de uma bay, parede e peças
- `native/src/render/buildings/facade_emit.cpp` — paredes de cada volume, lado, andar e bay
- `native/src/render/buildings/facade_parts.cpp` — peças de cada componente de bay
- `native/src/render/buildings/facade_props.cpp` · `native/src/render/buildings/bay_props.cpp` · `native/src/render/buildings/bay_props.h` — o que pende do vão e fica na sacada, vão a vão por uma semente: condensador, antena, vasos com planta, cadeira, mesa, varal, espreguiçadeira e bicicleta; bay_props.json → um modelo de cada, desenhado por instância
- `native/src/render/buildings/far_mass.cpp` — nível distante de um corpo colocado
- `native/src/render/buildings/lot_emit.cpp` · `native/src/render/buildings/lot_parts.cpp` · `native/src/render/buildings/lot_props.cpp` — chão, muros, cercas e portões do lote; peças postas pelo jogador (escada, rampa, pedras, vaga, relógio); peças (vagas, docas, ralos, arbustos, flores, piscina, móveis, galpões, varal)
- `native/src/render/buildings/occluders.cpp` · `native/src/render/buildings/occluders.h` — as paredes que certamente existem, por chunk do nível perto, para a oclusão do motor
- `native/src/render/buildings/pane_rooms.cpp` · `native/src/render/buildings/pane_rooms.h` — o que cada vidro desenha: semente do cômodo, cômodo atrás dele pela planta do andar, maior móvel visto e caixilho
- `native/src/render/buildings/interior_emit.cpp` · `native/src/render/buildings/interior_emit.h` — o interior de um prédio cortado (dono único do desenho): piso do andar visto, poço do elevador com a cabine, paredes da planta que descem a 0,9 m do lado da câmera, móveis
- `native/src/render/buildings/interior_cut.cpp` — as paredes externas do andar visto: descem a um rodapé do lado da câmera, as outras ganham um plano de reboco por dentro sobre o vão inteiro, janela e porta incluídas, como no 4.0
- `native/src/render/buildings/furniture_emit.cpp` · `native/src/render/buildings/furniture_look.cpp` · `native/src/render/buildings/furniture_look.h` — um móvel do prédio cortado: as partes do modelo (caixas instanciadas, cilindros e esferas em geometria fina) postas no quadro da peça; furniture.json → paleta e modelos
- `native/src/render/buildings/interior_lamps.cpp` · `native/src/render/buildings/interior_lamps.h` — onde ficam, no mundo, as lâmpadas do andar visto (para as luzes de cômodo)
- `native/src/render/buildings/part_models.cpp` — modelos de peça de fachada no referencial da bay
- `native/src/render/buildings/placed.cpp` — consultas de geometria e tinta de um corpo colocado
- `native/src/render/buildings/roof_emit.cpp` — telhados de um corpo colocado
- `native/src/render/buildings/roof_gear.cpp` — o que o jogador põe no telhado: painéis, claraboias, chaminés, caixas, pináculo e lanterna
- `native/src/render/buildings/roof_clip.cpp` · `native/src/render/buildings/roof_clip.h` — o telhado inclinado de um volume baixo recortado onde um volume mais alto do corpo aberto o encerra (só no corte; pedaços convexos exatos no plano das paredes)
- `native/src/render/buildings/roof_pitched.cpp` — telhados inclinados e dente de serra
- `native/src/render/buildings/roof_plant.cpp` — equipamentos de cobertura
- `native/src/render/buildings/roof_terrace.cpp` — terraços usados
- `native/src/render/buildings/roof_util.h` — predicados de telhado compartilhados
- `native/src/render/buildings/shell.cpp` · `native/src/render/buildings/shell.h` — escrita de vértices das malhas de prédio
- `native/src/render/chunk_jobs.h` · `native/src/render/chunk_queue.h` · `native/src/render/mesh_queue.h` — jobs por chunk, conversão no orçamento de envio e commit orçado; chunks ao redor da câmera; fila de uma superfície
- `native/src/render/effects/effect_world.cpp` · `native/src/render/effects/effect_world.h` — dono dos efeitos da explosão: lê os eventos dos golpes no ChangeLog, desenha crateras e fogos do estado, relógio próprio, clarão (luz), tremor da câmera, gêiseres
- `native/src/render/effects/effect_blast.cpp` · `native/src/render/effects/effect_things.cpp` · `native/src/render/effects/effect_ruins.cpp` · `native/src/render/effects/effect_bodies.cpp` — o corpo de quem foi atingido, como peças rígidas (cabeça, tronco, braços, pernas na pele e roupa do pedestre; membro arrancado voa sozinho; some quando a pessoa se levanta), o traçante, a ferida e o carro atingido; o monte que um prédio derrubado deixa (cai dentro dele e assenta; o do estado é posto direto, igual toda vez); os eventos tocados uma vez: bola de fogo, fumaça, poeira, estilhaços, anel de onda, fuligem, prédio atingido e caído; mobiliário jogado, veículos explodidos, sangue e corpo dilacerado
- `native/src/render/effects/effect_params.cpp` · `native/src/render/effects/effect_params.h` — data/render/effects.json estrito (aparência dos efeitos)
- `native/src/render/effects/mm_layer.h` · `native/src/render/effects/puffs.cpp` · `native/src/render/effects/puffs.h` — um MultiMesh de capacidade fixa; a reserva circular de fumaça, poeira, chama, faísca, sangue, água e papel (o shader move cada uma pela idade)
- `native/src/render/effects/marks.cpp` · `native/src/render/effects/marks.h` — marcas no chão em um MultiMesh: crateras do estado, fuligem e sangue guardados, anel de onda
- `native/src/render/effects/debris.cpp` · `native/src/render/effects/debris.h` · `native/src/render/effects/debris_step.inl` — peças rígidas jogadas (caixas e cilindros): gravidade, impulso por canto, repouso; carcaças que queimam
- `native/src/render/mesh_builder.cpp` · `native/src/render/mesh_builder.h` — emissores de primitivas e envio de malha
- `native/src/render/overlays/overlay_build.cpp` — malhas CPU das camadas, uma por chunk de 256 m
- `native/src/render/overlays/overlay_renderer.cpp` · `native/src/render/overlays/overlay_renderer.h` — jobs das zonas e ciclo do congestionamento
- `native/src/render/owned_segments.h` — segmentos que um chunk possui (pelo ponto médio)
- `native/src/render/props/prop_kit.cpp` · `native/src/render/props/prop_kit.h` — malhas do mobiliário a partir dos dados
- `native/src/render/props/prop_renderer.cpp` · `native/src/render/props/prop_renderer.h` · `native/src/render/props/signal_lamps.cpp` · `native/src/render/props/signal_lamps.h`
  — chunks do mobiliário; lentes dos semáforos, cada uma desenhada uma vez (um MultiMesh por chunk, acesa ou apagada pelo estado publicado)
- `native/src/render/props/street_props.cpp` · `native/src/render/props/street_props.h` — o mobiliário posto pela simulação no mundo: postes, fios, semáforos (um por acesso), árvores
- `native/src/render/render_layers.h` — o bit de cada renderizador: o painel de efeitos e os níveis gráficos escondem uma camada pela máscara da câmera
- `native/src/render/film_grade.cpp` · `native/src/render/film_grade.h` — gradação de filme da imagem na tela como tabela 3D (contraste em S, vibrance, viragem de cor)
- `native/src/render/region_table.h` — o estilo regional de uma tabela (`styles.<id>`), lido num lugar só
- `native/src/render/render_context.h` — cenário, tabelas de render e materiais recebidos pelos renderizadores
- `native/src/render/render_world.cpp` · `native/src/render/render_world.h` — consome o ChangeLog e aplica os orçamentos por quadro
- `native/src/render/roads/road_emit.cpp` — referenciais de segmento, nós e quad texturizado
- `native/src/render/roads/road_junction.cpp` · `native/src/render/roads/road_kerb.cpp` — placas de nó, beco sem saída e balão; a borda com meio-fio de uma esquina ou balão
- `native/src/render/roads/road_look.cpp` · `native/src/render/roads/road_look.h` — streets.json → StreetLook (aparência das vias)
- `native/src/render/roads/road_markings.cpp` — pintura viária como geometria fina elevada
- `native/src/render/roads/road_mesh.h` — peças do malhador de vias
- `native/src/render/roads/road_chunks.h` · `native/src/render/roads/road_renderer.cpp` · `native/src/render/roads/road_renderer.h` — chunks de via por segmento e nó
- `native/src/render/roads/road_verge.cpp` · `native/src/render/roads/verge_renderer.cpp` · `native/src/render/roads/verge_renderer.h` — o talude: faixa do fim da calçada até o terreno, por chunk, na grama do terreno
- `native/src/render/roads/road_section.cpp` · `native/src/render/roads/road_section.h` — a seção da via em peças: superfícies, rampa do rebaixo e ilha entre as pistas
- `native/src/render/roads/road_segment.cpp` — superfície do segmento: as peças da seção entre as bocas, com a ponta da ilha antes da travessia
- `native/src/render/wear/wear_field.cpp` · `native/src/render/wear/wear_field.h` — desgaste das ruas e calçadas: campo em janela que dá a volta, em camadas de uma textura em array, só as alteradas enviadas
- `native/src/render/rs_util.h` — instâncias RenderingServer sem nós de cena
- `native/src/render/cover/cover_kit.cpp` · `native/src/render/cover/cover_kit.h` · `native/src/render/cover/cover_renderer.cpp` · `native/src/render/cover/cover_renderer.h` · `native/src/render/cover/element_models.cpp` · `native/src/render/cover/element_models.h` · `native/src/render/cover/shore_stones.cpp` · `native/src/render/cover/shore_stones.h` — o que fica no chão aberto (pedras, arbustos, cactos, tufos e flores), as pedras da margem e do leito e os elementos do pincel: malhas de cover.json; colocação pela tinta e pela ecologia, modelos glTF e MultiMesh por chunk
- `native/src/render/water/shore_field.cpp` · `native/src/render/water/shore_field.h` · `native/src/render/water/water_renderer.cpp` · `native/src/render/water/water_renderer.h` — o nível da água ao lado de cada terra, para a praia; superfície dos lagos e rios acima do mar, por chunk
- `native/src/render/terrain_bridge.cpp` · `native/src/render/terrain_bridge.h` — API de scripting do Terrain3D
- `native/src/render/transit/transit_parts.cpp` · `native/src/render/transit/transit_parts.h` · `native/src/render/transit/transit_renderer.cpp` · `native/src/render/transit/transit_renderer.h` — caixas do trilho, da estação, do abrigo e da entrada do metrô a partir de transit.json; malhas por item (só o que mudou, poucas por quadro), corte do metrô, prévia do trilho e vagões dos trens
- `native/src/render/trees/linear_mips.cpp` · `native/src/render/trees/linear_mips.h` — mipmaps de uma imagem sRGB sobre fundo transparente, tirados na luz linear
- `native/src/render/trees/leaf_atlas.cpp` · `native/src/render/trees/leaf_atlas.h` · `native/src/render/trees/tree_kit.cpp` · `native/src/render/trees/tree_kit.h` · `native/src/render/trees/tree_part.h` · `native/src/render/trees/tree_parts.cpp` · `native/src/render/trees/tree_parts.h` · `native/src/render/trees/tree_pictures.cpp` · `native/src/render/trees/tree_pictures.h` — folhas e espécies (corpo facetado, cartões, frondes); retratos de cada espécie por elevação para o nível distante
- `native/src/render/trees/far_cards.cpp` · `native/src/render/trees/far_cards.h` · `native/src/render/trees/tree_look.cpp` · `native/src/render/trees/tree_place.cpp` · `native/src/render/trees/tree_place.h` · `native/src/render/trees/tree_renderer.cpp` · `native/src/render/trees/tree_renderer.h` — manchas de mato, parques e ruas; modelos perto por chunk, cartões longe numa malha por superchunk

**Godot — project/**
- `project/game/camera/camera_gestures.gd` — gestos do mouse (arrastar gira/inclina, meio ou Shift+direito move o chão)
- `project/game/camera/camera_motion.gd` — teclas e roda com suavização
- `project/game/camera/camera_pull.gd` — o olho nunca dentro nem atrás de prédio, com os tempos do Deoccluder
- `project/game/camera/camera_profile.gd` — lente, inclinação e altura do olho pela distância
- `project/game/camera/camera_shake.gd` — o tremor da câmera pelo relógio dos efeitos (a bomba), desfeito quando passa
- `project/game/camera/city_camera.gd` — rig da câmera (orbitar sobre um ponto do chão)
- `project/game/interior/interior_controller.gd` — a camada Interior no lado do script: o interruptor (menu de camadas), o prédio no meio da vista ou de dois cliques, o andar, a direção da câmera em 8 passos e as luzes; como no 4.0 a câmera fica onde o jogador pôs; relê o núcleo só quando a revisão muda
- `project/game/data/data_tables.gd` — único leitor de `data/**`; junta as partes de road_profiles e strings
- `project/game/flow/game_flow.gd` — máquina de estados da aplicação (única dona de troca de tela, pausa e saída)
- `project/game/flow/game_keys.gd` · `project/game/flow/build_stamp.gd` — as teclas do jogo (regras nos dados); o carimbo da versão
- `project/game/health/health_log.gd` — registro de saúde: erros do motor (Logger), quadros longos, sessão anterior
- `project/game/play.gd` · `project/game/play.tscn` — cena de jogo: mundo, câmera, HUD e captura por linha de comando
- `project/game/settings/settings.gd` — opções gráficas persistidas em user://settings.cfg (predefinição, escolhas, limite de quadros, vsync, tela cheia, idioma, chaves), aplicadas ao vivo
- `project/game/settings/quality.gd` · `project/game/settings/frame_governor.gd` · `project/game/settings/render_switches.gd` — um nível gráfico no renderizador; governador do nível automático e resolução dinâmica; chaves do painel de efeitos
- `project/game/store/city_store.gd` — único dono do CityCore no lado de script (`snapshot`)
- `project/game/store/store_api.gd` — os nomes de comando e de consulta que o script pode mandar ao CityCore (tabelas lidas pelo store)
- `project/game/store/city_saves.gd` — a cidade no disco: salvar, abrir, autosave, ao perder o foco e ao fechar, arquivo de fora
- `project/game/store/formatters.gd` — formatação de números/dinheiro (dono de `format_money`)
- `project/game/store/model_library.gd` — os modelos do jogador: um arquivo cada, valem para toda cidade
- `project/game/store/save_slots.gd` — onde as cidades são salvas: um arquivo por cidade, cópia do mapa, índice da última, quarentena e pasta dos testes
- `project/game/store/session.gd` — o que a cidade traz além do mapa: câmera, velocidade e níveis da simulação
- `project/game/tools/brush_defaults.gd` · `project/game/tools/brush_tool.gd` — valores de um pincel da Paisagem (land_panels.json); traço com dabs espaçados
- `project/game/tools/element_tool.gd` · `project/game/tools/cloud_tool.gd` · `project/game/tools/fog_tool.gd` · `project/game/tools/paint_tool.gd` · `project/game/tools/tree_tool.gd` · `project/game/tools/weather_tool.gd` — pincel de elementos; Céu (nuvens, neblina, clima), Solo e Natureza
- `project/game/tools/building_tool.gd` · `project/game/tools/lot_tool.gd` — Construir: o clique escolhe bloco, face e vão e as ferramentas da gaveta agem na escolha; Construir › Desenhar
- `project/game/tools/demolish_tool.gd` · `project/game/tools/strike_tool.gd` · `project/game/tools/shoot_tool.gd` — demolir; bomba (um clique, força do parâmetro `strength`, o controle da aba Bomba) e pistola (um tiro pelo raio da câmera); a aba Demolir chama `set_tool(STRIKE|SHOOT, ...)`
- `project/game/tools/road_tool.gd` · `project/game/tools/roundabout_tool.gd` · `project/game/tools/split_tool.gd` · `project/game/tools/upgrade_tool.gd` · `project/game/tools/control_tool.gd` · `project/game/tools/move_tool.gd` — via reta/curva com prévia; rotatória; Dividir; Melhorar; Controle; Mover
- `project/game/tools/inspect_tool.gd` · `project/game/tools/terrain_tool.gd` · `project/game/tools/zone_tool.gd` · `project/game/tools/transit_tool.gd` — inspecionar; Relevo; pincel de zona; parada, terminal, trilho, estação e linha
- `project/game/tools/tool_base.gd` · `project/game/tools/tool_controller.gd` — base das ferramentas (recebe pontos do chão); máquina de estados das ferramentas
- `project/render/effect_materials.gd` — o que os materiais dos efeitos da bomba pedem ao tone mapping do Environment: exposição e branco do inverso do ACES nas fumaças, cores da cratera como radiância
- `project/render/city_world.gd` — Terrain3D, céu, sol, água e materiais (inclui meios das texturas e parâmetros da água)
- `project/render/city_water.gd` · `project/render/change_marks.gd` — o plano da água; F8, as últimas edições marcadas no mapa
- `project/render/daylight.gd` · `project/render/daylight_rig.gd` — dia/noite; as luzes do dia feitas uma vez (sol com sombras, luzes planas do céu e do chão)
- `project/render/transit_overlay.gd` — o plano do transporte sobre o mapa com a ferramenta ligada
- `project/render/sky_view.gd` · `project/render/thunder.gd` · `project/render/weather_view.gd` — céu desenhado: nuvens levadas pelo vento, globais do vento e tempo fechado; trovão; chuva e raios
- `project/render/shore_view.gd` — entrega ao terreno os níveis da água ao lado da terra
- `project/render/element_effects.gd` — emissores dos efeitos do pincel de elementos
- `project/render/film_look.gd` · `project/render/shaders/vignette.gdshader` — aspecto da imagem depois do tom: tabela da gradação no Environment e vinheta sob a interface
- `project/render/model_studio.gd` — as fotos da prateleira de modelos, uma por quadro
- `project/render/selection_box.gd` — contorno do bloco escolhido no Construir
- `project/render/surface_means.gd` — média de cada mapa de superfície e tons da via, enviados uma vez ao material
- `project/render/lamp_radiance.gd` — o que uma lente ou farol aceso precisa irradiar para aparecer na sua cor pelo tone mapping
- `project/render/interior_lights.gd` — as luzes de cômodo do andar visto: seis luzes de ponto sem sombra sobre as primeiras lâmpadas do andar, com os valores do 4.0 (0,25 de dia a 1 à noite; `project/data/game/interior.json`)
- `project/render/preview_line.gd` · `project/render/tone_map.gd` · `project/render/shaders/color_space.gdshaderinc` — prévias das ferramentas; o ACES do Godot e o inverso (lentes acesas); espaço de cor de cada renderizador (shaders em linear nos dois)
- `project/render/shaders/people_color.gdshaderinc` · `project/render/shaders/people_skin.gdshaderinc` — a pessoa cozida em qualquer shader que a desenhe (andando em `project/render/shaders/people.gdshader`): códigos do visual, cor de pele, cabelo e roupa; as quatro bones por vértice com a pose de cada uma como três linhas de matriz
- `project/render/shaders/agent_glass.gdshader` · `project/render/shaders/agents.gdshader` · `project/render/shaders/people.gdshader` · `project/render/shaders/people_blob.gdshader` · `project/render/shaders/props.gdshader` — vidro dos veículos, veículos e ocupantes, pedestres cozidos, mancha sob quem está longe, mobiliário, árvores e lentes dos semáforos
- `project/render/shaders/building_ghost.gdshader` — o fantasma do prédio antes de colocar
- `project/render/shaders/building.gdshader` · `project/render/shaders/building_far.gdshader` · `project/render/shaders/building_parts.gdshader` · `project/render/shaders/building_common.gdshaderinc` · `project/render/shaders/building_colour.gdshaderinc` · `project/render/shaders/building_hash.gdshaderinc` · `project/render/shaders/building_lamps.gdshaderinc` · `project/render/shaders/building_glass.gdshader` · `project/render/shaders/building_sheer.gdshader` · `project/render/shaders/building_card.gdshader` · `project/render/shaders/building_props.gdshader` · `project/render/shaders/building_pane.gdshaderinc` · `project/render/shaders/building_far_window.gdshaderinc` · `project/render/shaders/building_sky_glass.gdshaderinc`
  — prédio, massas distantes/HLOD, peças de fachada instanciadas; ruído, acabamento e intemperismo; espaço de cor (entra e sai como cada renderizador passa); hash; lâmpadas e grade filtrada de janelas; guarda-corpos de vidro meio vistos através; grades como uma face, as barras no fragmento; o mobiliário de sacada e a frente do condensador; vidros do nível perto com luz própria e o que cada um desenha (vão com sol e céu exatos, cômodo por interior mapping, cortina, caixilho); janelas do nível médio; o céu no vidro
- `project/render/shaders/wear.gdshaderinc` — o desgaste lido pelas vias: trilhas, buracos, rachaduras e manchas de pisada
- `project/render/shaders/transit_glass.gdshader` · `project/render/shaders/transit_xray.gdshader` — vidro do transporte; o metrô em corte, através do chão
- `project/render/shaders/transit.gdshader` · `project/render/shaders/road.gdshader` · `project/render/shaders/road_detail.gdshaderinc` · `project/render/shaders/road_wear.gdshaderinc` · `project/render/shaders/water.gdshader` · `project/render/shaders/lattice_noise.gdshaderinc` — vias, meios-fios e pintura; camada de detalhe de perto das vias; sujeira e desgaste da pista e da calçada; água (ondulação do espectro de vento, reflexo pelo
  modelo do renderizador); ruído de valor por hash inteiro do terreno, da água e das vias
- `project/render/shaders/atmosphere.gdshader` · `project/render/shaders/canyon_light.gdshaderinc` · `project/render/shaders/cloud_field.gdshaderinc` · `project/render/shaders/ground_paint.gdshaderinc` · `project/render/shaders/lightning.gdshader` · `project/render/shaders/rain.gdshader` — passe de tela cheia (sombra das nuvens, céu que a rua perde entre os prédios, névoa, neblina, nuvens) e forma da nuvem; chão pintado e ravinas pelo mapa de controle; raio e chuva
- `project/render/shaders/verge.gdshader` · `project/render/shaders/overlay.gdshader` · `project/render/shaders/shore.gdshaderinc` · `project/render/shaders/sky.gdshader` · `project/render/shaders/terrain.gdshader` · `project/render/shaders/terrain_layers.gdshaderinc` — camadas de zonas e congestionamento, margem (praia, barranco e faixa molhada pelo nível da água), céu em gradiente, terreno toon (override do Terrain3D; ruído, camadas triplanares e grama); `project/render/shaders/vegetation.gdshader` · `project/render/shaders/vegetation_cards.gdshader` · `project/render/shaders/vegetation.gdshaderinc` — árvores: corpo, cartões, vento e troca perto/longe
- `project/render/shaders/tone_inverse.gdshaderinc` — a radiância de cena que o ACES do Environment mostra como uma cor de tela (o inverso do `project/render/tone_map.gd` no shader; o que é desenhado sem luz com cor de tela, como as fumaças e chamas)
- `project/render/shaders/puffs.gdshader` · `project/render/shaders/debris.gdshader` · `project/render/shaders/marks.gdshader` — fumaça, chama, faísca, sangue, água e papel movidos pela idade na GPU; peças jogadas; crateras, fuligem, sangue e anel de onda no chão
- `project/tests/e2e/e2e.gd` · `project/tests/e2e/e2e.tscn` — ponta a ponta: mundo + HUD reais, chunks desenhados, frota e buffers de instância
- `project/tests/core/camera_input_test.gd` · `project/tests/core/camera_input_test.tscn` · `project/tests/core/light_color_test.gd` · `project/tests/core/light_color_test.tscn` · `project/tests/core/building_light_test.gd` · `project/tests/core/building_light_test.tscn` — câmera por entrada real; luzes da rua na tela (lentes, brilho noturno, luz das vias) e cor e luz dos prédios, nos dois renderizadores
- `project/tests/perf/camera_pan.gd` · `project/tests/perf/camera_pan.tscn` — tempos de quadro com pan, giro e zoom sobre a cidade e rente às paredes; triângulos e bytes dos prédios por superfície
- `project/tests/core/transit_test.gd` · `project/tests/core/transit_test.tscn` — paradas, linha, ônibus, trem, desfazer e save do transporte
- `project/tests/core/building_edit_test.gd` · `project/tests/core/building_edit_test.tscn` · `project/tests/core/building_part_cases.gd` · `project/tests/core/building_pick_cases.gd` · `project/tests/core/building_tool_test.gd` · `project/tests/core/building_tool_test.tscn` — edições do Construir pelo store e pela ferramenta: desfazer ao corpo gerado, refazer aos mesmos bytes, recusas e ida e volta do save; o que um raio da câmera acerta
- `project/tests/core/road_commands_test.gd` · `project/tests/core/road_commands_test.tscn` — comandos de via/zona/demolir pelo store
- `project/tests/core/people_test.gd` · `project/tests/core/people_test.tscn` — quem anda a pé: portas, idades, pares, criança acompanhada e gente por hora
- `project/tests/core/road_edit_test.gd` · `project/tests/core/road_edit_test.tscn` — opções da via de classe, Mover, Controle e regras de edição
- `project/render/shaders/water_plain.gdshader` — a água numa cor só, para a aba Otimizações ver o que o sombreamento dela custa
- `project/tests/perf/tier_gpu.gd` · `project/tests/perf/tier_gpu.tscn` — o que cada nível gráfico custa à GPU nas três vistas: tempo medido em amostras e contagens
- `project/tests/core/shell_test.gd` · `project/tests/core/shell_test.tscn` — níveis gráficos, governador, chaves de render, painéis e Esc
- `project/tests/core/quality_test.gd` · `project/tests/core/quality_test.tscn` · `project/tests/core/inspector_test.gd` · `project/tests/core/inspector_test.tscn` — níveis gráficos lidos no que é desenhado; inspetor, edições de via, nó impossível, arquivo ruim, quarentena e sessão
- `project/tests/core/job_pool_test.gd` · `project/tests/core/job_pool_test.tscn` — a regra da fila de jobs: chaves de usuários diferentes convivem, a mais nova substitui a mesma
- `project/tests/core/model_test.gd` · `project/tests/core/model_test.tscn` — Modelos pelo store e pela ferramenta: catálogo, encaixe, recusas, desfazer, fusão, biblioteca e foto
- `project/tests/core/city_mass_test.gd` · `project/tests/core/city_mass_test.tscn` — a massa da cidade gerada: andares por distância do miolo
- `project/tests/core/yard_test.gd` · `project/tests/core/yard_test.tscn` — pátios de zona industrial em várias direções: docas com caminho, caminhão na própria faixa e um ciclo inteiro
- `project/tests/core/save_undo_test.gd` · `project/tests/core/save_undo_test.tscn` · `project/tests/core/signal_cases.gd` — desfazer/refazer e ida e volta do save; o semáforo ajustável pelo store e pelo painel
- `project/tests/visual/capture.gd` · `project/tests/visual/capture.tscn` · `project/tests/visual/capture_views.gd` — capturas da cidade real e suas vistas fixas · `project/tests/visual/capture_kerb.gd` · `project/tests/visual/capture_lorry.gd` · `project/tests/visual/capture_parity.gd` · `project/tests/visual/capture_memory.gd` · `project/tests/visual/capture_signals.gd` · `project/tests/visual/capture_soak.gd` · `project/tests/visual/capture_traffic.gd` · `project/tests/visual/capture_wait.gd` — rebaixo da guia, caminhão na doca, par de uma foto do 4.0, memória, semáforos, tráfego (soak) e lado parado do tráfego das capturas; a espera de toda foto até a cidade estar desenhada, registrada, com aviso de foto incompleta
- `project/tests/visual/capture_features.gd` · `project/tests/visual/capture_features.tscn` — capturas de recursos dirigidas como o jogador
- `project/tests/visual/capture_clock.gd` · `project/tests/visual/capture_clock.tscn` — a luz no jogo de verdade: botões sol, lua e relógio de play.tscn
- `project/tests/visual/capture_landscape.gd` · `project/tests/visual/capture_landscape.tscn` · `project/tests/visual/landscape_cards.gd` — capturas da paisagem nos ângulos do 4.0; rampa de cinzas, cubo cinza e árvores plantadas, medidos sozinhos
- `project/tests/core/interior_test.gd` · `project/tests/core/interior_test.tscn` — a camada Interior pelo store: o andar limitado ao topo, prédio que não existe recusado, lâmpadas, custo do interior estável e a vista em volta de um ponto
- `project/tests/visual/capture_interior.gd` · `project/tests/visual/capture_interior.tscn` — capturas da camada Interior dos prédios, dirigidas pelo controlador como o jogador (prédio, andar, volta, escala e hora por tabela)
- `project/tests/core/strike_test.gd` · `project/tests/core/strike_test.tscn` — a bomba pela store: curva K-B contra o ponto conhecido, prédios que caem aos poucos, via cortada pela cratera, mobiliário derrubado, desfazer e refazer
- `project/tests/visual/capture_blast.gd` — a bomba fotografada pelo relógio dos efeitos (nome do arquivo = instante real) e pela câmera do 4.0
- `project/tests/visual/capture_demolition.gd` · `project/tests/visual/capture_demolition.tscn` — capturas da bomba (casa, rua, terra, bomba grande) em instantes depois do golpe
- `project/tests/core/shoot_test.gd` · `project/tests/core/shoot_test.tscn` — a pistola pela store (cabeça mata, tronco fere, o ferido sangra, tiro no céu erra, rajada faz o carro explodir) e o que um golpe deixa num save: ida e volta de crateras, ruínas e mobiliário
- `project/tests/visual/capture_vehicles.gd` — capturas de perto do ônibus, do caminhão, da moto, da van e de um sedã (vidro, cabine, pessoas e pontas)
- `project/tests/visual/capture_roads.gd` · `project/tests/visual/capture_roads.tscn` — vias de perto em chão livre (cruzamentos, T, ponta, curva) e a ferramenta em uso, nos ângulos das fotos do 4.0
- `project/tests/visual/capture_transit.gd` · `project/tests/visual/capture_transit_site.gd` — parada, estação e trilho nas capturas; a rua com o outro lado livre, onde valem as poses do 4.0
- `project/tests/visual/capture_people.gd` — pessoas nas capturas: o casal, a família, a criança, o idoso, quem sai de uma porta, quem entra e quem espera na guia, como as fotos do 4.0, e uma multidão por baixo das sombras reais
- `project/tests/visual/capture_pair.gd` · `project/tests/visual/capture_pair.tscn` — fotos no enquadramento das do 4.0: uma lista de vistas serve aos dois; com `--buildings=` os próprios prédios do 4.0, na pose lida dele, e uma vista com `inside` corta um deles por dentro
- `project/tests/visual/pair_camera.gd` — a câmera exata de uma foto do 4.0: pose e lente
- `project/tests/visual/capture_ui.gd` · `project/tests/visual/capture_ui.tscn` · `project/tests/visual/capture_shell.gd` · `project/tests/visual/capture_shell.tscn` — capturas da interface por cliques; a casca inteira nas vistas das fotos do 4.0
- `project/tests/visual/ui_driver.gd` — entrada real (mouse/teclas) para as capturas
- `project/ui/builder_strip.gd` · `project/ui/model_shelf.gd` — seções e cartões das abas do Construir; prateleira de Modelos (busca, categorias, fotos, salvar e remover)
- `project/ui/drawer_strips.gd` — cartões e controles das gavetas
- `project/ui/hud_actions.gd` — menu, desfazer/refazer e atalhos
- `project/ui/hud_drawer.gd` · `project/ui/zone_strip.gd` — gavetas das categorias; a aba Zonas (cartões de uso e a densidade)
- `project/ui/hud_popover.gd` · `project/ui/pop_bodies.gd` · `project/ui/hud_switch.gd` — camadas, câmera e menu; corpos da simulação e da ajuda; o interruptor das camadas
- `project/ui/hud_top_bar.gd` · `project/ui/compass.gd` · `project/ui/start_screen.gd` · `project/ui/hud_dialog.gd` · `project/ui/graphics_panel.gd` — barra de cima; bússola (clique e arrasto); tela inicial e cortina; diálogo modal e Sobre; opções gráficas
- `project/ui/effects_panel.gd` · `project/ui/health_panel.gd` · `project/ui/profiler_hud.gd` — painel de efeitos F7 (chaves e otimizações); monitor F9; perfil Shift+F9
- `project/ui/hud_tip.gd` · `project/ui/hud_widgets.gd` — a dica própria (centrada sob o controle, só se o nome não está na tela); fábricas de botões/rótulos do tema
- `project/ui/inspector_panel.gd` · `project/ui/junction_panel.gd` — inspetor; tempos do semáforo e onda verde
- `project/ui/inspector_rows.gd` · `project/ui/selection_overlay.gd` — os números ao vivo do inspetor, como linhas; a faixa da via e o anel do cruzamento que a ferramenta Informação marca sobre o mapa
- `project/ui/land_hands.gd` — as mãos da gaveta Paisagem: cartões que redesenham o painel, ações do mapa inteiro e o Relevo pelas teclas
- `project/ui/land_strips.gd` — painéis da gaveta Paisagem a partir de land_panels.json: cartões na gaveta; pincel, sliders e o que age no mapa inteiro no painel de opções
- `project/ui/transit_strip.gd` — linhas e seus veículos, na gaveta do transporte
- `project/ui/interior_bar.gd` — a barra do andar da camada Interior (canto de cima à direita): o botão que a fecha, andar abaixo, nome do andar, andar acima; só aparece com um prédio aberto
- `project/ui/road_catalog.gd` — catálogo de vias
- `project/ui/road_options.gd` — opções da ferramenta de via (traçado, encaixe)
- `project/ui/junction_marks.gd` · `project/ui/preview_tag.gd` · `project/ui/road_swatch.gd` — marcas do controle de cada cruzamento na aba Controle; etiqueta de comprimento, preço e recusa junto do fim da via em mão; figura do perfil visto de cima
- `project/ui/roadcraft_hud.gd` · `project/ui/hud_messages.gd` — a casca: dock e quem abre cada painel; a dica da ferramenta e o aviso que passa sozinho
- `project/ui/theme/theme_builder.gd` · `project/ui/theme/theme_text.gd` · `project/ui/theme/theme_art.gd` — único dono dos estilos (tokens → Theme): caixas e botões; textos e pesos; desenhos (botão do controle deslizante, amostra de zona, vinheta)
- `project/ui/icons/**` — ícones SVG do Roadcraft (brancos; a cor vem do tema)
- `project/data/catalog/growth.json` · `project/data/catalog/maps.json` · `project/data/catalog/nature.json` · `project/data/catalog/road_profiles.json` · `project/data/catalog/road_profiles_a.json` · `project/data/catalog/road_profiles_b.json` · `project/data/catalog/street_furniture.json` · `project/data/catalog/traffic.json` · `project/data/catalog/tuning.json` · `project/data/catalog/blast.json` · `project/data/catalog/zones.json` — catálogo: crescimento, mapas, paisagem, perfis de via, mobiliário da rua e travessias, tráfego, tuning e zonas
- `project/data/game/camera.json` · `project/data/game/daylight.json` · `project/data/game/interior.json` · `project/data/game/saves.json` · `project/data/game/settings.json` · `project/data/game/sky.json` · `project/data/game/render_switches.json` · `project/data/game/optimizations.json` · `project/data/game/health.json` · `project/data/game/start.json` — câmera, dia/noite, saves, qualidade e céu desenhado
- `project/data/i18n/strings.csv` · `project/data/i18n/strings_b.csv` · `project/data/i18n/strings_c.csv` · `project/data/i18n/strings_d.csv` · `project/data/i18n/strings_e.csv` · `project/data/i18n/strings_ui.csv` — textos (partes do mesmo catálogo)
- `project/data/render/transit.json` · `project/data/render/agents.json` · `project/data/render/bay_props.json` · `project/data/render/buildings.json` · `project/data/render/cover.json` · `project/data/render/interiors.json` · `project/data/render/furniture.json` · `project/data/render/materials.json` · `project/data/render/models.json` · `project/data/render/overlays.json` · `project/data/render/palette.json` · `project/data/render/effects.json` · `project/data/render/streets.json` — tabelas do render
- `project/data/ui/builder.json` · `project/data/ui/drawers.json` · `project/data/ui/hud.json` · `project/data/ui/land_elements.json` · `project/data/ui/land_panels.json` · `project/data/ui/tokens.json` · `project/data/ui/toolbar.json` · `project/data/ui/keys.json` · `project/data/ui/help.json` — tabelas da interface
- `tools/gates/schemas/**` — um schema por tabela de dados

**Ferramentas — tools/**
- `tools/assets/bleed_alpha.py` · `tools/assets/cook_people.py` · `tools/assets/people_clean.py` · `tools/assets/export_builder_ui.py` · `tools/assets/ktx2_to_png.mjs` · `tools/assets/make_blue_noise.py` · `tools/assets/make_road_profiles.py` — a cor sob os furos de uma textura recortada; pessoas cozidas
  (MakeHuman CC0 + clipes Rocketbox MIT), pele da cabeça e da cintura, ícones e textos do Construir, KTX2 do Roadcraft → PNG, ruído azul do início dos raios da atmosfera, road_profiles.json da fonte do Roadcraft
- `tools/assets/make_furniture.py` — furniture.json: do que cada móvel é feito, parte por parte, nos tamanhos de interiors.json
- `tools/build/build.sh` · `tools/build/run_tests.sh` · `tools/build/hidden_run.ps1` — compilar, testar (todas as cenas, com tempo-limite), capturar, exportar e empacotar; rodar o jogo num desktop do Windows à parte (testes e capturas nunca abrem janela na tela do usuário)
- `tools/gates/gate_clones.py` · `tools/gates/gate_code.py` · `tools/gates/gate_data.py` · `tools/gates/gate_defs.py` —
  código duplicado; camadas, padrões proibidos e órfãos; schemas; números mágicos, cópias de dados, donos e estado na UI
- `tools/gates/gate_docs.py` · `tools/gates/gate_files.py` · `tools/gates/gate_tuning.py` — mapa deste documento; lixo,
  documento único, tamanhos e cabeçalhos; tuning ↔ JSON e espelhos
- `tools/gates/owners.json` · `tools/gates/rules.json` · `tools/gates/run_gates.py` — donos únicos, configuração, executa todos
<!-- MAPA:FIM -->

## Pendências
Backlog de recursos do Bloco E do roteiro (não faz parte do porte de desempenho B/C/D, que está concluído e testado).
- Interior (vista de dentro): o corte, o piso, o poço do elevador, as paredes da planta, as janelas por dentro e os móveis existem, com a câmera, o alvo (um prédio, nunca um raio) e as luzes de cômodo como no 4.0; faltam o meio-nível (`Volume.lift`, lances entre blocos: o modelo da Porte 2 não tem `lift`), a escada de núcleo (`Core` não tem tipo), as arestas arredondadas dos móveis e a largura variada dos livros, o interior de prédios assinatura e a barra do andar dentro da barra de cima (no 4.0 ela é parte dela; aqui é um painel no canto). A cabine do elevador não tem fonte primária (NBR NM 313 não é aberta). A conversão das luzes (energia 55·0,4²/π, atenuação 2) é derivada das fórmulas do three.js e do Godot, não medida. O piso (madeira ou porcelana) e os móveis dependem do id do prédio, e o id do prédio posto a partir de um documento do 4.0 não é o do 4.0. A aba Interior do Construir (Porte 2) ainda não liga a camada (contrato em `ports/interior/relatorio.md`).
- Porte build: o que falta está em `ports/build/cobertura.md` (estado `a portar`) e o plano em `ports/build/plano.md`. O corpo editável e as abas Modelos, Massa, Fachada, Peças, Telhado e Cores existem, com seleção por raio; faltam Desenhar e Interior. Os 38 lotes vêm com o porte `marcos`. Bandeiras no pináculo e na lanterna ainda não. Árvores de corpo colocado já são desenhadas; as dos lotes crescidos ainda não. a peça não tem fantasma sob o ponteiro.
- Sacadas: `decay` não chega ao mobiliário (idade 0, como os 441 prédios lidos do 4.0), então o padrão popular e o condensador amarelado não aparecem. Pátio do prédio posto sobre relevo sem nivelar: peças de `hung` viram colunas (`lot_emit.cpp`), causa a medir. Painel preto opaco numa sacada do par, causa não vista. Cartões e guarda-corpos de vidro ainda geram draw calls na sombra.
- Noite: as paredes ficam ~5x mais escuras que no 4.0 (mediana de luminância da foto 0,004 contra 0,019) e as janelas distantes se fundem em blocos maiores; o desgaste não tem foto de trilha fina (célula de 5 m pega a faixa inteira).
- Árvores: forma, tons e atlas seguem o 4.0 no mesmo quadro (`capture_landscape --only=arv_perto,...`), escolha de modelagem dita no _doc de `trees`; as matas seguem a mesma regra e a mesma malha do 4.0 (mesma árvore na mesma célula: `tree_place.cpp`), e passar a instância sem cor quando o `rs_util.h` da rua integrar. Paisagem: a luz é a do 4.0 no mesmo sol (16h, 29°), medida em discos pintados de albedo igual, e os mapas de chão entram sem ganho; a imagem passa pela gradação de filme (`film_grade.cpp`) e pela vinheta. Falta a sombra de contato do `canyonLight` do 4.0 e a luz do céu fora do terreno; pedras da costa, estratos da rocha, grama seca da ecologia e lembrar a escolha dos pincéis não foram portados.
- Água: a praia segue o nível da água de cada lugar (`ShoreField`); a terra acaba numa reta sem praia, noutro lugar que no 4.0, e o mar fica mais claro. Sombra das árvores: no 4.0 só as plantadas lançam (fotos; o código dele manda lançar em todas), no jogo também as nativas. Alvo de cor vem de uma foto antiga do 4.0 (o mapa 0 não tem lago). `noise2d` da via ainda tem o hash com degraus em Forward+ (`lattice_noise` é o certo).
- Pontes têm tabuleiro e pilares, mas não há vão estaiado nem rodovia elevada sobre terra (viadutos).
- Ocupantes, motociclista e ciclista: ainda de blocos. Desenho decidido: uma instância por ocupante no MultiMesh dos pedestres, pelve no ponto H do assento, até `cabin_radius_m`, fileiras de trás até 1/1,6 disso; faltam os clipes sentados e de pilotar em `people.bin`.
- Transporte: ônibus param na faixa sem baia e sem embarque (o `kerb_stops` ainda não atende ônibus); sem cancelas desenhadas nas passagens (nem o 4.0 tem); edição de via reencontra as faixas de todas as paradas e passagens, não só as da área; paradas e trilhos procurados em lista, sem índice por célula; a prévia do trilho é opaca e refeita a cada movimento do ponteiro; passagem fecha a 160 m (valor do 4.0: 7 s a 22 m/s; 49 CFR 234.225 pede 20 s). Escolas com alunos não são simuladas. Veículos: portas e vidros não abrem e quem vai dentro não se mexe (o 4.0 anima).
- Contorno, iluminação e shaders validados em Forward+ na RTX 3060 (capturas em `dist/captures`, incluindo HLOD
  distante e peças de fachada); falta conferir em GPU AMD/Intel e à noite com o jogo em movimento contínuo.
- `grass.detail` e `grass.detail-normal` sem mipmaps, e o shader pede mipmaps: importar com eles e reajustar os `detail_*` de `palette.json` contra o 4.0 (cintilado de longe). `Grass.png` sem mipmaps: fica pálido de longe.
- Paisagem, não feito: sombra de contato do `canyonLight` (o passe de tela não lê o atlas de sombra, então portar como está escureceria a sombra duas vezes); prova do hemisfério (laje vista de baixo nos dois jogos); longe da chuva menos azul que no 4.0, causa não provada; pedras da margem mais claras que as do 4.0; par com o 4.0 da cor dos elementos de longe. Vídeo em `parada`, 1600x900, médio: 437 MiB (base `a11384a` 547, com 146 a mais de buffers); texturas +34 e staging +3 sobre a base.
- Memória (Forward+ 1600x900, cidade toda desenhada): 10 km, 3.169 prédios — 185 MiB estática, 507 de vídeo, 308 de buffers;
  24 km, 457.714 prédios — 601 estática, 1.435 de vídeo, 793 de buffers (camera_pan parada; nenhum quadro > 50 ms, GPU ≤ 44 %;
  maior HLOD de superchunk 8,1 MiB).
- Interface: o painel de efeitos só traz as chaves do que já existe (nuvens, clima, partículas e transporte vêm com as sessões donas); o laboratório de objetos do 4.0 não tem par.
- Lotes: árvores do lote (`b40::lot_trees`) esperam `render/trees/tree_place.cpp` (e o include do repasse `render/buildings/civic.h`);
  sem lote de esquina e garagem em encosta; escritórios do mapa 0 (29-30 m, praça 5-7 m) sem fundo para estacionamento.
- Estilo regional: o seletor da tela inicial, `CityState::style` e as unidades dos formatadores existem, com um só estilo (`roadcraft`); o bloco no save (hoje vai no arquivo da sessão) e a leitura única pelo render e pela simulação esperam o mecanismo. O catálogo de vias ainda escreve "m" no texto de `strings`.
- Interface "em construção" (abas sem ferramenta, desenhadas assim): Quarteirões (Zonas), Interior (Construir) e Rua (Paisagem). Salvar é
  um arquivo por cidade (sem as gerações do Roadcraft; o arquivo que não abre vai para a quarentena); o autosave não grava cidade gerada
  que o jogador não editou nem salvou (o Roadcraft grava e poda as não tocadas).
- Rotatória: sem fundir trecho curto até a junção (Roadcraft < 14 m), sem placas de dê a preferência nos acessos.
- Oclusão: o GTAO do 4.0 tira 12 níveis do meio-fio ao sol e o SSAO do Godot, 1 a 2 em qualquer ajuste (sem oclusão os dois dão 193): falta um passe equivalente; o preset padrão do jogo (`medium`) nem liga a oclusão, e o 4.0 abre em `high`.
- Vias do jogador: altura segue o chão (sem greide de menor terraplenagem, pontes/túneis, rampa íngreme, muro; o talude leva a grama do terreno, sem o que foi pintado no chão);
  Quarteirões ainda não; demolir via deixa o corredor de terreno. Falta do porte de vias: Personalizar e Meus modelos, editor de perfil, teclas, regras por acesso, conectores, nomes de rua, legendas, vazios, e larguras de faixa e calçada, zebra de escada e nomes de rua por estilo (em `streets.json` `markings.styles`, na ordem do menu Pintura: `br`, o desenho do 4.0 e o padrão, `us`, `eu`, `classic` e, por último, `br_norm`, a norma do MBST IV; o mapa guarda o índice), linha contínua na aproximação de semáforo (MBST IV 4.2.1), comparar a geração com `streets.ts`/`plan.ts`; a prévia não avisa de trecho apertado; parada, preferência e minirrotatória rodam como cruzamento de prioridade; mover recusa um nó cujo outro lado já era fechado.
- Tráfego: o mapa 0 não tem indústria (zona industrial só perto de via expressa), logo nenhuma doca até o jogador pintar a zona (`yard_test` pinta); no pátio de 12 m o caminhão pode entrar ou sair de ré pela rua, e 1 doca em 137 fica sem caminho mesmo no grão fino; semáforos atuados pelos veículos (MBST V cap. 7) sem botão de pedestre, coordenação só por plano fixo (onda verde num sentido, verdes partindo do mínimo, sem fluxo medido); sem controle automático do cruzamento pelo fluxo medido (o 4.0: flowStats e controlAdvisor; aqui o semáforo vem da geometria do nó, control_of); rotas
  síncronas no tick (sem threads). Medir com `capture.tscn -- --soak=5` (retidos e paradas no cruzamento por segundo, chegadas).
- Carros de lote: 115 de 175 raspam algo no caminho, mediana 8 cm (muro em 91), pior 83 cm (pilar, picape em lote pequeno): a curva de Hermite segue o centro e o canto da carroceria abre na curva dos acessos estreitos. Solução: o A* híbrido do pátio (way_search) também para os carros, no worker. Medir com `capture.tscn -- --only=trafego`.
- Mobiliário da rua: postes opostos em rua de 8–12 m (NPIP-1 aceita alternados, sem pool em `road.gdshader`); peças sem instâncias (+19 MiB de vídeo).
- Pedestres: sem passageiros de ônibus, celular, guarda-chuva, sentados; o clipe `pedal` está cozido e sem uso até a bicicleta da simulação chegar aqui; o rosto é da cor da pele, sem traços, e a barra da roupa é a do pacote cozido; a curva `people_busy` não tem pesquisa por trás. Os 38 projetos de marcos do Roadcraft (`data/lots-roadcraft`) e o estádio ainda
  não foram portados; muros de divisa entre prédios colados ainda não são desenhados.
- Demolição (bomba, pistola, demolir): o prédio atingido vira um monte de lajes, não é fraturado em Voronoi com a casca e as janelas dele (`b40::Shell` num job do `JobPool`); o corpo de quem cai são peças rígidas (cabeça, tronco, braços, pernas) sem juntas, sem Jolt, sem a malha cozida da pessoa, sem levantar nem rastejar animados, e some 90 s depois; barriga aberta e vísceras não são desenhadas; corpos já caídos não levam tiro de novo (só quem ainda está na calçada, ou caído com vida); os fios dos postes derrubados não balançam; a bomba cai no chão sob o cursor (o telhado pede o raio contra as caixas dos prédios); árvores naturais fora das ruas e parques não caem com a bomba (`render/trees/tree_place.cpp` é de outra área); a caixa do demolir não tem prévia; as constantes de projeto sem norma estão no `_doc` de `blast.json` (resposta da estrutura, k da cratera, carga, pessoas, tiro); a norma CONTRAN 210/2006 da largura da pista está citada no `_doc` de `blast.json`; a aba Demolir/Pistola/Bomba da interface (Porte 4) deve chamar `set_tool(STRIKE|SHOOT, {strength})` e `demolish_box`. Fogo da bomba (medido contra o 4.0 em 0,4 / 1,2 / 4,0 s, mesma câmera): cobertura e núcleo batem até 1,2 s; a chama do jogo sai ≈8% mais pálida (o 4.0 escreve a cor da tela com corte por camada, o jogo passa o HDR pelo ACES); o anel de choque aparece, mas o do 4.0 é maior (2,4× o raio da explosão, o do jogo vai ao alcance de colapso); a cratera tem as cores do 4.0 passadas pelo inverso do ACES (`display_colors` em `materials.json`, `_set_crater_radiance` em `city_world.gd`) e fica ≈30% mais escura que a dele (com fumaça por cima, não separável); o disco visível da cratera (buraco, lábio e terra revolvida) tem ≈22,5 m de raio na força 45 (medido numa foto de cima a 120 m), o do 4.0 20–23 m; o buraco físico é 15,9 m na 45 e 3,7 m na 5, o do 4.0 3 m e 20 m (a lei dele é linear na força, a do jogo segue ∛kg); o decalque da cratera e o anel ficam na altura mais alta do chão sob eles (`Marks::top_ground`) e toda marca é puxada 0,5 m ao olho no shader; fumaça, poeira e sangue passam pelo inverso do ACES (`project/render/shaders/tone_inverse.gdshaderinc`), chama e faísca não (o inverso de um laranja saturado dá manchas verde-limão); os estilhaços são pontinhos (o 4.0 lança lajes); `puffs` tem `render_priority` 1 para ser desenhado depois das marcas.
