# Plano do aspecto global (exposição, sol, curva de tom, céu, atmosfera)

Dono: porte landscape. Nada global muda antes da medição abaixo. Quem muda é este arquivo que avisa.

## O que já está medido

| Alvo | Jogo | 4.0 | Ganho em radiância para casar | Quem mediu |
|---|---|---|---|---|
| Asfalto urbano | 94 | 56 | 0,51 | Porte 1, sol das 16h |
| Calçada | 172 | 158 | 0,82 | Porte 1, sol das 16h |
| Capim | 98 | 104 | 1,09 | Porte 1, sol das 16h |

Um ganho só não serve: os três pedem valores diferentes.

Conta a partir do código, sem foto: no meio-dia em chão plano, a entrada do ACES é cerca de 5,2 vezes o albedo no jogo e 2,9 vezes no 4.0.
- Jogo: sol 1,8, ambiente 0,5, exposição 1,4, viés 1,8.
- 4.0: sol 4,2/pi, céu e preenchimento 1,7/pi, entrada dividida por 0,6.
- Testei a troca direta para sol 1,34 e exposição 0,926 aplicada à medida acima: o asfalto dá 58 (certo), mas a calçada dá 126 (alvo 158) e o capim 61 (alvo 104).

Hipótese ainda sem prova: parte do excesso no escuro é soma, não ganho (reflexo especular do céu sobre asfalto e folhas). Reduzir o especular das folhas já tirou o véu pálido das copas.

## Alvos da próxima medição

Mesma câmera nos dois jogos, sol na mesma elevação, cidade pequena no 4.0 e mapa 0 no jogo. Fotos tiradas por esta sessão.

1. Asfalto de via urbana.
2. Calçada de concreto.
3. Grama em campo aberto.
4. Parede branca.
5. Chão pintado com albedo igual nos dois: areia 0,8, neve 0,93, concreto 0,66, asfalto 0,17.
6. Céu: zênite, meia altura e horizonte, de dia. Céu e chão à noite.

## Regra de decisão

- Se a razão entre os alvos casar nos dois jogos, muda a exposição ou o sol.
- Se só um alvo destoar, o problema é daquela superfície e vai para o dono dela.
- Se o erro for de soma no escuro, muda o reflexo do ambiente, não a exposição.

Depois de qualquer mudança global: fotos de antes e depois da mesma cena em `ports/landscape/evidence/luz/` e aviso às sessões.

## Medição feita (fotos próprias, sol a 29° nos dois, `evidence/luz/`)

O modo dia do 4.0 fixa 16h (`renderer.ts:3725`), com o sol a 29°. A primeira rodada comparou essas fotos com o jogo às 12h e foi descartada; os números abaixo são os da mesma altura de sol. Discos pintados têm o mesmo albedo nos dois jogos (`ground_paint.gdshaderinc` e `terrain.ts`).

| Alvo | 4.0 | Jogo antes | Jogo depois |
|---|---|---|---|
| Areia | 228,220,202 | 181,168,141 | 224,217,198 |
| Neve | 235,234,231 | 194,190,181 | 233,233,230 |
| Cascalho | 193,192,189 | 134,128,117 | 186,186,183 |
| Asfalto pintado | 124,127,143 | 92,89,88 | 127,132,146 |
| Grama | 74,103,55 | não comparável | 71,101,52 |

Causas achadas:

1. O 4.0 tem uma gradação de filme depois do ACES (`GRADED_OUTPUT_FRAGMENT`, `postprocess.ts`) e o jogo não tinha. Invertida a gradação, os alvos do 4.0 caem numa reta sem soma: `x = 1,63 / 1,72 / 1,82 × albedo − 0,06` (R, G, B). A hipótese do reflexo somado no escuro está descartada para o chão; não medi com esfera cinza.
2. O sol do 4.0 é `setRGB(1, 0.96, 0.9)`, valores lineares; aqui era `#fff0d2` lido como sRGB.
3. A cadeia de tom difere: o 4.0 entra na curva com 1/0,6 e o jogo com 1,4 × 1,8. A razão 0,661 vale para o que não recebe luz: céu, neblina, nuvens.
4. O ponto branco 6 dividia a curva por 0,977. Com 14,26 a divisão é 1.
5. O mapa de detalhe da grama é dado, não cor; lido como sRGB apagava metade do chão de perto.

## Decisão aplicada

Exposição igual (1,4). Sol 0,89, preenchimento 0,21, ambiente 0,266 azul, céu e ar com ganho 0,661, gradação como tabela 3D, vinheta sob a interface, trajetória do sol do 4.0. Saíram `grass_tint`, `land_gain`, `night_ground` e o grão alargado.

## Ainda sem fechar

- Porte 1 mede via, calçada e capim 1,3 vez mais escuros na cena dela; causa desconhecida.
- Sombra de rua mais azul e escura que no 4.0; `canyonLight` não portado.
- Grama muito de perto: 66,95,50 contra 81,106,52.
