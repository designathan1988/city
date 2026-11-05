# Feature: src/world/buildings/lotLibrary.ts

The program starts from `src/main.ts`, `src/peopleLab/main.ts`. A declaration is the feature's own when the program reaches it only through the feature; in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).

- Own: 7 declarations
- Slices of shared code: 5
- Code: 77 lines in 4 files (source/)
- Resources: 76 (76 entry, 0 style, 0 import; 76 only the feature uses)
- Uses: 1 shared declarations (interface/: 217 declarations in 58 files, the types they name included); hooks: 5; external APIs: 13

## Code

- src/buildingsWiring.ts — part, 13 lines
- src/render/buildings/parts.ts — part, 21 lines
- src/ui/v2/shell.ts — part, 11 lines
- src/world/buildings/lotLibrary.ts — whole, 32 lines

## Slices of shared code

- `src/buildingsWiring.ts#createBuildingWiring` — 13 of 1136 lines
- `src/render/buildings/parts.ts#createThumbnailStudio.request` — 15 of 23 lines
- `src/render/buildings/parts.ts#partSample` — 6 of 57 lines
- `src/ui/v2/shell.ts#mountShell.renderBuild` — 5 of 63 lines
- `src/ui/v2/shell.ts#mountShell.renderModels` — 6 of 45 lines

## Resources

- feature entry `builder.lot.balconyMid` — src/ui/i18n/en.ts:1270: `'builder.lot.balconyMid': 'Balconies and timber'`
- feature entry `builder.lot.glassOffice` — src/ui/i18n/en.ts:1271: `'builder.lot.glassOffice': 'Glass office'`
- feature entry `builder.lot.glassBalcony` — src/ui/i18n/en.ts:1272: `'builder.lot.glassBalcony': 'Glass with balconies'`
- feature entry `builder.lot.beigeClassic` — src/ui/i18n/en.ts:1273: `'builder.lot.beigeClassic': 'Classic beige'`
- feature entry `builder.lot.darkGlass` — src/ui/i18n/en.ts:1274: `'builder.lot.darkGlass': 'Dark glass'`
- feature entry `builder.lot.whiteBalcony` — src/ui/i18n/en.ts:1275: `'builder.lot.whiteBalcony': 'White with balconies'`
- feature entry `builder.lot.brickFrame` — src/ui/i18n/en.ts:1276: `'builder.lot.brickFrame': 'Brick and glass'`
- feature entry `builder.lot.roundGlass` — src/ui/i18n/en.ts:1277: `'builder.lot.roundGlass': 'Round glass'`
- feature entry `builder.lot.darkGrid` — src/ui/i18n/en.ts:1278: `'builder.lot.darkGrid': 'Dark grid'`
- feature entry `builder.lot.artDeco` — src/ui/i18n/en.ts:1279: `'builder.lot.artDeco': 'Art deco'`
- feature entry `builder.lot.twistGreen` — src/ui/i18n/en.ts:1280: `'builder.lot.twistGreen': 'Twisted teal'`
- feature entry `builder.lot.hexTerracotta` — src/ui/i18n/en.ts:1281: `'builder.lot.hexTerracotta': 'Hexagonal terracotta'`
- feature entry `builder.lot.stepGarden` — src/ui/i18n/en.ts:1282: `'builder.lot.stepGarden': 'Terrace gardens'`
- feature entry `builder.lot.waveWhite` — src/ui/i18n/en.ts:1283: `'builder.lot.waveWhite': 'Wave balconies'`
- feature entry `builder.lot.cubeBalcony` — src/ui/i18n/en.ts:1284: `'builder.lot.cubeBalcony': 'Cube balconies'`
- feature entry `builder.lot.twinNavy` — src/ui/i18n/en.ts:1285: `'builder.lot.twinNavy': 'Navy twins'`
- feature entry `builder.lot.octCopper` — src/ui/i18n/en.ts:1286: `'builder.lot.octCopper': 'Octagonal copper'`
- feature entry `builder.lot.stackedBlocks` — src/ui/i18n/en.ts:1287: `'builder.lot.stackedBlocks': 'Stacked blocks'`
- feature entry `builder.lot.pinkCorner` — src/ui/i18n/en.ts:1288: `'builder.lot.pinkCorner': 'Pink corner'`
- feature entry `builder.lot.triangleDark` — src/ui/i18n/en.ts:1289: `'builder.lot.triangleDark': 'Dark triangle'`
- feature entry `builder.lot.cityHallBrick` — src/ui/i18n/en.ts:1290: `'builder.lot.cityHallBrick': 'Brick town hall'`
- feature entry `builder.lot.decoSpire` — src/ui/i18n/en.ts:1291: `'builder.lot.decoSpire': 'Deco spire'`
- feature entry `builder.lot.hospitalA` — src/ui/i18n/en.ts:1292: `'builder.lot.hospitalA': 'Hospital'`
- feature entry `builder.lot.loftBrick` — src/ui/i18n/en.ts:1293: `'builder.lot.loftBrick': 'Brick lofts'`
- feature entry `builder.lot.cityHallWhite` — src/ui/i18n/en.ts:1294: `'builder.lot.cityHallWhite': 'White town hall'`
- feature entry `builder.lot.forum` — src/ui/i18n/en.ts:1295: `'builder.lot.forum': 'Courthouse'`
- feature entry `builder.lot.church` — src/ui/i18n/en.ts:1296: `'builder.lot.church': 'Church'`
- feature entry `builder.lot.bank` — src/ui/i18n/en.ts:1297: `'builder.lot.bank': 'Bank'`
- feature entry `builder.lot.school` — src/ui/i18n/en.ts:1298: `'builder.lot.school': 'School'`
- feature entry `builder.lot.fireStation` — src/ui/i18n/en.ts:1299: `'builder.lot.fireStation': 'Fire station'`
- feature entry `builder.lot.police` — src/ui/i18n/en.ts:1300: `'builder.lot.police': 'Police station'`
- feature entry `builder.lot.hospitalB` — src/ui/i18n/en.ts:1301: `'builder.lot.hospitalB': 'Hospital with a wing'`
- feature entry `builder.lot.cemetery` — src/ui/i18n/en.ts:1302: `'builder.lot.cemetery': 'Cemetery'`
- feature entry `builder.lot.beauxArts` — src/ui/i18n/en.ts:1303: `'builder.lot.beauxArts': 'Beaux-Arts hotel'`
- feature entry `builder.lot.decoTower` — src/ui/i18n/en.ts:1304: `'builder.lot.decoTower': 'Art Deco tower'`
- feature entry `builder.lot.modernOffice` — src/ui/i18n/en.ts:1305: `'builder.lot.modernOffice': 'Modernist office'`
- feature entry `builder.lot.residentialTerrace` — src/ui/i18n/en.ts:1306: `'builder.lot.residentialTerrace': 'Terraced flats'`
- feature entry `builder.lot.glassSpire` — src/ui/i18n/en.ts:1307: `'builder.lot.glassSpire': 'Blue glass tower'`
- feature entry `builder.lot.balconyMid` — src/ui/i18n/pt-BR.ts:1260: `'builder.lot.balconyMid': 'Varandas e madeira'`
- feature entry `builder.lot.glassOffice` — src/ui/i18n/pt-BR.ts:1261: `'builder.lot.glassOffice': 'Escritório de vidro'`
- feature entry `builder.lot.glassBalcony` — src/ui/i18n/pt-BR.ts:1262: `'builder.lot.glassBalcony': 'Vidro com varandas'`
- feature entry `builder.lot.beigeClassic` — src/ui/i18n/pt-BR.ts:1263: `'builder.lot.beigeClassic': 'Bege clássica'`
- feature entry `builder.lot.darkGlass` — src/ui/i18n/pt-BR.ts:1264: `'builder.lot.darkGlass': 'Vidro escuro'`
- feature entry `builder.lot.whiteBalcony` — src/ui/i18n/pt-BR.ts:1265: `'builder.lot.whiteBalcony': 'Branca com varandas'`
- feature entry `builder.lot.brickFrame` — src/ui/i18n/pt-BR.ts:1266: `'builder.lot.brickFrame': 'Tijolo e vidro'`
- feature entry `builder.lot.roundGlass` — src/ui/i18n/pt-BR.ts:1267: `'builder.lot.roundGlass': 'Redonda de vidro'`
- feature entry `builder.lot.darkGrid` — src/ui/i18n/pt-BR.ts:1268: `'builder.lot.darkGrid': 'Grade escura'`
- feature entry `builder.lot.artDeco` — src/ui/i18n/pt-BR.ts:1269: `'builder.lot.artDeco': 'Art déco'`
- feature entry `builder.lot.twistGreen` — src/ui/i18n/pt-BR.ts:1270: `'builder.lot.twistGreen': 'Torcida verde-água'`
- feature entry `builder.lot.hexTerracotta` — src/ui/i18n/pt-BR.ts:1271: `'builder.lot.hexTerracotta': 'Hexagonal de terracota'`
- feature entry `builder.lot.stepGarden` — src/ui/i18n/pt-BR.ts:1272: `'builder.lot.stepGarden': 'Terraços-jardim'`
- feature entry `builder.lot.waveWhite` — src/ui/i18n/pt-BR.ts:1273: `'builder.lot.waveWhite': 'Varandas em onda'`
- feature entry `builder.lot.cubeBalcony` — src/ui/i18n/pt-BR.ts:1274: `'builder.lot.cubeBalcony': 'Varandas em cubo'`
- feature entry `builder.lot.twinNavy` — src/ui/i18n/pt-BR.ts:1275: `'builder.lot.twinNavy': 'Gêmeas azul-marinho'`
- feature entry `builder.lot.octCopper` — src/ui/i18n/pt-BR.ts:1276: `'builder.lot.octCopper': 'Octogonal de cobre'`
- feature entry `builder.lot.stackedBlocks` — src/ui/i18n/pt-BR.ts:1277: `'builder.lot.stackedBlocks': 'Blocos empilhados'`
- feature entry `builder.lot.pinkCorner` — src/ui/i18n/pt-BR.ts:1278: `'builder.lot.pinkCorner': 'Esquina rosa'`
- feature entry `builder.lot.triangleDark` — src/ui/i18n/pt-BR.ts:1279: `'builder.lot.triangleDark': 'Triangular escura'`
- feature entry `builder.lot.cityHallBrick` — src/ui/i18n/pt-BR.ts:1280: `'builder.lot.cityHallBrick': 'Prefeitura de tijolo'`
- feature entry `builder.lot.decoSpire` — src/ui/i18n/pt-BR.ts:1281: `'builder.lot.decoSpire': 'Art déco com pináculo'`
- feature entry `builder.lot.hospitalA` — src/ui/i18n/pt-BR.ts:1282: `'builder.lot.hospitalA': 'Hospital'`
- feature entry `builder.lot.loftBrick` — src/ui/i18n/pt-BR.ts:1283: `'builder.lot.loftBrick': 'Lofts de tijolo'`
- feature entry `builder.lot.cityHallWhite` — src/ui/i18n/pt-BR.ts:1284: `'builder.lot.cityHallWhite': 'Prefeitura branca'`
- feature entry `builder.lot.forum` — src/ui/i18n/pt-BR.ts:1285: `'builder.lot.forum': 'Fórum'`
- feature entry `builder.lot.church` — src/ui/i18n/pt-BR.ts:1286: `'builder.lot.church': 'Igreja'`
- feature entry `builder.lot.bank` — src/ui/i18n/pt-BR.ts:1287: `'builder.lot.bank': 'Banco'`
- feature entry `builder.lot.school` — src/ui/i18n/pt-BR.ts:1288: `'builder.lot.school': 'Escola'`
- feature entry `builder.lot.fireStation` — src/ui/i18n/pt-BR.ts:1289: `'builder.lot.fireStation': 'Bombeiros'`
- feature entry `builder.lot.police` — src/ui/i18n/pt-BR.ts:1290: `'builder.lot.police': 'Delegacia'`
- feature entry `builder.lot.hospitalB` — src/ui/i18n/pt-BR.ts:1291: `'builder.lot.hospitalB': 'Hospital com ala'`
- feature entry `builder.lot.cemetery` — src/ui/i18n/pt-BR.ts:1292: `'builder.lot.cemetery': 'Cemitério'`
- feature entry `builder.lot.beauxArts` — src/ui/i18n/pt-BR.ts:1293: `'builder.lot.beauxArts': 'Hotel Beaux-Arts'`
- feature entry `builder.lot.decoTower` — src/ui/i18n/pt-BR.ts:1294: `'builder.lot.decoTower': 'Torre art déco'`
- feature entry `builder.lot.modernOffice` — src/ui/i18n/pt-BR.ts:1295: `'builder.lot.modernOffice': 'Escritório modernista'`
- feature entry `builder.lot.residentialTerrace` — src/ui/i18n/pt-BR.ts:1296: `'builder.lot.residentialTerrace': 'Residencial com terraços'`
- feature entry `builder.lot.glassSpire` — src/ui/i18n/pt-BR.ts:1297: `'builder.lot.glassSpire': 'Torre de vidro azul'`

## Where the program plugs the feature in

### src/world/buildings/lotLibrary.ts

- `src/world/buildings/lotLibrary.ts#loadedLot` (function, line 30) ← call from `src/render/buildings/parts.ts#partSample`
- `src/world/buildings/lotLibrary.ts#loadLot` (function, line 18) ← call from `src/buildingsWiring.ts#createBuildingWiring`, `src/render/buildings/parts.ts#createThumbnailStudio.request`
- `src/world/buildings/lotLibrary.ts#LOT_NAMES` (variable, line 9) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`, `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/lotLibrary.ts#lotKey` (function, line 12) ← call from `src/ui/v2/shell.ts#mountShell.renderModels`
- `src/world/buildings/lotLibrary.ts#lotKey` (function, line 12) ← read from `src/ui/v2/shell.ts#mountShell.renderBuild`

## Shared declarations the feature uses

### src/world/buildings/lotTemplate.ts

- `src/world/buildings/lotTemplate.ts#LotTemplate` (interface, line 5) — typeRef by 4: `src/world/buildings/lotLibrary.ts#loaded`, `src/world/buildings/lotLibrary.ts#loadedLot`, `src/world/buildings/lotLibrary.ts#loaders`, `src/world/buildings/lotLibrary.ts#loadLot`

## External APIs

- `es:Map.get` × 2
- `es:Object` × 2
- `es:ObjectConstructor.keys` × 2
- `es:Array.find` × 1
- `es:Array.map` × 1
- `es:Array.sort` × 1
- `es:Map` × 1
- `es:Map.set` × 1
- `es:Promise` × 1
- `es:String.lastIndexOf` × 1
- `es:String.length` × 1
- `es:String.slice` × 1
- `vite:ImportMeta.glob` × 1
