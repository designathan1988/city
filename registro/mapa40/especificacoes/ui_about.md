# ui:about

Grupo: interface. Entrada: painel. Gerado de `registro/mapa40/grafo.json` (commit do 4.0 22357c928fb9) por `ferramentas/mapa40/funcionalidades.cjs`: cada declaração própria abaixo tem arquivo e linhas no 4.0.

- Sementes: `src/ui/about.ts`
- Unidades próprias (só esta funcionalidade alcança): 2 em 1 arquivo(s), 18 linhas
- Alcance total: 20 unidades; usa 18 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): -
- Eventos de entrada: -
- Chaves de localStorage: -

## Onde o programa a liga (raiz: main.ts e wiring)
- `src/main.ts#mountInterface@1859` (2), `src/main.ts#ExpressionStatement@1986` (1)

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): -
- Workers: -
- Ganchos do three.js: -
- Timers: 0
- Usa: `ui:buildStamp` (4), `ui:i18n` (1)
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
- porte `interface`: portado 6

## Código próprio (linhas — declaração — comentário)

### src/ui/about.ts
- 5-14 `mountAbout` (function): The About dialog (`index.html`): which build this is, and the credits.
- 17-24 `openAbout` (function): Opens the About dialog (the menu's About).
