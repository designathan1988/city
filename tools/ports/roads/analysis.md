# Porte roads: análise (notas de trabalho)

## Estado de partida
- Clone em a11384a, limpo. `roads-greide.md` da pasta compartilhada é de antes do recomeço das 12:00 (queue.md): nada daquilo existe.
- Foto `antes/06_cruzamento.png` não mostra cruzamento (prédio na frente): preciso de captura própria.

## Limites de dono (records/queue.md)
- `elevation.ts`, greide, pontes, túneis, altura da via: `road-elevation` (mesma sessão, a seguir).
- `streetFurniture`, `streetObjects`, postes, sinais, barreiras: `street-furniture` (porte-6).
- `economy.ts`: `economy` (porte-3). `streetscapeTool`, `gullies`: `landscape` (porte-3). `src/sim/**`: `traffic` (porte-5).
- `ui/inspector.ts`, `ui/v2/shell.ts`: `interface` (porte-4).

## Modelo de dados
- 4.0: classe (6) + lanes + direction + RoadSection + parking. Jogo: Profile = lista de Strip (equivale a RoadProfileSpec.elements) em road_profiles*.json.
- Conclusão: Strip continua sendo o modelo; o editor e "Meus modelos" produzem Profile em tempo de execução (campo `custom` já existe).

## Ordem dos temas
1. Encaixe e ferramenta de via (arrastar, encadear, curva, livre, grade, guias), painel de opções.
2. Mover nó. 3. Catálogo em 5 categorias, modelos, editor de perfil. 4. Faixas, largura, vagas.
5. Cruzamentos (esquinas, pintura). 6. Geração de ruas (streets.ts, plan.ts).

## Fotos do 4.0 tiradas por esta sessão (ordem do usuário: as do coordenador não valem)
Planos: `look-roads.json`, `look-roads2.json`, `look-roads3.json`. Cidade pequena, seed 7. Pasta `reference/`.
- Válidas, vias em chão livre, sem prédio na frente: `58-free-junction-urban-top`, `61-free-junction-urban-low`, `55-free-junction-avenue-top`, `58-free-junction-avenue-low`, `61-free-junction-t-top`, `64-free-road-end`.
- Válidas, na cidade: `06-junction-avenue-top`, `09-junction-avenue-low`, `15-junction-local-low`, `18-junction-t-top`, `06-city-junction-local-straight-down`, `64-city-junction-avenue-top`, `67-city-junction-avenue-low`, `70-city-junction-t-top`.
- Válidas, interface e ferramenta: `26-drawer-draw`, `29/32/35-catalog-*`, `40..52-tab-*`, `61-tool-chain-straight`, `64-tool-laid-straight`, `69-tool-landing-on-road` (mostra a via cruzando, não pousando), `72-tool-laid-t` (o pouso em T), `83-tool-curve-bend`, `86-tool-laid-curve`, `96-profile-editor`.
- Não usar (nome diz por quê): `12`, `21`, `53`, `73`, `91`, `50`.
- Falta: foto do traço à mão ondulado. Ponteiro sintético não desenha (2 tentativas, saldo igual); `look.mjs` só arrasta reto. Exige mouse real por script Playwright próprio.
- Abertas, ainda não analisadas uma a uma: as demais da lista acima.

## O que as fotos ensinam sobre vias
- Preview: fita com bordas verdes, círculos nas pontas, etiqueta "270 m · R$ 410.400" junto do fim; já mostra a pintura da via.
- Ponta de rua: corte reto, sem balão; calçada termina junto.
- Cruzamento: raio de guia, faixa de retenção, faixa de pedestre, linha amarela tracejada (via urbana); avenida com linha amarela contínua e tracejado branco entre faixas.

## Fotos da sessão 4 do 4.0
`12-tool-free-wavy` (traço à mão de verdade), `17/20/23-tool-roundabout-*`, `28-roundabout-top`, `31-roundabout-low`, `34-road-end-bulb`.
A causa do traço que não saía: os eventos iam ao primeiro canvas, não a `#game`, e `setPointerCapture` lançava erro com ponteiro sintético antes de `take`.
Atenção: a pasta `reference/` tem fotos de sessões diferentes com o mesmo número; vale o nome inteiro. Fotos do jogo: `evidence/fotos-antes` (a11384a) e `evidence/fotos-depois` (16h). `fotos-depois2` foi tirada ao meio-dia e NÃO serve para comparar cor.

## Tom da via: questão aberta, sem correção
Medido na mesma luz (o modo dia do 4.0 é fixo às 16h, renderer.ts:3725; o jogo também):
- capim: 4.0 97, jogo 98 (bate). Asfalto ao sol, miolo do cruzamento: 4.0 mediana 56, jogo 135. Calçada: 4.0 156, jogo ~205.
- Hipótese refutada: cor sRGB usada como linear. road.gdshader converte (`srgb_to_linear(COLOR.rgb)`).
- O que os números indicam: ganho de luz (saída/albedo em linear) ~1 no 4.0 e ~3,4 no jogo, com albedo do asfalto 0,04 contra 0,07. O capim bate por ter cor própria.
- Não corrigi: `sun_energy`/`exposure` afetam tudo (prédios, árvores) e não são deste porte; mudar a cor da via para compensar esconderia a causa. Ainda não medi o ganho do 4.0 com um material neutro, então "ganho global" é hipótese, não prova.
- Comando para medir: `scratchpad/tons2.py` (mediana por trecho ao sol).
