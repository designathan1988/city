# Fila do porte

Recomeço de 2026-11-04 12:00: tudo o que havia em `ports/` foi apagado por ordem do usuário. A coordenadora prepara o ambiente (rastreamento, clone, fotos) antes de abrir as sessões. As filhas são sessões do app; o app só abre sessão em outra pasta num turno iniciado por mensagem do usuário. Seis sessões ao mesmo tempo, duas funcionalidades cada, em sequência, cada funcionalidade no seu clone.

Ambiente: base a11384a; fotos do 4.0 (28) em `ports/referencia/reference`, do jogo atual (46) em `ports/referencia/antes`, plano em `registro/look/look-tudo.json`. Clones sem DLL: cada filha compila no começo, em segundo plano.

Estados: em porte, na fila da sessão, entregue, verificada, integrada.

| Sessão | Funcionalidade | Estado | Absorve |
|---|---|---|---|
| porte-1-vias | roads | em porte | `cityGen/streets.ts` |
| porte-1-vias | road-elevation | na fila da sessão | |
| porte-2-predios | build | em porte | `render/wear.ts` |
| porte-2-predios | marcos | na fila da sessão | `lots/*.json`; estádio a conferir: `stadium` não aparece em `src/` |
| porte-3-paisagem | landscape | em porte | `clouds`, `detailRecipes.ts` |
| porte-3-paisagem | economy | na fila da sessão | |
| porte-4-interface | interface | devolvida 13:05 (pesquisa, plano, cobertura, commits) | `start-menu`, `camera`, gráficos e qualidade |
| porte-4-interface | demolition | na fila da sessão | `bulldozer`, `destruction` |
| porte-5-transito | traffic | em porte | ciclistas |
| porte-5-transito | transit | na fila da sessão | |
| porte-6-rua-pessoas | street-furniture | em porte | `poles`, `barriers` |
| porte-6-rua-pessoas | people | na fila da sessão | |

## Cobertura medida
Fora `src/lab` e `src/peopleLab`: 728 arquivos, 195.006 linhas; 81,8 % tocadas por algum rastreamento; 92 arquivos (7.291 linhas) sem nenhum, em boa parte infraestrutura do motor web (perfilador, cache, WebGPU).

## Critério de aceite
Cada entrega passa por: diff, `cobertura.md` contra `feature.md`, `verify`, `audit`, `pack`, `sandbox`, fotos lidas contra o antes e o 4.0. Só depois entra no master.
