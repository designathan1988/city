# ui:labels

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/labels.ts`
- Unidades próprias (só esta funcionalidade alcança): 3 em 1 arquivo(s), 9 linhas
- Alcance total: 31 unidades; usa 25 compartilhadas de outras; é dona principal de 3 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#currentStatus@3837` (2), `src/main.ts#roadClasses@1558` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `world:road` (6), `ui:i18n` (5)
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
- porte `interface`: portado 12
- porte `interior`: outro porte 4

## Código próprio (linhas — declaração — comentário)

### src/ui/labels.ts
- 9-15 `roadTypeDescription` (function): The one-line description under a class name, with its lane count filled in.
- 19-19 `vehicleCountLabel` (function)
- 20-20 `peopleCountLabel` (function)
