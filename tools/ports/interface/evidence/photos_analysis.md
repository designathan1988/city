# Análise das fotos da interface (rodada final)

Cada par está em `evidence/pares/<nome>.png`, com o 4.0 em cima e o jogo embaixo, no mesmo recorte de 1600×900. As fotos do 4.0 são minhas (`reference/`, planos `look-interface*.json`), cidade `small`, semente 7. As do jogo são `evidence/features/shell_*.png`, mapa 0, semente 7, Forward+.

**Hora:** cada par traz o relógio das duas fotos na legenda. O 4.0 foi fotografado com o relógio em 12:00 a 12:06, e a tela inicial às 06:32. No jogo, a tela inicial sai na hora de abertura; depois o botão do relógio e a velocidade 4× levam a 12:00, e o relógio só anda para cada foto. Resultado: 06:34 no início e 12:01 a 12:12 no resto, no máximo 11 minutos do par no 4.0. Por isso o botão aceso na barra é o relógio no jogo e o sol no 4.0. Em `health`, a barra do 4.0 está sob o painel, e a hora não se lê.

**Rodada na base integrada (master `95e3d47`):** `evidence/pares/` foi refeito; relidos em resolução cheia `start`, `hud`, `menu`, `layers`, `sim`, `drawer_roads`, `drawer_zones`, `drawer_land`, `drawer_ground`, `drawer_nature`, `drawer_sky`, `drawer_build_models`, `drawer_build_draw`, `insp_road` e `insp_node`. Os demais são telas que o merge não tocou. Os achados dessa rodada estão no relatório, em "Achados e correções"; onde a tabela abaixo diverge dele, vale o relatório. A luz do master já está na base; a luz nova da Porte 3 ainda não.

**O que estes pares provam:** só a interface. **Não servem como prova de paridade de mundo:** este clone não tem a luz da Porte 3, e cor, luz e sombra ao fundo são do visual antigo. A câmera do jogo e a do 4.0 também não mostram a mesma cidade. Li cada par inteiro em resolução cheia. "Sem defeito" quer dizer que conferi posição, tamanho, texto, cor e estado da interface, e não achei diferença além das listadas.

| Par | Defeitos e diferenças | Situação |
|---|---|---|
| `start` | Tamanho, Semente e Estilo regional a mais: pedidos da Porte 2 e da coordenadora, regra 10; o cartão fica mais alto. Cartão 380 px e botões de 46 px batem. Escurecimento pela vinheta medido: razão cena/fundo 0,458 a 0,552 no jogo contra 0,459 a 0,537 no 4.0 | Corrigido, sobra só o acréscimo |
| `hud` | Sem defeito na barra e no dock. Hora, veículos e pessoas são outros porque a simulação é outra. Cor do texto do dock e de 2×, 4× corrigida para `text`: era `muted` (163,180,176 contra 237,243,241) | Corrigido |
| `menu` | Linhas "Salvar (Ctrl+S)" e "Carregar o último jogo" a mais: funções do jogo, mantidas. Gráficos em uma linha, porque cabe | Declarado |
| `layers` | Interior dos prédios apagado: é da sessão interior | Dono: interior |
| `sim` | Sem defeito. Os valores são da simulação do jogo | Fechado |
| `help` | "Page Up / Down" move a câmera no jogo; no 4.0 sobe nó suave, que aqui não existe (road-elevation). Largura igualada a 380 px | Corrigido, resto declarado |
| `camera` | Sete botões com os ícones do 4.0, painel colado neles. Zoom 34 %: a foto do jogo está mais longe | Fechado |
| `graphics` | **Sincronia vertical (VSync): acréscimo do jogo.** A placa de vídeo mostra só o nome porque o Godot não tem a cadeia do ANGLE. Passo entre grupos: ver "Medidas" | Declarado |
| `about` | Sem defeito. Texto da versão vem do git | Fechado |
| `health` | Ponto redondo, sobre a barra, abas como o 4.0. Eventos são os reais de cada jogo | Fechado |
| `profiler` | Canto certo, dentro da tela. Conteúdo é outro: o 4.0 mede o navegador (descartes, heap JS, geometrias), o jogo mede os passos do `RenderWorld`. Sem botão de copiar JSON | Declarado |
| `tip` | Dica própria, centrada sob o botão, negrito. Sombra menor que a do 4.0 | Fechado; sombra aceita |
| `drawer_roads` | **Dono: roads.** Catálogo antigo (Ruas 13, Rodovias 6, Trilhos 3 contra Ruas 12, Rodovias 5, Meus modelos 0; Rua local 11,8 m R$ 1.378/m contra Rua residencial 10 m R$ 1.220/m), painel de opções à esquerda e Mover. Prova: `converter/out/features/roads/boundary.json` e o branch da Porte 1. Miniaturas: ver "Medidas" | Corrigida a cor; resto é roads |
| `drawer_rb` | Raio sem "(m)". Faixa do controle 14 a 60 m contra 32 a 128 m: é regra da ferramenta, `tuning.json`, dono roads | Declarado |
| `drawer_zones` | Cartões, amostras redondas e Densidade por ícone batem. Aba Lotes apagada (sessão build). Com a ferramenta, o jogo tinge faixas de 28 m por lado de via; o 4.0 tinge lotes, que o jogo não tem como objeto. Roxo do uso misto: a cidade gerada tem esse uso | Declarado |
| `drawer_build` | Cartões de função no lugar das plantas; abas apagadas: sessão build | Dono: build |
| `drawer_land` | Sem Dureza e Apagar, abas apagadas: sessão landscape. Escalas de força diferentes | Dono: landscape |
| `drawer_transit` | Abas apagadas: sessão transit | Dono: transit |
| `drawer_demolish` | Largura da nota igualada (duas linhas). Pistola e Bomba: Porte 7 | Dono: Porte 7 |
| `insp_road` | Cartões de números, ações em duas colunas, faixa azul sobre a via e pontas redondas. Faltam nome e numeração da rua, perfil, alturas e curva: sessões roads e road-elevation, com prova em `coverage.md` | Fechado para a interface |
| `insp_node` | Cartões, fase com a cor e lâmpadas como o 4.0. Falta o painel "Controle do cruzamento": roads | Fechado para a interface |

## Medidas pedidas
- **Altura de linha em Opções gráficas:** passo entre grupos, de título a título, 68 a 69 px no 4.0 e 72 px no jogo; a diferença de 3 px vem de uma linha divisória e do espaço entre os grupos.
- **Peso da fonte:** o jogo só tem pesos pelo `SystemFont` do sistema; título do grupo em peso normal e cor apagada, como no 4.0; botão escolhido em peso normal, onde o 4.0 usa 650, porque o tema do Godot não troca fonte por estado.
- **Miniaturas dos cartões de perfil:** o jogo estava **mais escuro**, não mais claro. Grama 54,83,43 contra 84,116,63; calçada 121,133,126 contra 196,192,180. Causa: a cor do ícone do botão (`muted`) multiplicava a figura. Razão pelo 0,64/0,71/0,69 de `#a3b4b0`. Com a figura em branco, a grama deu 85,117,63, igual ao 4.0.

## Interruptor de camada e sombra (`shell_switch_on`, `shell_switch_off`)
Antes e depois, mesma câmera. Decisão: **tira de tudo**. O interruptor de prédios tira o prédio e a sombra dele, porque `shadow_caster_mask` segue a máscara da câmera. Na foto de depois não há sombra de prédio no chão; ficam só as de árvores e postes, que continuam ligados. Motivo: sombra sem objeto seria um defeito visível, e no 4.0 a chave esconde o objeto, com a sombra junto.

## Cenas com ação
- **Relevo (`feat_terrain_1_chapada`, `feat_terrain_2_desfeito`):** os 0,84 % de pixels eram do meu roteiro, que dava dois traços e desfazia um. O desfazer funciona: 20 → 65 → 20 m, no núcleo e no Terrain3D. As paredes da chapada têm borda serrilhada na resolução do terreno: Porte 3.
- **Rotatória (`feat_roundabout_2_construida`):** refeita com a câmera sobre o ponto. Anel, quatro acessos e custo no aviso. Sem defeito de interface.
- **Camadas (`feat_layers`):** zonas do jogador e congestionamento ligados juntos. Sem o jogador zonear, a cidade gerada não se tinge (`road_commands_test`).
