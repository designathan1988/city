# Cidade — city builder (Godot 4.7 + núcleo C++ GDExtension)

Único documento do projeto. Regras, comandos, mapa do código e pendências ficam aqui; não crie outros
documentos nem memórias (o hook `tools/gates/hook_pre_write.py` bloqueia). Atualize este arquivo na mesma
mudança que altera o que ele descreve: o gate `MAPA_DESATUALIZADO` falha quando o mapa diverge do disco.

## Antes de concluir qualquer tarefa
1. `python3 tools/gates/run_gates.py` precisa terminar com "Gates OK" (o hook Stop bloqueia se não).
2. Mudou C++? `tools/build/build.sh core` (e `core-windows` antes de empacotar).
3. Mudou comportamento visível? `tools/build/build.sh e2e` (testes na camada que o jogador vê).
4. Pesquise na documentação oficial antes de implementar (Godot, Terrain3D, godot-cpp); não confie de memória.

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
| Núcleo Linux / Windows | `tools/build/build.sh core` / `MINGW=... tools/build/build.sh core-windows` |
| Testes e2e (precisa de display) | `GODOT=/caminho/godot tools/build/build.sh e2e` |
| Capturas de tela do jogo real | `tools/build/build.sh capture /pasta` |
| Exportar + zip | `tools/build/build.sh export && tools/build/build.sh package` |
| Rodar o jogo | `godot --path project` (Forward+; sem Vulkan use `--rendering-method gl_compatibility`) |

## Fluxo de dados e threads
`DataTables` lê `data/**` (tabelas grandes divididas em partes com o mesmo schema) → `CityStore.boot` envia o
catálogo ao `CityCore` (rejeita chave ausente) → `Simulation` roda em thread própria com tick fixo
(`sim_tick_hz`), sob `map_mutex` compartilhado e `traffic_mutex`; comandos, crescimento e relógio tomam
`map_mutex` exclusivo → mudanças vão para o `ChangeLog` (único canal de "sujo") → `RenderWorld.sync` consome o
log no thread principal e manda as malhas para o `JobPool`; o commit respeita `render.commit_budget_ms`. As
poses dos agentes saem por `PoseBuffer` (buffer triplo sem trava) e o salvamento codifica sob trava curta e grava
em thread própria (temporário + rename).

## Máquinas de estado
- `GameFlow.State`: BOOT → MAIN_MENU → LOADING → PLAYING ⇄ PAUSED → MAIN_MENU/QUITTING (tabela `TRANSITIONS`).
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
- `native/src/api/city_core_query.cpp` — leituras para a UI e os testes (snapshot, sondas, altura, raycast)
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
- `native/src/core/units.h` — unidades físicas e de calendário (único dono dos fatores)

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
- `native/src/sim/traffic/ped_crossing.cpp` — pedestres e carros nas travessias
- `native/src/sim/traffic/ped_routes.cpp` — rotas a pé e caminho de esquina
- `native/src/sim/traffic/pedestrians.cpp` — pedestres com destino
- `native/src/sim/traffic/route_arena.cpp` — arena de rotas (RouteRef) reutilizada
- `native/src/sim/traffic/routing.cpp` — rotas de carro por segmento dirigido, desvio e reconsideração
- `native/src/sim/traffic/traffic_state.h` — grafo de faixas, veículos, pedestres, meso e pools
- `native/src/sim/traffic/traffic_system.cpp` · `native/src/sim/traffic/traffic_system.h` — passo do tráfego, publicação de poses e rebuild
- `native/src/sim/traffic/vehicle_motion.cpp` — IDM e conectores
- `native/src/sim/traffic/window.cpp` — janela micro em volta da câmera e materialização do meso

**Render (só lê) — native/src/render**
- `native/src/render/agents/agent_meshes.h` — malhas dos agentes montadas uma vez a partir de agents.json
- `native/src/render/agents/agent_parts.cpp` — peças comuns: cores, caixas marcadas, rodas e corpos arqueados
- `native/src/render/agents/agent_renderer.cpp` · `native/src/render/agents/agent_renderer.h` — meshes, paletas e MultiMesh por quadro
- `native/src/render/agents/car_body.cpp` — carroceria loftada do carro de passeio
- `native/src/render/agents/heavy_meshes.cpp` — ônibus, caminhão e moto
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
- `native/src/render/chunk_jobs.h` — commit orçado das filas de malha por chunk
- `native/src/render/chunk_queue.h` — chunks ao redor da câmera (vias, mobiliário, prédios)
- `native/src/render/mesh_builder.cpp` · `native/src/render/mesh_builder.h` — emissores de primitivas e envio de malha
- `native/src/render/model_library.cpp` · `native/src/render/model_library.h` — massas unitárias e modelos de data/render/models.json
- `native/src/render/overlays/overlay_build.cpp` — malhas CPU das camadas, uma por chunk de 256 m
- `native/src/render/overlays/overlay_renderer.cpp` · `native/src/render/overlays/overlay_renderer.h` — jobs das zonas e ciclo do congestionamento
- `native/src/render/owned_segments.h` — segmentos que um chunk possui (pelo ponto médio)
- `native/src/render/props/prop_kit.cpp` · `native/src/render/props/prop_kit.h` — malhas do mobiliário a partir dos dados
- `native/src/render/props/prop_renderer.cpp` · `native/src/render/props/prop_renderer.h` — chunks do mobiliário
- `native/src/render/props/street_props.cpp` · `native/src/render/props/street_props.h` — colocação de postes, sinais, árvores e mobiliário
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
- `project/game/settings/settings.gd` — qualidade gráfica persistida em user://settings.cfg
- `project/game/store/city_store.gd` — único dono do CityCore no lado de script (`snapshot`)
- `project/game/store/formatters.gd` — formatação de números/dinheiro (dono de `format_money`)
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
- `project/render/shaders/agents.gdshader` — veículos
- `project/render/shaders/building.gdshader` — superfícies de prédio (interior mapping)
- `project/render/shaders/building_common.gdshaderinc` — hash, ruído, acabamento e intemperismo compartilhados
- `project/render/shaders/building_far.gdshader` — massas distantes/HLOD
- `project/render/shaders/building_parts.gdshader` — peças de fachada instanciadas
- `project/render/shaders/noise2d.gdshaderinc` — ruído de valor compartilhado por road e water
- `project/render/shaders/overlay.gdshader` — camadas de zonas e congestionamento
- `project/render/shaders/people.gdshader` — pedestres cozidos
- `project/render/shaders/props.gdshader` — mobiliário e árvores
- `project/render/shaders/road.gdshader` — vias, meios-fios e pintura
- `project/render/shaders/sky.gdshader` — céu em gradiente
- `project/render/shaders/terrain.gdshader` — terreno toon (override do Terrain3D)
- `project/render/shaders/water.gdshader` — água (ondas, Fresnel e espuma)
- `project/tests/core/camera_input_test.gd` · `project/tests/core/camera_input_test.tscn` — câmera por entrada real
- `project/tests/core/road_commands_test.gd` · `project/tests/core/road_commands_test.tscn` — comandos de via/zona/demolir pelo store
- `project/tests/core/save_undo_test.gd` · `project/tests/core/save_undo_test.tscn` — desfazer/refazer e ida e volta do save
- `project/tests/visual/capture.gd` · `project/tests/visual/capture.tscn` — capturas da cidade real
- `project/tests/visual/capture_features.gd` · `project/tests/visual/capture_features.tscn` — capturas de recursos dirigidas como o jogador
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
- `tools/assets/cook_people.py` — pessoas cozidas (MakeHuman CC0 + caminhada Rocketbox MIT)
- `tools/assets/ktx2_to_png.mjs` — KTX2 cozido do Roadcraft → PNG que o Godot importa
- `tools/assets/make_road_profiles.py` — gera road_profiles.json a partir da fonte do Roadcraft
- `tools/build/build.sh` — compilar, testar, capturar, exportar e empacotar
- `tools/gates/gate_clones.py` — código duplicado
- `tools/gates/gate_code.py` — camadas, padrões proibidos e órfãos
- `tools/gates/gate_data.py` — schemas das tabelas
- `tools/gates/gate_defs.py` — números mágicos, cópias de dados, donos e estado na UI
- `tools/gates/gate_docs.py` — mapa deste documento
- `tools/gates/gate_files.py` — lixo, documento único, tamanhos e cabeçalhos
- `tools/gates/gate_tuning.py` — tuning ↔ JSON e espelhos
- `tools/gates/hook_post_edit.py` — gates do arquivo editado
- `tools/gates/hook_pre_write.py` — bloqueia documentos novos
- `tools/gates/owners.json` — registro de donos únicos
- `tools/gates/rules.json` — configuração dos gates
- `tools/gates/run_gates.py` — executa todos os gates
<!-- MAPA:FIM -->

## Pendências
- Pontes têm tabuleiro e pilares, mas não há vão estaiado nem rodovia elevada sobre terra (viadutos).
- Ônibus/linhas de transporte, metrô e escolas com alunos ainda não são simulados (ônibus só aparecem como veículos).
- Contorno, iluminação e os shaders novos só foram validados em headless/Compatibility; falta revisar o visual em
  Forward+ numa GPU real (asfalto cinza, fachadas, HLOD e peças).
- Memória do mapa de 10 km com relevo: números antigos (~1,9 GB com tudo desenhado); o HLOD por superchunk reduz
  o nível distante, mas mapas de 16–24 km ainda não foram medidos.
- Interface "em construção": camadas Grade e Interior, Sobre, aba Mover, Construir (Modelos, Massa, Fachada, Peças,
  Telhado, Interior, Cores), Paisagem (Solo, Natureza, Céu, Rua, Pessoas; rio e Aplainar mapa), paradas/estações/linhas,
  pistola/bomba. Salvar é um arquivo por cidade (sem gerações/quarentena do Roadcraft); a água não seca sob o relevo.
- Rotatória: sem fundir trecho curto até a junção (Roadcraft < 14 m), sem placas de dê a preferência nos acessos.
- Vias do jogador: altura segue o chão (sem greide de menor terraplenagem, pontes/túneis, rampa íngreme, talude/muro);
  modo Livre e Quarteirões ainda não; demolir via deixa o corredor de terreno.
- Tráfego: sem estacionar/docas/embarque, semáforos só com ciclo fixo (sem atuação nem botão de pedestre); rotas
  síncronas no tick (sem threads). Medir com `capture.tscn -- --soak=5` (retidos > 20 s por minuto, chegadas/perdidas).
- Pedestres: sem multidão (desvio lateral, filas na guia), ciclistas, portas dos prédios e passageiros de ônibus.
- Pedestres: esqueleto rígido (só o osso dominante): a mistura dos 4 pesos (CUSTOM1) saiu esticada no Compatibility;
  só a caminhada (sem parados, celular, guarda-chuva, sentados); o pacote precisa levar o texto MIT da animação Rocketbox
  (`project/assets/people/LICENSES.json`). Os 38 projetos de marcos do Roadcraft (`data/lots-roadcraft`) e o estádio ainda
  não foram portados; muros de divisa entre prédios colados ainda não são desenhados.
