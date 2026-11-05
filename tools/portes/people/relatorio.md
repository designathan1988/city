# Relatório do porte people

Branch `convert/people`, clone `ports/people/workspace`, base a11384a. Sem push, sem integração. `project/override.cfg` (`Roadcraft-people`) fora do git.

## Requisitos e o que foi feito
| Pedido | Estado |
|---|---|
| Portas dos prédios | feito: sai pela porta e pelo portão, entra no fim da viagem |
| Gente em par, com criança, idoso | feito |
| Quanta gente por lugar e hora | feito |
| Parado em pé | feito: idle com transição, no lugar do passo congelado |
| Sombra de quem está longe | feito: disco além de 125 m |
| Ciclistas | **em parte**: simulação e malha são da Porte 5 (`c36331f`, `58549a8` em `convert/traffic`); aqui só o clipe `pedal`, cozido e **sem uso**. O corpo que pedala não está ligado: depende de `VehicleType::BICYCLE`, ausente nesta base. 62 declarações da cobertura ficam "pendente" |
| Texto MIT da Rocketbox | feito: em `LICENSES.json`, que já vai no pacote |

## Causas e correções
1. Ninguém usava porta. `lot_doors` acha a porta e o pé do caminho; `ped_doors` arquiva por calçada, lendo 6 lotes por passo. A viagem agora mira uma calçada com portas pelo lado dela (`plan_corner` com `want_side`).
2. Um quarto dos corpos era criança sozinha e um quarto idoso, no passo do adulto: o corpo vinha do hash. Agora vem da idade.
3. Sem companhia: `ped_party`, lado a lado, em fila onde não cabe ou há móvel ou gente à frente.
4. Densidade fixa o dia todo: `ped_demand`, por tipo de lugar e hora. Saíram `people_per_lane_m` e `zone_walk`.
5. Rosto vermelho: o interior da cabeça contaminou a cor no pacote (numa idosa, 302 de 323 vértices). A pele da cabeça agora é a do corpo: 12.901 vértices. O rosto fica sem traços.
6. Cintura: medi que a calça não atravessa a blusa (borda 3 a 8 cm abaixo da barra, até 2 vértices por quadro); a pele do tronco aparecia na faixa. 358 vértices pintados.

## Erros meus, achados pelos testes e fotos
- `sidewalk_under` comparava Z com a **altura** da pose: 827 de 1.606 portas caíam no lado errado da rua e gente cruzava fora da faixa. Corrigido, 0 agora, e o teste exige zero. A sonda e a foto 35 acharam.
- Supus que a calça atravessava a blusa e escrevi a retirada do que a roupa cobre: tirou 5 triângulos. A medição desmentiu; o código saiu.
- Escrevi nos dados "86 pessoas em 150 m" antes de medir. Medido: 59 a 61. Texto corrigido.
- Escrevi "medido" para a curva `people_busy`; é desenhada à mão no 4.0, sem pesquisa. Texto corrigido.

## Medições
Censo às 16:00 na base integrada (master 95e3d47), `people_test`, mapa 0. A cidade é outra, mais densa, com a lotação calculada pelo que há nela:
| | Jogo | 4.0 ou base |
|---|---|---|
| Pessoas na janela | 836 (pedidas 832) | |
| Em 150 m do centro | 104 | 86 (4.0, cidade menor) |
| Crianças sozinhas | 0 | |
| Às 3 h | 236 | constante na base |
| Dentro de móvel | 0 | 0 (base) |
| Portas lidas, cruzando a rua | 1.027, 0 | |
| Corpos que se tocam | 13 de 836, nenhum entre quem anda só | 3 de 1.000 (base, cidade antiga) |

Os 13: 10 de quem segue o par e 3 de quem lidera, segundo `touching_by`. Os 0 de quem anda só corrigem o que escrevi antes, com a cidade antiga: o toque é da companhia. Base sem companhia não foi medida nesta cidade.

As porcentagens de grupo, idade e chegada por porta abaixo são de antes do merge, com a cidade antiga: grupo 64 %, idosos 17 %, crianças 13 %, chegada por porta 93 %, calçadas com porta 520 de 780. O teste confere as faixas de grupo, idade e criança na cidade nova, com 0 falhas.

Memória na base integrada, 1600x900, sem cache de pipelines (`mem_clipes.log`): estática 185,6, vídeo 556,3, buffers 233,5 MiB. Sem delta: não tenho a mesma medida do master sem `people`. Os clipes novos pesam 18 linhas por corpo: +0,30 MiB de textura, +0,6 MiB de arquivo (`people.bin` 9,06 MiB).

## Clipes de quem vai em veículo
Pedido da Porte 5 (`ports/transit/evidence/contrato-porte6.md`), feito inteiro em `tools/assets/rider_poses.py`: `chairSit`, `carDrive`, `carRide`, `carRearRide`, `cabDrive`, `cabRide`, `motoRide`, `motoStop`, `bikeStop` (2 quadros) e `bikePedal` (30). `people.bin` passou à versão 3: tabela nome → linha e quadros no cabeçalho, e por corpo e clipe a pelve e as extensões acima, abaixo, à frente e de lado.

**O pacote só trazia `pedal`.** Os outros são poses que o 4.0 resolve na hora por cinemática inversa, e o pacote não guarda o esqueleto. Ele sai das paletas: o filho fica preso ao pai, então a junta é o ponto que as duas matrizes levam ao mesmo lugar (mínimos quadrados em todos os quadros). A pelve sai de `pedalFrame` em `tools/cook-people.mjs`, que a põe em (0; 0,93; -0,14). **Prova:** com essas juntas, mão e tornozelo do `pedal` dos adultos caem nos alvos do cozinheiro do pacote, [0,24; 0,95; 0,36] e [0,13; 0,31; -0,01], em 3 casas.

**O `bikePedal` foi refeito:** o do pacote é de outra bicicleta (quadril 0,93, contra 0,98 no jogo). Moto e bicicleta usam as medidas de `moto.rider` e `bicycle.rider` da Porte 5. O tronco inclina até a mão chegar na manopla. O pedivela de 170 mm é medida comum, não está em `agents.json`.

Posição real, metros a partir da pelve (mão esquerda; adultos, média de 8 corpos):
| Clipe | Mão | Pedido | Pé E | Pé D |
|---|---|---|---|---|
| `motoRide` | 0,256; 0,086; 0,636 | 0,26; 0,08; 0,65 | 0,17; -0,614; 0,36 | -0,17; -0,614; 0,36 |
| `motoStop` | 0,254; 0,093; 0,623 | 0,26; 0,08; 0,65 | 0,397; -0,705; 0,117 | -0,17; -0,614; 0,36 |
| `bikeStop` | 0,249; 0,024; 0,515 | 0,25; 0,02; 0,52 | 0,305; -0,778; 0,091 | -0,12; -0,448; 0,007 |
| `bikePedal` (quadro 0) | 0,249; 0,024; 0,516 | 0,25; 0,02; 0,52 | 0,12; -0,61; 0,23 | -0,12; -0,61; -0,11 |

**Crianças não alcançam:** em `motoRide` a mão e o pé ficam cerca de 15 cm curtos; é o tamanho do corpo. Não ponha criança em moto nem bicicleta de adulto. Assentos de carro têm a mesma limitação, menor (`carDrive`: mão [0,147; 0,396; 0,275] contra 0,16; 0,44; 0,40).

**Prova no jogo:** `evidence/clipes-jogo/` (`--only=clipes`, sonda `show_people_clip`), quem espera na guia em cada clipe, pelo `people.gdshader` real. `folha.png` amplia a mesma pessoa nos 8: pose certa, pele sem rasgo, valores da paleta até 1,868 dentro do ponto fixo de ±2. **Sem veículo em volta:** nenhuma foto mostra mão em volante ou manopla, porque ninguém ainda desenha os ocupantes. `carRearRide` e `cabRide` não foram fotografados, só medidos; os lados e o telefone do 4.0 não foram cozidos, como o contrato pede.

**Procedência:** poses por cinemática inversa no próprio esqueleto, sem animação de terceiros (`LICENSES.json`). O 4.0 tem respiração no sentado (6 s); aqui é parado, 2 quadros, como o contrato pede.

## Verificação
- Gates OK no HEAD. DLLs de release e debug compiladas.
- Testes depois dos clipes: `people` e `e2e` em Forward+ e Compatibility, com 0 falhas e 0 SCRIPT ERROR. Era pendência: o `people_test` agora rodou também em Compatibility. `road_commands`, `save_undo` e `camera_input` passaram antes do merge e não foram repetidos.
- **Não medidos:** tempo de quadro, CPU, GPU e `camera_pan`, com a máquina carregada. O custo por passo de `read_doors` e da companhia também não; só mantive a lotação. Soak longo não rodou.

## Fotos
**Base integrada** (master 95e3d47 mais `a72c729`), relógio em 16:00: `evidence/final-integrado/` (as 8 cenas) e `evidence/final-casal/` (`30_casal` e `33_idoso` refeitas). Sem par novo do 4.0: os pares abaixo são de antes do merge, e a cidade mudou.
- `final-casal/30_casal`: casal lado a lado na calçada, passo e altura certos. A de `final-integrado` ficou tapada por uma copa.
- `final-integrado/31_familia`, `32_crianca`, `34_saindo`, `35_entrando`, `36_espera`: lidas na folha `final-integrado/folha.png`, a 800 px por foto, não em resolução cheia. Gente no lugar certo, sem cruzar grade fora do portão.
- `final-integrado/33_idoso`: a pessoa aparece de lado, sem rosto legível. A refeita caiu dentro de uma copa e não vale. **Rosto de idoso segue sem confirmação em foto.**
- `final-integrado/37_multidao`: gente a mais de 125 m existe na foto, mas nas calçadas à sombra dos prédios. **A mancha de 125 m segue sem confirmação em foto.**
- A copa perto da câmera vira manchas verdes chapadas (`final-integrado/30_casal`, `final-casal/33_idoso`). Não é de `people`: fica com a paisagem.

### Pares, de antes do merge
Relógio do jogo em 16:00, luz em 16:00, `Settings.apply`. Pares lado a lado em `evidence/pares/` (jogo à esquerda). Enquadramento é parecido, não igual: as cidades são outras.
- `casal`: lado a lado como no 4.0; rosto limpo em 5x. Os corpos do 4.0 têm rosto com traços e roupa inteira, melhores que os nossos.
- `familia`: adulto com criança na calçada. O par 11 do 4.0 mostra um casal, então vale só para corpo e passo.
- `espera`: quatro pessoas em pé na guia, sem congelar; no 4.0, adulto com criança.
- `saindo`: da porta, no eixo do portão. Atravessa a grade fechada, como no 4.0, onde o portão também não abre.
- `entrando`: um já no caminho do lote, o par atrás na calçada.
- Sem par válido: `37_multidao` (o 4.0 saiu alto demais) e `32_crianca` (par 14 é casal).

## Defeitos que ficam
- Barra da blusa recortada: em `entrando` há mancha clara nas costas da moça de azul; na base integrada, a barra branca serrilhada de `final-integrado/33_idoso` e a faixa clara na barriga de quem está em `37_multidao` parecem o mesmo defeito. Não é pele (ampliar a regra não mudou nenhum vértice); está na peça do pacote. Pede recozinhar das fontes em `lab/roadcraft-novo/public/models`.
- Rosto sem traços: o 4.0 tem olhos e boca definidos de perto.
- Corpos que se tocam: 13 de 836 às 16 h na cidade nova, todos de quem anda em par. A base não foi medida nesta cidade.
- 7 % das chegadas ainda somem no nó, em ruas sem porta.
- Tabelas de hora são dos EUA (ATUS) e `people_busy` não tem fonte; valem para o estilo padrão.

## Para a integração
- Conflito certo com `convert/traffic`: `VehicleType`, `traffic.json`, `demand.cpp`, `agent_renderer.cpp`. Falta desenhar os ocupantes com os clipes (`ports/transit/relatorio.md`, "Estado para quem continua"): o clipe vai pelo nome, em `PeopleAsset::clip`.
- `people.bin` mudou de formato (versão 3): recozinhar se outra sessão mexer. `python3 tools/assets/cook_people.py <pacote> project/assets/people`, sem `-I`, senão não acha os módulos vizinhos.
- Rua (`convert/street-furniture`) e pessoas mexem em `CLAUDE.md`, `traffic_system.h` e `render_world`: sem conflito de lógica esperado, não testado.

## Commits
`a72c729` clipes de quem vai em veículo, versão 3; `f614edd` merge do master 95e3d47. Antes: `ddc3dff` idade, idle e pacote; `c54548e` portas, par, hora; `44d5dd8` mancha; `6e4c349` rosto e cintura; último, captura. Cobertura: 157 arquivos, 1.965 declarações: 259 portadas, 1.044 já existiam, 600 não se aplicam, 62 pendentes (as poses de `riderPoses.ts`: agora cozidas, falta desenhá-las nos veículos). Pesquisa em `pesquisa.md`, plano em `plano.md`.
