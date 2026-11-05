# Rodada de fotos: prova na base integrada, depois do talude

Master integrado, `user://` próprio, MSAA do jogo por `Settings.apply`, espera por `CaptureWait`.

| Foto | Par no 4.0 | O que precisa provar |
|---|---|---|
| `median_nose_top`, `median_nose_low` | `47-`, `50-median-nose` | canteiro, esquina, asfalto claro, sem parede, pedras no sentido do 4.0, rampa sobre a zebra |
| `median_top`, `median_low`, `median_paved_low` | `20-`, `23-`, `53-` | canteiro e canteiro pavimentado sem regressão |
| `pair_urban_top`, `line_near`, `close_kerb_low` | `58-`, `69-`, `60-` | via comum sem regressão |
| `fotos-v3-msaa4/median_nose_top` | `47-median-nose-top` | controle: com MSAA 4x, se o meio-fio de baixo chega aos 3,56 px do 4.0 |
