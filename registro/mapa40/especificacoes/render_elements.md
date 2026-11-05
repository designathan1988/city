# render:elements

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/elements.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 124 unidades; usa 107 compartilhadas de outras; é dona principal de 17 compartilhadas

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
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `url(`${name}.gltf`)`, `url(json.buffers[0]!.uri)`
- Bibliotecas externas: `three`, `virtual:model-urls/elements?ext=gltf,bin,png`

## Testes do 4.0 que a exercitam
- `tests/render/elementCells.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `demolition`: não se aplica 1
- porte `landscape`: portado 29
- porte `traffic`: de outra funcionalidade 1

## Código próprio (linhas — declaração — comentário)
