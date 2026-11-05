# Relatório do porte: traffic

Branch `convert/traffic` do clone `ports/traffic/workspace`, sem push. Mapa 0 (`--map=0`), Forward+ nas fotos, 1600x900.

## O que li
- `converter/out/features/traffic`: `feature.md`, `source/`, `boundary.json`. Refiz o rastreamento eu mesma: 184 arquivos, 3.322 declarações; conferi contra o `src/` do 4.0 (as cópias Labs e Projects são idênticas).
- `regras-sessoes.md`, `CLAUDE.md` do clone, `converter/README.md` e `DESIGN.md`.
- No 4.0, além do rastreamento: `render/carBody.ts`, `render/vehicleModels.ts`, `render/agents.ts`, `render/vehicleWindows.ts`, `render/riderPoses.ts`, `sim/vehicles/kerbStops.ts`, `sim/roads/controlAdvisor.ts`, `sim/roads/flowStats.ts`.

## Requisitos
Estacionar (rua e lote), docas e caminhões, embarque, coordenação de semáforos e onda verde, ciclofaixa e ciclistas. Soak com 0 entradas no vermelho e 0 perdidas. Ampliado pela coordenadora, por decisão do usuário: paridade por classe com foto do 4.0 e do jogo na mesma pose, paleta, emissão de fumaça por classe, faróis e lanternas à noite, estilos regionais.

## Pesquisa e fontes
| Assunto | Fonte |
|---|---|
| Vagas e pintura | CTB art. 181 I; MBST vol. IV (MER, LPP) |
| Bicicleta no bordo | CTB art. 58; AASHTO Guide for the Development of Bicycle Facilities (0,6 m do bordo) |
| Tempo de semáforo | CONTRAN MBST vol. V (entreverdes, janela de pedestres, atuação, cap. 7) |
| Onda verde | ciclo comum e defasagem pelo tempo de viagem, MBST vol. V |
| Manobra do caminhão | Reeds e Shepp, "Optimal paths for a car that goes both forwards and backwards" (1990); Dolgov et al., A* híbrido (2008); Felzenszwalb e Huttenlocher, distância exata |
| Vidro | CONTRAN Res. 960/2022 art. 4º I (transmitância 70 %); Schlick 1994; physicallybased.info |
| Paleta | Axalta 2025 Global Automotive Color Popularity Report (nota IPCM: branco 35, cinza 23, prata 14, azul 7, vermelho 4 %); frota: Veloe e Fipe sobre Senatran 2024 |
| Parada na guia | CTB art. 182 I (5 m do alinhamento da transversal) |
| Curva de Hermite monótona | Fritsch e Carlson 1980 |
| Godot | `tonemap.glsl` do 4.7 (ACES, inverso em `render/tone_map.gd`), MultiMesh, RenderingServer |

## Plano, por tema
1. Vagas, carros parados e tráfego dos lotes. 2. Bicicletas e ciclofaixa. 3. Semáforo: tempo fixo, defasagem, onda verde, painel, save v2. 4. Caminhões às docas. 5. Paleta e estilos. 6. Paridade por classe. 7. Embarque na guia.

## Commits
```
a10336d Vagas de rua e de lote, carros parados e tráfego dos lotes
bf7d133 Sonda do custo do passo do tráfego por parte
c36331f Bicicletas e ciclofaixa; carro comprometido não é chamado a um lote
58d6f21 Veículo nasce antes da linha de parada, com distância para parar
8d6681e Carro estacionado vira linha da vaga; parados publicados só quando mudam
6ad5165 Custo dos lotes: olhada a cada 0,25 s, sem alocar na publicação, frota conta quem manobra
58549a8 Bicicleta no bordo da pista onde não há ciclofaixa (CTB art. 58)
3085398 Capturas do tráfego parado: vaga, lote, manobra, bicicleta e ciclofaixa
2e7b66f Semáforo em tempo fixo, defasagem e onda verde
d768370 Caminhões às docas, com a rua fechada enquanto manobram
97ed8f8 Paleta por fonte para comparar, e a emissão de fumaça por classe em dados
2d426d1 Estilos regionais do tráfego: paleta e lado da mão por estilo
7da4b7f Carroceria dos carros como a do 4.0: colunas, molduras, vidro, lentes e perua
```
Os últimos commits desta entrega estão em `git log master..convert/traffic`.

## Resultados
Gates OK. DLL de release e de debug compiladas (`cmake --build native/build-win` e `build-win-debug`, -j 4). Import sem erro de script ou de shader.
Testes (`evidence/testes/resumo.txt`, cada um com limite de 280 s, no desktop oculto): 6 cenas em 8 execuções, todas com saída 0 e nenhum SCRIPT ERROR nem FAIL: road_commands, save_undo, camera_input, e2e, light_color (Compatibility e Forward+), building_light (Compatibility e Forward+). O prompt fala em 8 testes: são estas 8 execuções.

### Soak (`capture.tscn -- --map=0 --soak=5 --only=none`)
5 minutos: 0 entradas no vermelho, 0 do parado, 0 perdidas, 0 travamentos. Embarque em 3 minutos: 57 paradas planejadas, 33 descidas, 6 subidas, 18 entregas, 0 desistências.
O soak não exercita caminhão às docas: o mapa 0 não tem indústria.

### Carros de lote (`capture.tscn -- --only=trafego`, sonda `lot_clearance`)
Antes: 128 de 144 tocavam (muro até 0,50 m, volume até 0,95 m). Agora: 115 de 175 tocam, mediana 8 cm, pior 83 cm (pilar, picape em lote pequeno). **Não está resolvido.** Causa medida: o caminho segue o centro do carro e o canto da carroceria abre na curva dos acessos estreitos; dar mais folga (0,6 m) não mudou a mediana. Solução, ainda não feita: o A* híbrido do pátio também para os carros.

## Fotos analisadas
Pares em `evidence/fotos-jogo/paridade-6/lado` (`<par>-inteiro.png` e recortes 3x), 4.0 à esquerda, jogo à direita. Foto do 4.0 em `ports/traffic/reference/`. Relógio do jogo em 16:00 (22:00 nas noturnas). Lista de defeitos e causas: `evidence/paridade-defeitos.md`.

Li inteiras, em 1600x900, as fotos abaixo; recortes 3x conferidos em hatch e SUV. "Par válido" é mesma pose, hora e tamanho.

| Par | Vale? | O que vi |
|---|---|---|
| hatch frente e trás | sim | Forma, colunas, moldura preta, vidro, teto, tampa escura, placa, lanterna e espelho batem. Defeitos: pessoa de blocos; raios mais claros e mais finos que os do 4.0; ruído fino do sol no capô; farol branco sobre branco quase some na carroceria branca, como no 4.0, mas sem o tom creme tão forte. |
| sedã frente e trás | trás sim; frente com poste na frente | O "sedã" do 4.0 é a perua; a do jogo bate na silhueta, janela traseira e tampa. Cor diferente (sorteio da paleta). |
| SUV frente e trás | trás sim; frente com carros na frente | Trás: silhueta, vidro e tampa batem; raios claros demais. Frente: capô de carro escuro mostra reflexo em manchas largas, o 4.0 é liso. |
| van frente | sim, com poste na frente | Cabine, para-brisa alto e moldura batem. Bancos e motorista de blocos. |
| van trás | **não** | Caminhão colado à câmera tapa o quadro no jogo. A checagem de visada só olha o centro dos veículos. |
| moto frente e trás | **não** | No 4.0 a moto fica 0,75 m mais perto da câmera que o ponto gravado; a correção não achou moto com visada livre. Na foto fora de quadro: rodas inteiras, garfo, motor, tanque e farol certos; piloto de blocos e cortado. |
| bicicleta frente e trás | **não** | Mesmo motivo. Rodas com raios e quadro certos; ciclista de blocos. |
| caminhão frente e trás | **não** | Fotos do 4.0 tapadas. Nenhum veredito. |
| sedã à noite frente e trás | sim | Faróis, lanternas com brilho e placa batem. Asfalto bem mais claro que o do 4.0; dono: luz da via. |
| lote, semáforo dia e noite | **sem par do jogo** | Fotos do 4.0 existem, mas o par do jogo não foi tirado. |
| `37_embarque` | sem par | Pessoa na calçada saindo do carro parado na faixa; há uma faixa de vagas entre a porta e a guia, como o 4.0 permite. Sem porta aberta. |

As análises antigas (rodadas 1 a 3 do jogo) ficam sem valor de par: foram feitas em outros enquadramentos. Não as refiz uma a uma; o que elas cobriam de veículo está aqui, o de vagas e ciclofaixa segue sem par.

## Constantes com fonte, e as que são só do 4.0
Com fonte: tabela de pesquisa acima, cada uma no `_doc` do seu JSON; dimensões de veículo pelas fichas (`agents.json`).
Só do 4.0, sem fonte, dito no `_doc`: linhas dos carros (`top`, `screen`, `rear`, `belt`, `doors`, `tumble_m`), medidas de `car_body`, `moto.shape`, cor dos lentes acesos e apagados, `tint` do vidro (a 40 % em luz linear, medido na foto), os números de `_kerb_doc`, salvo `kerb_end_clear_m`, e a emissão de fumaça em `exhaust`. A vaga "demais" da paleta de frota é uma cor sem fonte.

## Cobertura
`ports/traffic/cobertura.md`, gerado por `evidence/cobertura_gerar.py`: 3.322 declarações. Portado 1.026, já existia 1.543, não se aplica 351, de outra funcionalidade 358, **não portado 44**. O que não foi portado: `vehiclePanels` (portas que abrem), `flowStats` e `controlAdvisor` (controle automático do cruzamento pelo fluxo).

## O que medi
Soak; contagens de vagas, lotes e docas; `lot_clearance`; `vehicles_near` (SUV do item 32: desvio 0,0 m, folga 0,75 m de cada lado; "três caixas" do item 30: uma picape); ciclo do caminhão uma vez, em via nova com zona industrial (chamado no passo 22, encostado no 32, saiu no 48; 74 docas).

## O que não medi, com o comando
- CPU do tráfego em máquina quieta: o soak marcou 0,75 a 3,2 ms/passo com outras sessões rodando, contra base 0,44. Todas as partes subiram juntas, então não dá para atribuir. `capture.tscn -- --map=0 --soak=5 --only=none`, em máquina quieta.
- Quadros, GPU e memória de vídeo depois da carroceria nova: `tests/perf/camera_pan`, máquina quieta. O nível distante ficou com 9 linhas no anel, contra 11 antes, mas o nível de perto subiu de triângulos: `CityStore.query(&"agent_cost")` dá a conta.
- Efeito da onda verde no tempo de viagem.
- Caminhão repetido várias vezes num ciclo longo.
- Caminhão, moto, bicicleta e van por trás: sem par válido (tabela acima). As duas fotos do caminhão no 4.0 saíram tapadas e a cota de rodadas do 4.0 acabou. Ônibus fica para a rodada do transit.
- Fotos de vagas, lote, doca e semáforo no jogo em par com o 4.0.
- GPU AMD e Intel.

## Defeitos conhecidos que ficam abertos
- Carros de lote raspando (acima).
- Pessoas em veículos, motociclista e ciclista são de blocos; o 4.0 usa o corpo dos pedestres sentado. É o maior desvio visual que resta. Falta o clipe sentado em `people.bin` e uma instância por ocupante perto da câmera.
- Portas não abrem, sem pisca, janelas não baixam.
- Linha pontilhada no chão da foto de doca `36_doca_3`: não identificada.
- Padrão fino de ruído no sol sobre carroceria e asfalto, visível no recorte 3x: é do sombreamento direcional, em toda a imagem, não do veículo. Fora do meu tema, para quem cuida da luz.
- À noite o asfalto do jogo fica bem mais claro que o do 4.0 e as faixas brilham: luz da via, Porte do mobiliário.
- Estilos: placa por estilo vazia, `eua` e `europa` sem números, semáforo por estilo sem leitor, faixas para mão esquerda.
- Fumaça: dados prontos, à espera do nome da API da Porte 7.

## MultiMesh (pedido da Porte 6)
Os agentes usam só o alfa da cor por instância, como semente; RGB é sempre 1. Tirar a cor economizaria cerca de 48 B por instância (48 KB por mil). Mantido.

## Adendo: capturas sem a qualidade do jogo e pasta de usuário própria

`capture.gd` não chamava `Settings.apply`: todas as fotos deste relatório anteriores a `5952e5f` saíram sem MSAA, e com sombra e escala fora do preset. Corrigido; o log mostra `QUALITY_MEDIUM`, `msaa_3d 1`. Desde então a pasta de usuário é própria (`project/override.cfg`, `Roadcraft-traffic`, fora do git). Medidas anteriores valem com a ressalva do cache compartilhado.

Pares refeitos em `evidence/fotos-jogo/paridade-8`, 13 dos 16 de `pares-final.json` (`lado/` tem os lados a lado). Vi em folha de contato (`folha-0.png`, `folha-1.png`), não em resolução cheia, salvo `par-sedan-frente`. Com MSAA as bordas de carroceria e faixa ficam lisas; não restou serrilhado a tratar.
- Valem: hatch de frente, hatch de trás, sedã de trás, SUV de trás, os dois de noite. Hatch de trás tem poste e pedestre na frente.
- Não valem: sedã e SUV de frente (outro carro no primeiro plano), caminhão e van (muro, árvore ou parede na frente), bicicleta de trás (zoom diferente). Mesma limitação já declarada para caminhão, bicicleta e van de trás.

`36_doca_3` não foi refeita, então a "linha pontilhada" segue sem conclusão. A rodada `--only=doca` não teve caminhão.

### Defeito aberto, achado nesta rodada

Na zona industrial que a captura pintou agora, nenhuma doca ganhou caminho: `docks_planned` 50, `docks_with_way` 0, 0 caminhões chamados em 30000 ticks, 726 chamadas recusadas por falta de caminho (`evidence/fotos-jogo/doca-msaa/run.log`). A rodada `doca-5` (13:59) achou 74 docas e completou o ciclo. O local mudou de (845, -938) para (-842, 865): a captura escolhe a ponta da rua pelos veículos da amostra, e `assign_errand` (3ddb04f) consome o gerador. O código do pátio não mudou nesse intervalo, então a causa provável é o lugar, mas **não está provada nem corrigida**, e o soak não mede isso porque o mapa 0 não tem docas.

### Corrigido na onda 2

O defeito acima está corrigido em `convert/transit`, e a causa não era o lugar. A hipótese sobre o local, a linha da moto na tabela de pares ("0,75 m mais perto") e os vereditos de `paridade-8` estão superados. Ver "Onda 2" em `ports/transit/relatorio.md`.

### Onda 3

O trabalho seguiu em `convert/transit`, a ponta única; `convert/traffic` segue parado em `6713e79`. Detalhes em `ports/transit/relatorio.md`, seção "Onda 3". O que toca o tráfego:

- O plano do pátio saiu do worker: é contado em trabalho, tick a tick, e duas rodadas dão as mesmas marcas (`356f597`, com a correção obrigatória em `15db376`).
- Freio de dia na sua cor: a lanterna creme do SUV era isso (`24cce8a`). À noite, o freio continua creme pela conta.
- O contrato dos ocupantes para a Porte 6 está lá.
