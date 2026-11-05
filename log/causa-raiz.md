# Sessão "Análise e correção de causa-raiz": registro para continuar o trabalho

Atualizado em 2026-11-05. Projeto: `C:\Users\Jonathan\Desktop\city` (main). Worktree de trabalho:
`C:\Users\Jonathan\Desktop\city-worktrees\causas-main` (branch `fix/causas-main`, igual ao main em `fde48c56`).
Worktree de comparação: `C:\Users\Jonathan\Desktop\city-worktrees\base-main` (destacado, para rodar o código antigo lado a lado).

## Objetivo (pedido do usuário)
Achar e corrigir na **causa-raiz** (nunca o efeito) todos os problemas de desempenho e arquitetura: abertura lenta do jogo,
câmera travando ao mover, CPU/GPU/memória, travas, donos duplicados. Sem perder qualidade visual. Ferramenta de análise
**genérica** (qualquer versão, sem caminhos/nomes fixos), rápida e rigorosa. Correções vão para o main. Problemas de outra
sessão: avisar a ela; os desta sessão: corrigir. Desenvolver em worktree, nunca direto no main; nunca dar push.

## Ferramentas entregues (`tools/analysis/`, no main)
- `causes.py` + `detectors.py`: analisador estático de causas por forma de código (travas do worker em leitores, realocação
  por quadro, mapas sem poda, acordadas ociosas, constantes divergentes/sem dono, chamada ao motor em laço, consulta que
  muda estado, polling por quadro, cache com chave incompleta, warnings do compilador com `--build`). `python tools/analysis/causes.py --root .`
- `sampler.py` + `sampler_report.py` + `unwind.py`: amostrador de pilhas para qualquer processo Windows x64.
  Desenrola pelas tabelas `.pdata` do próprio módulo (funciona em DLL mingw), copia a pilha com a thread congelada
  (~0,1 ms congelada), CPU exata por `QueryThreadCycleTime`, thread principal amostrada em toda varredura, linha do tempo.
  Símbolos pedem DLL com informação de depuração: build `RelWithDebInfo` à parte (ex.: pasta de build no scratchpad
  apontando para `native/`; ela grava uma DLL de ~140 MB em `project/bin`: **refazer a DLL normal antes de commitar**).
  Uso: `python tools/analysis/sampler.py --hidden --cmd "\"<godot.exe>\" --path \"<projeto>\project\"" --seconds 30 --window-s 1 --symbolizer <llvm-symbolizer.exe>`
- `suite.py`: suíte inteira em paralelo (~50 s, 25 cenas), mesmos logs de `dist/tests` que o hook confere.
  `python tools/analysis/suite.py --parallel 8 --godot "<godot.exe>"`

## Correções já no main (provadas)
- `b1859fc7`: snapshot sem `traffic_mutex` (TrafficView em buffer triplo; espera 35 ms → 0,2 ms); níveis do tráfego atômicos;
  MultiMesh com folga (`MM_ROOM_STEP`, p99 27 → 17 ms); vazamento de chaves do JobPool; simulação dorme em condition_variable;
  revisões de neblina/cânion (céu só relê quando muda); `CityStore.cached`; clique do inspetor 4,7 → 0,16 ms.
- `c78fdb32`: `render_cost` em tempo constante (a varredura do desgaste, chamada pelo health_log a cada quadro longo,
  realimentava os engasgos da câmera); contagem foi para a consulta `wear_probe`.
- `fde48c56` (abertura da cidade):
  - composição do terreno linear: `ComposeHold` segura a recomposição durante carga/`sync_grade` e compõe cada bloco uma
    vez (`native/src/sim/terrain/compose.cpp`); `nearest` visita só a região de cada peça (faixa, cunha, pontas).
    Alturas finais idênticas bit a bit (5.402 pontos, mapas 0 e 1). Geração do mapa 1: 14,95 s → 3,25 s.
  - `check_city` não gera mais a terra (`found_land(..., generate=false)`): o save não é mais decodificado com terra 2 vezes.
  - lagos planejados no JobPool (`TerrainGen::plan_lakes`), `Heightfield::fill` usa o pool (sem `std::thread` próprio).
  - `Heightfield::water_original`/`set_water`: save e "voltar ao original" não regeneram a água (antes: a cada save, sob a trava).

## Em andamento / a fazer (em ordem)
1. **Medir a abertura de novo** com o sampler no main (`saves/jogo.cidade` abre no início). Antes do último commit:
   carga 3–18 s = terra ~3 s, greide ~2 s (`solve_ground`, `base_height_at`, `water_at`), composição ~3,5 s, lotes ~3 s
   (`block_lots`, `LotStrips`), grafo de faixas ~2 s (`traffic::build_node`). Próximos alvos: greide, lotes e
   `build_node` (são de outras áreas: confirmar com a sessão "Roadcraft 4.0 migration analysis" antes de mexer em
   `road_grade*`); composição pode ir ao JobPool (blocos independentes; `log->terrain_rects` só depois do join).
2. **Câmera travando**: medir no main atual com o sampler movendo a câmera (`tests/perf/camera_pan`) e corrigir a causa.
3. Achados ainda abertos do `causes.py` no main: POLLING_PER_FRAME (4), UNPRUNED_MAP (2), constantes DIVERGENT/SEM DONO,
   ENGINE_CALL_IN_LOOP nos laços quentes, WORKER_LOCK_IN_READER (sondas), INCOMPLETE_CACHE_KEY (1).
4. Todo `CityState` cria um JobPool próprio: o estado de rascunho do `check_city` sobe 24 threads a cada abertura (medir).
5. Branch antigo `fix/root-causes`: portar (refazendo na arquitetura atual, nunca cherry-pick das DLLs) `4002176c`
   (nível distante dos veículos), `07600808` (build sem warnings) e `d9a9e272` (sondas), se ainda fizerem falta.
6. `tools/gates/rules.json` (`exclude_dirs` de tools/converter, oracle-4.0, processo, portes) só o usuário altera.

## Histórico completo (em ordem; cada tarefa nova entra no fim)
1. **Desafio inicial** (pedido do usuário): analisar o app inteiro e corrigir causas, não efeitos, com prova técnica, sem mexer
   direto no app. Criado o worktree `rootcause` (branch `fix/root-causes`, base antiga; depois enviado ao GitHub pela outra sessão).
2. **Rodadas /goal** ("implemente e corrija todas as pendências", processo de 7 passos com registro em
   `.claude/REGISTRO-GOAL.md`): ciclos 1 a 3, o 3 passou.
3. **Analisador estático** `tools/analise` (hoje `tools/analysis`) entregue ao main em `4b795dfc`, sem rodar nele, worktree mantido.
   Avisada a sessão "Roadcraft 4.0 migration analysis" para usá-lo, primeiro num worktree (mensagem reenviada uma vez).
4. **Correção de rumo do usuário**: a ferramenta tinha cenários fixos (nomes/caminhos: `caminhos.py`), o que o usuário recusou.
   Removido; o analisador virou genérico (pastas puladas por padrão, assinaturas de várias linhas, filtro de constantes
   triviais, receptor qualquer) e ganhou o **amostrador em tempo de execução** (`perfil.py`/`perfil_relatorio.py`) e a
   **suíte em paralelo** (`testes.py`): commit `334122b7`. Suíte de ~30 min em série para ~50 s.
5. **Correções C++ no main novo** `b1859fc7`: TrafficView em buffer triplo e níveis atômicos (snapshot sem `traffic_mutex`:
   espera máx. 35 → 0,2 ms); contagem de moradores/empregos no BuildingStore; JobPool sem vazar chaves; simulação dorme
   em condition_variable (contador `sim_wakeups`); MultiMesh com folga (p99 27 → 17 ms); revisões de neblina/cânion e
   leitura só na mudança (`sky_view.gd`); `CityStore.cached`; teste da câmera esperando o estado (era instável sob carga).
6. **Amostrador mostrou**: abertura com ~70 % da thread principal em `Heightfield::apply_modifier`; câmera travando por
   `WearField::worn_cells()` via `render_cost()` (chamado pelo health_log a cada quadro longo: engasgo que gera engasgo).
7. **Nomes em inglês** (pedido da outra sessão): `tools/analise` → `tools/analysis`, `causas.py` → `causes.py`,
   `detectores.py` → `detectors.py`, `perfil.py` → `sampler.py` (não `profile.py`, que sombreia a stdlib),
   `perfil_relatorio.py` → `sampler_report.py`, `testes.py` → `suite.py`. Junto: `render_cost` em tempo constante e consulta
   `wear_probe`; detector ENGINE_CALL_IN_LOOP. Commit `c78fdb32` (rebase sobre a elevação `a37880e2`, DLLs refeitas,
   suíte 25/25), main em fast-forward, outra sessão avisada.
8. **Abertura medida no main com a elevação**: o walk do amostrador se perdia acima do primeiro quadro (DLL mingw sem PDB);
   corrigido (`unwind.py`). A cadeia real: `apply_modifier ← recompose ← put_modifier ← read_terrain ← check_city`.
   O jogo abre `saves/jogo.cidade` (save antigo com corredores de via como modificadores). Causas: decodificação dupla
   (validação + leitura), recomposição quadrática por inserção, `apply_modifier` testando todo segmento em todo ponto,
   corredores inseridos e removidos um a um (`drop_baked_road_ground`), água/lagos numa thread só, save regenerando a água
   sob a trava. Todas corrigidas em `fde48c56` (ver acima). Amostrador também corrigido: tabela de unwind própria, cache,
   pilha copiada com a thread congelada (1,6 → 0,11 ms), CPU por ciclos, thread principal priorizada.
   Provas: alturas iguais bit a bit (5.402 pontos, mapas 0 e 1), mapa 1 de 14,95 s → 3,25 s, suíte 25/25, gates OK.
   Rebase sobre `317ce792` da outra sessão, DLLs refeitas, main em fast-forward, outra sessão avisada.
9. **Registro** (este arquivo) criado a pedido do usuário em `log/causa-raiz.md`; atualizado a cada tarefa e informação nova.
10. **Encerramento da sessão (2026-11-05)**: main em `fde48c56` com todo o trabalho desta sessão; `tools/analysis` sem
    mudanças pendentes; worktree `causas-main` igual ao main e limpo. Mudanças não commitadas em `tools/converter` no main
    são da outra sessão (não tocadas). Do branch antigo `fix/root-causes`, 5 de 8 commits já estão no main (via
    `b1859fc7`); **não portados, a avaliar na próxima sessão**: `4002176c` (nível distante dos veículos; a folga do
    MultiMesh já foi portada), `07600808` (build sem warnings: código morto e descarte explícito) e `d9a9e272`
    (sondas `main_thread_probe` e `pending_probe`: conferir se o main já as tem em `project/tests/perf/`). Este
    arquivo foi commitado no main sozinho (não é código; pode acusar o gate `DOC_UNICO`).

## Como trabalhar (o que funcionou)
- Worktree próprio; build: `MINGW=/c/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64 tools/build/build.sh core-windows`.
- Provar igualdade de resultado: plano do `look.tscn` com centenas de `query height_at` rodado no worktree antigo e no novo
  e comparado número a número (ver commit `fde48c56`).
- Commit: `git add` e `git commit` em comandos separados (hook); logs de `dist/tests` mais novos que o código (rode `suite.py`).
- Rebase com DLLs em conflito: `git checkout --theirs project/bin/*.dll`, refazer as duas DLLs das fontes juntas, suíte, `git add`, continuar.
- Merge no main só `git merge --ff-only fix/causas-main` em `Desktop\city`; não tocar nas mudanças não commitadas de outras sessões.
- Observação: este arquivo fora do CLAUDE.md pode acusar o gate `DOC_UNICO`; foi criado a pedido explícito do usuário.
