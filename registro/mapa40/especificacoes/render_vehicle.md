# render:vehicle

Grupo: render. Entrada: desenho. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/render/vehicleFrame.ts`, `src/render/vehicleLevels.ts`, `src/render/vehicleModels.ts`, `src/render/vehiclePanels.ts`, `src/render/vehicleWindows.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 17 linhas
- Alcance total: 144 unidades; usa 47 compartilhadas de outras; é dona principal de 95 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- nenhuma chamada direta da raiz: chega por outra funcionalidade

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: `onBeforeCompile`
- Timers: 0
- Usa: -
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: `meshoptimizer`, `three`, `three/addons/utils/BufferGeometryUtils.js`

## Testes do 4.0 que a exercitam
- `tests/render/interiorSolids.spec.ts`, `tests/render/occupantFit.spec.ts`, `tests/render/riderFit.spec.ts`, `tests/render/vehicleFrame.spec.ts`, `tests/render/vehicleLevels.spec.ts`, `tests/render/vehiclePanels.spec.ts`, `tests/render/windows.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `traffic`: já existia 4, portado 1, não portado 1
- porte `people`: já existia 2

## Código próprio (linhas — declaração — comentário)

### src/render/vehicleFrame.ts
- 2-6 `RoadFrame` (interface): A vehicle's one frame on the road: height, pitch and roll, from the road surface under its wheels.
- 20-31 `roadFrame` (function): The frame through the road under the axles and the sides.
