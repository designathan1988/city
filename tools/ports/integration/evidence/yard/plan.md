# Planejador de pátios: plano das correções

## O que foi medido (yard_test, mapa 0, semente 1; `evidence/yard/antes`, `medida`, `passo`)

- O plano de um pátio custa de 0,25 a 4,7 s de CPU (`plan_ms` soma o tempo de cada tick), quase todo na expansão de poses:
  campo 4–9 ms, docas < 0,5 ms, heurística holonômica 0,4–7 ms, expansão 250–4.700 ms.
- Por pose expandida: 10 arcos testados (0,19 µs cada, 1,9 µs), 0,91 heurística de Reeds–Shepp (1,28 µs, 1,2 µs), 0,25 tiro
  analítico (2,43 µs, 0,6 µs). A avaliação preguiçosa da heurística (Tolpin et al. 2013) quase nada ganharia: já é < 1 por pose.
- O número de poses é o que pesa: buscas grossas que esgotam sem achar (o pátio 20: 3 × 80.000 poses, 754 ms, nenhuma achada),
  buscas de saída grossas que param em 74 poses (o ponto de partida encaixotado pela grade de 0,5 m e o passo de 0,8 m), e buscas
  finas na grade inteira de 100.000 a 1.226.865 poses (o pátio 20: 3,9 s dos 4,7) que acham o caminho que a grossa perde.
- O pior tick: 4,3–7,3 ms, para um orçamento de 4.000 unidades. A unidade custa de 0 µs (criar o `ClearanceFill` de 1,5 milhão
  de células: 1,82 ms cobrados 0) a ~1 µs (um passo de busca: 4.000 unidades em 3,0–4,3 ms). O 4.0: 150 poses por tick, "under
  a millisecond" (`lorryDocks.ts` PLAN_BUDGET).

## Fontes

- Dolgov, Thrun, Montemerlo, Diebel, "Practical Search Techniques in Path Planning for Autonomous Driving", STAIR 2008 (lido
  inteiro): a heurística holonômica é programação dinâmica 2D a partir do objetivo (uma por objetivo); a não holonômica,
  pré-calculada offline numa tabela (160 × 160 de 1 m, 5°) e transladada e girada para o objetivo; o tiro de Reeds–Shepp em 1 de
  cada N nós, N menor perto do objetivo; grade de 0,5 m e 5°.
- González-Sieira, Mucientes, Bugarín, "Graduated Fidelity Lattices for Motion Planning under Uncertainty", 2019 (arXiv
  1905.13531, lido inteiro), sobre Pivtoraiko e Kelly, IROS 2008: alta fidelidade só onde é preciso manobrar (perto dos
  obstáculos) e perto do objetivo; de cada grupo de movimentos (mesmo início e fim de rumo), o mais longo que não colide, se as
  células das duas pontas comportam o passo (Alg. 3, `sa + sb > ‖xa − xb‖`). 87–90 % menos iterações e tempo que a fidelidade
  alta uniforme; custo do caminho +9,7 %.
- O 4.0 (`hybridAStar.ts`, `lorryDocks.ts`): os mapas de distância guardados por objetivo e meia largura ("DO NOT work out the
  whole yard's again for each search to the same goal"); só a busca grossa; 150 poses por tick.

## Correção 1: uma busca de fidelidade graduada no lugar de grossa + grossa + fina

A causa das buscas que esgotam e das finas de um milhão de poses é a mesma: uma resolução só para o pátio inteiro. A busca passa a
ter dois grãos, o de hoje (0,5 m, 72 rumos, passo 0,8 m) e o fino (0,25 m, 144 rumos, passo 0,4 m), escolhidos pose a pose:

- **Região de uma pose:** fina quando a folga do corpo nela (a menor distância dos discos do corpo ao sólido, menos o raio deles)
  é menor que o erro que a célula grossa admite, ou quando ela está a menos de `SHOT_NEAR` do objetivo (Dolgov: aproximação do
  objetivo; González-Sieira: alta fidelidade perto do objetivo). O erro de uma célula: duas poses fundidas nela distam até a sua
  diagonal e até uma faixa de rumo, e a ponta do corpo mais longe do eixo anda `2·R·sin(π/rumos)` com ela; o erro é
  `cell·√2 + 2·R·sin(π/rumos)` (R: a maior distância do eixo traseiro a uma ponta, `length/2 + rear`), 1,22 m para o caminhão no
  grão grosso; com folga acima disso, toda pose fundida com aquela está livre. Deriva das constantes
  do grão e do corpo: nenhum número novo.
- **Movimentos:** para cada sentido e curvatura, o passo grosso quando as duas pontas estão na região grossa e o arco não colide;
  senão o passo fino, se não colide (o Alg. 3: o mais longo que cabe, as células das pontas comportando o passo).

**Medido e descartado (`evidence/yard/graduada`):** a região pela folga do corpo (1,22 m) e o raio fino de 8 m em volta do objetivo
deixaram fino quase todo o pátio (o caminhão passa quase sempre a menos de 1,22 m de algo): os pátios simples ficaram de 3 a 5 vezes
mais caros (prédio 33: 513 → 2.234 ms; 35: 428 → 2.226 ms). A condição suficiente era rígida demais e não é a do artigo. **A regra
do Alg. 3 é a que vale:** de cada sentido e curvatura, o passo grosso se não colide, senão o fino; a pose fica na tabela do grão do
passo que a alcançou. O fino entra só onde o grosso bate (o caso medido: a saída encaixotada em 74 poses pelos arcos de 0,8 m).
- **Duplicatas:** cada pose cai na tabela do grão da sua região (lugar e rumo daquele grão); o passo de cada grão é maior que a
  diagonal da sua célula, então um passo sempre sai da célula (a regra de hoje).
- **Heurística holonômica:** no grão fino (a mais fina das duas), uma por objetivo e guardada no planejador do pátio enquanto o
  campo existe, como no 4.0 e em Dolgov; as buscas de todos os portões a uma doca e de todas as docas a um portão a reusam.
- **O pátio:** entrada e saída por portão, como hoje; a terceira busca (fina na grade inteira) sai, porque a fina já está onde é
  preciso. Um só limite de poses por busca: o de hoje para a fina (1.500.000), que só o pior caso alcança.

Critérios: no mesmo mapa e semente, docas com caminho ≥ as de hoje (14 de 14 no primeiro lugar; 52 de 56 no segundo), nenhuma
doca `YARD_NO_STOP`, `docks_reached` não pior; poses expandidas e `total_ms` por pátio menores; as trajetórias desenhadas
(captura do pátio com o caminhão na doca) sem roçar parede.

### O que ficou, medido na bancada (`yard_bench`: uma cidade salva, 78 pátios, 245 docas, os dois planejadores na mesma entrada)

O `yard_test` não serve para comparar versões: o plano anda pelos ticks, então a cidade que cresce enquanto isso, e com ela os
pátios que o teste encontra, mudam com a velocidade do planejador (as três primeiras comparações eram de pátios diferentes). A
bancada fixa a entrada; a métrica são as contagens (poses, docas), porque outra sessão mede desempenho nesta máquina (regras §5).

| versão | docas com caminho | poses | observação |
|---|---|---|---|
| A, a de antes (grossa, grossa, fina na grade inteira) | 228 | 30.733.963 | 68 % das poses nas finas, 46 dos 78 pátios |
| graduada no lugar das três buscas | 227 | 29.406.036 | −50 % nos pátios apertados, 2,5–3× nos fáceis; perde 1 doca (1398) |
| grossa, grossa, graduada, fina (mapa fino para todas) | 228 | 23.080.479 | a grossa com outro mapa perde caminhos (189: 2 → 1) |
| **grossa, grossa, graduada, fina, cada uma com o mapa da sua grade** | **228** | **23.686.312** | a grossa idêntica à de A em 77 de 78 pátios; nenhum pátio perde doca |

A graduada achou 62 das 77 vezes em que correu; a fina de último recurso correu 16 vezes e achou 2 (1395 e 1398). A etapa fina:
20.955.680 poses em A, 14.010.149 agora. O que sobra a mais em 12 pátios são as docas sem caminho nenhum (13): a graduada esgota
(~96 mil) antes da fina (~180 mil); não há prova barata de que não existe caminho (elas falham pelo raio de giro, não pela
geometria). Os mapas de distância: 2.982.976 células em A, 2.167.117 agora (um por objetivo e grão).

## Correção 2: a unidade de trabalho com o custo de cada operação

O orçamento continua em unidades (o mesmo mapa dá o mesmo plano no mesmo tick), mas cada operação passa a ser cobrada pelo que
custa, medido: criar o campo de folga e uma busca pelas células que aloca, uma célula da holonômica, um arco, uma pose, um tiro,
um teste de corpo célula a célula. As razões entre elas vêm da medida (o `SearchCost` e o `YardCost` ficam como sondas), e o
orçamento por tick, de um alvo de tempo: 1 ms, o do 4.0. Medido depois da correção 1, porque ela muda o que cada passo faz.

Critério: o pior tick do planejamento ≤ 1 ms na medida do yard_test.

O que a leitura mostra (o orçamento conta em células: `CELLS_PER_WORK` = 64 por unidade, `way_search.h`):

| operação | medido | cobrado |
|---|---|---|
| criar o `ClearanceFill` (1,5 milhão de células: `sq_` e `clear`) | 1,82 ms | 0 |
| preencher um anel, uma linha da rua | 0,04 µs por unidade | células / 64 |
| uma linha da transformada de distância | ~0,29 µs por unidade (7× a de preencher) | 1 + n / 64 |
| uma célula da holonômica | 0,086 µs | 1 |
| uma pose expandida | ~3 µs | 8 |
| um tiro de Reeds–Shepp | 2,4 µs | 8 |
| criar uma busca (tabelas de poses, mapa), `settled`, `join_ways`, `pre_dock` | a medir | 0, 64 ou 1 |

O plano: (1) nenhuma operação maior que uma fatia: a criação do `ClearanceFill` vai para o `advance`, linha a linha; (2) cada
operação cobrada pelo seu custo medido na unidade da célula; (3) o orçamento por tick = 1 ms (o do 4.0) dividido pelo custo da
unidade, medido nesta máquina e escrito com a medida. Antes de codificar: o pior caso e a média de cada operação da tabela, as que
faltam inclusive, na bancada (todos os pátios), com a máquina dividida com outra sessão (o pior caso medido assim é um teto).

Medido (`evidence/yard/medida-ops`, a bancada inteira; vezes, média, pior, cobrado):

| operação | vezes | média | pior | cobrado |
|---|---|---|---|---|
| criar o `ClearanceFill` | 78 | 1,15 ms | 3,33 ms | 0 |
| preencher um anel | 5.503 | 19 µs | 3,33 ms | caixa / 64 |
| criar uma busca | 579 | 0,17 ms | 2,10 ms | 0 |
| linha da transformada | 99.800 | 2,4 µs | 0,09 ms | 1 + n/64 (0,225 µs por unidade) |
| linha da rua | 52.851 | 0,5 µs | 0,05 ms | 1 + n/64 (0,048 µs por unidade) |
| célula da holonômica | 2.167.117 | 0,085 µs | | 1 |
| pose (com o tiro dela) | 23.686.312 | 4,2 µs | 0,35 ms | 8 |
| tiro | 6.881.198 | 2,5 µs | 0,34 ms | 8 |
| `join_ways` | 243 | 44 µs | 0,13 ms | 0 |
| `settled`, `next_dock` | 340, 323 | 4–7 µs | ≤ 0,07 ms | 64, 49 |

A unidade é a de escrever 64 células da grade (0,048 µs, a linha da rua). Execução:
1. **Nada maior que uma fatia.** O `ClearanceFill` cria as suas linhas no `advance`, uma a uma; o anel é preenchido linha a
   linha, cobrada pelas arestas que a linha corta e pelas células que escreve (o anel de via tem milhares de vértices: o custo é
   linhas × arestas, não a caixa); a busca reusa a memória do planejador (tabelas de poses e fila: a primeira escrita em ~3 MB
   novos é o que custa ao criar uma busca) e o mapa de distância novo enche as suas células no `advance` dele, cobradas.
2. **Cada operação cobrada pelo seu custo**, na unidade da célula: a pose ~72, o tiro ~52, a célula da holonômica 2, a linha da
   transformada 5 vezes a de preencher, `join_ways` pelas poses que lê.
3. **O orçamento** (`lorry_plan_work`): 1 ms ÷ 0,048 µs ≈ 20.800 unidades, com a medida escrita no `_doc`.
Verificação: o pior tick do planejamento ≤ 1 ms (sonda `yard_tick_ms`), a bancada com as mesmas docas e poses (a cobrança muda
só quando cada pedaço roda, não o que roda).

### O que a execução mostrou (bancada com o orçamento real por chamada: a sonda `slice_ms`, a pior fatia de cada pátio)

| passo | pior fatia: máx / mediana | o que foi achado |
|---|---|---|
| cobranças medidas, pose fixa em 72 | 4,20 / 1,75 ms | a pose da graduada custa 5,4 µs, a da grossa no aberto 2,5: um valor fixo erra pela metade |
| a pose cobrada pelo que faz (ajuste em 217 pontos) | 4,69 / 1,32 ms | o pico ficou, na fina do 1398 |
| a fila num deque (sem cópia ao crescer) | 4,04 / 1,31 ms | não era a fila |
| as páginas das tabelas contadas e cobradas | 4,35 / — | não eram as páginas (0,02 por pose na fina) |
| **poses e fila em pedaços que o clear mantém** | **2,11 / 1,60 ms** | era liberar os ~40 mil blocos de 4 KB do deque da busca anterior numa fatia |

O ajuste (`work_fit.py`, mínimos quadrados não negativos): a pose 0,86 µs, a célula de folga lida 0,0065 µs, o comprimento de
Reeds–Shepp 1,67 µs (1,28 µs medido sozinho), a página tomada 1,80 µs; erro por ponto 8,9 % na mediana, 33 % no pior. A aresta do
anel: no máximo 0,0154 µs (todo o tempo das linhas dado às arestas). O portão: 8,0 µs.

Em todas as etapas a pior fatia dá o mesmo tempo por unidade (±15 %): a cobrança está proporcional. O tempo absoluto não está
verificado: a máquina estava dividida com outra sessão (a transformada, com a mesma cobrança, foi 1,16–1,24 ms por fatia antes e
1,59 ms agora), e no `yard_test`, com tráfego e render ao lado, o pior tick foi 3,1 ms. Fica a medir com a máquina quieta. Duas rodadas do mesmo código, com o mesmo trabalho (o plano é o mesmo), deram para o mesmo pátio e etapa 1,58 e 3,64 ms (1397, a saída grossa) e 1,89 e 3,91 ms (1398, a fina), com o jogo de outra sessão rodando ao lado: o tempo de uma fatia nesta máquina varia o dobro de uma rodada para outra.
O plano não mudou em nenhum passo: as mesmas 228 docas com caminho e as mesmas 23.686.312 poses.

## Depois, medido à parte

A tabela pré-calculada da heurística não holonômica (Dolgov; Knepper e Kelly, IROS 2006), no lugar do Reeds–Shepp em tempo real
(~1/3 do custo de uma pose hoje): só depois das duas correções, com a medida delas, porque muda a ordem da busca.
