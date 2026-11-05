# Lista de fotos do porte interior (escrita antes de tirar) — versão 2

Por que a versão 1 caiu: ela dizia que os ids de prédio são os mesmos no 4.0 e no jogo. Conferido por dados (não por foto): a cidade
pequena semente 7 do 4.0 tem 1348 prédios (`evidence/ref-ids.json`, de `window.__roadcraft.doc.buildings.all()`) e o mapa 0 semente 7
do jogo tem 3154 (`evidence/jogo-ids.log`); o prédio 146 é uma casa de 2 andares nos dois, mas em lugares diferentes (jogo -697,8 / 303,0;
4.0 -37,8 / -461,9), e 434 e 42 são loja e escritório no 4.0 e casas no jogo. A cidade grande de 5 km do 4.0 não gerou na página (parada
detectada aos 62 s, `evidence/ref-run-ids-large.log`). Logo **não existe o mesmo prédio nos dois jogos**: nenhuma foto do jogo abaixo
tem par do 4.0 (regra 4 de `regras-sessoes.md` §9), e a análise de cada uma diz isso. As fotos servem para achar defeitos do jogo.

Jogo, mapa 0, semente 7, 1600x900, Forward+, `project/tests/visual/capture_interior.tscn` (mesma tabela em `SHOTS`).

Rodadas do jogo, ditas como foram: r1 (5 fotos da lista 1, rodou com erro de script por quadro nas luzes e a hora da noite não chegou ao céu),
r2 (esta lista, mas a espera do cenário falhava por chamada errada a `render_pending`: as fotos saíram sem esperar os pedaços),
r3 (esta lista, tudo certo na ferramenta; a foto 6 da torre saiu encoberta por vizinhos altos de 20 andares, e a leitura mostrou as janelas
tapadas por dentro), r4 (esta lista com a foto 6 trocada para a casa 146 e a janela corrigida). Passou de 2 rodadas por causas da minha
ferramenta e de defeitos achados nas fotos, não por varredura de parâmetros; fica dito no relatório.

| # | Prédio | Andar | Câmera (volta a partir da frente do lote) | px/m no alvo | Inclinação | Hora | Nome |
|---|---|---|---|---|---|---|---|
| 1 | casa 146 (2 andares, forma em L) | térreo | 0° | 38 | 75° | 12:00 | casa146-terreo |
| 2 | casa 146 | 1º | 90° | 38 | 75° | 12:00 | casa146-andar1-giro90 |
| 3 | torre 1604 (comercial, 21 andares) | 10º | 0° | 20 | 65° | 12:00 | torre1604-andar10 |
| 4 | comercial 1090 (3 andares) | 1º | 0° | 30 | 70° | 12:00 | comercial1090-andar1 |
| 5 | casa 146 | térreo | 0° | 38 | 75° | 22:00 | casa146-noite-terreo |
| 6 | casa 146, camada ligada pelo interruptor (prédio no meio da vista) | térreo | 0° | 8 | 60° | 12:00 | cidade-camada-146 |

O que cada foto confere: (1) piso, paredes baixas, divisórias, móveis; (2) o telhado do anexo baixo não atravessa o andar (defeito da
rodada anterior, corrigido por `roof_clip`); (3) planta de escritório em andar alto, enquadramento pelo corpo; (4) comercial de poucos
andares; (5) a noite chega ao céu e as luzes de cômodo acendem; (6) a camada ligada na cidade, como o jogador faz.
