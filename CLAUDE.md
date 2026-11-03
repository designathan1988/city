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
  `tr("CHAVE")` com `data/i18n/strings.csv`; botões, estatísticas e opções da HUD vêm de `data/ui/*.json`.
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
`DataTables` lê `data/**` → `CityStore.boot` envia catálogo ao `CityCore` (rejeita chave ausente) →
`Simulation` roda em thread própria (tick fixo `sim_tick_hz`, sob `world_mutex`), pathfinding em threads
dedicadas (`PathService`, `graph_mutex` compartilhado) → mudanças vão para o `ChangeLog` (único canal de
"sujo") → `RenderWorld.sync` no thread principal consome o log e reconstrói chunks com orçamento por quadro.

## Máquinas de estado
- `GameFlow.State`: BOOT → MAIN_MENU → LOADING → PLAYING ⇄ PAUSED → MAIN_MENU/QUITTING (tabela `TRANSITIONS`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram/saem de STOPPED.
- `ToolController.Tool`: NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT (só `set_tool`; botão segurado → `drag`/`tick`, solto → `release`); `RoadTool.Step`: START → CONTROL (curva) → END → (encadeia) END.

## Mapa do código
<!-- MAPA:INICIO -->
**Raiz e ferramentas**
- `.claude/settings.json` — hooks: gates no início da sessão, a cada edição e ao parar; memória automática desligada.
- `tools/gates/run_gates.py` — executa todos os gates; `tools/gates/rules.json` — configuração; `tools/gates/owners.json` — donos únicos.
- `tools/gates/gate_files.py` (lixo, doc único, tamanhos, cabeçalhos, raiz) · `tools/gates/gate_code.py` (camadas, padrões proibidos, órfãos)
- `tools/gates/gate_defs.py` (números mágicos, cópias de dados, donos, estado na UI) · `tools/gates/gate_clones.py` (código duplicado)
- `tools/gates/gate_data.py` (schemas) · `tools/gates/gate_docs.py` (este mapa) · `tools/gates/gate_tuning.py` (tuning ↔ JSON)
- `tools/gates/hook_pre_write.py` (bloqueia novos documentos) · `tools/gates/hook_post_edit.py` (gates do arquivo editado)
- `tools/gates/schemas/**` — um schema por tabela de dados.
- `tools/build/build.sh` — compilar, testar, capturar, exportar, empacotar.
- `tools/assets/cook_people.py` — converte as pessoas cozidas do Roadcraft (MakeHuman CC0 + caminhada Rocketbox MIT) em `project/assets/people/people.bin` (procedência em `project/assets/people/LICENSES.json`).

**Núcleo C++ — `native/`**
- `native/CMakeLists.txt` (godot-cpp 4.7, saída em `project/bin`) · `native/toolchain-mingw.cmake` (Windows via llvm-mingw).
- `native/src/core/math.h` vetores 2D · `native/src/core/grid.h` chunks e hash espacial · `native/src/core/random.h` hash/ruído
- `native/src/core/bytes.h` leitor/escritor binário · `native/src/core/units.h` unidades de tempo/medida · `native/src/core/color.h` cor hex
- `native/src/sim/catalog/catalog.h` tipos do catálogo (sem valores) · `native/src/sim/catalog/tuning_fields.h` nomes do tuning
- `native/src/sim/state/city_state.h` fonte única + `revision` (dono de "trabalho não salvo") · `native/src/sim/state/change_log.h` canal de mudanças
- `native/src/sim/terrain/heightfield.h` / `native/src/sim/terrain/heightfield.cpp` alturas, modificadores, ladrilhos esculpidos (`tile_dirty`) · `native/src/sim/terrain/terrain_brush.cpp` pincel de Relevo e formas (Roadcraft brush.ts)
- `native/src/sim/roads/road_network.h` / `native/src/sim/roads/road_network.cpp` grafo viário · `native/src/sim/roads/curve.h` / `native/src/sim/roads/curve.cpp` retas e arcos
- `native/src/sim/roads/intersection_geometry.cpp` cruzamentos (filetes)
- `native/src/sim/zones/zone_grid.h` / `native/src/sim/zones/zone_grid.cpp` células de 8 m (4 de profundidade)
- `native/src/sim/buildings/building_store.h` / `native/src/sim/buildings/building_store.cpp` registros e `lot_fits` (dono da regra do lote)
- `native/src/sim/buildings/building_rules.h` / `native/src/sim/buildings/building_rules.cpp` dono de altura/andar/pódio · `native/src/sim/buildings/growth.h` / `native/src/sim/buildings/growth.cpp` crescimento
- `native/src/sim/nature/tree_store.h` / `native/src/sim/nature/tree_store.cpp` árvores compactas por célula
- `native/src/sim/catalog/traffic_params.h` parâmetros de tráfego (`data/catalog/traffic.json`) · `native/src/api/traffic_loader.cpp` leitura estrita
- `native/src/sim/traffic/traffic_state.h` grafo de faixas, veículos, pedestres · `native/src/sim/traffic/traffic_system.h` interface · `native/src/sim/traffic/traffic_system.cpp` passo e poses
- `native/src/sim/traffic/lane_graph.cpp` faixas e conectores com conflitos · `native/src/sim/traffic/lane_geometry.cpp` pontos de faixa/conector/calçada
- `native/src/sim/traffic/junction_control.cpp` controle de cruzamento, semáforos, preferência · `native/src/sim/traffic/routing.cpp` A* por segmento dirigido (custo BPR pela lotação), desvio e reconsideração de rota · `native/src/sim/traffic/astar.h` A* único (carros e pedestres)
- `native/src/sim/traffic/junction_admission.cpp` antitravamento: saída livre, compromisso na linha/caixa livre, reserva da saída, liberação de nó travado, governador da frota
- `native/src/sim/traffic/demand.cpp` frota e viagens por zona · `native/src/sim/traffic/vehicle_motion.cpp` IDM e conectores · `native/src/sim/traffic/lane_change.cpp` MOBIL
- `native/src/sim/traffic/pedestrians.cpp` pedestres com destino (nascer, andar, esperar na guia) · `native/src/sim/traffic/ped_routes.cpp` A* a pé e caminho de esquina pelas faixas · `native/src/sim/traffic/ped_crossing.cpp` fase de pedestre, carros na faixa, carros cedem a quem atravessa
- `native/src/sim/economy/economy_state.h` números da cidade (`DEMAND_MAX`, `affords`) · `native/src/sim/economy/economy_system.h` interface
- `native/src/sim/economy/budget.cpp` estatística, demanda, utilidades, dinheiro · `native/src/sim/economy/coverage.cpp` cobertura, poluição, valor do solo
- `native/src/sim/commands/commands.h` API de comandos do jogador (encaixe, plano de via, construir, demolir, zonear)
- `native/src/sim/commands/road_plan.cpp` encaixe e validação de vias (= prévia; recusas do Roadcraft plan.ts) · `native/src/sim/commands/road_commands.cpp` construir via (corta em cruzamentos e em trechos ≤ `road_max_seg_m`, cobra) e demolir (devolve `economy_refund_share`)
- `native/src/sim/commands/road_edit.h` / `native/src/sim/commands/road_edit.cpp` dividir trecho, âncora → nó, prédios sob a via, terreno, fechamento da edição (nós, lotes, fila de crescimento) · `native/src/sim/commands/zone_commands.cpp` pincel de zona
- `native/src/sim/commands/edit_journal.h` / `native/src/sim/commands/journal_ops.cpp` diário de edições (ops `j_*`, únicos mutadores dos comandos) · `native/src/sim/commands/edit_history.cpp` pilhas desfazer/refazer, traços, limites `history_*`
- `native/src/sim/commands/road_tabs.cpp` Dividir e Melhorar (rua inteira, perfil assimétrico) · `native/src/sim/commands/roundabout.cpp` rotatória (corta vias, acessos em terreno livre) · `native/src/sim/commands/land_commands.cpp` pincel de relevo, prédio desenhado, editar prédio
- `native/src/sim/simulation/simulation.h` / `native/src/sim/simulation/simulation.cpp` relógio, tick, velocidade · `native/src/sim/simulation/city_growth.h` / `native/src/sim/simulation/city_growth.cpp` prédios crescem nos lotes livres das células editadas (`sim_growth_per_s`)
- `native/src/sim/simulation/world_setup.h` / `native/src/sim/simulation/world_setup.cpp` fundar cidade
- `native/src/sim/persistence/save_format.h` formato do arquivo da cidade · `native/src/sim/persistence/save_write.cpp` gravar · `native/src/sim/persistence/save_read.cpp` ler (terreno do seed + ladrilhos esculpidos)
- `native/src/render/render_world.h` / `native/src/render/render_world.cpp` consome o ChangeLog · `native/src/render/rs_util.h` instâncias RenderingServer · `native/src/render/chunk_queue.h` chunks de vias, mobiliário e prédios só no alcance da câmera (`render.*_end_m`), liberados ao sair
- `native/src/render/mesh_builder.h` / `native/src/render/mesh_builder.cpp` primitivas · `native/src/render/model_library.h` / `native/src/render/model_library.cpp` massas e modelos de data/render/models.json
- `native/src/render/building_gen.h` / `native/src/render/building_gen.cpp` `facade_layout` (dono da fachada), massa, telhado, módulos, HLOD
- `native/src/render/building_renderer.h` / `native/src/render/building_renderer.cpp` chunks e superchunks · `native/src/render/road_mesher.h` / `native/src/render/road_mesher.cpp` vias, faixas, postes, árvores
- `native/src/render/tree_renderer.h` / `native/src/render/tree_renderer.cpp` árvores por célula
- `native/src/render/buildings/civic.h` / `native/src/render/buildings/civic.cpp` escola, igreja, hospital/clínica · `native/src/render/buildings/civic_services.cpp` posto, mercado/shopping, prefeitura, polícia/bombeiros · `native/src/render/buildings/open_lots.cpp` praça, parque, quadra (e onde as árvores podem ficar)
- `native/src/render/buildings/roof_terrace.cpp` terraços usados (deck, vasos, mesas, espreguiçadeiras, pérgola, caixa d'água) · `native/src/render/buildings/roof_pitched.cpp` telhados inclinados e dente de serra
- `native/src/render/agents/agent_renderer.h` / `native/src/render/agents/agent_renderer.cpp` veículos e pedestres (MultiMesh) · `native/src/render/agents/agent_meshes.h` malhas
- `native/src/render/agents/car_body.cpp` carroceria loftada · `native/src/render/agents/heavy_meshes.cpp` ônibus (portas, espelhos), caminhão (degraus, para-lamas, tanque), moto · `native/src/render/agents/agent_parts.cpp` rodas (giram no shader) e peças
- `native/src/render/agents/people_asset.h` / `native/src/render/agents/people_asset.cpp` pessoas MakeHuman cozidas (`project/assets/people/people.bin`) · `native/src/render/agents/people_renderer.h` / `native/src/render/agents/people_renderer.cpp` pedestres com esqueleto no shader (`people.gdshader`), 2 níveis
- `native/src/render/overlays/overlay_renderer.h` / `native/src/render/overlays/overlay_renderer.cpp` camadas: cores das zonas e congestionamento · `native/src/render/terrain_bridge.h` / `native/src/render/terrain_bridge.cpp` envio ao Terrain3D
- `native/src/api/city_core.h` / `native/src/api/city_core.cpp` classe CityCore · `native/src/api/city_core_commands.cpp` comandos · `native/src/api/city_core_query.cpp` consultas/sondas
- `native/src/api/city_core_edit.cpp` desfazer/refazer, traços, abas de via, relevo, prédio desenhado · `native/src/api/city_core_persist.cpp` salvar/abrir (zstd) · `native/src/api/edit_result.h` resultado e `run_edit`
- `native/src/api/city_core_bind.cpp` bindings · `native/src/api/catalog_loader.h` / `native/src/api/catalog_loader.cpp` Dictionary → Catalog (estrito) · `native/src/api/register_types.cpp` entrada

**Godot — `project/`**
- `project/project.godot` autoloads (Settings, CityStore, GameFlow), variável global `night` · `project/export_presets.cfg` exportação Windows (único alvo).
- `project/game/boot.tscn` cena vazia inicial · `project/game/game.tscn` + `project/game/game.gd` cena de jogo.
- `project/game/flow/game_flow.gd` máquina de estados da aplicação · `project/game/store/city_store.gd` único dono do CityCore (`snapshot`)
- `project/game/store/formatters.gd` formatação (`format_money`) · `project/game/data/data_tables.gd` único leitor de `data/**`
- `project/game/settings/settings.gd` qualidade gráfica · `project/game/camera/city_camera.gd` câmera (rig do Roadcraft: segurar o chão sob o cursor)
- `project/game/camera/camera_gestures.gd` mouse: direito gira/inclina, meio ou Shift+direito arrasta o chão, inércia · `project/game/camera/camera_motion.gd` teclas e roda (zoom no cursor, suavizado) · `project/game/camera/camera_profile.gd` lente, inclinação e altura do olho pela distância
- `project/game/tools/tool_controller.gd` máquina de estados das ferramentas, clique → chão · `project/game/tools/tool_base.gd` base e textos de recusa
- `project/game/tools/road_tool.gd` via reta/curva com prévia · `project/game/tools/zone_tool.gd` pincel de zona · `project/game/tools/demolish_tool.gd` demolir · `project/game/tools/inspect_tool.gd` inspecionar
- `project/game/tools/upgrade_tool.gd` Melhorar · `project/game/tools/split_tool.gd` Dividir · `project/game/tools/roundabout_tool.gd` Rotatória · `project/game/tools/terrain_tool.gd` Relevo · `project/game/tools/lot_tool.gd` Construir › Desenhar
- `project/render/city_world.gd` terreno, céu, sol, água, contorno e materiais · `project/render/daylight.gd` dia/noite · `project/render/preview_line.gd` prévias
- `project/render/shaders/**` — fachada (interior mapping), telhado, props, vias, zonas, água, contorno, terreno toon, luz toon.
- `project/ui/theme/theme_builder.gd` Theme a partir dos tokens (variações por estilo do Roadcraft) · `project/ui/hud_widgets.gd` fábricas de botões/rótulos
- `project/ui/roadcraft_hud.gd` barra de cima, dock, dica, bússola · `project/ui/hud_drawer.gd` gavetas das categorias · `project/ui/hud_popover.gd` camadas, simulação, câmera, ajuda, menu, abrir
- `project/ui/hud_actions.gd` menu (salvar/abrir/carregar), desfazer/refazer, atalhos Ctrl · `project/ui/drawer_strips.gd` cartões e controles deslizantes das gavetas
- `project/ui/road_catalog.gd` catálogo de vias · `project/ui/road_swatch.gd` figura do perfil visto de cima · `project/ui/inspector_panel.gd` inspetor
- `project/ui/icons/**` ícones SVG do Roadcraft (brancos; a cor vem do tema)
- `project/ui/menus/main_menu.tscn` + `project/ui/menus/main_menu.gd` menu principal
- `project/data/catalog/**` tuning, mapas, vias, zonas, serviços · `project/data/render/**` modelos e paleta
- `project/data/ui/**` tokens, gavetas/ações/popovers (`project/data/ui/drawers.json`), barra antiga, HUD · `project/data/game/**` câmera, qualidade, dia/noite · `project/data/i18n/strings.csv` textos
- `project/tests/e2e/e2e.gd` + `project/tests/e2e/e2e.tscn` testes de ponta a ponta
- `project/tests/core/road_commands_test.gd` + `project/tests/core/road_commands_test.tscn` comandos de via/zona/demolir pelo store (sem tela)
- `project/tests/core/save_undo_test.gd` + `project/tests/core/save_undo_test.tscn` desfazer/refazer de cada edição e ida e volta do arquivo salvo (sem tela)
- `project/tests/core/camera_input_test.gd` + `project/tests/core/camera_input_test.tscn` câmera por entrada real: arrastar mantém o chão sob o cursor, zoom no cursor, giro, limites, teclas, clique direito cancela (sem tela)
- `project/tests/visual/capture_ui.gd` + `project/tests/visual/capture_ui.tscn` capturas da interface dirigida por cliques (gaveta, via, zona, crescimento, inspetor, demolir)
- `project/tests/visual/capture_features.gd` + `project/tests/visual/capture_features.tscn` capturas de desfazer, abas de via, relevo, Construir, camadas, salvar/abrir · `project/tests/visual/ui_driver.gd` entrada real (mouse, teclas) para as capturas
- `project/tests/visual/capture.gd` + `project/tests/visual/capture.tscn` capturas da cidade · `project/tests/visual/capture_game.gd` + `project/tests/visual/capture_game.tscn` + `project/tests/visual/game_driver.gd` capturas do jogo real
<!-- MAPA:FIM -->

## Pendências
- Pontes têm tabuleiro e pilares, mas não há vão estaiado nem rodovia elevada sobre terra (viadutos).
- Ônibus/linhas de transporte, metrô e escolas com alunos ainda não são simulados (ônibus só aparecem como veículos).
- Contorno e iluminação foram validados no renderizador Compatibility (sem GPU no ambiente de build);
  revisar o visual em Forward+ numa GPU real. Em especial: no Compatibility o COLOR de vértice chega já
  linear (vias e mobiliário decodificam só quando `OUTPUT_IS_SRGB` é falso); conferir que no Forward+ o asfalto
  fica cinza e não preto, e se o `lin(COLOR)` de `building.gdshader` também escurece as fachadas lá.
- Memória do mapa de 10 km com relevo (Compatibility/llvmpipe, 1280x720): ~1,9 GB residentes com tudo desenhado
  (0,7 GB de buffers de malha; antes passava de 3,5 GB e seguia crescendo), pico ~3,5 GB depois de 2 min de
  trânsito com 22 mil veículos e câmera passando por 9 vistas. O nível de perto
  dos prédios ainda custa dezenas de MB por chunk e o nível distante cobre 4,5 km; um HLOD por superchunk
  reduziria os dois. Mapas de 16 a 24 km não foram medidos.
- Interface "em construção": camadas Grade e Interior, Sobre, aba Mover, Construir (Modelos, Massa, Fachada, Peças,
  Telhado, Interior, Cores), Paisagem (Solo, Natureza, Céu, Rua, Pessoas; rio e Aplainar mapa), paradas/estações/linhas,
  pistola/bomba. Salvar é um arquivo por cidade (sem gerações/quarentena do Roadcraft); a água não seca sob o relevo.
- Rotatória: sem fundir trecho curto até a junção (Roadcraft < 14 m), sem placas de dê a preferência nos acessos.
- Vias do jogador: altura segue o chão (sem greide de menor terraplenagem, pontes/túneis, rampa íngreme, talude/muro);
  modo Livre e Quarteirões ainda não; demolir via deixa o corredor de terreno.
- Tráfego: sem modelo meso distante (tudo é micro, 20 Hz), sem estacionar/docas/embarque, semáforos só com ciclo fixo
  (sem atuação nem botão de pedestre), rotas síncronas no tick (sem threads). Antitravamento do Roadcraft feito
  (`junction_admission.cpp`); medir com `capture.tscn -- --soak=5` (retidos > 20 s por minuto, chegadas/perdidas).
- Pedestres: sem multidão (desvio lateral, filas na guia), ciclistas, portas dos prédios e passageiros de ônibus.
- Editar uma via reconstrói o grafo de faixas e reinicia toda a frota e os pedestres.
- Pedestres: esqueleto rígido (só o osso dominante): a mistura dos 4 pesos (CUSTOM1) saiu esticada no Compatibility e falta
  achar a causa; só a caminhada (sem parados, celular, guarda-chuva, sentados); o pacote precisa levar o texto MIT da animação Rocketbox
  (`project/assets/people/LICENSES.json`). Os 38 projetos de marcos do Roadcraft (`data/lots-roadcraft`) e o estádio ainda não foram portados;
  muros de divisa (empenas cegas) entre prédios colados ainda não são desenhados.
