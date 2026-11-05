# render:rain

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/rain.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 8 linhas
- Alcance total: 12 unidades; usa 3 compartilhadas de outras; é dona principal de 8 compartilhadas

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
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `three`

## Testes do 4.0 que a exercitam
- `tests/render/rainZoom.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: portado 26

## Código próprio (linhas — declaração — comentário)

### src/render/rain.ts
- 72-79 `dropAt` (function): A drop's place - `seed` its hash in [0, 1) on each axis, `tip` 0 at its head, 1 at its tail
