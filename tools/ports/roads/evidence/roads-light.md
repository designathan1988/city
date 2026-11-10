# Luz e curva de tom: diferença global com o 4.0 (achada no porte de vias, não corrigida lá)

Vale para quem é dono de `render/environment.ts` (`landscape` e `interface` têm esse arquivo no rastreamento).

| | 4.0 | Jogo |
|---|---|---|
| Curva | ACES, `toneMappingExposure` 1; three multiplica a entrada por 1/0,6 | ACES, `exposure` 1,4; Godot multiplica por 1,8 e divide pelo branco, `tonemap_white` 6 |
| Sol | `4,2 * (0,25 + 0,75 * min(1, altura*2,5 + 0,2))`, dividido por pi na conta de Godot | `sun_energy` 1,8 |
| Ambiente | hemisfério 0,8, ambiente 0,07, mapa de ambiente 0,6 | `hemi_base` 0,25 + `hemi_day_k` 0,3 |

Fonte: `src/render/environment.ts:164-259`, `src/render/renderer.ts:501-503`, `project/data/game/daylight.json`. O ajuste ACES (`RRTAndODTFit`) é igual nos dois, lembrado do código de three e de Godot; **não reconfirmei o de Godot na fonte**.

Consequência: a entrada da curva no jogo é cerca de 2 vezes a do 4.0. A curva tem pé quadrático, então o escuro sobe perto de 4 vezes e o claro quase nada. Medido às 16h: capim 97 x 98 (calibrado), calçada 156 x ~205, asfalto 56 x 135.

Não mexi: `sun_energy` e `exposure` afetam prédios, árvores e terreno, e o capim foi calibrado sobre eles. No porte de vias só corrijo a cor do asfalto por classe.

## Cor das superfícies da via, medida na tela (`evidence/medir_superficies.py`, mesma separação nos dois jogos)

Mediana por canal, razão azul/vermelho (a/v) e, entre parênteses, o albedo do dado. Fotos: 4.0 `reference/58-free-junction-urban-top.png`, jogo `photos-t10/cross_urban_top.png`.

| Superfície (albedo sRGB) | 4.0 RGB | a/v | Jogo RGB | a/v |
|---|---|---|---|---|
| Asfalto (58 61 63) | 51 55 64 | 1,25 | 89 87 79 | 0,89 |
| Calçada (126 126 123) | 154 156 153 | 0,99 | 183 177 159 | 0,87 |
| Tinta branca (238 232 215) | 226 223 215 | 0,95 | 228 223 211 | 0,93 |
| Tinta amarela (225 196 90) | 211 190 92 | 0,44 | 245 234 179 | 0,73 |
| Capim | 72 99 45 | 0,62 | 83 100 59 | 0,71 |

O que isto diz, e o que ainda é suposição:
- Mesmo albedo nos dois jogos, então a diferença vem depois do material. Nos claros a tinta branca quase coincide; quem separa são os escuros e os médios.
- Na conta do jogo a luz é quente nas duas fontes: `sun_day` #fff0d2 tem azul/vermelho 0,64 em linear e `hemi_day` #e6e4da tem 0,85. Ganho aproximado no asfalto, sem a curva de tom: 2,4 no vermelho e 1,6 no azul. No 4.0: 0,8 e 1,0.
- O asfalto do 4.0 fica **mais azul que o próprio albedo** (1,25 contra 1,09), e a calçada fica neutra. Suposição: o reflexo do céu pesa no material escuro. Não provei.
- Tinta amarela do jogo sai lavada, 179 de azul contra 92, com a mesma cor de origem. Suposição: ganho alto mais a dessaturação da curva. Não provei.

## Experiência: só a luz trocada, mesma via

`evidence/photos-t13-light/pair_urban_top.png`: `sun_energy` 0,89, `sun_day` #fffaf3, `hemi_day` #b5d8ff, `hemi_day_k` 0,266 no `daylight.json`, restaurado depois. Sem a gradação de filme, sem o preenchimento nem o sol novo do commit 9cc9ec7.

| Superfície | 4.0 | Jogo, luz velha | Jogo, luz trocada |
|---|---|---|---|
| Asfalto | 51 55 64 (1,25) | 87 85 77 (0,89) | 53 61 69 (1,30) |
| Calçada | 154 156 153 (0,99) | 182 175 158 (0,87) | 137 144 150 (1,09) |
| Tinta branca | 226 223 215 (0,95) | 244 241 233 (0,95) | 229 231 230 (1,00) |
| Tinta amarela | 211 190 92 (0,44) | 245 234 177 (0,72) | 227 219 156 (0,69) |

Prova: a cor oposta do asfalto era da luz. Sem tocar no albedo ele passa de 87 85 77 a 53 61 69. Sobram a calçada azul demais e a tinta amarela lavada, que a medida com a luz inteira da Porte 3 vai dizer se são dela ou da via.

## Medida com a luz inteira da Porte 3 (9cc9ec7), 16h, mesma câmera

Ramo temporário `measure/luz` no clone de vias: `convert/roads` 62263e5 mais `convert/landscape` 9cc9ec7. Não entra em `convert/roads`. Fotos: `evidence/photos-light3`, `evidence/photos-light4`. Par: `evidence/parity/par_urban_low_luz3.png`, `par_urban_top_luz3.png`.

| Superfície | 4.0 (`58-`) | Jogo, luz nova (`pair_urban_top`) | Razão em linear, 4.0 sobre jogo |
|---|---|---|---|
| Asfalto | 51 55 64 (1,25) | 38 43 50 (1,32) | 1,7 |
| Calçada | 154 156 153 (0,99) | 127 130 127 (1,00) | 1,5 |
| Tinta branca | 226 223 215 (0,95) | 235 232 222 (0,94) | 0,9 |
| Tinta amarela | 211 190 92 (0,44) | 231 214 113 (0,49) | 0,8 |
| Capim | 72 99 45 (0,62) | 48 78 37 (0,77), com sombra de árvores; 62 92 40 (0,65) em `road_end` | 1,3 em `road_end` |

**Resolvido pela luz:** a cor. Azul/vermelho do asfalto e da calçada ficou certo sem mexer em albedo.

**Não resolvido:** o asfalto e a calçada ficaram **mais escuros** que no 4.0, por um fator parecido, e a tinta mais clara. Albedo igual ao do 4.0: `asphalt.map` tem média 57 e o fator da classe urbana é 1; `footway.map` tem média linear #7e7e7b e o 4.0 multiplica a calçada de concreto por 1,08.

Hipóteses que testei e caíram:
- Hora do sol: troquei para 16h à força e os números não mudaram (`photos-light4`). O sol já estava às 16h.
- `envMapIntensity` 0,55 na via contra 0,12 no terreno: three 0.186 usa `scene.environmentIntensity` (0,6) para todo material sem `envMap` próprio (`WebGLRenderer.js:2738`), então os dois recebem o mesmo.

Causa ainda não achada. Os alvos da Porte 3 foram medidos no chão pintado, e o capim do jogo nesta cena (62 92 40) fica abaixo do que ela mediu (72 103 52).

## Discos pintados ao lado da via, mesma foto (protocolo da Porte 3 e da coordenadora)

`evidence/discos/burley/discos.png`, ramo temporário `measure/luz`, 16h, mediana de 24x24 px.

| Alvo | 4.0 (tabela da Porte 3) | Jogo, ao lado da via |
|---|---|---|
| Areia | 228 220 202 | 227 220 199 |
| Neve | 235 234 231 | 231 230 228 |
| Asfalto pintado | 124 127 143 | 120 123 138 |
| Via (asfalto) | 52 56 65 | 36 42 48 e 40 44 50 |
| Calçada | 157 158 156 | 120 123 121 e 126 128 125 |

**Veredito:** os discos batem na minha cena, então hora, sol e sombra da cena estão certos. Só a via e a calçada saem escuras: o erro é do material da via ou de uma luz que o terreno não recebe.

O albedo não é a causa por baixa calibração: `asphalt_by_class` é igual ao `color` de `ROAD_TYPES` do 4.0 (`roadTypes.ts:115-225`: `#5f6365`, `#3a3d3f`, `#35383a`, `#333638`, `#343638`, `#3b3d3f`), e o mapa da via é o mesmo. A tabela com o LBNL em `research.md` só escreve em linear essas cores; o valor delas é o do 4.0. A calçada: `paving_colors[0]` é `#7e7e7b`, a média linear do `footway.map`, ou seja, fator 1; o 4.0 usa 1,08 no concreto, e isso ainda falta conferir e aplicar.

Testes que não mudaram nada: `diffuse_lambert` (medianas iguais, `evidence/discos/lambert`), sem receber sombra (+2 a +4 níveis, `evidence/discos/semsombra`), sem mapa de normal (`photos-light5/semnormal`). F0 0,04 do 4.0 subiu o asfalto em uns 5 níveis e entrou em `convert/roads`, `d4074a4`.

Nota sobre a medida da coordenadora: `par_urban_top_luz.png` tem o 4.0 em cima e o jogo embaixo. O 51 55 64 é a metade de cima, o próprio 4.0.

Mediana contra média dos mapas, em linear: `asphalt` 0.042 e 0.040; `footway` 0.205 e 0.232.

Na calçada a mediana do mapa é maior que a média, por causa do rejunte escuro: albedo mediano 0,232 sai 126 na tela, enquanto o asfalto pintado, 0,17, sai 120. Albedo 1,36 vezes maior dá só 1,11 vez mais na tela, e a curva não achata tanto ali. Resíduo do material do jogo ainda sem causa.

## Causa do material e número fechado

Causa: `_mean_color` em `city_world.gd` reduzia o mapa por bilinear, que lê só alguns pixels. No `footway.map` eles caíam nas pedras e a média saía perto de 0,30 em vez de 0,205; o albedo `t * cor / média` encolhia na mesma razão. Prova na mesma foto, calçada mostrando um termo por vez (`evidence/discos/termo-*`): 0,21 dá 147, só o mapa 145, só a cor 147, só a média 166, a conta inteira 124. Corrigido em `377cbc0`.

Medida no ramo temporário: `convert/roads` 377cbc0 mais `convert/landscape` 01f6288, 16h, miolo da foto, `evidence/photos-light6`, par em `evidence/parity/par_urban_top_luz6.png`.

| Superfície | 4.0 | Jogo | Razão em linear |
|---|---|---|---|
| Asfalto | 52 56 65 (1,25) | 47 53 61 (1,30) | 1,15 a 1,25 |
| Calçada | 157 158 156 (0,99) | 151 153 152 (1,01) | 1,08 |
| Tinta branca | 227 224 216 | 233 230 221 | 0,9 |
| Areia pintada | 228 220 202 | 225 218 197 | |
| Asfalto pintado | 124 127 143 | 120 123 137 | |

O fator de cor da calçada é 1: no 4.0 o material padrão é `pavers` (`roadSurfaceReads.ts:199`), então os 1,08 do concreto não se aplicam aqui.
