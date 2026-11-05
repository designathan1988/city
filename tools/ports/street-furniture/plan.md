# Plano do porte street-furniture (sessão 6)

Escopo do prompt: postes, fios, barreiras, placas e semáforos da rua. Aba Paisagem › Rua (a sessão landscape deixou para cá).
Fora daqui, com dono em outro porte: travessia no meio da quadra e retorno (`streetObjects.ts`, própria do rastreamento `roads`),
explosões (`demolition`), árvores e relevo (`landscape`).

## Decisão central
O jogo deriva o kit da rua por regra (`sim/roads/street_furniture`, com normas), o 4.0 guarda tudo no documento. Fica assim:
- O kit derivado continua (segue a rede sozinho, sem peça órfã; os pedestres já o leem). Nível por segmento = conjunto do 4.0 (completo, básico, nenhum), já em `SEG_FURNITURE_MASK`.
- Novo dono `sim/street`: o que o jogador põe e tira, em `CityState::street`: itens, postes, vãos, barreiras e cortes (a marca que tira uma peça derivada, com desfazer).
- `traffic::street_furniture_of` junta os dois: quem lê (pedestres, render) vê uma lista só.

## Camadas
- `sim/catalog/furniture_types.h` + `street_furniture.json`: tipos novos, blocos poles, barriers, signs, com fonte.
- `sim/street/street_works.{h,cpp}`: registros, índice por célula, consultas.
- `sim/street/footway.{h,cpp}`: o que há sob um ponto (segmento, lado, v, u), calçada, canteiro, área de travessia.
- `sim/street/street_snap.cpp`: onde um item fica ou por que não fica (offFootway, narrow, occupied, crossing).
- `sim/street/pole_arms.cpp`: cruzetas por deflexão (poleArms).
- `sim/street/pole_routes.cpp`: grafo das linhas de guia e Dijkstra; `pole_runs.cpp`: plano da linha (cantos, estações, reuso, lâmpadas).
- `sim/street/derived_signs.cpp`: placas das regras.
- `sim/commands/street_commands.cpp`: comandos com journal (ADD/REM item, pole, span, barrier, cut); `journal_ops`, `edit_history`.
- `sim/persistence`: SAVE_VERSION 2, bloco street.
- `render/props`: `pole_kit`, instâncias por chunk (poste, fio, caixa de barreira), placas no mesh do chunk com UV do atlas; `sign_atlas` + `project/render/sign_sheet.gd`.
- `props.gdshader`: partes por máscara, fio curvado no vértice, grão da madeira, placa.
- `project/game/tools`: pole, barrier, streetscape; `hud_drawer` + `drawers.json`: aba Rua; catálogo de vias: conjunto de mobiliário.
- Teste novo `street_works_test`, capturas `capture_street.gd`.

## Fontes: o que foi lido e o que não foi
Com o aval do usuário, baixei e li em 2026-11-04 dois PDFs, por `pdftotext`, nas seções citadas:
**CONTRAN, MBST vol. I, Sinalização Vertical de Regulamentação, 2022** (cópia DNIT) e **Neoenergia DIS-NOR-012 rev. 08** (Critérios para Elaboração de
Projetos de Rede de Distribuição Aérea, 14/08/2025). Não li os documentos inteiros.

| Constante | No jogo | Fonte | Situação |
|---|---|---|---|
| R-1 | lado 0,35 m, 0,845 m entre lados | MBST I 3.7, recomendado urbana (mínimo 0,25) | confere |
| R-2 | lado 0,90 m | MBST I 3.7, recomendado urbana (mínimo 0,75) | confere |
| Placa redonda | 0,50 m | MBST I 3.7, recomendado urbana, demais vias (mínimo 0,40) | CORRIGIDO: era 0,60 |
| Altura da placa | borda inferior 2,1 m | MBST I: entre 2,0 e 2,5 m do solo | CORRIGIDO: o centro a 2,2 m deixava o Pare a 1,78 m |
| Afastamento da placa | poste a 0,75 m da guia | MBST I: borda da placa a 0,30 m da pista em reta; 0,40 em curva | CORRIGIDO: sobrava cerca de 0,13 m. Em curva a folga é 0,30, abaixo dos 0,40 |
| Vão | 35 m, máx. 40 m | DIS-NOR-012 6.6.31.9: 30 a 40 m, básico 35 | confere |
| Engastamento | e = L/10 + 0,60 m | DIS-NOR-012 6.8.2.6 e Tabela 30 | confere |
| Cruzeta | 2 m | DIS-NOR-012, lista: cruzeta 2,0 m | confere |
| Altura dos fios | ponto mais baixo 8,03 m (8,75 - 0,72 de flecha a 40 m) | DIS-NOR-012 Tabela 1, ruas e avenidas: 5,0 / 5,5 / 6,0 m | confere |
| Altura do poste | 12 m, 10,2 m acima do chão | DIS-NOR-012 6.8.2.2 e Tabela 30: urbana com média tensão, 12 m, 1,8 m enterrados | CORRIGIDO por decisão do usuário: era 10 m. Fica acima do 4.0 (8,5 m). A luminária ficou a 7,16 m, abaixo da rede |
| Flecha dos fios | 1,8 % do vão, até 1,4 m | as tabelas estão na DIS-NOR-013, 014 e 018, não lidas | NÃO VERIFICADA NA FONTE PRIMÁRIA: comportamento do 4.0 |
| Defensa, postes a cada 2,00 m | 2,00 m | NBR 6971 é paga; só DER-PR ES-OC 07/18 e ES-SV 07/23 | NÃO VERIFICADA NA FONTE PRIMÁRIA |
| Placas de pedestre e escola | 0,7 m | são de advertência, MBST vol. II, não lido | NÃO VERIFICADA NA FONTE PRIMÁRIA; a altura segue a regra do vol. I |
| Cor da luz de rua | `#ffd3a5`, 4000 K | ABILUX e nota técnica do Inmetro 2025, só por trechos de busca | NÃO VERIFICADA NA FONTE PRIMÁRIA |
| NBR 9050 faixa livre 1,20 m | 1,20 m | já estava no jogo; não relida | não verificada neste porte |

## Mudanças depois da regra de pares
Poça: deixou de viver no shader da via e virou disco aditivo por lâmpada, com `pool_gain` 0,234 pelo ACES dos dois jogos. Colunas a meia distância e poste aceso um sim, um não em rua com colunas. Barreiras com acabamento na instância. Valores por estilo em `styles.<id>`, dono `DataTables`; a tabela de fontes acima vale para `brasil`.
