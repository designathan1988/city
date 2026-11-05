# worker:ecology

Grupo: worker. Entrada: worker. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/ecology.worker.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 30 linhas
- Alcance total: 128 unidades; usa 127 compartilhadas de outras; é dona principal de 0 compartilhadas

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
- Usa: `world:ecology` (14), `render:far` (9), `render:ecology` (7), `worker:bays` (3), `render:buildings` (2)
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
- porte `landscape`: já existia 4

## Código próprio (linhas — declaração — comentário)

### src/render/ecology.worker.ts
- 8-37 `serve` (function): This service of the job system (`job.worker.ts`): its messages answered on the scope it is given.
