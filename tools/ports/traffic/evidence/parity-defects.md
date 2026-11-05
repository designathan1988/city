# Paridade por classe: defeitos da rodada paridade-2 (mesma pose, 16:00, 1600x900, lado a lado)

Fotos em `photos-game/paridade-2/lado`. Esquerda 4.0, direita jogo. Causa de cada defeito lida no código.

Rodada paridade-1 descartada: a amostra escolhia veículo fora dos 900 m publicados em volta da câmera (o veículo do centro da
foto não era o da amostra; o SUV nem aparecia). Corrigido em `capture_parity.gd`: câmera vai ao veículo, simulação dá 2 passos,
amostra de novo; só veículo no meio da quadra (`sample_mid_m`) e com a linha de visada livre.

## Carroceria (hatch, sedã, SUV, van, picape: um só gerador)
1. Sem colunas nem moldura: o para-brisa ocupa o anel inteiro da seção. `region_of`: toda faixa acima da cintura sobre o para-brisa
   é GLASS. Parece conversível com toldo. O 4.0: vidro só no topo da seção; lados são coluna preta; moldura de 35 mm em volta.
2. Vidro quase invisível, sem cor: `agent_glass` tem ALBEDO 0. O 4.0: #7896a2 a 34 %.
3. Espinho branco no pé da coluna A, na porta: faixa da cintura vira PAINT onde o vidro lateral fica baixo (`glass_h <= MIN_GLASS_M`).
4. Capô facetado, ombro duro: `y3 = clamp(belt, ..., top)` e `y4 = top` tiram o raio do ombro no capô; anel de 11 pontos.
5. Ponta sem fechar: a tampa (cap) é pintada acima do para-choque; no 4.0 é toda escura, com placa e grade sobre ela.
6. Faróis e lanternas no plano final (`xf = 0.5`), onde a largura é pequena: ficam juntos no meio, como um rosto. No 4.0 ficam na
   superfície do nariz, a `W/2 - 0,26 m` do meio (lanterna `W/2 - 0,18 m`). Sem pisca.
7. Lanterna de dia apagada e vinho (#722f37); no 4.0 acesa em carro andando (#ff3b2f), apagada (#5e1712) em carro parado.
8. Sem subida do para-choque (`noseLift`, `tailLift`) nem arredondamento dos últimos 6 cm.
9. Vidro lateral com limites próprios (`side_glass`) em vez dos do 4.0 (coluna A e última porta); portas por divisão ao meio.
10. Teto preto em um de cada cinco carros (`blackRoof`) não existe.
11. Sem perua (uma em três dos sedãs do 4.0): a foto do "sedã" do 4.0 é uma perua.

## Cabine
12. Bancos são caixas empilhadas. No 4.0: perfil reclinado, laterais, encosto de cabeça com hastes.
13. Painel é uma caixa; volante é só aro, sem raios nem cubo.
14. Pessoas de blocos. No 4.0: o mesmo corpo dos pedestres, sentado.

## Rodas
15. Pneu de 12 lados sem ombro, calota plana com setores escuros. No 4.0: ombro redondo, 16 lados, aro recuado, cinco raios.

## Espelho
16. Sem o vidro do espelho na face de trás.

## Veículos que não têm par válido
- Caminhão: as duas fotos do 4.0 saíram tapadas (parede lisa). Sem par: não comparo. Sem nova rodada do 4.0 (limite já passado).
- Moto: nenhuma moto no meio da quadra na amostra: `_pick` precisa cair para qualquer uma com visada livre.
- Bicicleta: bicicleta fora do centro; andando, desenhada com atraso: preferir parada.

## Fora do tema, para a lista de quem é dono
- À noite o asfalto do jogo fica muito mais claro que o do 4.0 e as faixas brilham (luz da via, Porte 7/mobiliário).
- Padrão fino em toda a imagem do jogo no recorte 3x (conferir: debanding do Godot?).
