# Pesquisa do porte de vias

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

Texto da norma em `evidence/sources/mbst-vol-iv-2022.txt`, seção 4.1.

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
