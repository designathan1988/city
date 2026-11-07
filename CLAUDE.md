# Cidade: city builder em Godot 4.7 com núcleo C++ (GDExtension)

Regras, métodos e estrutura do projeto. Atualize este arquivo na mesma mudança que altera o que ele descreve.
Arquivo criado ou apagado entra em `log/mapa.md` no mesmo commit: o gate `MAPA_DESATUALIZADO` confere.
O detalhe fica em `log/`: `mapa.md` (arquivo por arquivo), `ferramentas.md` (o que cada uma ensinou), `pendencias.md`
(o que falta). Registros de cada linha: `work-log.md` (porte e gráficos), `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`,
`arquitetura.md` (o analisador de causas).
Antes de começar, leia o registro da sua linha. Ao fechar o tema, atualize-o com hora e commit, no mesmo commit.

## Metodologia de trabalho (toda tarefa, nesta ordem; trabalho direto)
1. **Identifique:** leia o código envolvido e pergunte "por quê" até chegar à decisão de projeto que produz o sintoma.
   Liste todos os problemas.
2. **Pesquise** quando a tarefa toca API externa (Godot, godot-cpp, Terrain3D): documentação oficial e fontes relevantes,
   nunca só a memória.
3. **Compreenda:** ligue problema, causa e solução pelo código e, quando houve, pela pesquisa.
4. **Planeje** a correção da causa, com dono único. O esforço não entra na decisão, nem para menos nem para mais.
5. **Codifique** sem tentativa e erro: se não deu certo, volte à leitura até entender.
6. **Verifique o que a mudança alcança**, uma vez (tabela em "Execução e verificação"). Fotos: `photodiff.py` diz onde olhar,
   cada mudança é explicada pela causa, sem perda visual.
7. **Pare e confira:** passou ou deu erro? Se deu erro, volte ao passo 1 com o que o erro mostrou.

Ao fechar o tema, o registro da linha recebe os problemas, as fontes, o raciocínio, o plano e os arquivos criados, alterados
e apagados.

## Correção de causa
- A correção muda a decisão de projeto que produz o sintoma. Se a correção certa exige reescrever, reescreva.
- É remendo, e é proibido: mudar tempo-limite, limiar, número de threads ou tolerância; criar caso especial; engolir erro;
  afrouxar uma verificação; regravar a referência (`--bless`) para o teste passar; mover o custo de lugar.
- Toda correção diz, com arquivo e linha: qual é a causa, por que o sintoma não volta por outro caminho e quais outros
  sintomas somem junto.
- Caro não prova causa e barato não prova remendo: o critério é o de cima.

## Princípios
- A causa dos problemas de desempenho é a arquitetura. Sombra, polígono e tempo de GPU são sintomas e mudam com a placa.
- Faça o que foi pedido, inteiro. Nada de maquiar resultado ou de deixar para depois.
- Nunca espere às cegas um processo lançado (tempo-limite e acompanhamento). Pare só processos próprios, pelo PID.
- Nunca feche o jogo aberto na tela do usuário: `tools/build/free_dll.sh` libera a DLL.
- Testes e capturas nunca abrem janela na tela do usuário (`tools/build/hidden_run.ps1`).
- Toda mudança no jogo é entregue, ao fechar o tema, com um executável novo: o usuário abre `dist/windows/Roadcraft.exe`.
- Trabalho direto na `main`, em `C:\Users\Jonathan\Desktop\city`, sem worktree.
  Commit por tema e push em `origin/main` (`https://github.com/designathan1988/city`).

## Execução e verificação
- Investigar é ler código. Sem medições, perfis nem testes longos e pesados para investigar.
- Verifica-se só o que a mudança alcança:

| Mudou | Verifique |
|---|---|
| Ferramenta em `tools/` | os testes dela e `run_gates.py --file` nos arquivos mudados |
| GDScript, cena ou dados | `quick.sh <cenas ligadas>` e os gates dos arquivos mudados |
| C++ | as duas DLLs e `quick.sh <cenas ligadas>` |
| Fechamento do tema, antes do push | `quick.sh --ready`, `run_gates.py` completo e o executável |

- Prova visual, só em mudança visual: do elemento, de perto e de vários ângulos.

## Ferramentas e processos
- Tarefas independentes rodam em paralelo. Cada uma tem tempo-limite próprio; a que trava ou falha é parada e registrada,
  e as outras seguem.
- Nada é refeito sem mudança de entrada: o resultado fica guardado pela chave das suas entradas.
- Cada ferramenta processa só o que a mudança alcança. A execução completa é uma opção explícita.
- Ferramenta de verificação tem de poder reprovar: sai com código diferente de zero na falha, e conjunto vazio reprova.

## Idioma
- Todo texto novo ou alterado no repositório é escrito em inglês: código, nomes de arquivos e pastas, comentários,
  identificadores, dados, registros, relatórios, documentos e mensagens de commit. Escreva em inglês mesmo quando o arquivo,
  o registro ou este documento estiverem em português (a tradução do que já existe está em `log/pendencias.md`).
- Exceções: os textos que o jogador vê (traduções em `project/data/i18n`) e as cópias externas. A conversa com o usuário é
  em português.

## Referência: Roadcraft 4.0
- `C:\Users\Jonathan\Documents\Projects\city\roadcraft` (cópia de 29/10; a `roadcraft-4.0-main`, de 23/10, não vale) é a
  referência de visual e de comportamento de tudo: o jogo deve ter todos os gráficos e funcionalidades dele.
- Pode ser lida, aberta, rodada e estudada a qualquer momento. Comentário do 4.0 não prova nada sem o par.
- Portam-se funcionalidades, nunca código, na arquitetura atual, otimizada. Rastreio: `tools/converter`.
- Exceção: a lógica pura de simulação entra traduzida, sem edição à mão, por uma camada fina, conferida contra o 4.0.
  Regenera-se, nunca se conserta o gerado. Primeira área: fluxo e controle (`flow_control`).
- Em porte e paridade, o par com o 4.0 rodando é a prova que mais acha defeito: `npx vite` em 5300 na pasta dele,
  `node scripts/look.mjs <plano> <saída>` lá e o mesmo plano no `look` daqui (`{"size": "small", "seed": 7}` e
  `{"map": 0, "seed": 7}`).

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
| Verificar uma mudança | `tools/build/quick.sh [cena,cena]` (`--ready` no fechamento do tema, `--all` toda a suíte, `--bless` só com mudança visual entendida) |
| Rodar um teste só | `tools/build/runq.sh <cópia> <pasta> fast test:tests/core/<cena>` |
| Suíte em paralelo | `python tools/analysis/suite.py [--only=nomes]` (logs em `dist/tests`) |
| Gates | `python3 tools/gates/run_gates.py` (`--file X`, `--summary`); só os mudados: `tools/build/gates_changed.sh <cópia>` (defeito em `log/pendencias.md`) |
| Scripts que não compilam | `godot --headless --path project -s <caminho absoluto de tools/build/check_scripts.gd>` (job `scripts` do `runq.sh`) |
| Exportar e empacotar | `tools/build/build.sh export && tools/build/build.sh package` |
| Capturas | `tools/build/build.sh capture /pasta` |
| Olhar o jogo por um plano | `godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<json> --capture-dir=<pasta>` |
| Diferença de fotos | `python tools/build/photodiff.py <antes> <depois> <saída>` |
| Onde cada thread gasta | `python tools/analysis/sampler.py --hidden --cmd "<godot> --path project <cena>" --seconds 30 --symbolizer <llvm-symbolizer>` |
| Causas pela forma do código | `python tools/analysis/causes.py` |
| Causas pela arquitetura | `python tools/analysis/arch.py scan`, `status`, `next`, `explain <id>`, `predict <id>`, `done <id>`, `selfcheck` (`--only x` refaz só o que falhou; fila em `dist/arch/queue.json`) |
| Perfil da DLL de release | `tools/build/pgo.sh` (refazer depois de mudar laços quentes) |
| Tradutor do 4.0 | em `tools/converter`: `node src/cli.ts feature`, `convert`, `differential` (e `graph`, `translate`, `audit`, `verify`) |

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
- `build/`: `build.sh`, `quick.sh`, `runq.sh` e `runq_jobs.sh`, `run_tests.sh`, `pgo.sh`, `photodiff.py`, `free_dll.sh`,
  `hidden_run.ps1`, `check_scripts.gd` (scripts que não compilam), `rename_english.py` (renomeação para o inglês).
- `analysis/`: `suite.py`, `sampler.py` com `sampler_report.py` e `unwind.py`, `causes.py` com `detectors.py`, `live40.mjs`
  (o 4.0 aberto, respondendo em milissegundos), `arch.py` e `arch/` (o analisador de arquitetura), `arch_eval/` (gabaritos
  que só o selfcheck dele lê).
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
