# Plano do porte interior: a vista de dentro dos prédios

Escrito antes da primeira linha de código. Vale para `convert/interior` (clone `ports/interior/workspace`, partindo de `convert/build`
em 87ee3c2). Fontes das decisões: `research.md` (esta pasta) e a leitura do 4.0 listada em `report.md`.

## 1. O que o jogador vê e faz no 4.0 (requisitos)
1. **Camada "Interior dos prédios"** no menu de camadas (`shell.ts`: `v2.layer.inside`, dica `inside.toggle`). Ligar abre o prédio no
   meio da vista; se não há prédio ali, avisa (`hint.inside.none`) e não liga. Desligar solta o prédio.
2. **Barra do andar** (só com a camada ligada): botão andar abaixo, rótulo ("Térreo" / "N" andar), botão andar acima. O andar vai de 0
   ao último andar do prédio (`topLevel - 1`), nunca além.
3. **Dois cliques num prédio** o abrem (ou fecham, em terreno aberto); com o interior ligado, um clique em outro prédio o troca. O
   andar visto fica o mesmo, limitado ao topo do novo prédio.
4. **O corte** (`buildingMesh.ts`, `interior.ts: cutOpen`): tudo acima do andar visto some (volumes que começam acima saem; os que
   passam do andar perdem os andares de cima; os telhados desses volumes e o que pende sobre o andar, como relógio ou marquise,
   também). Sobra: o andar visto com **piso** (madeira nas casas e hotéis, porcelanato em algumas, pedra nos demais), **núcleos**
   (poço de concreto com porta, cabine do elevador parada no andar, lance de escada), **paredes internas com vão de porta**
   (planta da Porte 2) e **móveis**, tudo com os materiais do próprio prédio (acabamentos do shader `building`).
5. **Paredes que dão para a câmera** (`cutView`, horizontal, em 8 direções de 45°): no andar visto, as paredes externas e internas
   que ficam entre a câmera e o andar descem a um rodapé (0,9 m; as internas descem quando a face é vista de frente, com mais de
   60 % do vetor da vista); as outras sobem inteiras com janelas e ganham uma face interna de reboco. Girar a câmera 45° refaz o corte.
6. **Luz**: o andar visto é iluminado por até 6 luzes de ponto nas lâmpadas dele (teto, chão, mesa), mais fortes à noite.
7. **Câmera**: ligar a camada enquadra o prédio (a vista fica onde o jogador pôs; o enquadramento é uma ação da barra).
8. Nada disso é salvo: é vista, não cidade. O que Porte 2 (build) faz continua dela: núcleos e móveis postos à mão na aba Interior.

## 2. O que o jogo já tem (lido, não reescrito)
- `sim/buildings/interior/*` (Porte 2): `Interior::at(level)` dá móveis, paredes e espaços de cada andar, uma vez por tipo de andar.
  `floor_finish`, `purpose_of`. Tamanho e "visto de fora" de cada peça vêm de `data/render/interiors.json`.
- `render/buildings/pane_rooms.*` + `building_pane.gdshaderinc`: o cômodo atrás do vidro (interior mapping) para prédios fechados.
- Renderizador de prédios em chunks de 256 m com níveis near/mid/far (`building_renderer.*`), emissores de fachada/telhado/lote que
  escrevem num `Shell`, peças pequenas como caixas instanciadas (`BoxInstance`, 5 faixas de espessura, material `building_parts`).
- `ChangeLog.building_chunks` → `RenderWorld::sync` → `rebuild_chunk`: único canal de "sujo" dos chunks de prédio.
- `drawers.json` popovers.layers: a linha `interiorView` existe sem `action` ("em construção").

## 3. Causa/lacuna e solução em uma frase
O jogo desenha todo prédio fechado: não há como emitir um prédio cortado nem estado que diga qual abrir. Solução: um **corpo cortado**
(`Body` derivado, puro, em `sim`) que os **mesmos emissores** de fachada, telhado e lote desenham, mais um emissor de interior
(piso, núcleos, paredes, móveis) em `render`, ligados por um estado de vista em `CityState` que o job de chunk lê.

## 4. Camadas e donos (core → sim → render → api → Godot)
| Camada | Arquivo (novo ou tocado) | Dono de |
|---|---|---|
| sim | `sim/buildings/interior/cut.h/.cpp` | **dono único do corte**: `cut_open(Body, level, dir)`, `top_level(Body)`; regras do corte |
| sim | `sim/buildings/interior/view.h` (+ campo `CityState::interior`) | estado da vista: alvo (ou raio), andar, direção, revisão |
| sim | `sim/commands/interior_commands.cpp/.h` | `set_interior_view`: valida, marca os chunks (ChangeLog), nada de undo (não é edição da cidade) |
| sim | `sim/buildings/interior/lamps.cpp` | posições (m, lote→mundo) das lâmpadas do andar visto, para as luzes |
| sim/b40 | `types.h`: `Body::cut_level`, `Body::cut_view` (só desenho, nunca guardados) | vocabulário |
| render | `render/buildings/interior_emit.cpp` | **dono único do desenho do interior**: piso, núcleo, paredes internas, móveis |
| render | `render/buildings/interior_cut.cpp` | paredes do andar visto (rodapé × face interna) para `facade_emit` |
| render | `render/buildings/furniture_look.h/.cpp` | modelos dos móveis (lidos de `data/render/furniture.json`) |
| render | `building_bodies.*`, `building_renderer*.cpp`, `facade_emit.cpp`, `roof_emit.cpp`, `lot_emit.cpp`, `facade_bay.cpp` | só **adições**: corpo cortado no cache, vista copiada no `gather`, faixas do chunk cortado, pular telhado/lote/painel |
| api | `city_core_interior.cpp` | `set_interior(...)`, `interior()` (estado, andares, lâmpadas), ligação dos dois |
| dados | `data/render/furniture.json` + schema; `tuning.json`/`buildings.json` (chaves novas) | modelos dos móveis, alturas do corte, luzes |
| Godot | `project/game/interior/interior_controller.gd`, `project/ui/interior_bar.gd`, `project/render/interior_lights.gd` | entrada (liga, cliques, direção), barra, luzes |

Regras de arquitetura cumpridas: estado só em `CityState` (a UI lê o snapshot e envia comando; o andar nunca vive na UI); um
dono por regra (corte em `cut.cpp`, desenho em `interior_emit.cpp`; os emissores de fachada só chamam); sem número mágico (alturas
do corte e das luzes em `data/**`); `render` só lê; nenhum arquivo-deus (cada arquivo novo abaixo do limite de linhas do gate).

## 5. Como o corte funciona (detalhe)
1. **`cut_open`** (sim, puro): cópia do `Body`; volumes com `base > at` saem; os demais ficam com `min(storeys, at - base + 1)`
   andares (`fill` por andar truncado; relevos e uso fora do alcance ficam, inofensivos); `entrance_volume` reindexado;
   `cut_level = at`, `cut_view = (cos, sin)(dir·π/4)`. `at` é limitado a `[0, top-1]`.
2. **Chave do cache**: `BuildingBodies` ganha `cut_body(rec, level, dir)` guardado por (chave do corpo, andar, direção). Sem
   `PaneRooms`: as janelas do corte têm vidro liso (pesquisa §2).
3. **Fachada**: no andar visto (`bay.level == cut_level`) `emit_facades` chama `cut_bay` (arquivo novo): parede da face voltada para
   a câmera (`n·view < 0`) → rodapé de 0,9 m com topo e face interna, sem janela; as demais → face interna de reboco a 0,25 m para
   dentro e a abertura normal. Andares de baixo: fachada normal.
4. **Telhado e lote**: `emit_roofs` pula volumes que chegam ao andar visto + 1 (e o equipamento de cobertura deles); `emit_lot` pula
   elementos acima do andar visto; o módulo de máquinas dos núcleos não é emitido.
5. **Interior**: `emit_interior(shell, parts, pl)`: (a) piso do andar visto por volume (triangulação por orelhas do contorno), no
   acabamento de `floor_finish`; (b) cada núcleo: poço com porta, cabine parada no andar, lance de escada (espelho 0,18 m, piso
   0,28 m, largura 1,2 m, `research.md §3`); (c) paredes internas da planta: caixas de 0,12 m, altura `min(pé-direito − 0,4, 2,7)`
   (2,7 m = pé-direito de projeto usual; veja constantes), rebaixadas a 0,9 m se a face olha para a câmera; (d) móveis: cada peça
   do modelo de `furniture.json` como `BoxInstance` (caixas) no shell de peças, e cilindros/esferas como geometria fina no shell
   próximo; luminárias de teto penduradas.
6. **Estado → chunk**: `set_interior_view` (comando, trava exclusiva) escreve `CityState::interior`, marca no `ChangeLog` o chunk do
   alvo antigo e o do novo; o `gather` do job copia a vista (POD pequeno) e os ids; `build_cpu` troca o corpo dos registros cortados.
   A mesma rotina serve ao modo "raio" (todos os prédios a até R de um ponto, limitado por `interior.max_open`; R < 0 e `max_open`
   alto só para a sonda do pior caso).
7. **Faixas do chunk cortado** (`ranges`): o nível near fica sem limite de distância; o mid, escondido; o chunk conta como detalhado
   para o mesmo, então a malha distante o dobra (sem prédio duas vezes, sem buraco).
8. **Luz**: `interior().lamps` devolve até `interior.room_lights` posições; `interior_lights.gd` move as OmniLight3D (sem sombra,
   `omni_range` curto) e escala a energia por `night`. Cor 3000 K (a mistura de lâmpadas do vidro, já portada).
9. **Câmera**: o controlador chama `CityCamera.jump(...)` ao ligar para enquadrar o prédio (distância pelo tamanho dele, inclinação
   de vista de cima) e lê o `yaw` a cada quadro para a direção em 8 passos; só envia comando quando o passo muda.

## 6. Custo (instâncias, triângulos, memória, trabalho por tick)
**Trabalho por tick da simulação: zero.** A vista é estado de renderização; o comando roda uma vez por mudança.
**Um prédio aberto (caso do jogador):** um chunk (256 m) é refeito por troca de andar/direção/alvo, como qualquer edição de prédio
(prioridade alta: `priority` baixo no `submit`). O trabalho extra do corte é o interior de **um** andar de **um** prédio:
estimativa, a confirmar por contagem (`interior_probe`): piso ≤ 20 triângulos; núcleo ≈ 12 caixas; paredes ≈ 5–40 caixas; móveis
≈ 15–60 peças × 4–10 caixas = 60–600 instâncias de 64 bytes (≤ 40 KB), 12 triângulos de GPU cada.
**Cidade inteira aberta (pior caso):** [corrigido na §11: o mapa 0 semente 7 tem 3.154 prédios; 1.348 é a cidade pequena do 4.0] o mapa 0 tem cerca de 1.348 prédios: ≈ 1.348 × (150–800 instâncias) = 0,2–1,1 M instâncias
(13–70 MB de CPU, 2–13 M triângulos de GPU nas faixas de peças). Não é alcançável pelo jogador (um prédio por vez); no código o
modo de raio é limitado por `interior.max_open` e a sonda mede o custo real com o limite levantado, para que o relatório diga o
número medido e não o estimado. Os níveis de peças já somem por tamanho na tela (faixas por espessura), o que corta a maior parte
dos triângulos longe da câmera.
**Memória:** corpos cortados no cache da thread do job (um por prédio×andar×direção; vira lixo ao sair do cache, `CACHE_MAX`).

## 7. Dados e constantes (dono, fonte)
- `furniture.json`: modelos por peça (proporções do tamanho de `interiors.json`; paleta com `finish` do prédio e cor). Fonte:
  decisão de modelagem comparada às fotos do 4.0 (`report.md`), sem fonte normativa — registrado como tal.
- Escada: espelho 0,18 m, piso 0,28 m (NBR 9050 6.8.2; Goiás NT 11 5.7.3.1); largura 1,2 m (Goiás NT 11 5.4.2.1).
- Porta de parede interna: vão ≥ 0,80 m (NBR 9050 6.11.2.4); o gerador de plantas usa 1,0 m (Porte 2).
- Rodapé do corte 0,9 m; parede inteira até 2,7 m; limite de "de frente" 0,6: **comportamento do 4.0**, medido nas fotos do 4.0
  (não normativo); o valor do 4.0 só vira dado depois de medido no relatório (altura do rodapé contra a porta de 2,1 m na foto).
- Luzes: 6 pontos, energia calibrada por medição nas fotos (não há fonte aberta de lux para os cômodos; `research.md §4`).

## 8. Prova (sem tentativa e erro)
1. **Gates** (`python3 tools/gates/run_gates.py`), DLLs release e debug, os 8 testes existentes.
2. **Teste novo** `project/tests/core/interior_test.gd`: abre um prédio de cada tipo pelo store e confere por sonda: o andar é
   limitado ao topo; `render_cost`/sonda mostram o chunk refeito; número de caixas e triângulos do interior > 0 e < teto; o
   `Body` gerado não muda (bit a bit) com a vista desligada; desligar devolve a contagem original. Contagens, não tempo.
3. **Fotos** (lista escrita antes, 2 rodadas no máximo, 4.0 e jogo na mesma câmera, cidade pequena semente 7): (1) casa aberta no
   térreo; (2) torre aberta num andar do meio, câmera a 45°; (3) loja ou escritório aberto; (4) o mesmo prédio girado 90°; (5) a
   camada ligada na cidade (vista aberta); (6) de noite, com as luzes. Cada foto analisada uma a uma contra o 4.0.
4. **Pior caso**: a sonda abre N prédios (raio) e imprime instâncias, triângulos e bytes; vira tabela no relatório.

## 9. Fora do meu dono (avisado)
- Edição de núcleos e móveis à mão (aba Interior do Construir) e `lift`/meio-nível por volume (modelo da Porte 2): não os faço. O
  desenho do meio-nível (`levelFlights`) fica registrado como "outro porte" em `coverage.md` até o modelo ter `lift`.
- `hud_drawer.gd`/HUD: só uma linha de dados em `drawers.json` e dois arquivos novos de interface (a Porte 4 reescreveu o HUD).
- Vidro/cômodo de fora (interior mapping) é da Porte 2; só desligo o cômodo no corpo cortado.

## 10. Ordem dos commits (cada um compila)
1. `cut_open` + `Body::cut_*` + teste de unidade pelo store. 2. Estado + comando + marcação + API `set_interior`/`interior`.
3. Corpo cortado no renderizador: fachada, telhado, lote, painéis (prédio cortado sem interior ainda). 4. Piso, núcleos, paredes.
5. Móveis (`furniture.json`, schema, modelos). 6. Faixas do chunk cortado + luzes. 7. Interface: camada, barra, cliques, câmera.
8. Teste, sonda, fotos, `CLAUDE.md`, relatório.

## 11. Revisão depois da implementação (o que o código fez de diferente do plano acima)
- **Escada de núcleo e `levelFlights`:** não desenhadas. `Core` do modelo da Porte 2 não tem tipo nem `lift`; a escada que se vê nas
  casas é o móvel `FK_STAIRWELL` que a planta da Porte 2 já põe. As constantes de degrau da §7 (NBR 9050 6.8.2, Goiás NT 11) ficam só
  como pesquisa: nenhuma entrou em código.
- **Parede baixa de 0,9 m, parede inteira de 2,7 m e limite 0,6:** vêm do código do 4.0 (`cutOpen` e o emissor do corte), não de
  medição em foto minha; as fotos do 4.0 desta sessão só confirmam a parede baixa de forma qualitativa. Não têm fonte normativa.
- **Modo "raio" do comando** (`set_interior` com `target < 0`): existe e é testado (79 prédios a 150 m = 21 MB), mas a interface não
  o liga: o interruptor abre o prédio no meio da vista, como o `v2.layer.inside` do 4.0.
- **Janelas vistas por dentro** (não previsto): o reboco interno contornava nada e cobria o vão inteiro; agora contorna a abertura e
  fecha o vão por dentro (`emit_reveal`, dono único da forma). Custo: a casca da casa 0 passou de 286 para 368 triângulos.
- **Telhado de volume baixo que o volume alto encerra** (não previsto): recortado no plano das paredes (`roof_clip`), só no corte.
- **Câmera** (não previsto no detalhe): o ponto de vista gira em torno de um ponto do chão, então o andar alto sai do quadro; o
  núcleo diz `focus`, `extent` e `floor_height` e o controlador recua o ponto por altura / tan(inclinação). O salto é instantâneo.
- **Leitura do núcleo:** `interior` (corpo, andares, lâmpadas) é pesada; o controlador só a relê quando `interior_revision` muda.
- **Fotos:** a §8 pedia o mesmo prédio nos dois jogos. Conferido por dados que não existe: a cidade pequena semente 7 do 4.0 tem 1.348
  prédios, o mapa 0 semente 7 do jogo tem 3.154, e os ids apontam para prédios diferentes (`evidence/photos-list.md`). Nenhuma foto do
  jogo tem par do 4.0; a análise de cada uma diz isso.
- **Número que eu tinha errado:** a §5 estimava a cidade inteira aberta com 1.348 prédios, que é a cidade pequena do 4.0. O mapa 0 do jogo
  (semente 7) tem 3.154: a média medida é 21 MB / 79 prédios = 0,27 MB por prédio, logo ~840 MB para a cidade toda, mais ainda a favor do
  teto `interior.max_radius_m`.
- **Luzes de cômodo:** contagem (6), alcance (7 m) e energias (0,8 de dia, 3 à noite) são valores de partida, **não calibrados** (a §7 dizia
  "calibrada por medição nas fotos": isso não foi feito). Sem fonte aberta de lux; a energia da luz do Godot não é em lux. A cor é a de
  `LAMP_3000` de `building_lamps.gdshaderinc` (Porte 2), normalizada: (1,0 · 0,456 · 0,147) linear = `#ffb46b`.

## 12. Revisão depois da integração com o master (o que o 4.0 mostrou que eu tinha inventado)
Lido no 4.0 (`buildingsWiring.ts` e `renderer.ts`), com arquivo e linha:
- **Câmera:** o 4.0 não a move ao ligar a camada, ao subir ou descer andar nem ao clicar (`insideClick`, `openInside`, `seeInside`: só marcam o alvo e
  pedem quadro; a câmera segue com o jogador, e `focusBuilding` é do botão "enquadrar" do Construir, não da vista de dentro). Saíram do jogo o salto
  ao abrir (`rig.jump`), `frame_floor` e a seção `camera` de `interior.json`. O defeito "andar alto fora do quadro" da rodada anterior era do meu
  enquadramento, não existe sem ele.
- **Modo raio:** a vista de dentro do 4.0 monta `{level, x, y, radius: 0, only: id}` (`buildingsWiring.ts:1139`): um prédio, nunca um raio. Saíram o
  modo raio do comando, `interior.max_radius_m` e o teste `_around`. As medidas de custo do modo raio (21 MB a 150 m) ficam no relatório como
  contas, não como limite de projeto.
- **Luzes de cômodo:** são 6, cor `0xffd6a0`, alcance 8 m, decaimento 2, sem sombra, postas nas primeiras lâmpadas do andar na ordem em que o
  mobiliário as lista (`renderer.ts:143`, `1624-1626`; `roomLamps.ts:20-45`), com intensidade `55 · TUNED_UNIT² · (0,25 + 0,75 · noite)`
  (`renderer.ts:3713`; `TUNED_UNIT = 0,4 m`, `units.ts:12`). No Godot: `omni_attenuation = 2` (inverso do quadrado, doc do `OmniLight3D`) e energia
  `55 · 0,4² / π = 2,80` (o three.js divide o difuso de Lambert por π, o Godot não). Conta derivada, conferida na foto da noite do par.
- **Números do relatório anterior que caem:** "energia 0,8 de dia e 3 à noite", "alcance 7 m", "cor `#ffb46b`" e "câmera 55°, 1,8 por metro, 22 m"
  eram minhas e saem; o `interior.json` agora cita o 4.0 em cada valor.
- **Andares do registro e do corpo:** o 21 contra 12 do prédio 1604 vinha de `record_for` sortear andares numa tabela antiga enquanto o corpo sorteava
  de novo (`growth.cpp`, antes do master `8608f7a`); a Porte 2 fez o corpo o único dono, e `interior_test` agora confere registro e corpo.
