# Registro do laboratório F7 (porte do `src/lab` do 4.0 para o jogo)

Linha de trabalho nova, da sessão do conversor (2026-11-05). Worktree `C:\Users\Jonathan\Desktop\city-worktrees\lab`, branch `port/lab`,
`project/override.cfg` com `user://` próprio (`city_lab`). Divisão com a sessão de migração: esta linha toca `native/src/api/lab`,
`project/game/lab` e a interface do F7; nunca `render/buildings`, `sim/buildings/b40`, `render/roads`, `sim/roads`.

Fluxo obrigatório (ordem do usuário): 1 identificar, 2 pesquisar, 3 compreender, 4 planejar, 5 codificar sem testar, 6 testar uma vez,
7 parar e avaliar. Em erro, recomeça no passo 1. Cada rodada fica registrada abaixo.

## Rodada 1: aba Diagnóstico

### Passo 1: problemas identificados (18:10–18:25)

Medidas: `look.tscn`, mapa 0, semente 1, vista `[0, 0, 2.5, 45, 48]`, RTX 3060, janela oculta.

- **P1** O jogo não tem o Diagnóstico do 4.0 (`src/lab/doctor`): veredito GPU/CPU, causas por ms, A/B de um clique com o ruído medido. O F7 só tem chaves, otimizações e números soltos (`effects_panel.gd`).
- **P2** O jogo não sabe onde a GPU gasta: `get_captured_timestamps_*` só traz `vp_begin`/`vp_end` (medido: uma linha só, 8,2 ms).
- **P3** `LookMeasure.passes()` errava a unidade do tempo de GPU por mil (16795 "ms" para 16,8 ms).
- **P4** `look.gd` imprimia os passes com `"%s" % array`: com mais de um passe, erro de formatação e nada impresso.
- **P5** Os estágios do compositor não são o que o nome sugere. Com sombras desligadas: faixa até `PRE_OPAQUE` 1,71 → 1,67 ms; faixa `PRE_OPAQUE` → `POST_OPAQUE` 5,49 → 2,10 ms; GPU total 8,2 → 4,9 ms. A sombra está na faixa do opaco.
- **P6** Duas contas da CPU por quadro: `EffectsPanel._cpu_ms` (núcleo + render + preparo) e `LookMeasure` (só render). A do `look` subestima.
- **P7** O 4.0 estima cada camada por parte dos triângulos, de uma captura de quadro que o Godot não dá em tempo de execução. Copiar daria um chute.
- **P8** Trocar uma chave compila pipelines (91 → 153 com sombras desligadas): medir logo depois mede a compilação.
- **P9** `Settings.switches_changed()` grava `settings.cfg`: um A/B gravaria duas vezes e deixaria o arquivo mudado se fosse interrompido.
- **P10** Limites diferentes de engasgo: 25 ms no `look`, 50 ms no Diagnóstico do 4.0.
- **P11** O carimbo lido na thread principal é de um quadro anterior e pode ser lido duas vezes.
- **P12** O canyonLight desligado cortou a faixa `PRE_TRANSPARENT` 0,71 → 0,34 ms; a oclusão não mexeu em nada, porque o nível médio não a liga (já em Pendências).
- **P13** Nada do tradutor chegou ao jogo. `recorder/analysis.ts` é puro; `doctor/findings.ts` mistura regras com textos em inglês do 4.0.

### Passo 2: fontes consultadas

| Fonte | Liga a | O que disse |
|---|---|---|
| [Compositor](https://docs.godotengine.org/en/stable/tutorials/rendering/compositor.html) | P2 | `CompositorEffect` roda na thread de render, em Forward+ e Mobile |
| [CompositorEffect](https://docs.godotengine.org/en/stable/classes/class_compositoreffect.html) | P2, P5 | os cinco estágios; `PRE_OPAQUE` vem depois do depth prepass |
| [RenderingDevice](https://docs.godotengine.org/en/stable/classes/class_renderingdevice.html) | P2, P3 | assinaturas de `capture_timestamp` e `get_captured_*`; o trecho lido não traz unidades |
| [RenderingServer](https://docs.godotengine.org/en/stable/classes/class_renderingserver.html) | P6 | só `get_frame_setup_time_cpu` no trecho lido, sem descrição |
| [rendering_device_graph.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/rendering_device_graph.cpp) | P2 | o carimbo fica ligado aos comandos de antes e de depois: o grafo não cruza por ele |
| [render_forward_clustered.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/render_forward_clustered.cpp) | P5 | `PRE_OPAQUE` é chamado antes de `_pre_opaque_render`, onde as sombras são desenhadas |
| [rendering_device.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/rendering_device.cpp) | P3, P11 | GPU por `timestamp_query_result_to_time`, CPU por `get_ticks_usec`, `get_captured_timestamps_frame` é o número do quadro |
| [Busca: perfil por script](https://docs.godotengine.org/en/4.3/classes/class_renderingdevice.html) | P2 | nenhuma fonte ensina ligar o perfil de quadro por script |

`extension_api-4-7.json`: nenhum método público liga o perfil de quadro; só `EngineDebugger`. Fonte do 4.0 lida: `doctor/{index,findings,measure}.ts`, `recorder/analysis.ts`. Conferido pelo `call40`: `verdictOf` responde em Node.

A unidade do GPU (nanossegundos) é medida, não documentada: a soma das faixas em /1e6 dá 8,2 ms, igual ao timer da viewport.

### Passo 3: causas

- **P2** Causa: o motor só carimba passes com o perfil de quadro ligado, e isso só o debugger faz. Um jogo exportado nunca teria. Solução de causa: carimbos próprios pelo compositor, presos no grafo pelas dependências, ligados só durante a medida.
- **P5** Causa: a ordem do motor (`PRE_OPAQUE` antes das sombras). Nenhum carimbo separa sombra de opaco. Solução: a faixa se chama sombras + opaco, e a parte da sombra vem do A/B, que mede de verdade, inclusive o custo do shader ao ler a sombra.
- **P7** Causa: não há captura por draw. Solução: o custo de uma camada vem do A/B pela chave dela, medido e com ruído, o que é mais forte que a estimativa do 4.0.
- **P3, P4, P6, P10, P11** Causa comum: a medida vive em teste e o painel refaz parte dela. Solução: um dono, `FrameMeter`, no jogo; o `LookMeasure` herda dele.
- **P8, P9** Causa: o A/B usa o caminho do jogador. Solução: `settled` pelo contador de pipelines (o `programs` do 4.0) e aplicar com `apply_now`, sem gravar.
- **P13** A regra é pura e vai para `native/src/api/lab`; os textos ficam em `strings`. Rodada 1 escrita à mão por ser pequena, com o `call40` de oráculo. `recorder/analysis.ts` entra pelo tradutor na rodada do Gravador.

### Passo 4: plano

1. `FrameMeter` (P3, P4, P6, P10, P11): `project/game/lab/frame_meter.gd`; `LookMeasure` estende; `EffectsPanel` usa a conta de CPU dele.
2. `PassClock` (P2, P5): faixas em `data/game/lab.json`, com a chave que testa cada uma.
3. Regras em C++ (P1, P13): `native/src/api/lab/diagnosis.{h,cpp}` e a área `lab` (`city_lab.h`, `city_core_lab.cpp`).
4. Medida e A/B (P1, P7, P8, P9): `project/game/lab/doctor_run.gd`.
5. Interface (P1): `project/ui/lab_doctor.gd`, terceira aba do F7, com textos em `strings_ui.csv`.
6. Dados: `lab.json` e o schema. Teste: `project/tests/core/lab_test.gd`, com os vereditos do `call40`, a medida real e o A/B das sombras.
7. `CLAUDE.md`: mapa e ferramentas. P12: nada a corrigir aqui.

Depois desta rodada, em ordem: Recursos, Inspector, GPU, Gravador, Testes e Otimizações do laboratório; Zonas › Quarteirões; `sim/transit`; `sim:meso`; pedestres.

### Passo 5: código (18:30–18:50, nenhum teste rodado)

Desvio do plano: o plano previa `lab.json` com schema próprio; uma tabela nova exige mudar `rules.json`, que é do usuário. O `doctor` entrou em `render_switches.json`, a tabela do próprio F7, cujo schema aceita a chave.

| Arquivo | O quê | Liga a |
|---|---|---|
| `native/src/api/lab/diagnosis.{h,cpp}` (novo) | regras sem Godot: veredito, peso, estágios, engasgos, A/B | P1, P10, P13 |
| `native/src/api/lab/city_lab.{h,cpp}` (novo) | área `lab` do núcleo | P1 |
| `native/src/api/city_core.h`, `city_core_areas.cpp`, `register_types.cpp` | a área feita e registrada | P1 |
| `project/game/lab/pass_clock.gd` (novo) | carimbo por estágio pelo compositor | P2 |
| `project/game/lab/frame_meter.gd` (novo) | dono das leituras; cada quadro lido uma vez | P3, P6, P11, P8 |
| `project/game/lab/doctor_run.gd` (novo) | medida e A/B, sem gravar opções | P1, P7, P8, P9 |
| `project/ui/lab_doctor.gd` (novo), `effects_panel.gd` | aba Diagnóstico; a conta de CPU saiu do painel | P1, P6 |
| `project/tests/visual/look_measure.gd`, `look.gd` | herda o `FrameMeter`; passos `clock` e `switch`; impressão dos passes | P3, P4 |
| `project/game/store/store_api.gd` | `lab_diagnose`, `lab_compare` | P1 |
| `project/data/game/render_switches.json`, `strings_ui.csv` | regras, faixas e textos | P1, P5, P10 |
| `project/tests/core/lab_test.{gd,tscn}` (novo), `tools/build/run_tests.sh` | teste nos dois renderizadores | P1, P5 |
| `CLAUDE.md` | mapa e registro | regra do mapa |

### Passo 6: teste, uma vez (18:29–18:34)

`quick.sh --all`: build e scripts OK, gates OK, suíte 27/29. `lab_test` com todas as checagens ok nos dois renderizadores. Medida com a suíte ao lado: GPU 21,8 ms, sombras + opacos 15,8 ms (72%), estágios somam 20,3 de 21,8 ms; A/B das sombras 29,0 → 16,9 ms, ruído 8,1 ms, primitivas 15,0 M → 3,7 M.

### Passo 7: avaliação

- Deu erro. As 2 falhas são do próprio `lab_test`: a linha `FAILURES: 0` casa com `^FAIL` da suíte. Corrigido para `FALHAS:`.
- Achado: `edit_fuzz_test` quebrou (signal 11, em `upgrade_road`, depois de abrir a cidade) sob 8 cenas em paralelo, e a suíte disse ok, porque a saída foi 0. A regra da suíte agora lê `Program crashed`.
- O crash não se repetiu: 3 rodadas sozinho sem falha. A rodada de carga (32 processos da mesma cena) não vale como prova: todos dividem o mesmo arquivo de save e as falhas dela vêm disso. Causa do crash não achada; passada às outras sessões.
- O golden do `digest` foi escrito agora neste worktree, então não comparou nada.

### Rodada 1, fechamento (19:00)

Refeita sobre o `origin/main` com as DLLs refeitas das fontes: `quick.sh --all` com saída 0, suíte inteira, `lab_test` sem falha nos dois renderizadores. A sombra medida pelo A/B sem carga: GPU 10,3 → 5,4 ms, primitivas 12,7 M → 3,7 M. Erro de método registrado: restaurar uma DLL do git sem apagá-la deixa o build achar que nada mudou; o jogo abre sem a área nova e trava no `assert` do store. Antes de medir, apagar as DLLs. O crash do `edit_fuzz_test` foi achado pela sessão de causa-raiz (`j_set_profile`, índice -1).

Próximo no laboratório: Recursos, Inspector, GPU, Gravador, Testes e Otimizações.

### Instabilidade do `lab_test`, fechada

A sessão de migração viu 25,10 de 29,68 ms sob carga. Causa: a soma das médias dos estágios que pesam contra a mediana da GPU. Agora a mesma janela, a mesma estatística e todos os estágios: 31,95 de 31,95 ms com a suíte ao lado.

### Abas GPU, Verificações, Gravador e Testes (2026-11-06)

Lidos do 4.0: `lab/gpu/controller.ts`, `lab/recorder/analysis.ts` (trecho), `lab/resources/diagnostics.ts` (trecho), `lab/tests/index.ts` (trecho), não os arquivos inteiros. GPU: objetos, chamadas e triângulos vistos e de sombra e as dez vistas do motor (`Viewport.debug_draw`); a vista Arame só vale para malhas feitas depois de escolhida. Verificações: `ResourceWatch`, o que sobe lido como reta em 60 s, julgado só depois de 20 s. Gravador: corta a cada `Settings.changed`, deixa 750 ms de fora e grava JSON em `user://lab`. Testes: `DoctorRun.sweep`, o A/B de cada chave em sequência. `lab_test` prova corte, relatório, inclinação, vista e varredura de uma chave; fotos das abas com o jogo aberto.

Achado na foto: as abas ao vivo refaziam o painel inteiro a cada 0,25 s, com as abas e botões junto, e um clique podia cair num botão já trocado. Agora só `_live` é refeito, e nele não há botão.

Não feito: passes da GPU por nome (o motor só os dá ao profiler do depurador; o que existe são os estágios do Diagnóstico), captura de um quadro com lista de draw calls, contagem por camada, A/B de opções na varredura (só chaves), o relatório geral do laboratório e perfil de CPU dentro do jogo (fora dele, `tools/analysis/sampler.py`). `Nós soltos` mostra 32 com o jogo aberto; não verifiquei se são nós guardados de propósito ou vazamento.
- 2026-11-06: aba CPU do F7 (`CpuWatch`): janela de 600 quadros por zona do quadro, com média, p95, pior e fatia. O Consumidores mostra só o último quadro, que não diz o custo de costume. É por zona, não por função: o perfil por função continua no `tools/analysis/sampler.py`, de fora do jogo. Os estágios da GPU por nome já estavam no Diagnóstico.
- 2026-11-06: captura de quadro (`FrameCapture`, aba GPU). O 4.0 grava cada chamada de desenho por um gancho no three.js; o Godot não expõe a lista. O equivalente é por camada: tira cada `RenderLayer` da câmera por 4 quadros, a diferença nas leituras do motor (objetos, chamadas, primitivas, vistas e sombra juntas) é o que a camada desenha. Escreve JSON e PNG em `user://lab`.
