# Registro do goal: "Implemente e corrija todas as pendências"

- Início: 2026-11-05
- Código trabalhado: worktree `C:\Users\Jonathan\Desktop\city-worktrees\rootcause` (branch `fix/root-causes`). A aplicação em
  `C:\Users\Jonathan\Desktop\city` não é editada; este registro fica em `.claude/` (pasta do agente, fora do código).
- Escopo ("pendências"): os itens deixados em aberto no relatório anterior desta sessão (seção "O que não mudei"
  e observações do relatório). O backlog de funcionalidades da seção "Pendências" do CLAUDE.md do repositório-fonte
  (porte de prédios, noite, água, pontes...) é outro escopo (semanas de trabalho de conteúdo) e não está incluído.

## Passo 0 — Arquivo de registro
- Criado este arquivo: `C:\Users\Jonathan\Desktop\city\.claude\REGISTRO-GOAL.md`.

## Passo 1 — Identificação dos problemas
Ferramentas de investigação criadas (só medição): `project/tests/perf/pending_probe.gd/.tscn` (variações de sombra sem
limite de fps, ritmo de quadros `--pace`, cliques do inspetor num semáforo, trecho em pausa) e
`scratchpad/pending_run.ps1` (amostra, durante a pausa, as trocas de contexto por segundo de cada thread do Godot pelos
contadores do Windows). Mapa: metrópole (8.032 prédios), Forward+, RTX 3060.

Medições (logs em `%TEMP%\claude\...\scratchpad\step1_pending.log`, `step1_pace.log`, `step1_threads.txt`):
- Variações de sombra, vista parada, sem limite de fps, simulação NORMAL (frota crescendo durante a medição):
  como configurado GPU 5,79 ms, sombra 7,29 M prim./559 draws, visível 3,25 M; sem sombras GPU 3,83 ms;
  sem blend_splits sombra 6,30 M/287 draws; 2 divisões 9,35 M; 1 divisão 5,25 M.
- Ritmo (`--pace`, a frota crescendo): cap+vsync GPU 7,18 ms, sombra 10,1 M, visível 4,2 M → ... → cap+vsync de novo
  ~1 min depois: GPU 14,59 ms, sombra 23,5 M, visível 8,7 M. Com o limite de 60 fps o quadro fica em 16,67 ms enquanto
  a GPU cabe; na sonda anterior (velocidade máxima, 2.441 veículos) o quadro passou a 22 ms com GPU 21,6 ms.
- `agent_cost`: 3,61 M triângulos só de veículos; todo veículo fora de 140 m (cabin_radius_m) até 900 m
  (vehicle_radius_m) usa o nível "corpo" de 622–1.380 triângulos (sedan 1.284) e projeta sombra.
- Inspetor (clique num semáforo) com a simulação no máximo: média 0,146 ms, p95 0,88 ms, p99 3,33 ms, máx 4,71 ms.
- Pausa (30 s, simulação PAUSED): uma thread do Godot com 20,1 trocas de contexto/s (= sim_tick_hz 20).

### Lista de problemas
1. **P1 — Carga de GPU proporcional à frota inteira desenhada em detalhe fixo.** Cada carro entre 140 m e 900 m custa o
   nível "corpo" (~1.280 triângulos, com lanternas, grade, placas e rodas de 8 lados que somem em poucos pixels) e o
   desenha de novo em cada cascata de sombra que o MultiMesh (uma caixa de 1.800 m centrada na câmera, sem descarte por
   instância) toca: sombra ≈ 2,4–2,7× o visível. Efeito: GPU de 5,8 → 14,6 ms com a frota crescendo; 22 ms (abaixo de
   60 fps) na velocidade máxima. **Correção do relatório anterior:** lá atribuí os ~22 ms às sombras da cidade estática;
   a medição mostra que o motor é a geometria dos veículos (estáticos + sombras = ~2 ms).
2. **P2 — O inspetor espera o passo do tráfego.** `inspect_signal` toma `traffic_mutex` (segurado pela simulação o
   passo inteiro) para ler os verdes mínimos do cruzamento; o clique espera até 4,7 ms (p99 3,3 ms). Mesma família da
   causa corrigida no `snapshot`.
3. **P3 — A thread da simulação acorda 20×/s sem nada a fazer em pausa.** `Simulation::run` dorme até o próximo prazo
   em qualquer velocidade; parada, ela só confere a velocidade e dorme de novo (20,1 trocas de contexto/s medidas).
4. **P4 — Gate `MAPA_DESATUALIZADO` por `.claude/settings.json`.** O arquivo existe só no repositório-fonte
   (`C:\Projects\Roadcraft-windows\fonte`); o CLAUDE.md diz "só o usuário altera este arquivo". Não é código da aplicação.
5. **P5 — Commits pendentes.** O relatório perguntou se devia commitar; não houve resposta. Não é problema de código.

## Passo 2 — Pesquisa (fontes consultadas e relação com os problemas)
| # | Fonte | O que diz | Problema |
|---|---|---|---|
| 1 | https://docs.godotengine.org/en/stable/tutorials/3d/lights_and_shadows.html | PSSM divide o frustum em 2/4 áreas, cada uma com seu mapa; um objeto grande o bastante para estar nas quatro é desenhado 5 vezes (4 cascatas + cena); Blend Splits troca desempenho por transição suave; Cast Shadow do GeometryInstance3D decide quem projeta | P1 |
| 2 | https://docs.godotengine.org/en/stable/tutorials/performance/using_multimesh.html | Não há descarte de instâncias individuais num MultiMesh: tudo ou nada; dividir em áreas para descartar | P1 |
| 3 | https://docs.godotengine.org/en/stable/classes/class_multimesh.html | Instâncias muito distantes entre si: "every single instance will always render"; visible_instance_count limita o desenho | P1 |
| 4 | https://docs.godotengine.org/en/stable/tutorials/3d/mesh_lod.html | LOD automático vale por nó, não por instância de MultiMesh ("all instances will be drawn with the same LOD level"), escolhido pelo ponto do AABB mais próximo da câmera | P1 |
| 5 | https://docs.godotengine.org/en/stable/tutorials/3d/visibility_ranges.html | Faixas de visibilidade (HLOD) medem a distância ao centro do AABB do nó; malhas mais simples por faixa de distância | P1 |
| 6 | https://docs.godotengine.org/en/stable/tutorials/performance/optimizing_3d_performance.html | Ao longe, geometria mais simples (o jogador não nota); reduzir bastante a contagem de polígonos de modelos animados/numerosos | P1 |
| 7 | https://docs.godotengine.org/en/stable/classes/class_geometryinstance3d.html | SHADOW_CASTING_SETTING_OFF/ON e seu custo; lod_bias | P1 |
| 8 | https://cocalc.com/github/godotengine/godot/blob/master/servers/rendering/rendering_light_culler.cpp (e .h) | Luz direcional: um conjunto de planos de descarte por cascata; o teste é por AABB de instância (`cull_directional_light(bounds, light, cascade)`) | P1 |
| 9 | https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic | `std::atomic_load/atomic_store` de `shared_ptr` (C++11, depreciado no C++20): troca atômica do ponteiro publicado; leitor fica com uma cópia estável | P2 |
| 10 | https://en.cppreference.com/w/cpp/thread/condition_variable | `wait` com predicado (trata despertar espúrio); a variável compartilhada muda sob o mutex antes do `notify` para não perder o aviso | P3 |
| 11 | https://learn.microsoft.com/en-us/previous-versions/ms713413(v=vs.85) (timeBeginPeriod) | Temporizadores e esperas periódicas impedem estados de economia da CPU | P3 |
| 12 | https://learn.microsoft.com/pl-pl/previous-versions/visualstudio/visual-studio-2019/code-quality/ca1601 | Regra CA1601: evitar atividade periódica acima de 1×/s, que mantém a CPU ocupada e atrapalha a economia de energia | P3 |
| — | Godot `project/data/game/camera.json` (fov 55°→35°) e `tool_controller.gd` (metros por pixel) | base do cálculo do raio em que o detalhe some (P1) | P1 |

## Passo 3 — Causa, não efeito (raciocínio)
- **P1.** Medição: triângulos visíveis e de sombra crescem com a frota (4,2→8,7 M e 10→23,5 M), e `agent_cost` mostra
  3,6 M triângulos só de carros. Causa: a cadeia de detalhe dos veículos para no nível "corpo" (~1.280 tri.) e esse nível
  vale de 140 m a 900 m. Pelas fontes 2–4, o LOD automático do Godot não resolve isso, porque o MultiMesh é um nó só, com
  um AABB que contém a câmera (LOD 0 sempre) e sem descarte por instância. Pelas fontes 1 e 8, cada cascata que esse AABB
  toca desenha o MultiMesh inteiro de novo. Só quem posiciona cada carro (`AgentRenderer::update`, que já escolhe
  cabine × corpo pela distância) pode escolher o detalhe por instância: o defeito é que essa cadeia tem 2 elos e não 3.
  Pela fonte 6, a correção é um nível distante mais simples. O raio vem do tamanho do menor detalhe do nível corpo: a
  placa tem 0,11 m de altura e o fov mais fechado é 35° (camera.json). Em 1440 linhas,
  px = 0,11 × 1440 / (2 · d · tan 17,5°) < 1 ⇒ d > 250 m. Logo, a partir de 260 m o nível distante (sem lanternas, grade,
  placas, espelhos; menos estações e anéis; rodas de 4 lados) não muda nenhum pixel do detalhe. A sombra continua sendo
  projetada (mesma silhueta), então o visual das sombras não muda; só o custo cai. Desligar a sombra de carros distantes
  seria tratar o efeito: há sombra visível a 300 m.
  O que **não** é a causa: blend_splits e número de divisões (medidos: sem blend, 6,30 M contra 7,29 M; 2 divisões
  pioram). Mexer neles muda a qualidade calibrada da sombra sem atacar a geometria que cresce com a frota.
- **P2.** O inspetor precisa só dos verdes mínimos (`Junction::least`), que mudam apenas quando o tráfego replaneja um
  cruzamento (`build_junction`, chamado por `apply_network_changes`, `replan_signal` e `build_junctions`). Ler isso sob a
  trava de trabalho da simulação acopla um clique da UI a um passo inteiro. Mesma causa já corrigida no snapshot: falta
  publicação. Pela fonte 9: o tráfego publica um `shared_ptr<const>` imutável com os mínimos sempre que replaneja; o
  leitor pega uma cópia atômica e nunca espera o passo.
- **P3.** O laço `Simulation::run` é guiado só pelo relógio: em pausa não há trabalho, mas o prazo segue avançando e a
  thread acorda a cada 50 ms (medido: 20,1/s). Pelas fontes 10–12: esperar um evento (mudança de velocidade ou parada)
  numa condition_variable, com a velocidade mudada sob o mutex antes do aviso; ao voltar, o prazo recomeça de agora.
- **P4.** É configuração do ambiente do usuário, fora da aplicação: o próprio projeto proíbe outro autor de criar esse
  arquivo. Fica registrado e não é corrigido.
- **P5.** É uma decisão do usuário (commits só quando ele pedir). Fica registrado.

## Passo 4 — Plano
1. P1 (passos 1→3):
   - `project/data/render/agents.json`: `far_radius_m` = 260 (doc com o cálculo). Os arrays de detalhe de `car_body`
     ganham um 3º elemento (distante): `stations` 4, `arch_steps` 1, `end_steps` 0, `end_roll_steps` 0,
     `wheel_sides` 4. Atualizar o `_doc`.
   - `tools/gates/schemas/agents.schema.json`: `far_radius_m` obrigatório.
   - `render/agents/car_body.cpp`: `Loft::fine` (bool) vira `tier` (0 cabine, 1 corpo, 2 distante), indexando os arrays.
     O nível distante usa anéis mínimos (FAR_ROWS, preservando as faixas de vidro) e não chama `car_details`.
   - `render/agents/agent_meshes.h`: `build_car` recebe o nível.
   - `render/agents/agent_renderer.h/.cpp`: `FAR_LEVEL`, `LEVELS = 3`, malhas/draws/custos dos 3 níveis; escolha por
     distância (cabine < cabin_radius, corpo < far_radius, distante ≤ vehicle_radius), também para os parados; ônibus,
     caminhão, moto e bicicleta no nível distante com `wheel_sides[2]`; a sombra continua ligada.
2. P2: `sim/state/city_state.h`: `SignalLeast` + `shared_ptr<const SignalLeast> signal_least`.
   `lane_graph_internal.h/lane_graph_edit.cpp`: `apply_network_changes` devolve se replanejou algum cruzamento.
   `traffic_system.cpp`: `publish_signal_least` em `reset_traffic` e no passo quando houve replanejamento.
   `api/city_core_query.cpp`: `inspect_signal` lê a publicação com `std::atomic_load`, sem `traffic_mutex`.
3. P3: `sim/simulation/simulation.h/.cpp`: mutex e condition_variable; `set_speed` e `stop` mudam o estado sob o mutex
   e avisam; `run` espera enquanto PAUSED e recomeça o prazo de agora.
4. Ferramentas: `tests/perf/pending_probe` fica como sonda de diagnóstico (linha no mapa do CLAUDE.md).
5. P4 e P5: sem mudança de código (registrados).
6. Passo 6, uma única vez: compilar as duas DLLs (zero warnings), gates, suíte completa, `pending_probe` +
   amostragem de threads, `main_thread_probe`, `camera_pan` e captura de veículos para conferir o visual.

## Passo 5 — Código (sem testes, medições ou compilação nesta etapa)
Arquivos alterados neste goal (worktree `rootcause`), com o problema e o passo do plano:
| Arquivo | Mudança | Problema / plano |
|---|---|---|
| `native/src/render/agents/agent_meshes.h` | `AGENT_CABIN/BODY/FAR_LEVEL`, `AGENT_LEVELS = 3` (dono único dos níveis); `build_car(..., level)`; doc de 3 níveis | P1 / 4.1 |
| `native/src/render/agents/car_body.cpp` | `Loft::level`; os arrays de detalhe são indexados pelo nível; `FAR_ROWS` (uma faixa por região); distante sem `car_details`; `wheel_sides[level]` | P1 / 4.1 |
| `native/src/render/agents/agent_renderer.h` | `FAR_LEVEL`, `LEVELS = AGENT_LEVELS`, `level_of(dist2)`, `far_r_`; doc da causa (MultiMesh do tamanho da câmera, LOD do motor por nó, cascatas) | P1 / 4.1 |
| `native/src/render/agents/agent_renderer.cpp` | lê `far_radius_m`; confere um valor por nível nos 5 arrays; constrói os 3 níveis; escolhe o nível de carros em movimento e parados por `level_of`; laços de draw e custo nos 3 níveis (`agent_cost` ganha `*_far`) | P1 / 4.1 |
| `project/data/render/agents.json` | `far_radius_m: 260` (doc com o cálculo do pixel); 3º valor em `stations` (4), `arch_steps` (1), `end_steps` (0), `end_roll_steps` (0), `wheel_sides` (4) | P1 / 4.1 |
| `tools/gates/schemas/agents.schema.json` | `far_radius_m` declarado e obrigatório | P1 / 4.1 |
| `native/src/sim/state/city_state.h` | `SignalLeast` + `shared_ptr<const SignalLeast> signal_least`; `<vector>` | P2 / 4.2 |
| `native/src/sim/traffic/lane_graph_internal.h`, `lane_graph_edit.cpp` | `apply_network_changes` devolve `true` quando replaneja um cruzamento (sinais ou nós) | P2 / 4.2 |
| `native/src/sim/traffic/traffic_system.cpp` | `publish_signal_least` (único escritor) em `reset_traffic` e no passo após replanejamento | P2 / 4.2 |
| `native/src/api/city_core_query.cpp` | `inspect_signal` lê a publicação com `std::atomic_load`, sem `traffic_mutex` | P2 / 4.2 |
| `native/src/sim/simulation/simulation.h/.cpp` | `wake_mutex_` + `wake_`; `set_speed`/`stop` mudam sob o mutex e avisam; `run` espera em PAUSED e recomeça o prazo de agora | P3 / 4.3 |
| `project/tests/perf/pending_probe.gd/.tscn` (novos) | sonda de diagnóstico dos passos 1 e 6 | 4.4 |
| `CLAUDE.md` (cópia de trabalho, não versionada) | linha do mapa da sonda nova | 4.4 |
Nenhum arquivo deletado. P4 e P5: sem código (registrados no passo 1).

## Passo 6 — Testes e medições (uma única passada: `scratchpad\step6.ps1` + compilação e gates)
- **Compilação:** DLLs debug e release compiladas na primeira tentativa, 0 erros e 0 warnings.
- **Gates:** 1 problema, o P4 (`.claude/settings.json`, do ambiente), igual ao da baseline.
- **Suíte completa** (`run_tests.sh`): 16/16 ok (14 testes; luz e luz de prédio nos dois renderizadores; e2e incluído).
- **P1, GPU/veículos.** O sedan distante tem 476 triângulos, contra 1.284 no nível corpo (2,7× menos).

  `camera_pan` (cidade "town", mesmas vistas da rodada final anterior):

  | movimento | GPU antes → depois | visível | sombra |
  |---|---|---|---|
  | parada | 7,8 → 6,2 ms | 5,19 → 2,86 M | 13,5 → 6,5 M |
  | cruzar | 10,3 → 6,5 ms | | |
  | circular | 9,5 → 6,4 ms | | |
  | girar | 10,7 → 7,1 ms | | |
  | zoom | 5,4 → 4,8 ms | | |
  | perto | 11,1 → 8,7 ms | | |

  Memória idêntica (estática 302–336 MiB, vídeo 472–610 MiB, buffers 140–278 MiB), zero quadros acima de 33 ms.

  Metrópole:
  - `pending_probe`, sem limite de fps, como configurado: GPU 5,79 → 4,70 ms, sombra 7,29 → 4,91 M, visível 3,25 → 2,46 M.
  - `--pace`, depois de ~1 min de frota crescendo: GPU 14,59 → 10,87 ms, sombra 23,5 → 11,2 M, visível 8,7 → 4,5 M.
  - `main_thread_probe`, velocidade máxima: GPU p50 21,4 → 11,2 ms, quadro p50 21,8 → 16,95 ms (no limite de 60 fps).
    **A cauda continua:** quadro p95 24,0 ms, GPU p95 21,3 ms. Nesta rodada o passo do tráfego somou 5,6 s em 20 s
    (antes 1,8 s), ou seja, a máquina estava mais carregada. A cauda não foi eliminada e fica registrada como resultado parcial.
- **P2, inspetor.** Clique num semáforo com a simulação no máximo: média 0,146 → 0,032 ms, p99 3,33 → 0,09 ms,
  máx 4,71 → 0,16 ms.
- **P3, thread em pausa.** A amostragem é **inconclusiva**: nesta rodada havia workers ativos durante a pausa e 5 threads
  na faixa de 15–30 trocas/s, então não dá para isolar a thread da simulação pelo contador. Não repeti, pela regra de
  medir uma única vez. A correção se apoia no código e nos testes (todos os testes pausam e retomam a simulação).
- **Snapshot (regressão da correção anterior):** p99 0,56 ms, máx 1,6 ms. Chaves do pool com a fila vazia: 0.
- **Visual:** capturas 02/03/05/07/08/17/18/22 geradas sem erro em `scratchpad\shots_step6`. A comparação numérica com
  `dist/captures` **não é válida**: aquelas fotos são de 4/11, de outra versão do mapa e da luz (até o horizonte sem
  carros difere 21/255). Na inspeção visual não há artefatos. A garantia de não perder detalhe vem do cálculo do pixel
  (passo 3), não de uma comparação A/B, que esta passada não pôde fazer.

## Passo 7 — Parada para avaliação
Trabalho encerrado aqui, sem commits (P5 aguarda o usuário). Pendências que continuam abertas:
- P4: depende do usuário.
- Cauda da GPU na metrópole em velocidade máxima: p95 ainda acima de 16,7 ms.
- P3: falta uma medição conclusiva.
- Validação visual A/B do nível distante: falta uma referência atual.

---
# Ciclo 2 — "Implemente e corrija todas as pendências" (novo pedido, com passo 7 = verificar e recomeçar se falhou)
Escopo: o que o ciclo 1 deixou aberto: (A) cauda da GPU na metrópole em velocidade máxima (quadro p95 24 ms);
(B) medição de P3 inconclusiva; (C) validação visual A/B do nível distante sem referência atual. P4 (`.claude/settings.json`)
e P5 (commits) seguem dependendo do usuário.

## C2 Passo 1 — Identificação
Ferramentas: `pending_probe --tail` (novo modo: por quadro, GPU, primitivas visíveis/sombra, triângulos de veículos e
núcleo, com e sem limite de fps), `main_thread_probe` repetido, capturas A/B com `far_radius_m` 900 (nível distante
desligado) × 260 (`scratchpad\c2_step1.ps1`), isolamento sem pedestres (`people_radius_m = 0`, `c2_people.ps1`). Todos os
dados alterados para experimento foram restaurados (conferido: "restored: True").

Medições (logs em `scratchpad\c2_*.log`, imagens em `c2_far_900`, `c2_far_260`, `c2_diff_*.png`, `c2_far_street.png`):
- `--tail`, metrópole no máximo, frota já crescida: com limite os 10% mais lentos fazem quadro 16,70 ms (GPU 12,6),
  piores 17–19 ms; sem limite, GPU estável ~11,3 ms. Os quadros lentos não têm mais primitivas que os outros
  (6,0 M × 5,5 M visíveis), ou seja, não é geometria.
- `main_thread_probe` repetido (medição começa quando a velocidade vai ao máximo): quadro p95 22,5 ms, p99 27,0,
  máx 40,5; GPU p95 16,7, p99 23,2. **A cauda se reproduz.**
- Mesma sonda **sem pedestres desenhados**: quadro p95 16,8, p99 17,3, máx 21,5; GPU p95 14,6, p99 15,5, com média de GPU
  igual (11,8 × 11,0 ms). **A cauda some**: picos ligados ao desenho dos pedestres, não ao custo médio.
- Código: `PeopleRenderer::update` passa a cada quadro um buffer do tamanho exato a `MultiMeshDraw::set`, que chama
  `multimesh_allocate_data` toda vez que a contagem muda (variações × LODs, contagens mudando quase todo quadro). Os
  veículos não sofrem disso só porque `AgentRenderer` tem sua própria regra de folga (`ROOM_STEP`); trens
  (`transit_renderer`) e lentes de semáforo (`signal_lamps`) também reenchem por quadro sem folga.
- A/B do nível distante (mesmo código, `far_radius_m` 900 × 260): na rua ao fundo, onde estão os carros distantes,
  0,30% dos pixels diferem; a árvore amarela ao vento difere 4,23%; o carro perto 0,15%. As diferenças estão na
  animação (folhas, pedestres), não nos carros (inspeção ampliada: carros distantes idênticos).
- Composição dos triângulos de veículos (05_transito): cabine 159 carros = 1,61 M; corpo 640 = 0,82 M; distante
  2.743 = 1,33 M.
- P3: o contador de trocas de contexto do Windows não identifica qual thread é a da simulação; não existe contador de
  acordadas no próprio código.

### Lista de problemas (ciclo 2)
1. **C2-1 — Picos de quadro (cauda) causados por realocação de buffers de MultiMesh a cada quadro.** A regra de folga
   que evita realocar pertence a um usuário (`AgentRenderer`) em vez do dono do buffer (`MultiMeshDraw`); pedestres,
   trens e lentes realocam sempre que a contagem muda.
2. **C2-2 — P3 sem instrumento.** Não há como medir de forma conclusiva as acordadas da thread da simulação: falta um
   contador no próprio `Simulation`.
3. **(C) resolvido por medição:** o nível distante não muda os carros na imagem (A/B acima). Sem mudança de código.
4. P4 e P5: seguem dependendo do usuário.

## C2 Passo 2 — Pesquisa
| # | Fonte | O que diz | Problema |
|---|---|---|---|
| 1 | https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/storage_rd/mesh_storage.cpp | `_multimesh_allocate_data`: se o número de instâncias muda, libera o buffer da GPU (`free_rid`), cria outro zerado (`storage_buffer_create`), descarta os uniform sets (refeitos por dependência) e o buffer de comandos, zera os vetores de movimento e notifica `DEPENDENCY_CHANGED_MULTIMESH`. `_multimesh_set_buffer` exige o tamanho exato do alocado. `_multimesh_set_visible_instances` só grava o número visível (sem realocar). | C2-1 |
| 2 | https://docs.godotengine.org/en/stable/classes/class_multimesh.html | `visible_instance_count` limita o desenho "sem mudar o tamanho dos buffers" | C2-1 |
| 3 | https://docs.godotengine.org/en/stable/tutorials/performance/using_multimesh.html | alocar o máximo necessário e ajustar a contagem visível | C2-1 |
| 4 | https://docs.godotengine.org/pt-br/4.x/classes/class_multimesh.html / https://forum.godotengine.org/t/best-practices-for-spawning-large-number-of-objects/102004 | mesma orientação (contagem visível em vez de realocar); trabalho pesado concentrado num quadro vira pico | C2-1 |
| 5 | https://docs.godotengine.org/de/4.5/tutorials/performance/pipeline_compilations.html | outra fonte possível de pico (compilação de pipeline), descartada: os picos somem só tirando os pedestres, com os mesmos materiais em cena | C2-1 |
| 6 | https://en.cppreference.com/w/cpp/atomic/atomic | contador atômico lido por outra thread sem trava | C2-2 |

## C2 Passo 3 — Causa
- **C2-1.** Os quadros lentos têm a mesma geometria dos outros. Eles somem quando os pedestres não são desenhados, e a
  média de GPU não muda: o custo está num evento, não no volume. O código mostra o evento: a cada quadro, cada balde de
  pedestres (variação × LOD) com contagem diferente da anterior chama `multimesh_allocate_data`. Pela fonte 1, isso
  libera e recria o buffer da GPU, os uniform sets e o buffer de comandos: é churn de alocação na GPU, quadro a quadro.
  Os veículos escapavam só porque o `AgentRenderer` reimplementou uma regra de folga (`ROOM_STEP`) que é do dono do
  buffer. Responsabilidade no lugar errado: com duas regras, os outros usuários (pedestres, trens, lentes) ficaram sem
  nenhuma. Pelas fontes 2 e 3, a correção é alocar com folga e mostrar só a contagem com `visible_instances`. A causa
  se corrige no dono (`MultiMeshDraw::set`), que passa a ser a única regra; a regra duplicada sai do `AgentRenderer`.
- **C2-2.** Para provar que a thread não acorda em pausa, é preciso contar as acordadas onde elas acontecem: um contador
  atômico no laço de `Simulation::run`, lido pela sonda antes e depois de um trecho em pausa.

## C2 Passo 4 — Plano
1. C2-1:
   - `native/src/render/rs_util.h`: `MultiMeshDraw::set` aloca em degraus de `MM_ROOM_STEP` (32) só quando a contagem passa
     da capacidade ou fica dois degraus abaixo; completa o buffer até a capacidade; desenha a contagem com
     `multimesh_set_visible_instances`; guarda `capacity` (alocado) e `count` (desenhado).
   - `native/src/render/agents/agent_renderer.cpp`: sai a regra própria (`ROOM_STEP`, resize, `visible_instances`);
     `shown_` vem de `count`.
   - Pedestres, trens, lentes, árvores e cobertura passam a usar a regra sem mudar sua chamada.
2. C2-2: `native/src/sim/simulation/simulation.h/.cpp` com `wakeups_` (cada volta do laço) e `wakeups()`;
   `api/city_core_probe_render.cpp` (`pose_stats`) expõe `sim_wakeups`; `tests/perf/pending_probe.gd` mede as acordadas
   na pausa e as imprime.
3. Passo 6 (uma vez): compilar as 2 DLLs, gates, suíte completa, `main_thread_probe` (cauda), `pending_probe` (pausa e
   inspetor), `camera_pan`, capturas 05/07/14_pessoa/15_travessia comparadas com as do passo 1 (pedestres).

## C2 Passo 5 — Código (sem testes, medições ou compilação)
| Arquivo | Mudança | Problema / plano |
|---|---|---|
| `native/src/render/rs_util.h` | `MM_ROOM_STEP` (32) e a regra de folga dentro de `MultiMeshDraw::set` (dono único): realoca só ao passar da capacidade ou ao ficar 2 degraus abaixo, completa o buffer até a capacidade, desenha `count` com `multimesh_set_visible_instances`; `count` novo; `visible()` e `adopt()` seguem `count` | C2-1 / 4.1 |
| `native/src/render/agents/agent_renderer.cpp` | removidos `ROOM_STEP`, o redimensionamento e o `visible_instances` próprios; `shown_ = d.count` | C2-1 / 4.1 |
| `native/src/sim/simulation/simulation.h/.cpp` | `wakeups_` (atômico, uma volta do laço) e `wakeups()` | C2-2 / 4.2 |
| `native/src/api/city_core_probe_render.cpp` | `pose_stats.sim_wakeups` | C2-2 / 4.2 |
| `project/tests/perf/pending_probe.gd` | modo `--tail` (passo 1) e as acordadas da simulação na pausa e depois de retomar | C2-1, C2-2 / 4.2 |
Pedestres (`people_renderer.cpp`), trens (`transit_renderer.cpp`), lentes (`signal_lamps.cpp`), árvores e cobertura
passam a usar a regra sem mudança na chamada. Nenhum arquivo deletado.

## C2 Passo 6 — Testes e medições (uma passada: compilação, gates, `scratchpad\c2_step6.ps1`)
- Compilação: debug e release, 0 erros e 0 warnings.
- **Gates: 2 problemas.** `TAMANHO_FUNCAO` em `project/tests/perf/pending_probe.gd:24` (`_ready` com 70 linhas, máx 60),
  **erro novo** introduzido pelo modo `--tail` deste ciclo; e P4 (ambiente, já conhecido).
- Suíte completa: 16/16 ok.
- C2-1, cauda (`main_thread_probe`, metrópole no máximo, mesma sonda que a reproduziu no passo 1):

  | medida | passo 1 | depois | sem pedestres (isolamento) |
  |---|---|---|---|
  | quadro p95 | 22,5 ms | 16,76 ms | 16,8 ms |
  | quadro p99 | 27,0 ms | 17,01 ms | 17,3 ms |
  | quadro máx | 40,5 ms | 18,4 ms | 21,5 ms |
  | GPU p99 | 23,2 ms | 11,9 ms | 15,5 ms |

  Snapshot máx 0,24 ms; chaves do pool 0.
- C2-2, P3 (`pending_probe`): **1 acordada em 30,0 s de pausa** (a volta que entra na espera); ao retomar,
  **12 acordadas em 60 quadros** (~20 Hz, a simulação voltou a andar). Medição conclusiva.
- Inspetor: máx 0,149 ms. Sombras, como configurado: GPU 4,79 ms (igual ao ciclo 1).
- `camera_pan` (cidade "town"): GPU média 5,1–9,3 ms, sem nenhum quadro acima de 33 ms; memória igual ao ciclo 1
  (diferença ≤ 1 MiB).
- Capturas (mesma cena, comparadas com as do passo 6 do ciclo 1, antes da mudança):
  - 07_carro, 14_pessoa, 16_travessia_perto e 22_onibus ≤ 0,04% dos pixels > 8;
  - 15_travessia 0,12%;
  - 05_transito 1,0%, pela folhagem ao vento, como medido no passo 1.

  Sem perda visual nos pedestres.

## C2 Passo 7 — Verificação
**Deu erro**: o gate `TAMANHO_FUNCAO` falhou na sonda `pending_probe.gd`. Todo o resto passou. Revisando o registro: o
erro nasceu no passo 5 do ciclo 2, quando o modo `--tail` foi posto dentro de `_ready`, que já fazia a preparação da
cidade e a medição padrão. Recomeço no passo 1 (ciclo 3).

---
# Ciclo 3 — recomeço após o erro do ciclo 2

## C3 Passo 1 — Identificação
Investigação: o gate (`tools/gates/gate_code.py`, `rules.json`: `.gd` com função ≤ 60 linhas, arquivo ≤ 250) e a leitura
de `pending_probe.gd` (189 linhas no arquivo, `_ready` com 70).
1. **C3-1 — `_ready` da sonda com 70 linhas.** A função faz cinco coisas: lê os argumentos, monta a cidade e espera ela
   ser desenhada, escolhe o modo (`--pace`, `--tail`), roda as variações de sombra e mede a pausa. Cada modo novo cresce
   dentro dela.
2. **C3-2 — Cabeçalho desatualizado.** O `##` ainda diz que as acordadas são medidas de fora (contadores do Windows) e
   não cita `--pace` nem `--tail`. A documentação diverge do que o código faz.

## C3 Passo 2 — Pesquisa
| Fonte | O que diz | Problema |
|---|---|---|
| `CLAUDE.md` > Regras de arquitetura ("Sem arquivos-deus: limites de linhas por arquivo e por função") e `tools/gates/rules.json` (`max_function_lines` `.gd` 60) | o limite existe para cada função ter uma responsabilidade | C3-1 |
| https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/gdscript_styleguide.html | ordem do código (callbacks, depois métodos próprios); o bloco `##` explica o papel da classe e como usá-la; seções lógicas separadas | C3-1, C3-2 |

## C3 Passo 3 — Causa
O limite foi estourado porque `_ready` virou o despachante dos modos e, ao mesmo tempo, o executor do modo padrão. Cada
modo novo (`--pace` no ciclo 1, `--tail` no ciclo 2) cresce ali dentro. Encurtar linhas seria tratar o efeito; a causa é
a falta de separação. A correção dá uma função a cada responsabilidade: `_ready` só lê os argumentos, chama `_boot`
(prepara e espera a cidade) e despacha o modo; o modo padrão vira `_default_run`; a medição da pausa vira `_pause`. O
cabeçalho é corrigido junto, porque mentia sobre o método.

## C3 Passo 4 — Plano
1. `project/tests/perf/pending_probe.gd`: extrair `_boot()`, `_default_run()` e `_pause()`; `_ready` só lê os argumentos,
   prepara e despacha; atualizar o cabeçalho (modos e contador `sim_wakeups`). O comportamento de cada modo não muda.
2. Passo 6 (uma vez): gates (com o `TAMANHO_FUNCAO` resolvido) e uma execução da sonda no modo padrão, que exercita
   preparação, sombras, inspetor e pausa, para confirmar que a refatoração não mudou o que ela mede. Sem mudança em C++:
   as DLLs e a suíte do ciclo 2 seguem válidas.

## C3 Passo 5 — Código
| Arquivo | Mudança | Problema / plano |
|---|---|---|
| `project/tests/perf/pending_probe.gd` | `_ready` só lê os argumentos, chama `_boot()` e despacha o modo; extraídos `_boot()` (mundo, câmera, HUD, espera do desenho; devolve se iniciou), `_default_run()` (sombras, agentes, prédios, cliques) e `_pause()` (acordadas em pausa e ao retomar); cabeçalho `##` atualizado (modos `--pace`/`--tail`, contador `sim_wakeups`) | C3-1, C3-2 / 4.1 |
Nenhum outro arquivo alterado, criado ou deletado.

## C3 Passo 6 — Testes (uma passada)
- Gates: `TAMANHO_FUNCAO` resolvido. Resta só o P4 (`.claude/settings.json`: arquivo do ambiente do usuário, o mesmo da
  baseline; o projeto diz que só o usuário o altera).
- `pending_probe` no modo padrão (exercita `_boot`, `_default_run`, `_clicks` e `_pause`), sem erro de script:
  - cidade gerada;
  - sombras como configurado: GPU 4,90 ms (ciclo 2: 4,79);
  - inspetor: máx 0,082 ms;
  - pausa: **1 acordada em 30,0 s** e 12 acordadas em 60 quadros ao retomar, iguais ao ciclo 2.

  A refatoração não mudou o que a sonda mede.
- Sem mudança em C++ neste ciclo. Valem a compilação (0 warnings) e a suíte (16/16) do passo 6 do ciclo 2.

## C3 Passo 7 — Verificação
**Passou.** Nenhum erro novo; o único gate restante é o P4, do ambiente, fora da aplicação e da competência deste
trabalho. Situação final das pendências:

| Pendência | Estado | Evidência |
|---|---|---|
| Cauda da GPU na metrópole (ciclo 1) | corrigida na causa: realocação de buffers do MultiMesh por quadro; a regra de folga foi para o dono (`MultiMeshDraw`) | quadro p99 27,0 → 17,0 ms; máx 40,5 → 18,4 ms (C2 passo 6) |
| P3, thread acordando em pausa | corrigida (ciclo 1) e agora medida de forma conclusiva | 1 acordada em 30 s (C2 e C3 passo 6) |
| Validação visual do nível distante | feita, A/B com o mesmo código | carros distantes idênticos; as diferenças estão na folhagem animada (C2 passo 1) |
| P4, `.claude/settings.json` | depende do usuário | — |
| P5, commits | depende do usuário (nada commitado) | — |
