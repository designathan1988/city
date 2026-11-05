# render:water

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/water.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 1 linhas
- Alcance total: 33 unidades; usa 5 compartilhadas de outras; é dona principal de 27 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `onBeforeCompile`, `onBeforeRender`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/water.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: portado 68

## Código próprio (linhas — declaração — comentário)

### src/render/water.ts
- 38-38 `WATER_DRIFTS` (variable): Both layers' drifts, exposed so the wrap invariant can be tested.
