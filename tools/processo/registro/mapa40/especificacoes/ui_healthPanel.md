# ui:healthPanel

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/healthPanel.ts`
- Unidades próprias (só esta funcionalidade alcança): 10 em 2 arquivo(s), 264 linhas
- Alcance total: 31 unidades; usa 10 compartilhadas de outras; é dona principal de 11 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `health.button`, `health.context`, `health.copy`, `health.earlier`, `health.empty`, `health.recent`, `health.status.…`, `health.systems`, `health.tab.…`, `health.title`
- Eventos de entrada: `ouve:click`, `ouve:keydown`, `ouve:resize`
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#gameKeys@2479` (2), `src/main.ts#healthPanel@3987` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 2
- Usa: `ui:i18n` (13), `ui:healthWatch` (1)
- É usada por: -

## Estado (campos do documento/estado da simulação)
- Lê: -
- Escreve: -

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): -
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/core/health.spec.ts`, `tests/ui/healthPanel.spec.ts`, `tests/ui/visibleText.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: portado 43

## Código próprio (linhas — declaração — comentário)

### src/core/health.ts
- 94-103 `HealthLog.status` (method): How the game is now: `broken` if something failed within `windowMs`, `slow` if a frame was long within it, else `ok`.

### src/ui/healthPanel.ts
- 6-12 `HealthPanelSources` (interface): The game's health in the interface: a button in the top bar whose dot is green, amber or red
- 14-14 `ROWS` (variable)
- 15-15 `Tab` (type)
- 16-16 `TABS` (variable)
- 18-46 `STYLE` (variable)
- 48-48 `ICON` (variable)
- 51-51 `CLOCK` (variable): One formatter for every row (MDN: repeated formatting with the same options should reuse an `Intl.DateTimeFormat`).
- 52-54 `clock` (function)
- 57-266 `mountHealthPanel` (function): The panel's keys, for the game's keys (`editor/gameKeys.ts`): F9 toggles it, Escape closes it (false: it was closed).
