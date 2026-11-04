# Coordenação

## Objetivo e itens
- Qualidade visual e funcional do Roadcraft 4.0 na arquitetura própria. Os 14 itens e seus donos estão em `.claude/skills/roadcraft-engineering/reference/areas.md`.

## Responsável e arquivos
- Divisão por área em `.claude/skills/roadcraft-engineering/scripts/areas.json`, tirada de `regras-sessoes.md` (seções 10 a 12).

## Causas comprovadas
- Os hooks de `fonte/.claude/settings.json` (gates no Edit e no Stop) nunca carregaram nas sessões: todas começam em `C:\Projects\Roadcraft-windows`, e os hooks carregam só da pasta inicial (documentação de permissões do Claude Code). O `hook_regras.py` agora despacha os gates do worktree editado.

## Fontes
- https://code.claude.com/docs/en/skills, /hooks, /permissions, /settings, /worktrees (Claude Code 2.1.310).

## Decisões
- 2026-11-04: skill `roadcraft-engineering` e hook ampliado. A tabela do que é mecânico está em `reference/fiscalizacao.md`.

## Commits
- Nenhum: a instalação não tocou o repositório.

## Validações
- 2026-11-04: `test_fiscalizacao.py`, 120 verificações OK. Prova ao vivo: `echo teste --map=3` negado pelo hook nesta máquina.

## Pendências e bloqueios
- Estado real em 2026-11-04 06:55:
  - o master está em `18dd936`, já enviado (`origin/master` igual), com `.ok`; não estava em `b55fd58` sem push;
  - 8 sessões paradas e Godot fechado.
- `wt-interior-bomba` tem 26 arquivos alterados e 30 novos sem commit, vários de outras áreas: `sim/commands`, `city_state.h`, `city_gen.cpp`, `render_world.*`, `city_world.gd`, a HUD e os gates. É preciso decidir os donos dos trechos.
- `ambientacao-graficos` apagou `models.json` e `models.schema.json`, que são dos lotes, e alterou `data_tables.gd`, que é das vias (B4 do `revisar_branch`).
- `mobiliario-multidao` tem 2 rastros do 4.0 em linhas tocadas: `peds.ts` em `ped_state.h` e `junctions.ts` no `_doc` de `traffic.json` (B3).
- Arquivos sem dono em `areas.json`: `sim/catalog`, `growth`/`maps`/`road_profiles`/`tuning`/`zones.json`, `palette.json`, `noise2d`, `tools/build/*`.
- O verificador agora exige todas as linhas `ok` do `run_tests.sh` (hoje 6, não 4), a medida presente no soak e `trips_arrived` > 0.

- 2026-11-04 08:00, revisão só de leitura com `revisar_branch.py`: B2 herdado é falso positivo, porque `constantes()` compara com a ponta do master e não com o ponto de partida do branch. Contra o `merge-base`: lotes 0, lentes 0, mobiliario 0, pessoas 0, ambientacao 0, onibus 6 (`agent/params/*_roughness`), vegetacao 198 (`streets.json` 192).
- `pedestres-mobiliario` (b8aaa42, b35a88b): mesmos assuntos e horários de fbb4fcf e e974362, que já estão no master; simulação do merge dá 24 conflitos.
- `list_sessions` não mostra as sessões donas dos `wt-*`; há trabalho sem commit em interior-bomba, onibus, mobiliario, vegetacao e lentes.
- `ports/transit` tem 3 commits e 10 arquivos sem commit; fica fora da exclusão até o usuário decidir.

- 2026-11-04 integração `lotes-construcoes`: merge 77bedcc, ajuste 3d9aaa8 (aviso C++20 em `lot_access.cpp`, comentário do teste). 13 commits compilam um a um (`-j 4`).
- `verificar_integracao.py` em 3d9aaa8: 8 de 8 testes, 34 fotos, soak 914 viagens, 0 perdidas, 0 no vermelho. As 34 fotos lidas; 3 comparadas lado a lado com o antes (shopping, rua baixa, noite), as demais pela diferença de pixels contra 18dd936 (0 a 6,5 %).
- Fotos de antes: a pasta da verificação de 18dd936 é bloqueada pelo hook; o antes foi capturado de novo do 18dd936 num worktree temporário.
- Defeitos já existentes em 18dd936, para os portes: janelas com aspecto de chapa ondulada (`12_shopping`, `04_rua_baixa`); torres quase sem janela acesa à noite (`10_noite`); `18_alto` sem as vias na metade distante da cidade; `14_pessoa` com rosto manchado (só visto depois do merge, sem comparar com o antes).
- `.uid` sem commit no fonte: `agent_glass.gdshader.uid`, `building_colour.gdshaderinc.uid`.

- 2026-11-04 integrados e enviados: `lentes-semaforos` (75a4c92) e `ambientacao-graficos` (ce3b2b5, 34 fotos iguais às anteriores, diferença máxima 0,0003 %). `pedestres-mobiliario` não entra: cópia rebaseada do que está no master.
- `mobiliario-multidao` desfeito do master local: soak com `red_entries` 1. Causa: `stops_at_amber` supõe velocidade mantida até a linha; veículo que converte freia e cruza 0,85 s após o fim do amarelo (t=276,10, id 445, ds 133). Correção com a sessão fechar-mobiliario.
- `pessoas-recozinhar`: `people.bin` e `LICENSES.json` do formato 2 estavam sem commit; com a sessão fechar-pessoas.
- `ports/*` apagados; `ports/transit` (3 commits, 10 arquivos sem commit) arquivado em `ports-arquivo/transit-parado-2026-11-04`.
- Confirmações retiradas do hook por outra sessão do usuário (130 testes OK); papel sem barra. Sessões filhas em bypass.

- 2026-11-04 `onibus-caminhoes` integrado e enviado (95b5aad): sessão apagada pelo usuário; fechado pela coordenadora (espaço de cor pelo dono único, merge do master, gate SEGUNDO_DONO). 8 de 8 testes, 44 fotos (10 novas de veículos), soak 914/0/0/0. Lidas 7 fotos: ônibus, caminhão, moto, van, sedã, 07_carro, semáforo à noite; sem regressão.
- Pendente com a máquina quieta: `camera_pan` de lentes e veículos (com sessões compilando, o hook nega).
- 2026-11-04 `vegetacao-terreno` integrado e enviado (d6869e8): 8 de 8 testes, 44 fotos, soak 914/0/0/0. Mudança visual real nas árvores (até 28,8 % dos pixels); 5 fotos lidas.
- Defeitos das árvores novas, para o porte de Paisagem: coração da copa como bloco escuro de perto (`05_transito`); matas distantes parecem manchas achatadas (`11_relevo`, `17_horizonte`); constantes de `trees` sem fonte primária (`registro/vegetacao-terreno.md`); água não conferida no mapa 0, que não tem água.
- 2026-11-04 usuário cancelou fechar-mobiliario e fechar-pessoas. Trabalho preservado, fora do master: `mobiliario-multidao` em 0343a76 (+8; `vehicle_motion.cpp` e `window.cpp` sem commit), `pessoas-recozinhar` em 3c08717 (+8; `people_asset.cpp`, `LICENSES.json`, `people.bin` sem commit). Cozimento encerrado pelo PID 21448.
- Onda 1 lançada do master d6869e8: Vias, Construir, Paisagem, Interface, Demolir.

- 2026-11-04 skill corrigida a pedido do usuário: coordenadora lê o 4.0 e `converter/out`; papel reconhecido em texto colado; regras de jogo, compilação e push olham só o que a linha executa; revisão de constantes contra o `merge-base`; `cd` com `git` negado antes da janela. `test_fiscalizacao.py`: TUDO OK. `mobiliario-multidao` passou de 2 bloqueios para 0 na revisão. `roadcraft-engineering.zip` na raiz segue com a versão antiga.
- 2026-11-04 limpeza dos `wt-*` (pedido do usuário: o que não serve, apaga; nada guardado em pasta). Conferido branch a branch contra o master (`git cherry`, `merge-tree`, diff sem fim de linha):
  - já no master, sem trabalho pendente: lotes-construcoes, onibus-caminhoes, paridade-pedestres, paridade-transito, transito-causas, vegetacao-terreno, veiculos-visual, wt-base (ponta antiga); pedestres-mobiliario era cópia rebaseada de fbb4fcf/e974362.
  - integrados pelo worktree `integracao` (wt-base, branch a partir de 78afb24): `mobiliario-multidao` (8 commits: entreverdes, verde atuado e janela de pedestres pelo MBST; merge limpo), `ambientacao-graficos` (commit novo 4f6956e do trabalho que estava sem commit: `color_space.gdshaderinc` codifica no Compatibility pelo inverso do cúbico de `drivers/gles3/shaders/tonemap_inc.glsl`, conferido na fonte do Godot; guarda de inclusão), `lentes-semaforos` (commit novo 86d72ea: sonda `crossing_kerb` e fotos `20_rebaixo*`; o `capture.gd` tinha o bloco duplicado, corrigido, e passou de 250 linhas, então as fotos do rebaixo foram para `capture_kerb.gd`). Conflito em `city_core_probe.cpp` (mobiliário × lentes) resolvido mantendo `TOUCH_EPS_M` e `crossing_kerb`.
  - `pessoas-recozinhar` (8 commits + 3 arquivos sem commit, commitados em 83d0394 para teste): fora. A verificação `front_faces_out` que a própria sessão escreveu recusa o `people.bin` que ela mesma cozinhou ("faces do arquivo do lado de dentro"), o boot falha e 4 de 8 testes caem. Trabalho incompleto e inconsistente; branch apagado a pedido do usuário. Quem refizer as pessoas parte do master (`cook_people.py` em "Base antes do porte") e do rastreamento do 4.0.
  - `interior-bomba`: 26 arquivos alterados e 30 novos sem commit (bomba: Kingery-Bulmash, cratera, efeitos na GPU, marcas), 65 commits atrás do master, 8 conflitos no `merge-tree`, e pela própria pendência "falta quem reage ao BlastLog". Incompleto; branch apagado a pedido do usuário. A bomba volta como porte pelo conversor.
  - Build Windows: o CMake num host Windows escolhia NMake (inexistente); `build.sh core-windows` agora passa `-G Ninja`. DLLs de release e debug compiladas com `-j 4`, 0 avisos.
  - Testes da integração (desktop oculto, `hidden_run.ps1`): 8 de 8 OK; e2e com `red_entries` 0 e as três regras do amarelo OK.
  - Raiz do projeto virou repositório git (`C:\Projects\Roadcraft-windows`, commit b3c59ce): regras, skill, registro e ferramentas de coordenação com commit local; `fonte/`, `converter/` (repositório próprio), `ports/`, `ports-arquivo/`, `jogo/` e `wt-*` ignorados.
  - Capturas da integração (desktop oculto, 46 fotos, `wt-base/dist/captures` → `fonte/dist/captures`) contra o antes capturado do master 78afb24 (`diff_fotos.py`, pixels com diferença > 8/255): vistas fixas 01–06, 10–13, 17, 18 entre 0,04 % e 3,55 % (só o trânsito muda); as fotos de agente (07, 08, 09, 14, 15, 16, 19_*, 22–26) mudam porque o plano de semáforos do MBST muda qual veículo, travessia ou pessoa é escolhido; `20_rebaixo` e `20_rebaixo_frente` são novas. Lidas: 07_carro, 14_pessoa, 15_travessia, 19_semaforo_dia_0, 19_semaforo_noite_0, 20_rebaixo, 20_rebaixo_frente, 22_onibus, 23_caminhao_lado (antes e depois). Sem regressão de render.
  - Soak 5 min: 907 chegadas, 0 perdidas, 0 por travamento, 0 entradas no vermelho (antes: 914/0/0/0).
  - Defeito da ferramenta de captura (já existia, mostrou-se agora): `14_pessoa` e `23_caminhao_lado` escolhem o agente mais perto do centro e a câmera cai dentro de um prédio (foto só de parede). `capture.gd`/`capture_vehicles.gd` precisam descartar o agente cujo ponto de câmera está dentro de geometria (raycast do núcleo) e pegar o próximo.
  - master avançado por fast-forward para 46257cf (`integracao`); worktree `wt-base` e branch `integracao` apagados. Worktrees em `AppData\Local\Temp\claude\...\scratchpad` (3, de sessões antigas) continuam listados em `git worktree list`: apagar as pastas e rodar `git worktree prune` em `fonte`.
- 2026-11-04 hooks retirados a pedido do usuário: `fonte/.claude/settings.json` sem `hooks` (e os 5 clones de `ports/`), `hook_pre_write.py` e `hook_post_edit.py` apagados; os gates seguem como etapa de teste, rodados pela sessão. Skill `corrigir-na-causa` reduzida ao método: cabeçalho só com `name` e `description`, sem `allowed-tools` (a concessão caía na mensagem seguinte) nem `disallowed-tools` (tirava ferramentas da sessão). A cópia pessoal em `~/.claude/skills` tem precedência e é a única que `fonte/` e os clones enxergam: as duas ficam iguais.
- 2026-11-04 permissões (`Bash`, `PowerShell`, `Edit`, `Write`, `WebFetch`, `WebSearch`) em `fonte/.claude/settings.json` e nos 5 clones, por ordem do usuário; `ferramentas/regras-coordenador.md` apagado. Sessão nova (`--include-hook-events`): 0 eventos de hook em `fonte` e `ports/roads`, 2 no controle com hooks (`ports-arquivo/transit-parado`). `claude -p` lançado de dentro do app responde "Not logged in" (`claude auth status`: `loggedIn` false): filhas por `claude -p` não rodam daí sem login do CLI.
## Próxima ação
- Conferir cada pacote de `ports/<área>/package` e integrar; depois onda 2 (Trânsito, Pedestres e pessoas, Mobiliário) e onda 3.
