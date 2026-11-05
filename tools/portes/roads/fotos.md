# Fotos de vias, uma a uma

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

## Histórico

Rodadas antigas, com a câmera, a lente ou o preset de cada época. Valem como registro do caminho, não como prova.



Fotos do 4.0 em `reference/` (tiradas por mim com `port.ts reference`, cidade pequena, seed 7). Fotos do jogo em `evidence/fotos-antes3` (a11384a) e `evidence/fotos-depois3` (7dcf184), mapa 0, as duas com o sol das 16h. `fotos-depois2` e `fotos-antes` são do meio-dia e não servem para cor. A pasta `reference/` tem fotos de sessões diferentes com o mesmo número: vale o nome inteiro.

### 4.0

| Foto | O que mostra | Resultado |
|---|---|---|
| `06-city-junction-local-straight-down` | Cruzamento de vias urbanas de cima, entre torres | Asfalto escuro azulado, sem linha central visível de tão de cima, zebras com retenção, fios e árvores. Serve para tom e geometria da esquina |
| `06-junction-avenue-top` | Avenida na cidade | Canteiro verde, zebra por sentido, retenção junto da zebra |
| `09-junction-avenue-low` | Câmera atrás de prédio | **Não mostra via. Inútil** |
| `15-junction-local-low` | Câmera dentro de prédio | **Não mostra via. Inútil** |
| `18-junction-t-top` | T de avenida | Asfalto escuro, faixas, estacionamentos de lote |
| `26-drawer-draw` | Gaveta Desenhar | Painel esquerdo com Traçado (4), Encaixe (3), Faixas (4), Largura, Altura, Corte, Mobiliário, Teclas, Vagas (6). Abas Ruas 12, Avenidas 10, Rodovias 5, Especiais 4, Meus modelos 0. "Via urbana" é a escolhida ao abrir. Cartão com lápis para editar perfil |
| `29-catalog-avenues` | Avenidas | 10 cartões, de 18 m R$ 2.420/m a 27 m R$ 3.300/m. Faixa de ônibus vermelha |
| `32-catalog-highways` | Rodovias | 5 cartões, 20 m R$ 2.640/m, 29 m R$ 3.880/m, 10, 14 e 12 m |
| `35-catalog-special` | Especiais | Alça 6 m, alça 2 faixas 10 m, rural de paralelepípedo 8 m, concreto 12 m |
| `40-tab-upgrade` | Melhorar | Seis cartões em linha: residencial, urbana, avenida, bulevar, rodovia, alça. É a escada de classes |
| `43-tab-move` | Mover | Aba ligada, dica "Arraste um nó para reposicionar" |
| `46-tab-split` | Dividir | Dica "Clique numa via para dividir" |
| `49-tab-control` | Controle | Círculo com "A" em todo cruzamento, dica de clicar para mudar o controle, Shift volta |
| `52-tab-roundabout` | Rotatória | Barra Raio 40 m à esquerda, dica de ligar os quatro acessos |
| `55-free-junction-avenue-top` | Avenida x avenida em chão livre | Linha amarela contínua no meio, tracejado branco entre faixas |
| `58-free-junction-avenue-low` | Idem, baixa | Semáforos, pontas retas ao fundo |
| `58-free-junction-urban-top` | Urbana x urbana | Linha central amarela **tracejada** |
| `61-free-junction-t-top` | T avenida com urbana | Avenida sem canteiro: amarela contínua e tracejado branco. Urbana: amarela tracejada. Zebra só onde há braço |
| `61-free-junction-urban-low` | Urbana, baixa | Calçada de paver cinza, hidrantes vermelhos, placas, postes de madeira com fios, semáforos com braço |
| `61-tool-chain-straight` | Prévia da via | A via desenhada com bordas verdes, círculos nas pontas, etiqueta `270 m · R$ 410.400` junto do fim |
| `64-city-junction-avenue-top` | Avenida na cidade | Canteiro verde de 2 m, duas zebras por braço |
| `67-city-junction-avenue-low` | Idem, baixa | Boa para semáforo, placa, luminária e guia rebaixada |
| `70-city-junction-t-top` | T na cidade | O lado sem braço fica contínuo, sem zebra |
| `64-free-road-end` | Ponta de via | A pista vai até o fim e a calçada acaba rente, corte reto |
| `64-tool-laid-straight` | Via assentada | Corte reto, sem balão |
| `69-tool-landing-on-road` | Via encostando em outra | Mostra cruzando, não pousando. Pouco útil |
| `72-tool-laid-t` | T criado por ferramenta | Zebra nos três braços |
| `83-tool-curve-bend` | Dobra da curva | Prévia com a via desenhada |
| `86-tool-laid-curve` | Curva assentada | Dois cortes retos, faixas acompanham |
| `96-profile-editor` | Editor de perfil | Seção desenhada, lista de elementos, materiais |
| `12-tool-free-wavy` | Traço à mão assentado | Curvas suaves |
| `17-tool-roundabout-tab` | Rotatória sobre via | Círculo tracejado vermelho: recusada |
| `20-tool-roundabout-preview` | Rotatória livre | Círculo tracejado branco |
| `23-tool-roundabout-laid` | Rotatória assentada | Quatro acessos prontos, em asfalto **cinza claro**: classe local |
| `28-roundabout-top` | Rotatória de cima | Triângulos de dê a preferência e faixas nas entradas |
| `31-roundabout-low` | Rotatória baixa | Asfalto cinza claro da classe local, árvores do miolo tapam parte |
| `34-road-end-bulb` | Balão de retorno | Asfalto cinza claro da classe local; quadrados da textura aparecem como no jogo; seta branca no chão |

### Jogo, depois (`fotos-depois3`)

| Foto | Contra o 4.0 | Contra o antes (fração de pixels com diferença clara) |
|---|---|---|
| `cross_urban_top` | Asfalto claro demais, calçada creme, linha dupla onde o 4.0 tem tracejada, largura certa por acaso (12 m) | 1,1 %, ruído de árvores e sombras |
| `cross_urban_low` | Idem; sem semáforo nem placa; árvore na calçada que o 4.0 não põe | 2,5 % |
| `cross_avenue_top` | Avenida com linha amarela e tracejado, mas a foto cruza avenida com urbana; no 4.0 é avenida com avenida | 2,6 %: as pontas viraram corte reto |
| `cross_avenue_low` | Idem, zebra mais larga que a do 4.0 | 4,0 % |
| `t_top` | Forma boa: lado sem braço contínuo. Tom errado | 1,6 % |
| `road_end` | **Enquadramento ruim, a ponta ocupa pouco.** A calçada fecha a pista em U; no 4.0 a pista segue até o corte | 6,6 %: o balão saiu |
| `curve_top` | Curva suave, faixas acompanham | 0,8 % |
| `layout_top` | Conjunto sem balões | 2,6 % |
| `tool_drawer` | Faltam Faixas, Largura, Altura, Corte, Mobiliário, Teclas, Vagas, o 4º traçado e Meus modelos; Mover desligado; escolhida "Rua local", com faixas verdes no cartão; 13/10/6/4/3 cartões; 11,8 m R$ 1.378/m | 1,7 % |
| `tool_chain` | Prévia é faixa verde lisa, etiqueta presa no topo da tela | 2,0 % |
| `tool_curve_bend` | Idem | 4,9 % |
| `tool_free_stroke` | Forma boa, prévia lisa | sem antes |
| `tool_free_laid` | Boa | sem antes |

A barra de cima mostra 06:30 com o sol das 16h: é o relógio da simulação, parado na captura; não é deste porte.

### Causas achadas nas fotos

1. **Tom do asfalto.** Três causas somadas, medidas e lidas no código:
   - O 4.0 tinge o asfalto por classe de via (`roadTypes.ts`, `roadTones.ts`): local `#5f6365`, urbana `#3a3d3f`, avenida `#35383a`. O jogo usa `#4a4d53` para tudo. Por isso as fotos 23, 31 e 34 têm asfalto claro e as outras escuro: minha hipótese antiga de "só em chão livre" já caíra, e esta é a explicação certa.
   - O 4.0 multiplica a textura por classe/referência, com média da textura 0,041 linear. O jogo normaliza pela média e multiplica pela cor da faixa.
   - Luz e curva de tom: `exposure` 1,4, `tonemap_white` 6, `sun_energy` 1,8 contra `toneMappingExposure` 1 e sol 4,2/pi do 4.0. O ajuste ACES é o mesmo nos dois, mas tem pé quadrático: um ganho de 2 na entrada vira perto de 4 no escuro. Isso não é deste porte; ver `roads-luz.md` nas notas.
2. **Linha central.** O 4.0 decide pela classe: local sem linha, urbana tracejada, avenida sem canteiro contínua, com canteiro nenhuma. O jogo tem `centre` por perfil vindo de uma versão antiga.
3. **Catálogo.** `make_road_profiles.py` foi escrito de uma versão antiga do Roadcraft: outras larguras, outros perfis.

### Depois do catálogo (`evidence/fotos-t4`, commit 09d23df, lista em `evidence/fotos-t4/lista.md`)

Medido em corte transversal e por mediana, mesmo sol (`scratchpad/tons3.py`, `tons4.py`):

| Superfície | 4.0 | Jogo antes | Jogo agora |
|---|---|---|---|
| Asfalto da via urbana | 54 a 56 | 135 | 94 |
| Calçada | 150 a 170 | 206 a 221 | 162 a 185 |
| Capim | 104 | 98 | 98 |

A calçada chegou no tom do 4.0. O asfalto caiu pela metade do caminho: o que falta é a luz e a curva de tom globais (`roads-luz.md`), que não são deste porte. Não escureci a cor da classe além do valor do 4.0, porque isso esconderia a causa.

| Foto | Contra o 4.0 |
|---|---|
| `cross_urban_top` | Tracejado amarelo simples, 12 m, calçada cinza, sem árvore na calçada de 2 m. Semáforo e placas ainda são do porte de mobiliário |
| `cross_avenue_low` | Avenida x avenida sem canteiro, amarela contínua e tracejado branco, como `58-free-junction-avenue-low`. Os semáforos aparecem. Postes claros contra os de madeira do 4.0: porte de mobiliário |
| `road_end` | Pista até o corte, calçada rente, sem U, sem árvore na pista |
| `tool_drawer` | Ruas 12, Avenidas 10, Rodovias 5, Especiais 4, Via urbana escolhida, 12 m R$ 1.520/m. Ainda há a aba Trilhos e falta Meus modelos |

Árvores na pista (`road_end` antigo): eram árvores de rua plantadas na calçada, com a copa sobre a pista, não vegetação não limpa. O 4.0 só planta onde sobra faixa livre (NBR 9050, 1,2 m): uma calçada de 2 m não recebe. Com a calçada medida como a do 4.0, a regra do jogo (`fits_walk`) também deixou de plantar.

### Opções, Mover e Controle

`evidence/fotos-t6` (4cbd29e), `fotos-t8` e `fotos-t8b`, listas nas pastas.

| Foto | Contra o 4.0 |
|---|---|
| `fotos-t6/tool_drawer` | Traçado, Encaixe, Faixas, Largura com Padrão, Vagas, como `26-drawer-draw`. Faltam Altura e Corte (`road-elevation`), Mobiliário (clone do Porte 6), Teclas e o quarto traçado |
| `fotos-t8/tool_control_signal` | "S" amarelo no cruzamento e dica Semáforo, como `49-tab-control` |
| `fotos-t8/tool_move`, `tool_moved` | **Inúteis:** o teste moveu o ponteiro num passo só, sem arrasto, e a via não saiu do lugar. Sonda provou a causa e a captura passou a arrastar em seis passos; a ferramenta agora julga o arrasto também ao soltar |
| `fotos-t8b/tool_move` | Vias seguem o ponteiro. A faixa é lisa e opaca: mostrou que eu tinha tornado opaca a prévia das outras ferramentas, corrigido. O 4.0 reconstrói a via real durante o arrasto; aqui é só a faixa |
| `fotos-t8b/tool_moved` | Nó no lugar novo, cruzamento refeito com zebras, vias com a forma mantida, nada sobrando |

### Quadrados no asfalto, noite, save e custo

`evidence/fotos-t10` (lista na pasta), antes em `evidence/fotos-t4`, recortes em `evidence/quadrados/`, medida em `evidence/quadrados/contraste.py`.

| Foto | O que mostrou |
|---|---|
| `fotos-t10/road_end` contra `fotos-t4/road_end` | Mesma câmera. Ruído trocado por `lattice_noise`: diferença máxima de 6 níveis em 255, 0,02 % dos pixels acima de 4. **Os quadrados continuam:** o ruído nunca foi a causa |
| `fotos-t10/cross_urban_top` contra `fotos-t4` | 0,25 % dos pixels acima de 4, 0,01 % acima de 16 (agentes e copas, não o asfalto). Quadrados iguais |
| `quadrados/textura_contraste` | A causa: os remendos estão assados na textura `asphalt.map.png`, vinda da receita do 4.0 (`src/render/bake/roadRecipes.ts:48-50`, "Patches") |
| `quadrados/4_0_bulb`, `4_0_cidade`, `jogo_cross`, `jogo_road_end` | Recorte ampliado dos dois lados: o 4.0 tem os mesmos remendos. Contraste (luminância linear, remendo sobre o anel em volta): 4.0 0,699 e 0,695; jogo 0,775 e 0,748. O remendo do jogo é mais fraco que o do 4.0, não mais forte. Um terceiro ponto do 4.0 (1,069) caiu fora de remendo e foi descartado. Só dois remendos por jogo: é uma amostra, não uma estatística |
| `fotos-t10/night_urban_top`, `night_urban_low`, `night_avenue_low` contra `reference/56-`, `59-`, `62-`, `65-night-free-*` | Primeira noite de via do jogo, e está errada: a pista inteira banhada em bege, zebras brilhando, placa do cruzamento iluminada por igual, esquinas escuras azuladas, capim e árvores quase pretos. O 4.0 tem asfalto escuro com poças redondas e capim verde escuro. Poça e esquina: a Porte 6 já corrigiu no clone dela (`ef850d5`, `0a342bd`, que mexem em `road.gdshader`, `road_junction.cpp`, `materials.json`); esta foto é do jogo sem esses commits. Capim: ambiente noturno, Porte 3 |

### Linha central

`evidence/fotos-t11` (lista na pasta), antes em `evidence/fotos-t10`. Só as cenas afetadas.

| Foto | O que mostrou |
|---|---|
| `fotos-t11/cross_avenue_top` | Duas linhas amarelas nos quatro braços da avenida, separadas de 0,10 m, chegando à zebra. Sem pisca. As linhas pretas finas ao lado são sombra dos fios |
| `fotos-t11/cross_avenue_low` | De perto as duas linhas seguem separadas, longe viram uma só, como é esperado de 0,10 m. Chega à zebra sem cruzar a retenção do outro lado |

### Paridade da via, com par no mesmo enquadramento

Regra nova: nenhuma foto entra como boa sem o par e a lista do que sobra. As análises acima de `fotos-t4`, `t6`, `t8` e `t8b` foram feitas sem par e **não valem**: "nada sobrando" em `fotos-t8b/tool_moved` estava errado. A revisão foto a foto ainda não está completa.

Pares em `evidence/paridade/`, em cima o 4.0, embaixo o jogo.

| Par | O que mudou | Defeitos que sobram |
|---|---|---|
| `par_urban_low_luz3`, `par_urban_top_luz3` (luz da Porte 3, ramo temporário) | Asfalto carvão azulado, calçada cinza, sem faixa creme, zebra larga, tracejado 3/6 | Asfalto e calçada mais escuros que o 4.0, causa não achada. Retenção a uns 2 m da zebra, no 4.0 a 0,64 m (Porte 6). Câmera do par baixo não bate: o jogo está mais longe. Cena do jogo tem árvores onde o 4.0 tem campo |
| `par_avenue_top` (luz velha) | Linha simples de volta | Tom é da luz velha. Retenção como acima |
| `fotos-t13/bend_top` (sem par) | Sem degrau de asfalto claro na dobra | Faixa mais escura atravessa a placa, onde a sarjeta acaba; canto baixo com uma pequena reentrância. O 4.0 põe zebra numa dobra forte (`12-tool-free-wavy`), o jogo não |
| `par_urban_top_luz6` (média certa e luz atual da Porte 3, ramo temporário) | Via e calçada quase no tom do 4.0, medidas em `roads-luz.md` | Asfalto 47 53 61 contra 52 56 65, calçada 151 contra 157. Tinta branca clara demais, 233 contra 227. Retenção longe da zebra (Porte 6). Câmera do par não bate: o cruzamento sai deslocado e o jogo tem árvores onde o 4.0 tem campo |
| `fotos-t15/t_close` contra `61-free-junction-t-top` | Dois cantos com o mesmo raio; a causa era o toco de 10 m junto de um corte da via | Só comparei os dois cantos entre si, sem câmera igual à do 4.0. Faixa escura da dobra e nó movido não refeitos |
| `no_movido_esquina` (de `fotos-t8b/tool_moved`) | Nada ainda | Trecho curto entre a dobra e o T com zebra e retenção: não refeito com o código novo |

Linhas tracejadas partidas em pedaços na via diagonal: são a sombra dos fios sobre a pintura, e o 4.0 faz o mesmo (`64-free-road-end`). Não é defeito da via.

### Canteiro e placa do cruzamento (`fotos-par4` a `fotos-par7`)

Ramo temporário `measure/luz`: a prova final de paridade precisa sair com a luz integrada. Listas em cada `lista.md`. `user://` ainda compartilhado nestas rodadas.

| Par | Defeitos que sobram, lidos em resolução cheia |
|---|---|
| `paridade/par5_median_low.png` | Grama na mesma cor. A frase antiga sobre a largura do meio-fio estava errada: não medi; a medida está em `par8_canteiro_8x`, abaixo. Sem árvores no canteiro e com árvores e poste na calçada (Porte 6). Céu e fundo diferentes: a cena do jogo tem mata atrás |
| `paridade/par5_median_paved_low.png` | Piso e meios-fios batem. Postes de luz sobre o canteiro e a calçada: no 4.0 essa rodovia não tem poste (Porte 6). Sombra de poste longa na pista, junto do mesmo item |
| `paridade/par5_median_nose_low.png` | Saiu com rua urbana no lugar da local do 4.0, por erro meu de cena: a diferença de asfalto daqui não vale. Tracejado amarelo cruzando a zebra, corrigido depois |
| `paridade/par6_median_nose_low.png` | Placa com X duro nas bissetrizes, criado pela minha correção. Corrigido depois |
| `paridade/par6_pair_urban_top.png` | Riscos de sombra de fio sobre a tinta de longe. Zebra mais comprida que a do 4.0 e retenção mais longe (Porte 6) |
| `paridade/par7_miolo_3x.png` | Sem X. O claro da local chega menos longe que no 4.0 e sem borda marcada. Parede riscada entre calçada e grama (`road-elevation`). Sem piso de rampa na esquina. Borda em escada. Grão azulado na local, mais forte que no 4.0 |

### Resposta à revisão (`fotos-par8`, `fotos-par8-sem-fios`, commit 945fbea)

Primeira rodada com `user://` próprio e com MSAA. Ramo temporário `measure/luz`.

| Par | Defeitos que sobram, lidos em resolução cheia |
|---|---|
| `paridade/par8_canteiro_8x.png` (4.0, jogo, jogo sem fios) | Com fios: duas linhas finas dentro da grama, agora contínuas, e serra leve no meio-fio de baixo, onde um fio passa por cima. Sem fios: nada na grama, meio-fio contínuo. Meio-fio de baixo 0,4 px mais estreito que no 4.0 |
| `paridade/par8_miolo_3x.png` | Claro da local até o nó, como no 4.0. Grão azulado no claro, mais forte que no 4.0. Parede entre calçada e grama, sem listras, ainda sem talude. Sem piso de rampa na esquina. Poste e fios de outra porta. Sombra do poste na placa, ausente no 4.0 porque o poste dele é outro |

### Base integrada: merge c62f275, talude 9485433 (`fotos-par9`, `fotos-v2`, `fotos-v3`)

Sem ramo temporário: luz da Porte 3, cidade da Porte 2, `user://` próprio, MSAA do jogo, `CaptureWait`. `fotos-v1` não vale: o build falhou e saiu com a DLL antiga.

| Par | Defeitos que sobram, lidos em resolução cheia |
|---|---|
| `paridade/par9_median_nose_low.png` | Ponto sem uso igual ao `par8`, salvo a luz: faixa 50 54 61 contra 47 52 60; o 4.0 dá 52 56 64. Grama 35 79 24 contra 37 78 25 do 4.0 |
| `paridade/v2_calcada_3x.png` | Sem parede. Pedras no sentido do 4.0. Meio-fio do canteiro igual a olho, face mais fraca (medida em `elementos.md`) |
| `paridade/v3_median_nose_low.png` | Geometria e cor no mesmo lugar. Zebra mais comprida e retenção mais longe (Porte 6). Postes, fios, semáforos e placas de outra porta. Sem carros: a cena é de chão livre |
| `paridade/v3_miolo_3x.png` | Claro da local até o nó, com miolo menos pálido: 95 102 108 contra 107 111 121. Grão azulado mais forte que no 4.0 na local. Mancha escura no canto do lote. Sombra de poste que o 4.0 não tem ali |
| `fotos-par9-uso/05_transito.png` contra `fotos-v3-semuso/05_transito.png` | Trilha mais escura numa faixa só. Pista muito escura na sombra do cânion. Sem par com o 4.0 |

