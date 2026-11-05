# Plano do porte people

Base a11384a, branch `convert/people`, clone `ports/people/workspace`. Pesquisa em `pesquisa.md`, cobertura em `cobertura.md`.

## Causas medidas
1. Ninguém sai nem entra em prédio: `spawn_one` nasce no meio da calçada e `at_node` some no nó.
2. Um quarto dos corpos é criança andando sozinha e um quarto é idoso, no ritmo do adulto: `people_renderer` escolhe a variação pelo hash, sem classe (32 corpos: 8 por faixa). No 4.0: 9 % e 20 %.
3. Todos sozinhos: 0 em par contra 55 % no 4.0.
4. Quem espera na guia fica congelado no meio do passo: só o clipe de caminhada foi cozido.
5. Nível distante paga sombra real em toda pessoa até 280 m.

## Cortes de dono
- **Bicicleta, simulação e malha: Porte 5** (`c36331f`, `58549a8` em `convert/traffic`, 16 arquivos). Não refaço. Aqui: o clipe `pedal` no mesmo cozimento e o corpo que pedala, que depende de `VehicleType::BICYCLE`, ausente nesta base; vai como remendo pronto em `evidence/` para a integração.
- Gestos, adereços: o motor do 4.0 não os usa (censo: 0 de 86). Não se aplica.
- Ragdoll, feridos, pânico, fumaça: Porte 7. Embarque: Porte 5/transit.

## Solução
**Portas, `sim/traffic/ped_doors.cpp` (novo) e `sim/buildings/lot_doors` (novo).** `lot_door(body, building)`: porta na frente do volume de entrada, 0,45 m fora da parede, e o pé do portão 0,6 m fora da divisa, no mesmo `entrance_x`: reta sem cruzar nada. Índice por (segmento, lado), preguiçoso, com orçamento por passo, invalidado pela revisão do mapa. Novo `PedMode::LOT`, fora da multidão. Nascer: com a parte `out_share(tipo, hora)` sai da porta; senão, nasce na calçada, como hoje. Chegar: no último trecho escolhe uma porta à frente, no mesmo lado, e entra; sem porta, some no nó como hoje.
**Companhia, `sim/traffic/ped_party.cpp` (novo).** 40 % nascem com alguém ao lado: mesma rota, mesmo ritmo (o do mais lento), o seguidor fica ao lado do líder no trecho e segue o líder em esquina, travessia e porta. Criança sempre acompanhada.
**Idade.** Classes criança, adulto, idoso no `Pedestrian`, publicadas no `AgentRecord`; ritmo por classe; render escolhe só corpos da classe.
**Clipes.** Cozinhar walk, idle reamostrado a 60 quadros, pedal, num bloco por variação. Quem para passa do passo ao idle aos poucos (`Pedestrian::rest`, alfa da instância), o idle pelo relógio. `people.bin`, `cook_people.py` e `LICENSES.json` num commit, texto MIT junto.
**Sombra.** Até 125 m sombra real; além, disco sob a pessoa (`people.blob`), um MultiMesh, material próprio, sem escrever profundidade.
**Gente por hora.** `ped_demand.cpp`: pessoas por tipo de lugar e hora no lugar de `people_per_lane_m` e `zone_walk`.
**Destino.** A viagem mira uma calçada com portas e chega pelo lado dela (`plan_corner` com `want_side`).

## Dados
`traffic.json`: `ped_company_share`, `ped_company_child_share`, `ped_company_catch`, `ped_child_share`, `ped_elder_share`, `ped_child_pace`, `ped_elder_pace`, `ped_rest_fade_s`, `ped_door_off_m`, `ped_gate_out_m`, `ped_door_reach_m`, `ped_door_budget`, `work_stay_h`, `errand_stay_h`, tabelas `people_kind_m`, `people_busy`, `people_working`, `people_purchasing`, `zone_kinds`, schema junto. `agents.json`: `people.blob`.

## Testes e fotos
Sonda nova `people_census`: por classe, grupo, modo e lote. Teste: quem sai de porta está na porta; par mantém a distância; criança nunca sozinha. Soak e memória contra a base. Fotos 16:00 em par, lista antes: casal, família, criança, idoso, saída do lote, entrada, espera na guia, multidão distante (`reference/` 08, 11, 14, 17, 23, 26). Na espera, sem alvo no 4.0: dizer que não há par.

## Ordem dos commits
1. Cozimento e clipes. 2. Idade. 3. Portas. 4. Companhia. 5. Sombra. 6. Documento, relatório, cobertura.
