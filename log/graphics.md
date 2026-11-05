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

(a preencher ao terminar)

### Passo 6: testes e medições

(a preencher, uma vez)

### Passo 7: avaliação

(a preencher)
