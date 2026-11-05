# Plano do Construir: corpo editável, comandos, desfazer, save e interface

Escrito antes do código, por ordem da coordenadora. Vale para `convert/build`.

## O que o jogador faz no 4.0
Oito abas (`src/ui/builder/catalog.ts`): Modelos, Desenhar, Massa, Fachada, Peças, Telhado, Interior, Cores. Clica num prédio, escolhe bloco, face, vão ou peça, e edita: andares, alas, recuos, vãos de janela e porta por escopo (vão, zona, linha, coluna, andar, lado, volume), relevo, telhado e inclinação, peças soltas, equipamentos de cobertura, núcleos e móveis com o prédio cortado, tintas por escopo. Tudo desfaz, tudo salva.

## O que já existe no jogo
- `b40::Body` (`native/src/sim/buildings/b40/types.h`) já descreve volumes com contorno, tinta por volume e por andar, vãos sobrescritos, geometria de face, relevos, telhado com inclinação, elementos de lote e núcleos. **É o modelo; não se cria outro.**
- O corpo hoje é só derivado da semente: `record_body` gera, `BuildingBodies` guarda em cache. Um prédio não tem corpo próprio.
- `EditHistory`, `j_edit_building`, `finish_edit` e `ChangeLog` já fazem desfazer e redesenho por registro.

## O que falta no modelo
Comparando `Volume` e `Storey` do 4.0 com `b40::Volume`: equipamentos de cobertura (`roofDetails`), modo de bloco (`mode`), bloco aberto (`open`, `terrace`, `ramp`), `lift`, vão por lado inteiro, uso por andar, função do prédio editável, móveis e luzes colocados à mão. Entram como campos com valor padrão: **corpo gerado não muda, bit a bit**, e isso é testado.

## Decisões
Ajustadas depois de ler o código, antes de escrever a fase 1.
1. **Corpo editado imutável, nomeado por número.** `b40::BodyTable` guarda cada versão inteira e nunca a altera; o registro `Building` ganha `uint32_t body` (0 = feito da semente). Reordenando os campos, o número entra no alinhamento e o registro fica nos mesmos 48 bytes: prédio sem edição custa zero.
   - Por quê: o diário já troca registros inteiros (`EDIT_BLD`), então desfazer e refazer saem de graça, sem operação nova. Um job segura um `shared_ptr` e lê sem a trava do mapa.
   - Alternativa recusada: gravar diferenças por operação. Exige inverso de cada uma das dezenas de operações, e cada inverso é um lugar novo para errar.
   - Custo aceito: versões antigas ficam na tabela até o mapa fechar. Só o save compacta, gravando as versões em uso.
2. **Primeira edição materializa.** `record_body` gera, a cópia vira o corpo do prédio. `record_body` passa a consultar o editado primeiro, e é o único caminho de render e simulação (`lot_access`, árvores do lote).
3. **Cache do render.** A chave de `BuildingBodies` ganha a revisão do corpo, para um corpo editado nunca colidir com o gerado.
4. **Operações puras.** `op_*(Body&, ...)` sem `CityState`: testáveis sozinhas. Um executor único copia, aplica, valida, registra no diário e chama `finish_edit`.
5. **Validação antes de guardar.** Corpo inválido (andares fora do limite, volume sem apoio, peça fora do telhado, invasão de via ou vizinho) é recusado com a chave do texto, como `road_plan`.
6. **Save.** Seção opcional marcada por etiqueta (`SAVE_BODIES`) antes do `SAVE_END`, sem mudar `SAVE_VERSION`: save antigo abre sem corpos editados, e a mudança não briga com outra sessão que suba a versão.
6b. **Áreas de planta sem biblioteca.** O 4.0 usa `polygon-clipping` para anéis não convexos. Área e apoio saem exatos com orelhas (Meisters 1975) e corte convexo (Sutherland e Hodgman 1974), em `b40/plan_area`.
7. **Interface em arquivos próprios.** `project/ui/builder/*` e `project/game/tools/building_tool.gd`; abas e ferramentas em `data/ui/builder.json`. O porte `interface` reescreveu `hud_drawer.gd`: toco nele o mínimo, um ponto de entrada, para a integração não brigar.

## Fases, cada uma com commit próprio compilando
| Fase | Entrega | Donos novos |
|---|---|---|
| 1 | Corpo editável: guarda, `record_body`, cache, diário, save, teste de ida e volta | `sim/buildings/body_store`, `sim/persistence` |
| 2 | Massa: andares, redimensionar, ala, recuo, remover, dividir, alturas, mover, copiar, espelhar, girar | `sim/commands/body_mass`, `body_validate` |
| 3 | Fachada: vão por escopo, zona, padrão, geometria de face, relevo | `sim/commands/body_facade` |
| 4 | Telhado e Cores: tipo, inclinação, cumeeira, equipamentos e seu desenho; tintas por escopo, estilos | `body_roof`, `body_paint`, `render/buildings/roof_plant` |
| 5 | Peças: elementos soltos, repetir, fundir, trechos | `body_parts` |
| 6 | Seleção: raio contra corpo, volume, face, vão, elemento; alças | `sim/buildings/body_pick`, API |
| 7 | Interface: ferramenta, abas, inspetor, prévia, atalhos | `project/ui/builder/*`, `building_tool.gd` |
| 8 | Modelos: catálogo de `cityBuildings` como dados com schema, modelos do jogador | `data/catalog/blueprints.json` |
| 9 | Desenhar: formas e plantas, ações novo, junto, em cima, recorte | `body_plan` |
| 10 | Aba Interior: núcleos e móveis à mão; a vista cortada vem da Porte 8, pelo `RenderWorld` | `body_interior` |
| 11 | Modelagem avançada: extrudar, bisel, booleana, vértices | ver pendências |
| 12 | Ferramenta de lotes (`lotTool`): dividir, juntar, frente, camada | `sim/commands/lot_edit` |

Fase 6 vem depois dos comandos porque eles se testam por id, sem clique. Interface sem seleção não funciona, por isso 6 antes de 7.

**Ordem ajustada depois das fases 1 a 4:** 6 e 7 passam à frente da 5 e dos equipamentos de cobertura. Sem seleção e interface, nada do que existe por comando chega ao jogador; Peças e equipamentos entram depois, já com a tela para usar.

## Pendências de pesquisa, antes da fase em que entram
- **Fase 6:** ler `pick.ts` e `handles.ts` inteiros.
- **Fases 2 a 5:** ler cada operação de `editor/buildings.ts` e `buildingTool.ts` antes de escrever a dela. Li as assinaturas e parte do corpo, não tudo.
- **Fase 11:** as booleanas têm polígono como resultado, e `plan_area` só dá áreas. O 4.0 usa `polygon-clipping` nelas. Se precisar de biblioteca (Clipper2 é a candidata; licença e documentação não conferidas), baixar dependência nova pede autorização antes. Não decidido.
- **Fase 7:** conferir nas docs do Godot 4.7 a captura de entrada sobre `Control` e 3D, sem depender de memória.

## Prova
- Teste novo `project/tests/core/building_edit_test.gd`, pelo store: cada comando altera o que `render_cost` e a sonda mostram; desfazer volta ao corpo gerado; refazer repete; salvar e abrir devolve o mesmo corpo; corpo gerado igual antes e depois da fase 1.
- Foto só na etapa de teste, depois de gates e build, com lista escrita antes; no máximo 2 rodadas por tema; limite de 600 s; sem varrer parâmetro. As 20 fotos por tipo de prédio saem numa rodada planejada, 4.0 e jogo na mesma câmera.

## Depois do Construir, nesta ordem
Regra de leitura: antes de portar um tema, ler o `source/` dele, não só a lista de declarações.
1. Verificação símbolo a símbolo com `evidence/cobertura/candidatos.py`; decide-se lendo, e só entra em `provas.py` o que foi lido.
2. Rodada de fotos por tipo de prédio, com civis, praça, parque e marcos colocados no 4.0, em pares no mesmo enquadramento, posição, alvo, zoom e hora.
3. Defeito a corrigir, não item aberto: janelas distantes que se fundem em blocos maiores que no 4.0.
4. Medir o vidro de dia contra o alvo; custo do `pane()` por contagem de laços e amostras por fragmento; retirar ou medir o "sem cintilar".
5. Fonte ou medição, com a seção citada, para as constantes sem fonte: `room_daylight`, `outside_all_night` e iluminância de cômodos (NBR ISO/CIE 8995-1), `wheels_per_s` e `feet_per_s` (velocidade de caminhada em estudo publicado), tamanhos de móveis e proporções de planta (manual de projeto), e os limites do grupo `build`, incluindo degrau e rampa, que citei da NBR 9050 de memória.
6. Com os branches juntos, remedir parede e janela à noite com a Porte 3 (`moon_energy`, `hemi_night_k` e cores mudaram depois da minha medição).
7. `marcos`.

## Fora do meu dono, já avisado ou a avisar no relatório
- Distritos (`planCity.zoneAt`), causa das torres por todo o mapa: `roads`, marcado `a portar` em `ports/roads/cobertura.md`.
- Luz noturna, paredes e terreno: `landscape`; medição entregue por mensagem.
- Faixas amarelas das ruas à noite: sessão da rua, segundo a coordenadora.

## Risco declarado
São cerca de 12 fases e a maior parte do trabalho que falta. Se o contexto ou o tempo acabarem antes, a fase em curso fica sem commit parcial quebrado e `relatorio.md` diz exatamente onde parou.

## Fase 8, Modelos: plano escrito antes do código
Lido no 4.0: `cityBuildings.ts` (42 modelos), `blueprints.ts` (11 genéricos, `placeBody`, `bodyOf`), `catalog.ts`, prateleira `models` de `ui/v2/shell.ts`, `buildingSnap.ts`, `validate.ts`, `weldInto`, `placeHere`, `buildingsWiring.ts` (`library`, fotos por estúdio).

O que o jogador faz: abre Modelos, busca por nome, filtra por categoria (casas, públicos, comércio, trabalho, lazer, genéricos, lotes, meus), vê cada modelo em foto, escolhe, leva o fantasma do prédio sob o ponteiro. O fantasma cola na rua, gira com R, encosta no vizinho, e o clique coloca. Sobre outro prédio, os dois se fundem. Um prédio selecionado pode ser salvo como modelo próprio, com nome, e apagado da lista.

Decisões:
1. **Dados.** `project/data/render/models.json`, em `styles.<id>.models`: um modelo por linha, com blocos, lotes abertos, peças, fileiras, cercas em volta e núcleos, como o 4.0 os compõe. É catálogo de autoria, como a arte: as medidas de cada modelo não têm norma nem medição, e o relatório diz isso. Por ser vocabulário de região, fica sob o estilo.
2. **Do modelo ao corpo.** `sim/commands/model_body.cpp` monta um `b40::Body` no referencial do lote: x do meio da frente, y da rua para dentro, W e D do que o modelo ocupa. Sem semente: mesmo modelo, mesmos bytes. Padrões de fachada passam por `op_set_pattern`, sem segundo dono.
3. **Colocar.** `place_model` guarda o corpo em `BodyTable` e cria o registro com `body` != 0, num passo de desfazer. Recusa como o 4.0: corpo inválido, fora do mundo, sobre via, encosta acima do limite.
4. **Encaixe.** `sim/commands/place_snap.cpp`: rua, depois vizinho, depois grade; canto e faces rentes. Roda a cada movimento do ponteiro, então só consulta por vizinhança, nunca percorre o mapa.
5. **Fundir.** Prédio tocado entra no novo como volumes e peças, com paredes e telhado que tinha; nada é cortado. Vale para prédio crescido e editado.
6. **Fantasma e foto, um só caminho.** O render monta a malha do corpo uma vez por escolha, com os mesmos emissores do jogo. O fantasma é essa malha movida por transformação. As fotos saem de um estúdio em `SubViewport`, poucas por quadro, só com a gaveta aberta, como o estúdio do 4.0.
7. **Modelos do jogador.** Bytes de `body_codec` em `user://models/`, um arquivo por modelo, dono `project/game/store/model_library.gd`. Valem para todas as cidades, como a biblioteca do 4.0.
8. **Lotes (`lot:<nome>`).** A categoria existe na interface, mas os 38 projetos entram com o porte `marcos`.

Provas: teste pelo store (todo modelo vira corpo válido; bytes iguais em duas montagens; colocar, desfazer, refazer; recusas; save; encaixe na rua e no vizinho; fusão; biblioteca). Fotos só na rodada planejada, em par com o 4.0.

## Massa da cidade e dono dos andares (ordem visual, seção 11)
Medido pelo corpo, mapa 0 contra cidade pequena do 4.0, semente 7: no miolo o jogo tinha 7,2 andares de média e 24% de torres; o 4.0, 17,9 e 88%.
Causas: raio 1100 contra 640; sem `planCity` (centros, calor por lote); a zona sorteada por trecho; o registro sorteava andares numa tabela antiga e o corpo sorteava de novo.
Decisões:
1. `sim/citygen/city_plan.*` porta os centros e o `zoneAt` com o mesmo gerador; o raio vem de `growth.json`.
2. A zona fica nas margens da rua, como já é o modelo do jogo, medida a meio lote de cada lado; o 4.0 mede por lote.
3. O corpo é o único dono dos andares: `record_for` guarda o que ele responde, `floors` vira só teto para o bloco médio, e o teto é idempotente. A tabela `floors` de `growth.json` sai.
Prova: `city_mass_test` compara sonda e corpo em todos os prédios e confere a forma do miolo.

## Janelas: causas medidas e plano (rodada par2, com a luz da Porte 3 num ramo de medição)

Com a luz da Porte 3 os telhados e o conjunto já batem (p1: 51/79/91/104/184 contra 45/75/88/97/159). O que sobra é dos prédios:

1. Perto: o jogo abre um buraco real na parede. Os lados do vão recebem luz de céu inteira, e a sombra do mapa e o SSAO não chegam a 12-20 cm. O 4.0 pinta o vão no próprio vidro, por raio contra caixa, com o sol exato e o céu pelo fator de forma de Lambert. Por isso as molduras e os lados aqui ficam claros, e lá escuros.
2. Longe: o `far_window` escurece 25 % de toda janela. No 4.0 a parte depende do componente (`facadeKind`): `sash`, `wide`, `balcony`, `shopfront` nunca; `window` 5 em 20; `loggia`, `shuttered`, `gourmet` 1 em 4; `brise`, `french`, `bayWindow`, `ribbon` 1 em 5. Aqui ainda há um caso próprio de pele de vidro que o 4.0 não tem. Daí os quadrados escuros soltos nas torres. O `step()` não é a causa: o 4.0 faz igual, com o mesmo filtro.
3. Acabamento de vidro: `finish_mean` divide o mapa por 0,081 (vidro) e 0,226 (ardósia). O 4.0 multiplica o mapa como ele é. As torres de vidro saem até 12 vezes mais claras (limite de 2).
4. `dark_pane` difere do 4.0 em `sash` (nunca escuro lá), `wide`, `balcony` e `shopfront`; a persiana não depende da largura.

Solução:
- `facade_bay.cpp`: vão limpo (regra `flush` do 4.0) sem lados nem peitoril de malha; vidro na face da parede, com a fundura em `PaneLook.recess`. 8 triângulos a menos por janela.
- `shell.cpp`: fundura nos 5 bits altos de COLOR.a, em cm.
- Shader próprio `building_glass.gdshader` para a superfície dos vidros: vão por raio, moldura, sala, `AO` = céu visto pela abertura, e `light()` com a saída do raio de cada luz pela abertura. `light()` em vez de `AO_LIGHT_AFFECT`, porque este só escala a luz direta até o próprio AO, e o lado sem sol precisa de luz direta zero com céu maior que zero (fonte: `scene_forward_clustered.glsl`, `ao = mix(1.0, ao, ao_light_affect)`; doc de `spatial_shader`, `LIGHT_COLOR` com PI).
- `building.gdshader` perde sala, móveis e `pane()`: as paredes ficam mais leves. `far_window` e `glass_reflection` em arquivos próprios.
- `far_window`: classe de escurecimento por componente em `flags`; sem caso de pele de vidro; persiana só em `window`.
- `finish_mean` sai.

Prova: testes, sonda de triângulos, par p3 e p2 em recorte 3x, medidas de vidro e moldura.
