# Plano do porte: interface

Passo C da correção. Vale para o que falta sobre os commits `6ee02b4` e `d90a018`. Nenhuma linha de código antes deste arquivo.

**Como provei quem é dono:** `converter/out/features/<feature>/boundary.json` (lista `own`) e a seção Escopo de `ports/<feature>/prompt.md`. Sem as duas provas, o item é meu.

## 1. Itens que não são desta sessão, com a prova
| Item | Dono | Prova |
|---|---|---|
| Camada Interior e botões de andar | `build` | `editor/buildingTool.ts#BuildingTool.cutView` só em `build/boundary.json`. Na interface só existe a linha do menu (`mountShell.seeInside`). Sem o corte, o interruptor não mostra nada: a linha fica "em construção". |
| `surfaceDetail` e a chave de mesmo nome | `landscape` | `render/mesh/detailLayer.ts#*` em `landscape/boundary.json`; escopo "a camada de detalhe de perto". |
| `canyonLight` | `landscape` | `render/canyonLight.ts#*` em `landscape/boundary.json`. |
| Chaves nuvens e clima; opção `cloudPass` | `landscape` | Escopo "céu, clima, nuvens"; já estão no branch `convert/landscape`, commit `1c604b4`. |
| Chave partículas; opção `exhaust` | `demolition` e `traffic` | `render/exhaust.ts#*` nos dois `boundary.json`. |
| Chave transporte | `transit` | Escopo "Ônibus, linhas e metrô". |
| Opção `wear` | `build` | Escopo "incluindo o desgaste de materiais (`src/render/wear.ts`)". |
| Opção `hairStrands` | `people` | `render/people/hairTexture.ts#hairStrandTexture` em `people/boundary.json`; o jogo não tem mechas. |
| Nível por peça do veículo | `traffic` | `render/vehicleLevels.ts#*` em `traffic/boundary.json`. O jogo tem dois níveis; ver Q4. |
| Linhas do painel de opções do Vias | `roads` | `project/ui/road_options.gd` já existe no branch `convert/roads`. |
| Inspetor: estacionamento | `traffic` | Escopo "Estacionar (rua e lote)". |
| Inspetor: altura das pontas, ponto de altura | `road-elevation` | Escopo "Pontes, túneis, viadutos e greide". |
| Inspetor: travessias, retorno, curva, painel de cruzamento, seção | `roads` | `ui/roads/*` em `roads/boundary.json`: 181 declarações medidas. |
| Nome da rua e numeração | `roads` | `world/roads/streetNames.ts#NAMES` e `#nameFor` só em `roads/boundary.json`. Sem eles não há nome a mostrar. |

**Campos que nada lê no próprio 4.0** (`unread: true` em `render/optimizationsQuality.ts`): `grassBlades`, `vegetationDistance`, `impostors`, `birds`, `rain`, `mist`, `waterfalls`. A ordem E citou pássaros, chuva, grama e névoa. Portar um campo sem leitor seria criar um número sem efeito: não entram na tabela. Fica registrado no relatório.

**`sharpen`:** vale 0 em todos os níveis, e o comentário do 4.0 proíbe nitidez na resolução nativa. Não há passe a portar. A chave de mesmo nome não teria efeito.

## 2. Qualidade (ordem E, itens "campos de nível")
Todo campo entra em `data/game/settings.json tiers`, é aplicado por `QualityTier.apply` e tem teste que lê o efeito no renderizador, não o dicionário. Teste novo: `tests/core/quality_test`.

| # | Campo | Solução | Dono | Teste | Foto |
|---|---|---|---|---|---|
| Q1 | `vegetation` (parte de 0 a 1) | Árvores soltas e matas caem por hash estável do item, nos dois níveis; árvores de rua e parque ficam, como no 4.0. Mudar o valor reconstrói os chunks. | `render/trees/tree_renderer` | Contagem de instâncias em 0, 0,5 e 1 | baixa e ultra, mesmo ângulo |
| Q2 | `detail_cutoff_zoom` (px/m, a escala do 4.0) | `CityCamera.pixels_per_metre()`; ao cruzar o limite, a máscara da câmera tira mobiliário, veículos e pessoas. Pintura por parâmetro global em `road.gdshader`. Só age ao cruzar, nada por quadro além de uma comparação. | `game/settings/render_switches.gd`, `city_camera.gd` | Câmera real, máscara antes e depois do limite | longe, com e sem |
| Q3 | `pedestrian_detail` | 0 usa só o nível distante; 1 e 2 usam os dois. A face animada, que separaria 1 de 2, não existe no jogo. | `render/agents/people_renderer` | Contagem por nível | perto, baixa e alta |
| Q4 | `occupant_zoom`, `vehicle_error_px`, `vehicle_whole_cap` | Alcance do nível com cabine = distância do `occupant_zoom`, vezes 0,5/erro: o erro de tela de uma forma fixa cai com a distância. O teto usa `nth_element` só quando passa do limite: ficam os mais perto. | `render/agents/agent_renderer` | Contagem por nível, com teto baixo | perto, baixa e alta |
| Q5 | `sky_clouds`, `cloud_shadows` | Ponto de entrada: campos na tabela, lidos por `Settings.quality()` a cada `changed`. A paisagem liga. | `game/settings/settings.gd` | Ouvinte de teste recebe os valores | nenhuma: sem efeito visível neste branch |
| Q6 | `post` (`postProcessing`) | Liga e desliga SSAO, brilho e SMAA de uma vez. | `quality.gd` | Estado do Environment | baixa e alta |
| Q7 | SMAA no Compatibility | O Godot não tem SMAA lá: a opção some da lista e o nível cai para MSAA, sem prometer o que não entrega. | `quality.gd`, `graphics_panel.gd` | Lista por renderizador | nenhuma |
| Q8 | `_doc` do atlas | Conferir `directional_shadow_atlas_set_size` e o fonte; corrigir o texto e o multiplicador se não se sustentar. | `settings.json`, `quality.gd` | Tamanho lido do `RenderingServer` | nenhuma |
| Q9 | `detailMemoryMB` | O jogo mede a memória por `render_cost`, mas não despeja por orçamento. Medir o nível perto; se ele não passa do limite do nível, o campo fica sem par, com a medida como motivo. | `building_renderer` | Medida por nível | nenhuma |

**Constantes do 4.0 nesta tabela** (`occupant_zoom`, `vehicle_whole_cap` etc.): não têm fonte primária. Saem de medição minha (contagens e GPU em amostras) ou ficam declaradas como escolha de comportamento.

## 3. Painel de efeitos (F7)
**Opções:** 32 no 4.0, 7 no jogo. Entram, só por dados e uma linha em `quality.gd`: `postChain`, `agentShadows`, `vegetation`, `detailProps`, `detailCutoff`, `pedestrianDetail`, `vehicleError`, `vehicleWhole`, `occupants`, `population`, `focusDetail`, `uploadEvery`, `waterMaterial`, `terrainLod`.
- `focusDetail`: chave `crowd` em `set_sim_switch`, dono `sim/traffic/ped_crowd.cpp`.
- `uploadEvery`: `RenderWorld::set_upload_every`.
- `waterMaterial`: troca de material em `city_world.gd`.
- `terrainLod`: `mesh_lods` do Terrain3D. **Exige pesquisa antes:** a documentação do Terrain3D ainda não foi lida. Se a API não der o efeito sem recriar o terreno, sai com esse motivo.

**Sem par, com motivo:** `opaqueSort`, `precision`, `frustumCull`, `shadowType` (do motor); `shadowEvery`, `farCascadeEvery`, `reliefEvery` (nenhuma API do Godot atualiza sombra direcional a cada N quadros; a confirmar no XML de `RenderingServer`); `castMasses` (perto não há malha de massa); `castTexel` (peças finas já não projetam).

**Chaves novas:**
- `bNightWindows` e `bGlass`: parâmetros globais em `building.gdshader`, declarados em `project.godot`.
- `bLots`: `BuildingRenderer` deixa de emitir o lote e reconstrói; é ferramenta de medida.
- `skyFill`: `ambient_light_sky_contribution`. `shadowUpdate`: sem API, some com motivo.
- `fillLight`, `pointLights`, `bFlags`, `bSignatures`, `farTraffic`, `props`, `bShell`, `bFrames`, `bProps`, `bFurniture`: conferir um a um na cobertura. Só entra o que tem objeto próprio no jogo.

**Decisão da sombra:** a chave de camada tira a camada do desenho e da sombra pela máscara da câmera, como o `switchedOff` do 4.0. Foto de antes e depois no passo H confirma ou derruba. Se a sombra sem objeto aparecer no 4.0, passo para a máscara da luz.

## 4. Inspetor (ordem E)
- **Núcleo:** `inspect` passa a devolver comprimento, sentido e tráfego ao vivo. Consulta nova `segment_traffic` e `node_traffic` em `api/city_core_query.cpp`, sob `traffic_mutex`: veículos, parados, soma de velocidade, capacidade (`max(1, comprimento/12)` por faixa, como `simAnswers.ts`) e volume.
- **Nó:** controle, ligações, movimentos, ciclo, estágio e lâmpadas, de `junction_control`.
- **Edição:** classe (`upgrade_road`), sentido (perfil de mão única ou dupla da mesma família), inverter (novo `OpKind::REVERSE`, troca as pontas da via com desfazer), dividir no meio (`split_road`), duplicar (`duplicate_road`, cópia paralela ao lado) e demolir.
- **Dono:** `sim/commands/road_tabs.cpp`, com `road_duplicate.cpp` se o limite de linhas pedir; `ui/inspector_panel.gd` e `ui/inspector_rows.gd`.
- **Dados:** rótulos em `strings_ui.csv`.
- **Teste:** `tests/core/inspector_test`: editar, desfazer, refazer e conferir o estado; números contra a contagem da sonda `traffic_probe`.
- **Foto:** via e cruzamento, contra a referência do 4.0.

## 5. Barra, menu e camadas
| Item | Solução | Dono | Teste | Foto |
|---|---|---|---|---|
| Pintura (br, us, eu, classic) | Linha no menu; `marking_style` no `CityState`, salvo e com desfazer; cores por estilo em `streets.json`; reconstrói as vias. Regras do estilo são `roads` (`markingStyle.ts`): trago só as cores (`STYLE_PAINT`). Desenhos próprios de cada estilo ficam com `roads`. | `sim/commands`, `render/roads/road_look` | Estado e save | os 4 estilos |
| Aviso de cruzamento impossível | `impossible_nodes`: nós com menor vão entre braços abaixo do limite de `legAngles.ts`. Botão na barra com a contagem; clique leva ao nó no Inspecionar. | `sim/roads/junction`, `hud_top_bar.gd` | Criar nó com ângulo agudo pelo comando real | barra com aviso |
| Camada Grade | Grade de 10 m por parâmetro no `terrain.gdshader`, ligada pela linha da camada. | `render/city_world.gd` | Parâmetro e foto | com e sem |
| F8, mapa de mudanças | Lista de mudanças do `edit_history` na aba Mundo do F9. Sobre o mapa, as células mudadas por `overlay_renderer`. | `ui/health_panel.gd`, `render/overlays` | Editar e contar | F8 ligado |
| Abrir mapa externo | `DisplayServer.file_dialog_show` (nativo) com filtro do nosso formato; cópia para os saves e abre. Arquivo ruim vai para o aviso, sem perder a cidade aberta. | `save_slots.gd`, `hud_actions.gd` | `load_from_path` com arquivo bom e corrompido | nenhuma: diálogo do sistema |
| Pop-up Câmera | Zoom em px/m, igual ao 4.0, e centro. | `pop_bodies.gd` | Leitura | Câmera |
| Simulação | Rede (vias, nós). | `pop_bodies.gd` | Leitura | Simulação |

## 6. Teclas (`data/ui/keys.json`, `game_keys.gd`)
Ctrl+D duplica; Delete e Backspace; `[ ]` raio; `{ } - = + _` força; 1 a 9 modo do terreno com o Relevo aberto; letras `p g f m`; F8.
- **Câmera:** conferir `CameraMotion` contra `cameraMotion.ts`: W A S D, setas, Q E, PageUp/Down, = e -, Shift+Q/E.
- **PageUp/Down no nó suave:** `road-elevation` (altura). A câmera mantém essas teclas.
- **Teste:** por entrada real em `shell_test` e `camera_input_test`.

## 7. Save e robustez
| Item | Solução | Dono | Teste |
|---|---|---|---|
| Quarentena | Nome com data e hora, nunca sobrescreve. | `save_slots.gd` | Duas quarentenas seguidas |
| Save falho | `hint.saveFailed`, no máximo 1 por minuto. | `hud_actions.gd` | Duas falhas seguidas |
| Autosave | O 4.0 grava 0,7 a 3 s depois da edição porque grava por pedaços. Aqui o save inteiro: medir o tempo de encode no mapa 0 antes de mudar os 120 s. Gravar ao perder o foco, como `visibilitychange`. | `city_store.gd` | Notificação real |
| Confirmações | `confirm.newMap`, `newWorld`, `newLargeWorld` por `hud_dialog.gd`, só com trabalho a salvar. | `hud_actions.gd` | Clique real |
| `normalizeSettings` | Valores salvos voltam ao mais perto do que o menu oferece. | `session.gd` | Save com valor fora da lista |
| Logger | `health_log.gd` tem `Mutex`; conferir que nada imprime nos virtuais. | `health_log.gd` | Leitura e teste |

## 8. Saúde (F9)
Três abas (saúde, mundo, estado); copiar para a área de transferência; detalhe ao clicar; ponto de cor; setas e Home/End; sessão anterior; `slowFrame` nomeando o sistema pela sonda `render_cost`; `stepBudget`; `noteSimulationIssues`.
- **Dono:** `ui/health_panel.gd`, dividido em `ui/health_tabs.gd` se passar do limite.
- **Teste:** `shell_test`. **Foto:** cada aba, contra `22-health`.

## 9. Idioma, dicas, câmera
- **Idioma ao vivo:** `NOTIFICATION_TRANSLATION_CHANGED` em `RoadcraftHud`, que refaz barra, gaveta, dock, popover e inspetor abertos. Teste: troca com tudo aberto, nenhum texto antigo.
- **Dica própria:** tooltip do tema; some se o nome já está na tela. Toast 2,4 s. Lista suspensa com setas, Home, End, Esc.
- **Câmera:** auditar contra `camera.ts`: clique direito curto cancela, recuo de prédio à frente, centro preso à borda. O que faltar entra em `city_camera.gd`, com teste por entrada real. Já tem: inércia, perspectiva e ortográfica.

## 10. Governador e padrão gráfico (ordem F)
- Rodar o governador no jogo de verdade, não só com amostras sintéticas.
- `tests/perf/tier_gpu.gd`: para cada nível, `viewport_get_measured_render_time_gpu` em amostras nas mesmas vistas, mais contagens por `RenderingServer.get_rendering_info` (objetos, primitivas, chamadas).
- **Regra da decisão:** a GPU está dividida com outras sessões, o que infla o tempo. Se `ultra` couber folgado em 16,7 ms, a prova vale. Se não couber, `default` e `gpu_rules` descem para o maior nível que coube.
- **Limites:** janela do governador e `gpu_rules` não têm fonte. A janela sai da medida do intervalo entre quadros; as regras ficam só com o que medi nesta RTX 3060, o resto vai para o relatório como sem prova.
- Comando para a máquina quieta no relatório.

## 11. Thunder
Ponto de entrada `Thunder.play(metros, volume)` testado: atraso pela velocidade do som, filtro e ganho pela distância, contados no player.

**Colisão achada:** `convert/landscape` tem `render/thunder.gd` com a mesma `class_name Thunder`, já chamada por `weather_view.gd`. O rastreamento põe `ui/thunder.ts` só na interface, mas a sessão da paisagem portou. Os dois branches juntos dão erro de classe duplicada. Não mexo no outro clone: mantenho o meu, como a ordem pede, e o relatório diz qual linha tirar na integração.

## 12. Defeitos das fotos apontados pela coordenadora
| # | Defeito | Solução | Dono | Prova |
|---|---|---|---|---|
| 1 | Início, Sobre e Opções gráficas sem cortina escura sobre a cidade | Achar por que não escurece no jogo real e corrigir a causa | `ui/start_screen.gd`, `ui/hud_dialog.gd` | Foto contra `01-start` |
| 2 | Câmera mostra "—" em Zoom e Centro | Leitura ligada: "100 %" e "X 0 · Y 0" | `ui/hud_popover.gd` | Foto contra `25-pop-camera` |
| 3 | Ajuda sem "Esta ferramenta" | Seção por ferramenta, em dados | `ui/pop_bodies.gd`, `data/ui/help.json` | Foto contra `30-pop-help` |
| 4 | Profiler fora da tela | Corrigir âncora e tamanho | `ui/profiler_hud.gd` | Foto |
| 5 | "Heap JS" e "1600 × 900" cortado; 379 ms de CPU a 20 quadros | Rótulo do jogo, largura pelo conteúdo; ler o que o número mede e explicar no relatório | `ui/effects_panel.gd` | Foto, medida |
| 6 | Saúde fotografada vazia | Foto com eventos reais, nas três abas | `tests/visual/capture_shell.gd` | Foto |
| 7 | Zonas com "Misto" a mais e sem "Densidade"; rotatória 22 m (4.0: 40 m); "Cores das zonas" desligado | Conferir no 4.0 e igualar | `data/ui/drawers.json`, `ui/hud_drawer.gd`, `hud_popover.gd` | Foto contra `zones`, `layers` |
| 8 | Foto do "toast" comparada com a dica de mouse do 4.0 | Comparar cada um com o seu par: aviso com aviso, dica com dica | `tests/visual/capture_shell.gd` | Duas fotos |

**Regra de foto (ordem do usuário, pela coordenadora):**
- Comando de foto, captura ou teste: no máximo 600 s. Estourou: `taskkill /PID <pid> /T /F`, achar a causa, nunca relançar igual.
- Nunca esperar mais de 120 s em primeiro plano: segundo plano e log a cada 60 a 90 s.
- Foto só depois de código completo, gates e build, em lista escrita antes; no máximo 2 rodadas completas por tema, depois só as cenas afetadas.
- Nada de varrer parâmetro olhando foto.
- Toda foto do relatório em `ports/interface/evidence/`. As `evidence/features/ui_*` são anteriores ao código final: regenerar.

**Pedidos seguintes da coordenadora:**
- `feat_terrain_1_chapada` e `feat_terrain_2_desfeito` mudam só 0,84 % dos pixels. Rodar a mesma sequência no clone limpo (`ports/demolition/workspace`, o master): se lá também não aparece, o defeito é do relevo (Porte 3) e eu só aviso; se lá funciona, é regressão minha e eu corrijo. Responder em uma linha.
- `feat_roundabout_2_construida`: refotografar com a câmera sobre o ponto.

## 13. O que a cobertura revelou
- **Gesto de dois dedos:** `camera_gestures.gd::pinch`, por `InputEventScreenDrag`. Sem tela de toque aqui: teste por evento sintético, declarado como tal.
- **Câmera nunca dentro de prédio:** `city_camera.gd::_pull_back` com a consulta `roof_at`; recua com tempo próprio (`PULL_*`).
- **Lista suspensa própria:** `hud_widgets.gd::dropdown`, com setas, Home, End e Esc.
- **Dica:** `hud_widgets.gd::tip_for`, some se o nome já estiver na tela.
- **Bordas da gaveta:** `hud_drawer.gd::_fade_edges`.

## 14. Pedidos da coordenadora depois do plano
- **Oclusão por cena:** `render/buildings/occluders.*`, paredes por chunk só no nível perto, chave `occlusion` e opção no F7. Liga ou não pelo número de `tier_gpu`.
- **Chave do `canyonLight`:** parâmetro global `canyon_light`, o passe é da Porte 3.
- **Botões de luz:** assinatura combinada com a Porte 3 (`set_sky_mode`, `sky_mode`, `follow_clock`).
- **Relatório:** tabela do que cada nível liga, lida de `render/quality.ts`.
- **Demolição e bomba:** saíram desta sessão, são da Porte 7.

## 15. Fechamento
1. `coverage.md` refeita declaração a declaração (passo D). Vem antes do código porque é ela que mostra o que ainda falta: o que aparecer lá entra neste plano.
2. Código, nesta ordem: saves, teclas, núcleo e inspetor, qualidade, painel, saúde, idioma.
3. Commits por tema sobre `a11384a`, cada um compilando.
4. Gates, release e debug, testes, capturas, fotos uma a uma.
5. Novo `report.md`.
