# Rodada final de fotos do street-furniture (lista escrita antes de fotografar)

Regra da coordenadora: no máximo uma rodada completa daqui em diante; depois de correção, só as cenas afetadas.
Código fotografado: o commit citado em `final/commit.txt`, com gates OK e DLLs de release e debug compiladas.

## `final-rua/` — `capture_street.tscn`, dirigida pelo mouse (o que cada foto prova)
- `rua_01_aba`: aba Rua com o mesmo arranjo de `reference/rua/23-aba-rua`.
- `rua_02_postes_opcoes`: linha de opções Luzes abaixo dos cartões, sem mudar a largura dos cartões.
- `rua_03_postes_previa`: prévia 3D da linha durante o arrasto.
- `rua_04_postes_prontos`: a linha construída no mesmo lugar da prévia.
- `rua_05_pecas`: as onze peças colocadas pelo mouse, botão Desfazer aceso.
- `rua_06_barreiras`: as cinco barreiras construídas (a checagem exige 5).
- `rua_07_placa_opcoes`: seletor de tipo e campo de texto da placa.
- `rua_10_rua_dia` e `rua_11_volta_dia`: poste, fios e coluna contra `reference/rua/07-poste-rua` e `10-poste-rua-volta`.
- `rua_10_rua_noite` e `rua_13_ao_longo_noite`: noite sem cone duro, contra `reference/rua/17-poste-noite`.
- `rua_20_*`: cada peça e barreira de perto (banco, lixeira, hidrante, caixa, orelhão, cerca, muro, sebe, defensa, gradil).
- `rua_21_placas`: frente das placas com texto.

## `final/` — `capture.tscn`, a cidade inteira (regressão)
- `01_rua`, `04_rua_baixa`, `06_cruzamento`, `13_calcada`: nada perdido fora da rua.
- `19_semaforo_dia_*`, `19_semaforo_noite_*`: semáforo contra `reference/rua/22-semaforo-rua` e `25-semaforo-perto`.
- Log `final.log`: memória estática, de vídeo e de buffers, comparadas às da base.

Tudo o que estiver em `street1` a `street7` e `cap1` é rodada de trabalho, anterior ao código final: não vale como prova.
