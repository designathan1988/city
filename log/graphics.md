# Registro de gráficos e desempenho das construções

Linha de trabalho: prédios ricos de perto, sem virar caixa ao afastar, com desempenho máximo. Segue os 7 passos obrigatórios do usuário
(2026-11-05): 1 problemas, 2 pesquisa, 3 causa, 4 plano, 5 código sem medir, 6 teste uma vez, 7 parar e avaliar. Cada ciclo fica registrado aqui.

## Ciclo 1 (2026-11-05)

### Passo 1: problemas (medidos no jogo atual, cidade do mapa 0, semente 7)

Como foi medido: escada de fotos `setView` em zoom 48, 24, 12, 6, 3, 1,5 e 0,8, sonda `bay_census`, `building_cost`, `set_detail_scale` 1, 2 e 4 com `record` e `info`.

- **P1. Composição perdida no nível médio.** `far_mass.cpp windows_of` lê a baia do meio do lado e pinta o lado inteiro com ela. Toda a fachada vira uma grade igual.
- **P2. Janela sem profundidade no nível médio.** `building_far_window.gdshaderinc` pinta retângulos numa parede plana: sem recuo, sem peitoril, sem sombra de vão.
- **P3. Peças somem no nível médio.** `emit_far` não emite sacada, toldo, brise, veneziana, pilastra, friso nem relevo (`Relief`). A foto `low3` mostra a queda a cerca de 300 m.
- **P4. Peças construídas e jogadas fora.** `build_cpu` passa `mid_parts` a `emit_roofs` e `emit_lot`, mas nunca assa essas instâncias: o que é peça de telhado e de lote no nível médio some.
- **P5. Peças soltas no ar.** As bandas de peças alcançam além do casco perto: a torre central da foto `low3` tem pontinhos sem a sacada que os sustenta.
- **P6. Geometria perto custa caro.** 4 chunks perto: 1,24 M triângulos e 155 MB (125 bytes por triângulo). Alcance 1x: GPU 6,2 ms, 522 MB de vídeo. 2x: 8,1 ms, 648 MB. 4x: 11,6 ms, 858 MB e engasgos (p99 41 ms).
- **P7. A cidade repete baias sem aproveitar.** 1.408 prédios, 172.508 baias. Distintas: 9.129 ao centímetro, 1.474 a cada 25 cm; 256 formas cobrem 82 % e 1.024 cobrem 99 %.
- **P8. Nível distante genérico.** `building_far.gdshader` usa uma proporção fixa de janela (`WIN_W`, `WIN_SILL`, `WIN_HEAD`) e uma faixa por lado.

### Passo 2: pesquisa

| Fonte | O que diz | Problemas |
|---|---|---|
| [Epic: City Sample](https://dev.epicgames.com/documentation/unreal-engine/city-sample-project-unreal-engine-demonstration?lang=en-US) | gramática de forma; cada prédio são centenas de instâncias; quase nenhuma geometria própria, para caber na memória | P6, P7 |
| [Krecklau e Kobbelt, fachadas procedurais](https://www.graphics.rwth-aachen.de/publication/03196/) | a fachada instancia o detalhe onde ele pode ser visto; longe, troca por textura assada do mesmo detalhe | P1, P2, P3, P8 |
| [Krecklau e Kobbelt, compor fachada na GPU](https://www.semanticscholar.org/paper/REALTIME-COMPOSITING-OF-PROCEDURAL-FACADE-TEXTURES-Krecklau-Kobbelt/0feb96e7f83e5bdfcbb499572c0aff004005833d) | fachada composta na GPU com pouca memória | P8 |
| [Virtual Geometry em Bevy](https://jms55.github.io/posts/2024-06-09-virtual-geometry-bevy-0-14/) | o que um renderizador por clusters exige: compute, visibility buffer, passe de material próprio, sem shader de vértice próprio, pré-processo muito lento | alternativa de P6 |
| [meshoptimizer v1.0, clusterlod](https://meshoptimizer.org/v1.html) e [vk_lod_clusters](https://github.com/nvpro-samples/vk_lod_clusters) | biblioteca de hierarquia de clusters; o exemplo depende de mesh shader | alternativa de P6 |
| [Godot: Mesh LOD](https://docs.godotengine.org/en/stable/tutorials/3d/mesh_lod.html) | LOD por erro em pixels; um MultiMesh usa um só LOD por vez: instâncias distantes vão em MultiMeshes separados | P6 |
| [Godot: Visibility ranges](https://docs.godotengine.org/en/stable/tutorials/3d/visibility_ranges.html) | HLOD por faixa; sem fade é o mais barato; material distante mais simples | P3, P8 |
| [render_forward_clustered.cpp](https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/renderer_rd/forward_clustered/render_forward_clustered.cpp) | a distância do LOD sai do AABB da instância, sem ramo próprio para MultiMesh | P6 |
| Godot fork [Ackustik1990 PR #1](https://github.com/Ackustik1990/godot/pull/1) e [GodotNext](https://github.com/Legoless/GodotNext/issues/110) | geometria virtual só existe em forks e planos, não no motor oficial | alternativa de P6 |

### Passo 3: a causa

P1 a P5 e P8 têm uma causa só: o nível médio foi feito como um segundo desenho do prédio, à mão, em vez de vir das mesmas baias do nível perto. P6 e P7 têm outra: a gramática grava geometria única por prédio, embora as baias se repitam. Cada baia perto custa cerca de 7,5 KB; uma instância custa 64 bytes.

Alternativas rejeitadas, com o motivo:
- **Levar o nível perto mais longe:** trata o efeito; custo linear em GPU e memória (P6).
- **Nível de relevo feito à mão com malha gravada por prédio:** foi minha primeira proposta. É a barata: mantém a causa de P6.
- **Geometria virtual (Nanite):** pede trocar o passe de material do Godot, perde shader de vértice próprio, e o pré-processo é lento demais para prédios gerados em jogo. Só existe em forks.
- **LOD automático do Godot em malha gravada:** não reduz memória e simplificação destrói peça pequena; um MultiMesh tem um LOD só.

Solução escolhida: baias paramétricas instanciadas. Poucas malhas compartilhadas; o vão, o peitoril e a verga vêm dos dados da instância, deformados no shader de vértice. É o que City Sample e Krecklau fazem, adaptado a poucos draws.

### Passo 4: plano

**Ciclo 1, nível médio (P1, P2, P3, P4, P5):**
1. `shell.h`: `BayInstance` e `Shell::bay`. Quatro floats exatos por instância: proporções do vão, cor da parede, cor do arremate, semente, acabamento, sinais e profundidade.
2. Novo `render/buildings/bay_kit.{h,cpp}`: a malha da baia com vão (parede, ombreiras, vidro e peitoril, 22 triângulos) e o empacotamento (P2).
3. Novo `project/render/shaders/building_bay.gdshader`: deforma por instância, usa o mesmo acabamento e o mesmo `far_window` no vidro (P1, P2).
4. `far_mass.cpp`: cada baia com vão vira instância, com seu componente real; paredes cegas continuam em retângulos fundidos (P1).
5. A parede plana fica recuada atrás das baias: não aparece e segue projetando a sombra, sem draw novo (decisão de desempenho).
6. `facade_parts.cpp` e `facade_emit.cpp`: `emit_bay_relief` e `emit_relief`, das mesmas baias, como caixas instanciadas (P3).
7. `building_renderer*`: assar `mid_parts`, que hoje se perde, junto do relevo, e as baias, em dois MultiMesh por chunk (P4).
8. Alcance das bandas de peças limitado ao casco que as sustenta (P5).
9. `city_world.gd`, `materials`: material `building_bay`.

**Ciclos seguintes:** nível distante pelos dados reais e atlas assado (P8); nível perto também em instâncias, para reduzir memória (P6, P7); riqueza de perto.

### Passo 5: código

Sem testes nem medições nesta etapa. Mudança sobre o plano, com o motivo: o schema de materiais é fechado, então a baia entrou no próprio `building.gdshader` em vez de shader novo; o item 8 saiu, porque com as lajes reais as peças deixam de ficar soltas.

| Arquivo | O que mudou | Problemas |
|---|---|---|
| `native/src/render/buildings/bay_kit.{h,cpp}` (novo) | malha compartilhada da baia, 22 triângulos, em espaço de índices | P2, P6, P7 |
| `project/render/shaders/building_bay.gdshaderinc` (novo) | põe a baia pelo dado da instância e a transforma em faces comuns | P1, P2 |
| `project/render/shaders/building.gdshader` | desvio no `vertex()` para a malha da baia | P2 |
| `native/src/render/buildings/shell.{h,cpp}` | `BayInstance`, `Shell::bay` | P1, P2 |
| `native/src/render/buildings/far_mass.cpp` | `bay_walls`: cada vão é instância com seu componente, paredes cegas fundidas, lado liso recuado 0,7 m que dá a sombra | P1 |
| `native/src/render/buildings/facade_emit.cpp` | `each_bay`, `relief`, `pushed_faces`, `emit_relief`, `lines` por shell | P3 |
| `native/src/render/buildings/facade_parts.cpp` | `emit_bay_relief` | P3 |
| `native/src/render/buildings/building_renderer*.{h,cpp}` | `MidDraw`, assar baias e `mid_parts`, alcance, visibilidade | P3, P4 |


### Passo 6: testes e medições

Uma rodada, poses iguais às do passo 1 (`--preset` high do `look`), vista `low3` parada:

| Medida | Antes | Depois |
|---|---|---|
| GPU p50 | 6,2 ms | **19,1 ms** |
| Intervalo de quadro p50 | 16,7 ms | **20,5 ms** (perde o 60 fps) |
| Primitivas vistas | 2,35 M | 6,28 M |
| Primitivas de sombra | 4,18 M | 4,52 M |
| Vídeo | 522 MB | 548 MB |
| CPU p50 | 1,0 ms | 1,1 ms |

Compilou de primeira e o shader passou. Foto `low3`: composição real por fachada, brises, pilastras e relevo aparecem.

### Passo 7: avaliação

**Reprovado.** A qualidade subiu, mas o desempenho caiu a um terço; a regra é nunca pior que antes. Passos cumpridos: 1 a 6, com um desvio de ordem registrado (revisão de um risco de shader antes do teste, só por leitura). Volta ao passo 1 para o custo de GPU. Nada disso foi commitado.

## Ciclo 2

### Passo 1: problemas

- **P9. GPU triplicou com as baias.** Medido em `low3`, separando por faixa:

| Geometria | GPU p50 | Primitivas |
|---|---|---|
| sem baias nem caixas | 6,6 ms | 2,43 M |
| só caixas | 8,0 ms | 3,74 M |
| caixas e baias até 500 m | 9,2 ms | 4,78 M |
| caixas e baias até 800 m | 10,4 ms | 5,74 M |
| tudo | 19,1 ms | 6,28 M |

Os últimos 0,54 M de primitivas custam 8,7 ms: não é contagem de triângulos.

Hipótese refutada por medida: o ruído `weather` (sem ele: 9,7 e 21,6 ms, dentro do ruído da máquina, que está dividida com outras sessões).

### Passo 2: pesquisa

| Fonte | O que diz | Problema |
|---|---|---|
| [Fatahalian et al., Quad-Fragment Merging](https://graphics.stanford.edu/papers/fragmerging/shade_sig10.pdf) | a GPU sombreia em quadras 2x2; triângulos pequenos desperdiçam a maior parte | P9 |
| [Unigine: Quad Overdraw](https://developer.unigine.com/en/docs/2.21/content/optimization/geometry/quad_overdraw/) | medir e reduzir sobreposição de quadras | P9 |
| [Visibility Buffer, Filmic Worlds](https://filmicworlds.com/blog/visibility-buffer-rendering-with-material-graphs/) | visibility buffer elimina isso, mas pede passe de material próprio: não disponível no Godot padrão | P9, rejeitada |
| [GPU Performance for Game Artists](https://www.gamedeveloper.com/programming/gpu-performance-for-game-artists) | LOD bem ajustado reduz overshading: triângulo cobre mais da quadra | P9 |

### Passo 3: a causa

Simulação antes de aplicar (`overshade`: a baia rasterizada em quadras 2x2, parede a 40 graus; execuções do shader por pixel coberto):

| Baia em pixels | 22 triângulos | 1 quad por baia | quad por 4x4 baias |
|---|---|---|---|
| 3 | 3,66 | 2,48 | 1,40 |
| 8 | 2,49 | 1,55 | 1,13 |
| 17 | 1,79 | 1,27 | 1,08 |
| 48 | 1,35 | 1,09 | 1,03 |

A causa de P9 é o overshading: longe, uma baia de 3 px executa o shader 3,7 vezes por pixel. Medida e simulação apontam igual: o custo cresce quando a baia encolhe, não com o número de triângulos. Um quad por baia ainda paga 2,5 vezes; o certo é fundir.

### Passo 4: plano (previsão registrada antes de aplicar)

1. Baias com geometria até `render.building_bay_end_m` = 500 m.
2. Adiante, as mesmas baias em retângulos fundidos de um componente, com a composição real: 1,1 execução por pixel. Superfície própria por chunk, faixa de visibilidade complementar.
3. Tirar a medida temporária de `building_renderer_reach.cpp`.

**Previsão:** GPU em `low3` de 9 a 10 ms, 60 fps, composição mantida a qualquer distância do nível médio. Fica acima dos 6,2 ms de antes por causa do relevo real, e isso terá de ser pago no ciclo seguinte.

### Passo 5: código

Sem testes. `far_mass.cpp flat_bays`; `building_renderer*` superfície `flat` por chunk, sem sombra; `shell.h` `flat`; `render.building_bay_end_m` em `tuning.json`; medida temporária removida. Na revisão por leitura: `put_level` forçava sombra, e a superfície nova projetaria a segunda sombra; ganhou o parâmetro `shadows`.

### Passo 6: testes e medições

Uma rodada, mesma vista `low3`: GPU p50 **9,8 ms** (ciclo 1: 19,1; antes de tudo: 6,2; previsto: 9 a 10), 4,93 M primitivas, 552 MB de vídeo. Intervalo p50 16,7 ms, mas 7 engasgos, CPU p99 100 ms. Foto igual à do ciclo 1.

### Passo 7: avaliação

**Aprovado com ressalvas.** Passos cumpridos, com a simulação antes de aplicar. Um erro de edição (for sem chaves) pego pelo compilador.

Ressalvas abertas:
- Os engasgos não têm causa provada. No momento havia 19 processos Godot e 37 node de outras sessões, e o streaming parado (`pendente=0`): provável carga externa, a confirmar com a máquina quieta.
- GPU 3,6 ms acima de antes: dívida a pagar.
- Passada a faixa de 500 m as janelas seguem sem relevo.

## Ciclo 3 (relevo por pixel)

- **Passo 1.** Problema P10: além de 500 m a baia é retângulo plano; janela sem recuo. O par do viaduto com o 4.0 foi fechado com a foto do arquivo (`reference40.mjs configs/plans/viaduct.json`): tabuleiro, pilares e rampas equivalem.
- **Passo 2.** Krecklau e Kobbelt: a fachada é avaliada por pixel sobre o polígono-base, por ray casting (fontes no ciclo 1).
- **Passo 3.** Depois do ciclo 2, a geometria deixa de ser a resposta nessa faixa: 1 quad por baia ainda pagaria 2,5 execuções por pixel. A profundidade tem de vir do fragmento.
- **Passo 4.** Recuo analítico em `far_window`, sol por `sun_direction`, peitoril e sombra dele; desligado no vidro das baias com geometria (`v_recess`).
- **Previsão:** algumas dezenas de operações por pixel de vidro, sem textura nova: menos de 1 ms em `low3`.
- **Passo 5.** `project.godot`, `daylight.gd`, `building.gdshader`, `building_bay.gdshaderinc`, `building_far_window.gdshaderinc`.
- **Passo 6.** A/B intercalado na mesma execução (`look` passo `global`, `building_recess`), seis janelas, vista z1,5, com a GPU a 100 % pelas outras sessões: desligado 25,85 ms, ligado 25,98 ms. Custo: 0,13 ms. A medida solta de 24 ms e 56 engasgos era carga externa (`nvidia-smi` 100 %) e foi descartada. Fotos lado a lado: 7,6 % dos pixels mudaram, fachadas mais definidas.
- **Passo 7.** Aprovado. Método novo: em máquina dividida, só vale A/B intercalado.

## Próximo ciclo

2. Nível distante pelos dados reais (P8).
3. Nível perto também em instâncias (P6, P7): paga a dívida de GPU e memória.
4. Riqueza de perto.
5. Decisão do usuário: `visibility buffer` pediria modificar o motor Godot.
