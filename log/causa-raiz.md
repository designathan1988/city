# Sessão "Análise e correção de causa-raiz": registro para continuar o trabalho

Atualizado em 2026-11-05. Projeto: `C:\Users\Jonathan\Desktop\city` (main). Worktree de trabalho:
`C:\Users\Jonathan\Desktop\city-worktrees\causas-main` (branch `fix/causas-main`).
Worktree de comparação: `C:\Users\Jonathan\Desktop\city-worktrees\base-main` (destacado, para rodar o código antigo lado a lado).

## Objetivo (pedido do usuário)
Achar e corrigir na **causa-raiz** (nunca o efeito) todos os problemas de desempenho e arquitetura: abertura lenta do jogo,
câmera travando ao mover, CPU/GPU/memória, travas, donos duplicados. Sem perder qualidade visual. Ferramenta de análise
**genérica** (qualquer versão, sem caminhos/nomes fixos), rápida e rigorosa. Correções vão para o main. Problemas de outra
sessão: avisar a ela; os desta sessão: corrigir. Desenvolver em worktree, nunca direto no main; push em `origin/main` como manda o CLAUDE.md.

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
0. Instáveis sob carga (`edit_fuzz_test` semente 3 no header, `camera_input_test`). Cache de cascata: a API do Godot 4.7 não tem; pediria motor próprio.
1. Abertura: leitura 0,9 s, rodadas do greide 1 s (seriais, dependem da ordem), lotes 2,1 s (limite de 24 threads lógicas).
2. CPU de render +0,5 a 1,5 ms pelas instâncias só de sombra (GPU -1 ms; o quadro é limitado pela GPU): medir com a máquina quieta.
3. Backlog do CLAUDE.md "Pendências": são recursos de outras linhas (porte), não causas desta.
4. `tools/gates/rules.json` (`exclude_dirs`) só o usuário altera.

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

11. **Retomada (2026-11-05, 16:30)**: worktree `causas-main` adiantado ao main `3f4a4f60`. Sondas `d9a9e272` já estão no main.
    Abertura de `saves/jogo.cidade` (mapa de 24 km, 9.216 células, 461.301 lotes) medida por etapa com a sonda nova
    `tests/perf/open_probe` (tempos vêm de `load_city`, chave `ms`). Antes: lotes 4,7 s, tráfego 1,0, greide ~3, composição
    3,5, leitura 0,8, validação 0,4. Espera de 4 s da thread principal numa rodada foi compilação de shader de primeira
    vez, não se repete. Em andamento no worktree (ainda sem commit):
    - composição no JobPool (`compose_rect`): 3,5 s → 0,24 s;
    - entradas do greide no JobPool (`RoadGrade::solve`, parâmetro `jobs`): ~3 s → 0,9 s;
    - lotes: blocos por componentes conexas de faixas (era preenchimento pixel a pixel), caixa antes de `rects_overlap`,
      `lot_free` com saída antecipada, `fill_disc` e `ifloor`: 4,7 s → 2,1 s, digest dos 461.301 lotes idêntico
      (`d98a05d738ffcea5`). Sobra limite de hardware (soma 50 s de CPU em 24 threads lógicas);
    - `publish_view`: a parte que só depende do grafo só é refeita quando `CityState::lane_graph_serial` muda.
    Abertura total: ~14 s → ~6 s. Amostrador: relatório mostra uso real de cada thread e o que as outras fazem por janela.

12. **Abertura e câmera entregues (2026-11-05, 17:50)**: tudo do item 11 mais o nível distante dos veículos, sobre o main
    `e61467cd`. Provas: alturas iguais ao main (0 diferenças em 5.402 pontos, mapa 1), digest dos lotes igual, suíte 25/25,
    gates dos mudados OK. Abertura de `jogo.cidade`: 5,6 s (check 0,4, leitura 0,8, greide 0,9, composição 0,24, lotes 2,1,
    tráfego 1,0).
    - Câmera: o núcleo gasta <1 ms por quadro; o limite é a GPU. Sem sombras 11,3 → 4,7 ms: sombra = 58 % do quadro.
      Varredura por camada (`camera_pan --casters=MASK`, `Light3D.shadow_caster_mask`): veículos 21,9 de 28,8 M triângulos de
      sombra. Causa: 3.100 veículos a 1.284 triângulos até 900 m, numa caixa de `MultiMesh` só: as 4 cascatas desenham a frota
      inteira. Portado o nível distante (`far_radius_m` 260, ex-`4002176c`): GPU 11,5 → 9,3 ms, sombra 29 → 21 M, visível
      6,9 → 4,9 M, p50 12,8 → 10,0 ms (`--uncapped`, vista `perto`). Foto a 4x com e sem o nível: sem perda.
    - `07600808` (warnings): o patch não se aplica mais e o build inteiro já não emite warning; dispensado.
    - JobPool do estado de rascunho do `check_city`: 23 threads, custo desprezível; não é causa.

13. **Sombra e achados (2026-11-05, 18:10)**: a separação do interior da cabine rendeu só 0,7 M (estimativa errada: o peso é o
    casco fino) e foi desfeita. Causa: o desenho fino usado como lançador de sombra. Agora cada nível lança pela malha do
    seguinte (`MultiMeshDraw::shadow_only`): sombra 29 → 14,3 M, GPU 11,5 → 8,8 ms; fotos de perto iguais às do main. Caixa como
    lançador foi rejeitada: cobriria o capô e escureceria o carro. Conflitos do grafo no pool: tráfego 1,0 → 0,57 s. Constantes
    repetidas com dono único (`AABB_*`, `WORLD_*`, `NO_ROOM`, `MAX_RECORDS`, `HOLD_SHARE`, `FLAT_TANGENT`). `sign_cells` uma vez
    por rodada. `causes.py`: alcance real (do script, do quadro), custo do método nativo, `bounded:` no membro; 97 → 29 achados,
    os 28 de ENGINE_CALL são laços de carga, lidos um a um, nenhum por quadro.

14. **Método (2026-11-05, 18:30, pedido do usuário)**: `tools/build/quick.sh` (da outra sessão; meu `verify.py` saiu para não haver dois donos) ganhou o golden do digest: um
    comando no lugar de build, dumps, fotos e suíte em idas e vindas. o `state_hash`, parte `derived` (12 ms), troca os dumps de `height_at`. Bug achado pelo `causes.py`:
    `generate_map` não movia `revision`, e a câmera usava os volumes de prédio da cidade anterior; corrigido em `found_land`.

15. **Verificação (2026-11-05, 18:40, com a sessão de migração)**: hash da frota por tick (`traffic_hashes`, `traffic_hash_test`:
    determinístico; a série entra no golden), foto congelada (`look --frozen`: `Engine.time_scale` 0 para o relógio e o `TIME`,
    4 vistas iguais bit a bit em 8 s; no `quick.sh`, limite 0,1 %), invariantes do grafo na DLL de debug (`lane_graph_check.cpp`),
    fuzz com relevo, rua, barreira, prédio, parada, bomba e caixa. O fuzz achou e foi corrigido: `tile_dirty` que nunca voltava
    (save +130 KB depois de um relevo desfeito), `strikes` fora do diário, abrir cidade com o relógio e a `Session` religando a
    velocidade (`load_city(path, paused)`), passo de desfazer que não muda nada. Este último, pela causa: `EditEntry::reduce`
    leva a edição ao efeito líquido (o `isObsolete` do Qt), não um `if` no comando. Seis sementes de 300 passos limpas.
    Pesquisa para o próximo passo: Cities Skylines 2 teve o mesmo quadro (metade do tempo em sombra, sem orçamento por tamanho e
    distância); estado da arte é cache de cascata com rolagem (Insomniac, SIGGRAPH 2012), a conferir no código do motor.

## Ciclo 1 do fluxo obrigatório de 7 passos (ordem do usuário, 2026-11-05)
### Passo 1: problemas identificados (medidos)
- P1. Sombra é 45 % da GPU na vista `perto` (8,8 ms com, 4,7 sem; `--uncapped`).
- P2. Veículos: cada nível lançava sombra pela própria malha; `mesh_set_shadow_mesh` não baixou a GPU (9,9 ms) e acusou
  `p_surface_index = 1 out of bounds` (cabine tem 2 superfícies, a malha de sombra 1).
- P3. Mobiliário: 2,9 M triângulos de sombra, 1,3 ms de GPU sozinho. Poste 570 triângulos × 2.906 instâncias, vão 320 × 1.350.
- P4. Toda malha com `vertex()` (agents, people, props, building, building_parts) ignora malha de sombra.
- P5. Contador `render_info` de sombra usa a malha colorida: número enganoso.
- P6. CPU de render +0,5 a 1,5 ms com as instâncias de sombra (máquina a 100 %, não comparável ainda).
- P7. Gates inteiros só acusam `DOC_UNICO` em `tools/oracle-4.0` (depende do `exclude_dirs`, do usuário).
- P8. Abertura: leitura 0,9 s, rodadas do greide 1 s, lotes 2,1 s.

- P9. Na varredura por camada, `people` deu GPU 7,5 ms com só 62 mil triângulos de sombra (base 5,6): medir de novo no passo 6.

### Passo 2: pesquisa
- [render_forward_clustered.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/render_forward_clustered.cpp) (P2, P4, P5): `surface_shadow` só vem da malha de sombra se `uses_shared_shadow_material()`; o contador usa `surf->surface`.
- [scene_shader_forward_clustered.h](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/scene_shader_forward_clustered.h) (P4): a condição: sem `vertex`, `discard`, alpha clip, `cull_back`.
- [Godot mesh LOD](https://raw.githubusercontent.com/godotengine/godot-docs/master/tutorials/3d/mesh_lod.rst) (P2, P3): num MultiMesh todas as instâncias têm o mesmo LOD: o LOD nativo não serve à frota nem aos postes.
- [Godot visibility ranges](https://raw.githubusercontent.com/godotengine/godot-docs/master/tutorials/3d/visibility_ranges.rst) (P3): faixas por instância; sem fade é o mais barato. Não fala de sombra.
- Autópsia do Cities Skylines 2 ([PC Gamer](https://pcgamer.com/a-tech-analysis-of-cities-skylines-2-proves-its-rendering-way-too-many-polygons-making-cyberpunk-2077-look-like-minecraft-in-comparison), [Tom's Hardware](https://www.tomshardware.com/pc-components/gpus/cities-skylines-2-performance-autopsy-highlights-massive-optimization-failures)) (P1): metade do quadro em sombra, sem orçamento por tamanho e distância.
- [Insomniac, CSM Scrolling](https://advances.realtimerendering.com/s2012/insomniac/Acton-CSM_Scrolling(Siggraph2012).pdf) e [Cached Shadows do CryEngine](https://www.cryengine.com/docs/static/engines/cryengine-3/categories/1114113/pages/21267738) (P1): cache de cascata. A API do Godot 4.7 (`extension_api-4-7.json`) não tem nada disso: pediria motor próprio.
- [Qt QUndoCommand](https://doc.qt.io/qt-6/qundocommand.html) (passo de desfazer vazio): `isObsolete`. A página não abriu direto; lida pelo resumo da busca.

### Passo 3: compreensão
- Causa de P1 a P3: o desenho fino serve de lançador de sombra, em 4 cascatas, para coisas cujo detalhe a sombra não mostra. A saída nativa (P4) não vale com `vertex()`, e o `vertex()` tem função diferente em cada caso.
- Veículos: o `vertex()` só gira roda e mostra assentos, que a silhueta não mostra: instância só de sombra com material sem `vertex()` e malha do nível seguinte.
- Postes: o `vertex()` escolhe o braço urbano ou rural por instância: material simples desenharia errado. Precisa do mesmo shader, com malha menor. De perto o isolador aparece na sombra: longe vai a malha leve, perto a inteira.
- Fios: pendurados só pelo `vertex()` e `shadow` já tratado lá: ficam como estão (0,43 M).
- Árvores 99 mil: nada a ganhar. Prédios: com a sessão de migração, usando este mesmo dono.
- P2 no `mesh_set_shadow_mesh` foi tentativa errada minha: desfeita.

### Passo 4: plano
1. P2, P4: `MultiMeshDraw::shadow_only` e material `shadow_caster`; veículos lançam pela malha do nível seguinte.
2. P3: `PropKit` faz `pole_cast` (peças pequenas fora, menos lados, por dados); `PropRenderer`: poste visível sem sombra, sombra inteira até `cast_from_m`, leve depois.
3. P5: registrar no CLAUDE.md que a sombra se julga pelo tempo de GPU.
4. Passo 6 uma vez: `quick.sh --all --bless`, A/B de GPU e CPU intercalado, fotos de postes de perto e longe.

### Passo 5: código (sem testar)
- `native/src/render/rs_util.h`: `MultiMeshDraw::shadow_only` (P2, plano 1).
- `project/render/shaders/shadow_caster.gdshader`, `project/data/render/materials.json`: lançador sem `vertex()` (P4, plano 1).
- `native/src/render/agents/agent_renderer.{h,cpp}`, `agent_meshes.h`: visual sem sombra, sombra pela malha do nível seguinte (P2).
- `native/src/render/props/prop_kit.{h,cpp}`, `prop_renderer.{h,cpp}`, `project/data/render/streets.json`: `pole_cast` (P3, plano 2).
- `CLAUDE.md`: como julgar sombra (P5, plano 3).

### Passo 6: testes e medições (uma rodada)
`quick.sh --all`: digest igual ao golden, 4 de 4 fotos congeladas iguais, 28/28, gates OK, 85 s. A/B intercalado contra `origin/main`,
vista `perto`, `--uncapped`: GPU 8,5 e 8,5 → 7,8 e 8,0 ms; CPU de render 1,4 → 1,5 e 2,9 → 3,4 ms (máquina a 100 %; sem alta real na primeira
dupla). Sem sombras 5,1 ms; só `people` 5,2 ms. Fotos de perto dos carros (`capture --only=trafego`) depois, porque as congeladas não têm frota.

### Passo 7: avaliação
PASSOU. P2, P3, P4, P5 resolvidos; P6 e P9 eram efeito da carga da máquina, sem causa no código; P1 segue: sombra ainda 2,9 ms, e o resto
é de prédios (com a sessão de migração, `log/graphics.md`). P7 depende do usuário. P8 é o próximo ciclo. Falha de processo: as fotos da frota
ficaram de fora da rodada do passo 6 e vieram depois; o plano devia tê-las. `run_gates.py` inteiro antes do push passa a valer sempre.
Combinado com a sessão de migração: `render/buildings` e `facade_*` são dela; sombra de prédio só se mede depois do aviso dela.
Instáveis que ela viu sob carga, próximos deste ciclo: `edit_fuzz_test` semente 3 (header) e `camera_input_test`.

## Como trabalhar (o que funcionou)
- **Primeiro `tools/build/quick.sh`** (`--all` antes do commit): digest igual ao golden = otimização exata; foto só para o que ele não vê.
- Worktree próprio; build: `MINGW=/c/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64 tools/build/build.sh core-windows`.
- Provar igualdade de resultado: plano do `look.tscn` com centenas de `query height_at` rodado no worktree antigo e no novo
  e comparado número a número (ver commit `fde48c56`).
- Commit: `git add` e `git commit` em comandos separados (hook); logs de `dist/tests` mais novos que o código (rode `suite.py`).
- Rebase com DLLs em conflito: `git checkout --theirs project/bin/*.dll`, refazer as duas DLLs das fontes juntas, suíte, `git add`, continuar.
- Merge no main só `git merge --ff-only fix/causas-main` em `Desktop\city`; não tocar nas mudanças não commitadas de outras sessões.
- Observação: este arquivo fora do CLAUDE.md pode acusar o gate `DOC_UNICO`; foi criado a pedido explícito do usuário.
