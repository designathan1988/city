# Pesquisa do porte: interface

Passo A da correção da coordenadora. Godot em uso: `4.7.2.stable.official.ed1daf0bf` (`godot_console --version`).

Como li cada fonte:
- **Fonte e XML:** baixados da tag `4.7.2-stable` e lidos direto. O texto citado é exato.
- **Páginas `docs.godotengine.org`:** lidas por um leitor que resume. As frases entre aspas vêm desse resumo; onde a precisão importa, conferi no XML da tag.

## 1. A máscara da câmera tira a camada do passe de sombra?
**Sim, confirmado no fonte. Antes era memória; agora é leitura.**

Fonte: https://raw.githubusercontent.com/godotengine/godot/4.7.2-stable/servers/rendering/renderer_scene_cull.cpp
- Linha 2922: `#define LAYER_CHECK (cull_data.visible_layers & idata.layer_mask)`.
- Linha 3248, luz direcional (o sol do jogo), por cascata: a instância só entra em `cascade_geometry_instances` com `(LAYER_CHECK & cull_data.cull->shadows[j].caster_mask)`.
- Linhas 2427, 2510, 2578 e 2644, luzes omni, spot e de área: `p_visible_layers & instance->layer_mask & light_get_shadow_caster_mask(...)`. **Não valem para o sol:** o `case LIGHT_DIRECTIONAL` da linha 2376 é vazio, e a luz direcional passa pela 3248.
- Linha 3429: `cull_data.visible_layers = p_visible_layers`. Linha 2783: `_render_scene` recebe `camera->visible_layers`.

**Conclusão:** desligar uma camada na `cull_mask` da câmera tira os objetos do desenho e também das sombras. Vale para `render_switches.gd` e para `detail_props`.

**Lacuna na documentação:** a página de `Camera3D.cull_mask` e a de luzes e sombras não tratam disso. Só o fonte prova.

**Alternativa por luz (apontada pela coordenadora, conferida por mim):** `Light3D.shadow_caster_mask`, exposta como `RenderingServer.light_set_shadow_caster_mask`. Com ela uma camada pode sumir da sombra e ficar no desenho. O inverso, sumir do desenho e manter a sombra, a máscara da câmera não faz: as linhas 2427 e 3248 exigem a camada visível para a câmera **e** a máscara da luz.

**Decisão pendente:** o interruptor de camada do painel F7 tira a camada do desenho e da sombra, como o `switchedOff` do 4.0, que esconde o objeto inteiro para o quadro. A escolha fica registrada com foto de antes e depois no passo H.

## 2. API de vídeo
| Assunto | Fonte | Conclusão |
|---|---|---|
| `Viewport.scaling_3d_scale` | https://docs.godotengine.org/en/stable/classes/class_viewport.html | Abaixo de 1,0 acelera e perde qualidade. |
| Escala 3D e FSR | https://docs.godotengine.org/en/stable/tutorials/3d/resolution_scaling.html | Bilinear existe em todos os renderizadores e é o mais rápido. FSR 1.0 e 2.2 são só Forward+. A escala é por eixo: 0,5 corta os pixels em cerca de 4x. A página pede expor escala e opções gráficas ao jogador, e avisa que a escala não substitui baixar as opções. Acima de 1,0 não é recomendado. |
| SMAA | https://docs.godotengine.org/en/stable/tutorials/3d/3d_antialiasing.html | Custo "low" na tabela, um pouco mais lento que FXAA. Forward+ e Mobile, **não Compatibility**. |
| MSAA | mesma página | Custo "medium", disponível em todos os renderizadores. |
| SSAO | https://raw.githubusercontent.com/godotengine/godot/4.7.2-stable/doc/classes/Environment.xml e `ProjectSettings.xml` | Só Forward+ e Compatibility, não Mobile. `half_size` é "significantly faster". |
| Padrões do SSAO | `ProjectSettings.xml`, linhas 2938 a 2955 | `adaptive_target` 0,5, `blur_passes` 2, `fadeout_from` 50, `fadeout_to` 300, `half_size` true, `quality` 2. Os quatro valores de `settings.json ssao` coincidem com o padrão do motor: **fonte confirmada**. |
| `environment_set_ssao_quality` | `RenderingServer.xml`, linha 1480 | Argumentos: `quality, half_size, adaptive_target, blur_passes, fadeout_from, fadeout_to`. |
| Anisotropia | `ProjectSettings.xml`, linha 3464; `Viewport.anisotropic_filtering_level` | Potência de dois de amostras, 0 desliga. O padrão do projeto é 2 (4x). |
| Sombra direcional | `ProjectSettings.xml`, linha 3146; https://docs.godotengine.org/en/stable/tutorials/3d/lights_and_shadows.html | O padrão de `directional_shadow/size` é 4096. O PSSM padrão tem 4 divisões. O atlas é dividido entre as luzes. Distância máxima menor melhora sombra e desempenho. |
| Tempo de GPU | `RenderingServer.xml`, linha 4105 | `viewport_get_measured_render_time_gpu` dá o tempo de GPU do último quadro em ms. Reflete a ocupação mesmo com taxa de quadros limitada. Só devolve valor acima de 0 depois de `viewport_set_measure_render_time`. Para o total, somam-se todos os viewports desenhados. |
| Logger | https://docs.godotengine.org/en/stable/classes/class_logger.html | Registro por `OS.add_logger()`. `_log_error` e `_log_message` são chamados **fora do thread principal, possivelmente ao mesmo tempo**: pede `Mutex`. Chamar `push_error` ou `print` dentro deles não é suportado, por recursão. |

| `Performance.TIME_PROCESS` | https://raw.githubusercontent.com/godotengine/godot/4.7.2-stable/main/main.cpp, linhas 5100 e 5136 a 5139 | `process_max` guarda o **pior** quadro do último segundo, e o monitor recebe esse valor. Explica os 379 ms com 20 quadros por segundo na foto do painel de efeitos: não é média. O painel passou a somar `render_cost.process`, `viewport_get_measured_render_time_cpu` e `get_frame_setup_time_cpu`. |
| Atlas da sombra direcional | https://raw.githubusercontent.com/godotengine/godot/4.7.2-stable/servers/rendering/renderer_rd/storage_rd/light_storage.cpp, linha 2815 em diante | Com 4 divisões cada cascata mede metade do atlas por lado: `shadow_px * 2` em `QualityTier.apply` e o `_doc` do `settings.json` se sustentam. |
| `Control.set_anchors_preset` | `Control.xml` da tag: mantém os deslocamentos por padrão | Causa do defeito 1 da coordenadora: num painel já na árvore, a âncora nova com os deslocamentos antigos deixa tamanho zero. A cortina nunca cobriu a tela. A chamada certa é `set_anchors_and_offsets_preset`. |
| Terrain3D `mesh_size` | https://terrain3d.readthedocs.io/en/stable/api/class_terrain3d.html | Página aberta, mas meu filtro de texto só achou o par `ocean_mesh_size` ("Lod0 has 4*ocean_mesh_size + 2 quads"). Para `mesh_size` o teste lê o valor de volta depois de mudar. |

### Problemas que a pesquisa abriu no meu código
Viram itens do `plano.md`:
1. **`health_log.gd` e o Logger:** conferido. `_log_error` só chama `note_from_thread`, que usa `Mutex`; nada imprime nos virtuais.
2. **SMAA no Compatibility:** corrigido. `QualityTier.has_smaa`: a opção some do painel e o nível usa MSAA 2x.
3. **Tempo de GPU:** `viewport_set_measure_render_time` é ligado em `Settings.attach`. O jogo desenha um só viewport 3D; o total pedido pela documentação é esse.
4. **`_doc` do `settings.json`:** conferido no fonte, se sustenta (tabela acima).

## 2b. Oclusão por cena (pedido da coordenadora)
Fontes: https://raw.githubusercontent.com/godotengine/godot-docs/master/tutorials/3d/occlusion_culling.rst, `OccluderInstance3D.xml`, `ProjectSettings.xml` linhas 3232 a 3244 e `RenderingServer.xml` linhas 2958 a 2969, todos lidos.
- **O que o Godot oferece:** um só caminho, o buffer de baixa resolução rasterizado na CPU pelo Embree. Não há outro para escolher; a decisão é o que dar a ele.
- **Só objeto inteiramente escondido sai,** pela caixa dele. Objetos menores saem mais: os subchunks do nível perto servem melhor que o superchunk.
- **O sistema é quase estático:** mover ou esconder um oclusor recomputa em segundo plano por vários quadros. Oclusor por prédio em 450 mil prédios está fora. Um por chunk, só no nível perto, como o `OCCLUDER_REACH` e o `drawn` do 4.0.
- **Procedural:** a documentação aponta `ArrayOccluder3D`; pelo servidor é `occluder_create`, `occluder_set_mesh` e `instance_create2`.
- **Forward+ já tem pré-passe de profundidade:** o ganho vem de menos vértices e chamadas, não de sombreamento. A documentação avisa que, com poucas oportunidades, pode não pagar a CPU. **A decisão de ligar sai da medida** (objetos e chamadas com e sem, `tests/perf/tier_gpu`).
- **O 4.0** cita Downs 2001 e Hasselgren 2016: cidade 2,5D esconde de 80 a 90 % do que o frustum mantém, e só paredes que certamente existem entram. Vale aqui: só volumes retangulares no chão.

## 3. Fonte de cada constante
| Constante | Valor | Fonte | Estado |
|---|---|---|---|
| `thunder.sound_ms` | 343 m/s | c = √(γRT/M). R = 8,314 462 618 J/(mol·K), exato: https://physics.nist.gov/cgi-bin/cuu/Value?r. Com γ = 1,4, M = 28,96442 g/mol e T = 293,15 K dá **343,24 m/s**. | **Parcial.** Só R foi aberto na fonte primária. γ e M não: o relatório da NASA NTRS 19770009539 não abriu e a página do NPL falhou no certificado. A fórmula e o resultado batem com secundárias: Cramer 1993, JASA 93(5), doi:10.1121/1.405827, citado por https://labs.sonicfield.org/library/the-variation-of-the-specific-heat-ratio-and-the-speed-of-sound-in-air-with-temp, e o valor de 340,294 m/s a 288,15 K em https://www.pdas.com/programs/atmos76.f90 |
| `health.long_frame_ms` | 50 ms | https://www.w3.org/TR/longtasks-1/ define tarefa longa acima de 50 ms, ligada à meta RAIL de resposta em 100 ms. | **Ressalva.** A fonte existe, mas é de navegador, e a página consta como rascunho descontinuado. Numa tela de 60 Hz, 50 ms são 3 quadros perdidos. Fica como limiar de registro, não como orçamento. |
| `ssao.*` | 0,5 / 2 / 50 / 300 | Padrões do Godot, acima. | Confirmado |
| `governor.window` e demais | 90 etc. | Só `ARQUITETURA.md` item 8 do 4.0, que **não é fonte**. | **Sem fonte: medir** (seção 4) |
| `gpu_rules` | regras por nome | Só `core/gpuTier.ts` do 4.0. | **Sem fonte: medir** nesta RTX 3060; as demais placas ficam sem prova |
| `tiers.*` | vários | Só 4.0, salvo o que a documentação orienta. | **Sem fonte: medir** |
| `dynamic_resolution.*` | vários | A documentação orienta o método, não os números. | **Sem fonte: medir** |
| `start.reveal_limit_s` | 90 s | Só 4.0. | **Sem fonte: medir** o tempo real de abertura do mapa 0 |
| `thunder.*`, salvo o som | vários | Só 4.0. | **Sem fonte**: escolha de som, a declarar como tal |

## 4. O que não tem fonte e sai de medição
A regra manda medir quando não há fonte. A máquina segue carregada por outras sessões, então:
- Vale contagem, e `viewport_get_measured_render_time_gpu` em amostras, que mede a GPU e não o relógio do processo. Ainda assim, GPU dividida com outras sessões **infla** o tempo. Uma medida folgada aqui prova; uma ruim não reprova.
- O comando para a máquina quieta vai no relatório.

Resultado no passo F do `plano.md`.

## 5. O que esta pesquisa não cobriu
- O corpo do relatório da NASA e a página do NPL não abriram.
- `Godot Logger` e `lights_and_shadows` foram lidos pelo resumo, não pelo XML.
- Terrain3D não foi pesquisado: a interface não toca nele até aqui. Se o plano tocar, a pesquisa volta.
