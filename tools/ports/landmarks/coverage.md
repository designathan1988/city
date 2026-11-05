# Cobertura do porte marcos

| Do 4.0 | O que faz para o jogador | No jogo | Estado |
|---|---|---|---|
| `src/world/buildings/lotLibrary.ts` `LOT_NAMES`, `lotKey` | lista os lotes | `data/ui/builder.json` `lots` | portado |
| `lotLibrary.ts` `loadLot`, `loadedLot` | lê um lote | `DataTables.lot`, `ModelLibrary.lots` | portado |
| `lots/*.json` (38) | os projetos | `project/data/lots/*.json`, schema `lot.schema.json`, em metros | portado |
| `shell.ts` `renderModels`, `renderBuild` | cartões na aba Lotes | `ui/model_shelf.gd`, grupo `lots` | portado; cartões não vistos em foto |
| `buildingsWiring.ts` `choosePreset` | lote na mão | `_choose` em `model_shelf.gd` | portado |
| `parts.ts` `partSample`, `request` | foto do cartão | `ModelStudio` com os bytes do lote | portado; não visto em foto |
| 38 entradas `builder.lot.*` | nomes | `strings_e.csv` | portado |
| `render/buildings/signature.ts`, `landmarks.ts`, `facadeKit.ts` | o desenho próprio de cada lote | fora do rastreamento original | **não portado** |
| estádio | pendência antiga | não existe no 4.0 (`src/`, `lots/`) | pendência removida |
