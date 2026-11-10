# Plano — demolition (Porte 7)

Escrito antes da primeira linha de código. Fontes e constantes: `research.md`. Rastreamento lido: `converter/out/features/demolition`
(721 declarações) e `demolition-p7` (1.044: o primeiro mais `toolChoices`, `riggedCitizens`, `carBody`; o que sobra é de pessoas/veículos
e fica com os donos deles). Leitura do 4.0: `actionsWiring.ts`, `bulldozer.ts`, `blast.ts`, `destruction.ts`, `fracture.ts`, `gore.ts`,
`casualties.ts` (sim e render), `playEffects.ts`, `exhaust.ts`, `ragdoll.ts` (estrutura), `walk.ts` (impact/shot/getUp).

## Causa do branch antigo e requisito central
O branch antigo desenhava a explosão sem mudar a cidade. Aqui a explosão é um **comando da simulação** que muda `CityState`:
prédios (danificados/derrubados), vias (cratera corta), mobiliário, árvores, veículos e pedestres (mortos/feridos), terreno (cratera),
destroços que bloqueiam, fogo; tudo **desfazível** (diário) e **salvo**. O render só lê esse estado e o anima.

## Camadas e donos (core → sim → render → api)
| Dono | Arquivos novos | Papel |
|---|---|---|
| dados | `data/catalog/blast.json` + `tools/gates/schemas/blast.schema.json`; `data/render/effects.json` + schema | limiares, curvas K-B (com fonte), carga por intensidade, crateras, tiro; aparência dos efeitos |
| `sim/demolition` | `blast_model.{h,cpp}` | **dono único** da física do sopro: `overpressure(Z)`, `impulse(Z)`, probabilidades de morte/tímpano, raio da cratera. Puro C++ |
| `sim/demolition` | `damage_state.h` | estado: crateras, ruínas (destroços), prédios danificados (acertos), fogo, fila de eventos para o render |
| `sim/commands` | `strike_commands.cpp` | `strike_at`, `shoot_at`, `demolish_box`: validam, mudam o estado pelo diário, anunciam no `ChangeLog` |
| `sim/commands` | `journal_damage.cpp` | operações do diário `DMG` (cratera/ruína/acerto/fogo) com inverso |
| `sim/persistence` | `save_demolition.cpp` | bloco próprio do save (etiqueta+tamanho; save sem ele abre com padrões) |
| `sim/traffic` | adições: `kill_pedestrian`, `shock` (susto), `remove_vehicle`, trechos bloqueados por destroço/cratera | o tráfego lê o estado de dano |
| `render/effects` | `effect_world.{h,cpp}` (dono), `puffs.cpp`, `debris.cpp`, `marks.cpp`, `ruins.cpp`, `ragdoll.cpp` | lê eventos do `ChangeLog` e o estado; anima |
| `api` | `city_core_strike.cpp` | `strike`, `shoot`, `demolish_box`, sondas; bindings |
| Godot | `game/tools/strike_tool.gd`, `shoot_tool.gd`, `tests/…/capture_demolition.gd` | ferramentas e captura (o desenho da aba Demolir/Pistola/Bomba é da Porte 4) |

Camada `render` só lê; `sim` não conhece Godot; nada de arquivo-deus (limites do `rules.json`).

## Física (dono: `blast_model`)
1. Intensidade do jogador `S` (1..50, como o 4.0) → carga TNT `W = 0,008·S³` kg (S=5 → 1 kg; S=50 → 1 t): escolha de design em
   `blast.json` (`charge_kg_per_s3`), o resto é física.
2. Em cada ponto: `Z = R/W^(1/3)`, `p(Z)` e `i(Z)` pelas curvas de Swisdak/K-B de superfície; fora da faixa, o valor da borda.
3. **Prédios:** a sobrepressão no ponto da pegada mais próximo. `p ≥ 34,5 kPa` → colapso (prédio sai do store, vira ruína e
   destroços); `20,7 ≤ p < 34,5` → dano estrutural (fração de fragmentos soltos); `6,9 ≤ p < 20,7` → dano leve (vidros, fuligem,
   desgaste: `decay`); o prédio atingido diretamente (clique nele) recebe o acerto no ponto clicado com a intensidade cheia.
4. **Pessoas:** probabilidade de morte pelo modelo de pulmão (DDESB TP 20 §2.7.3) com `p` e `i`; sorteio determinístico
   (`personHash`) → morto (dilacerado e carbonizado se `p` muito alto), ferido (derrubado, pode perder membro) ou só assustado;
   quem está dentro de um prédio que desaba morre (ocupantes saem lançados no 4.0; aqui: lançados como cadáveres).
5. **Cratera:** raio `k_solo·W^(1/3)` (terra, asfalto), profundidade pelo razão diâmetro/profundidade; em terra o terreno é rebaixado
   (carimbo no `Heightfield` desfazível); em via, o trecho dentro da cratera fica **cortado**: segmentos cujo eixo cruza o círculo
   ganham `damage` (o tráfego e pedestres tratam como bloqueado: remove o trecho do grafo de faixas até o jogador reparar — reparar
   é demolir o trecho cortado e reconstruir; o `demolish` já devolve o reembolso). Raio mínimo do corte vem de dados.
6. **Mobiliário:** postes/semáforos/hidrantes/árvores dentro do alcance (`p` ≥ limiar de derrubada) entram numa lista de "removidos"
   por segmento/lugar (o mobiliário é derivado da via: a lista é a única exceção e é salva); hidrante quebrado vira gêiser.
7. **Veículos:** os dentro de `1,1·R` saem do tráfego (`remove_vehicle`), carcaça queimando fica como destroço; o resto da fila
   desvia/espera pela via cortada.
8. **Destroços:** o prédio derrubado deixa `Ruin{centro, pegada, semente, altura}` que bloqueia a calçada/pista sob a pegada e
   suspende o crescimento no lote (`hold_growth` 90 s do 4.0); demolir de novo limpa.
9. **Fogo:** o prédio atingido pega fogo (até 3 por vez), chamas por tempo simulado (`Simulation` tick), apaga sozinho.

## Render (`render/effects`, lê eventos + estado)
- **Puffs** (fumaça, poeira, chama, faísca, sangue, água, papel): um MultiMesh de quadros-bilhete com `custom` = (nasc, tipo, semente,
  vida) e tamanho; o movimento e a cor saem do **vertex/fragment shader** pela idade (como o 4.0), reserva circular de 4.000; só a
  fatia escrita sobe. Sombra desligada. `project/render/shaders/puffs.gdshader`.
- **Destroços** (caixas, cilindros, lajes, carcaças): corpo rígido simples em C++ (gravidade, canto×chão, impulso) no render, até
  1.400 peças, MultiMesh por tipo; peças em repouso dormem e não reescrevem. Carcaça do veículo = malha própria do tipo.
- **Marcas** (cratera, fuligem, sangue): quadrados texturizados elevados em MultiMesh (mesmo método das marcas viárias), texturas
  geradas em CPU (ruído determinístico). **Onda** (anel aditivo), **clarão** (`light_*` do RenderingServer), **tremor** (valor lido pela câmera).
- **Ruínas:** o job do `JobPool` coloca o prédio (`BuildingBodies::place`), emite fachada e telhado (`emit_facades/emit_roofs`), corta
  em células de Voronoi (grade com jitter, semente do prédio), dá espessura à casca, liga fragmentos vizinhos; peças voam/caem, as
  pousadas ficam no mesh "parado" do prédio; o que sobra desaba abaixo de 35%.
- **Vítimas/ragdoll:** corpos articulados no `PhysicsServer3D` (Jolt) em espaço próprio, juntas cone-twist/dobradiça, chão por
  heightmap local; a pose dos ossos vai para a textura de paleta do shader das pessoas; membros arrancados, vísceras (correntes de
  Verlet), sangue; quem vive levanta (devolve `get_up` à simulação).

## Desfazer, save, ChangeLog, tarefas
- Um golpe é um passo do diário (`cmd.strike`): `REM_BLD`/`ADD_MOD`/novas ops `DMG_*` com inverso; mortos não voltam (as pessoas não
  estão no histórico, como o tráfego), mas crateras, ruínas, prédios, mobiliário e terreno voltam.
- `ChangeLog` ganha `damage_events` (lista drenada pelo render sob o mutex do log) e `building_chunks`/`road_chunks` dos tocados.
- Save: `save_demolition.cpp` (crateras, ruínas, mobiliário removido, acertos de prédios, fogo restante).

## Testes e fotos (etapa de teste, lista escrita antes)
- Núcleo: curva K-B contra pontos conhecidos (Z=10 → 14,9 kPa), limiares, determinismo de `strike` (mesmo estado → mesmas mortes),
  efeito real (contagens de prédios/pedestres/vias antes e depois), desfazer devolve o estado, save+open devolve crateras/ruínas.
- E2E: HUD/snapshot mostram o dano; buffers de instância dos efeitos não vazios após o golpe e vazios quando tudo acaba.
- Fotos (mapa 0, seed 7, uma rodada de cada, câmera fora de geometria): demolir (prédio, via), pistola (pedestre), bomba força 5 em
  prédio, rua, terra e prédio grande, tempos t0/t1/t3/t8; contra as fotos do 4.0 em `ports/demolition/reference`.

## O que depende de outro dono (escrito no dono, dito no relatório)
- Aba Demolir/Pistola/Bomba da HUD e o stepper de força: Porte 4 (`drawers.json` já tem os itens; o jogo recebe `strike`/`shoot`/`demolish_box`).
- Pedestres não têm "medo": eu acrescento `panic_until` em `Pedestrian` e a leitura em `traffic/pedestrians.cpp`.

## Item 3: o corpo com a pessoa cozida e juntas (plano revisto: fontes primárias, sem cópia de shader)

**O que existe.** `people.bin` traz, por variação (32) e nível (2), a malha com 4 ossos e pesos por vértice e a paleta da caminhada (matriz osso→pose por quadro). Não traz esqueleto. O esqueleto de origem é o do MPFB2 (`makehumancommunity/mpfb2`, commit `afb9f530a7c2741dedb8df0ebae2e0b183caec21`, `src/mpfb/data/rigs/standard/rig.game_engine.json`, 53 ossos com `parent`), cuja cópia local é `public/models/people/skeleton-game-engine.json` do 4.0 (campo `source` com o mesmo commit). Os pais dos ossos vêm dele, nunca da malha: pelvis → Root; spine_01 → pelvis; spine_02 → spine_01; spine_03 → spine_02; neck_01 e clavicle_l/r → spine_03; head → neck_01; upperarm → clavicle; lowerarm → upperarm; hand → lowerarm; thigh_l/r → pelvis; calf → thigh; foot → calf. Os 20 ossos que a malha usa existem todos no arquivo (nenhum falta). A ordem dos 23 índices cozidos não está em disco (`people.json` é gerado com o servidor do 4.0 ligado): o mapeamento índice → nome (1 pelvis; 2–4 spine_01–03; 5 neck_01; 6 head; 7–10 braço esquerdo clavicle…hand; 11–14 direito; 15–17 thigh/calf/foot esquerdos; 19–21 direitos; 0 Root, 18 ball_l e 22 ball_r sem vértice) foi lido dos centroides ponderados pelos pesos (`evidence/rig_probe.py`). É inferência: o dado (`data/render/ragdoll.json`) guarda índice e nome e o teste confere que o centroide de cada osso cai onde o nome diz. Os pivôs das juntas saem da malha de cada variação (média dos vértices pesados nos dois ossos), não de números copiados.

**Corpos rígidos (a segmentação da fonte).** A tabela de massas (abaixo) e a de limites (abaixo) segmentam o corpo em 16 partes: pelve, abdome, tórax, cabeça e pescoço, e por lado braço, antebraço, mão, coxa, canela, pé. Os ossos da malha que a fonte não separa dividem o corpo da sua parte: spine_02 e spine_03 (e as duas clavículas) são o tórax; neck_01 e head são cabeça e pescoço; Root, ball_l e ball_r não têm vértice. Cada osso da malha recebe a matriz do corpo da sua parte vezes o deslocamento fixo da pose de ligação.

**Solução.** (1) `PersonRig` (render/effects): por variação, centroide, massa e pivô de cada corpo, calculados uma vez na carga a partir dos pesos do nível 0. (2) `Ragdoll`: cada corpo é rígido (posição, quaternion, velocidades), integrado por XPBD. Fontes do método: Macklin, Müller e Chentanez (2016), "XPBD: Position-Based Simulation of Compliant Constrained Dynamics", Motion in Games 2016, pp. 49–54, DOI 10.1145/2994258.2994272 (restrição com complacência α, α̃ = α/Δt², multiplicador acumulado por restrição e por passo), e Müller, Macklin, Chentanez, Jeschke e Kim (2020), "Detailed Rigid Body Simulation with Extended Position Based Dynamics", Computer Graphics Forum 39(8), pp. 101–112, DOI 10.1111/cgf.14105 (corpos rígidos por XPBD: restrições de posição e de ângulo entre corpos, juntas esféricas com limite de balanço e de torção, contato com atrito, amortecimento de junta, passos pequenos com várias iterações). Nota de autoria: a lista da coordenação trazia Kim também no artigo de 2016; os registros do artigo (ACM, página do Müller) dão só Macklin, Müller e Chentanez, e Kim consta no de 2020. Contato: esfera de cada corpo contra o terreno (altura do `CityState`, só leitura). O golpe dá a velocidade de cada corpo pela direção e a potência do evento. (3) Pintura: um MultiMesh por variação com `ragdoll.gdshader`, que **inclui `people_color.gdshaderinc` e `people_skin.gdshaderinc`** (commit 8cbc236) e só traz de seu o que muda: a pose de cada osso vem de uma textura de matrizes (3 texels por osso, ponto fixo de 16 bits como a paleta da caminhada) em vez do quadro da paleta; `skin_add` é chamado com a mesma pose nos dois lados (a = b, t = 0). Nenhum fragmento copiado. (4) Membro arrancado: o osso sai da pessoa (escala zero na matriz) e a peça segue como em `effect_bodies.cpp`. (5) O corpo some depois de `body_life_s`; nunca passa de `max_bodies`.

**Massas.** Dempster (1955) adaptado por Winter (2009), *Biomechanics and Motor Control of Human Movement*, 4.ª ed., tabela de parâmetros de segmento, conferida na tabela publicada em `BMClab/BMC` (`data/BSP_DempsterWinter.txt`, lida em 2026-11-04): mão 0,006; antebraço 0,016; braço 0,028; pé 0,0145; perna 0,0465; coxa 0,100; cabeça e pescoço 0,081; tórax 0,216; abdome 0,139; pelve 0,142; a soma dos 16 corpos dá 1,000. Cada corpo recebe a fração da sua parte; a massa total da pessoa vem do dado, com a sua fonte (média adulta, pesquisada na hora de escrever o dado).

**Limites de junta (só entra o que tem fonte).** Fonte: NASA OCHMO-HB-004 Rev. A (dez. 2023), Apêndice A, Tabela 5 "Unsuited Range of Motion" (estudo do JSC de 2007/2008 e, onde a linha diz, de 1979), PDF `https://www.nasa.gov/wp-content/uploads/2023/12/ochmo-hb-004-rev-a-dec2023.pdf`; o texto do PDF (páginas 40 a 42) foi extraído com a posição de cada número e cada valor conferido contra o rótulo da sua linha. Valores em graus (a tabela dá sinal por convenção de eixo; aqui o módulo de cada lado): tornozelo dorsiflexão 25, flexão plantar 50; joelho flexão 125, extensão 0; quadril flexão 165, extensão 15, abdução 65, adução 20, rotação interna 35, externa 35; tronco (pelve até tórax) flexão 45, extensão 15, inclinação lateral 40 para cada lado, rotação 30 para cada lado; ombro flexão 145, extensão 70, abdução 165, adução 45, rotação lateral 46 e medial 91 (linha "Shoulder 1979 Study"); pescoço (estudo de 1979, cabeça sobre o tronco) flexão 34, extensão 65, inclinação 35 e 29, rotação 73 e 72; cotovelo flexão 130, extensão 0; punho extensão 90, flexão 60 (como impresso), desvio radial 30, ulnar 25, supinação 80, pronação 80.

Como o dado usa isso: (a) cada junta tem dois eixos de balanço e um de torção, cada um com [mín, máx] lido acima; (b) a posição de referência da tabela é a anatômica (braço pendente) e a pose de ligação da malha é em A (braços descendo em diagonal, não em T: no arquivo do MPFB2 o braço sai do ombro a ≈42° da vertical, e a malha cozida a ≈49°), então o deslocamento de cada junta entre as duas é derivado da geometria dos pivôs de cada variação (ângulo entre o osso e a direção anatômica), não uma constante solta; (c) onde a tabela dá o conjunto (tronco com duas juntas, pelve → abdome e abdome → tórax), o total é da fonte e a **divisão em partes iguais é escolha de modelagem**, dita no dado; (d) o ombro da tabela é braço sobre o tronco, com a escápula já dentro: toda a faixa entra na junta tórax → braço e a clavícula é parte do tórax; (e) a cabeça e o pescoço são um corpo, com a faixa do pescoço na junta tórax → cabeça; (f) sem linha na tabela o limite é zero: inversão e eversão do tornozelo, torção do joelho e do cotovelo; a torção do antebraço é a pronação e supinação da linha do punho, aplicada na junta antebraço → mão. Nenhum limite fora destas linhas. Ressalvas ditas: no quadril a flexão de 165° é a da tabela (ângulo coxa-tronco medido em pessoa viva) e não há colisão entre coxa e barriga na simulação, então a coxa pode atravessar o tronco perto do limite; punho com extensão maior que flexão está como impresso e se confere em outra fonte antes de a pessoa ser usada de perto.

**Donos e limites.** Arquivos novos só em `render/effects/` (`person_rig.*`, `ragdoll.*`, `ragdoll_pose.*`), um shader (`ragdoll.gdshader`, com os dois includes) e um dado (`data/render/ragdoll.json`, com o `_doc` citando o commit e o caminho do MPFB2, a tabela da NASA e Dempster/Winter); `people_asset.*` ganha só um acessor de vértices e pesos do nível 0. Sem constante sem fonte: massas e limites vêm das tabelas acima; atrito, amortecimento e critério de sono são pesquisados na hora de escrever o dado e entram com fonte ou não entram.

**Provas.** Teste de render sem janela: o ragdoll cai de uma altura, repousa no chão sem atravessar, mantém o comprimento de cada osso (erro < 1 cm), nenhuma junta fora do limite da tabela (erro < 1°), centroide de cada osso onde o nome diz; contagem de instâncias, triângulos e bytes por quadro (dita no relatório). Fotos: o mesmo golpe fotografado no 4.0 e no jogo (mesma câmera, 0,5 s, 1,5 s e 5 s), pessoa de perto, lida em resolução cheia.
