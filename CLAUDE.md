# Cidade — city builder (Godot 4.7 + núcleo C++ GDExtension)

Único documento do projeto. Regras, comandos, mapa do código e pendências ficam aqui; não crie outros
documentos nem memórias (o hook `tools/gates/hook_pre_write.py` bloqueia). Atualize este arquivo na mesma
mudança que altera o que ele descreve: o gate `MAPA_DESATUALIZADO` falha quando o mapa diverge do disco.

## Processo de trabalho (inegociável, toda tarefa, nesta ordem; trabalho direto, sem agentes)
1. **Análise:** ler o código envolvido, reproduzir e medir (sonda `render_cost`, `tests/perf/camera_pan`, capturas, CPU,
   GPU, memória) até achar a causa raiz. Corrigir a causa, nunca o efeito. Medir é desta etapa.
2. **Pesquisa** na documentação oficial (Godot, godot-cpp, Terrain3D) até entender o que fazer e o método; nunca confiar
   só na memória.
3. **Compreensão e plano** antes de codificar: a solução completa e correta, nunca a mais barata; sem arquivo-deus nem
   funcionalidade espalhada; regras deste arquivo (dono único, sem número mágico, dados em `data/**`, mapa em dia).
4. **Codificação** sem tentativa e erro: se não deu certo, voltar à documentação até entender.
5. **Teste:** "Gates OK" em `python3 tools/gates/run_gates.py` (o hook Stop bloqueia); C++ mudou → `tools/build/build.sh
   core` / `core-windows` (DLL de release e de debug); visível mudou → `tools/build/build.sh e2e`. Medir de novo: quadros
   sem travar, GPU nunca a 100%, CPU e memória nunca acima de antes. Fotos analisadas com atenção (contra a referência e
   o estado anterior): nenhuma perda de qualidade visual; todo problema achado é corrigido.
6. Nunca esperar às cegas um processo lançado (tempo-limite e acompanhamento); parar só processos próprios, pelo PID.
7. Commit por tema e push regular em `origin/master`.
- **Referência — intransponível:** `C:\Users\Jonathan\Documents\Projects\roadcraft-4.0-main` (Roadcraft 4.0) é a referência
  de visual e de comportamento de tudo (gráficos, prédios, tráfego, semáforos, pedestres, carros, postes e fios): o jogo
  deve ter todos os gráficos e funcionalidades dele. **Nunca copiar nem traduzir o código dele** (não é otimizado nem tem
  desempenho; foi abandonado por isso): estudar como ele parece e se comporta, achar a nossa causa e implementar na
  arquitetura atual, otimizado e do jeito certo.

## Regras de arquitetura (verificadas automaticamente)
- **Fonte única da verdade:** `CityState` (`native/src/sim/state/city_state.h`). Só `sim/commands` muda o
  estado a pedido do jogador; só `sim/simulation` avança o tempo. A UI nunca guarda números do jogo
  (`ESTADO_NA_UI`): lê `CityStore.snapshot` e envia `CityStore.dispatch(...)`.
- **Um dono por definição:** valores ajustáveis vivem só em `project/data/**` (tabelas JSON com schema).
  Nomes do tuning: X-macro `CITY_TUNING_FIELDS`; valores: `data/catalog/tuning.json` (`gate_tuning.py` exige
  as mesmas chaves e que todas sejam lidas). Regras com dono único estão em `tools/gates/owners.json`
  (altura do prédio, andar, regra do lote, "há trabalho a salvar", fachada, dinheiro, estado do jogo);
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

## Máquinas de estado
- `GameFlow.State`: BOOT → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram/saem de STOPPED.
- `ToolController.Tool`: NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT (só `set_tool`; botão segurado → `drag`/`tick`, solto → `release`); `RoadTool.Step`: START → CONTROL (curva) → END → (encadeia) END.

## Mapa do código
<!-- MAPA:INICIO -->
**Raiz e ferramentas**
- `.claude/settings.json` — hooks: gates na sessão, a cada edição e ao parar; memória automática desligada
- `native/CMakeLists.txt` — Godot 4.7 API, precisão simples, saída em `project/bin`
- `native/toolchain-mingw.cmake` — compilação cruzada Windows via llvm-mingw
- `project/export_presets.cfg` · `project/project.godot` — exportação Windows e projeto Godot (autoloads, cena inicial)

**API GDExtension — native/src/api**
- `native/src/api/catalog_loader.cpp` · `native/src/api/catalog_loader.h` — Dictionary → Catalog estrito; a X-macro do tuning dita as chaves
- `native/src/api/city_core.cpp` · `native/src/api/city_core.h` — ciclo de vida: catálogo, render, mapa novo, velocidade, sincronização por quadro
- `native/src/api/city_core_bind.cpp` — bindings de script do CityCore
- `native/src/api/city_core_commands.cpp` — comandos do jogador e prévias (sim/commands) como API de script
- `native/src/api/city_core_edit.cpp` — desfazer/refazer, traços e edições das gavetas (dividir, melhorar, rotatória, relevo, prédio desenhado)
- `native/src/api/city_core_persist.cpp` — salvar/abrir: encode sob trava curta, ZSTD e gravação em thread própria
- `native/src/api/city_core_query.cpp` · `native/src/api/city_core_probe.cpp` — leituras para a UI e os testes (snapshot, altura, raycast); sondas: custo do quadro, chegada das poses e tráfego
- `native/src/api/edit_result.h` — EditResult como Dictionary e `run_edit` (comando dentro de um passo de undo)
- `native/src/api/register_types.cpp` — entrada da GDExtension: registra CityCore
- `native/src/api/traffic_loader.cpp` — Dictionary → TrafficParams estrito

**Núcleo — native/src/core**
- `native/src/core/bytes.h` — leitor/escritor binário do formato de save
- `native/src/core/color.h` — "#RRGGBB"/"#RRGGBBAA" → RGBA8 empacotado (único parser de cor)
- `native/src/core/grid.h` — chunks, chaves de célula, hash espacial e orçamento de build
- `native/src/core/job_pool.cpp` · `native/src/core/job_pool.h` — pool de jobs por prioridade e `parallel_for`
- `native/src/core/math.h` — vetores 2D no plano XZ
- `native/src/core/random.h` — hash, RNG e ruído determinísticos
- `native/src/core/seeded.h` — geradores com os números do Roadcraft bit a bit
- `native/src/core/units.h` · `native/src/core/laps.h` — unidades físicas e de calendário (único dono dos fatores); cronômetro das sondas

**Núcleo — native/src/sim**
- `native/src/sim/buildings/building_store.cpp` · `native/src/sim/buildings/building_store.h` — registros de prédio, ocupação e consultas espaciais
- `native/src/sim/buildings/growth.h` — regras do que cresce num lote (pisos por zona/rua, altura orgânica)
- `native/src/sim/catalog/catalog.h` — tipos do catálogo (sem valores)
- `native/src/sim/catalog/profile_types.h` — formas da seção de via que road_profiles.json preenche
- `native/src/sim/catalog/traffic_params.h` — parâmetros de tráfego e pedestres
- `native/src/sim/catalog/tuning_fields.h` — nomes de todo tunable escalar (X-macro)
- `native/src/sim/citygen/block_lots.cpp` — quarteirão fechado cortado em lotes (blockLots.ts)
- `native/src/sim/citygen/city_gen.cpp` · `native/src/sim/citygen/city_gen.h` — pipeline da cidade gerada
- `native/src/sim/citygen/lot_gen.cpp` · `native/src/sim/citygen/lot_gen.h` — lotes ao longo das ruas zonadas: grade de ocupação, recuos, testes
- `native/src/sim/citygen/lot_place.cpp` — colocação dos lotes: quarteirões fechados e testadas das ruas
- `native/src/sim/citygen/lot_strips.h` — cortador de lotes de uma célula (interno de sim/citygen)
- `native/src/sim/citygen/road_gen.cpp` · `native/src/sim/citygen/road_gen.h` — linhas da malha e marcha sobre o terreno
- `native/src/sim/citygen/road_gen_graph.cpp` — grafo planar das ruas geradas (cruzamentos, nós fundidos)
- `native/src/sim/citygen/road_gen_tidy.cpp` — limpeza do grafo gerado (nós e braços degenerados)
- `native/src/sim/commands/commands.h` — API de comandos do jogador
- `native/src/sim/commands/edit_history.cpp` — pilhas de desfazer/refazer e limites
- `native/src/sim/commands/edit_journal.h` — diário de edições (único mutador dos comandos)
- `native/src/sim/commands/journal_ops.cpp` — operações `j_*` do diário
- `native/src/sim/commands/land_commands.cpp` — relevo, prédio desenhado e edição de prédio
- `native/src/sim/commands/road_commands.cpp` — construir e demolir vias
- `native/src/sim/commands/road_edit.cpp` · `native/src/sim/commands/road_edit.h` — passos comuns das edições de via
- `native/src/sim/commands/road_plan.cpp` — encaixe e validação de vias (= prévia)
- `native/src/sim/commands/road_tabs.cpp` — Dividir e Melhorar
- `native/src/sim/commands/roundabout.cpp` — rotatória
- `native/src/sim/commands/zone_commands.cpp` — pincel de zona
- `native/src/sim/lots/lot_cache.cpp` · `native/src/sim/lots/lot_cache.h` — lotes por célula recalculados em paralelo
- `native/src/sim/persistence/save_format.h` — formato do save da cidade
- `native/src/sim/persistence/save_read.cpp` — abrir: terreno do seed, ladrilhos esculpidos, modificadores e grafo
- `native/src/sim/persistence/save_write.cpp` — gravar: cabeçalho, terreno, modificadores, vias e prédios
- `native/src/sim/roads/geometry.cpp` · `native/src/sim/roads/geometry.h` — geometria pura das vias (Bézier, tabelas, interseções)
- `native/src/sim/roads/junction.cpp` · `native/src/sim/roads/junction.h` — braços, recuos, esquinas e balões
- `native/src/sim/roads/profiles.cpp` · `native/src/sim/roads/profiles.h` — derivação do perfil (faixas, linhas, calçadas, preço)
- `native/src/sim/roads/road_network.cpp` · `native/src/sim/roads/road_network.h` — armazenamento e consultas do grafo viário
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
- `native/src/sim/traffic/junction_control.cpp` — controle de cruzamento, semáforos e preferência
- `native/src/sim/traffic/lane_change.cpp` — MOBIL
- `native/src/sim/traffic/lane_geometry.cpp` — pontos de faixa, conector e calçada
- `native/src/sim/traffic/lane_graph.cpp` — grafo de faixas e conectores com conflitos (build completo e por nó)
- `native/src/sim/traffic/lane_graph_edit.cpp` — rebuild incremental das faixas e nós sujos
- `native/src/sim/traffic/lane_graph_internal.h` — superfície interna do grafo de faixas
- `native/src/sim/traffic/lane_list.cpp` — listas por faixa/conector e lista micro de veículos
- `native/src/sim/traffic/meso.cpp` — modelo meso distante (uma fila por faixa)
- `native/src/sim/traffic/ped_crossing.cpp` · `native/src/sim/traffic/ped_crowd.cpp` — pedestres e carros nas travessias; multidão: faixas da calçada (desvio, distância) e lugares de espera na guia
- `native/src/sim/traffic/ped_routes.cpp` — rotas a pé e caminho de esquina
- `native/src/sim/traffic/pedestrians.cpp` — pedestres com destino
- `native/src/sim/traffic/route_arena.cpp` — arena de rotas (RouteRef) reutilizada
- `native/src/sim/traffic/routing.cpp` — rotas de carro por segmento dirigido, desvio e reconsideração
- `native/src/sim/traffic/traffic_state.h` — grafo de faixas, veículos, pedestres, meso e pools
- `native/src/sim/traffic/traffic_system.cpp` · `native/src/sim/traffic/traffic_system.h` — passo do tráfego, publicação de poses e rebuild
- `native/src/sim/traffic/vehicle_motion.cpp` — IDM e conectores
- `native/src/sim/traffic/window.cpp` — janela micro em volta da câmera e materialização do meso

**Render (só lê) — native/src/render**
- `native/src/render/agents/agent_cabin.cpp` · `native/src/render/agents/agent_parts.cpp` — cabine de qualquer veículo (forro com vãos de porta, painel, fileiras de bancos nos estilos carro/cabine/poltrona, volante, pessoas com a ordem de ocupação) e vidro à parte; peças comuns: cores, caixas e barras marcadas, painéis de vidro, rodas e casco oco com arcos
- `native/src/render/agents/agent_meshes.h` · `native/src/render/agents/agent_renderer.cpp` · `native/src/render/agents/agent_renderer.h` — malhas dos agentes (agents.json) em dois níveis (perto: cabine e vidro transparente; longe: sem cabine, vidro opaco), paletas, lotação e ordem dos assentos por veículo e MultiMesh por quadro
- `native/src/render/agents/car_body.cpp` — carroceria loftada do carro de passeio
- `native/src/render/agents/bus_body.cpp` · `native/src/render/agents/heavy_meshes.cpp` — ônibus de piso baixo com salão; caminhão com cabine, moto e peças dos pesados
- `native/src/render/agents/people_asset.cpp` · `native/src/render/agents/people_asset.h` — pessoas cozidas (people.bin)
- `native/src/render/agents/people_renderer.cpp` · `native/src/render/agents/people_renderer.h` — pedestres com esqueleto no shader e 2 níveis
- `native/src/render/buildings/b40_data.cpp` · `native/src/render/buildings/b40_data.h` — buildings.json → tabelas do gerador 4.0
- `native/src/render/buildings/b40_gen.h` — gerador 4.0 passo a passo num Xoshiro128
- `native/src/render/buildings/b40_paint.cpp` · `native/src/render/buildings/b40_paint.h` — ajuste HSL de telhado e segunda pele
- `native/src/render/buildings/b40_types.h` — vocabulário 4.0 (bays, telhados, acabamentos) e corpos
- `native/src/render/buildings/building_bodies.cpp` · `native/src/render/buildings/building_bodies.h` — cache de corpos e colocação no mundo
- `native/src/render/buildings/building_renderer.cpp` · `native/src/render/buildings/building_renderer.h` — streaming dos níveis near/mid/far por chunk e superchunk
- `native/src/render/buildings/building_renderer_build.cpp` — montagem CPU dos níveis por subchunk e bandas de peças
- `native/src/render/buildings/building_renderer_commit.cpp` — upload ao RenderingServer dos níveis, peças e bandas
- `native/src/render/buildings/civic.cpp` · `native/src/render/buildings/civic.h` — corpos cívicos (praça, escola, igreja, hospital)
- `native/src/render/buildings/civic_services.cpp` — posto, mercado, prefeitura, polícia e bombeiros
- `native/src/render/buildings/facade.h` — corpo colocado no mundo e emissores que o desenham
- `native/src/render/buildings/facade_bay.cpp` · `native/src/render/buildings/facade_bay.h` — abertura de uma bay, parede e peças
- `native/src/render/buildings/facade_compose.cpp` — gramática de fachada 4.0 (dono da fachada)
- `native/src/render/buildings/facade_emit.cpp` — paredes de cada volume, lado, andar e bay
- `native/src/render/buildings/facade_parts.cpp` — peças de cada componente de bay
- `native/src/render/buildings/facade_props.cpp` — AC, antenas e vasos dos andares altos
- `native/src/render/buildings/far_mass.cpp` — nível distante de um corpo colocado
- `native/src/render/buildings/lot_emit.cpp` — elementos de lote (calçadas, muros, portões)
- `native/src/render/buildings/lot_furnish.cpp` — mobiliário do lote (jardim, piscina, cerca)
- `native/src/render/buildings/lot_plan.cpp` — plano do lote crescido (frente, lados e fundo)
- `native/src/render/buildings/massing.h` — passos comuns do massing 4.0
- `native/src/render/buildings/massing_core.cpp` — blocos, telhados, segunda pele e corpo de um registro
- `native/src/render/buildings/massing_house.cpp` — casas (asa, varanda, bangalô etc.)
- `native/src/render/buildings/massing_tall.cpp` — apartamentos, torres, lojas e escritórios
- `native/src/render/buildings/massing_works.cpp` — galpões industriais
- `native/src/render/buildings/open_lots.cpp` — praça, parque, quadra e lote vazio
- `native/src/render/buildings/part_models.cpp` — modelos de peça de fachada no referencial da bay
- `native/src/render/buildings/placed.cpp` — consultas de geometria e tinta de um corpo colocado
- `native/src/render/buildings/roof_emit.cpp` — telhados de um corpo colocado
- `native/src/render/buildings/roof_pitched.cpp` — telhados inclinados e dente de serra
- `native/src/render/buildings/roof_plant.cpp` — equipamentos de cobertura
- `native/src/render/buildings/roof_terrace.cpp` — terraços usados
- `native/src/render/buildings/roof_util.h` — predicados de telhado compartilhados
- `native/src/render/buildings/shell.cpp` · `native/src/render/buildings/shell.h` — escrita de vértices das malhas de prédio
- `native/src/render/chunk_jobs.h` · `native/src/render/chunk_queue.h` · `native/src/render/mesh_queue.h` — jobs por chunk, conversão no orçamento de envio e commit orçado; chunks ao redor da câmera; fila de uma superfície
- `native/src/render/mesh_builder.cpp` · `native/src/render/mesh_builder.h` — emissores de primitivas e envio de malha
- `native/src/render/model_library.cpp` · `native/src/render/model_library.h` — massas unitárias e modelos de data/render/models.json
- `native/src/render/overlays/overlay_build.cpp` — malhas CPU das camadas, uma por chunk de 256 m
- `native/src/render/overlays/overlay_renderer.cpp` · `native/src/render/overlays/overlay_renderer.h` — jobs das zonas e ciclo do congestionamento
- `native/src/render/owned_segments.h` — segmentos que um chunk possui (pelo ponto médio)
- `native/src/render/props/prop_kit.cpp` · `native/src/render/props/prop_kit.h` — malhas do mobiliário a partir dos dados
- `native/src/render/props/prop_renderer.cpp` · `native/src/render/props/prop_renderer.h` · `native/src/render/props/signal_lamps.cpp` · `native/src/render/props/signal_lamps.h`
  — chunks do mobiliário; lentes acesas dos semáforos (estado publicado pela simulação)
- `native/src/render/props/street_props.cpp` · `native/src/render/props/street_props.h` — colocação de postes, semáforos (um por acesso), árvores e mobiliário
- `native/src/render/render_context.h` — cenário, tabelas de render e materiais recebidos pelos renderizadores
- `native/src/render/render_world.cpp` · `native/src/render/render_world.h` — consome o ChangeLog e aplica os orçamentos por quadro
- `native/src/render/roads/road_emit.cpp` — referenciais de segmento, nós e quad texturizado
- `native/src/render/roads/road_junction.cpp` — placas de nó, beco sem saída e balão
- `native/src/render/roads/road_look.cpp` · `native/src/render/roads/road_look.h` — streets.json → StreetLook e fileiras de postes
- `native/src/render/roads/road_markings.cpp` — pintura viária como geometria fina elevada
- `native/src/render/roads/road_mesh.h` — peças do malhador de vias
- `native/src/render/roads/road_renderer.cpp` · `native/src/render/roads/road_renderer.h` — chunks de via por segmento e nó
- `native/src/render/roads/road_segment.cpp` — superfície do segmento a partir do perfil
- `native/src/render/rs_util.h` — instâncias RenderingServer sem nós de cena
- `native/src/render/terrain_bridge.cpp` · `native/src/render/terrain_bridge.h` — API de scripting do Terrain3D
- `native/src/render/trees/tree_kit.cpp` · `native/src/render/trees/tree_kit.h` — malhas das espécies de árvore
- `native/src/render/trees/tree_place.cpp` · `native/src/render/trees/tree_place.h` — colocação de árvores por chunk
- `native/src/render/trees/tree_renderer.cpp` · `native/src/render/trees/tree_renderer.h` — streaming dos chunks de árvore

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
- `project/game/store/save_slots.gd` — onde as cidades são salvas: um arquivo por cidade, cópia do mapa, índice da última e pasta dos testes
- `project/game/tools/demolish_tool.gd` — demolir
- `project/game/tools/inspect_tool.gd` — inspecionar
- `project/game/tools/lot_tool.gd` — Construir › Desenhar
- `project/game/tools/road_tool.gd` — via reta/curva com prévia
- `project/game/tools/roundabout_tool.gd` — rotatória
- `project/game/tools/split_tool.gd` — Dividir
- `project/game/tools/terrain_tool.gd` — Relevo
- `project/game/tools/tool_base.gd` — base das ferramentas (recebe pontos do chão)
- `project/game/tools/tool_controller.gd` — máquina de estados das ferramentas
- `project/game/tools/upgrade_tool.gd` — Melhorar
- `project/game/tools/zone_tool.gd` — pincel de zona
- `project/render/city_world.gd` — Terrain3D, céu, sol, água e materiais (inclui meios das texturas e parâmetros da água)
- `project/render/daylight.gd` — dia/noite
- `project/render/preview_line.gd` — prévias das ferramentas
- `project/render/shaders/agent_glass.gdshader` · `project/render/shaders/agents.gdshader` · `project/render/shaders/people.gdshader` · `project/render/shaders/props.gdshader` — vidro dos veículos, veículos e ocupantes, pedestres cozidos, mobiliário e árvores
- `project/render/shaders/building.gdshader` · `project/render/shaders/building_far.gdshader` · `project/render/shaders/building_parts.gdshader` · `project/render/shaders/building_common.gdshaderinc`
  — prédio (interior mapping), massas distantes/HLOD, peças de fachada instanciadas; hash, ruído, acabamento e intemperismo
- `project/render/shaders/road.gdshader` · `project/render/shaders/water.gdshader` · `project/render/shaders/noise2d.gdshaderinc` — vias, meios-fios e pintura; água (ondas, Fresnel,
  espuma); ruído de valor de ambos
- `project/render/shaders/overlay.gdshader` · `project/render/shaders/sky.gdshader` · `project/render/shaders/terrain.gdshader` — camadas de zonas e congestionamento, céu em gradiente, terreno toon (override do Terrain3D)
- `project/tests/e2e/e2e.gd` · `project/tests/e2e/e2e.tscn` — ponta a ponta: mundo + HUD reais, chunks desenhados, frota e buffers de instância
- `project/tests/core/camera_input_test.gd` · `project/tests/core/camera_input_test.tscn` — câmera por entrada real
- `project/tests/perf/camera_pan.gd` · `project/tests/perf/camera_pan.tscn` — tempos de quadro com pan, giro e zoom sobre a cidade
- `project/tests/core/road_commands_test.gd` · `project/tests/core/road_commands_test.tscn` — comandos de via/zona/demolir pelo store
- `project/tests/core/save_undo_test.gd` · `project/tests/core/save_undo_test.tscn` — desfazer/refazer e ida e volta do save
- `project/tests/visual/capture.gd` · `project/tests/visual/capture.tscn` — capturas da cidade real · `project/tests/visual/capture_memory.gd` · `project/tests/visual/capture_signals.gd` · `project/tests/visual/capture_soak.gd` — memória, semáforos e tráfego (soak) das capturas
- `project/tests/visual/capture_features.gd` · `project/tests/visual/capture_features.tscn` — capturas de recursos dirigidas como o jogador
- `project/tests/visual/capture_vehicles.gd` — capturas de perto do ônibus, do caminhão e da moto (vidro, cabine e pessoas)
- `project/tests/visual/capture_ui.gd` · `project/tests/visual/capture_ui.tscn` — capturas da interface por cliques
- `project/tests/visual/ui_driver.gd` — entrada real (mouse/teclas) para as capturas
- `project/ui/drawer_strips.gd` — cartões e controles das gavetas
- `project/ui/hud_actions.gd` — menu, desfazer/refazer e atalhos
- `project/ui/hud_drawer.gd` — gavetas das categorias
- `project/ui/hud_popover.gd` — camadas, simulação, câmera, ajuda e menu
- `project/ui/hud_widgets.gd` — fábricas de botões/rótulos do tema
- `project/ui/inspector_panel.gd` — inspetor
- `project/ui/road_catalog.gd` — catálogo de vias
- `project/ui/road_swatch.gd` — figura do perfil visto de cima
- `project/ui/roadcraft_hud.gd` — barra de cima, dock, dica e bússola
- `project/ui/theme/theme_builder.gd` — único dono dos estilos (tokens → Theme)
- `project/ui/icons/**` — ícones SVG do Roadcraft (brancos; a cor vem do tema)
- `project/data/catalog/growth.json` · `project/data/catalog/maps.json` · `project/data/catalog/road_profiles.json` · `project/data/catalog/road_profiles_a.json` · `project/data/catalog/road_profiles_b.json` · `project/data/catalog/traffic.json` · `project/data/catalog/tuning.json` · `project/data/catalog/zones.json` — catálogo: crescimento, mapas, perfis de via, tráfego, tuning e zonas
- `project/data/game/camera.json` · `project/data/game/daylight.json` · `project/data/game/saves.json` · `project/data/game/settings.json` — câmera, dia/noite, saves e qualidade
- `project/data/i18n/strings.csv` · `project/data/i18n/strings_b.csv` · `project/data/i18n/strings_c.csv` — textos (partes do mesmo catálogo)
- `project/data/render/agents.json` · `project/data/render/buildings.json` · `project/data/render/materials.json` · `project/data/render/models.json` · `project/data/render/overlays.json` · `project/data/render/palette.json` · `project/data/render/streets.json` — tabelas do render
- `project/data/ui/drawers.json` · `project/data/ui/hud.json` · `project/data/ui/tokens.json` · `project/data/ui/toolbar.json` — tabelas da interface
- `tools/gates/schemas/**` — um schema por tabela de dados

**Ferramentas — tools/**
- `tools/assets/cook_people.py` · `tools/assets/ktx2_to_png.mjs` · `tools/assets/make_road_profiles.py` — pessoas cozidas
  (MakeHuman CC0 + caminhada Rocketbox MIT), KTX2 do Roadcraft → PNG, road_profiles.json da fonte do Roadcraft
- `tools/build/build.sh` · `tools/build/run_tests.sh` · `tools/build/hidden_run.ps1` — compilar, testar (todas as cenas, com tempo-limite), capturar, exportar e empacotar; rodar o jogo num desktop do Windows à parte (testes e capturas nunca abrem janela na tela do usuário)
- `tools/gates/gate_clones.py` · `tools/gates/gate_code.py` · `tools/gates/gate_data.py` · `tools/gates/gate_defs.py` —
  código duplicado; camadas, padrões proibidos e órfãos; schemas; números mágicos, cópias de dados, donos e estado na UI
- `tools/gates/gate_docs.py` · `tools/gates/gate_files.py` · `tools/gates/gate_tuning.py` — mapa deste documento; lixo,
  documento único, tamanhos e cabeçalhos; tuning ↔ JSON e espelhos
- `tools/gates/hook_post_edit.py` · `tools/gates/hook_pre_write.py` — gates do arquivo editado; bloqueia documentos novos
- `tools/gates/owners.json` · `tools/gates/rules.json` · `tools/gates/run_gates.py` — donos únicos, configuração, executa todos
<!-- MAPA:FIM -->

## Pendências
Backlog de recursos do Bloco E do roteiro (não faz parte do porte de desempenho B/C/D, que está concluído e testado).
- Pontes têm tabuleiro e pilares, mas não há vão estaiado nem rodovia elevada sobre terra (viadutos).
- Ônibus/linhas de transporte, metrô e escolas com alunos ainda não são simulados (ônibus só aparecem como veículos, a
  lotação só desenhada, pela semente). Veículos: portas e vidros não abrem e quem vai dentro não se mexe (o 4.0 anima).
- Contorno, iluminação e shaders validados em Forward+ na RTX 3060 (capturas em `dist/captures`, incluindo HLOD
  distante e peças de fachada); falta conferir em GPU AMD/Intel e à noite com o jogo em movimento contínuo.
- Memória (Forward+ 1600x900, cidade toda desenhada): 10 km, 3.169 prédios — 185 MiB estática, 507 de vídeo, 308 de buffers;
  24 km, 457.714 prédios — 598 estática, 1.408 de vídeo, 766 de buffers (camera_pan parada; nenhum quadro > 50 ms, GPU ≤ 44 %).
- Menu principal / tela inicial (semente, tamanho, novo/abrir): o jogo abre numa cidade gerada. Lote: os pátios
  livres do massing 4.0 (`free`, furnishZones.ts) não são mobiliados (`lot_furnish.cpp` condensado).
- Interface "em construção": camadas Grade e Interior, Sobre, aba Mover, Construir (Modelos, Massa, Fachada, Peças,
  Telhado, Interior, Cores), Paisagem (Solo, Natureza, Céu, Rua, Pessoas; rio e Aplainar mapa), paradas/estações/linhas,
  pistola/bomba. Salvar é um arquivo por cidade (sem gerações/quarentena do Roadcraft); o autosave não grava cidade
  gerada que o jogador não editou nem salvou (o Roadcraft grava e poda as não tocadas); a água não seca sob o relevo.
- Rotatória: sem fundir trecho curto até a junção (Roadcraft < 14 m), sem placas de dê a preferência nos acessos.
- Vias do jogador: altura segue o chão (sem greide de menor terraplenagem, pontes/túneis, rampa íngreme, talude/muro);
  modo Livre e Quarteirões ainda não; demolir via deixa o corredor de terreno.
- Tráfego: sem estacionar/docas/embarque, semáforos só com ciclo fixo (sem atuação nem botão de pedestre); rotas
  síncronas no tick (sem threads). Medir com `capture.tscn -- --soak=5` (retidos > 20 s por minuto, chegadas/perdidas).
- Pedestres: ciclistas, portas dos prédios e passageiros de ônibus; não desviam de árvores, postes e mobiliário da calçada
  (a colocação deles é do render: precisa passar para a simulação, dono único, para os dois lerem).
- Pedestres: só a caminhada (sem parados, celular, guarda-chuva, sentados); o pacote precisa levar o texto MIT da animação Rocketbox
  (`project/assets/people/LICENSES.json`). Os 38 projetos de marcos do Roadcraft (`data/lots-roadcraft`) e o estádio ainda
  não foram portados; muros de divisa entre prédios colados ainda não são desenhados.
