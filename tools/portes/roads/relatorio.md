# Relatório do porte de vias (`roads`)

**Situação: porte incompleto.** O que o jogador faz com a ferramenta de via agora é bem mais próximo do 4.0, mas a maior parte do rastreamento não foi portada. Este relatório diz o que existe, não o que seria desejável. Clone `ports/roads/workspace`, branch `convert/roads`, sem push nem integração.

## Cobertura

| Estado | Arquivos | Declarações próprias |
|---|---|---|
| em parte | 10 | 233 |
| já existia | 1 | 11 |
| não portado | 98 | 1113 |
| não se aplica | 165 | 1045 |
| portado | 6 | 102 |
| total | 280 | 2504 |

Lista completa em `cobertura.md`. Atenção a como ler a tabela:
- Boa parte de **não se aplica** e de **não portado** foi classificada **por pasta**, sem ler cada declaração; a linha diz isso. É o caso do render de ladrilhos do 4.0 (`src/render/mesh`, `roadSurface*`), dos utilitários de `src/core` e de `src/world`. Só 280 arquivos × leitura real daria certeza.
- **portado** e **em parte** somam pouco porque contam declarações do 4.0, não comportamento: uma função do jogo cobre muitas delas.

## O que leu

`feature.md`, `boundary.json` e o rastreamento refeito por mim (2.504 declarações, 280 arquivos, 123 entradas em `entradas-roads.txt`; 130 arquivos conferidos inteiros contra `src/`, 150 parciais só por existência e proporção). Lidos de verdade: `roadTool`, `snap`, `roadPath`, `commit` em parte, `editRules`, `nodeMover`, `catalog`, `profile`, `templates`, `roadTypes`, `roadSection` em parte, `markings`, `markingStyle`, `economy`, `furnitureSets`, `landscape` em parte, `ui/v2/shell.ts` nas vias, `main.ts` na prévia e no controle, `render/environment.ts`. **Não li** `network*`, `junction/*`, `lanelets`, `roadField`, `streets.ts`, `plan.ts`, `profileEditor`, `junctionPanel`.

## Requisitos

Ferramenta de via (arrastar, encadear, curva com dobra, traço à mão, encaixe), catálogo, aparência por classe, prévia, opções, abas Mover e Controle, regras de edição, geração de ruas, editor de perfil, modelos do jogador.

## Plano e o que foi feito

Temas em ordem: ferramenta e encaixe; ponta de via; tolerância medida do traço; catálogo e classes; prévia; opções; Mover; Controle; regras. Geração de ruas, editor de perfil e modelos **não foram feitos**.

## Commits

- `9485433` Talude até o terreno, calçada no sentido do 4.0 e rampa onde a zebra está
- `2e24f97` capture_roads espera por CaptureWait e repete a espera depois de salto sem nada pendente
- `c62f275` Merge master (luz da Porte 3, cidade e Construir da Porte 2, espera das capturas)
- `689b80d` Captura: a espera de toda foto num auxiliar que registra quanto esperou e avisa foto incompleta
- `56a7864` uid do city_mass_test (o Godot gera e a Porte 2 nao tinha commitado)
- `0092932` Integracao: cidade, Construir e interiores da Porte 2 (convert/build em 0f5d8da) sobre a luz da Porte 3
- `29a9bf5` Integracao: luz, matas, ceu e agua da Porte 3 (convert/landscape em 5bc31ba)
- `5bc31ba` Agua transparente sobre o leito, noite pelo cubo cinza e palma inteira
- `e2d7d2d` Matas pela regra do 4.0, mipmaps na luz linear e palmas que caem
- `945fbea` Faces do meio-fio sem listras, placa com o miolo curto e capturas com o antialias do jogo
- `2b0a0e6` Arvores: tons em linear, tronco com forma e casca, copa e cone do 4.0, reflexo das folhas
- `908bbf3` Placa do cruzamento com os braços em degradê
- `165e238` Placa do cruzamento no asfalto de cada braço, e linhas até a retenção
- `34ce206` Luz: predios distantes lancam sombra, ambiente medido num cubo cinza e cascatas pela vista
- `5fcd481` Canteiro como o 4.0: grama do mapa, ilha sobre as faixas e ponta antes da travessia
- `0f5d8da` Cidade gerada: tamanhos do 4.0 por mapa e pracas de bairro
- `a03e47a` Andares: o par da loja respeita o teto dos vizinhos
- `8608f7a` Corpo como unico dono dos andares
- `f750c94` Cidade gerada: plano do 4.0, com centros da semente, calor e cruzamentos em T
- `b375e59` Marcas de uso da pista e da calçada, e linhas na largura que o 4.0 desenha
- `0a4eb94` Construir: aba Modelos, com catalogo, encaixe, fusao e modelos do jogador
- `ff69fb8` Camada de detalhe e meio-fio das vias como o 4.0
- `548860a` Capturas de par com a câmera exata do 4.0
- `377cbc0` Média dos mapas conta todos os pixels: a calçada saía escura
- `01f6288` Agua pela medida e ar que afina com a altura
- `4cf7159` Luz: botoes do ceu no jogo, nublado e ar do 4.0, reflexo do ceu comum, estilos da paisagem
- `cedd077` Estilo regional: secao styles.<id> em buildings.json e interiors.json
- `d4074a4` Toco de via junto de junção volta a ser via, e estilos regionais da pintura
- `022b734` Luz: ponto branco exato, ar pelo ganho do ceu, neblina em tufos, sombra das nuvens e ceu da noite
- `e13c1ba` Construir: equipamentos de cobertura, e a forma do telhado com dono único
- `62263e5` Paridade da pintura: sarjeta suja em vez de faixa creme, desenho do 4.0 por padrão
- `9cc9ec7` Luz global: gradacao de filme, sol do 4.0, preenchimento, ceu e relogio
- `45764ae` Constantes sem fonte viram geometria, norma ou recuo real
- `cd0e9d6` Construir: aba Peças, com escada, rampa, pedras, vaga e relógio
- `378c007` Linha dupla na avenida sem canteiro, pelo MBST vol. IV
- `627a65c` Ruído da via por hash inteiro, save sem variantes e noite nas capturas
- `96dcd38` Construir: interface das abas Massa, Fachada, Telhado e Cores
- `82eb387` Pendências do porte de vias no mapa; recusa por trecho apertado não informa valor
- `54d874d` Construir: seleção por raio, vãos que seguem a parede e ala com vãos próprios
- `dc41351` Regras de edição: ângulo mínimo em todo pouso e trecho apertado medido pelo recuo real
- `50ae13c` Aba Controle: cada cruzamento diz o que o rege, e um clique troca
- `5d12a18` Elementos do pincel (objetos e efeitos), areia nas margens do rio, confirmacoes de limpar, palha na grama e tons medidos nas fotos do 4.0
- `87ee3c2` Construir: abas Telhado e Cores por comando
- `6dcef27` Aba Mover: o nó vai e as vias são assentadas de novo
- `e7219d7` Construir: aba Fachada por comando
- `4cbd29e` Faixas, Largura e Vagas nas vias de classe, com variantes no catálogo e no save
- `57ab19f` Construir: corpo editável por prédio e a aba Massa por comando
- `c8fced8` Paisagem: cobertura do chao, mapa de terra e rocha ligado, relevo da grama, agua pelo fundo, neblina sem grade e luz da noite
- `0348903` Ecologia: misturas das comunidades em nature.json, com fonte ou modelagem por grupo; relevo pelas classes da Embrapa
- `576f713` Prévia da via como será assentada, com etiqueta junto do fim
- `09d23df` Catálogo de vias e aparência por classe como o Roadcraft 4.0
- `2f3c8a5` Vidro de lado, fotos como o jogador vê e relógio na hora da foto
- `12ff650` Desgaste de ruas e calçadas: campo de uso em janela, só as camadas alteradas enviadas
- `7dcf184` Traço à mão: tolerância de 2 m, medida; custo da prévia no teste
- `1c604b4` Paisagem no render: ceu desenhado, nuvens e sua sombra, neblina, chuva, raios, chao pintado, ravinas, grao do capim e agua dos rios
- `a2ab4b0` Arvores: normal dos cartoes corrigida, retratos por elevacao no nivel distante e ecologia na colocacao
- `28c8a4c` capture_roads: sem o meio-dia, o sol do jogo já é o das 16h do modo dia do 4.0
- `6ee0f26` Ponta de via: corte reto por padrão, balão de retorno a pedido no inspetor
- `eb1b193` Janelas acesas calibradas nas fotos do 4.0: y 0,52, sem estourar nem halo
- `0d87a18` Interiores: planta e móveis de cada andar, e o cômodo real atrás de cada vidro
- `a2b6eec` Ferramenta de via: arrastar, encadear, curva, traço à mão e encaixe do Roadcraft 4.0
- `5f5b9c8` Vidro dos prédios: cômodo, cortina, caixilho e lâmpada por hora, em todos os níveis
- `ea41ebd` Paisagem na gaveta: abas Solo, Natureza e Céu, pincéis de pintura, árvores, neblina, nuvens e clima
- `0488c78` Paisagem: dono sim/nature (clima, nuvens, neblina, chão pintado, árvores plantadas, ecologia), comandos com desfazer e save
- `efab0cd` Janelas: o aro não tampa mais o vidro (causa da chapa ondulada e das torres apagadas)


## Estado atual: `c978aee`, pose exata, preset do 4.0

Câmera lida de `viewCamera`, lente 35°, 16h, preset `high`, o mesmo do 4.0. Gates OK, DLLs de release e debug, 14/14 testes (`evidence/tests-oclusao-final`). Nada foi enviado por push.

### Fechados, com causa medida

| Item | Causa | Correção | Medida | Prova |
|---|---|---|---|---|
| Grão azulado | Mapas em BC1 no jogo, RGBA8 no 4.0 (`surface-bake-plugin.ts`, CODEC). Desvio de b-r do asfalto: 0,53 no PNG, 3,45 na GPU | Quatro mapas sem perda. BC7 medido e recusado: 1,56, e média de 2,82 para 1,71 | Desvio de b-r na foto: 3,8–5,1 antes, 1,35–1,52 depois, 1,63–2,92 no 4.0 | `medir_grao.py` |
| Face do meio-fio | Eixos trocados e altura relativa. O 4.0 lê `(run, altura no mundo)`: `mesh/tileMesh.ts`, `uvs.push2(u0, topA / scale)` | Face por comprimento e altura no mundo, topo por seção e comprimento, normal do meio-fio | Blocos cinza sumiram; emendas e manchas como no 4.0 | `paridade/par11_meiofio_6x.png` |
| Ponta do lote | A linha do lote seguia o arco do meio-fio: raio 2,8 m menos calçada 2 m dá ponta redonda de 0,8 m | As duas bordas seguem até se encontrar | Ponta viva, no mesmo lugar do 4.0 | `paridade/par12_cantos_4x.png` |
| Faixa do talude | Não é defeito. Rampa reta nos dois: 18% abaixo do gramado no 4.0, 15% no jogo | Uma faixa, terreno segurado 2,2 m além da via | Perfil por distância da borda | `medir_talude.py` |
| X do cruzamento | O tom misturava por ângulo e fechava no centro. O 4.0 pinta por vértice numa malha de 3,2 m: largura constante | Leque por quadrante, tom por pixel com a diferença das distâncias aos eixos (`M_JOIN`) | Contraste a 2, 3 e 4 m do nó: 2,62, 3,83 e 4,05, contra 2,22, 3,17 e 3,83 no 4.0 e 4,95, 5,26 e 4,99 antes. A 6 m o anel pega o meio-fio: 3,92 contra 5,27 | `medir_x.py`, `paridade/par15_esquina_top.png` |
| Linha no asfalto | A via é quebrada a cada 112 m (`road_max_seg_m`) e o comprimento recomeçava: o pixel da linha cai em `dz` −0,14 m do nó `joint` | O comprimento corre pela rua (`street_start`) | Linha sumiu | `paridade/par14_linha_5x.png` |

### Par de uso

Levantamento: `wear.ts` deposita por `wheels` e `feet`, internos ao renderizador. A textura é alcançável por `__roadcraft.scene().gl.properties` e o passo `eval` do plano roda na página. Então o mesmo valor foi escrito nos dois jogos, um em cada braço (`look-par15.json`, `PairCamera.lay_wear`), com o tráfego parado e a foto sem uso e a com uso na mesma execução. A razão entre as duas cancela luz e mapa.

| Uso | Esperado | Asfalto 4.0 | Asfalto jogo | Calçada 4.0 | Calçada jogo |
|---|---|---|---|---|---|
| 0,25 | 0,912 | 0,880 | 0,882 | 0,941 | 0,937 |
| 0,50 | 0,740 | 0,672 | 0,671 | 0,800 | 0,813 |
| 0,75 | 0,624 | 0,612 | 0,587 | 0,709 | 0,670 |
| 1,00 | 0,620 | 0,955 | 0,612 | 0,686 | 0,646 |

Os 0,955 são a mediana caindo em cima de buracos: eles clareiam o asfalto escuro. Parte escura do braço: 0,614 no 4.0, 0,607 no jogo. Parte com buraco neste braço: 55% contra 36%.

**Limite do que o par prova:** a função uso → aparência, não o uso em si. O 4.0 e o jogo têm ruídos diferentes, então os buracos caem em lugares diferentes. Quanto uso o tráfego deposita não está provado. As fotos do jogo são de `fotos-par13`, em `medium`; a razão cancela o preset, mas não foi refeita em `high`.

### Tom do meio-fio: fechado com passe de oclusão

Causa: o SSAO do Godot tirava 1 a 2 níveis onde o GTAO do 4.0 tira 12. Correção: `project/render/shaders/occlusion.gdshader`, um GTAO (Jimenez et al. 2016) pela profundidade, num quadro de tela cheia em `blend_mul`, no lugar do SSAO. Nenhuma cor de material mudou. Valores do 4.0: alcance médio 0,75 (raio 1 vezes a parte sorteada de 0,5 a 1), expoente 1,6, espessura 0,8, escala 1,05, intensidade 0,85. Só nos presets com oclusão e em Forward+, como o SSAO.

Canal verde, canteiro de `median_low`, mesma pose (`medir_face.py`, `evidence/fotos-oclusao`, par em `fotos-oclusao/par_meiofio_3x.png`):

| | Topo do meio-fio | Pé |
|---|---|---|
| 4.0, com oclusão | 185, 182, 181, 180, 183 | 149 a 167 |
| Jogo, com o passe | 190, 188, 186, 181, 185 | 161 a 179 |
| Jogo, antes (SSAO) | 192 | 182 |
| Sem oclusão nos dois | 193 | 170 no 4.0, 182 no jogo |

Sobram 4 a 5 níveis no topo. Queda no pé: 21 níveis nos dois. Céu e asfalto plano iguais com e sem o passe (59 e 106).

Defeitos achados e o que aconteceu:
- **Zero efeito na primeira rodada:** `PROJECTION_MATRIX[1][1]` é negativo no Vulkan do Godot, e o corte por distância nunca passava. Provado com uma sonda: quadro cinza desenhado e passe ligado. Corrigido com `abs`.
- **Faixas numa chapa lisa de perto** (`22_onibus`, câmera a menos de 2 m): cada passo fixo cai longe na tela. O decaimento por distância do XeGTAO foi testado e recusado: devolveu o meio-fio a 190. Corrigido com limite do alcance na tela, 15% da altura, como o raio máximo do HBAO. O meio-fio não mudou. Sobram degraus fracos nos últimos 40 pixels da borda dessa foto.
- **Copas:** um pouco mais de contraste entre as folhas (grão 28,1 contra 25,8 no ipê de `04_rua_baixa`), porque não há o denoise do 4.0. Fica assim e está dito.

Não verificado: tempo de quadro e GPU, máquina carregada. O passe faz até 37 leituras de profundidade por pixel em tela cheia, no lugar do SSAO. Comando para a janela quieta, com `high` no `settings.cfg`:

```bash
godot --path project --resolution 1600x900 res://tests/perf/camera_pan.tscn -- --no-flow --moves=parada,girar
```

Também não verificado: lago com água transparente (o mapa 0 não tem) e cidade à noite. O preset padrão segue `medium`, sem oclusão; o 4.0 abre em `high`.

### Custo

Mapa 0, 304 nós, 438 trechos, 36 chunks. `road_mesh_counts`, valem sob carga.

| | Triângulos | Vértices | Bytes | Maior chunk |
|---|---|---|---|---|
| Vias, antes do X | 152.302 | 277.116 | 17.346.120 | 1.063.168 |
| Vias, agora | 138.478 | 271.080 | 16.842.216 | 1.035.584 |
| Talude, quatro faixas | não medido | | | |
| Talude, uma faixa, passo de 3 m | 42.944 | 46.960 | 3.145.088 | 194.864 |
| Talude, trecho reto de um vão | 13.224 | 17.240 | 1.124.128 | 64.576 |
| Talude, agora: ponta com dois vãos | 4.544 | 8.560 | 533.888 | 29.760 |

Draw calls na cena parada: 276 com talude, 249 sem (`--verges=0`), 27 na vista. Desgaste: 8 MiB de textura e 8 MiB de cópia, nenhum triângulo.

De onde vinham os 42.944: 29.720 do passo de 3 m em trecho reto, onde a via tem um vão só; 8.680 das esquinas, com dez vãos onde bastam dois. Os 4.544 que sobram são curvas, esquinas e pontas. O passo de curva é o da via, então o talude não se separa dela. A resolução do terreno é 4 m, mas o terreno é segurado na altura da via até 2,2 m além, por isso o trecho reto não precisa de pontos no meio.

Segunda superfície do mesmo chunk: não reduz draw call, porque o talude usa outro shader e cada superfície é desenhada à parte. Economiza no máximo 36 instâncias de `RenderingServer`. A documentação do Godot lida não dá o custo por superfície, então isso é leitura minha do material diferente, não medida. Não fiz.

Tempo de quadro: não medido, a máquina não ficou quieta. Comando, na base mais a ponta:

```bash
godot --path project --resolution 1600x900 res://tests/perf/camera_pan.tscn -- --no-flow --moves=parada,girar
```

Repetir com `--verges=0` e com `--wear=0`.

### Fotos que não contam

`par9` e `v3` (câmera fora), tudo antes de `par13` para tom de meio-fio (preset `medium`), `fotos-par11-dll-antiga`, `fotos-par12-sem-efeito`, `custo/contagens-*` para tempo (CPU a 69%).

Medidos ainda sem releitura neste preset: `median_nose_low`, `median_paved_*`, `pair_*` de `fotos-par15` passaram pelas medidas acima, não por releitura completa em resolução cheia.

## Resultados

- Gates OK. `core-windows` gera as DLLs de release e de debug juntas; 9 cenas de teste OK em `evidence/tests-final` (`road_edit_test` é nova).
- Tom ao sol, 16h: asfalto da via urbana 56 no 4.0, 135 antes, 94 agora; calçada ~158, ~214, ~172; capim 104, 98, 98.
- Custo da prévia: 35 peças, 40 us por movimento em chão aberto; 43 peças, 673 us atravessando a cidade.
- Save real da versão 1 (build a11384a, oito classes de via, incluindo as duas que saíram do catálogo): abriu, 962 vias e R$ 16.337.400 iguais antes e depois, 748 veículos e 630 andando, regravou na versão 2 e reabriu. `evidence/save-v1`.
- Quadrados no asfalto: remendos da textura, presentes no 4.0, mais fracos no jogo (0,775 e 0,748 contra 0,699 e 0,695). Trocar o ruído não os tira nem devia.
- Trazida do porte de tráfego a correção 58d6f21 (nascer a centímetros da linha): a geometria nova expôs o defeito no e2e, medido por sonda (`SONDA red v=0 v_was=10,87`). Mesma mudança nos dois clones.

## Problemas que sobram, sabidos

- A prévia não avisa de trecho apertado: só a tentativa de assentar.
- A prévia do Mover é a faixa, não a via reconstruída como no 4.0.
- Mover recusa um nó cujo outro lado já formava ângulo fechado, mesmo sem piorar.
- Parada, preferência, sem controle e minirrotatória rodam como prioridade: comportamento é do porte de tráfego.
- Altura, Corte: `road-elevation`. Mobiliário: clone do Porte 6. Teclas e o quarto traçado (Quarteirões): não feitos.
- A aba Trilhos segue no catálogo; o 4.0 tem Meus modelos no lugar.
- Linha contínua na aproximação de semáforo (MBST vol. IV 4.2.1) não existe: a linha é a mesma até o cruzamento.
- Zebra e retenção diferem do 4.0, mas a travessia é do porte de mobiliário.
- Exposição e sol: `evidence/roads-luz.md`, entregue ao Porte 3.

## O que não foi verificado
- Pasta `user://`: a partir das rodadas seguintes a `fotos-par7`, o clone usa `Roadcraft-roads` (`project/override.cfg`, fora do git, `regras-sessoes.md` seção 12). Medidas e fotos anteriores valem com a ressalva do `settings.cfg` e do cache de pipelines compartilhados.

- Tempos de quadro, GPU, CPU, `camera_pan`: não medidos, máquina carregada. A coordenadora faz a janela quieta na integração, com o comando acima.
- Noite certa: as fotos noturnas existem (`evidence/fotos-t10`) e estão erradas; a correção de poça e esquina é da Porte 6, em commits que este clone não tem. A noite com os dois juntos não foi vista: só depois da integração. Capim escuro: Porte 3.
- O custo da prévia na cidade foi medido com a máquina carregada: 673 us por movimento, 43 peças. Não é um tempo de referência.

## Pesquisa e constantes

Fontes consultadas nesta sessão, com a conclusão de cada uma. "Primária" é norma, documentação oficial ou artigo original. O valor do 4.0 não conta como fonte: serve só para saber o que o jogador via.

## Fontes

| Assunto | Fonte | Tipo | Conclusão |
|---|---|---|---|
| Simplificar o traço | Ramer, *Computer Graphics and Image Processing* 1(3), 244-256, 1972; Douglas e Peucker, *The Canadian Cartographer* 10(2), 112-122, 1973. Citação lida em https://docs.generic-mapping-tools.org/6.6/gmtsimplify.html e https://saga-gis.sourceforge.io/saga_tool_doc/9.12.3/shapes_lines_4.html | Secundária sobre artigos originais; **os artigos em si não foram lidos** | O parâmetro é a distância máxima entre a linha original e a simplificada. Mantém o ponto mais longe da corda e recorre nas duas metades. A GMT avisa que o método pode gerar cruzamentos. |
| Curvas tangentes nas juntas | Patente US 5818459, https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5818459; slides UPC, https://dccg.upc.edu/wp-content/uploads/2026/05/5.-B-splines.pdf; slides UCSD, https://visualcomp.ucsd.edu/classes/cse167/wi17/slides/curves.pdf | Patente e material de curso | A B-spline quadrática uniforme vira quadráticas de Bézier com junta no ponto médio de cada par de pontos de controle, e o segmento entre eles é a tangente na junta. Nenhuma fonte afirma C1 com todas as letras: a continuidade é conclusão minha dessa construção, e o teste `curvas tangentes nas juntas` a confere. |
| Raio mínimo da quadrática | Dedução própria, escrita em `geometry.cpp` | Derivação | `B'(t) = 2(A + tD)`, `B' x B''` é constante, logo o raio mínimo é `2 |A + tD|^3 / |A x D|` no `t` de menor `|A + tD|`. |
| Tela para mundo | https://docs.godotengine.org/en/stable/classes/class_camera3d.html | Primária | `project_ray_origin` e `project_ray_normal` dão o raio em espaço de mundo. `px_at` usa a distância à câmera e `fov`. Que `fov` é o ângulo vertical vem da propriedade `keep_aspect` padrão: **assumido da memória, não reconfirmado na página**. |
| Tamanho de alvo | https://www.w3.org/WAI/WCAG22/Understanding/target-size-minimum.html e https://www.w3.org/WAI/WCAG22/Understanding/target-size-enhanced.html | Primária (W3C) | 2.5.8, nível AA: alvo de pelo menos 24 por 24 px CSS. 2.5.5, nível AAA: 44 por 44. A norma é de páginas web, aplicada aqui por analogia. |
| Clique ou arrasto | https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsystemmetrics | Primária | `SM_CXDRAG`: quantos pixels o ponteiro anda antes de virar arrasto, para o clique não virar arrasto sem querer. A página não dá valor padrão: **medido nesta máquina, 4 px**. |
| Idem | https://doc.qt.io/QT-5/qstylehints.html | Primária | Qt tem o mesmo conceito (`startDragDistance`). A busca não confirmou o número 10 que eu lembrava; não o uso. |
| Ângulo de encaixe | Guia do Inkscape, https://docs.huihoo.com/inkscape/guide-2nd/html/Transforms.html; https://bugs.launchpad.net/inkscape/+bug/1486403/comments/0 | Secundária, versão antiga | Rotação com Ctrl em múltiplos de 15 graus por padrão. É precedente de ferramenta de desenho, não norma. |
| Opções de encaixe | Fórum e guias do Cities: Skylines II, https://steamcommunity.com/app/949230/discussions/0/3937895062998876459, https://prodigygamers.com/?p=25789 | **Não é fonte primária** | Só ajudou a entender a ideia de "uma chave para tudo e uma por tipo". Nenhuma constante vem daqui. |

## Constantes

| Constante | Valor | Origem | Situação |
|---|---|---|---|
| `road.free_tolerance_m` | 2 | **Medição** (`rdp_medida.py`, 40 traços por caso, mesmo algoritmo, ruído assumido de 2 px). De perto: tol 5 desvia 5,51 m, 3 desvia 3,48, 2 desvia 2,37, 1 desvia 1,90. Critério: desvio menor que uma faixa (3,3 m da via urbana) | Era 5 (valor do 4.0); trocado pela medição. O ruído de 2 px é suposição, não medi um ponteiro real |
| `road.free_max_controls` | 48 | **Medição do custo**: 35 peças custam 36 us por movimento (`road_commands_test`, chão aberto, máquina carregada). De longe, tol 2 gera ~49 peças num traço de 600 m, então o limite é o que afrouxa a simplificação | Limite de trabalho, escolha de modelagem. Perto de muitas vias o custo sobe: **não medido** |
| `road.snap_pick_px` | 26 | Piso normativo: raio 22 px equivale ao alvo de 44 px (WCAG 2.5.5). 26 está acima dele | **Escolha de modelagem** com piso em norma |
| `road.snap_node_px` | 30 | Maior que `snap_pick_px` para o nó ganhar da via sob ele; mesmo piso | **Escolha de modelagem** |
| `road.snap_land_px` | 40 | Até onde o pouso pode correr do ponteiro: maior que `snap_node_px` | **Escolha de modelagem**, sem medição |
| `road.click_px` | 7 | Piso medido: `SM_CXDRAG` = 4 px. Maior porque o erro aqui é mais caro: um arrasto falso em Curva assenta a via em vez de esperar a dobra | **Escolha de modelagem** com piso medido |
| `road.snap_step_deg` | 15 | Divide 45 e 90, então as diagonais e os eixos entram na mesma escada; precedente no Inkscape. O cone de captura é a metade do passo: derivado, os cones se tocam sem buraco | **Escolha de modelagem** |
| `road.grid_m` | 1 | Unidade das larguras de seção | **Escolha de modelagem** |
| `road.verge_m` | 1 | Faixa além da calçada no raio mínimo e na área de clique | **Escolha de modelagem**, sem fonte; candidata a norma de projeto viário |
| `road.overlap_pad_m` | 10 | Já existia antes deste porte. Afasta o teste de sobreposição das pontas | **Sem fonte.** O 4.0 usa o recuo real do cruzamento (`overlapsAnother`, `editRules.ts`); some quando esse teste for portado |
| `BEND_LIMIT` 0,52 e `BEND_GAIN` 1,36 (`road_path.cpp`) | | Sensação do arrasto em Curva | **Sem fonte, herdado do comportamento do 4.0.** Falta medir ou derivar |
| `LOOK_*` 0,4, 4,8 e 0,2 (`road_snap.cpp`) | | Quanto se anda no braço para ler a direção dele | **Sem fonte, herdado do comportamento do 4.0** |

## Regras de edição

| Constante | Valor | Origem | Situação |
|---|---|---|---|
| `road.sharp_cos` | 0,9063 (25°) | Valor do 4.0 (`MIN_LEG_ANGLE`) | **Escolha de modelagem**, sem fonte. Era 0,94 (20°) e valia só em cruzamento; agora vale em todo pouso. Manuais de projeto pedem bem mais que isso (não li qual); o jogo aceita o que o jogador desenha |
| `road.squeeze_slack_m` | 0,4 | Valor do 4.0 (`SQUEEZE_SLACK`) | **Escolha de modelagem**: folga contra ruído |

## Valores visuais do catálogo

Vêm do 4.0 como referência de visual e de catálogo, não como medição: tons por classe e `surface_colors` (`roadTypes.ts`, `roadTones.ts`), médias da calçada e do meio-fio (`STREET_MEANS` de `roadStandIn.ts`, convertidas para sRGB), tinta `#e1c45a` e `#eee8d7` (`markingStyle.ts`), larguras, velocidades e prioridades (`catalog.ts`, `roadTypes.ts`), prévia (`main.ts`). **Escolha de modelagem do 4.0, sem fonte primária.** Os preços por metro batem com as fotos do 4.0 nos 31 perfis conferidos por conta.

Não mudei `dash_m` e `gap_m` de `streets.json`: o jogo tem 2/4, 3/6 e 3/9 m por faixa de velocidade e o 4.0 tem 3/6, 3/9 e 4/12. O 4.0 cita o MBST vol. IV, mas **não li a norma**; falta conferir antes de escolher.

## Albedo do pavimento

Fontes primárias lidas:

- Pomerantz, Akbari, Chang, Levinson e Pon, *Examples of Cooler Reflective Streets for Urban Heat-Island Mitigation*, LBNL-49283, https://eta-publications.lbl.gov/sites/default/files/lbnl-49283.pdf. Texto extraído do PDF e lido nos trechos sobre albedo. Asfalto fresco: 0,05. Selagens de agregado envelhecidas: cerca de 0,12, "similar to aged AC". Define albedo como razão entre luz solar refletida e incidente, média sobre o espectro solar.
- Heat Island Group, https://heatisland.lbl.gov/coolscience/cool-pavements. Asfalto novo 5 %, envelhecido 10 a 20 %, por oxidação do ligante e exposição do agregado. Não diz como mediu.

**Nenhuma das duas dá refletância visível.** Não achei o artigo de Berdahl e Akbari sobre pavimentos nem li a norma ASTM. Portanto a constante fica marcada: **refletância solar, não visível**. O método do LBNL-49283 é o da ASTM E1918-97, piranômetro a 0,5 m da superfície, que integra o espectro solar inteiro. O jogo usa o valor como ordem de grandeza para um albedo de render, que é visível; o asfalto pode diferir nas duas bandas e não medi isso.

| Via | Albedo linear (Y) | Faixa física |
|---|---|---|
| Antes, todas | 0,074 | Entre novo e envelhecido, sem motivo |
| Urbana `#3a3d3f` | 0,046 | Novo |
| Avenida `#35383a` | 0,039 | Um pouco abaixo de novo |
| Bulevar `#333638` | 0,036 | Um pouco abaixo de novo |
| Local `#5f6365` | 0,123 | Envelhecido |

## Linha central e tracejado

Regra da coordenadora: onde norma e 4.0 diferem na aparência, o padrão é o que o 4.0 desenha, e a norma fica como segunda tabela. Em `streets.json`, `markings.style` escolhe entre `roadcraft` (padrão) e `brasil`.

Texto da norma em `evidence/fontes/mbst-vol-iv-2022.txt`, seção 4.1.

| Item | Padrão, como o 4.0 desenha | Tabela `brasil` | Divergência |
|---|---|---|---|
| Traço e vão, v < 60 | 3 e 6 m (`markingStyle.ts`, estilo `br`, que o jogo novo do 4.0 usa em `main.ts:2021`). Medido em `reference/58-`: traço de 38 px, período de 114 px, razão 0,33 | 2 e 4 m | O quadro da 4.1.2 não tem 3/6 abaixo de 60 km/h, só de 60 a 80 |
| 60 a 80 | 3 e 9 m | 3 e 6 m | Quadro aceita 3/9 aí |
| v ≥ 80 | 4 e 12 m | 3 e 9 m | Quadro aceita 4/12 aí |
| Linha central | 0,16 m (`markings.ts:81`) | 0,10, e 0,15 a partir de 80 km/h | Acima do máximo de 0,15 da norma |
| Linha entre faixas | 0,128 m (`markings.ts:91`) | 0,10, e 0,15 a partir de 80 km/h | |
| Via urbana com mais de uma faixa por sentido, sem canteiro | Linha simples contínua | Linha dupla, 4.1.3 LFO-3 | A norma pede dupla |
| Zebra | Barra de 0,5 m, período de 1,0 m | Barra de 0,35, período de 0,9 | |

Fica sem mudança nos dois: rua local sem linha, uma faixa por sentido tracejada. Não feito em nenhum dos dois: linha contínua de 15 a 30 m na aproximação de semáforo, 4.2.1.

## Constantes que a coordenadora cobrou

| Constante | Antes | Agora | Fonte ou medida |
|---|---|---|---|
| `road.sharp_cos` | 0,9063 (25°), sem fonte | Fica, declarada como regra de jogo | DNIT IPR-718, *Manual de Projeto de Interseções* (2005), 8.4.1, p. 203: 75° a 90° pouco restringidos, 60° aceitável, abaixo disso o realinhamento "deve ser considerado". A norma não dá limite de recusa. Recusar a 60° proibiria as bifurcações que o 4.0 aceita. Abaixo de 60° o jogo aceita o que a norma desaconselha, e não avisa. Green Book: não li o livro; só fontes secundárias (citação da FHWA, 60° a 120°), e `highways.dot.gov` respondeu 403 |
| `road.verge_m` | 1 m, sem fonte, em dois usos | Sai. `road.inner_radius_m` 4,7 | IPR-718, Tabela 6, p. 80: veículo de projeto VP, "Raio min. da roda interna traseira 4,7". A borda interna da pista de uma curva não fecha mais que isso. O outro uso (alcance de pegar a via) passa a ser a própria superfície |
| `squeeze_slack_m` | 0,4 m, cópia do 4.0 | Sai | No 4.0 é ruído do solucionador dele (`networkBuild.ts`, `SQUEEZE_NOISE`). O jogo calcula o aperto de forma determinística: sobra 1e-6 m de aritmética |
| `BEND_GAIN` | 1,36 em código | `road.bend_gain` 1,36, em dado | Comportamento do 4.0 (`roadTool.ts:567`): a curva passa a 68 % do ponto mais distante do arrasto. Opção geométrica: 2, que leva a curva ao ponto, porque B(½) = (P0 + 2C + P2)/4 |
| `BEND_LIMIT` | 0,52 em código | `road.bend_limit` 0,52, em dado | `roadTool.ts:565`. Opção geométrica: 0,5, o semicírculo |
| `LOOK_MIN_M`, `LOOK_MAX_M`, `LOOK_SHARE` | 0,4, 4,8, 0,2 | Saem | A direção do braço é a tangente exata, `arm_of`, o mesmo dono da junção |
| `overlap_pad_m` | 10 m, sem fonte | Sai | Recuo real da junção que cada ponta faria, `end_mouth`, como o 4.0 lê os recuos em `overlapsAnother` e `withinSharedPlate` |


## Fotos, uma a uma

Em `fotos.md`, que abre com o estado atual.
