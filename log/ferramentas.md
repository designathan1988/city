# Ferramentas e detalhes do projeto

O que cada ferramenta faz e ensinou, e o texto completo do que o `CLAUDE.md` resume.

## Ferramentas (o que cada uma faz, o que mediu e o que ensinou; 2026-11-05)
- **Tempos medidos** (RTX 3060, 24 núcleos): ciclo `fast` de 6 a 15 s (eram 3 min com as duas DLLs em série e `-j 4`); suíte
  inteira 65 a 75 s com 8 em paralelo (~30 min em série); a cidade do mapa 0 abre em 211 ms (`look`, 6,5 s a rodada inteira); a
  captura da cidade faz 52 fotos em 48 s; `capture_roads` gasta ~43 s antes da primeira foto (causa a medir com o `sampler.py`).
  Lento é defeito de método: medir onde vai o tempo e trocar o método, nunca esperar.
- **`runq.sh`** (deleted 2026-11-07, replaced by `tools/tasks`): fila de jobs, cada um vigiado pelo log (parado = travado, morto só pelo PID do próprio job); um script que não
  compila quebra o job na hora. `fast` compila os scripts antes da DLL (o jogo aberto prende a DLL: nada de jogo ao lado do link).
  A DLL de release é refeita para o commit.
- **`suite.py`** (deleted 2026-11-07, replaced by `tools/tasks`; sessão de causa-raiz): mesma lista e regra de falha do `run_tests.sh`, todas as cenas ao mesmo tempo; uma
  falha sob carga é repetida sozinha e dita INSTÁVEL, nunca escondida. **`sampler.py`**: congela cada thread por ~0,1 ms a cada 5 ms (copia a pilha e desenrola depois) e soma as
  pilhas (símbolos pedem a DLL com informação de depuração: `-DCMAKE_BUILD_TYPE=RelWithDebInfo` numa pasta de build à parte, essa
  DLL nunca é commitada). **`causes.py`**: analisador estático de causas-raiz (<1 s).
- **Sombra se julga pelo tempo de GPU**: `render_info` conta a malha colorida, não a de sombra (`render_forward_clustered.cpp`), e `mesh_set_shadow_mesh` só vale para material sem `vertex()` (`uses_shared_shadow_material`), então os nossos o ignoram; o caminho é a instância `shadow_only`. `camera_pan --casters=MASK` dá a sombra por camada.
- **Bug de memória e de tempo não se caça com carga**: a DLL de debug tem o hardening da libc++ (índice fora do limite para na hora, com o lugar); uma falha que só aparece sob carga se lê na pilha (`llvm-symbolizer` com a DLL de `RelWithDebInfo`) e no código. Prova visual é do elemento, de perto e de vários ângulos.
- **Gates**: inteiros atravessam o repositório todo; na iteração, `gates_changed.sh`.
- **Rebase e as DLLs**: elas conflitam sempre (binárias, as duas branches mudaram o C++): nunca escolher um lado; refazer as duas a
  partir das fontes juntas e rodar a suíte de novo no código juntado.
- **`look.gd`** (+ `look_measure.gd`): o `scripts/look.mjs` do 4.0 no jogo. Plano `{"city": {map, seed, generate}, "steps": [...]}`;
  passos `setView [x, y, zoom, azimute°, elevação°]` (o `y` do mapa do 4.0, cujo norte é o nosso `-z`: a câmera exata de uma foto dele, `PairCamera`; abaixo de ~11° o 4.0 limita a inclinação), `shot`, `wait`, `frames`,
  `hour`, `dispatch`/`query` (args `{"at": [x, z]}`, `{"profile": id}`, `{"v3": [...]}`), `key`, `click`, `drag`; as medidas vêm de
  RenderingServer e Performance (`look_measure.gd`: gravação dos quadros com p50/p95/p99, engasgos >25 ms e vsyncs perdidos pelo
  período do p10; uso com pico/média/mínimo; draws, primitivas e objetos vistos e de sombra; memória; compilações de pipeline;
  passes da GPU pelos carimbos do RenderingDevice; os passos `record`, `usage`, `until`, `view`, `passes` e `info` no `look.gd`
  ainda a ligar; `clock` e `switch` ligam os carimbos e uma chave do F7). Uma pergunta de diagnóstico vira um passo `query` no mesmo log das fotos.
- **`photodiff.py`**: diferença pixel a pixel de duas rodadas (barra e dock mascarados): a parte mudada, a caixa e o mapa vermelho
  por foto. Diz onde olhar, não o veredito: na elevação toda borda das vistas baixas mudou porque a câmera mira o chão do `raycast`
  (0,6 m abaixo da pista, o chão sob a via é rebaixado), não porque a via mudou; foi medido com `query height_at`.
- **O par com o 4.0 rodando é a prova que mais acha defeito**: `npx vite` em 5300 na pasta dele, `node scripts/look.mjs <plano> <saída>` lá e o mesmo plano no `look` (`{"size": "small", "seed": 7}` e `{"map": 0, "seed": 7}`); as matas caem árvore por árvore no mesmo lugar, e o que não bate é defeito. Foi assim que apareceram a cidade espelhada e a névoa no dobro. Comentário do 4.0 não prova nada sem o par.
- **Equivalentes nativos do Godot 4.7** (lidos em `native/godot-cpp/gdextension/extension_api-4-7.json`, a API exata do projeto):
  tempo de CPU/GPU do viewport `viewport_set_measure_render_time` + `viewport_get_measured_render_time_cpu/gpu`; por quadro
  `viewport_get_render_info` (OBJECTS, PRIMITIVES, DRAW_CALLS; tipo VISIBLE ou SHADOW); `get_rendering_info` (VIDEO, TEXTURE e
  BUFFER_MEM_USED, PIPELINE_COMPILATIONS_*); passes da GPU em `RenderingDevice.get_captured_timestamps_*` (Forward+); vistas de
  depuração `Viewport.debug_draw` (OVERDRAW, WIREFRAME, NORMAL_BUFFER, UNSHADED, LIGHTING, SHADOW_ATLAS, PSSM_SPLITS, OCCLUDERS,
  DISABLE_LOD…); `Performance.get_monitor`.
- **Tradutor (`tools/converter`, sessão própria)**: traduz TypeScript puro do 4.0 em C++ compilado e verificado; não tem alvo GDScript
  nem DOM (a interface é sempre à mão, com o Godot nativo). `feature` acha a fronteira; `convert` traduz o que é puro (spec gerado) e
  deixa o resto como `method`/`write` no livro-razão; `differential --hand <arquivo>` compara C++ escrito à mão com o TS nos mesmos
  dados e mostra o passo e o valor de cada diferença. Valores do Tuning entram por padrões da library (`ROAD_TUNING.grade.ramp` →
  `tn.grade_ramp`, `context` `const Tuning &tn`); uma struct com nome que o jogo já usa ganha o nome do módulo (`ElevationProfile`).
  `scripts/*.mjs` do 4.0 ficam fora do programa TS. Área do laboratório no núcleo: `native/src/api/lab` (camada api; uma camada nova
  pediria `rules.json`, que é do usuário).
- **A medida contra o 4.0 vale mais que a tese**: um vitest de poucas linhas em `tools/oracle-4.0/tests/world/` (nome sem `zz` nem
  `_`, que o `vitest.config` exclui; resultado gravado em arquivo, o console sai calado) dá o número do 4.0 em 2 s. Assim o greide
  bateu: o teste usava a classe errada (`avenue4Lanes`; o tipo 2 do 4.0 é `avenue`), a ponta sem saída não é junção (só a margem de
  2 m) e uma junta onde só a estrutura muda é transição (margem), como em `junction/build.ts`.
- **Ferramentas do 4.0** (`city\roadcraft`: `scripts/`, `src/lab`, a API `window.__roadcraft` de `main.ts`, `reports/`) e o
  porte: `verify.mjs` (só o que a mudança tocou + goldens bit a bit da cidade e do tráfego de uma semente + censo + fotos contra a
  referência aprovada) e o relatório "Verificação rápida sem suítes" (portões com orçamento, hash do estado por tick, asserções em
  níveis, save ida e volta, foto com relógio congelado a 0,1 %) — a portar; `look.mjs` — em parte (`look.gd`); `look-compare.mjs`
  (HyAB em OKLab) — a portar no `photodiff`; `lib/measureCity.mjs` (recusa GPU/CPU ocupadas por PID e memória livre <3 GB, cena
  assentada, a mesma cena em toda janela), `displayAwake` (monitor apagado = 30 Hz), `sessionJob` (teto de CPU e memória por sessão)
  — a portar nas medidas; `frames`, `layers`, `material*`, `gpuSteady`, `buildingParts`, `shellMaps` (A/B intercalado, mediana) — a
  portar como passos do `look`; o laboratório F7 (`src/lab`, ~14 mil linhas: Diagnóstico com veredito GPU/CPU, causas por ms e A/B
  de um clique com o ruído medido; medidores; consumidores; verificações de vazamento, pipelines novos e ritmo; perfil de CPU com
  suspeitas, como produto refeito sem mudança e biblioteca cobrada do chamador; cobertura; GPU com passes, captura de quadro, objetos
  e vistas; gravador; testes; otimizações; Inspector com a entidade sob o cursor e as 5 alturas do ponto) — a portar: a lógica pura
  pelo tradutor em `native/src/api/lab`, a interface à mão. Lidos inteiros: `scripts/` de topo exceto `probe.mjs`, `check-code`,
  `model-catalog*`, `model-sheet`, `cook-*`, `bake-people-pics`, `import-makehuman-pack`, `package-desktop` e `accuse`; `src/lab`
  núcleo, `doctor`, `resources`, `inspector`, `code` e `gpu`. A ler: `src/lab/{recorder,tests,optimizations,catalog}`,
  `scripts/{probe,diagnose,lab,vite,check-code}`, `tests/{arch,census,fuzz,scale,perf}`, `eslint-rules`, `desktop`.


## 2026-11-07: one orchestration owner for every support tool (English)
**Cause of the slowness, read in the code:** no tool declared its work as data (inputs, deadline, verdict). Each one was an
imperative script driving its own processes, so each invented its own scheduler and guesses: hangs judged by log silence
(`runq_jobs.sh`), by CPU-time change (arch supervisor) or not at all (converter `spawnSync`/`execFile`, `build.sh capture`,
the import/scripts/build/digest steps of `quick.sh`); serial loops (`runq.sh`, `run_tests.sh`, `run_gates.py`, sandbox steps,
differential seeds) where one stuck task held the rest; outputs wiped instead of keyed (`run.py rmtree`, `quick.sh rm -rf
photos`, converter output folders), so nothing was reused; change scope by a hand-written path table (`quick.sh scenes_of`)
or `git status | awk '{print $2}'` (`gates_changed.sh`); improvised exit codes (`--summary` returned 0, "END OF QUEUE" 0,
empty sets 0, a test failing under load and passing alone counted as a pass). The arch selfcheck hang was this decision in
one place: `scan.py` took any `OSError` of `os.rename` for "another task won", deleted its copy and waited forever on a
marker, while the supervisor, having no deadline, took the waiting loop's CPU ticks as life.

**Fix:** `tools/tasks/` is the one owner (contract: a `Task` with argv or a Python call, a required `timeout_s`, content-keyed
`inputs`, `extra`, `outputs`, `after`, `resources`, `hidden`, `verdict`).
- `inputs.py` keys files by git blob ids (never modification times) and is the only owner of "what changed".
- `cache.py` stores PASS/FAIL by key in `dist/cache/tasks` (TIMEOUT and BROKEN are never cached), plus `memo` for
  sub-results and `materialize` (a shared directory under an OS lock with a deadline; errors raise).
- `proc.py`/`win32.py` start every task in a Windows Job Object (KILL_ON_JOB_CLOSE) or a POSIX process group, optionally on
  the private desktop `WinSta0\roadcraft_tests` (moved from `hidden_run.ps1`), and kill the whole tree by PID at the deadline.
- `pool.py` runs the dependency graph in parallel within cores and OS readers/writer locks (`dll`: builds exclusive, Godot
  runs shared); a failure only SKIPs its dependents; recorded times order the work and never kill.
- `cli.py run|serve|status|--list`; `suites.json` (schema in `tools/gates/schemas/suites.schema.json`) holds the suites
  `quick` (`--ready`, `--all`, `--bless`), `ready`, `fast`, `tests`, `capture`, `gates`; the Godot test list and timeouts
  moved there from `run_tests.sh`. Exit 0 only for a non-empty selection that all passed; 1 for any failure; 2 for usage
  or an empty selection. `serve` speaks JSON lines for the Node converter.
- Deleted: `tools/analysis/suite.py` (with its UNSTABLE rule), `tools/build/run_tests.sh`, `runq.sh`, `runq_jobs.sh`,
  `hidden_run.ps1`, `gates_changed.sh`; `tools/converter/scripts/live-run.mjs` (it guessed hangs from the slowest item).
- `quick.sh` is `exec python tools/tasks/cli.py run quick "$@"`: a scene reruns exactly when an input of it changed (the
  debug DLL is an output of `build_debug` and an input of every Godot task); a missing golden fails ("run --bless").
- `photodiff.py` pairs the union of names; a missing side, an empty set or any changed pixel fails; it reports the mean
  difference and the per-channel shift (the 24-level cut is only for the red picture).
- Captures exit nonzero on incomplete photos (`CaptureWait.exit_code`); `look.gd` fails a plan with zero shots.
- `sampler.py` runs its target hidden under a deadline, kills the tree in `finally`, caches symbols by (module hash,
  address), fails on zero samples. `causes.py` scans the files from `inputs.py` (native/src and project scripts, no
  converter copies or fixtures), parses each file once per content (`code_parse.py`), roots per-frame at real
  `_process`/`_physics_process`/`sync`/`tick` member definitions (the 32 ENGINE_CALL_IN_LOOP findings were boot loaders).
- Gates (`tools/gates`) rebuilt on lexers (C++/GLSL/JS, GDScript, Python, shell, Godot resources, XML, prose): rules read
  tokens, not raw lines; `--file`/`--files-from` judge only what was asked; scope from the git snapshot; each rule in its
  own try (`GATE_CRASH` fails); per-file results memoized by content; `--selftest` runs one failing and one passing
  fixture per rule (60 cases, 30 rules). New rules: SIDECAR_WITHOUT_SOURCE, PYTHON_IMPORT_UNRESOLVED (it would have
  caught the 11 broken `tools/process` imports of the English rename), NON_ENGLISH_TEXT on changed lines.
- Converter: every child process is a task (`src/common/tasks.ts`); one machine-config resolver (`src/common/machine.ts`,
  env vars only override; tests pass without them); derived graphs keyed by their inputs (`src/common/derived.ts`); the
  differential compiles a translation once and reads each seed as data.

Commands: see CLAUDE.md "Comandos". Tests of the orchestrator: `python -m unittest discover tools/tasks/tests` (26 pass).

## 2026-11-08 03:40: two processes never write one output, nor swap one shared file, at once (English)
**Problem:** two converter extractions run together (zoning and bomb) each asked their own `cli.py serve` for the same
Jelly analysis (one key). Both Jelly processes wrote `tokens.json` in place; the first to end stored in the cache a copy
taken while the other was still writing, torn in the middle (JSON broken at byte 51,460,743 of 133 MB), and every later
run restored the torn file. `Cache.store` assumed "another process stored the same key first: that is the same result",
false when the outputs are shared files written in place. A second fault of the same kind: two runs started in the same
second both `os.replace` onto `dist/tasks/latest.txt`, and on Windows a rename over a file another process renames or
holds open fails (access denied).
**Fix:** a task holds one exclusive OS lock per declared output while it runs (`pool.py` `_lock_names`, taken all at once
by `locks.py` `take_all`, in one global order), and with its locks held it looks in the cache again (`_hit` in
`_execute`): a run that waited takes the result the other stored. Shared files (`latest.txt`, `times.json`, a memo) are
replaced and read under their own lock (`cache.py` `write_atomic`, `read_shared`). Test:
`test_two_runs_never_write_one_output_together` (a task writing its output in two parts, run twice at once: one runs,
one takes the stored result, the cache holds the whole file); without the output locks it fails. The 24 tests that
`unittest discover` finds pass; the gates pass on the six files.

## 2026-11-08 04:35: a git started by `cli.py serve` hung on serve's own request pipe (English)
**Problem:** the converter's whole-program convert died three times at the same step ("the compile of all_headers.cpp:
BROKEN after 0 s", keying failed: `git ls-files` past its 120 s). Watched live: the git had one thread waiting in the kernel,
no CPU, five modules loaded, a console of its own; its parent, a `serve` started that same second. Cause: `inputs._git` ran
`subprocess.run` with no `stdin` when it had no input, so git inherited serve's stdin, the request pipe from the converter, on
which serve's reader thread waits in a synchronous read. Git for Windows asks each standard handle's name at its start (to
know a MSYS terminal), and that query on a synchronous pipe waits for the pending read: git went on only when another request
came, never when the request that needed it was the only one (a single header compile, a fresh serve). Not the machine's load,
not the worktree's sandbox (both were suspected first and ruled out by the stuck process itself).
**Fix:** a git given no input reads NUL (`inputs._git`); every other child of the orchestrator already did (`proc.py`,
`win32.JobTree`). Test: `test_serve_answers_with_its_request_pipe_open` (one request, the pipe left open: the answer must come
in 60 s); with the old spawn it fails after 60 s, with the fix it passes.
**And:** the pool read the repository again after every task (`snapshot.invalidate` on each finish), so each of the
converter's hundreds of compiles, run in its cache outside the repository, listed it with git anew. Now only a task that may
write into the repository does: a declared output inside it, or no declared output and a run inside it (`pool._writes_repo`,
the contract in `Task`'s docstring). Test: `test_a_run_outside_the_repository_keeps_its_snapshot` (fails if every finish
invalidates). The task's locks are named by `locks.task_lock_names` (pool.py stays under its 300 lines). 26 tests pass.


# Texto completo das seções resumidas no CLAUDE.md (até 2026-11-06)

# Cidade — city builder (Godot 4.7 + núcleo C++ GDExtension)

Único documento de regras do projeto. Regras, comandos, mapa do código e pendências ficam aqui; não crie outros
documentos nem memórias (o gate `DOC_UNICO` confere), fora os registros de sessão em `log/`. Atualize este arquivo na mesma
mudança que altera o que ele descreve: o gate `MAPA_DESATUALIZADO` falha quando o mapa diverge do disco.

**Registros de sessão (`log/`, ordem do usuário, 2026-11-05):** antes de começar, leia todos os `log/*.md`. Cada um é o
histórico de uma linha de trabalho: o que foi feito, o estado, os comandos e o que falta, em ordem.
- `log/work-log.md`: porte (elevação, postes, tradutor) e gráficos das construções, nos 7 passos obrigatórios do usuário.
- `log/conversor.md`: tradutor.
- `log/causa-raiz.md`: desempenho e causas.
- `log/lab.md`: laboratório F7.
- `log/base.md`: otimização da base e qualidade (compilação, memória, CPU, GPU, pessoas).

Ao terminar cada tarefa, ou ao saber algo novo, atualize o registro da sua linha (crie um se a linha for nova), com hora e
commit, e commite-o junto.

## Processo de trabalho (inegociável, toda tarefa, nesta ordem; trabalho direto, sem agentes)
1. **Análise:** ler o código envolvido, reproduzir e medir (sonda `render_cost`, `tests/perf/camera_pan`, capturas, CPU,
   GPU, memória) até achar a causa raiz. Corrigir a causa, nunca o efeito. Medir é desta etapa.
2. **Pesquisa** na documentação oficial (Godot, godot-cpp, Terrain3D) até entender o que fazer e o método; nunca confiar
   só na memória.
3. **Compreensão e plano** antes de codificar: a solução completa e correta, nunca a mais barata; sem arquivo-deus nem
   funcionalidade espalhada; regras deste arquivo (dono único, sem número mágico, dados em `data/**`, mapa em dia).
4. **Codificação** sem tentativa e erro: se não deu certo, voltar à documentação até entender.
5. **Teste, rápido e sem afrouxar** (ver "Ferramentas"): na iteração, `tools/build/runq.sh <cópia> <pasta> fast
   test:<cena>` (scripts compilados, só a DLL de debug com todos os núcleos, só o teste afetado: segundos); antes do commit,
   os gates dos arquivos mudados (`tools/build/gates_changed.sh`), as duas DLLs (`tools/build/build.sh core-windows`) e a
   suíte inteira em paralelo (`tools/analysis/suite.py`, ~50 s); "Gates OK" em `python3 tools/gates/run_gates.py` antes do
   push. Medir de novo: quadros sem travar, GPU nunca a 100%, CPU e memória nunca acima de antes. Fotos tiradas e analisadas
   com atenção: a diferença contra o estado anterior (`tools/build/photodiff.py`) diz onde olhar e cada mudança é explicada
   pela causa; nenhuma perda de qualidade visual; todo problema achado é corrigido.
6. Nunca esperar às cegas um processo lançado (tempo-limite e acompanhamento); parar só processos próprios, pelo PID.
7. Trabalho direto na `main`, em `C:\Users\Jonathan\Desktop\city`, sem worktree (ordem do usuário, 2026-11-06). Commit por tema e push regular em `origin/main` (`https://github.com/designathan1988/city`, a pasta única do projeto: ver
   "Estrutura do projeto").
- **Referência — sempre acessível:** `C:\Users\Jonathan\Documents\Projects\city\roadcraft` (Roadcraft 4.0, a cópia de 29/10; a `roadcraft-4.0-main`, de 23/10, é antiga e não vale: não tem, por exemplo, os postes urbano e rural) é a referência
  de visual e de comportamento de tudo (gráficos, prédios, tráfego, semáforos, pedestres, carros, postes e fios): o jogo
  deve ter todos os gráficos e funcionalidades dele. A pasta pode ser lida, aberta, rodada e estudada a qualquer momento.
  O código dele é referência de comportamento, não de implementação: não se porta o código; portam-se as funcionalidades,
  cujo rastreamento passa pelo conversor (`tools/converter`: `scripts/port.ts extract`, `node src/cli.ts feature`) e cuja conversão é feita na arquitetura
  atual, otimizada; nada de colar código não otimizado sem a arquitetura e os gates.
  **Exceção (ordem do usuário, 2026-11-05):** a lógica pura de simulação entra traduzida, com a estrutura do 4.0, sem edição à mão, ligada ao jogo por uma camada fina e conferida contra o 4.0; regenera-se, nunca se conserta o gerado. Primeira área: fluxo e controle (`flow_control`).

## Regras de arquitetura (verificadas automaticamente)
- **Fonte única da verdade:** `CityState` (`native/src/sim/state/city_state.h`). Só `sim/commands` muda o
  estado a pedido do jogador; só `sim/simulation` avança o tempo. A UI nunca guarda números do jogo
  (`ESTADO_NA_UI`): lê `CityStore.snapshot` e envia `CityStore.dispatch(...)`.
- **Um dono por definição:** valores ajustáveis vivem só em `project/data/**` (tabelas JSON com schema).
  Nomes do tuning: X-macro `CITY_TUNING_FIELDS`; valores: `data/catalog/tuning.json` (`gate_tuning.py` exige
  as mesmas chaves e que todas sejam lidas). Regras com dono único estão em `tools/gates/owners.json`
  (altura do prédio, andar, regra do lote, "há trabalho a salvar", fachada, mobiliário da rua, dinheiro, cobrança das vias, estado do jogo);
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
- **Hash do estado:** `probes/state_hash` é a cidade como um número (os bytes do save, inteiros e por parte). O `edit_fuzz_test` sorteia edições e exige que desfazer, refazer, salvar e abrir passem pelos mesmos hashes: toda edição nova entra no sorteio dele, e toda coisa nova salva já é coberta.
- **Testes e capturas (`--no-flow`) rodam nos dados:** não leem nem gravam `settings.cfg`, nem leem `health.json`; abrem no nível `tests_preset` (nunca `auto`, para não depender da placa de vídeo; outro nível é pedido pelo nome, `--preset=`); saves ficam em `user://saves_tests`. O `user://` é um só para todo clone com o mesmo nome de projeto; para isolar um clone, `project/override.cfg` com `application/config/use_custom_user_dir=true` e `custom_user_dir_name`.
- **Pendências** vão para a seção abaixo, nunca para comentários TODO/FIXME (`SEM_MARCADORES_SOLTOS`).

## Comandos
| O quê | Comando |
|---|---|
| O que um clone precisa | O `godot-cpp` já vem em `native/godot-cpp` (commit e103d348 do oficial, API 4.7) e as DLLs do núcleo em `project/bin` (refeitas e versionadas a cada mudança no C++): o jogo abre no Godot sem compilar. O meshoptimizer e a Clipper2 (booleanas de polígono em inteiros, 2.0.1) vêm pelo CMake na primeira compilação (`FetchContent`, pasta de build; precisa de rede uma vez). Instalados à parte: Godot 4.7.2 (`winget install GodotEngine.GodotEngine`), llvm-mingw `20261020-ucrt-x86_64` (`C:/Projects/toolchains`), CMake 4.4, Node 23.6+ (o tradutor e a cópia do 4.0), Python 3.12 (gates) e, para o oráculo, `npm install` em `tools/oracle-4.0` |
| Gates | `python3 tools/gates/run_gates.py` (`--file X` para um arquivo, `--summary` resumo) |
| Núcleo Linux / Windows | `tools/build/build.sh core` / `MINGW=... tools/build/build.sh core-windows` (DLL de release e de debug, a que o editor e os testes carregam) |
| Testes core + e2e (precisa de display) | `GODOT=/caminho/godot tools/build/build.sh e2e` |
| Capturas de tela do jogo real | `tools/build/build.sh capture /pasta` |
| Exportar + zip | `tools/build/build.sh export && tools/build/build.sh package` |
| Rodar o jogo | `godot --path project` (Forward+; sem Vulkan use `--rendering-method gl_compatibility`). A DLL pede x86-64-v3 (AVX2, FMA, BMI: Haswell e Excavator em diante) |
| Perfil da DLL de release | `tools/build/pgo.sh` (DLL instrumentada joga escondida; refazer depois de mudar os laços quentes; `llvm-profdata show --topn=50 native/pgo/citycore.profdata` lista as funções quentes) |
| Verificação de uma mudança, de uma vez | `tools/build/quick.sh [cena,cena]`: gates dos mudados, scripts e DLL de debug ao mesmo tempo, depois as cenas que os arquivos tocam e o `edit_fuzz_test`; `--ready` refaz as duas DLLs com o mesmo teste curto e deixa `dist/quick/passed`, a prova de que o teste curto passou (~20 s mais a compilação); `--bless` aceita fotos e hash que mudaram de propósito (as fotos congeladas repetem: três bit a bit, a da cidade do alto varia até 0,04% com a máquina carregada, abaixo do limite de 0,1%; sem mudança visual, não precisa dele); `--all` roda toda a suíte (logs em `dist/quick`) |
| Ciclo rápido (iteração) | `tools/build/runq.sh <cópia> <pasta> fast test:tests/core/<cena>`: scripts → DLL de debug (`-j` = núcleos) → o teste; cada job vigiado (log parado 180 s = travado, morto pelo PID), eventos em `<pasta>/events.log`. Jobs: `scripts`, `import`, `build`, `fast`, `gates`, `test:<cena>[@renderizador]`, `scene:<nome>:<cena>[:args]` |
| Suíte inteira em paralelo | `python tools/analysis/suite.py [--root <cópia>] [--only=nomes] [--parallel N]` (logs em `dist/tests`) |
| Gates dos arquivos mudados | `tools/build/gates_changed.sh <cópia>` (`run_gates.py --file` de cada arquivo do `git status`) |
| Olhar o jogo por um plano | `godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<json> --capture-dir=<pasta>` (ou o job `scene:look:tests/visual/look.tscn:--plan=<json>` do `runq.sh`) |
| Diferença de duas rodadas de fotos | `python tools/build/photodiff.py <antes> <depois> <saída>` |
| Onde cada thread gasta | `python tools/analysis/sampler.py --hidden --cmd "<godot> --path project <cena>" --seconds 30 --symbolizer <llvm-symbolizer>` |
| Tradutor do 4.0 (TS → C++) | `tools/converter`: `node src/cli.ts feature`, `convert`, `differential` (ver "Ferramentas") |

## Estrutura do projeto (pasta única, 2026-11-05)
`C:\Users\Jonathan\Desktop\city` é o projeto inteiro, e o que vai ao GitHub: o jogo (`native/`, `project/`) e as ferramentas em
`tools/`, cada uma com o que é dela:
- `tools/gates`, `tools/build`, `tools/assets`: os gates, o build e os assets do jogo.
- `tools/converter`: o rastreador e tradutor do 4.0. Tem git próprio: a sessão dele trabalha nele, e o conteúdo é trazido para
  cá por `git read-tree`. As histórias dele, a do worktree `converter-slice` e a do conversor antigo vão como branches
  `tools/converter/*`.
- `tools/process`: as ferramentas de processo e os registros (ex-`C:\Projects\Roadcraft-windows`): portão, leitura, mapa do
  4.0, hooks, `records/`, `features-4.0/`, `bin/gate`. Os rastreios do conversor antigo estão em `converter-old/out/features`.
- `tools/ports`: o que cada porte escreveu (planos, coberturas, relatórios, regras); os branches dos clones vão como
  `ports/<porte>/*`.
- `tools/oracle-4.0`: a cópia instrumentada do 4.0 para o oráculo (`ORACLE_INSTRUMENT=1`), sem os caches nem os dados de sessão.

## Fluxo de dados e threads
`DataTables` lê `data/**` (tabelas grandes divididas em partes com o mesmo schema) → `CityStore.boot` envia o
catálogo ao `CityCore` (rejeita chave ausente) → `Simulation` roda em thread própria com tick fixo
(`sim_tick_hz`), sob `map_mutex` compartilhado e `traffic_mutex`; comandos, crescimento e relógio tomam
`map_mutex` exclusivo → `ChangeLog` (único canal de "sujo"; drenado sob trava compartilhada + mutex do log) →
`RenderWorld.sync` (nunca toma o mapa exclusivo) manda as malhas ao `JobPool`, que leem só o mapa (vias, rua, dano), nunca o tráfego: o grafo de faixas alcança uma edição no passo seguinte e o passo o reescreve sob a trava compartilhada, então controle e regra do cruzamento vêm da rede (`control_of`, `approach_rule`); o job de prédio copia o que lê sob trava curta
(`HeightPatch`); a conversão da malha entra em `render.upload_budget_mb` por quadro (o envio à GPU roda na próxima chamada do
thread principal) e o commit só troca malhas, em `render.commit_budget_ms`. Poses por `PoseBuffer`; o save grava em thread própria.

## Luz e relógio
`Daylight` é o dono do sol, do preenchimento, do ambiente, da luz de cima e do céu, e encaixa as cascatas na vista (`_fit_cascades`). Da noite fechada em diante o sol vira lua sem sombra: `shadows_on` diz o que o nível pede, `_cast` decide, e o passe sai (é a cidade inteira desenhada de novo, para uma sombra que os postes apagam). A luz da rua à noite é `LampLights`. O ambiente é chapado e a luz de cima, sem sombra, só chega no que olha para o céu: medidos num cubo cinza nos dois jogos (`capture_landscape --only=cubo_a,cubo_b`). `CityWorld.set_sky_mode("day"|"night"|"cycle")`: dia e noite fixam as horas de `daylight.json` (16h e 22h, como a barra do 4.0), `cycle` segue a hora do snapshot passo a passo (`clock_step_h`). Testes e capturas movem relógio e céu juntos, nunca um só (`UiDriver.at_hour`: `set_hour` do céu e `set_day_hour` do jogo). Par de foto com o 4.0: mesma hora (o modo dia do 4.0 é 16h, não o relógio), mesma câmera (`capture_landscape.gd` `_shot` recebe azimute, inclinação e zoom do `setView`) e 1920x1080.

## Máquinas de estado
- `GameFlow.State`: BOOT → MENU (tela inicial) → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram/saem de STOPPED.
- `ToolController.Tool` (de mãos vazias é INSPECT): NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT, MOVE, CONTROL, PAINT, TREES, CLOUD, FOG, WEATHER, ELEMENTS, STRIKE, SHOOT, BUILD, TRANSIT, POLE, BARRIER, STREETSCAPE (só `set_tool`; botão segurado → `drag`/`tick`, solto → `release`); `RoadTool`: um arrasto desenha a via, um clique segura o começo (encadeia) e, no alinhamento CURVE, o clique no fim deixa a curva seguindo o ponteiro até o próximo clique (`Alignment`: STRAIGHT, CURVE, FREE).

