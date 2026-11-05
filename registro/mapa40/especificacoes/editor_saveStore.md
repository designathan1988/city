# editor:saveStore

Grupo: editor. Entrada: ferramenta. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/editor/saveStore.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 4 linhas
- Alcance total: 17 unidades; usa 1 compartilhadas de outras; é dona principal de 15 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`
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
- `tests/editor/binarySave.spec.ts`, `tests/editor/chunkedSave.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: não se aplica 44

## Código próprio (linhas — declaração — comentário)

### src/editor/saveStore.ts
- 26-29 `gzip` (function): gzip streams (MDN, `CompressionStream`): the texts of the saves before the binary frames (read, never written now).
