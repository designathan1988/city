# worker:flatGround

Grupo: worker. Entrada: worker. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/flatGround.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 23 linhas
- Alcance total: 52 unidades; usa 51 compartilhadas de outras; é dona principal de 0 compartilhadas

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
- Usa: `render:plate` (9), `worker:bays` (3), `world:terrain` (3), `render:woods` (2)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: não se aplica 6

## Código próprio (linhas — declaração — comentário)

### src/render/flatGround.worker.ts
- 7-29 `serve` (function): This service of the job system (`job.worker.ts`): its messages answered on the scope it is given.
