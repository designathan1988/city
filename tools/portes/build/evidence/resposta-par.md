# Resposta à coordenadora: os três itens, com o mesmo prédio na mesma pose

Tudo abaixo vem da rodada nova, com a luz da Porte 3 (ramo `medir/luz`). Sim, o par2 saiu com essa luz, como você supôs. Daqui em diante `user://` é próprio (`project/override.cfg`, pasta `Roadcraft-build`); o que veio antes vale com a ressalva do cache compartilhado.

## 3. Enquadramento: causa e correção

O par antigo não podia dar certo, por quatro razões medidas:
- **Prédios diferentes.** Dos 1.348 prédios do 4.0, no máximo 8 têm um prédio do jogo a menos de 6 m. A distância mediana é de 14 a 15 m. Com qualquer pose, a torre azul não estaria ali.
- **Câmera.** O 4.0 usa lente fixa de 35°. O jogo calculava a lente pela altura.
- **Espelho.** O `y` da planta do 4.0 é `-z` no mundo. As vistas p5 e p6 estavam espelhadas no jogo.
- **Chão.** O chão do 4.0 não é plano. O mapa 0 do jogo é.

Correção:
- `body_doc`: o documento de prédio do 4.0 agora vira corpo. É o mesmo formato de `lots/*.json`, então serve para `marcos`.
- `capture_pair --buildings=`: põe os prédios lidos do mapa vivo, 429 de 441 (12 recusados por tamanho, `building.problem.size`).
- Posição, olhar e lente lidos de `viewCamera` (`evidence/pose/vistas-pose.json`).

Par refeito: `evidence/recortes/p3-torres-par7.png`. A torre azul, a torre bege e o prédio de vidro agora são os mesmos nos dois.

Limite: alinhei a altura pelo prédio central (11 m, medidos na própria foto, não lidos do 4.0). Prédios em outro nível do chão ficam deslocados. Não consegui ler a altura do chão do 4.0 por `window.__roadcraft`. Compare prédio a prédio, não a foto inteira. Ruas e chão não valem nessas fotos, porque o mapa do par não tem ruas.

## 2. Brilho: só fachada, mesmo prédio, mesma pose

Cor média da parede (RGB), lado de sol e lado de sombra:

| Recorte | 4.0 | Jogo |
|---|---|---|
| Torre bege, sol | 222,211,187 | 222,213,194 |
| Prédio à esquerda, sol | 199,183,152 | 208,193,167 |
| Torre bege, base na sombra | 152,147,123 | 168,166,150 |
| Torre azul, sombra | 72,79,84 | 100,120,138 |
| Laranja | 151,77,38 | 159,102,69 |

Parede ao sol: igual ou quase, sem nada a mexer. Parede na sombra: o jogo fica mais clara e mais azul. O percentil 5 mostra o mesmo (47 contra 85 a 96).

Causa provável, ainda **não provada**: falta o `render/canyonLight.ts` do 4.0, que tira céu do fundo das ruas. Pela `boundary.json` é da Porte 3 e não está no clone dela. Em campo aberto o cubo da Porte 3 bate, o que combina com isso. Não mexo em valor nenhum antes de medir com esse passe.

Corrigido nesta rodada, com medida: `finish_mean` dividia o vidro por 0,081; o reflexo do céu entrava duas vezes no Forward+; o nível distante não levava o tom do acabamento. p1 foi de 79/114/125/141/211 para 50/78/90/96/168 (4.0: 45/75/88/97/159).

## 1. Relevo: o que é e o que não é

Sua observação estava certa quanto ao efeito, mas parte da causa era outra: as fotos mostravam prédios diferentes. Com o mesmo prédio (`evidence/recortes/p3-bege-par5.png`):
- **Pilastra e cornija existem**, nas medidas do documento: largura 0,55 m, saliência 0,32 m, coroa `deco`. O que falta é a sombra delas na parede. Causa ainda a medir: atlas de sombra e SSAO, em par com a Porte 3.
- **Batente fundo:** corrigido. O vão agora é desenhado no vidro, com o sol exato e o céu pelo fator de forma, como no 4.0. São 8 triângulos a menos por janela.
- **Guarda-corpo nos recuos: falta de verdade.** A peça existe, mas `part_bands` a corta quando a barra fica abaixo de meio pixel. O 4.0 a desenha de longe como cartão, com as barras no shader. É o meu próximo item.
- **Pedra:** é o mesmo mapa nos dois. Mancha e escala estão perto; a diferença restante vem da sombra.

Outros defeitos reais achados no mesmo par (`evidence/recortes/p3-vidro-par5.png`):
- Telhado do prédio de vidro claro no jogo, escuro no 4.0, e com poucos equipamentos.
- Sala com piso de madeira e sofá em prédio de escritório. Suspeito do caminho novo do documento, a conferir.
- Moldura mais escura e fina: 148 contra 185 no valor mais claro.
- Vasos de varanda grandes e cúbicos.

## Plano, nesta ordem
1. Cartão de longe para as peças finas (guarda-corpos dos recuos).
2. Uso e função do prédio no caminho do documento; telhado e equipamentos.
3. Moldura, vasos e equipamentos de fachada, cada um contra o mesmo prédio.
4. Sombra das pilastras, medida com a Porte 3. Falta decidir quem faz o `canyonLight`.

## O que não foi verificado
- A causa do item 2 e a sombra das pilastras são hipóteses.
- Tempo de quadro e memória do `building_glass` (com `light()`) não foram medidos: a máquina está carregada.
- `medir/luz` não se entrega; serve só para as fotos.
