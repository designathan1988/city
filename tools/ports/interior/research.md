# Pesquisa do porte interior (vista de dentro dos prédios)

Escrita antes da primeira linha de código. Cada item: o que foi aberto (URL), o que diz, e a conclusão para o jogo. Os textos
baixados ficaram no scratchpad da sessão (HTML e PDF convertidos em texto); as citações abaixo são curtas.

## 1. Corte de geometria e secção em tempo real no Godot 4.7

**1.1 Camadas de renderização (`VisualInstance3D.layers`, `Camera3D.cull_mask`, `RenderingServer.instance_set_layer_mask`).**
- https://docs.godotengine.org/en/stable/classes/class_visualinstance3d.html — `layers`: "The render layer(s) this
  VisualInstance3D is drawn on. This object will only be visible for Camera3Ds whose cull mask includes any of the render layers
  this VisualInstance3D is set to." Também controla quais instâncias uma luz afeta.
- https://docs.godotengine.org/en/stable/classes/class_camera3d.html — `cull_mask`: "which VisualInstance3D.layers are rendered by
  this camera".
- https://docs.godotengine.org/en/stable/classes/class_renderingserver.html — `instance_set_layer_mask(instance, mask)`: "Sets
  the render layers that this instance will be drawn to".
- **Conclusão:** camadas decidem *se uma instância inteira* aparece para uma câmera, nunca *que parte* dela. O corte por andar
  tira a parte de cima de um único prédio, e a malha do prédio vive misturada com a de outras centenas no mesmo chunk de 256 m.
  Camada não corta andar. Serve só para esconder o prédio inteiro (não é o caso: ele continua visível, baixo).

**1.2 Plano de corte no shader (`discard`).**
- https://docs.godotengine.org/en/stable/tutorials/shaders/shader_reference/shading_language.html — "Discarding: Fragment, light,
  and custom functions ... can use the discard keyword. If used, the fragment is discarded and nothing is written. Beware that
  discard has a performance cost when used, as it will prevent the depth prepass from being effective on any surfaces using the
  shader." Seção "Global uniforms": um uniforme global vale para todos os shaders do projeto.
- https://github.com/godotengine/godot-proposals/issues/4423 (e a conversa citada nele) — o Godot não tem plano de corte
  nativo; o uso corrente é passar o plano ao shader e descartar fragmentos atrás dele.
- **Conclusão (alternativa recusada):** `discard` num plano de altura cortaria o *céu do prédio*, mas (a) o shader `building` é
  comum a todos os prédios do chunk, então o corte valeria para a cidade inteira ou exigiria um id por vértice; (b) o corte não
  gera tampa: o andar cortado ficaria vazio e sem piso, e os vãos de janela não fecham; (c) a parede do lado da câmera tem de
  baixar até um rodapé, que é geometria nova (um topo e uma face interna), não só um descarte; (d) `discard` desliga o
  pré-passe de profundidade em todas as superfícies do shader (custo em toda a cidade, não só no prédio aberto). Por isso o corte
  é geometria real, emitida no lugar da fachada inteira (seção 5 do plano).

**1.3 Posição de instância e faixas de visibilidade.**
- `instance_geometry_set_visibility_range(instance, min, max, ...)` (mesma página do RenderingServer; "Equivalent to
  GeometryInstance3D.visibility_range_begin"). O renderizador de prédios do jogo já usa isso para trocar near/mid. O chunk com um
  prédio aberto precisa ficar no nível detalhado qualquer que seja a distância da câmera (decisão do plano).

## 2. Interior mapping versus geometria real
- van Overveld/van Dongen, "Interior Mapping: a new technique for rendering realistic buildings", CGI 2008:
  https://www.proun-game.com/Oogst3D/CODING/InteriorMapping/InteriorMapping.pdf — raios no shader contra planos de piso, teto e
  parede dão cômodos "perspectivamente corretos" atrás da janela; "the number of rooms rendered does not influence the framerate
  or memory usage" e a colisão do raio tem tempo constante. Não modela móveis em geometria.
- **Conclusão:** interior mapping (já usado pela Porte 2 em `render/buildings/pane_rooms.*` + `building_pane.gdshaderinc`) é a
  técnica certa para *janelas vistas de fora*: custo constante por pixel, memória zero por cômodo. Não serve para o prédio cortado:
  ali o jogador olha o piso de cima, por uma abertura sem vidro, e quer ver planta, paredes, núcleos e móveis com paralaxe
  completa. O custo de geometria real é por peça e só existe para o prédio aberto (um andar), não para a cidade: um prédio
  cortado emite 1 piso + paredes internas + os móveis desse andar. O plano calcula o pior caso (cidade inteira aberta).
- Num prédio cortado o vidro das janelas **não** deve desenhar o cômodo por interior mapping (o cômodo real está do outro lado do
  vidro): o corte emite as janelas com `PaneRoom` zerado (vidro liso).

## 3. Planta, núcleos e circulação (valores que viram dados)
Textos baixados e lidos nos trechos citados.
- **ABNT NBR 9050:2020 (versão corrigida 25.01.2021)** — cópia hospedada pela Prefeitura de São Paulo:
  https://drive.prefeitura.sp.gov.br/cidade/secretarias/upload/NBR9050_20.pdf (39.711 palavras extraídas).
  - 6.8.2 (escadas): "0,63 m ≤ p + 2e ≤ 0,65 m; pisos (p): 0,28 m ≤ p ≤ 0,32 m; espelhos (e): 0,16 m ≤ e ≤ 0,18 m". Uso: lance
    da escada do núcleo e entre blocos em meio-nível (altura do espelho 0,18 m, piso 0,28 m: 2·0,18 + 0,28 = 0,64 m, dentro da faixa).
  - 6.11.1 (corredores): "0,90 m ... até 4,00 m; 1,20 m ... até 10,00 m; 1,50 m ... superior a 10,00 m; 1,50 m de uso público".
  - 6.11.2.4 (portas): "vão livre maior ou igual a 0,80 m de largura". Uso: largura dos vãos de porta das paredes internas
    (o gerador de plantas já usa 1,0 m; 0,80 m é o mínimo, não o valor).
  - 6.10.2.1: "O elevador vertical deve atender à ABNT NBR NM 313". A NBR NM 313 não está aberta ao público: **a cabine do elevador
    não tem fonte primária aqui**. A cabine é derivada do poço (poço menos parede e folga); fica registrada como *sem fonte
    primária* na cobertura.
- **Saídas de emergência (NBR 9077 adotada pelos corpos de bombeiros)** — a NBR 9077 é paga (ABNT); as instruções técnicas dos
  bombeiros a aplicam e são públicas. Aberta: Goiás NT 11/2022
  https://www.bombeiros.go.gov.br/wp-content/uploads/2021/03/NT-11_2022-Saidas-de-Emergencia.pdf
  - 5.7.3.1: degraus com altura "entre 16 cm e 18 cm, com tolerância de 0,5 cm" e largura pela fórmula de Blondel
    "63 cm ≤ (2h + b) ≤ 64 cm" (a mesma faixa que a NBR 9050 6.8.2, mais estreita).
  - 5.4.2.1: largura mínima de saída "1,2 m para as ocupações em geral" (1,65 m no grupo H-2/H-3); unidade de passagem 0,55 m
    (5.4.1.3). Uso: largura do lance da escada de emergência; o lance de núcleo desenhado é de 1,2 m.
  - Não encontrei o texto da NBR 9077 em si em fonte aberta; a conclusão vale pela instrução técnica que a aplica.
- **Conclusão:** estas normas fixam *dimensões de elementos de circulação* (escada, corredor, porta). Elas não dizem onde o
  núcleo fica na planta; essa posição é do gerador (`massing_tall.cpp`, já portado). A planta, as paredes e os móveis seguem as
  regras de `sim/buildings/interior` (Porte 2); este porte só os mostra.

## 4. Iluminação interior
- https://docs.godotengine.org/en/stable/tutorials/3d/lights_and_shadows.html — "Light number limits": no Forward+ há o limite
  padrão de 512 elementos agrupados (luz omni, spot, decal, sonda) na vista da câmera, ajustável em Project Settings →
  Rendering → Limits → Cluster Builder; no Mobile, 8 OmniLights por malha.
- https://docs.godotengine.org/en/stable/classes/class_omnilight3d.html — `omni_range`: "The light's radius ... the light will
  never reach anything outside this radius"; luz omni com sombra custa um mapa cúbico por luz.
- https://docs.godotengine.org/en/stable/classes/class_light3d.html — `light_cull_mask` ("The light will affect objects in the
  selected layers"), `light_energy`, `light_temperature`, `shadow_enabled` (padrão falso).
- **Conclusão:** poucas OmniLight3D sem sombra, com `omni_range` curto, sobre as lâmpadas do andar aberto (o 4.0 usa 6). O
  andar aberto é iluminado por elas, mais forte à noite; a cor da lâmpada vem do mesmo `lamp_mix` do vidro (corpo negro, já
  portado). A intensidade em lux não tem fonte aqui: a tabela da NBR ISO/CIE 8995-1 não está em fonte aberta que eu tenha
  conseguido ler (as páginas achadas são secundárias e divergem), então a energia da luz é **calibrada por medição** nas fotos e
  fica registrada como tal.

## 5. Decisões que a pesquisa fecha
1. O corte é geometria real emitida pelos mesmos emissores de fachada, não clipping nem camada (seção 1).
2. As janelas do prédio aberto não usam interior mapping (seção 2).
3. Escada: espelho 0,18 m, piso 0,28 m (NBR 9050 6.8.2 e Goiás NT 11 5.7.3.1); lance de 1,2 m (Goiás NT 11 5.4.2.1).
4. Luzes: no máximo algumas OmniLight3D sem sombra, `omni_range` do cômodo; limite de 512 elementos agrupados (seção 4).
5. Cabine do elevador e lux por cômodo: sem fonte primária; derivado/calibrado, marcados na cobertura.

## 6. Luzes de cômodo: a conversão do three.js do 4.0 para o Godot (acrescentada na rodada da base integrada)
- 4.0, `src/render/renderer.ts:143` (`ROOM_LIGHTS = 6`), `:1624-1626` (`new PointLight(0xffd6a0, 0, m(8), 2)`, sem sombra), `:3713` (intensidade
  `55 * TUNED_UNIT ** decay * (0.25 + 0.75 * lastDark)`), `src/core/units.ts:12` (`TUNED_UNIT = m(0.4)`), `src/render/roomLamps.ts:20-45` (as lâmpadas do andar,
  na ordem do mobiliário).
- three.js 0.186 (o do 4.0, `node_modules/three`): atenuação `1/max(d^decay, 0.01) * saturate(1 - (d/cutoff)^4)^2`
  (`src/renderers/shaders/ShaderChunk/lights_pars_begin.glsl.js:56-71`, `getDistanceAttenuation`, de Lagarde e de Rousiers, "Moving Frostbite to PBR",
  p. 32 eq. 26); difuso `RECIPROCAL_PI * diffuseColor` (`common.glsl.js:103-105`, `BRDF_Lambert`), logo a luz direta que sai de uma superfície é
  `albedo/π · intensidade · atenuação · cosθ`. Não há modo antigo de luzes (`useLegacyLights` não existe no código do 4.0).
- Godot 4.7, [OmniLight3D](https://docs.godotengine.org/en/stable/classes/class_omnilight3d.html): `omni_attenuation = 1.0` por padrão (curva "de artista");
  "um valor de 2.0 é o fisicamente correto: dá a atenuação do inverso do quadrado"; "a luz nunca passa do `omni_range`, qualquer que seja a atenuação". O
  `light()` dos shaders deste jogo recebe `LIGHT_COLOR` já com π (`building_glass.gdshader`, comentário), então o difuso padrão do Godot sai
  `albedo · energia · atenuação · cosθ`, sem o 1/π.
- Conclusão: o equivalente é `omni_attenuation = 2`, `omni_range = 8 m` e energia `55 · 0,4² / π = 2,80` (intensidade do 4.0 em m² dividida por π). O que não
  está provado é o fator de exposição entre os dois renderizadores inteiros (tom, ambiente): isso a foto da noite do par mede.
