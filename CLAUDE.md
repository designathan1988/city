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
- `GameFlow.State`: BOOT → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram/saem de STOPPED.
- `ToolController.Tool`: NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT, MOVE, CONTROL, PAINT, TREES, CLOUD, FOG, WEATHER, ELEMENTS, BUILD (só `set_tool`; botão segurado → `drag`/`tick`, solto → `release`); `RoadTool.Step`: START → CONTROL (curva) → END → (encadeia) END.

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
- `native/src/api/catalog_loader.cpp` · `native/src/api/catalog_loader.h` · `native/src/api/furniture_loader.cpp` — Dictionary → Catalog estrito; as X-macros ditam as chaves (tuning, mobiliário da rua)
- `native/src/api/city_core.cpp` · `native/src/api/city_core.h` — ciclo de vida: catálogo, render, mapa novo, velocidade, sincronização por quadro
- `native/src/api/city_core_bind.cpp` — bindings de script do CityCore
- `native/src/api/city_core_commands.cpp` — comandos do jogador e prévias (sim/commands) como API de script
- `native/src/api/city_core_body.cpp` — o corpo de um prédio para o Construir: o que ele tem e as edições como comandos de script
- `native/src/api/city_core_edit.cpp` — desfazer/refazer, traços e edições das gavetas (dividir, melhorar, rotatória, relevo, prédio desenhado)
- `native/src/api/city_core_persist.cpp` — salvar/abrir: encode sob trava curta, ZSTD e gravação em thread própria
- `native/src/api/city_core_query.cpp` · `native/src/api/city_core_probe.cpp` · `native/src/api/city_core_jobs.cpp` — leituras para a UI e os testes (snapshot, altura, raycast); sondas: custo do quadro, chegada das poses, tráfego e a regra da fila de jobs
- `native/src/api/edit_result.h` — EditResult como Dictionary e `run_edit` (comando dentro de um passo de undo)
- `native/src/api/city_core_nature.cpp` · `native/src/api/nature_loader.cpp` — gaveta Paisagem como API de script (clima, nuvens, neblina, pintura, árvores, bioma); nature.json → NatureDef estrito
- `native/src/api/interiors_loader.cpp` · `native/src/api/interiors_loader.h` — interiors.json → tabelas dos interiores
- `native/src/api/city_core_models.cpp` · `native/src/api/models_loader.cpp` · `native/src/api/models_loader.h` — Construir › Modelos: catálogo, corpo em mãos como bytes, encaixe, colocar e desenhar à parte; models.json → catálogo
- `native/src/api/register_types.cpp` — entrada da GDExtension: registra CityCore
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
- `native/src/sim/catalog/catalog.h` · `native/src/sim/catalog/furniture_types.h` — tipos do catálogo (sem valores); mobiliário da rua e travessia
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
- `native/src/sim/commands/body_edit.cpp` · `native/src/sim/commands/body_facade.cpp` · `native/src/sim/commands/body_finish.cpp` · `native/src/sim/commands/body_mass.cpp` · `native/src/sim/commands/body_ops.h` · `native/src/sim/commands/body_parts.cpp` — o único caminho de uma edição até o corpo (cópia, validação, diário) e o que um corpo precisa ser; abas Fachada, Telhado, Cores, Massa e Peças
- `native/src/sim/commands/body_pick.cpp` · `native/src/sim/commands/body_pick.h` — o que um raio da câmera acerta: registro, bloco, face, andar e vão
- `native/src/sim/commands/commands.h` — API de comandos do jogador
- `native/src/sim/commands/edit_history.cpp` — pilhas de desfazer/refazer e limites
- `native/src/sim/commands/edit_journal.h` — diário de edições (único mutador dos comandos)
- `native/src/sim/commands/journal_ops.cpp` — operações `j_*` do diário
- `native/src/sim/commands/land_commands.cpp` — relevo, prédio desenhado e edição de prédio
- `native/src/sim/commands/element_commands.cpp` — pincel de elementos com desfazer
- `native/src/sim/commands/nature_commands.cpp` — clima, nuvens, neblina, pintura, árvores plantadas, bioma e relevo de volta ao original
- `native/src/sim/commands/model_ops.h` · `native/src/sim/commands/model_body.cpp` · `native/src/sim/commands/model_place.cpp` · `native/src/sim/commands/place_snap.cpp` · `native/src/sim/commands/body_weld.cpp` — modelo feito corpo; colocar (recusas e fusão, um passo de desfazer); encaixe na rua, no vizinho e na grade; dois prédios feitos um
- `native/src/sim/commands/road_commands.cpp` — construir e demolir vias
- `native/src/sim/commands/road_edit.cpp` · `native/src/sim/commands/road_edit.h` — passos comuns das edições de via
- `native/src/sim/commands/road_plan.cpp` — encaixe e validação de vias (= prévia)
- `native/src/sim/commands/road_path.cpp` · `native/src/sim/commands/road_snap.cpp` — o gesto vira peças (reta, curva, traço à mão) e o plano e a obra delas; onde a via pousa (rede, ângulos, grade)
- `native/src/sim/commands/road_move.cpp` — Mover: o nó vai e as vias são assentadas de novo
- `native/src/sim/commands/road_rules.cpp` — o que uma edição de via não pode deixar: trecho apertado demais entre duas junções
- `native/src/sim/commands/road_tabs.cpp` — Dividir e Melhorar
- `native/src/sim/commands/roundabout.cpp` — rotatória
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
- `native/src/sim/persistence/save_nature.cpp` — gravar e abrir a paisagem
- `native/src/sim/persistence/save_read.cpp` — abrir: terreno do seed, ladrilhos esculpidos, modificadores e grafo
- `native/src/sim/persistence/save_write.cpp` — gravar: cabeçalho, terreno, modificadores, vias e prédios
- `native/src/sim/roads/geometry.cpp` · `native/src/sim/roads/geometry.h` — geometria pura das vias (Bézier, tabelas, interseções)
- `native/src/sim/roads/junction.cpp` · `native/src/sim/roads/junction.h` — braços, recuos, esquinas e balões
- `native/src/sim/roads/profiles.cpp` · `native/src/sim/roads/profiles.h` — derivação do perfil (faixas, linhas, calçadas, preço)
- `native/src/sim/roads/road_network.cpp` · `native/src/sim/roads/road_network.h` — armazenamento e consultas do grafo viário
- `native/src/sim/roads/road_variants.cpp` · `native/src/sim/roads/road_variants.h` — via de classe com as faixas, a largura e as vagas das opções da ferramenta, um perfil a mais no catálogo
- `native/src/sim/roads/street_furniture.cpp` · `native/src/sim/roads/street_furniture.h` · `native/src/sim/roads/street_furniture_posts.cpp` · `native/src/sim/roads/street_furniture_internal.h` — dono único de onde fica o mobiliário da rua (hidrantes, lixeiras, bancos, árvores; sempre com passagem para quem anda), lido pelos pedestres e pelo render; postes e semáforos; zonas da calçada
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
- `native/src/sim/traffic/junction_control.cpp` · `native/src/sim/traffic/signal_timing.cpp` · `native/src/sim/traffic/light_rule.cpp` — controle de cruzamento, estágios e preferência; plano no tempo pelo MBST V (entreverdes, luz, janela de pedestres); regra única do semáforo para os veículos (âmbar: para quem ainda para na linha como o entreverdes supõe, mantido até o verde) e a parada que cada um vê pela rota
- `native/src/sim/traffic/lane_change.cpp` — MOBIL
- `native/src/sim/traffic/lane_geometry.cpp` — pontos de faixa, conector e calçada
- `native/src/sim/traffic/lane_graph.cpp` — grafo de faixas e conectores com conflitos (build completo e por nó)
- `native/src/sim/traffic/lane_graph_edit.cpp` — rebuild incremental das faixas e nós sujos
- `native/src/sim/traffic/lane_graph_internal.h` — superfície interna do grafo de faixas
- `native/src/sim/traffic/lane_list.cpp` — listas por faixa/conector e lista micro de veículos
- `native/src/sim/traffic/meso.cpp` — modelo meso distante (uma fila por faixa)
- `native/src/sim/traffic/ped_crossing.cpp` · `native/src/sim/traffic/ped_crowd.cpp` — pedestres e carros nas travessias; multidão: quem vê quem (listas por calçada e por cruzamento, as "walking areas" do SUMO) e os corpos remapeados no trecho de cada um
- `native/src/sim/traffic/ped_furniture.cpp` · `native/src/sim/traffic/ped_kerb.cpp` · `native/src/sim/traffic/ped_stripes.cpp` · `native/src/sim/traffic/ped_stripes.h` — mobiliário como corpos nas calçadas; espera na guia; faixas do SUMO em calçada, esquina e faixa
- `native/src/sim/traffic/ped_routes.cpp` · `native/src/sim/traffic/pedestrians.cpp` — rotas a pé e caminho de esquina; pedestres com destino
- `native/src/sim/traffic/route_arena.cpp` — arena de rotas (RouteRef) reutilizada
- `native/src/sim/traffic/routing.cpp` — rotas de carro por segmento dirigido, desvio e reconsideração
- `native/src/sim/traffic/traffic_state.h` · `native/src/sim/traffic/ped_state.h` — grafo de faixas, veículos, meso e pools; os pedestres (caminho, multidão, mobiliário como corpos) e a fatia de rota
- `native/src/sim/traffic/traffic_system.cpp` · `native/src/sim/traffic/traffic_system.h` — passo do tráfego, publicação de poses e rebuild
- `native/src/sim/traffic/vehicle_motion.cpp` — IDM e conectores
- `native/src/sim/traffic/window.cpp` — janela micro em volta da câmera e materialização do meso

**Render (só lê) — native/src/render**
- `native/src/render/agents/agent_cabin.cpp` · `native/src/render/agents/agent_parts.cpp` — cabine de qualquer veículo (forro com vãos de porta, painel, fileiras de bancos nos estilos carro/cabine/poltrona, volante, pessoas com a ordem de ocupação) e vidro à parte; peças comuns: cores, caixas e barras marcadas, painéis de vidro, rodas e casco oco com arcos
- `native/src/render/agents/agent_meshes.h` · `native/src/render/agents/agent_renderer.cpp` · `native/src/render/agents/agent_renderer.h` — malhas dos agentes (agents.json) em dois níveis (perto: cabine e vidro transparente; longe: sem cabine, vidro opaco), paletas, lotação e ordem dos assentos por veículo e MultiMesh por quadro
- `native/src/render/agents/car_body.cpp` · `native/src/render/agents/car_parts.cpp` · `native/src/render/agents/car_shape.cpp` · `native/src/render/agents/car_shape.h` — carroceria loftada do carro (faixas por região, normais suaves); pontas, peças e cabine; forma (linha do teto monótona, anel)
- `native/src/render/agents/bus_body.cpp` · `native/src/render/agents/heavy_meshes.cpp` — ônibus de entrada baixa (deque atrás, lugar da cadeira de rodas) pela NBR 15570; caminhão (cabine, baú, chassi) pela ficha; moto; peças dos pesados (pneu pela designação, rodas duplas)
- `native/src/render/agents/people_asset.cpp` · `native/src/render/agents/people_asset.h` — pessoas cozidas (people.bin)
- `native/src/render/agents/people_renderer.cpp` · `native/src/render/agents/people_renderer.h` — pedestres com esqueleto no shader e 2 níveis
- `native/src/render/atmosphere/fog_field.cpp` · `native/src/render/atmosphere/fog_field.h` — janela da neblina pintada, escrita em job e lida pelo passe de atmosfera
- `native/src/render/buildings/body_stage.cpp` · `native/src/render/buildings/body_stage.h` — um corpo desenhado à parte da cidade: o fantasma sob o ponteiro e as fotos dos modelos
- `native/src/render/buildings/building_bodies.cpp` · `native/src/render/buildings/building_bodies.h` · `native/src/render/buildings/building_look.cpp` · `native/src/render/buildings/building_look.h`
  — cache de corpos e colocação no mundo; buildings.json → frações dos níveis, bandas de peças e medidas das peças finas
- `native/src/render/buildings/building_renderer.cpp` · `native/src/render/buildings/building_renderer.h` — streaming dos níveis near/mid/far por chunk e superchunk
- `native/src/render/buildings/building_renderer_build.cpp` — montagem CPU dos níveis por subchunk e bandas de peças
- `native/src/render/buildings/building_renderer_commit.cpp` — upload ao RenderingServer dos níveis, peças e bandas
- `native/src/render/buildings/building_renderer_probe.cpp` — o que os testes leem dos níveis: o que a vista ainda pede e como está um chunk
- `native/src/render/buildings/civic.h` — repasse para `native/src/sim/buildings/b40/civic.h` enquanto `native/src/render/trees/tree_place.cpp` (outra área) não troca o include
- `native/src/render/buildings/facade.h` — corpo colocado no mundo e emissores que o desenham
- `native/src/render/buildings/facade_bay.cpp` · `native/src/render/buildings/facade_bay.h` — abertura de uma bay, parede e peças
- `native/src/render/buildings/facade_emit.cpp` — paredes de cada volume, lado, andar e bay
- `native/src/render/buildings/facade_parts.cpp` — peças de cada componente de bay
- `native/src/render/buildings/facade_props.cpp` — AC, antenas e vasos dos andares altos
- `native/src/render/buildings/far_mass.cpp` — nível distante de um corpo colocado
- `native/src/render/buildings/lot_emit.cpp` · `native/src/render/buildings/lot_parts.cpp` · `native/src/render/buildings/lot_props.cpp` — chão, muros, cercas e portões do lote; peças postas pelo jogador (escada, rampa, pedras, vaga, relógio); peças (vagas, docas, ralos, arbustos, flores, piscina, móveis, galpões, varal)
- `native/src/render/buildings/pane_rooms.cpp` · `native/src/render/buildings/pane_rooms.h` — o que cada vidro desenha: semente do cômodo, cômodo atrás dele pela planta do andar, maior móvel visto e caixilho
- `native/src/render/buildings/part_models.cpp` — modelos de peça de fachada no referencial da bay
- `native/src/render/buildings/placed.cpp` — consultas de geometria e tinta de um corpo colocado
- `native/src/render/buildings/roof_emit.cpp` — telhados de um corpo colocado
- `native/src/render/buildings/roof_gear.cpp` — o que o jogador põe no telhado: painéis, claraboias, chaminés, caixas, pináculo e lanterna
- `native/src/render/buildings/roof_pitched.cpp` — telhados inclinados e dente de serra
- `native/src/render/buildings/roof_plant.cpp` — equipamentos de cobertura
- `native/src/render/buildings/roof_terrace.cpp` — terraços usados
- `native/src/render/buildings/roof_util.h` — predicados de telhado compartilhados
- `native/src/render/buildings/shell.cpp` · `native/src/render/buildings/shell.h` — escrita de vértices das malhas de prédio
- `native/src/render/chunk_jobs.h` · `native/src/render/chunk_queue.h` · `native/src/render/mesh_queue.h` — jobs por chunk, conversão no orçamento de envio e commit orçado; chunks ao redor da câmera; fila de uma superfície
- `native/src/render/mesh_builder.cpp` · `native/src/render/mesh_builder.h` — emissores de primitivas e envio de malha
- `native/src/render/overlays/overlay_build.cpp` — malhas CPU das camadas, uma por chunk de 256 m
- `native/src/render/overlays/overlay_renderer.cpp` · `native/src/render/overlays/overlay_renderer.h` — jobs das zonas e ciclo do congestionamento
- `native/src/render/owned_segments.h` — segmentos que um chunk possui (pelo ponto médio)
- `native/src/render/props/prop_kit.cpp` · `native/src/render/props/prop_kit.h` — malhas do mobiliário a partir dos dados
- `native/src/render/props/prop_renderer.cpp` · `native/src/render/props/prop_renderer.h` · `native/src/render/props/signal_lamps.cpp` · `native/src/render/props/signal_lamps.h`
  — chunks do mobiliário; lentes dos semáforos, cada uma desenhada uma vez (um MultiMesh por chunk, acesa ou apagada pelo estado publicado)
- `native/src/render/props/street_props.cpp` · `native/src/render/props/street_props.h` — o mobiliário posto pela simulação no mundo: postes, fios, semáforos (um por acesso), árvores
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
- `native/src/render/cover/cover_kit.cpp` · `native/src/render/cover/cover_kit.h` · `native/src/render/cover/cover_renderer.cpp` · `native/src/render/cover/cover_renderer.h` · `native/src/render/cover/element_models.cpp` · `native/src/render/cover/element_models.h` — o que fica no chão aberto (pedras, arbustos, cactos, tufos e flores) e os elementos do pincel: malhas de cover.json; colocação pela tinta e pela ecologia, modelos glTF e MultiMesh por chunk
- `native/src/render/water/water_renderer.cpp` · `native/src/render/water/water_renderer.h` — superfície dos lagos e rios acima do mar, por chunk
- `native/src/render/terrain_bridge.cpp` · `native/src/render/terrain_bridge.h` — API de scripting do Terrain3D
- `native/src/render/trees/linear_mips.cpp` · `native/src/render/trees/linear_mips.h` — mipmaps de uma imagem sRGB sobre fundo transparente, tirados na luz linear
- `native/src/render/trees/leaf_atlas.cpp` · `native/src/render/trees/leaf_atlas.h` · `native/src/render/trees/tree_kit.cpp` · `native/src/render/trees/tree_kit.h` · `native/src/render/trees/tree_part.h` · `native/src/render/trees/tree_parts.cpp` · `native/src/render/trees/tree_parts.h` · `native/src/render/trees/tree_pictures.cpp` · `native/src/render/trees/tree_pictures.h` — folhas e espécies (corpo facetado, cartões, frondes); retratos de cada espécie por elevação para o nível distante
- `native/src/render/trees/far_cards.cpp` · `native/src/render/trees/far_cards.h` · `native/src/render/trees/tree_look.cpp` · `native/src/render/trees/tree_place.cpp` · `native/src/render/trees/tree_place.h` · `native/src/render/trees/tree_renderer.cpp` · `native/src/render/trees/tree_renderer.h` — manchas de mato, parques e ruas; modelos perto por chunk, cartões longe numa malha por superchunk

**Godot — project/**
- `project/game/camera/camera_gestures.gd` — gestos do mouse (arrastar gira/inclina, meio ou Shift+direito move o chão)
- `project/game/camera/camera_motion.gd` — teclas e roda com suavização
- `project/game/camera/camera_profile.gd` — lente, inclinação e altura do olho pela distância
- `project/game/camera/city_camera.gd` — rig da câmera (orbitar sobre um ponto do chão)
- `project/game/data/data_tables.gd` — único leitor de `data/**`; junta as partes de road_profiles e strings
- `project/game/flow/game_flow.gd` — máquina de estados da aplicação (única dona de troca de tela, pausa e saída)
- `project/game/play.gd` · `project/game/play.tscn` — cena de jogo: mundo, câmera, HUD e captura por linha de comando
- `project/game/settings/settings.gd` — qualidade gráfica persistida em user://settings.cfg e limite de quadros (menor fora de foco)
- `project/game/store/city_store.gd` — único dono do CityCore no lado de script (`snapshot`)
- `project/game/store/formatters.gd` — formatação de números/dinheiro (dono de `format_money`)
- `project/game/store/model_library.gd` — os modelos do jogador: um arquivo cada, valem para toda cidade
- `project/game/store/save_slots.gd` — onde as cidades são salvas: um arquivo por cidade, cópia do mapa, índice da última e pasta dos testes
- `project/game/tools/brush_defaults.gd` · `project/game/tools/brush_tool.gd` — valores de um pincel da Paisagem (land_panels.json); traço com dabs espaçados
- `project/game/tools/element_tool.gd` — pincel de elementos
- `project/game/tools/cloud_tool.gd` · `project/game/tools/fog_tool.gd` · `project/game/tools/paint_tool.gd` · `project/game/tools/tree_tool.gd` · `project/game/tools/weather_tool.gd` — Céu (nuvens, neblina, clima), Solo e Natureza
- `project/game/tools/building_tool.gd` — Construir: o clique escolhe bloco, face e vão; as ferramentas da gaveta agem na escolha
- `project/game/tools/demolish_tool.gd` — demolir
- `project/game/tools/inspect_tool.gd` — inspecionar
- `project/game/tools/lot_tool.gd` — Construir › Desenhar
- `project/game/tools/road_tool.gd` — via reta/curva com prévia
- `project/game/tools/control_tool.gd` — Controle
- `project/game/tools/move_tool.gd` — Mover
- `project/game/tools/roundabout_tool.gd` — rotatória
- `project/game/tools/split_tool.gd` — Dividir
- `project/game/tools/terrain_tool.gd` — Relevo
- `project/game/tools/tool_base.gd` — base das ferramentas (recebe pontos do chão)
- `project/game/tools/tool_controller.gd` — máquina de estados das ferramentas
- `project/game/tools/upgrade_tool.gd` — Melhorar
- `project/game/tools/zone_tool.gd` — pincel de zona
- `project/render/city_world.gd` — Terrain3D, céu, sol, água e materiais (inclui meios das texturas e parâmetros da água)
- `project/render/daylight.gd` — dia/noite
- `project/render/sky_view.gd` · `project/render/thunder.gd` · `project/render/weather_view.gd` — céu desenhado: nuvens levadas pelo vento, globais do vento e tempo fechado; trovão; chuva e raios
- `project/render/element_effects.gd` — emissores dos efeitos do pincel de elementos
- `project/render/film_look.gd` · `project/render/shaders/vignette.gdshader` — aspecto da imagem depois do tom: tabela da gradação no Environment e vinheta sob a interface
- `project/render/model_studio.gd` — as fotos da prateleira de modelos, uma por quadro
- `project/render/selection_box.gd` — contorno do bloco escolhido no Construir
- `project/render/surface_means.gd` — média de cada mapa de superfície e tons da via, enviados uma vez ao material
- `project/render/preview_line.gd` · `project/render/tone_map.gd` · `project/render/shaders/color_space.gdshaderinc` — prévias das ferramentas; o ACES do Godot e o inverso (lentes acesas); espaço de cor de cada renderizador (shaders em linear nos dois)
- `project/render/shaders/agent_glass.gdshader` · `project/render/shaders/agents.gdshader` · `project/render/shaders/people.gdshader` · `project/render/shaders/props.gdshader` — vidro dos veículos, veículos e ocupantes, pedestres cozidos, mobiliário, árvores e lentes dos semáforos
- `project/render/shaders/building_ghost.gdshader` — o fantasma do prédio antes de colocar
- `project/render/shaders/building.gdshader` · `project/render/shaders/building_far.gdshader` · `project/render/shaders/building_parts.gdshader` · `project/render/shaders/building_common.gdshaderinc` · `project/render/shaders/building_colour.gdshaderinc` · `project/render/shaders/building_hash.gdshaderinc` · `project/render/shaders/building_lamps.gdshaderinc` · `project/render/shaders/building_glass.gdshader` · `project/render/shaders/building_sheer.gdshader` · `project/render/shaders/building_card.gdshader` · `project/render/shaders/building_pane.gdshaderinc` · `project/render/shaders/building_far_window.gdshaderinc` · `project/render/shaders/building_sky_glass.gdshaderinc`
  — prédio, massas distantes/HLOD, peças de fachada instanciadas; ruído, acabamento e intemperismo; espaço de cor (entra e sai como cada renderizador passa); hash; lâmpadas e grade filtrada de janelas; guarda-corpos de vidro meio vistos através; grades como uma face, as barras no fragmento; vidros do nível perto com luz própria e o que cada um desenha (vão com sol e céu exatos, cômodo por interior mapping, cortina, caixilho); janelas do nível médio; o céu no vidro
- `project/render/shaders/wear.gdshaderinc` — o desgaste lido pelas vias: trilhas, buracos, rachaduras e manchas de pisada
- `project/render/shaders/road.gdshader` · `project/render/shaders/road_detail.gdshaderinc` · `project/render/shaders/road_wear.gdshaderinc` · `project/render/shaders/water.gdshader` · `project/render/shaders/lattice_noise.gdshaderinc` — vias, meios-fios e pintura; camada de detalhe de perto das vias; sujeira e desgaste da pista e da calçada; água (ondulação do espectro de vento, reflexo pelo
  modelo do renderizador); ruído de valor por hash inteiro do terreno, da água e das vias
- `project/render/shaders/atmosphere.gdshader` · `project/render/shaders/cloud_field.gdshaderinc` · `project/render/shaders/ground_paint.gdshaderinc` · `project/render/shaders/lightning.gdshader` · `project/render/shaders/rain.gdshader` — passe de tela cheia (sombra das nuvens, névoa, neblina, nuvens) e forma da nuvem; chão pintado e ravinas pelo mapa de controle; raio e chuva
- `project/render/shaders/verge.gdshader` · `project/render/shaders/overlay.gdshader` · `project/render/shaders/sky.gdshader` · `project/render/shaders/terrain.gdshader` · `project/render/shaders/terrain_layers.gdshaderinc` — camadas de zonas e congestionamento, céu em gradiente, terreno toon (override do Terrain3D; ruído, camadas triplanares e grama); `project/render/shaders/vegetation.gdshader` · `project/render/shaders/vegetation_cards.gdshader` · `project/render/shaders/vegetation.gdshaderinc` — árvores: corpo, cartões, vento e troca perto/longe
- `project/tests/e2e/e2e.gd` · `project/tests/e2e/e2e.tscn` — ponta a ponta: mundo + HUD reais, chunks desenhados, frota e buffers de instância
- `project/tests/core/camera_input_test.gd` · `project/tests/core/camera_input_test.tscn` · `project/tests/core/light_color_test.gd` · `project/tests/core/light_color_test.tscn` · `project/tests/core/building_light_test.gd` · `project/tests/core/building_light_test.tscn` — câmera por entrada real; luzes da rua na tela (lentes, brilho noturno, luz das vias) e cor e luz dos prédios, nos dois renderizadores
- `project/tests/perf/camera_pan.gd` · `project/tests/perf/camera_pan.tscn` — tempos de quadro com pan, giro e zoom sobre a cidade
- `project/tests/core/building_edit_test.gd` · `project/tests/core/building_edit_test.tscn` · `project/tests/core/building_part_cases.gd` · `project/tests/core/building_pick_cases.gd` · `project/tests/core/building_tool_test.gd` · `project/tests/core/building_tool_test.tscn` — edições do Construir pelo store e pela ferramenta: desfazer ao corpo gerado, refazer aos mesmos bytes, recusas e ida e volta do save; o que um raio da câmera acerta
- `project/tests/core/road_commands_test.gd` · `project/tests/core/road_commands_test.tscn` — comandos de via/zona/demolir pelo store
- `project/tests/core/road_edit_test.gd` · `project/tests/core/road_edit_test.tscn` — opções da via de classe, Mover, Controle e regras de edição
- `project/tests/core/job_pool_test.gd` · `project/tests/core/job_pool_test.tscn` — a regra da fila de jobs: chaves de usuários diferentes convivem, a mais nova substitui a mesma
- `project/tests/core/model_test.gd` · `project/tests/core/model_test.tscn` — Modelos pelo store e pela ferramenta: catálogo, encaixe, recusas, desfazer, fusão, biblioteca e foto
- `project/tests/core/city_mass_test.gd` · `project/tests/core/city_mass_test.tscn` — a massa da cidade gerada: andares por distância do miolo
- `project/tests/core/save_undo_test.gd` · `project/tests/core/save_undo_test.tscn` — desfazer/refazer e ida e volta do save
- `project/tests/visual/capture.gd` · `project/tests/visual/capture.tscn` · `project/tests/visual/capture_views.gd` — capturas da cidade real e suas vistas fixas · `project/tests/visual/capture_kerb.gd` · `project/tests/visual/capture_memory.gd` · `project/tests/visual/capture_signals.gd` · `project/tests/visual/capture_soak.gd` · `project/tests/visual/capture_wait.gd` — rebaixo da guia, memória, semáforos e tráfego (soak) das capturas; a espera de toda foto até a cidade estar desenhada, registrada, com aviso de foto incompleta
- `project/tests/visual/capture_features.gd` · `project/tests/visual/capture_features.tscn` — capturas de recursos dirigidas como o jogador
- `project/tests/visual/capture_clock.gd` · `project/tests/visual/capture_clock.tscn` — a luz no jogo de verdade: botões sol, lua e relógio de play.tscn
- `project/tests/visual/capture_landscape.gd` · `project/tests/visual/capture_landscape.tscn` · `project/tests/visual/landscape_cards.gd` — capturas da paisagem nos ângulos do 4.0; rampa de cinzas, cubo cinza e árvores plantadas, medidos sozinhos
- `project/tests/visual/capture_vehicles.gd` — capturas de perto do ônibus, do caminhão, da moto, da van e de um sedã (vidro, cabine, pessoas e pontas)
- `project/tests/visual/capture_roads.gd` · `project/tests/visual/capture_roads.tscn` — vias de perto em chão livre (cruzamentos, T, ponta, curva) e a ferramenta em uso, nos ângulos das fotos do 4.0
- `project/tests/visual/capture_pair.gd` · `project/tests/visual/capture_pair.tscn` — fotos no enquadramento das do 4.0: uma lista de vistas serve aos dois; com `--buildings=` os próprios prédios do 4.0, na pose lida dele
- `project/tests/visual/pair_camera.gd` — a câmera exata de uma foto do 4.0: pose e lente
- `project/tests/visual/capture_ui.gd` · `project/tests/visual/capture_ui.tscn` — capturas da interface por cliques
- `project/tests/visual/ui_driver.gd` — entrada real (mouse/teclas) para as capturas
- `project/ui/builder_strip.gd` · `project/ui/model_shelf.gd` — seções e cartões das abas do Construir; prateleira de Modelos (busca, categorias, fotos, salvar e remover)
- `project/ui/drawer_strips.gd` — cartões e controles das gavetas
- `project/ui/hud_actions.gd` — menu, desfazer/refazer e atalhos
- `project/ui/hud_drawer.gd` — gavetas das categorias
- `project/ui/hud_popover.gd` — camadas, simulação, câmera, ajuda e menu
- `project/ui/hud_widgets.gd` — fábricas de botões/rótulos do tema
- `project/ui/inspector_panel.gd` — inspetor
- `project/ui/land_strips.gd` — painéis da gaveta Paisagem a partir de land_panels.json
- `project/ui/road_catalog.gd` — catálogo de vias
- `project/ui/road_options.gd` — opções da ferramenta de via (traçado, encaixe)
- `project/ui/junction_marks.gd` — marcas do controle de cada cruzamento na aba Controle
- `project/ui/preview_tag.gd` — etiqueta de comprimento, preço e recusa junto do fim da via em mão
- `project/ui/road_swatch.gd` — figura do perfil visto de cima
- `project/ui/roadcraft_hud.gd` — barra de cima, dock, dica e bússola
- `project/ui/theme/theme_builder.gd` — único dono dos estilos (tokens → Theme)
- `project/ui/icons/**` — ícones SVG do Roadcraft (brancos; a cor vem do tema)
- `project/data/catalog/growth.json` · `project/data/catalog/maps.json` · `project/data/catalog/nature.json` · `project/data/catalog/road_profiles.json` · `project/data/catalog/road_profiles_a.json` · `project/data/catalog/road_profiles_b.json` · `project/data/catalog/street_furniture.json` · `project/data/catalog/traffic.json` · `project/data/catalog/tuning.json` · `project/data/catalog/zones.json` — catálogo: crescimento, mapas, paisagem, perfis de via, mobiliário da rua e travessias, tráfego, tuning e zonas
- `project/data/game/camera.json` · `project/data/game/daylight.json` · `project/data/game/saves.json` · `project/data/game/settings.json` · `project/data/game/sky.json` — câmera, dia/noite, saves, qualidade e céu desenhado
- `project/data/i18n/strings.csv` · `project/data/i18n/strings_b.csv` · `project/data/i18n/strings_c.csv` · `project/data/i18n/strings_d.csv` · `project/data/i18n/strings_e.csv` — textos (partes do mesmo catálogo)
- `project/data/render/agents.json` · `project/data/render/buildings.json` · `project/data/render/cover.json` · `project/data/render/interiors.json` · `project/data/render/materials.json` · `project/data/render/models.json` · `project/data/render/overlays.json` · `project/data/render/palette.json` · `project/data/render/streets.json` — tabelas do render
- `project/data/ui/builder.json` · `project/data/ui/drawers.json` · `project/data/ui/hud.json` · `project/data/ui/land_elements.json` · `project/data/ui/land_panels.json` · `project/data/ui/tokens.json` · `project/data/ui/toolbar.json` — tabelas da interface
- `tools/gates/schemas/**` — um schema por tabela de dados

**Ferramentas — tools/**
- `tools/assets/cook_people.py` · `tools/assets/export_builder_ui.py` · `tools/assets/ktx2_to_png.mjs` · `tools/assets/make_road_profiles.py` — pessoas cozidas
  (MakeHuman CC0 + caminhada Rocketbox MIT), ícones e textos do Construir, KTX2 do Roadcraft → PNG, road_profiles.json da fonte do Roadcraft
- `tools/build/build.sh` · `tools/build/run_tests.sh` · `tools/build/hidden_run.ps1` — compilar, testar (todas as cenas, com tempo-limite), capturar, exportar e empacotar; rodar o jogo num desktop do Windows à parte (testes e capturas nunca abrem janela na tela do usuário)
- `tools/gates/gate_clones.py` · `tools/gates/gate_code.py` · `tools/gates/gate_data.py` · `tools/gates/gate_defs.py` —
  código duplicado; camadas, padrões proibidos e órfãos; schemas; números mágicos, cópias de dados, donos e estado na UI
- `tools/gates/gate_docs.py` · `tools/gates/gate_files.py` · `tools/gates/gate_tuning.py` — mapa deste documento; lixo,
  documento único, tamanhos e cabeçalhos; tuning ↔ JSON e espelhos
- `tools/gates/owners.json` · `tools/gates/rules.json` · `tools/gates/run_gates.py` — donos únicos, configuração, executa todos
<!-- MAPA:FIM -->

## Pendências
Backlog de recursos do Bloco E do roteiro (não faz parte do porte de desempenho B/C/D, que está concluído e testado).
- Porte build: o que falta está em `ports/build/cobertura.md` (estado `a portar`) e o plano em `ports/build/plano.md`. O corpo editável e as abas Modelos, Massa, Fachada, Peças, Telhado e Cores existem, com seleção por raio; faltam Desenhar e Interior. Dos modelos, a função no registro só existe para as 13 do jogo (os outros entram sem função), e os 38 lotes vêm com o porte `marcos`. Bandeiras no pináculo e na lanterna ainda não. Árvores de corpo colocado já são desenhadas; as dos lotes crescidos ainda não. a peça não tem fantasma sob o ponteiro.
- Noite: as paredes ficam ~5x mais escuras que no 4.0 (mediana de luminância da foto 0,004 contra 0,019) e as janelas distantes se fundem em blocos maiores; o desgaste não tem foto de trilha fina (célula de 5 m pega a faixa inteira).
- Árvores: forma, tons e atlas seguem o 4.0 no mesmo quadro (`capture_landscape --only=arv_perto,...`), escolha de modelagem dita no _doc de `trees`; as matas seguem a mesma regra e a mesma malha do 4.0 (mesma árvore na mesma célula: `tree_place.cpp`), e passar a instância sem cor quando o `rs_util.h` da rua integrar. Paisagem: a luz é a do 4.0 no mesmo sol (16h, 29°), medida em discos pintados de albedo igual, e os mapas de chão entram sem ganho; a imagem passa pela gradação de filme (`film_grade.cpp`) e pela vinheta. Falta o `canyonLight` do 4.0 (escurecimento das ruas) e a luz do céu fora do terreno; pedras da costa, estratos da rocha, grama seca da ecologia e lembrar a escolha dos pincéis não foram portados.
- Água: alvo de cor vem de uma foto antiga do 4.0 (o mapa 0 não tem lago).
- Pontes têm tabuleiro e pilares, mas não há vão estaiado nem rodovia elevada sobre terra (viadutos).
- Ônibus/linhas de transporte, metrô e escolas com alunos ainda não são simulados (ônibus só aparecem como veículos, a
  lotação só desenhada, pela semente). Veículos: portas e vidros não abrem e quem vai dentro não se mexe (o 4.0 anima).
- Contorno, iluminação e shaders validados em Forward+ na RTX 3060 (capturas em `dist/captures`, incluindo HLOD
  distante e peças de fachada); falta conferir em GPU AMD/Intel e à noite com o jogo em movimento contínuo.
- Memória (Forward+ 1600x900, cidade toda desenhada): 10 km, 3.169 prédios — 185 MiB estática, 507 de vídeo, 308 de buffers;
  24 km, 457.714 prédios — 601 estática, 1.435 de vídeo, 793 de buffers (camera_pan parada; nenhum quadro > 50 ms, GPU ≤ 44 %;
  maior HLOD de superchunk 8,1 MiB).
- Menu principal / tela inicial (semente, tamanho, novo/abrir): o jogo abre numa cidade gerada. Lotes: árvores do lote (`b40::lot_trees`) esperam `render/trees/tree_place.cpp` (e o include do repasse `render/buildings/civic.h`);
  estacionar espera `sim/traffic` ler `sim/buildings/lot_access`; sem lote de esquina, garagem em encosta e caminhão nas docas; escritórios do mapa 0 (29-30 m, praça 5-7 m) sem fundo para estacionamento.
- Interface "em construção": camadas Grade e Interior, Sobre, aba Mover, Construir (Modelos, Massa, Fachada, Peças,
  Telhado, Interior, Cores), Paisagem (Rua, Pessoas e Aplainar mapa), paradas/estações/linhas,
  pistola/bomba. Salvar é um arquivo por cidade (sem gerações/quarentena do Roadcraft); o autosave não grava cidade
  gerada que o jogador não editou nem salvou (o Roadcraft grava e poda as não tocadas).
- Rotatória: sem fundir trecho curto até a junção (Roadcraft < 14 m), sem placas de dê a preferência nos acessos.
- Vias do jogador: altura segue o chão (sem greide de menor terraplenagem, pontes/túneis, rampa íngreme, muro; o talude leva a grama do terreno, sem o que foi pintado no chão);
  Quarteirões ainda não; demolir via deixa o corredor de terreno. Falta do porte de vias: Personalizar e Meus modelos, editor de perfil, teclas, regras por acesso, conectores, nomes de rua, legendas, os estilos `eua` e `europa` de pintura, vazios, e larguras de faixa e calçada, zebra de escada e nomes de rua por estilo (em `streets.json` `markings.styles` há `roadcraft`, o desenho do 4.0, e `brasil`, a norma; o carregador lê `markings.style`), linha contínua na aproximação de semáforo (MBST IV 4.2.1), comparar a geração com `streets.ts`/`plan.ts`; a prévia não avisa de trecho apertado; parada, preferência e minirrotatória rodam como cruzamento de prioridade; mover recusa um nó cujo outro lado já era fechado.
- Tráfego: sem estacionar/docas/embarque, semáforos atuados pelos veículos (MBST V cap. 7) sem botão de pedestre nem coordenação; rotas
  síncronas no tick (sem threads). Medir com `capture.tscn -- --soak=5` (retidos e paradas no cruzamento por segundo, chegadas).
- Mobiliário da rua: postes opostos em rua de 8–12 m (NPIP-1 aceita alternados, sem pool em `road.gdshader`); peças sem instâncias (+19 MiB de vídeo).
- Pedestres: só a caminhada (sem ciclistas, portas dos prédios, passageiros de ônibus, parados, celular, guarda-chuva, sentados); o pacote precisa levar
  o texto MIT da animação Rocketbox (`project/assets/people/LICENSES.json`). Os 38 projetos de marcos do Roadcraft (`data/lots-roadcraft`) e o estádio ainda
  não foram portados; muros de divisa entre prédios colados ainda não são desenhados.
