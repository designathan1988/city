# Plano: o que pende na fachada e fica na sacada

## O que o 4.0 faz (lido em `facadeProps.ts`, `facadePropModels.ts`, `kit.ts`, `buildingMesh.ts:1828`)

Dez peças: condensador, antena, vaso, folhagem, arbusto, cadeira, mesa, varal, espreguiçadeira, bicicleta.

- Só do andar 1 para cima e só nos componentes mobiliados (`window`, `sashWindow`, `shutteredWindow`, `ribbon`, `balcony`, `gourmet`, `loggia`).
- Sorteio por vão: um fluxo mulberry32 semeado pelo hash do vão, então o mesmo vão dá sempre a mesma coisa.
- Janela: condensador sob o peitoril (7 % residencial, 6 % misto, 4 % comercial, 2 % industrial) ou antena ao lado (5 %, nunca industrial).
- Sacada e gourmet: padrão pelo uso, pelo componente e pela idade (`office`, `high`, `popular`, `middle`). 45 % nua, 25 % uma coisa, 15 % duas ou três, 10 % a coisa do padrão, 5 % jardim de 3 ou 4 vasos.
- Loggia: 15 % planta, 20 % cadeira, 7 % espreguiçadeira, nunca em comercial ou industrial.
- O vaso sempre vem com folhagem ou arbusto, escala sorteada (30 a 55 cm).
- A cor vem da instância e o tom de cada parte da peça vem do modelo. Sombra só em condensador, vaso, folhagem e arbusto.

## O que o jogo tem

`facade_props.cpp`: condensador, antena em caixa, vaso e planta em caixa de uma cor só, cadeira de duas caixas. Sem varal, mesa, espreguiçadeira, bicicleta, loggia, padrão ou idade. É a causa dos vasos cúbicos grandes do par.

## Solução

Portar a funcionalidade na arquitetura atual, sem copiar o código:

1. `sim` não muda. O sorteio é render: `render/buildings/facade_props.cpp` passa a decidir como no 4.0 e devolve `PropInstance { kind, frame, scale, turn, colour }`.
2. Peças: `render/buildings/prop_models.{h,cpp}` gera uma malha por tipo, uma vez, com o tom de cada parte na cor do vértice. Primitivas próprias (caixa, cilindro, disco, esfera facetada, anel, fita de folha).
3. Desenho: um `MultiMesh` por tipo e por chunk no nível perto, como as faixas de peças, com o layout sem cor (`INSTANCE_CUSTOM` = cor e acabamento). Sombra só nos quatro tipos do 4.0, com instância separada desses tipos.
4. Shader: `building_props.gdshader` igual ao `building_parts`, multiplicando a cor da instância pelo `COLOR` do vértice. A frente do condensador (ventoinha, grade, aletas) desenhada no fragmento, como no 4.0, em vez de triângulos.
5. Dados em `project/data/render/buildings.json` `facade_props`: paletas, chances e medidas, schema estrito, estilo em `styles.<id>` quando variar por região. Medidas são escolha de modelagem, dita no `_doc`; as chances do 4.0 não têm fonte primária, e o `_doc` diz isso.
6. Fim do que substitui: `part_ac`, `pot`, `chair` e a antena em caixa saem. Sem dois caminhos.
7. Sonda: `building_cost` ganha `props` (instâncias e bytes por tipo).

## Prova

Gates, DLLs de release e debug, 13 testes. Teste novo no `building_edit_test` ou próprio: mesma semente dá as mesmas peças, nenhuma no térreo, nenhuma em comercial na loggia.

Fotos, lista escrita antes de tirar: sacada residencial de perto (vaso, planta, cadeira), varal, mesa, espreguiçadeira, bicicleta, condensador, antena; par com o 4.0 no mesmo prédio, 16h, mesma pose. Comparar `building_cost` e `camera_pan perto` com a ponta.
