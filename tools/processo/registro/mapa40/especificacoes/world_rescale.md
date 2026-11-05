# world:rescale

Grupo: world. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/world/rescale.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 95 unidades; usa 84 compartilhadas de outras; é dona principal de 11 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: `?.curve`, `?.dashOrigin`, `?.points`, `?.section`, `SerializedDoc.barriers`, `SerializedDoc.buildings`, `SerializedDoc.clouds`, `SerializedDoc.elements`, `SerializedDoc.fog`, `SerializedDoc.gullies`, `SerializedDoc.landscape`, `SerializedDoc.lots`, `SerializedDoc.nodes`, `SerializedDoc.paint`, `SerializedDoc.poles`, `SerializedDoc.segments`, `SerializedDoc.terrain`, `SerializedDoc.transit`, `SerializedDoc.treeClearings`, `SerializedDoc.trees`, `SerializedDoc.zoneMarks`, `SerializedDoc.zones`
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/world/rescale.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)
