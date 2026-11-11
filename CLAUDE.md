# Cidade: city builder em Godot 4.7 com núcleo C++ (GDExtension)

Regras, métodos e estrutura do projeto. Atualize este arquivo na mesma mudança que altera o que ele descreve.
Arquivo criado ou apagado entra em `log/mapa.md` no mesmo commit: o gate `MAPA_DESATUALIZADO` confere.
O detalhe fica em `log/`: `mapa.md` (arquivo por arquivo), `ferramentas.md` (o que cada uma ensinou), `pendencias.md`
(o que falta). Registros de cada linha: `work-log.md` (porte e gráficos), `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`,
`arquitetura.md` (o analisador de causas), `review.md` (the full reading: findings, problems, process errors),
`feature-trace.md` (the road flow traced by hand and the generic feature tracer's causes), `how-we-work.md` (the binding way, step by step, and the contradictions settled), `live-instance-design.md` (the live instance, the contracts' guards, the player's input, the order of the work).
Antes de começar, leia o registro da sua linha. Ao fechar o tema, atualize-o com hora e commit, no mesmo commit.

**Where the tree stands (2026-11-10).** `native/`, `project/`, `tools/` and `log/` were taken back to `a1a771c4` (2026-11-07 20:29) by the owner's order: the city generated there is whole, and the lab ports made from 2026-11-08 21:30 on broke it.
Main as it stood is the tag `antes-da-volta-2026-11-10`: read it for WHAT was built and found; its lab ports never come back (the earlier fixes to this game's own code were brought back by the owner's authorisation, being nothing of the roadcraft). Each performance fix and each feature is made again one at a time, designed for this architecture,
measured (memory, CPU, GPU, frame) and photographed in the 24 km city (map `km24`) before and after. Code translated from the 4.0 still stands here (the translated runtime in `core/`, `flow_control`): it is redone the same way, not regenerated.

## Metodologia de trabalho (toda tarefa, nesta ordem; trabalho direto)
1. **Identify:** read the code involved and ask "why" until you reach the design decision that produces the symptom. List every problem.
2. **Research** with context7 on every implementation and fix that touches an outside API (Godot, godot-cpp, Terrain3D, a library): the official documentation and sources, never memory alone.
3. **Compreenda:** ligue problema, causa e solução pelo código e, quando houve, pela pesquisa.
4. **Plan** the fix of the cause, with one owner, always choosing the most expensive and laborious solution that solves the
   cause (owner, 2026-11-11): a cheap solution that does not solve the cause is forbidden, and so is offering cheap options.
5. **Codifique** sem tentativa e erro: se não deu certo, volte à leitura até entender.
6. **Verify what the change reaches**, once (the table of what each kind of change asks for, below). A photo is judged on its own (nothing broken,
   out of shape, overlapping or missing), never made equal pixel for pixel to another run, an older commit or a kept reference.
7. **Pare e confira:** passou ou deu erro? Se deu erro, volte ao passo 1 com o que o erro mostrou.

At a theme's close, the line's record gets the problems, the sources, the reasoning, the plan and the files created, changed and deleted.
A reported problem is solved with the `trace-fix` skill (`python .claude/skills/trace-fix/ledger.py resume` first): the flow traced end to end, read whole with the Read tool, recorded on disk, fixed at the cause, tested and photographed; its hooks refuse an edit of a file not read whole.

## Correção de causa
- A correção muda a decisão de projeto que produz o sintoma. Se a correção certa exige reescrever, reescreva.
- É remendo, e é proibido: mudar tempo-limite, limiar, número de threads ou tolerância; criar caso especial; engolir erro;
  afrouxar uma verificação; regravar a referência (`--bless`) para o teste passar; mover o custo de lugar.
- Toda correção diz, com arquivo e linha: qual é a causa, por que o sintoma não volta por outro caminho e quais outros
  sintomas somem junto.
- Caro não prova causa e barato não prova remendo: o critério é o de cima.
- **Nothing of the roadcraft is copied, ever** (owner, 2026-11-11, binding in every prompt, skill and order): no line,
  line-by-line translation, structure, name or function of the roadcraft lab, which is only a reference for what the
  features must reach; the lab ports in the tag `antes-da-volta-2026-11-10` came from it and never come back. The fixes made
  earlier to this game's own code were brought back from the tag by the owner's authorisation (2026-11-10), being nothing of
  the roadcraft: do not confuse the two. Code of the roadcraft that came in is redone at its cause, designed for this tree.

## Princípios
- A causa dos problemas de desempenho é a arquitetura. Sombra, polígono e tempo de GPU são sintomas e mudam com a placa.
- Faça o que foi pedido, inteiro. Nada de maquiar resultado ou de deixar para depois.
- Nunca espere às cegas um processo lançado (tempo-limite e acompanhamento). Pare só processos próprios, pelo PID.
- Nunca feche o jogo aberto na tela do usuário: `tools/build/free_dll.sh` libera a DLL.
- Tests and captures never open a window on the user's screen (`tools/tasks` runs them `hidden`, on a private desktop).
- Toda mudança no jogo é entregue, ao fechar o tema, com um executável novo: o usuário abre `dist/windows/Roadcraft.exe`.
- Trabalho direto na `main`, em `C:\Users\Jonathan\Desktop\city`, sem worktree.
  Commit por tema e push em `origin/main` (`https://github.com/designathan1988/city`).
- **The lab is a reference, never code to copy.** `C:\Users\Jonathan\Documents\Projects\lab\roadcraft` is the game with everything working. Read it only to write down WHAT the game must do: the behaviour, the rules, the cases, the values, what the player sees.
  Never copy its code, translate it line by line or reproduce its structure (it is a browser's, on one thread): the code is designed for this game's architecture and checked against that behaviour in tests and photos. `roadcraft-4.0-main` is old and no reference.
- A launched process runs in the foreground or under a monitor, with a short deadline and its lines shown as they come: never in the background unwatched.
- A photo is taken only when the view is drawn (`{"until": "render_pending", "equals": 0}` before each `shot`) and is shown with its commit and date written on it.

## Execução e verificação
- Investigating is reading code: no measurements, profiles or long, heavy tests to investigate. A measurement only checks the effect of a change its cause already explains; every feature and performance fix is measured in the 24 km city at its end, the previous build and this one.
- Verifica-se só o que a mudança alcança:

| Mudou | Verifique |
|---|---|
| Tool under `tools/` | its own tests and `run_gates.py --file` on the changed files |
| GDScript, cena ou dados | `quick.sh <cenas ligadas>` e os gates dos arquivos mudados |
| C++ | as duas DLLs e `quick.sh <cenas ligadas>` |
| Fechamento do tema, antes do push | `quick.sh --ready`, `run_gates.py` completo e o executável |

- Prova visual, só em mudança visual: do elemento, de perto e de vários ângulos.

## Ferramentas e processos
- Independent tasks run in parallel, each with its own deadline; one that hangs or fails is stopped and recorded, and the others go on.
- Nada é refeito sem mudança de entrada: o resultado fica guardado pela chave das suas entradas.
- Cada ferramenta processa só o que a mudança alcança. A execução completa é uma opção explícita.
- Ferramenta de verificação tem de poder reprovar: sai com código diferente de zero na falha, e conjunto vazio reprova.
- **The feature tracer is generic, never "engessado"** (owner, binding): no rule, table or case for a program, library,
  API or pattern; specs and official declarations only; a gap fixes the mechanism (gate: `featuretrace/tests/test_generic.py`).

## Idioma
- Todo texto novo ou alterado no repositório é escrito em inglês: código, nomes de arquivos e pastas, comentários,
  identificadores, dados, registros, relatórios, documentos e mensagens de commit. Escreva em inglês mesmo quando o arquivo,
  o registro ou este documento estiverem em português (a tradução do que já existe está em `log/pendencias.md`).
- Exceções: os textos que o jogador vê (traduções em `project/data/i18n`) e as cópias externas. A conversa com o usuário é
  em português.

## Regras de arquitetura
- **Fonte única da verdade:** `CityState` (`native/src/sim/state/city_state.h`). Só `sim/commands` muda o estado a pedido
  do jogador; só `sim/simulation` avança o tempo.
- **Camadas C++:** `core` → `sim/*` (C++ puro, sem Godot) → `render` (só lê) → `api`.
- **Sem arquivo-deus:** nem classe-deus repartida em vários arquivos, nem funcionalidade espalhada. Divide-se por
  responsabilidade, nunca para caber num limite.
- **Um dono por definição:** regras com dono único em `tools/gates/owners.json`. Sem código duplicado: extraia um dono.
- **Dados:** valores ajustáveis só em `project/data/**` (JSON com schema), lidos só por `DataTables`. Nomes do tuning na
  X-macro `CITY_TUNING_FIELDS`, valores em `project/data/catalog/tuning.json`.
- **Sem números mágicos** em `native/src`, `project/game`, `project/ui`: dê nome ou mova para dados.
- **Interface:** não guarda números do jogo: lê `CityStore.snapshot` e envia `CityStore.dispatch(...)`. Cores e tamanhos só
  por `data/ui/tokens.json` e `ui/theme/theme_builder.gd`; textos só por `tr("CHAVE")`; botões e opções em `data/ui/*.json`.
- **Fluxo:** troca de tela, pausa e saída só por `GameFlow`.
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

## Comandos
| O quê | Comando |
|---|---|
| Rodar o jogo | `godot --path project` (sem Vulkan: `--rendering-method gl_compatibility`) |
| Compilar as duas DLLs | `MINGW=... tools/build/build.sh core-windows` (Linux: `core`) |
| Check a change | `tools/build/quick.sh [task,task]` (= `python tools/tasks/cli.py run quick`: only the tasks whose inputs changed; `--ready` at a theme's close, `--all` nothing from the cache, `--bless` the city's digest written again, only for a change meant to move it; its frozen photos are for looking, compared with nothing) |
| Run one test | `python tools/tasks/cli.py run fast <scene>` |
| Suite in parallel | `python tools/tasks/cli.py run tests` (logs in `dist/tasks/<run>`, `cli.py status`; every task has its own deadline and is killed by PID) |
| Gates | `python tools/gates/run_gates.py` (`--file X`, `--files-from F`, `--summary`, `--selftest`); changed files only: `python tools/tasks/cli.py run gates_changed` |
| Scripts that do not compile | `python tools/tasks/cli.py run scripts` (`tools/build/check_scripts.gd`) |
| Orchestrator | `python tools/tasks/cli.py run <suite or task> [--list]`, `serve` (JSON lines, for Node tools), `status`; its tests: `python -m unittest discover tools/tasks/tests` |
| Exportar e empacotar | `tools/build/build.sh export && tools/build/build.sh package` |
| Captures | `tools/build/build.sh capture [folder]` (= `cli.py run capture`, hidden, one task per plan) |
| Olhar o jogo por um plano | `godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<json> --capture-dir=<pasta>` |
| Where two photo sets differ | `python tools/build/photodiff.py <a> <b> <out>` (a help to find where to look, never a verdict: two runs of the same code differ in pixels) |
| Onde cada thread gasta | `python tools/analysis/sampler.py --hidden --cmd "<godot> --path project <cena>" --seconds 30 --symbolizer <llvm-symbolizer>` |
| Causas pela forma do código | `python tools/analysis/causes.py` |
| Causas pela arquitetura | `python tools/analysis/arch.py scan`, `status`, `next`, `explain <id>`, `predict <id>`, `done <id>`, `selfcheck` (`--only x` refaz só o que falhou; fila em `dist/arch/queue.json`) |
| Perfil da DLL de release | `tools/build/pgo.sh` (refazer depois de mudar laços quentes) |

**O que um clone precisa:** `native/godot-cpp` (API 4.7) e as DLLs em `project/bin` já vêm no repositório: o jogo abre sem
compilar. Meshoptimizer e Clipper2 vêm pelo CMake na primeira compilação. À parte: Godot 4.7.2, llvm-mingw
`20261020-ucrt-x86_64` (`C:/Projects/toolchains`), CMake 4.4, Node 23.6+, Python 3.12. A DLL pede x86-64-v3 (AVX2, FMA, BMI).

**DLLs em rebase:** conflitam sempre. Nunca escolha um lado: refaça as duas a partir das fontes juntas e rode a suíte.

## Estrutura do projeto
`C:\Users\Jonathan\Desktop\city` é o projeto inteiro. O detalhe arquivo por arquivo está em `log/mapa.md`.

**`native/src`: núcleo C++**
- `core/`: bytes, colour, grid, job pool, maths, memory, polygons, random, units; the translated runtime still there is a debt, redone at its cause.
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
- `render/`: `RenderWorld` (consome o `ChangeLog`, aplica os orçamentos), `rs_util.h`, `mesh_builder`, `chunk_queue.h`,
  `chunk_jobs.h` (orçamento de upload), filas, níveis e ordem das malhas, luzes, gradação de filme e ponte com o Terrain3D.
- `render/{agents,buildings,roads,props,trees,cover,water,transit,effects,overlays,atmosphere,wear}/`: cada coisa desenhada.
- `api/`: `CityCore` (repartido por área em `city_core_*.cpp`) e as áreas de script (`builder`, `landscape`, `transit`,
  `street`, `probes`, `lab`), carregadores de dados.

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
- `tasks/`: the one orchestration owner of every support tool (tasks as data: inputs keyed by content, a deadline each,
  a verdict; parallel pool, cache in `dist/cache/tasks`, Job Object kill, hidden desktop; `suites.json`).
- `build/`: `build.sh`, `quick.sh` (a thin call of `tools/tasks`), `pgo.sh`, `photodiff.py`, `free_dll.sh`,
  `check_scripts.gd` (scripts that do not compile), `rename_english.py` (the English rename, with `--check`).
- `analysis/`: `sampler.py` with `sampler_report.py` and `unwind.py`, `causes.py` with `detectors.py` and `code_parse.py`, `arch.py` and `arch/` (the architecture analyser), `arch_eval/` (answer keys only its selfcheck reads).
- `assets/`: cozinha de pessoas, marcos, ruído, móveis, postes e perfis de via.
- `converter/`: the old porting tool, with its own git; it produces and regenerates no game code (translating the roadcraft is forbidden). What it translated and still stands (`flow_control`) is a debt, redesigned at its cause.
- `converter-slice/`: the converter's older slice, with its own git.
- `process/`: ferramentas de processo e registros. `ports/`: planos, coberturas e relatórios de cada porte.

**Outros:** `dist/` (builds, capturas e logs, fora do git) e `log/` (registros).

## Fluxo de dados e threads
- `DataTables` lê `data/**` → `CityStore.boot` envia o catálogo ao `CityCore`, que rejeita chave ausente.
- `Simulation` roda em thread própria com tick fixo (`sim_tick_hz`), sob `map_mutex` compartilhado e `traffic_mutex`.
- Commands and the clock take `map_mutex` exclusively; growth decides under the shared lock and takes the exclusive one only to add (`grow_step`).
- `ChangeLog` é o único canal de "sujo", drenado sob trava compartilhada e o mutex do log.
- `RenderWorld.sync` nunca toma o mapa exclusivo. Manda as malhas ao `JobPool`, que leem só o mapa, nunca o tráfego.
- A mesh job copies what it reads under a short shared lock (`MapRead`, `CityCut`, `HeightPatch`) and builds with the lock let go.
- The land's maps are `TerrainBridge`'s (fine ground round the view, far ground whole); Terrain3D keeps no region. A record's derived body has one owner, the tables' `BodyCache`, bounded in bytes.
- A conversão de malha entra em `render.upload_budget_mb` por quadro; o commit só troca malhas, em `render.commit_budget_ms`.
- Poses por `PoseBuffer`. O save grava em thread própria.

## Luz e relógio
- `Daylight` is the owner of the sun, the ambient and the sky. At night the sun becomes the moon, which casts all night (P347); the street's light is `LampLights`.
- `CityWorld.set_sky_mode("day"|"night"|"cycle")`: dia e noite fixam as horas de `daylight.json` (16h e 22h).
- Testes e capturas movem relógio e céu juntos (`UiDriver.at_hour`).

## Máquinas de estado
- `GameFlow.State`: BOOT → MENU → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram e saem de STOPPED.
- `ToolController.Tool`: uma por ferramenta, de mãos vazias é INSPECT; só `set_tool` troca; segurado `drag`/`tick`, solto `release`.
