# editor:storeTurns

Grupo: editor. Entrada: ferramenta. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/storeTurns.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 20 linhas
- Alcance total: 7 unidades; usa 0 compartilhadas de outras; é dona principal de 4 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:null`
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- nenhuma cobertura de porte cita os arquivos desta funcionalidade

## Código próprio (linhas — declaração — comentário)

### src/editor/storeTurns.ts
- 4-10 `nextTask` (function): A task of its own (a message to itself, HTML's task queue): what came in meanwhile (a write's message, an IndexedDB event) runs first.
- 19-19 `StoreTurns.reads` (field): The last read's turn.
- 32-43 `StoreTurns.read` (method): A read run in its turn: after the reads before it, and when no write is under way.
