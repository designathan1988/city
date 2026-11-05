# Rodada do 4.0 (traffic): lista escrita antes de fotografar

Cidade pequena, seed 7, plano `look/look-rodada-4.json`, saída em `ports/traffic/reference/`.

| Foto | O que prova | Par no jogo |
|---|---|---|
| classe-hatch, sedan, suv, van, truck, bus, motorcycle, bicycle | forma e pintura de cada classe, três quartos de frente, mesmo ângulo | a tirar depois, por classe |
| lote-fileira | fileira de carros num lote, quase de cima | `rodada-3b/31_vaga_lote.png` |
| lote-manobra | carro andando dentro do lote | `rodada-3b/32_carro_manobra.png` |
| semaforo-dia, semaforo-noite | semáforo baixo, na altura das cabeças | `19_semaforo_*` |
| noite-sedan, noite-bus | faróis, lanternas e vidro à noite | a tirar depois |
| doca-chegando, doca-parado | caminhão no pátio e parado na doca; pode não sair se nenhum for chamado em 200 s | a tirar depois de portar |

Medido na rodada: docas reais da cidade (`doc.buildings.all()`), que substitui o número falso da rodada anterior.

## Resultado (13:28 a 13:32)
Valem: 09-classe-hatch, 21-classe-truck, 33-lote-fileira (com fumaça de escapamento), 47-noite-sedan (lanternas e faróis).
Longe demais para par de modelo: 12-classe-sedan, 15-classe-suv, 18-classe-van, 27-classe-motorcycle.
Inválidas: 24-classe-bus (nenhum ônibus na frota: não há linhas), 30-classe-bicycle, 39-semaforo-dia e 44-semaforo-noite (câmera entre prédios altos), 36-lote-manobra (não mostra manobra), 50-noite-bus.
Não saíram: doca-chegando e doca-parado (200 s sem caminhão no pátio; só 2 caminhões na frota).
Medido: 81 docas em 32 prédios (o número 0 da rodada anterior estava errado).
