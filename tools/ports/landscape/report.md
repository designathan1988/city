# Relatório do porte landscape

## Correção importante (leia antes do resto)

**As fotos do jogo das seções abaixo têm 1920x1061, não 1920x1080 como este relatório dizia.** A tela tem 1080 linhas e a barra de título levava 19. Por isso aqueles pares não têm a mesma resolução e não valem como par: a escala difere 1,8 %. Os valores de tom valem só como indicação até serem refeitos. A captura agora usa janela sem borda e o tamanho é conferido por script.

Parte daquelas medidas ficou velha: a luz mudou no commit 34ce206, e a seção "Seção 11" vale sobre as outras.

## Rodada dos pares no mesmo chão (commits e2d7d2d a 822506a e seguintes)

Esta seção vale sobre as de baixo. As medidas de tom e de cobertura de antes foram feitas com a câmera 400 m ao lado do 4.0.

**Pasta de usuário própria (regras, seção 12):** `project/override.cfg` dá ao clone o diretório `Roadcraft-landscape`, fora do git por `.git/info/exclude`. Daqui em diante a pasta é própria; as medidas anteriores valem com a ressalva do cache compartilhado.

**Enquadramento:** a captura olhava x=2600, e o 4.0 x=3000. Agora os dois usam o mesmo `setView`, com `y` do 4.0 igual a `-z` do jogo. As matas usam a mesma densidade, o mesmo hash e a mesma malha de células do 4.0, então cada árvore cai no mesmo lugar. Os pares do campo e das matas passam a ser do mesmo chão.

### Matas

**Causa, provada no par:** o jogo tinha regra própria (faixas por quantil, cerca de 140 árvores por hectare no miolo, copas de 8 m). O 4.0 usa manchas em 170/55/18 m, faixas por densidade e um orçamento de 2.500 árvores por 1.920 m de lado, ou 6,78 por hectare, com tamanhos de 11 a 19 m no miolo.

Mudanças:
- Regra e malha do 4.0 em `tree_place.cpp` e `tree_look.cpp`.
- Só carvalho, cipreste e palmeira nas matas; o `broadleaf_tall` estreito saiu e entrou um segundo carvalho.
- A vizinhança conta árvores de verdade: `crowd` e a inclinação para o lado aberto.
- A carta distante é a própria árvore, uma por árvore, e não lança sombra (`farForest.ts` tem `castShadow = false`).
- O bioma usa o mesmo ruído do 4.0 e a mesma semente, sem o bit de mapa plano.

Cobertura de copas, com o corte de cada imagem (Otsu sobre a luminância dividida pela mediana da linha):

| Foto | 4.0 | Jogo |
|---|---|---|
| `chuva` | 0,199 | 0,210 |
| `campo_meio` | 0,190 | 0,204 |
| `matas_meio` | 0,086 | 0,086 |
| `nuvens_sombra` | 0,080 | 0,101 |
| `horizonte` | 0,202 | 0,240 |

O 0,181 contra 0,103 em `chuva` que eu passei à coordenadora era viés da métrica de limiar fixo: na chuva as copas do 4.0 ficam mais claras e escapavam do corte. O excesso real está nas vistas distantes, de 19% a 26%. Causa ainda não achada. Parte das árvores cai em faixa mais densa que no 4.0 (conífera onde o 4.0 tem palmeira): a ecologia do jogo ainda não é a do 4.0 célula a célula (grade de 16 m contra 6,4 m, e `altitude` medida do mar, não do ponto mais baixo).

### Palmeira e folhas

Causas:
1. **Regressão minha em 2b0a0e6:** o laço do atlas usava `side += REGIONS`; com 3 regiões, só metade da fronde era pintada.
2. **Mipmaps, provado:** num teste de 8x8 com texels alternados de tom 200 e de nada, `Image.generate_mipmaps` deu 100, a média dos números, e não 146, a média da luz. Sobram 22% da luz onde a cobertura é de 50%. `linear_mips.cpp` tira os níveis na luz linear. Script: `mips.gd` no bloco de notas da sessão.
3. A fronde era uma curva reta; agora arqueia e cai por ângulo.
4. O palmito usava o tom da folha; agora tem tom de casca e 4 cocos.
5. De longe a palmeira virava graveto: no retrato de 128 px, um ponto do atlas caía em folíolo ou em vão e quase tudo ficava abaixo do corte. Agora o retrato lê um nível onde o texel é a cobertura dos folíolos.

O tronco da palmeira perdeu a casca e a saia e ficou 1,3 vez mais grosso, pela medida do par `solta_palma_perto` (65 px no 4.0, 50 no jogo). Não achei o motivo dessa diferença no código: o raio do modelo dá 53 px.

**Pontilhado:** `tree_fade` dependia só da distância, com faixa de 3 m, e a árvore nessa faixa ficava em dither para sempre com o quadro parado. Agora cada árvore troca de nível inteira na sua distância, sem dither. Ao mover a câmera a troca é seca; não medi em movimento.

**Hipótese refutada:** tirei a especular das folhas para ver se o lado ao sol ficava menos pálido. Tom das folhas mais claras: 4.0 101,140,76; jogo 96,132,73; sem especular 95,126,74. Não era a especular, e o tom já está a 5%.

### Serrilhado e preset

Medido no par `solta_carvalho`, com a fração de bordas fortes (diferença entre vizinhos acima de 40):

| Caso | Bordas fortes |
|---|---|
| 4.0 | 0,0062 |
| Médio, antes | 0,0083 |
| Médio, com alfa para cobertura | 0,0071 |
| Alto, sem alfa para cobertura | 0,0046 |

A causa é de borda, não de textura: o MSAA não age sobre alfa cortado ("MSAA does not reduce transparency aliasing for materials using the Alpha Scissor transparency mode", documentação do Godot, 3D antialiasing). O filtro anisotrópico já estava em 16x. As folhas passaram a usar `alpha_to_coverage`.

Tempo de GPU em 1920x1080, média de 30 quadros, com outras sessões usando a máquina, `--preset=`:

| Foto | Médio | Alto |
|---|---|---|
| `campo_meio` | 4,63 ms | 6,16 ms |
| `matas_meio` | 5,17 ms | 5,57 ms |
| `luz_rua` | 6,12 ms | 7,34 ms |
| `noite_cidade` | 6,23 ms | 8,41 ms |

**Proposta:** padrão `high` na RTX 3060. O pior caso medido ficou em 8,4 ms de 16,6 ms, e ele traz SMAA, SSAO e MSAA 4x. Só testei essa placa, e a medida foi feita antes do `canyonLight`. O painel é da Porte 4.

### Sombra

Trama medida numa parede cinza lisa, na penumbra da sombra de um pilar (`LandscapeCards.weave`, média do Laplaciano):

| Filtro | Blur 1,0 | Blur 0,6 | GPU |
|---|---|---|---|
| Soft Low | 13,93 | 12,57 | 4,8 a 5,6 ms |
| Soft Medium | 8,89 | 7,31 | 5,7 a 5,9 ms |
| Soft High | 6,36 | 5,36 | 5,8 a 6,0 ms |
| Soft Ultra | 4,31 | 3,87 | 5,5 a 6,0 ms |

Escolhi Soft Ultra com blur 0,6. A largura da penumbra na borda do cubo já era a do 4.0 (3 px nos dois, medida antes dessa troca). O custo ficou dentro do ruído da rodada.

### `canyonLight`

Li `canyonLight.ts` inteiro. A passagem entrou no passe de atmosfera que já existe, como multiplicador (`keep`), sem `CompositorEffect`:
- `canyon_field.cpp`: janela de 2.048 m em células de 4 m, com a altura e o máximo a 48 m em volta, escrita em job. 1 MiB de textura.
- `canyon_light.gdshaderinc`: 8 direções e 3 passos, como no 4.0. Quem não tem nada acima de si no alcance sai logo.

Cidade inteira, luminância p5/50/95: 36/105/203 para 33/99/202, contra 25/85/190 no 4.0. Falta cerca de metade do caminho. GPU: `luz_rua` 6,68 para 7,72 ms, `luz_parede` 5,30 para 6,68 ms, `luz_cidade` 6,99 para 7,72 ms, com a máquina carregada.

Falta:
- A sombra de contato (o Godot não dá o mapa de sombra ao shader).
- A prova na tabela de paredes da Porte 2, que precisa dos prédios do `capture_pair` dela. Aqui os prédios são outros.
- O GTAO do 4.0 fica por conta do SSAO do preset alto.

### Noite

A lua do jogo vinha do lado oposto, a 38°. Agora fica fixa a 56,5°, como no 4.0. Medi nas faces do cubo às 22h, pela razão de luz do dia: `moon` `#9fb4d8`, energia 0,224 (os três canais dão o mesmo valor). Ambiente `#ecf7ff`.

| Face às 22h | 4.0 | Jogo, com ambiente 0,0762 |
|---|---|---|
| Topo | 39,48,64 | 34,44,59 |
| Parede sem lua | 18,21,23 | 15,18,20 |
| Parede ao luar | 24,29,36 | 24,30,39 |
| Chão | 10,23,8 | 8,20,5 |

Subi o ambiente para 0,092 depois dessa medida, para fechar a sombra; ainda não remedi.

### Água

A água era opaca. Agora a folha fecha com a profundidade vertical, e o leito molhado (cascalho e lodo) é desenhado no próprio shader da água. Areia `#b1a586`, antes `#C9B98F`.

Falta: as pedras da margem não foram portadas. A areia pelo nível da água entrou depois (ver "Margem, rio e grama").

### Gramado de perto

Medi em `solta_palma_perto`: o tom fica 7% abaixo do 4.0 (77,103,51 contra 84,110,55) e o grão é até maior no jogo (19,0 contra 15,6). As manchas são um pouco menores (4,6 contra 5,6). O que parece mais liso vem das manchas, não do grão.

### MultiMesh

A árvore precisa só de `custom data` (tinta e espécie); a cor é sempre branca. Dá para usar `MM_PLAIN_STRIDE` da Porte 6 depois da integração.

### Margem, rio e grama (commit 7973649)

**Rio, causa medida:** a areia larga não vinha da regra de altura. A pintura do pincel de rio (`lay_river`) cobria 1,2 vez o raio da pincelada, 150 m em cada lado. O pincel do 4.0 (`editor/terrainBrush.ts`) carimba e corta as árvores, mas não pinta areia. Trocar só a regra de altura mudou a foto em 0,28 de 255 na média; tirar a pintura resolveu.

**Regra de altura, causa lida no código:** `terrain.gdshader` comparava a altura com o nível do mar. Toda terra abaixo de 2 m ficava bege, em qualquer lugar. Agora `ShoreField` guarda, por amostra de água, o nível da água ao lado de cada terra, dois passos para fora sobre a margem (reach 19 m, grade de 8 m). `shore.gdshaderinc` desenha praia, barranco e faixa molhada por esse nível e pela inclinação. Custa uma textura R16F de 1.280 x 1.280, 3,1 MiB no mapa 0, feita em job e refeita só quando terra ou água mudam. Não medi o tempo desse job nem o custo de GPU da margem.

**Foto de referência:** `41-rio` não vale mais. Ela usava `addTerrainStamp` direto e pulava o corte das árvores do pincel. A referência certa é `07-rio_corte`, com carimbo e `cutTrees`, como o pincel faz.

Par `evidence/rio2/par.jpg`, lido inteiro, defeitos:
1. O jogo não tem as pedras da margem e do leito do 4.0 (não portadas).
2. A água do jogo tem grão fino e o rio parece mais largo. A faixa clara de cascalho na beira da água é mais estreita e mais ruidosa que a do 4.0.
3. A mancha oval e a diagonal clara no rio do 4.0 são o carimbo acumulado da malha de água. É defeito do 4.0 e não copiei.
4. A borda da areia serrilha nos dois jogos, pela grade da água (8 m aqui, 6,4 m lá).

**Grama, causa medida:** a cor das lâminas sumia a 0,064 m por pixel, e no 4.0 uma parte dela fica até 0,44. Energia de borda sobre a média, em `ceu_alto`, mesmo tom (94,118,69 contra 94,119,68):

| Trecho | 4.0 | Antes | Depois |
|---|---|---|---|
| Meio do gramado | 0,249 | 0,159 | 0,213 |
| Mais longe | 0,186 | 0,110 | 0,204 |

### Neblina, folha e mar (commits 4aaf699, d5aa565, 1d0b110)

**Neblina sumiu e voltou, causa provada:** `FogField`, `CanyonField` e `ShoreField` enviavam jobs com a mesma chave (-1). A fila descarta o pedido antigo da mesma chave, e o campo descartado ficava esperando para sempre. Com `ShoreField` entrando no primeiro quadro, a neblina nunca chegava: a mesma foto deu 75,105,60 duas vezes. Com uma chave por campo deu 196,200,193, contra 204,207,201 no 4.0. A regressão foi minha e durou do commit 7973649 ao 1d0b110; o `canyonLight` já corria o mesmo risco desde 822506a.

**Folha:** o aro e a nervura eram decididos texel a texel e, de perto, viravam dentes. Agora cada linha tem um pixel de transição, e a folha entra por cima da que já está lá. Conferi em `evidence/folha_perto.png` (antes) e `folha_perto2.png` (depois): os dentes sumiram.

**Horizonte:** o Terrain3D desenha um chão plano depois do fim do mapa, na altura do mar, e ele cobria a água. Era a faixa cinza. O chão foi para 30 m abaixo da água e o mar apareceu. Faixa em `nuvens_ceu`: 123,121,116 antes, 54,94,128 agora, contra 23,57,102 no 4.0.

### Sombra das árvores: causa medida, decisão aberta

Corrigindo o que escrevi antes: a sombra grande em `ceu_baixo` e `ceu_alto` não é de nuvem.

**O que está provado, em fotos do 4.0:** só as árvores plantadas lançam sombra no chão. As árvores nativas das matas não lançam, na mesma foto, na mesma hora e perto das plantadas (`evidence/somb_ordem.jpg`: à direita, com três árvores plantadas pelo `plan-order.json`).

**Hipóteses refutadas:**
- Hora: às 16:25 (`17-somb_meio_20s`, `14-somb_horiz_20s`) o 4.0 segue sem sombra nas matas, igual às 12:02. O modo dia fixa o sol às 16h no código (`renderer.ts`, `fixedHour`).
- Espera: nada muda em 28 s (`evidence/somb40.jpg`).
- Ordem da câmera: as árvores nativas seguem sem sombra depois das poses `arv_*`.

**Não achei a causa no código do 4.0.** `nature-forest` e `planted-forest` saem da mesma `createTreeCells`, com `castShadow: true` nos dois. A intenção escrita é lançar; o que aparece é não lançar.

**O jogo:** lança a sombra do miolo em toda árvore perto da câmera, nativa ou plantada. Em `arv_copa`, numa árvore plantada, a sombra do jogo tem a forma e o comprimento da do 4.0. Em zoom 200 (`ceu_baixo`), a sombra de um só miolo fora da foto cobre o primeiro plano; isso é dedução pela escala, não isolei a árvore.

**Se a decisão for seguir as fotos do 4.0:** as árvores nativas precisam de um MultiMesh separado das plantadas e das de rua, sem sombra. Hoje as três saem no mesmo buffer por espécie (`tree_renderer.cpp`). Não mudei nada: o custo é pequeno, mas a mudança tira sombra de todas as matas, e o código do 4.0 diz o contrário das fotos dele.

### Os pares na base integrada

Fotos `evidence/final`, `finaln` e `finalr`, em 1920x1080, depois do merge 836e34d, com `CaptureWait`. Montagens em `evidence/final/montagem`. A hora é a do modo dia e noite: o relógio do 4.0 mostra 12h, mas o modo dia dele fixa o sol às 16h (ver "Luz e relógio" no `CLAUDE.md`); `rio` foi feita com o relógio em 16h.

**Foto que mente:** quase todas saíram com `quadros=0 pendente=0` depois do salto de câmera, como a coordenadora avisou. Conferi uma a uma: toda mata do 4.0 está na foto, nenhuma saiu vazia. Só `luz_cidade` (4), `luz_rua` (37) e `noite_cidade` (33) esperaram quadros.

| Par | Defeitos que vi, lendo a foto inteira |
|---|---|
| `campo_perto` | Folha do jogo oval e clara; a do 4.0 é pontuda. Tronco mais largo e mais cônico. Sombra no gramado só no jogo. O pontilhado no canto de cima do 4.0 é defeito dele |
| `campo_meio` | Copas do jogo mais uniformes, sem o recorte escuro embaixo. Palmeiras e coníferas em lugares diferentes, uma palmeira a mais no canto. Sombras só no jogo. Chão com menos manchas amarelas |
| `matas_meio` | Mesmas manchas de mata. Copas mais escuras, sem topos claros. Mais coníferas no jogo |
| `matas_alto` | Mesmas manchas. Copas mais escuras e uniformes. Chão mais frio e azulado no alto da foto. Cidade do jogo maior (fora da minha área) |
| `horizonte` | Sombras longas só no jogo. Árvores do segundo plano em lugares diferentes. Fio claro do mar no horizonte, à direita. Chão com menos manchas |
| `chuva` | Mata distante não clareia como no 4.0. Palmeiras trocadas: seis embaixo no 4.0, quase nenhuma no jogo. Riscos de chuva mais longos e brancos no alto |
| `neblina` | Tom a 4% do 4.0. A forma do banco é parecida, com o lado esquerdo diferente. As bordas brancas nas árvores do 4.0 são defeito dele e não copiei |
| `nuvens_sombra` | Sombras de nuvem mais escuras e em outros lugares. Mar no canto de cima |
| `nuvens_ceu` | Mar mais claro que no 4.0 (54,94,128 contra 23,57,102) e só no canto; no 4.0 ele corre por toda a faixa de cima |
| `rio` | Falta a pedra na margem e no leito. Água com grão fino. Faixa de cascalho mais estreita e ruidosa. A mancha oval e a diagonal do 4.0 são defeito dele |
| `ceu_baixo`, `ceu_alto` | Céu parecido. Grande sombra no primeiro plano só no jogo. Grama de perto com menos contraste. O contorno branco de árvores no horizonte do 4.0 é defeito dele |
| `ceu_noite` | Árvores do jogo são silhueta; as do 4.0 têm pontos azulados nas bordas, que não copiei. Céu parecido |
| `noite_campo` | Chão 9,22,6 contra 11,24,12: azul abaixo do 4.0. Copas menos destacadas do chão |
| `rampa` | Serve só para tom: a rampa não caiu no mesmo lugar e tem outra largura. Palmeiras em lugares diferentes em três pontos. Copas mais escuras |
| `luz_rua`, `luz_parede`, `noite_cidade` | **Não são pares:** a câmera é a mesma, mas ruas e prédios são outros, mesmo na base integrada. Os painéis pretos de `luz_parede3` sumiram. Janelas e postes não são meus |
| `mar` | **Não é par:** o jogo tem mar nessa pose e o 4.0 tem mata. O fim da terra não coincide. No jogo a terra acaba numa reta, sem praia |

Isso dá 14 pares que valem e 4 fotos que não valem, fora `mar`.

### Base c061e52 (master c00c8f6)

Merge com o `JobKey` tipado: as três constantes `INT64_MIN` saíram, neblina usa `JobLane::FOG`, `canyonLight` e margem ganharam `CANYON` e `SHORE`. Gates, release, debug e as 13 execuções de teste passam. **Correção:** no merge 836e34d rodei só 8 das execuções, porque a lista do meu script estava velha; as 13 valem a partir de c061e52.

Refiz árvore, neblina, paisagem e noite (`evidence/final2`, `final2a`, `final2n`; montagens em `final2/montagem`). Em toda foto: `pendente=0 pedir=0 sem_vez=0 substituidos=0`. Contra a rodada anterior, a diferença média foi de até 0,27 de 255, fora `chuva` (2,29) e `neblina` (1,54), que têm movimento. A tabela acima vale para esta base. Li de novo `chuva` e `neblina`: mesmos defeitos. Neblina 196,200,193 contra 204,207,201, GPU 10,61 ms.

Pares `arv_*`, às 16h, lidos inteiros:

| Par | Copas 4.0 / jogo (`coverage.py`) | Defeitos que vi |
|---|---|---|
| `arv_perto` | 0,323 / 0,338 | Uma palmeira a mais à esquerda. Folhas ovais, as do 4.0 pontudas |
| `arv_copa` | 0,197 / 0,221 | Árvore plantada do centro uns 150 px à direita do 4.0 e mais larga, causa não medida. Tronco com nervuras e base aberta, o do 4.0 é reto. Palmeira a mais à esquerda |
| `arv_media` | 0,148 / 0,174 | Sombras do jogo mais suaves. Palmeira a mais perto da mata de cima |
| `arv_longe` | 0,049 / 0,052 | Copas mais escuras e uniformes |
| `arv_alto` | 0,041 / 0,043 | Copas mais escuras. Chão mais frio no alto |

### Medidas na base integrada

- GPU, preset médio, 1920x1080, máquina em uso: de 3,55 ms (`campo_perto`) a 9,78 ms (`neblina`).
- `luz_cidade`, luminância p5/50/95: 32/99/192, contra 24/83/188 no 4.0.
- A prova do `canyonLight` na tabela de paredes da Porte 2 não foi feita. Nada disso prova paridade do `canyonLight`.
- O filtro de sombra, a proposta de preset e os demais tempos de GPU desta seção foram medidos antes do merge.

### Abertos, por ordem

1. Sombra das árvores nativas: decisão aberta. O 4.0 não mostra, o jogo mostra.
2. `canyonLight`: falta metade do caminho, a sombra de contato e a prova.
3. Fim da terra e tom do mar.
4. Espécies e cobertura distante (19% a 26% acima): a ecologia ainda não é a do 4.0 célula a célula.
5. Copas mais escuras e uniformes de longe; mata distante na chuva.
6. Forma da folha, tronco largo.
7. Pedras da margem e do leito.
8. Azul do chão à noite (9,22,6 contra 11,24,12).

### Rodada do mesmo mundo (commit 719f98c)

**Mesmo mundo:** o terreno e as matas já eram os mesmos (semente 7 nos dois). Para a cidade, usei `capture_pair` da Porte 2 com `--views=vistas-pose.json` (pose lida de `viewCamera`, lente 35°) e `--buildings=predios-40.json` (429 de 441 prédios do 4.0; 12 recusados por tamanho). **Limites:** as ruas do 4.0 não vêm junto, então o jogo tem gramado onde o 4.0 tem rua e calçada; e o chão difere, então os prédios ficam deslocados na foto. Por isso a tabela abaixo compara o mesmo prédio em recortes diferentes, e a foto inteira não vale para rua nem chão.

**Nível do 4.0:** `tierForGpu` dá `ultra` para a RTX 3060 (`core/gpuTier.ts`), com oclusão e SMAA. O par justo é o preset alto; o médio não tem oclusão nem SMAA.

**Causa nova, achada no par:** o 4.0 lança sombra de prédio sobre prédio e de peça sobre telhado, e o jogo quase não lançava. O viés do Godot é `shadow_bias / 100 x profundidade da cascata x max(blur x raio do filtro, 1)` (`light_storage.cpp`, `renderer_scene_cull.cpp`). Com 0,1 fixo, numa vista da cidade o viés crescia com a cascata. Agora ele é calculado da primeira cascata.

**Duas ressalvas sobre esse commit:**
- A mensagem diz "2 a 7 m" e "4 cm". São deduções. Usei `first` como profundidade, mas o Godot usa `z_max - z_min` no espaço da luz, que é maior. O viés real é mais de 4 cm e não foi medido.
- `shadow_filter_scale` 2,4 vale para Soft Ultra. O preset baixo tem outro filtro.

| Recorte | 4.0 | Antes (médio) | Médio | Alto |
|---|---|---|---|---|
| Laranja | 177,90,47 | 189,93,49 | 156,73,40 | 174,86,46 |
| Bege ao sol | 180,165,136 | 178,165,143 | 175,163,140 | 176,164,141 |
| Bege na sombra | 111,103,80 | 164,151,126 | 167,154,128 | 153,142,118 |
| Azul frente | 30,48,65 | 101,123,144 | 91,113,135 | 61,82,104 |
| Azul vão | 51,76,101 | 88,110,132 | 77,99,121 | 40,60,83 |
| Telhado central | 58,53,44 | 108,110,103 | 91,92,89 | 91,92,89 |

Foto `p3-torres`, luminância p5/25/50/75/95: 4.0 25/55/75/133/182; médio 28/65/101/145/188; alto 28/62/96/143/188. Antes: 28/66/108/148/192.

Isso não fecha a tabela. Os recortes são aproximados. O telhado é outro material (bege escuro no 4.0, cinza no jogo), da Porte 2. O p50 segue 21 acima. O `canyonLight` sozinho, sem ele contra com ele: p50 112 para 108.

**Testes de valor:** desvio pela normal 1 cria trama nos telhados e tira 11 de uma parede ao sol; 4,7 tira as sombras das paredes; 2,5 foi o que ficou. **Sombra das peças:** liguei de teste e nada mudou nas paredes; a sombra das peças no telhado do 4.0 não veio disso, e o arquivo é da Porte 2. Desfiz.

**Hemisfério:** entrou a luz negativa de baixo (`ground_under`). **Não está provada:** nenhum recorte mudou, nenhum recorte é superfície virada para baixo, e não refiz o cubo.

### Custo de oclusão e SMAA

`bash cap4.sh <pasta> campo_meio,matas_meio,horizonte,luz_cidade,luz_rua,luz_parede 16 <preset>`, 1920x1080, média de 30 quadros, GPU medida pelo Godot. **Máquina carregada:** o `nvidia-smi` deu 80% no fim da rodada, então os números valem como ordem de grandeza e o pedido de máquina quieta não foi cumprido. Medido antes de 719f98c. Contagens não medidas.

| Vista | Médio | Alto | Custo do SMAA | Custo da oclusão |
|---|---|---|---|---|
| `campo_meio` | 7,77 | 11,73 | 1,30 | 2,31 |
| `matas_meio` | 6,37 | 10,78 | 1,07 | 2,05 |
| `horizonte` | 7,39 | 11,98 | 1,24 | 2,46 |
| `luz_cidade` | 15,19 | 21,19 | 0,30 | 2,57 |
| `luz_rua` | 12,94 | 18,85 | 1,48 | 2,83 |
| `luz_parede` | 13,25 | 20,28 | 1,17 | 4,67 |

Nessa rodada o alto passou de 16,6 ms na cidade. Não repito a proposta de padrão alto até medir com a máquina quieta; a medida antiga era de antes do `canyonLight`.

### Pares de paisagem nos dois presets

`evidence/fin3_m` (médio) e `fin3_h` (alto), 16h nos dois. Luminância p5/50/95, foto inteira sem a interface. Lidas depois: ver "Fotos `fin3`".

| Foto | 4.0 | Médio | Alto |
|---|---|---|---|
| `campo_perto` | 14/69/138 | 16/75/149 | 17/75/149 |
| `campo_meio` | 37/99/117 | 32/97/114 | 33/97/114 |
| `matas_meio` | 45/95/104 | 35/94/102 | 36/94/102 |
| `matas_alto` | 62/92/99 | 63/92/98 | 63/92/98 |
| `horizonte` | 22/110/148 | 21/112/165 | 21/112/165 |
| `chuva` | 49/90/121 | 29/75/100 | 29/75/98 |
| `nuvens_sombra` | 51/93/104 | 40/94/102 | 43/94/102 |
| `nuvens_ceu` | 36/96/108 | 31/96/106 | 32/96/106 |
| `neblina` | 88/162/209 | 82/157/207 | 83/157/207 |
| `rio` | 72/99/163 | 74/97/156 | 74/97/156 |
| `ceu_baixo` | 54/114/196 | 35/113/197 | 35/112/197 |
| `ceu_alto` | 53/114/196 | 35/112/197 | 34/112/197 |

`chuva` é o defeito maior: p50 15 abaixo. O p5 de `ceu_baixo` e `ceu_alto` é a sombra da árvore no primeiro plano.

**`auto`:** com a pasta própria, `capture_clock` (cena `play`) e as demais capturas deram 0 linhas de erro, e não há `settings.cfg` em `Roadcraft-landscape`.

**Teste da ordem da câmera:** `plan-order.json` passou pelas poses `arv_*` e voltou a `campo_meio` e `horizonte`. As árvores nativas seguem sem sombra e só as três plantadas lançam (`evidence/somb_ordem.jpg`). A ordem não é a causa.

### Os 10 defeitos: onde estão

| Defeito | Estado |
|---|---|
| `canyonLight` | Pela metade. Sombra de contato não feita; tabela de paredes medida acima, não fechada |
| Chuva mais escura | Causa provada e corrigida; falta a cor da névoa |
| Pedras da margem e do leito | Não feito |
| Grama de perto a 7% | Não feito nesta rodada |
| Copas de longe | Não feito. No par de agora elas estão mais escuras, não mais claras como o item dizia |
| Hemisfério sem cor de chão | Código entrou, sem prova |
| Ar sem integração por raio | Não feito |
| Nitidez CAS | Sem diferença a portar: `quality.ts:132`, `:165`, `:200`, `:241` (em `coverage.md`) |
| Tabela de RGB por superfície | Paredes acima; falta o resto |
| 1,2x fora do terreno | Não medido |

### Cascatas de sombra (commit 1c46296)

**Causa provada, a maior da rodada:** `_fit_cascades` lia `-camera.global_basis.z.y`. A câmera do Godot olha para `-Z`, então isso é negativo olhando para baixo, e a conta caía sempre no piso de 0,05. Prova pelo número: altura 97,6 m / 0,05 = 1.952; x3 = 5.856; x0,34 = 1.991, exatamente o fundo da primeira cascata medido em `lam2`. A vista olhava 285 m à frente. Com o sinal certo a primeira cascata tem 428 m (`lam3`). Toda vista inclinada tinha um texel várias vezes maior que o devido, desde o commit que trouxe `_fit_cascades`. Isso **invalida** a conclusão anterior sobre o viés: a sombra de prédio sobre prédio sumia em boa parte pelo texel.

**Viés:** versão em uso 4.7.2.stable.official.ed1daf0bf; conferi a tag `4.7.2-stable`, não o master. Nessa tag o viés é `shadow_bias / 100 x bias_scale x soft_shadow_scale`, sem o `max(..., 1)` do master, com `bias_scale = 2 radius + pancake`. `_first_depth` calcula o `radius` da primeira cascata pela lente, aspecto, near e split, e refaz quando muda. O lado da cascata é metade do mapa em 4 splits (`get_directional_light_shadow_size`). `shadow_pancake_m` 20 virou dado. Os "2 a 7 m / 4 cm" de 719f98c eram deduções erradas e foram retirados.

**Medida por cascata** (`lam3`, preset alto, poste e laje no meio de cada cascata; `lam1` e `lam2` não valem: posição errada e cascata errada):

| Cascata | Profundidade | Vão entre o pé e a sombra | Metro por pixel | Largura do poste |
|---|---|---|---|---|
| 0 | 214 m | 0,55 m | 0,069 | 1,7 m |
| 1 | 731 m | 0,71 m | 0,237 | 5,9 m |
| 2 | 1.321 m | 2,14 m | 0,429 | 10,6 m |
| 3 | 2.054 m | Não medido | 0,666 | Poste na borda de cima da foto |

Essa medida só limita o vazamento por cima: o poste é mais largo que o vão, então vazamento menor que meia largura não aparece. Não vi acne nas lajes da foto. A coluna de trama do script pega a borda da laje e não vale. O desvio 2,5 foi conferido depois da correção só em 2,5.

**Paredes, preset alto** (`par3_h` antes, `par4_h` depois, recortes aproximados):

| Recorte | 4.0 | Antes | Depois |
|---|---|---|---|
| Laranja | 177,90,47 | 174,86,46 | 190,95,50 |
| Bege ao sol | 180,165,136 | 176,164,141 | 175,163,140 |
| Bege na sombra | 111,103,80 | 153,142,118 | 123,117,100 |
| Azul frente | 30,48,65 | 61,82,104 | 37,57,79 |
| Telhado | 58,53,44 | 91,92,89 | 94,95,94 |

p5/25/50/75/95 da foto: 4.0 25/55/75/133/182; agora 21/58/95/144/188. Os telhados agora têm as sombras dos equipamentos, como no 4.0 (`par4_h/telhados.png`). O recorte laranja subiu e não sei por quê; o p50 segue 20 acima, e o jogo tem gramado onde o 4.0 tem rua. O telhado é material da Porte 2.

### Item 2: chuva e p5

**Chuva, causa provada:** o 4.0 engrossa o ar com a chuva (`renderer.ts:3934`, `fog = rain x 0,2`, altura 120) e o jogo não. Portei a regra (`rain_mist`, `rain_mist_height_m`). Chuva p5/50/95: 4.0 49/90/121; antes 29/75/98; agora 45/93/126. Chão: 76,96,100 contra 90,118,121 no alto da foto e 64,89,62 contra 61,92,66 embaixo. A névoa do jogo está clara demais longe; a cor dela ainda não foi medida.

**p5 de `ceu_baixo` e `ceu_alto`, provado:** é o mesmo mundo e a causa é a sombra. A árvore solta à direita e o bosque à esquerda estão no mesmo lugar nos dois. Sem a sombra das árvores (teste `semsombra`, desfeito) o p5 vai de 35 para 54, contra 54 no 4.0; e `ceu_alto` de 34 para 54, contra 53. Com a cascata certa a sombra ficou nítida e forte (`fin4_h/ceu_baixo.png`), então a decisão sobre a sombra das árvores nativas pesa mais. Em `campo_meio` o p5 não muda.

### Fotos `fin3`, lidas em resolução cheia

Li as 12 de cada preset, uma a uma, contra o 4.0. Elas são de antes da correção das cascatas, então a sombra nelas está velha. `fin4_h` já saiu e só li duas (`campo_meio`, `ceu_baixo`); `fin4_m` não saiu. Entre médio e alto as diferenças vistas são bordas de folha e penumbra; o tom é igual pelos percentis.

| Foto | Defeitos em resolução cheia |
|---|---|
| `chuva` | Sem névoa (corrigido). Riscos longos e brancos. Palmeiras trocadas |
| `ceu_baixo`, `ceu_alto` | Sombra no primeiro plano só no jogo. Tronco largo e cônico. Copas mais claras; o 4.0 tem copas bem escuras |
| `campo_perto` | Folha oval, tronco largo |
| `campo_meio` | Copas uniformes, chão mais liso, sem manchas amarelas |
| `matas_meio`, `matas_alto` | Mais coníferas; copas uniformes; verde do chão mais saturado no alto |
| `horizonte` | Tronco cônico, topo branco das copas maior que no 4.0, sombras longas só no jogo. Copas distantes em outro lugar à direita |
| `nuvens_sombra` | Sombras de nuvem em outros lugares e mais escuras; mar no canto de cima só no jogo |
| `nuvens_ceu` | Sombra de nuvem à esquerda muito mais forte. Mar só no canto |
| `neblina` | Trama diagonal fina sobre a neblina, nos dois presets (chamei de defeito novo; o 4.0 também tem, ver "Trama da neblina"). Banco com borda mais definida à esquerda. Árvores sem o contorno branco do 4.0, que é defeito dele |
| `rio` | Sem pedras. Água granulada. Sem a mancha do carimbo do 4.0, que é defeito dele |

### Custo de GPU

`cap4.sh` agora grava carga antes e depois, em `carga.log`, e marca `INVALIDA` com outro Godot ou compilação rodando, ou GPU acima de 15%. **Todas as rodadas com esse registro saíram INVALIDA**, então nenhum tempo deste relatório serve para decidir preset. O registro é só antes e depois da rodada, não de cada vista como foi pedido; falta isso.

### Trama da neblina e cor do ar (commits 7f72136 e d35b90d)

**Correção:** escrevi que a trama diagonal era defeito novo do jogo. Não era. O 4.0 tem o mesmo pico de frequência na foto (0,445; 0,312 cycles/px, com o eixo vertical invertido), com 65% da amplitude do jogo: ele marcha 36 passos e o jogo marchava 24.

**Trama, causa provada:** o IGN no início de cada raio, pico calculado em 0,441; -0,309 e medido na foto em 0,445; -0,312. O passe não tira média de quadros, então o padrão fica na tela. Agora o início vem de um ladrilho de ruído azul de 64 px, feito por void and cluster (`tools/assets/make_blue_noise.py`), e a neblina marcha 36 passos. Pico de 2.856 para 331 com ruído azul e 24 passos; com 36 passos a maior frequência da foto já é a forma do banco (período de 16 px). Desvio fino 2,10 para 1,41 (4.0 1,48). Isso melhora o 4.0 num defeito dele. Recortes em `azul_m/trama_antes_depois_4.0.png`. **O custo dos 36 passos não foi medido.**

**Cor do ar:** o jogo usava a cor do véu de distância na névoa, sem o tempo fechado. O 4.0 usa o horizonte do céu já fechado (`renderer.ts:3940`, `environment.ts:269`), na névoa e na luz das nuvens. Portei a regra: `sky_view.gd` lê o `horizon` do céu e passa para linear. Corrige também o espaço de cor: o valor ia sem conversão.

A névoa antiga, azul e forte, escondia que o chão do jogo já é menos azul que o do 4.0 sem chuva. Razão jogo / 4.0 por canal:

| Faixa | Sem chuva (`campo_meio`) | Chuva antes | Chuva agora |
|---|---|---|---|
| y 100-240 | 0,95 0,97 0,90 | 0,99 1,04 1,03 | 0,92 0,93 0,84 |
| y 240-480 | 0,95 0,98 0,93 | 0,98 1,04 1,05 | 0,93 0,95 0,86 |
| y 480-640 | 0,93 0,98 0,95 | 0,97 1,03 1,07 | 0,93 0,96 0,89 |

Agora a chuva fica mais perto da base nos três canais, mas abaixo dela: até 4 pontos em R e G e 7 em B. p5/50/95: 4.0 44/86/118, antes da regra 45/93/126, agora 35/81/113. **A olho, o longe do 4.0 é mais azul-esverdeado que o jogo agora.** Causa do resto não provada. Candidata: o jogo mistura o cinza em sRGB, com outro cinza e a mesma parte em zênite e horizonte; o 4.0 mistura em linear, com 0,7 no horizonte. Isso daria 4% em G e 6% em B na cor da névoa, não os pontos todos. A cor nova em `sky_light` muda o corpo das nuvens, e nenhuma foto mostra corpo de nuvem: **não verificado**. Conferi o ganho: 0,89 / (4,2 / pi) = 0,666, que bate com `sky_gain` 0,661 no nível do chão (a regra de pi vem da memória, não abri a fonte).

Os riscos da chuva, que chamei de "longos e brancos", medem quase igual: cobertura 12,14% contra 12,23%, contraste p50 23 contra 21, largura 1,63 contra 1,35 px. Não é defeito de cor. A inclinação estimada difere (0,90 contra 0,70), numa grade grosseira.

**Viés, pontos da coordenadora:** `MAX(soft_shadow_scale, 1.0)` está no `master`, não na tag `4.7.2-stable` em uso, conferi as duas; não pus no divisor, pois erraria nos filtros 0 e 1. O `_doc` avisa para rever isso ao atualizar o Godot. `shadow_depth_max_m` 8.000 é menor que `far` 30.000 (`camera.json`), então `MIN(shadow_max, z_far)` nunca corta. Depois apliquei também o `minf`, ver "Viés medido por escada de placas".

### Fotos `fin5`, lidas em resolução cheia

Li as 12 de cada preset, com as cascatas certas, no estado do commit d35b90d. Entre médio e alto as fotos quase não mudam: p5/50/95 diferem no máximo 2 pontos. No alto a borda da sombra é mais nítida.

| Vista | 4.0 | Médio | Alto |
|---|---|---|---|
| `campo_meio` | 35/95/113 | 30/92/109 | 31/92/109 |
| `campo_perto` | 13/66/130 | 15/70/140 | 16/70/140 |
| `ceu_alto` | 48/108/193 | 31/105/194 | 31/105/194 |
| `ceu_baixo` | 49/109/193 | 32/106/194 | 31/105/194 |
| `chuva` | 44/86/118 | 35/81/113 | 36/81/112 |
| `horizonte` | 22/105/145 | 20/106/162 | 20/106/162 |
| `matas_alto` | 60/88/95 | 62/87/93 | 62/87/93 |
| `matas_meio` | 42/90/100 | 32/89/98 | 33/89/98 |
| `neblina` | 83/156/209 | 76/154/206 | 76/154/207 |
| `nuvens_ceu` | 35/92/104 | 29/91/102 | 30/91/102 |
| `nuvens_sombra` | 49/88/101 | 38/90/97 | 40/90/97 |
| `rio` (referência `41-rio`, inválida) | 61/95/158 | 69/93/155 | 68/93/155 |

Essas referências têm a barra do 4.0 e foram cortadas igual. A sombra das árvores explica o p5 baixo de `ceu_baixo` e `ceu_alto` (teste `semsombra`); nas outras vistas não foi provado.

| Vista | Defeitos |
|---|---|
| `campo_perto` | Folha oval e serrilhada na borda; o 4.0 tem folha pontuda com nervura. Tronco largo e cônico. Faixas de sombra no chão só no jogo. O pontilhado no 4.0 é defeito dele |
| `campo_meio` | Sombras de árvore a conferir contra `09-campo_meio`. Copas uniformes |
| `ceu_baixo`, `ceu_alto` | Sombra grande no primeiro plano só no jogo. Tronco cônico |
| `horizonte` | Topo branco das copas maior que no 4.0, p95 162 contra 145. Sombras longas só no jogo |
| `matas_meio`, `matas_alto` | Copas uniformes. Verde mais saturado |
| `chuva` | Longe menos azul que no 4.0. Sombras fracas sob as árvores |
| `nuvens_*` | Sombra de nuvem em outro lugar e mais escura |
| `neblina` | Trama resolvida; sobra grão fino sem direção, no nível do 4.0. Banco com borda mais definida à esquerda |
| `rio` | Sem pedras. Margem com grão onde o 4.0 tem faixa rasa lisa, e água mais clara. A borda em dentes entre areia e grama existe nos dois |

### Carga por vista

`cap4.sh` usa `carga.ps1`: a cada foto nova registra outros Godot, compilações, 3D dos outros e 3D desta rodada, pelo contador `GPUEngine` por processo. Uma foto fecha uma vista e abre a seguinte. O registro atrasa alguns segundos: no fim de `fin5_h` a amostra já não acha o jogo. Toda a `fin5` saiu INVALIDA, com outro Godot e até 83% de 3D dos outros. A primeira versão de `carga.ps1` contava o próprio jogo como outro; `fin4_m` tem esse erro e não vale como registro de carga. Sigo sem número de custo.

### Viés medido por escada de placas (commit d2aaaaf)

**`camera.far`:** apliquei `minf(end, camera.far)` em `_first_depth`, como no motor (`renderer_scene_cull.cpp` L2158 da tag). Com `shadow_depth_max_m` 8.000 e `far` 30.000 o valor não muda hoje; a conta passa a valer para qualquer câmera.

**Método:** o poste de `lam3` só limitava o vazamento por cima. Agora há, em cada cascata, oito placas que só lançam sombra (`SHADOW_CASTING_SETTING_SHADOWS_ONLY`), em alturas que dobram de 1/8 a 16 texels. Como a placa não aparece, a foto mostra só a marca. O script mede a razão entre a foto com placas e a vazia, no meio da marca. A cascata 3 foi lida desta vez. Fotos em `esc1_m`, `esc1_h`, `esc_n*`, `esc_c*`.

Altura da placa no ponto de meia força da marca, com o desvio normal 2,5 (leitura entre degraus, cerca de 2 texels nas quatro):

| Cascata | Médio: profundidade, texel, altura | Alto: profundidade, texel, altura |
|---|---|---|
| 0 | 204 m, 0,33 m, 0,6 m | 214 m, 0,17 m, 0,3 m |
| 1 | 554 m, 0,46 m, 0,9 m | 731 m, 0,36 m, 0,7 m |
| 2 | 808 m, 0,59 m, 1,2 m | 1.321 m, 0,52 m, 1,0 m |
| 3 | 1.058 m, 0,77 m, 1,5 m | 2.054 m, 0,82 m, 2 m |

**Isso é o vazamento real, e é maior que os 4 cm do dado:** quem define a altura é o desvio normal e o filtro, não `shadow_bias_m`. Um objeto mais baixo que isso quase não marca o chão. Força total só vem em cerca de 4 texels: 0,7 m no alto e 1,3 m no médio, na cascata 0. O texel usado nesta tabela é estimado pela esfera da fatia.

**Acne:** rodei 2,5, 1,25 e 0,6 nos dois presets. Em 1,25 e 0,6 o bloco cinza mostra trama na face de cima e na face ao sol; em 2,5 não (`esc1_h/blocos.png`). Variação fina no bloco perto, alto: 4,87, 6,71, 7,03. Em 1,25 a altura de meia força cai para cerca de 1 texel. **Fica 2,5:** é o menor valor testado sem acne no bloco. Não testei entre 1,25 e 2,5, nem paredes de prédio. O chão e as árvores não mostraram acne em nenhum valor (`esc_c0.6_medium`).

**Não explicado:** o patamar das placas altas muda com o desvio normal (cascata 0: 0,61, 0,55, 0,53), mas a sombra de corpos reais não muda: a do bloco fica em 51,3 nos três valores. Parece efeito da placa fina; não provei. Em `campo_meio` a sombra de um trecho clareou de 52,8 para 56,5 com o desvio menor, sem causa provada. As placas 6 e 7 da cascata 2 no alto caíram sobre uma mata e não valem. Na cascata 3 a marca é fraca (0,87 a 0,92) porque a sombra já está sumindo pela distância; a leitura lá é menos precisa.

### Memória de vídeo em `parada` (commit 0b4f29a)

0b4f29a saiu com o gate de tamanho falhando (`_run` com 62 linhas); a699a30 corrige. Entre os dois só muda a captura de teste; os 13 testes rodaram antes dessa extração e não foram repetidos depois dela.

**Resultado:** vídeo 483 para 465 MiB, texturas 294,7 para 261,4 MiB, buffers iguais (155,7), `camera_pan --moves=parada` a 1600x900, preset médio, máquina carregada. **Não voltou ao número de antes:** os 403 da Porte 2 seguem 62 MiB abaixo, e os ~70 MiB **não estão fechados item a item.**

| O que | Antes | Depois |
|---|---|---|
| Soma das texturas importadas, carregadas sozinhas | 60,9 MiB | 27,6 MiB |
| `Leaves.png` | 16,00 | 5,34 |
| `Rocks_Diffuse.png` | 16,00 | 2,67 |
| `Mushrooms.png`, `PathRocks_Diffuse.png` | 4,00 cada | 0,67 cada |
| `Flowers.png` | 3,8 pela conta (a sonda deu 1,98, é a primeira da lista e não é confiável) | não confiável (-0,64) |

**O que a medida não explica:**
- As texturas caíram 33 MiB e o vídeo só 18. A diferença entre vídeo e texturas mais buffers subiu de 32 para 48 MiB, e não sei por quê.
- A vigília por quadro não separa etapas: 290 MiB aparecem de uma vez no quadro 2, então o pedido "antes e depois de cada etapa de carga" não deu resposta. `texture_debug_usage` não existe no script. Falta bisecionar os commits ou medir na base `a11384a`; não fiz.
- Pela conta (não medida), o que entrou depois da base em texturas de tempo de execução: desgaste 8, retratos das árvores cerca de 5, atlas de folhas +1,3, neblina 2 (só com neblina pintada), `canyon` 1, tabela da gradação 1, ruído 3D cerca de 1. Soma perto de 19 MiB. Com as importadas, os 76 podem fechar, mas isso é hipótese.

**O que fiz diferente do pedido:**
- `grass.detail.png` e `grass.detail-normal.png` **seguem sem mipmaps e sem compressão.** Com mipmaps, o detalhe fino da grama mudou. Detalhe fino (4.0 / antes / com mipmaps): `campo_meio` 13,2 / 33,4 / 11,3; `campo_perto` 30,7 / 37,9 / 18,5; `ceu_baixo` 28,9 / 29,8 / 18,7. Melhora uma vista e piora duas. Os `detail_*` de `palette.json` foram ajustados sobre a textura sem mipmaps, então a correção completa é importar com mipmaps e reajustar esses dados contra o 4.0. Custa só 1,3 MiB, mas o shader pede mipmaps que a textura não tem.
- **O cintilado de longe não melhorou:** o ganho medido (5,85 para 5,05 na faixa de baixo) vinha de `grass.detail`, que voltou. Este é o pedido que segue aberto.
- `Grass.png` voltou ao original: com mipmaps o capim ficava pálido de longe (97,133,42 para 126,145,88), porque ele é opaco e a textura é quase branca fora das folhas. Custa 1 MiB.
- `linear_mips.cpp` não se aplica: ele roda em imagem montada no jogo, e aqui a imagem é importada. Resolvi na fonte com `tools/assets/bleed_alpha.py`, que preenche a cor sob os furos de `Leaves.png` e `Flowers.png` sem tocar no alfa. Sem isso as plantas ficavam brancas de longe (`tex_depois`).

**Perda achada e corrigida:** com mipmaps, o quadrado de folhas no chão sumia de longe (364 para 133 pixels): o alfa médio de `Leaves.png` é 0,437, abaixo do corte de 0,5. `element_models.cpp` agora usa alpha hash. Voltou a 410 pixels.

**Fotos (`tex_antes`, `tex3_depois`, recorte `longe_antes_depois.png`):** paisagem sem elementos igual (diferença média até 0,12). De perto os elementos não mostraram perda. **De longe as cores mudaram:** flores amarelas e laranja viraram salmão, as folhas no chão perderam a variedade de cor, e os trevos mudaram de tom (102,134,32 para 79,121,41). Pode ser só a média correta do que o antes mostrava por serrilhado, mas **não provei e não há par com o 4.0 nessa vista.** Falta tirar esse par. O código do 4.0 que li não desliga mipmaps nesses modelos, mas não conferi o carregador.

### Os 62 MiB, item a item (commit 02de1bc)

**Número final, `camera_pan --moves=parada`, 1600x900, preset médio:** base `a11384a` 547 MiB, ponta **437 MiB**. A base tem mais vídeo porque seus buffers são 302 MiB; a ponta tem 156. A comparação justa com os 403 da Porte 2 é sem os buffers: 403 menos 156 dá 247, e a base sem buffers dá 245. Isso supõe que a Porte 2 tinha 156 de buffers, o que **não confirmei com ela.** Por essa conta, a ponta está 34 MiB acima dos 403. Tudo por sonda, na pasta `ports/landscape/base`, já removida; registros em `evidence/mem_bis/`.

**Curva pelos commits (MiB):**

| Commit | Texturas | Buffers | Restante | Total |
|---|---|---|---|---|
| `a11384a` base | 223,4 | 302,2 | 21,5 | 547,1 |
| `1c604b4` render e céu | 230,0 | 313,9 | 22,5 | 566,4 |
| `5d12a18` cobertura e elementos | 277,3 | 317,4 | 30,1 | 624,8 |
| `9cc9ec7` luz global | 278,3 | 319,4 | 32,1 | 629,8 |
| `5bc31ba` água e noite | 282,6 | 308,4 | 32,2 | 623,2 |
| `836e34d` merge do master | 294,7 | 160,1 | 32,2 | 487,0 |
| `c061e52` merge com JobKey | 294,7 | 155,7 | 32,2 | 482,6 |
| `0b4f29a` comprimidas | 261,4 | 155,7 | 48,2 | 465,3 |
| 02de1bc `Leaves` em 1024 | 257,4 | 155,7 | 24,2 | 437,3 |

**Por etapa de carga, ponta `a699a30` contra a base, sonda dentro do quadro:**

| Etapa | Texturas | Restante |
|---|---|---|
| Ambiente (tabela da gradação) | +1,0 | |
| Terreno e céu | +3,3 | |
| Árvores (atlas e retratos) | +9,6 | |
| Cobertura e elementos, com `Leaves` em 2048 | +11,8 | +24,0 |
| Desgaste | +8,0 | |
| Terceiro quadro | +4,1 | +3,0 |
| **Soma** | **+37,8** (medido +38,0) | **+27,0** (medido +26,9) |

Com `Leaves` em 1024: cobertura cerca de +7,8 e restante +2,9. O terceiro quadro (+4,1) não abri: são campos criados no primeiro uso, mas não medi qual.

**O restante é staging do motor, provado:** na tag, `MEMORY_TOTAL` é tudo o que a VMA alocou (`get_total_memory_used`). Cada trabalhador de transferência mantém um buffer de `next_power_of_2(maior transferência)` e só o libera no fim do programa (`_acquire_transfer_worker`, `_free_transfer_workers`). Por elemento, só `Leaves.png` move esse número: +24,0 ao carregar. Carregada sozinha, ela leva o restante de 9 para 33, e `Rocks_Diffuse`, de 3 para 9. Não é o formato: com S3TC no lugar de BPTC o salto continuou. O salto é maior do que o tamanho da textura explica (5,33 MiB não pediria 32); **não provei por quê.** Não é o atlas de sombra nem alvo de render: esses aparecem nas duas pontas iguais, nos quadros 1 e 2 (+84,4 e +118,4), e não são SSAO, que está desligado no médio.

**Correção:** `Leaves.png` limitada a 1024 no importador (`process/size_limit`). Vídeo 465 para 437, restante 48,2 para 24,2, texturas -4,0. Foto `tex5/2048_1024.png`: de perto e de longe não vi diferença.

**O que sobra sobre a base, tudo funcionalidade nova:** árvores 9,6; desgaste 8,0; elementos cerca de 7,8; terceiro quadro 4,1; terreno e céu 3,3; gradação 1,0; staging 2,9. Não cortei isso porque cada item é o que o porte acrescentou. A regra "memória nunca acima de antes" segue quebrada nesses 34 MiB, e quem decide se aceita é a coordenadora.

**Atenção à regra geral:** qualquer textura grande carregada depois aumenta o staging para sempre. Vale para quem trouxer textura de 2048 com mipmaps.

### Pedras da margem e estado dos defeitos (commit d147122)

**Pedras, feito:** malha de 3 m ao redor da água, cada pedra mantida pela altura sobre o nível da água ao lado, nos dois presets. Regra e faixas em `cover.json shore`, escolha de modelagem contra o frame `07-rio_corte`. Alcance 1.300 m em desenhos próprios, porque a cobertura para em 250 m. Chunks perto da água são refeitos quando ela muda. Fotos `pedra2/rio.png`, `pedra2_h/rio.png`. Comparado com o 4.0: mesma distribuição geral. **Diferença:** as pedras do jogo são mais claras, as do 4.0 têm tons escuros e marrons. A margem continua com grão e a faixa rasa lisa do 4.0 não existe. Custo em GPU não medido: 7,0 para 8,5 ms em `rio` com a máquina carregada, número sem valor de decisão.

**Estado de cada defeito da lista:**

| Defeito | Estado |
|---|---|
| Pedras da margem e do leito | Feito, cor mais clara que no 4.0 |
| Chuva | Névoa e cor do ar feitas. Longe ainda menos azul (razão B 0,86 contra base 0,93). Conferi a mistura do céu: a diferença para o 4.0 é de cerca de 2% na cor da névoa, não explica. **Causa do resto não provada** |
| Copas de longe | Medido: o topo pálido existe nos dois, cor 159,171,142 contra 152,167,144; área 36% contra 21% dos pixels claros numa faixa que inclui chão. Não é defeito de cor. A diferença de área não foi explicada |
| Hemisfério com prova no cubo | **Não feito.** O cubo não mostra face de baixo; falta a laje vista de baixo nos dois jogos. `ground_under` segue sem prova, e o `_doc` diz isso |
| Grama de perto | **Não feito.** `grass.detail` sem mipmaps; a correção é importar com mipmaps e reajustar `detail_*`. Cintilado igual |
| Sombra de contato do `canyonLight` | **Não feito, com razão de engenharia.** O 4.0 lê o mapa de sombra para não escurecer o que já está na sombra (`sunReaches`). O passe de tela do Godot não enxerga o atlas de sombra; portar sem isso escurece duas vezes a sombra. Precisa de outro método, que não estudei |
| Sombra das árvores nativas | Decisão aberta do usuário |
| Cor dos elementos de longe | Sem par com o 4.0 |
| Custo de GPU | Nada válido: máquina sempre carregada |

## Seção 11: ordem do usuário

Estado no commit 34ce206. Mesma câmera, mesmo sol (16h) e 1920x1080 nos dois jogos, tamanhos conferidos por script. Montagens em `evidence/luz2/`, 4.0 à esquerda.

### Item 3, terreno e luz: em parte

**Causa 1, provada:** o nível distante dos prédios não lançava sombra (`building_renderer_commit.cpp`), e ele começa a 1.100 m. Na vista da cidade inteira nada projetava sombra no chão. É o maior defeito desta vista, e não é oclusão: o preset alto, com SSAO, quase não mudava a imagem.

**Causa 2, provada:** ambiente azul demais e fraco, medido num cubo cinza de albedo 0,216 nos dois jogos.

| Face | 4.0 | Antes | Agora |
|---|---|---|---|
| Topo | 161,162,164 | 147,151,155 | 163,163,164 |
| Parede ao sol | 163,162,156 | 154,156,157 | 165,163,159 |
| Parede à sombra, com preenchimento | 82,92,99 | 71,88,107 | 79,90,100 |
| Parede à sombra, sem preenchimento | 52,65,75 | 35,56,82 | 50,64,77 |
| Sombra no chão | 32,56,31 | 21,46,29 | 29,55,30 |

A conta pelas faces previa 1,41/1,27/1,21 de luz a mais no chão à sombra; a medida direta deu 1,39/1,25/1,10. É um déficit só, em terreno e objetos, o que explica os 1,08x a 1,25x da Porte 1.

Solução: ambiente chapado menos azul, mais uma luz de cima sem sombra, que só atinge o que olha para o céu, como o hemisfério do 4.0. Não é o hemisfério exato: as faces voltadas para baixo recebem a luz de parede, não a cor do chão. Mantive assim porque quase não se veem.

**Causa 3, provada:** `shadow_bias` 0,02 listrava telhado liso; voltou ao padrão do Godot, 0,1.

Cidade inteira, 4.0 contra jogo:

| Medida | 4.0 | Antes | Agora |
|---|---|---|---|
| Cidade, luminância p5/p50/p95 | 31/90/181 | 50/123/210 | 45/105/202 |
| Cidade, azul/vermelho | 1,05 | 1,06 (sem sombra) | 1,06 |
| Terreno, RGB | 64,89,53 | 49,79,48 | 56,86,49 |
| Terreno, azul/vermelho | 0,82 | 0,97 | 0,88 |

Estes números misturam luz e composição: a cidade do jogo tem mais gramado e telhado claro, e torres azuis. A sobra de luminância parece vir disso, mas não medi separado.

**Falta:**
- Terreno de longe ainda mais escuro e menos vermelho que no 4.0 (56,86,49 contra 64,89,53). Suspeito do ar, não provei.
- `canyonLight` não portado. Li o shader: é um passe com campo de alturas. Sem ele as ruas fundas não escurecem como no 4.0.
- GTAO: o SSAO do Godot não mudou nada nesta distância, e o preset padrão segue sem ele. Falta medir de perto e o custo.
- Bordas de sombra de longe mais borradas que no 4.0 (ver o cubo): filtro sobre texel grande.
- `_mean_color` da Porte 1 ainda não está neste ramo.

### Item 4, serrilhado: em parte

Janelas de longe em pontos: nem borda nem textura. É o `step()` duro em `building_far.gdshader`, que nem MSAA nem filtro de textura alcançam. `convert/build` já trocou por filtro pelo tamanho do pixel, então não dupliquei: conferir na integração.

Telhados salpicados depois da sombra ligada: padrão do filtro de sombra, descrito na documentação do Godot. Subi o filtro por preset (Soft Medium no médio), e o salpicado sumiu do cubo; na cidade ainda não medi.

Falta: provar os degraus de parede de perto, decidir SMAA e anisotropia no preset padrão e medir o custo.

### Pares antigos

Os 18 pares de `evidence/luz/` ainda não foram refeitos em 1920x1080, e a releitura em resolução cheia segue pendente.

### Não verificado

- Custo na GPU da sombra do nível distante, do atlas 8192 do preset alto e do filtro: tempos de quadro não valem com a máquina carregada. Comando: `tests/perf/camera_pan.tscn`, com a máquina quieta. Este é o risco maior desta mudança.
- Foto noturna e de chuva com a luz nova.
- Compatibility: os testes passam, sem foto.
- Vista de rua da Porte 6 (poste de placa) com as cascatas novas: usei a conta dela, mas não refiz a pose.

Clone: `ports/landscape/workspace`, branch `convert/landscape`. Nada integrado. Este relatório vale para o último commit do branch.

## Situação

A paisagem não está pronta. Na base integrada, 14 das 18 fotos valem como par; os abertos estão em "Abertos, por ordem".

## O que foi lido

- `converter/out/features/landscape/feature.md`, `source/` e `boundary.json`, com o rastreamento estendido para `src/render/elements.ts` e as confirmações de `src/main.ts`.
- Do 4.0, além do rastreamento: `render/environment.ts`, `render/postprocess.ts`, `render/renderer.ts`, `render/terrain.ts`, `render/mesh/detailLayer.ts`, `render/sunCascades.ts`, `render/shadowCasters.ts`, `render/canyonLight.ts`, `render/isoViewport.ts`, `view/cameraProfile.ts`, `world/fogPaint.ts`.
- Do Godot: documentação de `Environment`, e `tonemap.glsl` para a ordem de tom, sRGB e tabela de cor.

## Requisitos e o que foi feito

| Requisito | Estado |
|---|---|
| Terreno, vegetação, céu, clima, nuvens, camada de detalhe | portado; linha a linha em `coverage.md` |
| Elementos do pincel e confirmações de limpar | portado |
| Copa com miolo escuro, matas chapadas | corrigido (normal dos cartões, retratos por elevação) |
| Constantes de `trees` sem fonte | ditas como escolha de modelagem no `_doc`; tons agora vêm da medida |
| Luz global, tom, gradação, céu, atmosfera | medido e decidido, seção abaixo |
| Luz seguindo o relógio, três modos | `CityWorld.set_sky_mode`, `sky_mode`, `follow_clock`; botões da barra ligados |
| Sombras, oclusão, SMAA, brilho | valores portados; `canyonLight` não |
| Estilos regionais | `styles.roadcraft` e `styles.brasil` em `nature.json`; o carregador lê `roadcraft` |

## Luz global: o que foi medido

Plano e decisão em `plan.md`. Duas rodadas foram descartadas por erro meu: comparei as fotos do 4.0 com o jogo às 12h, mas o modo dia do 4.0 fixa 16h (`renderer.ts:3725`). As medidas abaixo são com o sol a 29° nos dois, mesma câmera (pose lida do 4.0, `poses.json`), 1920x1080.

Causas achadas, cada uma provada:

1. **Gradação de filme do 4.0 ausente.** É por isso que um ganho só nunca servia. Portada como tabela 3D do `Environment`, sem passe a mais.
2. **Sol escrito errado.** `setRGB(1, 0.96, 0.9)` é linear; aqui era `#fff0d2` lido como sRGB. Era a causa do jogo amarelado.
3. **Cadeia de tom diferente.** 1/0,6 no 4.0 contra 1,4 × 1,8 aqui. Céu, neblina e nuvens, que não recebem luz, levam o ganho 0,661.
4. **Ponto branco.** 6 dividia a curva por 0,977; 14,26 divide por 1.
5. **Mapa de detalhe da grama lido como cor.** É dado (`use: 'data'`). Lido como sRGB, tinha média 0,245 em vez de 0,5 e apagava metade do chão de perto. Era a causa do grão fino fraco.

Saíram os ajustes que compensavam a luz errada: `grass_tint`, `land_gain`, `night_ground` e o alcance alargado do grão. A grama passou a bater sem nenhum tom próprio.

Valores: sol 0,89, preenchimento 0,21, ambiente 0,266 em `#b5d8ff`, exposição 1,4, branco 14,26. Fonte física do sol: cerca de 98.000 lx na superfície (en.wikipedia.org/wiki/Sunlight); energia 0,89 representa isso, 1 unidade ≈ 110 klx. O ambiente é escolha de modelagem na proporção do 4.0.

### Alvos, 4.0 contra jogo

| Alvo | 4.0 | Jogo |
|---|---|---|
| Areia pintada | 228,220,202 | 224,217,198 |
| Neve pintada | 235,234,231 | 233,233,230 |
| Cascalho pintado | 193,192,189 | 186,186,183 |
| Asfalto pintado | 124,127,143 (azul/vermelho 1,15) | 127,132,146 (1,15) |
| Grama, campo médio | 76,105,56 | 71,102,50 |
| Água | 121,138,152 | 123,138,152 |
| Céu olhando para cima, dia | 154,201,229 | 155,202,229 |
| Céu olhando para cima, noite | 19,60,110 | 19,60,108 |
| Campo à noite | 10,23,9 | 8,20,3 |

Grão fino em chão livre (desvio, diferença entre vizinhos): 4,95 e 2,47 no 4.0, 4,9 e 2,19 no jogo.

### Rampa de cinzas sem luz

Cartas sem iluminação com a mesma radiância nos dois jogos, 17 passos, em `evidence/luz/rampa-tabela.txt`. Extrato:

| Entrada | 4.0 | Jogo |
|---|---|---|
| 0 | 0,1,5 | 1,1,1 |
| 16 | 3,5,11 | 2,3,5 |
| 64 | 43,46,50 | 43,44,46 |
| 128 | 144,144,142 | 145,143,141 |
| 255 | 232,229,224 | 238,234,229 |

O azul nas sombras do 4.0 não vem da gradação: `GRADED_OUTPUT_FRAGMENT` só soma a viragem acima de 0,15 de luma, sem nada abaixo. Vem da névoa sobre as cartas; quando deixei as cartas receberem a névoa no jogo, o azul subiu. Ainda sobra de 2 a 6 níveis de azul no escuro e 6 no branco.

O teste `light_color_test` confere a gradação em 0, 46, 127 e 255 contra a conta do shader do 4.0. Isso prova a função; quem prova a tabela na tela é a rampa.

## Relógio

`capture_clock.tscn` abre `play.tscn` e aperta os botões reais da barra. Em `evidence/relogio/`: sol, lua e relógio, depois a barra em 4× com o céu seguindo 08:28, 10:28, 12:28 e 14:28 (sol 0,89). A foto pelo botão aparece com o HUD. Não fotografei o relógio passando pela noite neste fluxo; isso só está em `evidence/luz13`, pelos ticks.

## Commits

0488c78 sim, ea41ebd interface, a2ab4b0 árvores, 1c604b4 render e céu, 0348903 ecologia, c8fced8 cobertura e água, 5d12a18 elementos, 9cc9ec7 luz global, 022b734 ponto branco, ar, neblina e sombra das nuvens, 4cf7159 botões do céu, nublado e estilos, mais o último commit do branch com água e ar pela altura, 2b0a0e6 árvores, e2d7d2d matas, mipmaps e palmas, 5bc31ba água, noite e palma, 9793686 folhas e estipe, 822506a `canyonLight` e sombra, 156631b bioma e palmeira de longe, f558f8b borda das folhas, 7973649 margem e grama, 4aaf699 mar depois do mapa, d5aa565 folha sem degraus, 1d0b110 chave de job por campo, 836e34d merge do master, c061e52 merge do master com JobKey, 719f98c viés da sombra e hemisfério, 1c46296 cascatas, viés exato e névoa da chuva, 7f72136 ruído azul e 36 passos, d35b90d cor do ar, d2aaaaf viés medido por escada de placas, 0b4f29a texturas dos elementos, 02de1bc Leaves em 1024, d147122 pedras da margem.

## Testes e builds

Gates OK. DLLs de release e debug compilam. Oito testes passam em `evidence/testes/` e na última rodada (`save_undo`, `road_commands`, `camera_input`, `light_color` e `building_light` nos dois renderizadores, `e2e`). O teste de lente passou a aceitar 3 passos em vez de 1: o verde do semáforo fica na borda do gamut do ACES.

## Fotos analisadas

Montagens lado a lado em `evidence/luz/`, 4.0 à esquerda, pose no nome. Os mapas são diferentes (cidade pequena semente 7 contra mapa 0), então só a câmera é a mesma; comparação vale para tom, não para o que está em cena. O 4.0 tem HUD, o jogo não.

Li em tamanho de montagem, não em resolução cheia, e estes defeitos apareceram:

- `ceu_baixo`: céu e grama muito próximos; grama de perto mais escura (66,95,50 contra 81,106,52).
- `neblina`: tufos e densidade próximos; sem árvores na região no mapa 0, então a leitura das copas apagadas não foi conferida.
- `nuvens_sombra`: sombras do mesmo tamanho e tom; matas do jogo mais pálidas e mais densas que no 4.0.
- `chuva`: jogo ainda mais escuro que o 4.0, mesmo depois do ajuste (só não refeita depois da névoa nova).
- `rio`: cor certa; a água do 4.0 mostra o fundo, a do jogo não.
- `matas_alto`: chão perto (63,98,53 contra 71,100,55); copas mais claras e mais cheias.
- `luz_rua3`: paredes ao sol pálidas, sem o calor do creme do 4.0; ruas sem o escurecimento de cânion.
- `noite_cidade2`: poças de luz mais duras e amarelas; paredes mais frias.

Não reli nesta rodada: `campo_perto`, `campo_meio`, `matas_meio`, `horizonte`, `nuvens_ceu`, `ceu_alto`, `ceu_noite`, `luz_parede3`. As análises antigas de `evidence/pontos/` usam outro enquadramento e não valem como par.

## Defeitos ainda abertos

1. **Via e calçada escuras.** Com os discos pintados na cena da Porte 1 batendo, a luz sobre o terreno está certa. Sobram uns 1,2x de luz extra do 4.0 fora do terreno e uns 1,1x a 1,2x do material no jogo. Porte 1 acha o do material; depois faço a luz extra.
2. **`canyonLight` pela metade.** A oclusão pelo campo de alturas entrou; faltam a sombra de contato e a prova na tabela de paredes.
3. **Chuva mais escura que no 4.0.**
4. **Água:** a folha e o leito entraram; faltam as pedras da margem e do leito.
5. **Grama muito de perto:** tom a 7% do 4.0 em `solta_palma_perto`; a cor das lâminas já vai até 0,44 m por pixel.
6. **Copas de longe mais claras e cheias.**
7. **Ambiente de hemisfério:** o 4.0 tem cor de chão por baixo; o Godot é plano.
8. **Ar:** densidade constante por altura do olho, não integrada por raio.
9. **Nitidez CAS do 4.0 não portada.**
10. Tabela de RGB e azul/vermelho por superfície (calçada, parede, sombra) sem pontos confiáveis, porque os mapas diferem.

## Constantes com fonte

Em `_doc` de `daylight.json`, `sky.json`, `nature.json`, `palette.json` e `streets.json`, e por modelo em `research.md`. Tons de copa vêm da medida nas fotos do 4.0; a gradação e a vinheta são escolha de modelagem, o aspecto do 4.0.

## Cobertura

`coverage.md`: uma linha por arquivo e declaração. `canyonLight.ts` dividido com a Porte 4, `shadowCasters.ts`, `sunCascades.ts` e `effectSwitches.ts` acrescentados. O 4.0 não tem Henyey-Greenstein: aquilo era da fonte Schneider, e o halo do sol dele é `sunSide`, portado.

## Não verificado

- `camera_pan`, tempos de quadro, GPU, CPU e memória, com a máquina carregada. Comando: `tools/build/build.sh e2e` e `tests/perf/camera_pan.tscn`, com a máquina quieta.
- Custo de `Daylight.set_hour` por passo do relógio e do ruído novo da neblina (seis amostras por passo do raio).
- Compatibility: os testes de luz passam, mas sem foto dos pares.
- Oclusão e SMAA: valores postos no preset alto; o preset padrão é médio, sem eles, então nenhuma foto os mostra.
- `capture_features` e `capture_ui` não refeitas depois da luz.
- Licença dos modelos de elementos.
- Efeitos do pincel além de poeira e fuligem.
- `settings.gd`: a captura em `play.tscn` acusou erro ao ler o preset `auto` gravado em `user://settings.cfg`, vindo de outra sessão; não investiguei.
