# Pares 4.0 x jogo: lista escrita antes de fotografar

Regra da coordenadora: nenhuma foto é boa sem o par do 4.0 no mesmo enquadramento, lido em resolução cheia, com os defeitos listados.

**Como o par é feito.** As cidades são diferentes (4.0: cidade pequena, semente 7; jogo: mapa 0), então "mesma posição" é a mesma pose em relação ao
mesmo tipo de objeto. No 4.0 cada foto grava, de `scene().viewCamera()`, o olho e a direção da câmera no referencial do objeto (ao longo da rua, para
fora da pista, para cima, em metros) e o campo de visão. O jogo reproduz esses números em `capture_street_pairs.gd`, na mesma resolução (1600x900) e
hora (12:00 e 22:00). O que não dá para igualar: o entorno de cada cidade, e a HUD, que aparece no 4.0 e não no jogo.

| Par | Prova |
|---|---|
| poste | mastro, cruzeta, fios e luminária de dia |
| poste-noite | luz da rua à noite, poças e queda entre postes |
| esquina-noite | piso da esquina recebendo luz |
| banco, lixeira, hidrante | peças da calçada |
| cerca, muro, sebe, defensa, gradil | as cinco barreiras |
| placa-0 a placa-9 | a frente de cada tipo de placa: Pare, Dê a preferência, velocidade, proibido estacionar, contramão, pedestre, escola, direção, rua, informação |
| semaforo | poste de semáforo |

## Rodada de perto (a primeira deixou placas e barreiras pequenas ou de costas)

A rodada larga pôs as peças a 15-22 m e as 10 placas passaram do quarteirão: `placa-4` caiu no cruzamento, e as de trânsito foram vistas pelo verso, que o
4.0 pinta espelhado. Nesta, uma placa por vez no mesmo ponto da calçada, de frente, e barreiras dentro do quarteirão. Valem como par de detalhe:

| Par | Prova |
|---|---|
| poste-perto, luminaria-perto | poste de rede e coluna de luz, base ao topo |
| banco-perto, lixeira-perto, hidrante-perto | forma, cor e tamanho das peças |
| cerca-perto, muro-perto, sebe-perto, defensa-perto, gradil-perto | forma, material e altura de cada barreira |
| placa-0-perto a placa-9-perto | a face de cada tipo, com a borda, a cor e o texto |

Da rodada larga ficam só: poste, banco, lixeira, hidrante, cerca, muro, sebe, defensa, semaforo, poste-noite, esquina-noite.

## Rodada de correção (só o que a correção mudou)

Depois de corrigir os defeitos de `pares-analise.md`, repito só: placa-0 a placa-9 (texto, orla, figura, seta, proporção), cerca, muro, sebe, defensa, gradil de perto
(acabamento metálico), poste-noite e esquina-noite (disco por lâmpada, colunas alternadas), banco-perto (regressão: peça que não mudou). Poses em `poses-correcao.json`.

## Rodada de correção 2 (só a noite)

Depois de acender um poste sim, um não: poste-noite e esquina-noite (quanto de luz chega à pista, medido contra o 4.0), poste e semaforo de dia (regressão: postes sem lâmpada, colunas alternadas). Poses em `poses-noite.json`.

## Rodada do estilo roadcraft (só o que os dados mudaram)

O estilo padrão passa a desenhar o 4.0: poste de 8,4 m, placas do tamanho do 4.0, zebra e retenção de `approach.ts`. poste e semaforo provam poste, fios, zebra e retenção; placa-0-perto e placa-2-perto provam o tamanho da placa. Poses em `poses-estilo.json`.

## Rodada da calçada (regras-sessoes.md, seção 11)

Às 12:00 nos dois jogos, em trecho de sol (as ruas dos pares anteriores estavam à sombra dos prédios no 4.0). O passo de `eval` imprime o inventário: peças por tipo, placas, postes acesos, quilômetros de via.

| Par | Prova |
|---|---|
| sol-lamp, sol-pole | sombra de poste no chão ao sol, base do poste |
| sol-bench, sol-bin, sol-hydrant | sombra e contato das peças baixas |
| (sem sol-tree: a cidade pequena do 4.0 não tem `tree` em landscape; a primeira tentativa parou aí) | |
| sol-rua | densidade de peças e ladrilho num trecho de calçada |

## Rodada dos fios por vão e da árvore (lista escrita antes das fotos)

Código com um vão por instância, poças na malha do chunk e fonte da folha solta depois do lote. Mesmas poses dos pares anteriores (`poses-final.json`), contra as fotos do 4.0 já tiradas.

| Par | Prova |
|---|---|
| poste, semaforo | os nove fios no lugar, ligados às cruzetas, contra `reference/pares` e `calcada-final` |
| esquina-noite, poste-noite | poças como disco por quatro cantos, sem borda, contra `pares-noite2` |
| placa-0-perto, placa-2-perto | texto da folha intacto depois de soltar a fonte |
| sol-rua, sol-pole, sol-lamp | calçada ao sol, às 12:00, sem árvore, contra `reference/sol` (recorte 3x para a prova da árvore) |

Resultado: `fios-final/`, código `accd70a` antes do comentário final. Recortes `fios_poste` e `fios_semaforo` contra `calcada-final`: fios iguais. A sombra na pista muda porque `calcada-final` tinha a cascata ligada. `esquina-noite` contra `pares-noite2`: mesma poça, e as árvores saíram. `placa-2-perto`: texto inteiro, teste de tinta passa em todas as células.

## Rodada da noite e dos nomes (lista escrita antes das fotos)

Código com colunas no ritmo do 4.0 e nomes de rua. A noite do jogo e o 4.0 a 22:00. Foto de mundo às 16:00 (seção 11, item 5), legenda diz a hora.

| Par | Prova |
|---|---|
| esquina-noite, poste-noite | perfil da pista ao longo da rua: trechos escuros entre as lâmpadas, como no 4.0 |
| name-16h (rua_20_streetname e pose nova) | placa com nome na esquina, de perto, à tarde |

## Rodada das folhas em camadas (lista escrita antes das fotos)

| Foto | Prova |
|---|---|
| rua_22_placa_* | placas pela nova amostragem, mesmas cores de `msaa/` |
| par_placa-2-perto, par_poste | plates e postes no mundo, sem cor trocada |
| rua_23_folha_0..N | 312 nomes extras: cada célula com tinta, nas camadas que abrirem |

## Rodada da defensa e da trama (lista escrita antes das fotos)

| Foto | Prova |
|---|---|
| defensa-perto | o W do perfil, com MSAA, às 16:00 |
| muro-perto, sebe-perto | se a trama fina persiste com MSAA e em qual superfície |
| rua_23_folha_0..N | folha que dorme entre rodadas e volta com as células antigas |
