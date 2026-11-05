# Relatório do porte build: ESTADO PARCIAL, a funcionalidade não está concluída

Este relatório descreve o que foi feito até aqui. A entrega não está pronta: a maior parte do rastreamento segue `a portar`. Contagem em `coverage.md`.

## O que li
- Inteiros: `session-rules.md`, `CLAUDE.md` do clone, `converter/README.md` e `DESIGN.md`; no 4.0, `CLAUDE.md`, `ARQUITETURA.md`, `MIGRACAO-WEBGPU.md`.
- Do rastreamento, inteiros: `wear.ts`, `glassRoom.ts`, `roomBehind.ts`, `roomLamps.ts` (render), `lightSlots.ts`, `massingMaterial.ts`, `facadeLevel.ts`, `interior.ts`, `spaces.ts`, `partyWalls.ts`.
- Parcial: `kit.ts` (até a linha 200 e o trecho de `setNight`), `feature.md` (cabeçalho, lista de código, início de slices e de recursos, encaixes de 3 arquivos), `boundary.json` (estrutura e declarações próprias). Os outros arquivos de `source/`, cerca de 45 mil linhas, NÃO foram lidos; vi só a lista de declarações.

## Causa dos defeitos registrados
Uma causa só. `Shell::box` no modo instanciado ignorava o `skip`: o anel do caixilho, aberto nas duas faces, virava um cubo sólido com a tinta e o acabamento da guarnição diante de todo vidro. Com acabamento metálico o mapa de normais lia como chapa ondulada (`12_shopping`, `04_rua_baixa`); de noite nenhum cômodo aparecia (`10_noite`). Prova: as fotos de `evidence/fotos/aro` depois da correção.

## Validade das análises de foto até aqui
Pela regra de fotos da coordenadora, nenhuma foto minha desta versão vale como "igual ao 4.0": nenhuma tem par no mesmo enquadramento (posição, alvo, zoom e hora). O jogo e o 4.0 estavam em cidades diferentes, com prédios diferentes, e a `comparacao_40_x_jogo.png` junta vistas parecidas, não pares. As medições de janela abaixo são estatísticas de duas fotos diferentes: comparam distribuição de luminância, não o mesmo prédio. Elas seguem como calibração declarada, não como prova visual.
Serão refeitas na rodada planejada, com o mesmo tipo de prédio, a mesma posição relativa, o mesmo alvo, o mesmo zoom e a mesma hora, lidas em resolução cheia, cada defeito listado e levado à correção.

## Fotos do 4.0
Por ordem do usuário, não usei as fotos de `ports/referencia`. As minhas estão em `ports/build/reference`, renderizadas por ANGLE/Direct3D11 no Chrome (`report.json`), cidade pequena, semente 7, 1.348 prédios, câmera mirada em prédios reais (`evidence/look/gerar_plano.py`).
- `setSkyMode('night')` fixa a luz em 22:00 (`renderer.ts`, `fixedHour`). Por isso as fotos rotuladas 19:30 e 02:00 da primeira rodada são todas de 22:00: estão separadas em `evidence/fotos/luz-fixa-22h`, sem uso. As válidas por hora são `hora*-centro` (modo `cycle`).
- A primeira rodada foi encerrada de fora pela coordenadora; refiz só o que faltou.

## Constantes: método e fonte
O valor do 4.0 não é fonte. Duas fontes físicas:
- **Parte acesa por hora:** BLS, American Time Use Survey, Tabela A-3, 2007-11, linha Sleeping (https://www.bls.gov/tus/tables/a3_0711.pdf). Conferi 13 horas contra a tabela: `awake_by_hour` = 100 % menos o que dorme.
- **Cor das lâmpadas:** corpo negro no sRGB. Os valores de tabela foram conferidos pelo locus de Planck (Kang et al. 2002), no máximo 4 de 255 de diferença em cada canal. A tabela original de M. Charity recusou conexão daqui; a conferência é só a calculada.

Calibração por medição, não por fonte (`evidence/look/medir_vidro.py`, `medir_janelas.py`), com luminância linear Rec.709 nas fotos `*-hora2200-centro`, `*-dia-torre`, `*-noite-torre`, contra as do jogo:
- `window_glow` 0,62 (perto), 0,42 (longe): janela acesa y 0,516 contra 0,522; p95 0,635 contra 0,633; parte acesa 20,6 % contra 17,9 %. Saturação ainda menor: r/b 1,82 contra 2,15.
- `glass_reflect` 0,04: F0 de Schlick do vidro. `glass_sky_floor` 0: sem piso artificial.
- `FAR_ROOM_LEVEL` 0,68: média do nível de `pane_lamp`, conta, não ajuste.
- Sem fonte primária, só herdados como comportamento: `room_daylight` 0,2, `outside_all_night` 0,43, `wheels_per_s`, `feet_per_s`, tamanhos de móveis de `interiors.json`, proporções das plantas. Faltam fonte ou medição.

Parte acesa medida no 4.0 por hora: 19:30 24,7 %, 22:00 17,9 %, 02:00 1,7 %, 05:00 4,4 %, proporcional à tabela do ATUS.

## Commits (branch `convert/build`)
Ver `git log`. Temas: aro do caixilho; vidro por interior mapping e janelas por hora; interiores; calibração noturna; desgaste.

## Resultados
- Gates OK; DLLs release e debug compiladas sem aviso.
- 8 testes passaram nos commits do vidro e da calibração (`evidence/5f5b9c878915`, `evidence/eb1b1932d260`); resultado do commit do desgaste no fim deste arquivo.
- Memória, mapa 0, mesma captura sem tráfego rodando: vídeo 560 → 529 MiB, buffers 340 → 309 MiB antes do desgaste. Com a rodada de tráfego dá 580 e 351, mas não é comparação igual (outra rodada, outra câmera): falta medir igual a igual.
- Desgaste, com controle (mediana sRGB, pista / calçada): sem tráfego 49 / 144; tráfego com taxa zero 49 / 144; taxa real 33 / 140; 40× 33 / 115. A pista satura em cerca de 4 min simulados.

## O que não foi verificado
- Tempos de quadro, GPU, CPU: máquina carregada. Comando: `godot --path project res://tests/perf/camera_pan.tscn`, comparar com o commit a11384a.
- Custo do `pane()` no fragmento (traço do cômodo e 12 laços do caixilho): pode pesar em GPU fraca; não medido.
- Renderizador Compatibility nas capturas do vidro: os testes passam nele, mas não fotografei.
- Janela do desgaste seguindo a câmera em mapa grande (`follow`): não exercitada, só mapa 0.
- Coerência visual dos móveis atrás do vidro: vi cômodos e cortinas de perto, sem conferir móvel a móvel.

## Problemas conhecidos, abertos
- Paredes de noite ~5× mais escuras que no 4.0 (mediana 0,004 contra 0,019). A luz noturna é do `daylight`, tocado pela sessão landscape.
- Janelas distantes se fundem em blocos maiores que no 4.0.
- Vidro de dia com tom ainda não medido contra o alvo y≈0,06.

## Cobertura
`coverage.md`, com prova por símbolo conferida pelo gerador. A versão anterior marcava 2.419 linhas como "já existia" por nome de arquivo, sem verificar; a coordenadora achou linhas falsas (`emitRocks`, `emitRoofDetails`). Foi erro meu e está refeita: sem prova, a linha é `a portar`.

## A remedir na integração
A paisagem mudou a luz noturna em `convert/landscape` (c8fced8 e seguintes: `moon_energy` 0,319, `hemi_night_k` 0,813, cores neutras) e mediu parede 0,0135 contra 0,013 do 4.0. Janela não muda de brilho, mas muda de contraste com a parede. Com os branches juntos, remedir com `evidence/look/medir_janelas.py`. Esse valor do 4.0 difere do meu 0,019 por ser outra medida ou outra foto: ainda não comparei as duas.

Também a remedir: a paisagem mudou a luz global (`convert/landscape` 9cc9ec7: sol 0,89 branco, preenchimento, ambiente azul, gradação de filme depois do ACES). Ela viu paredes ao sol mais pálidas e o creme sem calor, sem saber ainda se é albedo ou falta de `canyonLight` e oclusão. Conferir as paredes de dia e o vidro de dia com os branches juntos, em par com o 4.0.

## Construir: onde está
Plano em `plan.md`. Feito, com teste e sem foto: corpo editável com desfazer e save; Massa, Fachada, Telhado (com equipamentos), Cores e Peças por comando; seleção por raio; interface dessas abas (`building_tool.gd`, `builder_strip.gd`, `data/ui/builder.json`).
`layer.ts` não foi marcado como Porte 8: busquei e ele não tem nada da vista interior, só uma linha sobre oclusão.
Limites da verificação: os botões da gaveta não foram clicados por entrada real; nada foi visto na tela; o `rodar_testes.ps1` deu um falso "0 falhas" sem rodar nada antes de eu corrigi-lo; as rodadas válidas são `evidence/testes-*` a partir de `testes-selecao`.

## Falta
Do Construir: Modelos, Desenhar, aba Interior, modelagem avançada, ferramenta de lotes, bandeiras, fantasma da peça. Depois, na ordem de `plan.md`: símbolos, fotos em par, janelas distantes, medições, constantes sem fonte, `landmarks` (não começou).

## Último estado
Commit 12ff650: gates OK, DLLs release e debug, 8 de 8 testes (`evidence/12ff650d1d37`), sem vazamento nem erro de shader nos logs. `coverage.md`: 139 portado, 128 não se aplica, 37 outro porte, 3.864 a portar. O "sem cintilar" do commit do vidro não foi medido: vi só fotos paradas, nunca a câmera em movimento.

## Massa da cidade e mesmo mapa do 4.0 (ordem visual, seção 11)
Medido pelo corpo desenhado. 4.0: cidade pequena, semente 7, `evidence/massa/40.json`. Jogo: mapa 0, semente 7, `evidence/massa/jogo-depois.json`, teste `city_mass_test`. Cada célula: média de andares / máximo / % com 10 ou mais.

| Faixa do raio | 4.0 | Jogo antes | Jogo depois |
|---|---|---|---|
| 0–15% | 17,9 / 44 / 88% | 7,2 / 22 / 24% | 17,9 / 45 / 85% |
| 15–30% | 15,8 / 42 / 78% | 7,6 / 24 / 27% | 16,6 / 45 / 78% |
| 30–50% | 7,1 / 43 / 19% | 6,1 / 24 / 20% | 7,9 / 41 / 22% |
| 50–75% | 2,4 / 15 / 0,2% | 3,2 / 24 / 7% | 2,5 / 22 / 1,1% |
| 75–100% | 1,6 / 2 / 0% | 1,9 / 8 / 0% | 1,6 / 3 / 0% |

Contagens: 4.0 tem 1.348 prédios; o jogo tinha 3.154 e agora tem 1.369 mais 16 praças. Ruas: no mapa vivo do 4.0 (`window.__roadcraft`, `evidence/massa/plan-vias.json`) são 160 nós e 294 trechos, 1.348 lotes e 1.348 prédios, extensão ±540 m; o jogo tem 304 nós e 438 ruas, a mesma grade com 144 trechos de 120 m partidos em dois, porque corta a cada 100 m.

Causas, cada uma provada:
1. Raio 1100 contra 640. A tabela `RADIUS` do 4.0 tem 420, 640 e 880, mas só dois tamanhos chegam ao jogador: cidade pequena (`DEFAULT_CITY`, 640) e cidade grande (`LARGE_CITY_RADIUS`, 1.600). 1100 não existe no 4.0.
2. Sem `planCity`: zona sorteada por trecho. Portado em `sim/citygen/city_plan.*`.
3. O plano recebia a semente com `FLAT_BIT`. Com a semente limpa, os centros calculados à mão batem com o 4.0.
4. `find_cuts` não aceitava cruzamento em T: o anel externo sumia e a cidade perdia uma quadra de cada lado (nós em ±420 e ±468 em vez de ±540).
5. Registro e corpo sorteavam andares separados, e `record_for` perdia a densidade. Agora a sonda bate com o corpo nos 1.385 registros do mapa 0 e nos 8.076 da cidade grande.
6. O par da loja furava o teto: 4 prédios de 8.076 na cidade grande e 1 de 1.380 em outra semente, achados pelo teste.

Relevo, medido com um mapa temporário (raio 640, sem chão plano; não commitado), semente 7: 438 ruas nos dois, 1.460 lotes contra 1.455, 1.381 prédios contra 1.369, massa igual dentro do ruído. Sementes 11 e 23 no relevo deram 1.449 e 1.457 lotes. Observação: o mapa 0 já é plano por `FLAT_BIT`, então ele não marcha no relevo.

O que ainda difere do 4.0: a zona é medida por margem de rua e não por lote; os lotes são os do jogo e não `planLots`, então o mesmo prédio não cai no mesmo lugar; a cidade grande usa o mesmo plano, mas não comparei com a do 4.0; o jogo ainda gera cívicos, que a cidade do 4.0 não tem (11 na semente 7); as 16 praças não foram comparadas com as do 4.0 por contagem; nada disso foi visto em foto ainda. O commit intermediário `f750c94` foi separado sem compilar sozinho.

`f750c94` compila sozinho: saída 0, 0 erros, medido com a árvore naquele commit.

Instâncias de MultiMesh: as peças de fachada não precisam de cor (`bake_band` escrevia branco e `building_parts.gdshader` lê só `INSTANCE_CUSTOM`). Sem cor, cada instância passa de 80 para 64 B por cópia, nas 3 cópias que o motor guarda. Commit `9985515`; `building_edit_test`, `model_test` e `e2e` passaram. A estática não foi remedida com a máquina quieta.

## Janelas e tom da massa

Ramo de medição: `medir/luz` é `convert/build` com a `convert/landscape` da Porte 3 por cima, só para fotografar; não se entrega. Sem a luz dela, nenhuma cor de prédio se compara. Com ela, p1 passou de 79/114/125/141/211 para 51/79/91/104/184 (4.0: 45/75/88/97/159).

Causas medidas e corrigidas (detalhe em `plan.md`):
- Perto: o vão era buraco de malha, com lados sob céu inteiro. Agora o vidro desenha o vão, por raio contra caixa, com o sol exato e o céu pelo fator de forma de Lambert (Baum, Rushmeier e Winget, 1989). Superfície e shader próprios, `building_glass.gdshader`, com `light()`. São 8 triângulos a menos por janela de vão limpo.
- Longe: escurecer 25 % de toda janela vinha de uma regra errada, e não do `step()`: o 4.0 usa o mesmo `step()` com o mesmo filtro. A parte agora vem do componente. Os quadrados soltos nas torres sumiram (`evidence/recortes/p2-janelas-3x-par3.png`).
- `finish_mean` dividia o vidro por 0,081 e a ardósia por 0,226. Saiu.
- Reflexo do céu contado duas vezes no Forward+: o do motor e o de `glass_reflection`. Agora só o que passa de 1 vez o céu (1,6 claro, 1,3 escuro).
- Nível distante sem o tom do acabamento: agora `finish_far`, a média linear de cada mapa medida texel a texel, vezes 0,91 de intemperismo (valor do 4.0, sem fonte primária; pendente de medir no nosso `weather`).

Fora do meu dono: o 4.0 tem `render/canyonLight.ts`, que escurece o fundo das ruas entre prédios. Pela `boundary.json` é da Porte 3 e não está no clone dela. Explica parte da rua clara em p4.

## Par com o mesmo prédio

A partir daqui `user://` é próprio do clone (`project/override.cfg`, pasta `Roadcraft-build`); medidas anteriores valem com a ressalva do cache compartilhado. Lotes: 4.0 1.348 com 831.930 m²; jogo 1.385 com 863.928 m², logo a cobertura da quadra é a mesma e o miolo verde vem do desenho do quintal. Causas e números do par: `evidence/pair-answer.md`.

## A foto não pode mentir: `render_pending` e os prédios que não vinham

Medido em `evidence/espera1` (antes): na foto vazia, `pendente=0` com `pedir=5 sem_vez=5 orcamento=0 fila=false`, e o chunk sem o nível perto (`chunk=62`). Das hipóteses da coordenadora: (c) é a causa, (a) não (`fila=false`, sem backlog), (b) não (o nível perto era pedido no mesmo quadro, mas não cabia no orçamento).

Quatro causas:
1. `BuildingRenderer::pending()` só somava o que já fora pedido. O que ficava sem vez no orçamento não contava.
2. Árvores e cobertura gastavam o orçamento do quadro antes dos prédios.
3. O nível médio começava em `near_end` mesmo sem o perto pronto: um buraco para o jogador também, não só na foto.
4. A cobertura usava a mesma chave de job do prédio do chunk (`key * 4 + 3`). Um job novo substituía o do prédio na fila, e o prédio ficava pedido para sempre. Apareceu na rodada 6 de `espera2` (`pedidos=1` em todas as fotos, 400 quadros cada): sem as três correções anteriores ela ficava escondida. É bug de correção do jogo.

Prova, `evidence/espera3`, 10 rodadas da captura completa, 47 fotos cada: 0 avisos de foto incompleta, 0 esperas com resto, maior espera 69 quadros. `12_igreja` e `12_escola` nas 10 rodadas: `chunk=63`, prédio presente, diferença média para a rodada 1 de no máximo 0,3 em 255.

O `ranges()` não muda o que o jogador vê com a câmera parada. Antes (`espera1/r1`) e depois (`espera3/r1`), mesma foto: mediana de 0,01 e no máximo 0,44 em 255 nas 46 fotos que já saíam certas; a única diferença grande é `12_escola` (32,88), que antes saía vazia. Depois que o nível perto sobe, o nível médio volta a começar em `near_end`.

## Cívicos que a cidade gera

A cidade não tem conjunto fixo. `civic_for` pega o maior lote de cada célula do mapa com área de pelo menos 500 m², sorteia 40 % pelo hash da célula e escolhe na tabela `growth.json civic.by_zone` pela zona do lote. Com o plano novo a zona de cada célula mudou, e por isso muda a lista: a captura (mapa 0) dá escola, igreja, posto, mercado, bombeiros, quadra e shopping, mais as praças do plano; a semente 7 dá posto 2, escola 2, quadra 2, mercado, parque, clínica, prefeitura e igreja, mais 16 praças. Clínica e parque não sumiram da tabela: saem ou não conforme a semente.

Vi as 8 fotos cívicas na base integrada (`evidence/espera3/civicos.png`): todas com prédio e lote. Igreja com torre, nave e casa paroquial; escola de três pavimentos; bombeiros vermelho com torre; mercado, posto com cobertura e pátio; praça com caminhos e árvores; quadra pintada; shopping. Nenhum vazio, nenhum recorte quebrado.

Diferença do 4.0 que precisa de decisão: `civicFor` não existe mais no código do 4.0, e o mapa vivo da semente 7 tem zero cívicos (só casa, escritório, apartamento, torre, fábrica, galpão, loja, restaurante, farmácia, mercado, hotel, academia, banco, padaria, lanchonete e bar). Lá o jogador é quem constrói os serviços. A regra atual do jogo vem de uma versão antiga do 4.0.

## Fila de jobs com chave tipada

`JobKey { lane, id }` em `core/job_pool.h`, um `JobLane` por usuário. `JobPool::submit` só aceita `JobKey`, então colisão entre renderizadores deixa de compilar. Saiu a aritmética de chave de todos (`key * 4 + n`, `1 << 18`, `1 << 19`, `1 << 20`, `1 << 21`, `BAKE_KEY_BIT`, `-1` da neblina). Gate `JOB_SEM_CHAVE`, sonda `jobs_replaced` (a captura imprime) e teste `job_pool_test`: mesma id em lanes diferentes, as duas rodam; a mesma chave, a nova substitui. Três capturas completas (`evidence/espera4`): 0 fotos incompletas, 0 esperas com resto, `jobs_replaced` 0.

## Guarda-corpo dos recuos

Três causas: a peça do terraço nascia no pé do volume, não no beiral; um volume com bloco em cima não recebia borda nenhuma; e de longe a peça sumia. Agora a borda sai bay a bay onde o telhado acaba, como no 4.0, e a grade é um cartão, com as barras no fragmento (`building_card.gdshader`), nos níveis perto e médio. Par: `evidence/recortes/p3-bege-par9.png`, mesmo prédio e mesma pose, grades nos mesmos recuos. Li o recorte inteiro (4.0 no 1º e 3º painéis, jogo no 2º e 4º). As grades batem. O que resta de diferente: as pilastras têm relevo com sombra no 4.0 e saem chapadas no jogo; a parede na sombra é mais clara no jogo; a sombra diagonal que o vizinho joga na parede no 3º painel não aparece no 4º, causa ainda não medida; o terraço vizinho é marrom-escuro com condensador branco no 4.0 e cinza-claro com caixas marrons no jogo; os vidros do 4.0 são mais escuros.

## Custo do vidro novo

Medido com `tests/perf/camera_pan.tscn`, mapa 0, 1600x900, Forward+, RTX 3060. Comando, com a carga da máquina gravada ao lado (`carga.txt`, `gpu.txt`):

```
powershell -File ports/build/evidence/medir_quadro.ps1 -Dir <pasta> [-Extra "--uncapped --moves=parada,perto"]
```

Três versões: `9d73208` (antes do vidro), `f00f092` (o vidro), `95e3d47` (a ponta). Nas duas primeiras entrou a mesma sonda `building_cost`, sem commit. O movimento `perto` é novo: rente às paredes a 120 m, com fachada enchendo a tela (`evidence/quadro-8f6232d/perto.png`), o pior caso do `pane()`.

Contagens em `perto` (não dependem da carga da máquina):

| | 9d73208 | f00f092 | ponta |
|---|---|---|---|
| paredes do nível perto, triângulos | 2.636 mil | 2.128 mil | 2.087 mil |
| paredes do nível perto, MiB | 313,0 | 252,9 | 248,0 |
| vidros do nível perto | 143,2 mil, 21,3 MiB | igual | igual |
| guarda-corpo de vidro e cartões | 0 | 0 | 73,3 mil, 8,7 MiB |
| peças do nível perto | 51,5 mil, 6,7 MiB | igual | 37,5 mil, 5,1 MiB |
| instâncias | 48.618 | 48.618 | 45.574 |
| todas as superfícies dos prédios, MiB | 445,7 | 385,6 | 387,8 |
| triângulos visíveis, sem limite | 3.172 mil | 2.952 mil | 3.002 mil |
| triângulos de sombra, sem limite | 7.798 mil | 7.200 mil | 5.254 mil |
| draw calls visíveis / sombra | 241 / 530 | 240 / 526 | 262 / 386 |
| vídeo / buffers, MiB | 545 / 293 | 516 / 264 | 593 / 270 |

Memória em `parada` (vídeo / buffers / estática): `9d73208` 406 / 153 / 192; `f00f092` 403 / 150 / 192; `099718e` 403 / 150 / 194; `8a9e90a` (o merge) 479 / 156 / 198; ponta 480 / 157 / 198. O vidro baixou a memória. Os +76 MiB de vídeo entraram com o merge do master (70 deles em texturas); depois do merge meus commits somam +1. Não localizei qual textura.

Tempo sem limite de quadros (`--uncapped`, 2 GHz; GPU média em ms, `parada` / `perto`): `9d73208` 3,8 / 5,4; `f00f092` 3,6 / 5,1; `8a9e90a` 4,2 / sem dado (a versão dele não tem `perto`); ponta 4,3 / 5,4. O vidro saiu 0,2 a 0,3 ms mais barato: as 500 mil paredes a menos pesam mais que o shader. O merge somou 0,6 ms. A ponta é a rodada `livre-8f6232d`, que começou com outro Godot aberto e a GPU a 5 %; as rodadas `livre-95e3d47` e `livre-95e3d47b` foram contaminadas por outras sessões no meio (GPU média 8,2 ms) e estão descartadas. Falta portanto uma rodada limpa da ponta; a diferença de 0,1 ms sobre o merge não está provada.

Com o limite do jogo, máquina quieta (`quadro-8f6232d`): nenhum quadro acima de 33 ms em nenhum movimento; p99 até 18,7 ms; GPU p95 até 7,8 ms e máximo de 11,4 ms em 16,7 ms; pico de 69 % no `nvidia-smi`, que inclui a montagem da cidade. Com o limite a GPU baixa o relógio e os movimentos dão quase todos 6 ms, então esse tempo não compara versões; por isso o `--uncapped`.

Fica na lista: desde o merge, os cartões e os guarda-corpos de vidro somam +39 draw calls visíveis e +84 de sombra em `parada` (245 / 384 contra 284 / 468). O cartão não desenha na sombra, mas a superfície ainda gera a chamada. Mudar o nível perto para a instância sem sombra das peças tira essas chamadas.

## Dois defeitos achados nessa medição

- **Topo do relevo listrado.** `relief_caps` usava a cor da moldura com o acabamento do `trim`; onde ele é metal saía a chapa ondulada, com nervuras a cada 0,2 m. A cabeça do relevo é alvenaria. Prova: `evidence/recortes/perto-faixa2.png` contra `perto-faixa3.png`.
- **Cartão com sombra.** No 4.0 o cartão não projeta sombra (barras abaixo de um texel). Primeiro atribuí a ele as listras, e estava errado: a foto seguinte mostrou que elas continuavam. A mudança fica porque é a regra certa.

## Telhado, função e mobiliário de sacada

**Telhado plano.** Causa: o jogo só olhava a tinta do bloco e caía no sorteio por prédio, ignorando a do prédio. Agora há uma regra só, `flat_roof_of`, usada perto e longe: a do bloco ou do prédio, salvo telha, ardósia ou laje genérica. Par: `evidence/recortes/p4-40-par10.png`. Nos telhados com tinta própria, o tom escuro do 4.0 aparece nos mesmos prédios. Os que caem no sorteio diferem porque o gêmeo não tem o `id` do 4.0: limite do par, não defeito.

**Função do prédio.** O registro só tinha 13 funções e o documento jogava fora a dele. `BuildingFn` ganhou as 25 que faltavam, com nomes num dono só (`api/function_names.h`), e `purpose_of` as leva ao interior. Mapeiam para `PU_*` que já existem, então `floor_finish` não muda. A "sala residencial em escritório" do par não foi reproduzida: por leitura, um documento `office` dá `PU_OFFICE` e piso de pedra; não tenho foto que prove de que prédio era aquela sala.

**Mobiliário de sacada.** Dez peças por instância (`bay_props.json`, `bay_props.*`, `facade_props.cpp`, `building_props.gdshader`), um `MultiMesh` por tipo e chunk, com o sorteio na mesma ordem do 4.0. Sombra só em condensador, vaso, folhagem e arbusto. `decay` vale 0 nos 441 prédios do 4.0 lidos, então passo idade 0; o padrão `popular` e o condensador amarelado existem no código e não aparecem hoje.

Lido em resolução cheia (`evidence/recortes/p7-pecas.png`, `p7-a` a `p7-d`): cadeira, mesa redonda com cadeiras, vasos com folhagem e arbusto em cores diferentes, condensador com ventoinha, grade e suportes. Varal, espreguiçadeira, bicicleta e antena não caíram em nenhuma das quatro vistas: compilam e entram no sorteio, mas **não vi nenhum deles em foto**. As peças não coincidem vão a vão com o 4.0, porque o hash do vão usa o `id` do prédio.

Custo em `perto` (5.026 peças; `parada`, 796): triângulos visíveis 3.122 → 3.636 mil, sombra 5.630 → 6.383 mil, draw calls 262 / 386 → 307 / 424, vídeo 593 → 591 MiB, buffers 270 → 268. Tempos desta rodada não valem: outras sessões compilavam e rodavam jogos.

**Em aberto, achado nas fotos:**
- Fora do miolo o par não casa: a calibração de -11 m do chão só vale no miolo (`p7-b` e `p7-d` saem com outra altura de câmera). Sem par fiel de perto ainda.
- Blocos coloridos e cunhas de pedra nos pátios do gêmeo (`evidence/recortes/lote-blocos.png`): peças de 0,3 a 0,9 m viram colunas. Hipótese, sem medir: `hung` as põe na altura do piso e estende até o ponto mais baixo do lote, e o gêmeo fica sobre o relevo sem nivelar. Não verifiquei se a cidade gerada mostra o mesmo.
- Painel preto opaco na frente de uma sacada em `p7-c`. Causa não investigada.

## Respostas à Porte 8

- Prédio 1604: já corrigido em `8608f7a`, confirmado pela Porte 8.
- Loja com bancos e caixas nos dois andares: causa fechada em `03fef1c`. Sem função, o jogo sorteava o uso pela semente. Com a função do documento, `shop` dá loja no térreo e apartamentos em cima, porque o `purpose` já trata o andar. Não refiz o par da Porte 8 com essa ponta.
- Lote leste claro: vaga sem tinta caía em concreto; agora é asfalto (`#4a4d4f`). Foto antes e depois: `evidence/recortes/vaga-antes-depois.png`.
- Bancos do pátio oeste: não investigado.

