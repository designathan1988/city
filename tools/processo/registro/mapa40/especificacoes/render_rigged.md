# render:rigged

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/riggedCitizens.ts`
- Unidades próprias (só esta funcionalidade alcança): 0 em 0 arquivo(s), 0 linhas
- Alcance total: 709 unidades; usa 676 compartilhadas de outras; é dona principal de 33 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: `le:roadcraft.peopleStyle`

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `customDepthMaterial`, `onAfterRender`, `onAfterShadow`, `onBeforeCompile`, `onBeforeShadow`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `/__cook/people/…`
- Bibliotecas externas: `three`, `three/addons/loaders/GLTFLoader.js`

## Testes do 4.0 que a exercitam
- `tests/render/agentPrograms.spec.ts`, `tests/render/castingMemory.spec.ts`, `tests/render/citizenLod.spec.ts`, `tests/render/citizenShadow.spec.ts`, `tests/render/peopleMemory.spec.ts`, `tests/render/personTextureUnits.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `people`: não se aplica 71
- porte `traffic`: de outra funcionalidade 1

## Código próprio (linhas — declaração — comentário)
