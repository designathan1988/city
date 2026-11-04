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
## Próxima ação
- Conferir cada pacote de `ports/<área>/package` e integrar; depois onda 2 (Trânsito, Pedestres e pessoas, Mobiliário) e onda 3.
