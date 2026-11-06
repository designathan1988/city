# Cidade: city builder em Godot 4.7 com núcleo C++ (GDExtension)

Regras, métodos e estrutura do projeto. Atualize este arquivo na mesma mudança que altera o que ele descreve.
Arquivo criado ou apagado entra em `log/mapa.md` no mesmo commit: o gate `MAPA_DESATUALIZADO` confere.
O detalhe fica em `log/`: `mapa.md` (arquivo por arquivo), `ferramentas.md` (o que cada uma ensinou), `pendencias.md`
(o que falta). Registros de cada linha: `work-log.md` (porte e gráficos), `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`.
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
- `analysis/`: `suite.py`, `sampler.py`, `causes.py`, `live40.mjs` (o 4.0 aberto, respondendo em milissegundos).
- `assets/`: cozinha de pessoas, marcos, ruído, móveis, postes e perfis de via.
- `converter/`: rastreador e tradutor do 4.0, com git próprio; o conteúdo vem por `git read-tree`.
- `converter-slice/`: fatia antiga do conversor.
- `process/`: ferramentas de processo e registros. `ports/`: planos, coberturas e relatórios de cada porte.
- `oracle-4.0/`: cópia instrumentada do 4.0 para o oráculo (`ORACLE_INSTRUMENT=1`).

**Outros:** `dist/` (builds, capturas e logs, fora do git) e `log/` (registros).

## Fluxo de dados e threads
- `DataTables` lê `data/**` → `CityStore.boot` envia o catálogo ao `CityCore`, que rejeita chave ausente.
- `Simulation` roda em thread própria com tick fixo (`sim_tick_hz`), sob `map_mutex` compartilhado e `traffic_mutex`.
- Comandos, crescimento e relógio tomam `map_mutex` exclusivo.
- `ChangeLog` é o único canal de "sujo", drenado sob trava compartilhada e o mutex do log.
- `RenderWorld.sync` nunca toma o mapa exclusivo. Manda as malhas ao `JobPool`, que leem só o mapa, nunca o tráfego.
- O job de prédio copia o que lê sob trava curta (`HeightPatch`).
- A conversão de malha entra em `render.upload_budget_mb` por quadro; o commit só troca malhas, em `render.commit_budget_ms`.
- Poses por `PoseBuffer`. O save grava em thread própria.

## Luz e relógio
- `Daylight` é o dono do sol, do ambiente e do céu. À noite o sol vira lua sem sombra; a luz da rua é `LampLights`.
- `CityWorld.set_sky_mode("day"|"night"|"cycle")`: dia e noite fixam as horas de `daylight.json` (16h e 22h).
- Testes e capturas movem relógio e céu juntos (`UiDriver.at_hour`).
- Par de foto com o 4.0: mesma hora, mesma câmera e 1920x1080.

## Máquinas de estado
- `GameFlow.State`: BOOT → MENU → PLAYING → QUITTING (tabela `TRANSITIONS`; fechar a janela também passa por `quit`).
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; só `start()/stop()` entram e saem de STOPPED.
- `ToolController.Tool`: uma por ferramenta, de mãos vazias é INSPECT; só `set_tool` troca; segurado `drag`/`tick`, solto `release`.
