# worker:networkBake

Grupo: worker. Entrada: worker. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/networkBake.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 28 linhas
- Alcance total: 1246 unidades; usa 1242 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#networkBakeOf@2090` (5)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:{ id: ask.id, bake: bakeNetwork(ask) } s`, `envia:{ id: ask.id, error: String((error as Er`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:doc` (5), `world:network` (5), `worker:bays` (4)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `RoadDoc.nodes`, `RoadDoc.segments`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/openingOffPage.spec.ts`

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/networkBake.worker.ts
- 12-19 `NetworkBakeAsk` (interface): A CITY'S NETWORK SOLVED OFF THE PAGE WHILE THE CURTAIN IS UP (`main.ts` openCityNow): the document opened here as the page opens it (`editor/history.ts` restoreInto), its network built whole and handed back as solved (`Network.exportBake`);
- 21-21 `NetworkBakeReply` (type)
- 24-30 `bakeNetwork` (function): The network of `ask`'s document, solved whole and exported (`Network.exportBake`).
- 32-43 `serve` (function)
