# Pares 4.0 x jogo: análise (substitui a análise de fotos do relatório anterior)

A análise antiga do relatório dizia "placa redonda, Pare, banco, hidrante, lixeira e caixa de correio próximos" sem par no mesmo enquadramento. Ela está
retirada: a leitura em par mostrou o contrário em vários itens.

**Conjuntos.** 4.0: `reference/pares/` (larga, 22 fotos) e `reference/perto/` (20), tirados em 2026-11-04. Jogo: `evidence/pares-jogo-1db1d62/` (31, código
`1db1d62`, rodada 1). `evidence/pares-jogo/` tem 5 fotos de uma tentativa que falhou por classe fora do cache e não vale. Poses em `evidence/poses-pares.json`.

**O que não é par.** `reference/perto/10-poste-perto` saiu tapada por um prédio e não vale. As cidades são diferentes, então o entorno nunca bate; a HUD
aparece só no 4.0; o jogo tem árvore na calçada e o 4.0 não. A primeira rodada do 4.0 deixou placas pequenas ou de costas e `placa-4` caiu no cruzamento: por isso
a rodada de perto. Erro meu de escala: as poses vieram divididas por 1,257, e no 4.0 uma unidade é um metro; corrigido antes de fotografar o jogo.

## Rodada 1: defeitos achados (código `1db1d62`)

| # | Par | Defeito no jogo | Como é no 4.0 | Causa | Dono |
|---|---|---|---|---|---|
| 1 | placa-0 | Pare sem o texto `PARE` e sem a orla branca | `PARE` branco em negrito, orla branca | texto: `clear_cache()` do `1db1d62` deixou sem glifos todo lote depois do primeiro; orla: não existia | meu |
| 2 | placa-1 | Dê a preferência branco com uma ponta vermelha | faixa vermelha em volta, miolo branco | a borda era um retângulo, cortado pelo triângulo da malha | meu |
| 3 | placa-5 | pedestre: losango amarelo liso | figura andando e filete preto | só havia a palavra `PEDESTRES`, e ela sumiu pelo item 1; figura não existia | meu |
| 4 | placa-6 | `ESCOLA` sem texto | `ESCOLA` e filete preto | item 1; filete não existia | meu |
| 5 | placa-7 | direção vazia, sem seta | texto e seta | item 1; seta não existia | meu |
| 6 | placa-8, 9 | rua e informação vazias | texto | item 1 | meu |
| 7 | placa-3 | sem o `E` | `E` sob a barra | item 1, e a barra era desenhada antes da letra | meu |
| 8 | placas | Pare com 0,78 m entre faces, não os 0,845 m declarados | (norma MBST) | o polígono era inscrito no círculo de `w_m`; afirmei uma medida que a malha não tinha | meu |
| 9 | placas retangulares | texto esticado 1,6 a 1,8 vez na horizontal, se houvesse | proporção certa | célula 2:1 sobre placa 3,2:1 e 3,6:1 | meu |
| 10 | defensa | branca, chapada | cinza-esverdeada, aço | sem `metalness`: `#b4bcbf` puro sob luz forte | meu |
| 11 | cerca, gradil | claros | escuros, pintura metálica | idem | meu |
| 12 | poste-noite | poças em ilhas separadas por trechos pretos | encadeadas, a pista toda iluminada | colunas na mesma estação dos postes; o 4.0 alterna meia distância | meu |
| 13 | esquina-noite | placa do cruzamento escura e marrom | luz contínua pelo cruzamento | a poça vivia no shader da via e por construção não chegava à placa | meu |
| 14 | poste-noite | calçada sem luz | luz cruza a calçada | idem: poça presa à pista | meu |
| 15 | placa-2 | tronco na frente da placa | livre | captura pôs a placa atrás de uma árvore; defeito do enquadramento, não do jogo | captura |

Fora deste porte, vistos e passados à coordenadora, não corrigidos aqui:

| # | Par | O que se vê | Dono provável |
|---|---|---|---|
| A | banco-perto | faixa branca dos balizadores do lote rasgada; no 4.0 é limpa | lotes |
| B | todas de dia | calçada e meio-fio quase brancos; no 4.0 cinza-azulados, e o dia todo mais claro | Porte 1 (albedo das vias) e céu |
| C | noite | prédios e entorno quase pretos, janelas sem luz por andar; no 4.0 mais claros e com janelas quentes | prédios e dia/noite |

## Correções e o que as fotos da correção mostram

Conjuntos: `evidence/pares-correcao/` (18 pares e a folha; placas e barreiras, antes de acender um poste sim, um não), `evidence/pares-noite/` (mesmo código, ganho 0,32) e
`evidence/pares-noite2/` (ganho 0,234; o estado final da luz). As placas e barreiras não mudaram depois de `pares-correcao`. Li em resolução cheia as fotos citadas abaixo.

| # | Como ficou | Conferido em |
|---|---|---|
| 1, 3 a 7 | texto, figura e seta aparecem; a captura confere a tinta de cada uma das 13 células com tinta, 0 falhas | `rua_23_folha`, placa-0, 3, 5, 6, 7, 9 |
| 2 | faixa vermelha em volta, miolo branco | placa-1 |
| 8 | a malha agora enche `w_m` x `h_m`: o Pare tem os 0,845 m declarados entre faces | código; não medido em foto |
| 9 | letra na proporção da placa | placa-7, 9 |
| 10 | defensa cinza-azulada, não mais branca | defensa-perto |
| 11 | gradil verde-escuro, como no 4.0 | gradil-perto |
| 12 | poças encadeadas, sem os trechos pretos | poste-noite |
| 13, 14 | **parcial**: a poça passa a cair na placa e na calçada, mas o miolo de um cruzamento largo segue escuro | esquina-noite |
| 15 | captura escolhe trecho livre, placa de frente | placa-0 |

**Noite contra o 4.0, medido no pixel** (faixa da pista, x 760-1000, y 350-780, 0-255, média dos canais):

| | média | mediana | p95 | máximo |
|---|---|---|---|---|
| 4.0 | 63,5 | 59 | 134 | 227 |
| jogo antes (`1db1d62`) | 34,9 | 3 | 140 | 170 |
| jogo agora | 79,9 | 89 | 159 | 185 |

Não bate. O jogo saiu de escuro demais para claro demais: média 26 % acima, mediana 51 % acima. Parei de propósito: mexer mais olhando foto seria varrer parâmetro, o que a regra proíbe.
A rua do jogo e a do 4.0 também não são iguais (larguras e árvores diferentes), então esse número compara ruas parecidas, não a mesma rua. O ganho 0,234 foi calculado pelos dois ACES, sem ajuste por foto.

**Correção de leitura minha.** Na tabela da rodada 1 escrevi "luz contínua pelo cruzamento" (13) e "luz cruza a calçada" (14) no 4.0. Relendo `96-esquina-noite` e `92-poste-noite`, isso exagera: no 4.0 o cruzamento também é escuro e a
calçada recebe pouca luz. O que muda é que lá o asfalto ainda se vê no escuro, e a faixa de luz é mais estreita e irregular. No jogo a faixa ocupa a pista quase toda, de forma lisa.

**Defeitos que ficam, com o que falta:**
- A faixa de luz da pista é mais larga, lisa e clara que a do 4.0, como na tabela acima. É meu e não está resolvido.
- Defensa: o aço do 4.0 reflete o céu e sai cinza-esverdeado; aqui sai mais azulada e chapada. O metal está no material, mas a caixa é lisa e o perfil W do 4.0 não foi feito.
- A placa de informação e a de direção mostram o `40` da captura, que é o texto digitado nela, não defeito.
- O `4.0` espelha a face no verso em algumas placas; o jogo mostra verso cinza. Não comparei além disso.

Cerca, muro e sebe de perto (`pares-correcao`), lidas em resolução cheia contra `reference/perto/30`, `34`, `38`:
- Cerca: postes e duas travessas nos mesmos lugares; cor próxima. Sem defeito de forma.
- Muro: corpo e capeamento presentes, sem defeito de forma.
- Sebe: caixa lisa nos dois; a do jogo sai bem mais clara e saturada que a do 4.0, com a mesma cor `#3d6532`, porque o 4.0 está mais escuro nessa hora.
- Defensa: lá está junto à pista e aqui sobre a calçada, porque a captura a pôs em outro ponto; o par compara a peça, não o lugar.
- Vista em muro, sebe e paredes de prédio: uma trama fina de pontos nas superfícies lisas. Aparece fora das minhas peças, então não é da barreira; não investiguei a causa. Vai à coordenadora com `par_muro-perto` e `par_sebe-perto`.

Não reli `banco-perto` depois da correção: a peça não mudou.
