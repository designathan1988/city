# Feature: src/ui/v2/start.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 6 declarations
- Slices of shared code: 1
- Code: 80 lines in 2 files (source/)
- Resources: 28 (16 entry, 12 style, 0 import; 18 only the feature uses)
- Uses: 8 shared declarations (interface/: 9 declarations in 2 files, the types they name included); hooks: 1; external APIs: 24

## Code

- src/main.ts — part, 3 lines
- src/ui/v2/start.ts — whole, 77 lines

## Slices of shared code

- `src/main.ts#mountInterface` — 3 of 88 lines

## Resources

- shared entry `action.newMap` — src/ui/i18n/en.ts:90: `'action.newMap': 'New map'`
- shared entry `action.openMap` — src/ui/i18n/en.ts:94: `'action.openMap': 'Open map'`
- shared entry `action.about` — src/ui/i18n/en.ts:175: `'action.about': 'About'`
- shared entry `menu.graphicsOptions` — src/ui/i18n/en.ts:180: `'menu.graphicsOptions': 'Graphics options…'`
- shared entry `menu.language` — src/ui/i18n/en.ts:292: `'menu.language': 'Language'`
- feature entry `start.tagline` — src/ui/i18n/en.ts:1862: `'start.tagline': 'Build the streets, the city grows by itself.'`
- feature entry `start.continue` — src/ui/i18n/en.ts:1863: `'start.continue': 'Continue'`
- feature entry `start.play` — src/ui/i18n/en.ts:1864: `'start.play': 'Start building'`
- shared entry `action.newMap` — src/ui/i18n/pt-BR.ts:89: `'action.newMap': 'Novo mapa'`
- shared entry `action.openMap` — src/ui/i18n/pt-BR.ts:93: `'action.openMap': 'Abrir mapa'`
- shared entry `action.about` — src/ui/i18n/pt-BR.ts:174: `'action.about': 'Sobre'`
- shared entry `menu.graphicsOptions` — src/ui/i18n/pt-BR.ts:179: `'menu.graphicsOptions': 'Opções gráficas…'`
- shared entry `menu.language` — src/ui/i18n/pt-BR.ts:291: `'menu.language': 'Idioma'`
- feature entry `start.tagline` — src/ui/i18n/pt-BR.ts:1852: `'start.tagline': 'Construa as ruas, a cidade cresce sozinha.'`
- feature entry `start.continue` — src/ui/i18n/pt-BR.ts:1853: `'start.continue': 'Continuar'`
- feature entry `start.play` — src/ui/i18n/pt-BR.ts:1854: `'start.play': 'Começar a construir'`
- feature style `v2-start` — src/ui/v2/shell.css:393: `.v2-start { position: absolute; inset: 0; z-index: 30; display: grid; place-items: center; pointer-events: auto; background: radial-gradient(ellipse at 50% 40%,`
- feature style `v2-start-card` — src/ui/v2/shell.css:395: `.v2-start-card { display: grid; gap: 10px; width: min(380px, calc(100vw - 32px)); padding: 32px 28px 22px; border-radius: 18px; background: var(--v-bg); border:`
- feature style `v2-start-card` — src/ui/v2/shell.css:397: `.v2-start-card h1 { margin: 0; font-size: 40px; font-weight: 800; letter-spacing: 0.01em; }`
- feature style `v2-start-tag` — src/ui/v2/shell.css:398: `.v2-start-tag { margin: 0 0 14px; color: var(--v-muted); font-size: 14px; }`
- feature style `v2-start-b` — src/ui/v2/shell.css:399: `.v2-start-b { height: 46px; border: 1px solid var(--v-line); border-radius: 12px; background: var(--v-raise); font-size: 16px; font-weight: 600; }`
- feature style `v2-start-b` — src/ui/v2/shell.css:400: `.v2-start-b:hover { background: var(--v-raise-2); border-color: var(--v-accent-line); }`
- feature style `v2-start-b` — src/ui/v2/shell.css:401: `.v2-start-b.primary { background: var(--v-accent); color: var(--v-accent-ink); border-color: var(--v-accent); font-size: 17px; }`
- feature style `v2-start-b` — src/ui/v2/shell.css:402: `.v2-start-b.primary:hover { filter: brightness(1.08); }`
- feature style `v2-start-langs` — src/ui/v2/shell.css:403: `.v2-start-langs { display: flex; justify-content: center; gap: 6px; margin-top: 8px; }`
- feature style `v2-start-lang` — src/ui/v2/shell.css:404: `.v2-start-lang { padding: 6px 12px; border: 1px solid var(--v-line); border-radius: 999px; background: none; color: var(--v-muted); }`
- feature style `v2-start-lang` — src/ui/v2/shell.css:405: `.v2-start-lang.on { color: var(--v-accent-ink); background: var(--v-accent); border-color: var(--v-accent); }`
- feature style `v2-start-about` — src/ui/v2/shell.css:406: `.v2-start-about { justify-self: center; margin-top: 4px; border: 0; background: none; color: var(--v-muted); font-size: 13px; text-decoration: underline; }`

## Where the program plugs the feature in

### src/ui/v2/start.ts

- `src/ui/v2/start.ts#mountStart` (function, line 15) ← call from `src/main.ts#mountInterface`

## Shared declarations the feature uses

### src/main.ts

- `src/main.ts#mountInterface` (function, line 1859) — implementation by 1: `src/ui/v2/start.ts#mountStart.fill`
- `src/main.ts#newMap` (function, line 2015) — implementation by 1: `src/ui/v2/start.ts#mountStart.fill`

### src/ui/i18n/index.ts

- `src/ui/i18n/index.ts#language` (function, line 50) — call by 1: `src/ui/v2/start.ts#mountStart.fill`
- `src/ui/i18n/index.ts#LANGUAGES` (variable, line 14) — read by 1: `src/ui/v2/start.ts#mountStart.fill`
- `src/ui/i18n/index.ts#LanguageSpec` (interface, line 8) — read by 1: `src/ui/v2/start.ts#mountStart.fill`
- `src/ui/i18n/index.ts#onLanguageChange` (function, line 108) — call by 1: `src/ui/v2/start.ts#mountStart`
- `src/ui/i18n/index.ts#setLanguage` (function, line 95) — call by 1: `src/ui/v2/start.ts#mountStart.fill`
- `src/ui/i18n/index.ts#t` (function, line 55) — call by 1: `src/ui/v2/start.ts#mountStart.fill`

## External APIs

- `dom:document` × 8
- `dom:Document.createElement` × 8
- `dom:Element.className` × 7
- `dom:Node.appendChild` × 6
- `dom:Element.setAttribute` × 5
- `dom:Element.textContent` × 5
- `dom:GlobalEventHandlers.onclick` × 3
- `dom:HTMLButtonElement.type` × 3
- `dom:KeyboardEvent.key` × 2
- `dom:window` × 2
- `dom:ChildNode.remove` × 1
- `dom:Event.preventDefault` × 1
- `dom:Event.stopPropagation` × 1
- `dom:HTMLButtonElement` × 1
- `dom:HTMLElement` × 1
- `dom:HTMLOrSVGElement.focus` × 1
- `dom:KeyboardEvent` × 1
- `dom:Node.isConnected` × 1
- `dom:ParentNode.append` × 1
- `dom:ParentNode.querySelector` × 1
- `dom:ParentNode.replaceChildren` × 1
- `dom:Window.addEventListener` × 1
- `dom:Window.removeEventListener` × 1
- `es:String` × 1
