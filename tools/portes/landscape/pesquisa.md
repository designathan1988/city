# Pesquisa do porte landscape: fonte de cada modelo

Como ler a coluna **Lida**:
- **direta**: li o texto da própria fonte;
- **citada**: li um trabalho que cita a fonte, ou só o trecho que a busca devolveu, não a fonte inteira;
- **não lida**: não consegui abrir a fonte. O número continua no jogo e está marcado aqui e no `_doc`.

A coluna **Origem** diz de onde veio o que está no código: **4.0** (comportamento portado, depois conferido na fonte), **fonte** (escolhido da fonte) ou **modelagem** (sem valor publicado).

## Motor e renderização

| Modelo | Fonte | Lida | Conclusão | Origem |
|---|---|---|---|---|
| Passe de tela cheia da atmosfera | [Godot, Advanced post-processing](https://docs.godotengine.org/en/stable/tutorials/shaders/advanced_postprocessing.html) | direta | Quad 2x2 em clip space, `POSITION = vec4(VERTEX.xy, 1.0, 1.0)` desde a 4.3 (reverse-Z), NDC diferente na Compatibility. O shader segue isso. | fonte |
| Mapa de controle do chão pintado | Terrain3D, documentação do formato do control map | direta | Bits de base, overlay, blend, uv, buraco e autoshader usados como superfície, bioma, quantidade e ravina. | fonte |
| Normal dos cartões de folha | [scene_forward_clustered.glsl](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/shaders/forward_clustered/scene_forward_clustered.glsl), [scene_shader_forward_clustered.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/scene_shader_forward_clustered.cpp) | direta | `cull_disabled` define `DO_SIDE_CHECK`, que inverte a normal da face de trás antes do código do fragmento. Nossos cartões saíam de costas: lado do sol escuro. É a causa provada do "coração escuro". Corrigido no enrolamento dos cartões. | fonte |
| Mapa de terra e rocha | Arquivo `.import` do projeto (`importer="2d_array_texture"`, 9 fatias) | direta | O mapa é um array de texturas e o shader o declarava `sampler2D`: toda leitura de terra e rocha dava o valor padrão. Causa do solo cinza e da rocha branca. Agora `sampler2DArray`. | fonte |
| Grão da grama | Terreno do 4.0 (`terrain.ts`: grão por nível de pixel) | direta | Portado o comportamento: o mapa relido na escala do pixel. Força ajustada pela medida do mesmo campo. | 4.0 |
| Filtro anisotrópico | Configuração do projeto, sem valor definido (padrão do Godot) | **documentação não confirmada**: a página da classe é longa demais para a leitura | Ligado no máximo para o campo rasante manter o grão. O efeito está medido nas fotos, não na documentação. | medição |

## Nuvens, neblina e céu

| Modelo | Fonte | Lida | Conclusão | Origem |
|---|---|---|---|---|
| Forma da nuvem | [Schneider & Vos, The Real-time Volumetric Cloudscapes of Horizon Zero Dawn, SIGGRAPH 2015](https://www.advances.realtimerendering.com/s2015/The%20Real-time%20Volumetric%20Cloudscapes%20of%20Horizon%20-%20Zero%20Dawn%20-%20ARTR.pdf) | direta (slides 35, 37, 76) | Forma de base vezes sinal de altura, depois erosão por subtração de um segundo ruído, só na borda. O `cloud_field` faz isso, com os puffs do 4.0 no lugar da textura de base. | 4.0, confirmado na fonte |
| Luz na nuvem | Mesma fonte, slides 52 a 55 | direta | Lei de Beer para a transmitância e Henyey-Greenstein para a anisotropia. O jogo usa Beer. **Não usa Henyey-Greenstein**: a luz da nuvem é modelagem simples, o clarão contra o sol não existe. Fica como diferença declarada. | 4.0 e modelagem |
| Sombra da nuvem no chão | Corda do raio do sol na esfera de cada puff, com Beer | - | Atalho analítico no lugar de marchar o volume. Fator 2,2 é modelagem. | modelagem |
| Fiapos da neblina | [Vlachos, Water Flow in Portal 2, SIGGRAPH 2010](https://www.advances.realtimerendering.com/s2010/Vlachos-Waterflow(SIGGRAPH%202010%20Advanced%20RealTime%20Rendering%20Course).pdf) | direta (slide 23) | Duas camadas defasadas em meia fase, para esconder o reinício de cada uma. O atmosphere faz isso com o ruído 3D. | fonte |
| Altura da neblina | [AMS Glossary, shallow fog](https://glossary.ametsoc.org/wiki/shallow-fog/), [WMO, tabela 0-20-003](https://ci.codes.wmo.int/bufr4/codeflag/0-20-003/_12:1) | citada (trecho de busca das próprias páginas) | Neblina rasa é a de até 2 m em terra: confirma o mínimo. **Corrigido:** o `_doc` dizia "profunda acima de 100 m" citando a WMO, e isso não foi achado. O máximo de 160 m agora consta como modelagem. | fonte e modelagem |
| Gradiente do céu | Fotos minhas do 4.0 (`reference-minha/03.jpg`) | medição | Radiância pelo inverso do ACES: céu medido (0,27; 0,52; 0,85) contra gradiente (0,24; 0,51; 0,87), o que valida o caminho. Cores recalculadas para o que o 4.0 mostra. | medição |

## Noite

| Modelo | Fonte | Lida | Conclusão | Origem |
|---|---|---|---|---|
| Luz do luar e do céu | [Kyba, Mohar & Posch 2017, How bright is moonlight?, Astronomy & Geophysics](https://fgga.univie.ac.at/en/research/publications/publications-detail-view/pure/a574d8b2-bc2a-441a-96ea-74134b686215/show/publ/Pure) | citada (resumo e trecho de busca) | Lua cheia típica de 0,05 a 0,1 lx, até 0,32 lx nos trópicos; céu sem lua cerca de 2 mlx. A razão física contra o sol não serve à tela: um olho adaptado não é uma câmera de exposição fixa. A fonte dá a ordem entre lua e céu, a medição dá o nível. | fonte e medição |
| Nível da noite | Fotos do 4.0 da sessão de prédios (`ports/build/reference`) | medição | Parede mediana a 22h: 0,019 contra 0,004. Pelo inverso do ACES isso é ganho 2,66 em radiância, aplicado à lua e ao ambiente. Depois, 0,017 no nosso contra 0,013 na foto `09-hora2200-centro`. | medição |

## Chuva, raios e vento

| Modelo | Fonte | Lida | Conclusão | Origem |
|---|---|---|---|---|
| Queda da gota | [Gunn & Kinzer 1949, J. Meteor. 6(4), 243-248](https://doi.org/10.1175/1520-0469(1949)006%3C0243:TTVOFF%3E2.0.CO;2) | citada | Artigo e método confirmados. **A tabela não foi lida**: os 8,8 m/s e 9,17 m/s não têm conferência direta. Um resumo secundário dá cerca de 10 m/s para as maiores gotas, compatível. | 4.0, tabela não conferida |
| Raios: golpes por descarga | Rakov & Uman 2003, por trecho de busca de um resumo do livro (documento não identificado entre os resultados) | citada | 3 a 5 golpes por descarga, intervalos de dezenas de ms. | 4.0, confirmado por citação |
| Raios: intervalo e duração | Trechos de busca: estudo de 1992 com 199 intervalos em 46 descargas, média geométrica 57 ms; rede austríaca, 175 ms de média | citada | `stroke_s` 0,06 confere. `strike_life_s` 0,45 é mais longo que a média medida, é escolha para o jogador ver o raio. | 4.0 e modelagem |
| Trovão | Som a 343 m/s no ar a 20 °C | física conhecida, sem leitura | Atraso pela distância. O ruído marrom filtrado é modelagem. | modelagem |
| Vento máximo | [MeteoSwiss, tabela Beaufort](https://www.meteoswiss.admin.ch:443/dam/jcr:c1064242-4a6e-4df2-8d21-96e68d1def7a/Beaufort_Tabelle_en.pdf) | citada (trecho de busca) | Força 11 é 28,5 a 32,6 m/s. O limite de 30 m/s fica nela. | fonte |
| Curvatura e rajada das árvores | Sousa, GPU Gems 3 cap. 16; Peltola 1993; Kaimal 1972; EN 1991-1-4; Jackson 2019 | citada no projeto antes deste porte | Já estava no `vegetation.gdshaderinc`, não foi relida agora. | anterior |
| Correnteza até 4 m/s | USGS, medição de vazão | **não lida** | Citação de memória. Fica como modelagem até a leitura. | modelagem |

## Água

| Modelo | Fonte | Lida | Conclusão | Origem |
|---|---|---|---|---|
| Ondulação e reflexo | JONSWAP, Phillips 1958, Cox & Munk 1954 | citada no projeto antes deste porte | Sem mudança neste porte. | anterior |
| Cor da água | Inverso do ACES sobre o rio capturado | medição | Antes as cores de tela eram usadas como albedo, e o rio saía quase branco. Albedo agora calculado pelo ganho medido (2,27; 1,98; 1,57). Alvo de tela segue vindo de `14_lago_perto.jpg`, foto da coordenadora: minhas fotos não têm água. | medição |

## Ecologia

| Modelo | Fonte | Lida | Conclusão | Origem |
|---|---|---|---|---|
| Sistema de formações | IBGE, Manual Técnico da Vegetação Brasileira, 2ª ed., 2012 | **não lida inteira**: o [PDF da UFRRJ](https://arquivos.ufrrj.br/arquivos/20230911682bdc3789538c4db60f2fb9d/manual_tecnico_vegetacao_brasileira_IBGE_2012.pdf) passa do limite de 10 MB da leitura | Li só o [trecho do IBGE sobre a Savana-Estépica](https://redepermacultura.ufsc.br/permabrasuca/wp-content/uploads/sites/6/2024/12/SES.pdf): sem dominância clara de árvores, lenhosas espinhosas, cactáceas e bromeliáceas dominantes, solo raso e pedregoso, quatro formações. | 4.0, parte confirmada |
| Cerradão | Ribeiro & Walter 2008, por trecho de busca de um estudo que cita (artigo não identificado entre os resultados) | citada | Cobertura arbórea de 50 a 90 %. `closed_canopy` + `trees` = 0,9, a ponta densa. | 4.0, dentro da faixa |
| Cerrado denso e típico | Ribeiro & Walter 1998, em [Pontes, Ferreira & Sano 2009](https://www.alice.cnptia.embrapa.br/alice/bitstream/doc/557301/1/Detalhamento-fitofisionomias-bioma-2009.pdf) | citada | Cobertura de 20 a 70 %. **Corrigido:** `trees` era 0,24, valor do 4.0; agora 0,45, o meio da faixa. | fonte |
| Mata de galeria, vereda, ralo | Ribeiro & Walter | **não lida** | Procurei 70-95 %, 5-10 % e 5-20 % e não confirmei: esses números não estão no jogo como fonte. | modelagem |
| Cordilheiras e capões do Pantanal | Trechos de busca de estudos da Nhecolândia e do Norte do Pantanal (artigos não lidos) | citada | Diques antigos de 80 a 300 m de largura e 1 a 2,5 m de altura, não inundáveis, com cerradão ou floresta semidecídua. É o que `cordilheira_*` faz. | 4.0, confirmado por citação |
| Campo rupestre, Pampa, Mata Atlântica, Amazônia | IBGE 2012 | **não lida** | Sem número publicado lido. Tudo como modelagem no `_doc`. | 4.0 e modelagem |
| Classes de relevo | Embrapa, por trechos de busca de trabalhos do acervo dela e de artigos que a citam (1999, 2018) | citada | Plano até 3 %, suave ondulado até 8 %, ondulado até 20 %, forte ondulado até 45 %, montanhoso até 75 %, escarpado acima. **Corrigido:** o `_doc` citava a classe mas os valores eram outros (5, 15, 40, 56 graus). Agora 4,6, 11,3, 24,2 e 36,9 graus. | fonte |
| Faixa ciliar | Lei 12.651/2012, art. 4º, I, a | **não lida nesta pesquisa** | Citação de memória: 30 m para rios de menos de 10 m. A busca não retornou o texto da lei. | a conferir |
| Distância à água | Borgefors 1986, chanfro 3-4 | citada no código | Algoritmo de dois passes. | 4.0 |
| Misturas das comunidades | Nenhuma | - | As 42 constantes saíram de `ecology.cpp` para `nature.json`, `ecology.mixes`. O `_doc` diz, por grupo, o que vem da descrição de campo e o que é modelagem. | modelagem |

## O que ficou sem conferência direta

1. Tabela de Gunn & Kinzer (8,8 m/s).
2. Manual do IBGE fora da Savana-Estépica, por limite de tamanho da leitura.
3. Capítulo de Ribeiro & Walter 2008, só pelas citações.
4. Lei 12.651/2012 e USGS.
5. Henyey-Greenstein está na fonte, mas o 4.0 não usa: o halo do sol dele é `sunSide`, portado em `sky.gdshader`.


## Luz global

| Modelo | Fonte | Lida | Conclusão |
|---|---|---|---|
| Sol na superfície | https://en.wikipedia.org/wiki/Sunlight | direta | cerca de 98.000 lx; o artigo calcula, sem citar medida; energia 0,89 representa isso |
| Tabela de cor | https://docs.godotengine.org/en/stable/classes/class_environment.html | direta | `adjustment_color_correction` aceita Texture3D |
| Ordem no tom | `servers/rendering/renderer_rd/shaders/effects/tonemap.glsl`, ramo master | direta | a tabela é lida em cor de tela, sem remapear meio texel |
| Gradação de filme | `render/postprocess.ts`, GRADED_OUTPUT_FRAGMENT | 4.0 | aspecto do 4.0, sem fonte externa |
| Céu limpo, 10 a 20 klx | não lida | não lida | número de memória, marcado como escolha de modelagem no `_doc` |
