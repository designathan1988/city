# render:instance

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/instanceBounds.ts`, `src/render/instanceCull.ts`, `src/render/instanceRuns.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 8 linhas
- Alcance total: 100 unidades; usa 69 compartilhadas de outras; é dona principal de 30 compartilhadas

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
- `tests/render/agentCulling.spec.ts`, `tests/render/gardenMatrix.spec.ts`, `tests/render/instanceRuns.spec.ts`, `tests/scale/census/derivedBound.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `landscape`: já existia 4
- porte `street-furniture`: já existia 3

## Código próprio (linhas — declaração — comentário)

### src/render/instanceCull.ts
- 8-15 `enclose` (function): The smallest sphere round two spheres.
