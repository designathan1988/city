# core:fork

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/forkMap.ts`
- Unidades próprias (só esta funcionalidade alcança): 25 em 1 arquivo(s), 97 linhas
- Alcance total: 37 unidades; usa 0 compartilhadas de outras; é dona principal de 12 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Interage com: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/forkMap.spec.ts`, `tests/world/docFork.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `roads`: não se aplica 6

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/forkMap.ts
- 12-14 `ForkMap.GetAccessor` (method)
- 16-18 `ForkMap.size` (method)
- 20-25 `ForkMap.get` (method)
- 27-29 `ForkMap.has` (method)
- 32-34 `ForkMap.owns` (method): Whether the fork holds this key's value itself (written here), not the one below.
- 49-53 `ForkMap.set` (method)
- 55-61 `ForkMap.delete` (method)
- 64-68 `ForkMap.clear` (method): DO NOT call it on a working copy for an edit: it marks every key below
- 70-76 `ForkMap.entries` (method)
- 78-80 `ForkMap.keys` (method)
- 82-84 `ForkMap.values` (method)
- 86-88 `ForkMap.MethodDeclaration` (method)
- 90-92 `ForkMap.forEach` (method)
- 113-121 `ForkSet.constructor` (method)
- 123-125 `ForkSet.GetAccessor` (method)
- 127-129 `ForkSet.size` (method)
- 131-133 `ForkSet.has` (method)
- 135-138 `ForkSet.add` (method)
- 140-142 `ForkSet.delete` (method)
- 144-146 `ForkSet.clear` (method)
- 148-150 `ForkSet.keys` (method)
- 152-154 `ForkSet.values` (method)
- 156-158 `ForkSet.entries` (method)
- 160-162 `ForkSet.MethodDeclaration` (method)
- 164-166 `ForkSet.forEach` (method)
