# Lista de fotos pareadas do porte interior (escrita antes de tirar) — versão 3

Método (o da Porte 2, `ports/build/evidence/resposta-par.md`): os prédios do núcleo do 4.0 (`ports/build/evidence/pose/predios-40.json`, 441
documentos, 429 postos) são postos no jogo onde estão no 4.0 (`capture_pair.gd --buildings=`); a câmera do jogo é a lida do 4.0 depois de
cada `setView` (posição, olhar, vetor de cima e lente: `pose.js`); o prédio é cortado no andar pedido nos dois jogos (4.0 pelo menu
Camadas e o botão Andar acima; jogo pelo comando `set_interior`, que é o que o interruptor e a barra mandam). Mesma hora (o modo dia do
4.0 é 16h), 1600x900. Núcleo do 4.0 não tem casas: os prédios abaixo são lojas, apartamentos e escritório.

Limite conhecido de antemão: a escolha dos móveis e do piso (madeira, porcelana) depende do id do prédio (`floorKind(b, level)` no 4.0,
`floor_finish(..., pl.id)` no jogo) e o id do prédio posto no jogo não é o do 4.0; se o piso ou os móveis diferirem por isso, a análise diz
que a causa é o id e não o corte.

| # | Prédio do 4.0 | Andar | Câmera (x, y no plano do 4.0; azimute; elevação; zoom) | Hora |
|---|---|---|---|---|
| 1 | loja 1087 (4 andares, 20,5x28,6 m) | térreo | -280,9 / -163,4; 20°; 65°; 22 | dia |
| 2 | loja 1087 | 2 | a mesma do 1 | dia |
| 3 | apartamentos 126 (7 andares) | 3 | -37,8 / -296,8; 20°; 70°; 22 | dia |
| 4 | escritório 70 (23 andares), térreo (saguão) | térreo | 77,7 / 10,7; 20°; 65°; 22 | dia |
| 5 | loja 1013 (6 andares), camada na cidade | térreo | -102,6 / -229,7; 20°; 55°; 7 | dia |
| 6 | loja 1087 à noite | térreo | a mesma do 1 | noite |

Uma rodada do 4.0 (`node scripts/port.ts reference interior evidence/par/ref-plano-par.json --timeout 600`) e uma do jogo
(`capture_pair.tscn --views=evidence/par/vistas-par.json --buildings=...`), nada mais; se uma foto do 4.0 sair sem o corte, o par não
vale e a análise diz isso.
