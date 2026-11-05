# ui:i18n

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/i18n/en.ts`, `src/ui/i18n/index.ts`, `src/ui/i18n/pt-BR.ts`
- Unidades próprias (só esta funcionalidade alcança): 4 em 1 arquivo(s), 26 linhas
- Alcance total: 24 unidades; usa 0 compartilhadas de outras; é dona principal de 20 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#ExpressionStatement@159` (1), `src/main.ts#ExpressionStatement@160` (1)

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
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- `tests/ui/builderCatalog.spec.ts`, `tests/ui/decimals.spec.ts`, `tests/ui/healthPanel.spec.ts`, `tests/ui/i18n.spec.ts`, `tests/ui/i18nKeys.spec.ts`, `tests/ui/money.spec.ts`, `tests/ui/oldInterface.spec.ts`, `tests/ui/plurals.spec.ts`, `tests/ui/roadKeys.spec.ts`, `tests/ui/roadProfileEditor.spec.ts`, `tests/ui/visibleText.spec.ts`

## Estado no jogo (lido de ports/*/cobertura.md)
- porte `interface`: portado 44, outra sessão 2
- porte `build`: não se aplica 2

## Código próprio (linhas — declaração — comentário)

### src/ui/i18n/index.ts
- 28-28 `isLanguage` (function)
- 30-45 `detect` (function)
- 68-70 `setGlobalParams` (function): Placeholders any sentence may use without its caller passing them: the road tool's rebindable keys
- 136-141 `initLanguage` (function): Resolves the initial language and applies it. Call once, at start-up.
