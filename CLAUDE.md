# Cidade: city builder em Godot 4.7 com núcleo C++ (GDExtension)

Regras, métodos e estrutura do projeto. Atualize este arquivo na mesma mudança que altera o que ele descreve.
Arquivo criado ou apagado entra em `log/mapa.md` no mesmo commit: o gate `MAPA_DESATUALIZADO` confere.
O detalhe fica em `log/`: `mapa.md` (arquivo por arquivo), `ferramentas.md` (o que cada uma ensinou), `pendencias.md`
(o que falta). Registros de cada linha: `work-log.md` (porte e gráficos), `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`,
`arquitetura.md` (o analisador de causas).
Antes de começar, leia o registro da sua linha. Ao terminar, atualize-o com hora e commit, no mesmo commit.

## Metodologia de trabalho (toda tarefa, nesta ordem; trabalho direto, sem agentes)
1. **Identifique:** leia o código envolvido até achar a causa, não só o efeito. Registre todos os problemas em lista.
2. **Pesquise** na documentação oficial e em fontes relevantes (Godot, godot-cpp, Terrain3D), nunca só na memória.
   Registre cada fonte e a qual problema ela se liga.
3. **Compreenda:** junte código e pesquisa. Registre o raciocínio que liga problema, causa e solução.
4. **Planeje** a solução completa e correta, mesmo cara e trabalhosa, com dono único. Registre o plano.
5. **Codifique** sem tentativa e erro: se não deu certo, volte à leitura até entender. Sem testes neste passo.
   Registre os arquivos criados, alterados e apagados.
6. **Teste uma única vez:** `tools/build/quick.sh --ready`, as duas DLLs, a suíte e `python3 tools/gates/run_gates.py`.
   Fotos analisadas com atenção: `photodiff.py` diz onde olhar, cada mudança é explicada pela causa, sem perda visual.
7. **Pare e verifique:** passou ou deu erro? Se deu erro, revise o registro, entenda e recomece no passo 1.

## Princípios
- Corrija a causa, nunca o efeito. Na dúvida, vá pelo caminho mais caro.
- A causa dos problemas de desempenho é a arquitetura. Sombra, polígono e tempo de GPU são sintomas e mudam com a placa.
- Sem medições nem testes longos e pesados: o trabalho é no código.
- Faça o que foi pedido, inteiro. Nada de remendo, de maquiar resultado ou de deixar para depois.
- Nunca espere às cegas um processo lançado (tempo-limite e acompanhamento). Pare só processos próprios, pelo PID.
- Nunca feche o jogo aberto na tela do usuário: `tools/build/free_dll.sh` libera a DLL.
- Testes e capturas nunca abrem janela na tela do usuário (`tools/build/hidden_run.ps1`).
- Toda mudança no jogo é entregue com um executável novo: o usuário abre `dist/windows/Roadcraft.exe`.
- Trabalho direto na `main`, em `C:\Users\Jonathan\Desktop\city`, sem worktree.
  Commit por tema e push em `origin/main` (`https://github.com/designathan1988/city`).

## Referência: Roadcraft 4.0
- `C:\Users\Jonathan\Documents\Projects\city\roadcraft` (cópia de 29/10; a `roadcraft-4.0-main`, de 23/10, não vale) é a
  referência de visual e de comportamento de tudo: o jogo deve ter todos os gráficos e funcionalidades dele.
- Pode ser lida, aberta, rodada e estudada a qualquer momento. Comentário do 4.0 não prova nada sem o par.
- Portam-se funcionalidades, nunca código, na arquitetura atual, otimizada. Rastreio: `tools/converter`.
- Exceção: a lógica pura de simulação entra traduzida, sem edição à mão, por uma camada fina, conferida contra o 4.0.
  Regenera-se, nunca se conserta o gerado. Primeira área: fluxo e controle (`flow_control`).
- O par com o 4.0 rodando é a prova que mais acha defeito: `npx vite` em 5300 na pasta dele, `node scripts/look.mjs <plano>
  <saída>` lá e o mesmo plano no `look` daqui (`{"size": "small", "seed": 7}` e `{"map": 0, "seed": 7}`).

## Regras de arquitetura
- **Fonte única da verdade:** `CityState` (`native/src/sim/state/city_state.h`). Só `sim/commands` muda o estado a pedido
  do jogador; só `sim/simulation` avança o tempo.
- **Camadas C++:** `core` → `sim/*` (C++ puro, sem Godot) → `render` (só lê) → `api`.
- **Sem arquivo-deus:** nem classe-deus repartida em vários arquivos, nem funcionalidade espalhada. Divide-se por
  responsabilidade, nunca para caber num limite.
- **Um dono por definição:** regras com dono único em `tools/gates/owners.json`. Sem código duplicado: extraia um dono.
- **Dados:** valores ajustáveis só em `project/data/**` (JSON com schema), lidos só por `DataTables`. Nomes do tuning na
  X-macro `CITY_TUNING_FIELDS`, valores em `data/catalog/tuning.json`.
- **Sem números mágicos** em `native/src`, `project/game`, `project/ui`: dê nome ou mova para dados.
- **Interface:** não guarda números do jogo: lê `CityStore.snapshot` e envia `CityStore.dispatch(...)`. Cores e tamanhos só
  por `data/ui/tokens.json` e `ui/theme/theme_builder.gd`; textos só por `tr("CHAVE")`; botões e opções em `data/ui/*.json`.
- **Fluxo:** troca de tela, pausa e saída só por `GameFlow`.
- **Nomes só em inglês** em arquivos, pastas e código.
- **Cabeçalho:** todo arquivo começa com um comentário que diz a responsabilidade dele.
- **Pendências** vão para `log/pendencias.md`, nunca para comentários TODO ou FIXME.

## Testes
- Conferem o que o jogador vê: HUD, buffer de instâncias enviado à GPU, sondas do núcleo.
- **Hash do estado:** `probes/state_hash` é a cidade como um número. O `edit_fuzz_test` exige que desfazer, refazer, salvar e
  abrir passem pelos mesmos hashes: toda edição nova entra no sorteio dele.
- **Testes e capturas (`--no-flow`) rodam nos dados:** não leem nem gravam `settings.cfg`, nem leem `health.json`. Abrem no
  nível `tests_preset` (outro pelo nome, `--preset=`). Saves em `user://saves_tests`.
- Para isolar o `user://` de um clone: `project/override.cfg` com `application/config/use_custom_user_dir=true` e
  `custom_user_dir_name`.
- Bug de memória e de tempo não se caça com carga: a DLL de debug tem o hardening da libc++, e a falha se lê na pilha e no código.
- Prova visual é do elemento, de perto e de vários ângulos.

## Comandos
| O quê | Comando |
|---|---|
| Rodar o jogo | `godot --path project` (sem Vulkan: `--rendering-method gl_compatibility`) |
| Compilar as duas DLLs | `MINGW=... tools/build/build.sh core-windows` (Linux: `core`) |
| Verificar uma mudança | `tools/build/quick.sh [cena,cena]` (`--ready` antes do commit, `--all` toda a suíte, `--bless` só com mudança visual entendida) |
| Rodar um teste só | `tools/build/runq.sh <cópia> <pasta> fast test:tests/core/<cena>` |
| Suíte em paralelo | `python tools/analysis/suite.py [--only=nomes]` (logs em `dist/tests`) |
| Gates | `python3 tools/gates/run_gates.py` (`--file X`, `--summary`); só os mudados: `tools/build/gates_changed.sh <cópia>` |
| Exportar e empacotar | `tools/build/build.sh export && tools/build/build.sh package` |
| Capturas | `tools/build/build.sh capture /pasta` |
| Olhar o jogo por um plano | `godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<json> --capture-dir=<pasta>` |
| Diferença de fotos | `python tools/build/photodiff.py <antes> <depois> <saída>` |
| Onde cada thread gasta | `python tools/analysis/sampler.py --hidden --cmd "<godot> --path project <cena>" --seconds 30 --symbolizer <llvm-symbolizer>` |
| Causas pela forma do código | `python tools/analysis/causes.py` |
| Causas pela arquitetura | `python tools/analysis/arch.py scan`, `status`, `next`, `explain <id>`, `selfcheck` (tarefas lado a lado, painel em tempo real; `--only x --again` refaz só o que falhou; fila em `dist/arch/queue.json`) |
| Perfil da DLL de release | `tools/build/pgo.sh` (refazer depois de mudar laços quentes) |
| Tradutor do 4.0 | em `tools/converter`: `node src/cli.ts feature`, `convert`, `differential` |

**O que um clone precisa:** `native/godot-cpp` (API 4.7) e as DLLs em `project/bin` já vêm no repositório: o jogo abre sem
compilar. Meshoptimizer e Clipper2 vêm pelo CMake na primeira compilação. À parte: Godot 4.7.2, llvm-mingw
`20261020-ucrt-x86_64` (`C:/Projects/toolchains`), CMake 4.4, Node 23.6+, Python 3.12. A DLL pede x86-64-v3 (AVX2, FMA, BMI).

**DLLs em rebase:** conflitam sempre. Nunca escolha um lado: refaça as duas a partir das fontes juntas e rode a suíte.

## Estrutura do projeto
`C:\Users\Jonathan\Desktop\city` é o projeto inteiro. O detalhe arquivo por arquivo está em `log/mapa.md`.

**`native/src`: núcleo C++**
- `core/`: bytes, cor, grade, pool de jobs, matemática, memória, polígonos, aleatório, unidades e o runtime do código traduzido.
- `sim/state/`: `CityState`, `ChangeLog` (canal único de "sujo"), `PoseBuffer`.
- `sim/catalog/`: tipos do catálogo, sem valores.
- `sim/commands/`: comandos do jogador, diário de edições, desfazer e refazer.
- `sim/simulation/`: tick, velocidades, relógio e crescimento.
- `sim/roads/`, `sim/street/`: rede viária, greide, perfis, junções; mobiliário e obras da rua.
- `sim/traffic/`: veículos, faixas, cruzamentos, semáforos, pedestres, caminhões, pátio.
- `sim/transit/`: ônibus, trens e linhas.
- `sim/buildings/`: registros, gerador de corpos (`b40/`) e interiores (`interior/`).
- `sim/lots/`, `sim/citygen/`: lotes; a cidade gerada.
- `sim/terrain/`, `sim/nature/`: relevo; paisagem, clima e ecologia.
- `sim/demolition/`, `sim/economy/`, `sim/persistence/`: explosão e dano; dinheiro das vias; save.
- `render/`: `RenderWorld` (consome o `ChangeLog`, aplica os orçamentos), `rs_util.h`, `mesh_builder`, `chunk_queue.h`.
- `render/{agents,buildings,roads,props,trees,cover,water,transit,effects,overlays,atmosphere,wear}/`: cada coisa desenhada.
- `api/`: `CityCore` e as áreas de script (`builder`, `landscape`, `transit`, `street`, `probes`, `lab`), carregadores de dados.

**`project`: Godot**
- `game/`: `store/` (`CityStore`, `StoreApi`, saves), `flow/` (`GameFlow`), `data/` (`DataTables`), `camera/`, `tools/`
  (ferramentas do jogador), `settings/` (opções, níveis gráficos), `interior/`, `lab/` (F7), `health/`, `play.gd`.
- `render/`: mundo, céu, luz, água e passes de tela; `render/shaders/`: todos os shaders.
- `ui/`: HUD, painéis e gavetas; `ui/theme/`: estilos; `ui/icons/`: ícones.
- `data/`: `catalog/` (regras e tuning), `game/` (câmera, luz, opções), `render/` (aparência), `ui/` (interface), `i18n/` (textos).
- `assets/`: texturas, pessoas cozidas, lotes e modelos. `addons/terrain_3d/`: Terrain3D. `bin/`: as DLLs do núcleo.
- `tests/`: `core/` (testes), `e2e/`, `perf/` (sondas), `visual/` (capturas, `look.gd`, `ui_driver.gd`).

**`tools`**
- `gates/`: regras verificadas, `rules.json`, `owners.json`, `schemas/`.
- `build/`: `build.sh`, `quick.sh`, `runq.sh`, `run_tests.sh`, `pgo.sh`, `photodiff.py`, `free_dll.sh`, `hidden_run.ps1`.
- `analysis/`: `suite.py`, `sampler.py`, `causes.py`, `live40.mjs` (o 4.0 aberto, respondendo em milissegundos), `arch.py` e `arch/` (o analisador
  de arquitetura), `arch_eval/` (gabaritos que só o selfcheck dele lê).
- `assets/`: cozinha de pessoas, marcos, ruído, móveis, postes e perfis de via.
- `converter/`: rastreador e tradutor do 4.0, com git próprio; o conteúdo vem por `git read-tree`.
- `converter-slice/`: fatia antiga do conversor.
- `process/`: ferramentas de processo e registros. `ports/`: planos, coberturas e relatórios de cada porte.
- `oracle-4.0/`: cópia instrumentada do 4.0 para o oráculo (`ORACLE_INSTRUMENT=1`).

**Outros:** `dist/` (builds, capturas e logs, fora do git) e `log/` (registros).

## Fluxo de dados e threads
- Execução isolada aprovada em 2026-11-07: branch `codex/performance-causes` no worktree gerenciado; desenho em
  `docs/superpowers/specs/2026-11-07-performance-causes-design.md`, plano e progresso em `docs/superpowers/plans/` e
  `log/performance-causes-progress.md`. O pedido do usuário preserva o checkout principal e restringe esta execução a
  código/leitura estática, sem build, jogo, testes ou medições. Implementação em andamento; não é entrega executável validada.
- `DataTables` lê `data/**` → `CityStore.boot` envia o catálogo ao `CityCore`, que rejeita chave ausente.
- `Simulation` roda em thread própria com tick fixo (`sim_tick_hz`), sob `map_mutex` compartilhado e `traffic_mutex`.
- Comandos, crescimento e relógio tomam `map_mutex` exclusivo.
- `ChangeLog` é o único canal de "sujo", drenado sob trava compartilhada e o mutex do log.
- Water preparation participates as the tenth rotating producer: bounded cell work retains original geometry/order and checks regional water versions across frames. TerrainBridge and whole GPU publication/retirement still require common S04 admission.
- Reference pictures retain an abstract PreparedReference root with immutable packed input; SurveyTriangles streams original corner order and SurveyPixels pins RGBA bytes without another full copy. Surface extraction/capture/depth still require common continuation/admission.
- Reference survey captures immutable ModelInputs and city/table/target witnesses, prepares outside map_mutex and revalidates before final placement. Caller continuation/mesh input admission remain open; no asynchronous or runtime validation claim.
- Async requests use distinct inboxes and captured terminal finalizers even when queued work is skipped. Shore cancellation restores requested pages; bake receipts release only matching reservations and retry only never-started CPU inputs. Running bakes still require S04 partition.
- Shared chunk completion queues publish into ordered heaps; frame commits inspect one result per deadline check and read cached waiting/baking byte totals. Whole-unit GPU work and global retirement still require S04.
- `RenderWorld.sync` nunca toma o mapa exclusivo. Manda as malhas ao `JobPool`, que leem só o mapa, nunca o tráfego.
- Alturas finais usam páginas espaciais imutáveis; `HeightPatch` pina raiz/janela sob trava curta, sem copiar amostras. Workers de prédio/neblina/margem retêm somente os tiles regionais fora da trava; ruínas pinam a região antes da fila. Aposentadoria/admissão global dessas raízes ainda consta no progresso.
- A fila de jobs substitui/remove solicitações pendentes por índice e libera capturas fora do mutex; trabalhos em execução
  consultam cancelamento cooperativo. Helpers de `parallel_for` conservam seu contrato de conclusão e participação do chamador.
- Neblina e margem d'água copiam terreno/entradas antes de rasterizar fora da trava. As demais migrações constam no progresso.
- Corpos autorais: `BodyTable` consulta versões fracamente por geração; prédios vivos, payloads tipados do histórico,
  ruínas e jobs pinados mantêm as raízes. Lotes e transporte registram deltas ordenados de registros, sem clonar suas
  coleções inteiras por comando. O orçamento de histórico aberto e sua paginação continuam em implementação.
- Contabilidade do histórico percorre payloads tipados e capacidades retidas; raízes compartilhadas contam uma vez
  no conjunto aberto/undo/redo. As cargas de metadados de nós/controle são explícitas, não uma medida de RSS.
- Saves serialize into stable output pages, frame extra blocks in place and write GCMP/ZSTD blocks through the single save_file owner, releasing pages as they finish. FileAccessCompressed WRITE retained the whole raw file and is no longer used. Immutable source capture outside map_mutex and total snapshot RAM admission remain open.
- Load decodifica uma candidata privada uma vez; sucesso troca a cidade e reanexa as áreas da API. Cidade anterior
  fica aposentada até seu pool drenar, sem espera de jobs no quadro. Falha de decode não altera a cidade aberta.
  Criação de mapa usa a mesma fronteira. Leitores conferem bytes disponíveis antes de reservar coleções.
- Lotes autorais expõem registros constantes; restore/apply atualizam índice de bounds e stands por célula. Geometria
  é preparada uma vez por polígono alterado; trocar só a zona a reutiliza. Consultas preservam a ordem do livro.
- Mobiliário completo é compartilhado por `StreetPlacement`; precedência procedural/autoral ocorre antes da projeção
  por tipos. Pedestres, props e árvores reutilizam essa base. `render.placement_cache_mb` limita só a retenção, sem
  excluir peças; zero desativa cache. Mudanças físicas/autoria aposentam entradas; controle integra a chave.
- Snapshot publica gerações/revisões de cidade, edições, registros de prédios, grafo de tráfego e histórico fechado.
  Inspector reaproveita controles e uma leitura por estado de tráfego; projeções de junctions/handles/seleção seguem
  câmera e dados. Marcas de junctions usam índice preparado antes da projeção; plano de transporte e rascunho
  têm desenho separado. Miniaturas têm assinantes/tokens, cache por sessão/tamanho e pedidos por cards visíveis.
- Camera conserva respostas por papel/argumentos e revisoes fisicas diretas; molas e timers continuam ativos.
  Volumes de predios usam registro/geracao e LRU limitado por `render.camera_solids_cache_mb`.
- Placas: jobs fixam o atlas submetido; reset troca seu epoch. Canvas conserva paginas CPU e capacidade
  geometrica, aposenta imagens/celulas antigas e rejeita readback de outro epoch.
- A conversão de malha entra em `render.upload_budget_mb` por quadro; o commit só troca malhas, em `render.commit_budget_ms`.
- Poses por `PoseBuffer`. O save grava em thread própria.

## Luz e relógio
- `Daylight` é o dono do sol, do ambiente e do céu. À noite o sol vira lua sem sombra; a luz da rua é `LampLights`.
- `CityWorld.set_sky_mode("day"|"night"|"cycle")`: dia e noite fixam as horas de `daylight.json` (16h e 22h).
- Testes e capturas movem relógio e céu juntos (`UiDriver.at_hour`).
- Par de foto com o 4.0: mesma hora, mesma câmera e 1920x1080.

- SkyView prepara formas por revisao e move centros; fog conserva a grade de amostragem ao recortar trabalho XZ.
- Settings conserva resolucao interna de opcoes; corte de zoom tem `detail_hysteresis_share`. Governador de
  qualidade consome pressao GPU/CPU/tick com `cpu_budget_slack`; resolucao dinamica continua so por GPU.

## Máquinas de estado
- `GameFlow.State`: BOOT → MENU → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram e saem de STOPPED.
- `ToolController.Tool`: uma por ferramenta, de mãos vazias é INSPECT; só `set_tool` troca; segurado `drag`/`tick`, solto `release`.

## Escopo dos custos
- `FramePressure` registra so o intervalo ativo de process da SceneTree; CPU render e carga do tick sao separados.
- FrameMeter publica `native_render_subtotal`/`cpu_complete=false`; CityLab nao conclui gargalo total com essa cobertura.
- Probes opcionais de pares usam candidatos locais; a janela CpuWatch solta a raiz COW antes de escrever.

## Trilhos derivados
- TransitData distingue revisao de trilhos; TransitRun reutiliza grafos por modo/revisao/join. `rail_path.cpp`
  possui respostas por endpoints e scratch com lease sem espera de geometria, limitados juntos por
  `sim.rail_path_cache_mb` por modo. Dijkstra termina pelo custo completo dos conectores, preservando empates.
- TransitData conserva tabelas privadas; projecoes const e mapas de ID servem os leitores. O escopo externo
  aplica deltas aos indices de paradas e pecas de trilho, inclusive load/undo/redo; picking preserva ordem de empate.

## Continuidades e nomes viarios
- RoadMetrics conserva separadas a sequencia orientada de juntas e a continuidade de pecas RUNS_ON;
  prepara offsets/comprimentos por revisao fisica. A preparacao muda ordem das somas, nao as formulas de estacao.
- NamedPlates compartilha cadeias reais por nome e cobertura; geometria fisica tem outra chave e indice por chunk.
  A cadeia gulosa por perfil nao e substituida por componente conectado. Nomes tem revisao propria em StreetWorks.

## Candidatos e base de geracao
- GrowthQueue possui candidatos livres e dependencias espaciais de ocupacao. LotCache e BuildingStore notificam
  mutacoes diretamente; consumo do log pelo render nao decide crescimento. Ordem de fila/lote e compactacao se mantem.
- Plano fluvial imutavel e compartilhado por seed/half/spacing; mapas temporarios de lagos ficam por fase.
- RoadSegment prepara arco de lotes de 24 passos, emprestado por vistas locais pinadas; arco de vias segue 48.

## Janela de chunks
- ChunkStream e dono dos conjuntos e da fila ordenada persistente. A janela so e redescoberta por mudanca real
  de XZ/reach/drop; mark insere chaves novas/sujas mesmo com camera parada. inherit_known invalida a selecao.
- LeafAtlas transfere sua imagem CPU para TreePictures; nao a rele da GPU para preparar pictures.

## Reaplicacao das fontes
- `tools/performance/apply_fixes.py` exporta conteudos reais com hashes e exige fechamento estatico das 92 ocorrencias.
  Aplica apenas em outro worktree codex/ da base auditada, recusa o original/conflitos e reconhece arquivos ja aplicados.
  O script nao compila, testa, executa o jogo, comita nem integra a branch; pacote ainda nao foi gerado.

## Massas de predios em jobs
- BodyCache compartilha massas imutaveis por identidade exata, limitado em bytes por `render.body_cache_mb`.
  Inputs de predios e closures de ruinas fixam o cache submetido; reset cria outro. Nenhum BuildingBodies e TLS.
- BuildingBodies e workspace por job: fixa massa ativa, um corte e estado mutavel de interiores. Compartilhar pisos
  preparados e aposentadoria imediata de derivados autorais ainda constam abertos na matriz de causas.
- Refinamento de raiz autoral: Version possui blocos editaveis e massa resolvida; BodyInput entrega aliases da
  mesma raiz. Ultima raiz libera ambos; massas autorais nao entram no LRU. O limite render.body_cache_mb governa
  apenas massas procedurais. Historico inclui ambos os payloads e canonicaliza aliases para contar uma raiz.

## Indices transitorios do trafego
- Celulas de pedestres mantem apenas ocupacao atual, com buffers de chaves reutilizados e remocao de vazias.
- Retirar uma fonte de estacionamento aposenta seu plano de patio e planner pendente; caminho do caminhao em uso
  e copia propria. Voltar a fonte retirada pode exigir novo planejamento.
- Arrays de faixas/conectores ativos possuem posicao reversa e preservam a remocao swap/pop. Vehicle.pass tem
  escritor unico set_pass e lista reversa propria por conector; conn_pass continua sendo apenas o ultimo titular.

- Publicacao de poses/views percorre IDs vivos em ordem crescente; reducoes limpam somente entradas tocadas no
  buffer fisico. Volume guarda fila de mudancas para cada view fisica, inclusive quando o consumidor pula frames.
- Construcao/remocao de junctions mantem indice ordenado dos sinais. Luzes limpam slots tocados anteriores; lamps
  usa identidade unica de publicacao de PoseFrame, separada dos timestamps usados na interpolacao.
- Puffs preservam slots logicos do anel e RNG, mas empacotam apenas vivos na ordem original. begin_frame nao envia;
  flush ocorre uma vez depois de todos os produtores. Upload agregado S04 continua aberto.
- CrowdScratch pertence ao TrafficState: indices de pontos por junction ocupado e scratch separados por calcada
  paralela. BoundsIndex.query_ordered usa minimo ID por subarvore para filtrar espacialmente sem trocar a ordem
  dos candidatos. O limite original de corpos e os predicados finais continuam iguais.

- WearField separa queued de dirty real. Saltos acumulam uniao de faixas fisicas por mascaras de linhas/colunas;
  realize_clear limpa spans uma vez por tile, ao receber marca ou na preparacao limitada do upload. RNG de
  arredondamento continua sendo consumido em bytes saturados. Probe de ocupacao respeita limpeza logica pendente.

- FogPaint possui pinceladas ordenadas e indice espacial mantido por edicao/undo/load. Workers copiam candidatos
  locais em ordem cronologica e limitam buckets a janela. Salvar conserva o layout FogDab e sua ordem.
- Canyon usa maximos deslizantes em duas passagens e workspace de corpo por job, com o mesmo BodyCache epoch
  de predios/ruinas. Invalidacao regional e publicacao comum continuam pendentes em C12/S04.

- RenderWorld alterna primeiro turno entre as oito filas de chunks participantes, em builds/commits. Ainda nao e
  contrato de quadro integral: S04 continua aberto. UploadBudget zero/cadencia pulada agora nao admite bake;
  item maior que janela positiva ainda exige particionamento antes de declarar teto estrito.

- ActorIndex pertence ao TrafficState. Nascimento/remocao e escritores existentes de pose (lider/seguidor,
  set_off_pose e publicacao) atualizam bounds conservadores sem passe global adicional. Acoes consultam
  candidatos locais ordenados por ID e conservam predicados finais. crowded enxerga nascimentos do mesmo tick.
- Catalogo prepara os ajustes log-normais de letalidade/timpano antes de publicar; parametros avulsos modificados
  conservam calculo puro de fallback. Indices de dano por identidade e admissao comum de rotas ainda pendentes.

- DamageState possui registros privados; comandos usam mutadores que atualizam indices de cicatriz/ferimento por
  identidade exata seed+x+z, ruinas por bounds e objetos derrubados por tipo/regiao. Getters sao const. Decoder
  usa DamageRecords local e publica dados+indices juntos, mantendo ordem/layout legado e raizes autorais.
- Remover/inserir no meio de ruinas/ferimentos conserva vetores ordenados e pode reconstruir indices. Isso nao
  encerra a admissao de eventos/reconciliacao/uploads de efeitos, que segue pendente em C67/S04.

- StreetChains (RoadNetwork) compartilha continuacoes elegiveis e caminhadas greedy por semente, com revisoes
  locais dos nos visitados. Nao troca a regra por componente conexo: conserva ordem seed/end1/end0, empate
  de primeiro braco e guarda por ponta. sim.street_chain_cache_mb limita ambas as classes por bytes logicos;
  zero/oversized atendem sem reter. Preparacao/checagem de dependencias e destruicao de raizes ficam fora do mutex.

- RoadNetwork mantem contagens de segmentos/nos vivos e lados com zona; snapshot nao varre slots apos edicao.
  SnapCounts foi removido. set_speed/set_day_hour/set_levels retornam changed booleano.
- CityStore.watch assina campos por vida do Node; todos os consumidores internos usam projecoes selecionadas.
  Refresh identico nao emite; refresh reentrante nao entrega valor antigo depois do novo. O sinal changed
  completo segue disponivel a clientes externos. Cadencia de relogio/metricas continua 0.25 s; cache de consulta
  inclui geracao da cidade. Estado de saves so troca depois de new_map com ok=true.

- Corpos retidos possuem draw proprio e bounds da uniao de vertices influenciados por osso em todos os LODs.
  Bounds de pose incluem soma de pesos UNORM16 e erro de aritmetica; motor pode rejeitar corpo por camera/cascata
  sem retirar caster fora da camera. Faixas de LOD/floor sao as das pessoas, com detail_hold.h compartilhado.
- Bone texture e array de paginas de uma linha por corpo; so paginas dirty sao enviadas. Scratch de palette/
  get-up e reutilizado. Limite/ordem de 64 corpos, fisica e sono continuam. Criacao fria/uploads/retirada de recursos
  ainda precisam da admissao S04; C65 continua parcial para sombras de agentes, trens e demais efeitos.

- `AgentRenderer` prepara parked por raiz imutável, com keeper e buffers próprios; câmera/frustum/LOD/cap governam seleção.
  Publicação estática compara linhas ativas e só altera bounds quando necessário. Sombras distantes compartilham a malha
  imutável, preservada no reset da cidade; cada setor usa a união dos hulls transformados. Admissão global ainda em andamento.

- `SpatialInstanceDraw` é o dono dos setores de atores/efeitos (`render.instance_sector_m`), das linhas estáveis por setor
  e dos bounds transformados. Pessoas preservam seleção da câmera e admitem casters separadamente; animações e face
  têm envelope preparado dos dados CPU. Objetos da mão/cabeça reutilizam intervalos dos mesmos bones. Trens publicam
  identidade de carro independente do modo. Puffs usam envelope da trajetória inteira e vento, sem readback.
- `StaticInstanceDraw` assenta a página temporal com uma publicação idêntica no quadro seguinte à mudança e conserva
  os buffers depois disso. Não substitui a admissão global ainda pendente.
- Marcas de cratera reutilizam identidade exata e gerações dos blocos do relevo natural; modificadores de crateras/vias
  não invalidam essa base. Só o prefixo que já cabia na política visual recebe preparação; a cidade retém todos os registros.

- `ProjectedErrorView` limita erro geométrico vertical pela projeção e viewport reais, incluindo divisão por W e
  arredondamento. Montes usam níveis aninhados com erro preparado por monte, limite `render.geometry_error_px` e
  transição complementar (`render.geometry_fade_s`). Sombras mantêm a malha completa por setor. Saltos de câmera
  aposentam nível que exceda o limite; sem projeção válida o desenho é completo. Crateras permanecem incrementais.

- Fratura prepara sementes de Voronoi só nas células consultadas por superfícies, com a mesma identidade/ordem.
  Entrada, células consumidas e canais do writer são aposentados por fase; os limites antes da construção e
  publicação ainda dependem da admissão comum em andamento.

- `PreparedBody` transporta o corpo imutável entre rascunho, tamanho, validação de posição e stage. Os bytes continuam
  na fronteira de arquivo/API legada; HandleDrag pede rascunho sem serialização. A referência não possui cidade/RIDs,
  e o dono da mão/preview controla sua vida. A admissão do preparo/publicação do stage ainda está em andamento.

- `CityBuilder.body_info` retém uma projeção do registro/geração consultados; devolve dicionários aninhados próprios
  ao chamador. Misses independentes não esperam preparo de outro leitor; a trava protege apenas troca de metadados.
  O contrato de mudança de `edit_body` ainda depende da substituição dos digests e da cópia no no-op.

- Via, borda e estrutura têm streams e créditos próprios; uma admissão não multiplica três jobs. Estruturas têm
  um único stream com alcance da via e alcance maior por chave de ponte/viaduto/túnel. A rotação do dono do quadro
  inclui os oito produtores atuais; migração dos demais produtores e publicação/aposentadoria seguem em andamento.

- O índice de poses do pátio reinicia por epoch e materializa páginas/links só quando consultados; blocos de nós
  reservam armazenamento sem construir posições não usadas. A busca cobra a preparação alcançada e mantém sua ordem.
  `ParkingState` carrega dívida de trabalho entre ticks e planejadores; isso ainda não cobre toda a captura/geometria
  ou quebra todas as unidades variáveis do pátio.

- `BodyCache` retém massas procedurais e valores imutáveis de pisos/salas numa única LRU por bytes.
  Versões autorais mantêm observador fraco de derivados; sua última raiz os aposenta antes de liberar a massa.
  Valores retidos não pinam Body/Data; workspaces pinam fonte e cache. Cortes e lookups mutáveis são locais ao job.

- A base de natureza por região pertence a `CityState::plant_ground`: árvores near/far e cobertura compartilham
  ecologia e ocupação imutáveis numa LRU `sim.plant_ground_cache_mb`. Stamps persistentes por fonte não dependem
  do consumo do ChangeLog. Índices de exclusão mantêm os predicados e a janela original; biome paint tem revisão
  distinta de superfície/cobertura. Água editada/restaurada usa `write_water`/`set_water` para marcar a dependência.

- `ShoreField` deriva páginas com halo de propagação a partir de snapshots locais de água/terreno.
  Páginas alteradas entram no banco inativo de Texture2DArray; um pequeno mapa R8 troca a revisão completa.
  O shader cruza páginas por canto, preservando a interpolação. Essa publicação ainda aguarda admissão geral S04.

- Logística consulta candidatos móveis no ActorIndex e resolve empates pela posição original em micro_list.
  Docas elegíveis usam rank de IDs ascendente sobre o AVL existente; contagens por fonte acompanham lorries.
  LaneClosures é o dono dos índices por faixa e por proprietário, preservando a sequência de inserção.
  Essas estruturas não substituem a admissão comum de rotas, ainda em andamento.

- Rotas de veículos e pedestres pertencem a `RoutePool`: páginas por classe de tamanho, handles estáveis,
  devolução explícita na morte/troca e liberação de páginas vazias. Não há compactação global da frota nem
  contadores de fatias mortas. Admissão de buscas/eventos e continuação de dívida ainda estão em andamento.

- RenderWorld agora distribui créditos entre DEZ produtores, incluindo a continuação de trânsito.
  TransitTrackCursor preserva passos/ordem da geometria, mas prepara pernas/peças/trechos ao longo dos quadros.
  Não há items_per_frame local. Fontes por registro e cores da primeira linha são mantidas por TransitData;
  preview idêntico reaproveita geometria e participa dos mesmos créditos. Uploads/aposentadoria gerais S04 seguem abertos.

- Congestionamento retém as faixas geométricas e seus ranges de cor por segmento. Velocidades só atualizam
  atributos RGBA8 cujo resultado quantizado mudou; estrada/greide invalida a geometria pelos road keys existentes.
  O formato é VERTEX+COLOR e tem stride de atributo 4. Admissão geral dos overlays continua pendente em S04.

- Survey de referência usa candidatos espaciais/altura com ordinais estáveis; o dono mantém a escolha original
  do primeiro parceiro e suas dependências. Pontos de tops aposentados e buffers consumidos saem por fase.
  Capturas têm epoch/ticket e placement próprio; a publicação valida geração da cidade e versão do registro alvo.
  Referência ainda expande triângulos e calcula survey sob edição: C88 parcial, sem limite global comprovado.

**Admissão e validade de render:** `render/job_request.h` retira a identidade do pedido ao registrar mudança/saída, antes de haver crédito para substituí-lo; cancela closures CPU/bake fora do mutex de resultados. Pedido antigo não se torna válido por esperar crédito. Árvores/cobertura invalidam toda a cauda suja antes do laço admitido; mudanças de share incluem chunks ainda ausentes com pedido em voo. A malha visível só é trocada por resultado atual. A época conservadora pode exigir recaptura; não é uma revisão global nem prova de custo limitado de bake/aposentadoria.

**Identidade autoral de corpos:** `b40/body_content.h` possui a ordem dos campos persistentes; escrita atual e igualdade exata usam esse esquema. `body_digest` fornece semente visual compatível, nunca prova igualdade. Campos de corte/visão temporários ficam fora da identidade. `edit_body` ainda prepara clone/valida antes de descobrir no-op; não tratar essa migração como admissão ou mutação preguiçosa concluída.

**Base de lotes:** `LotRasterCache` em CityState compartilha spans imutáveis de círculos antes do recorte por margem, tanto na grade de quadras de 1 m quanto na de faixas de 2 m. Chaves são bits exatos de posição/raio/resolução; `sim.lot_raster_cache_mb` limita retenção lógica. A seleção de vias/arms e decisões de cada janela continuam locais. Miss não segura mutex durante rasterização; grades desconhecidas/entradas inelegíveis mantêm fill_disc completo.

**Dependências do transporte em execução:** TransitData mantém revisões por domínio/linha e vínculos inversos parada→linhas em comandos/replay/load. TransitRun preserva loops/rotas intactos, com reconciliação indexada da frota. Preparo regional/limitado e aposentadoria de fontes seguem parciais em C60; trajetórias bit a bit não são prometidas.

**Dono de rotas de rua:** RouteAdmission tem uma fila, uma busca RoadRouteSearch reutilizável e um WorkBudget por tick. Eventos meso e quanta de busca alternam créditos pela mesma cota; pontos seguros após trânsito/pedestres usam apenas o saldo. Ônibus, meso, viagens, lotes, caminhões, reparos, reconsideração/desvios, nascimentos e fugas de pedestres participam. Os antigos find_route/find_walk_route/run_sync e cotas independentes de rotas/rerotas/pedestres foram removidos após a migração dos consumidores.

**Fonte e busca:** LaneGraph prepara route_conns preservando a primeira ocorrência de destino/nó/virada quando a prova de custo permite deduplicação; física mantém todos os conectores. RouteCounts publica páginas imutáveis pequenas nos escritores de ocupação. RoadRouteSearch empresta mundo/contagens ao avanço, não retém ponteiro/trava/callback do mundo. AStarScratch possui uma máquina start/pop/edge/path, páginas esparsas e heap StableChunks. Revisões de cidade/grafo/estradas guardam leituras retomadas; Pending, Unprepared, falha real e aposentadoria de origem/destino têm tratamentos distintos.

**Propriedade de caminhos:** RoutePool tem páginas por classe, handles estáveis com identidade da alocação e nascimento do destino final; RouteDraft é saída não publicada. RouteJoin escreve prefixo/consulta/cauda/sufixo por crédito e só adota um caminho inteiro. Gerações de nós/segmentos/atores não entram nos campos autorais de save. Renascimento de slot não representa a fonte antiga; leitores de caminho validam seu destino antes da progressão futura.

**Consumidores Pending:** um pedido retém receita, argumentos, tentativa e dono. Nova intenção desejada prevalece sobre reparo/meso/opcional antigo; atores mantêm caminho utilizável/espera segura. Eventos meso retêm horário e identidade de agendamento. Lotes preservam dívida, baias/planos de doca têm testemunhas, e docas prontas são registros imutáveis pinados. Publicação ainda contém verificações físicas/cópias/preparo variável; heap obsoleto, memória retida e aposentadoria não têm teto agregado completo (C52/C57/C60 parciais).

**Nascimentos:** BirthReservations guarda propostas de faixa; WalkerBirthReservations reutiliza ActorIndex em um índice separado só de metadados. Leases não chamam CityState na destruição. Receitas mantêm escolhas enquanto esperam; ator/índice físico só recebe identidade e rota completa no commit. PedBirthTicket distingue espera, sucesso, falha e cancelamento; ônibus/parada cancelam ou aposentam um nascimento pronto ainda não recebido. Pessoas retidas usam geração, e passageiros desembarcados são liberados para andar. Reset cancela todos os jobs de caminhada antes de limpar ped_pool, incluindo os que pertencem a veículos.

**Portas:** PedDoor usa item_version do prédio e nascimento do segmento; leituras adiadas capturam a revisão. Descarte de uma porta antiga só invalida seu próprio seen, e associação inversa de calçada evita peso duplicado após limpar/refazer. Altura é atualizada ao usar; destinos retirados deixam de orientar caminhantes. Caches/preparo regionais ainda têm trabalho variável; C53 fecha os multiplicadores de busca síncrona e varredura high-water, sem alegar um limite global de tempo/RAM.

**Entrada ordenada em implementação:** InputSequence possui receitas paginadas de comandos/interpolação/tempo/zona, tickets e barreiras explícitas; CityStore executa créditos de data/game/editing.json e publica snapshot por lote. Histórico/persistência/quit estão sendo migrados, sem sleep de fechamento. Os pincéis e demais consumidores de retorno ainda usam o caminho antigo: C20 permanece aberto e nenhuma fila pode ser habilitada para eles antes de concluir a migração dos pontos de entrada e das barreiras. O crédito conta transições/amostras, não milissegundos nem custo interno máximo de uma edição. APIs diretas continuam síncronas e não devolvem dicionário fictício de aceitação.
