# Lista de fotos da etapa de teste — demolition (escrita antes de fotografar)

Regras: mapa 0, seed 7, 1600x900, uma instância por `hidden_run.ps1`, câmera fora de geometria, ≤600 s, saída em `ports/demolition/evidence/fotos-3/`.
Rodada final do jogo (a 3ª da bomba só porque a 1ª foi derrubada antes de fotografar; a bomba aqui entra para pareamento com o 4.0, no máximo 12 fotos).

## A. Pareada com o 4.0 (`reference/r2-*`): mesma vista (azimute 25°, inclinação 38°), casa baixa
| Foto | Pareia com | O que se confere |
|---|---|---|
| bomba_f5_t0 / t05 / t1 / t2 / t5 / t12 | `13..23-r2-0x-b5` | bola de fogo, fumaça cinza, anel, nada de buraco em casa de pé; fumaça some em ~5 s |
| bomba_f45_t0 / t05 / t1 / t2 / t5 / t17 | `30..40-r2-1x-b45` | nuvem de poeira e fogo, prédios que desabam, monte de peças, cratera marrom no fim |

## B. Pistola e corpos (câmera 8 m do pedestre)
| Foto | O que se confere |
|---|---|
| ped_antes | o pedestre em pé, vivo |
| ped_tronco_t0 / t1 | primeiro tiro no tronco: traçante, sangue, ferido curvado ou no chão |
| ped_perna_t0 / t1 / t3 | tiro na perna: cai, corpo no chão (peças com pele e roupa), levanta |
| ped_cabeca_t0 / t1 / t8 | tiro na cabeça: morre, corpo e poça de sangue ficam |

## C. Carro (câmera 10 m)
| Foto | O que se confere |
|---|---|
| carro_antes / carro_tiro / carro_estouro_t0 / t2 | faíscas e cacos de vidro, o carro pára, explode na rajada |

Foto fora da lista não entra no relatório. Comando: `tools/build/hidden_run.ps1 ... capture_demolition.tscn -- --phase=pareada,people,car --capture-dir=...`.
