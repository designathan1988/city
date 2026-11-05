# core:recent

Grupo: core. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/core/recentCache.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 6 linhas
- Alcance total: 6 unidades; usa 0 compartilhadas de outras; é dona principal de 4 compartilhadas

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
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `road-elevation`: a portar 6
- porte `roads`: não se aplica 8

## Código próprio (arquivo:linhas — declaração — comentário)

### src/core/recentCache.ts
- 27-29 `RecentCache.size` (method): How many values are held.
- 31-33 `RecentCache.clear` (method)
