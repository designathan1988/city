### Contrato com a Porte 6: quem vai dentro dos veículos

Hoje o ocupante é de blocos, dentro da malha do veículo (`Figure`, `Limb::SEAT`, `agent_cabin.cpp`). O desenho novo é meu, os clipes são da Porte 6.

**Como eu desenho.** Cada ocupante perto da câmera vira uma instância no mesmo MultiMesh dos pedestres (`people_renderer`), com o mesmo esqueleto no shader e o mesmo `INSTANCE_CUSTOM` (variação, frames de cima, frames de baixo + cabelo + pele, fase). A matriz sai de `pose do veículo × assento`, escrita por quadro no CPU, só para quem está no raio. A pelve da pessoa fica no ponto H do assento. Longe do raio não há ocupante: o vidro já é opaco. Os blocos saem da malha quando isso entrar. Sem IK no jogo: mãos e pés vêm prontos no clipe.

**Clipes que preciso em `people.bin`.** Nomes do 4.0 (`RiderClipKey`), no mesmo esqueleto de 23 ossos, com a mesma paleta (3 texels RGBA por osso e quadro):

| Clipe | Quem usa | Quadros | Como avança |
|---|---|---|---|
| `carDrive` | motorista de carro, mãos no volante | 2 | parado |
| `carRide` | passageiro da frente | 2 | parado |
| `carRearRide` | banco de trás | 2 | parado |
| `cabDrive` | motorista de ônibus e caminhão | 2 | parado |
| `cabRide` | carona do caminhão | 2 | parado |
| `chairSit` | passageiro do ônibus | 2 | parado |
| `motoRide` | moto andando | 2 | parado |
| `motoStop` | moto parada, um pé no chão | 2 | parado |
| `bikePedal` | bicicleta andando | 30 (já cozido, `pedal_row` 97) | uma volta do pedivela por ciclo, pela distância, nunca por `fps` |
| `bikeStop` | bicicleta parada, um pé no chão | 2 | parado |

Os lados (`Left`, `Right`, `Mirror`, `Phone`) ficam para depois: nada no jogo ainda escolhe entre eles.

**O que o arquivo precisa trazer.**
- Uma tabela por nome no cabeçalho: nome do clipe → linha e quadros dentro do bloco da variação. Não quero linha fixa no código, como `pedal_row` é hoje. Se a versão subir de 2, avise o número.
- Por clipe sentado, a posição da pelve no espaço da raiz (x, y, z, metros, por variação), porque eu ancoro a pelve no assento.
- Por clipe sentado, as extensões do corpo em relação à pelve (acima, abaixo, à frente, de lado), para encaixar a pessoa no assento como o `seatFitScale` do 4.0: a menor razão entre a folga do assento e a extensão, no máximo 1,1.
- As 32 variações e os 2 níveis, como hoje. Clipe parado não precisa de `fps`.

**Quantos e a que distância** (lidos do jogo, `load_[i].seats`, sem o motorista):

| Veículo | Motorista | Passageiros no máximo | Lotação |
|---|---|---|---|
| sedã, hatch, SUV, perua, picape | 1 | 3 | faixas de `occupants.load.car` (67 %, 27 %, 5 %, 1 %) |
| van | 1 | 1 | `car` |
| ônibus | 1 | 55 | `bus`, cinco faixas iguais |
| caminhão | 1 | 1 | `truck`, meio a meio |
| moto, bicicleta | 1 | 0 | só quem pilota |

Raio: até `cabin_radius_m`, 140 m, o de hoje da cabine. As fileiras de trás vão até 140 / 1,6 = 87,5 m, a mesma razão do 4.0 entre a fileira da frente e as outras. Nível 0 do esqueleto até 40 m (`people.lod_m`), nível 1 depois. O pior caso por veículo é o ônibus cheio, com 56 instâncias. Quantas instâncias a cidade soma no raio eu meço quando o clipe chegar: hoje não há como contar sem ele.

**Duas rodas.** O 4.0 desenha moto e bicicleta em tamanho fixo ("hands on grips only at that size"). Por isso o clipe `motoRide` tem de pôr as mãos nas manoplas e os pés nas pedaleiras da Honda CG 160 de `agents.json` (`moto.rider`: quadril 0,86 m, mão [0,26; 0,94; 0,41], tornozelo [0,17; 0,246; 0,12]). O `bikePedal` faz o mesmo com `bicycle.rider` (quadril 0,98 m, mão [0,25; 1,0; 0,30]). Se o corpo cozido não bater nessas medidas, quem ajusta sou eu, nos dados do veículo: me mande a posição real da mão e do pé no clipe.
