# people:body

Grupo: people. Entrada: subsistema. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/people/body/assets.ts`, `src/people/body/expressions.ts`, `src/people/body/macro.ts`, `src/people/body/morph.ts`, `src/people/body/proxy.ts`
- Unidades próprias (só esta funcionalidade alcança): 1 em 1 arquivo(s), 3 linhas
- Alcance total: 65 unidades; usa 1 compartilhadas de outras; é dona principal de 63 compartilhadas

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
- Recursos (JSON, modelos, texturas, fetch): `public/models/people/base.bin`, `public/models/people/base.json`, `public/models/people/faceunits.bin`, `public/models/people/faceunits.json`, `public/models/people/modifiers.json`, `public/models/people/skeleton-game-engine.json`, `public/models/people/targets-local.bin`, `public/models/people/targets-local.json`, `public/models/people/targets-macro-pca.bin`, `public/models/people/targets-macro-pca.json`, `public/models/people/weights-game-engine.bin`, `url`
- Bibliotecas externas: `virtual:model-urls/people/proxies?ext=json,bin,webp`

## Testes do 4.0 que a exercitam
- `tests/people/dressed.spec.ts`, `tests/people/hairStyles.spec.ts`, `tests/people/morph.spec.ts`, `tests/people/proxy.spec.ts`, `tests/people/rig.spec.ts`, `tests/people/spec.spec.ts`, `tests/render/classRigMade.spec.ts`, `tests/render/cookedPerson.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: já existia 111
- porte `traffic`: de outra funcionalidade 2

## Código próprio (linhas — declaração — comentário)

### src/people/body/morph.ts
- 97-99 `Morpher.components` (method): How many basis shapes the macro sliders fold into (`shape`).
