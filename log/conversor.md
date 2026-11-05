# Registro do conversor (tools/converter)

Para outra sessão continuar o trabalho do conversor e tradutor do Roadcraft 4.0 para o jogo.

**Regra do usuário (2026-11-05): a cada tarefa feita e a cada informação nova, atualizar este arquivo.** O "Histórico"
recebe uma linha por acontecimento (data, hora, o quê, commit); "Estado atual" e "A fazer" são reescritos para ficar
sempre verdadeiros.

## Estado atual (2026-11-05 15:55, sessão encerrada)

- `tools/converter` (master `9499eac`) e `tools/converter-slice` (slice-envelope `564a35d`): limpos, tudo commitado.
- Tradutor com runtime C++ dos built-ins do ECMAScript (regra única `js::<nome>`). A suíte inteira passa (105 testes;
  o cppgraph pede `CONVERTER_CLANGD=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64/bin/clangd.exe`), e os
  6.449 casos do runtime dão o mesmo que o Node.
- Referência nova (`Documents\Projects\city\roadcraft`) com grafo e Jelly prontos em `out/city40`; as features e as
  conversões ainda não foram refeitas nela.
- Últimos números (cópia antiga, mesmos insumos): road-elevation 30 traduzidas de 1.028 unidades; lab 32 de 240;
  passadas por área da sessão parceira com `5673af1`: sim 282/3.325, core 95/4.586, lab 66/1.111, people 8, ui 2.

## Onde está cada coisa

- Conversor: `C:\Users\Jonathan\Desktop\city\tools\converter`, git próprio, branch `master`. Commits só ali; o conteúdo
  vai para o repositório do jogo por `git read-tree` (sessão dona do jogo).
- Worktree `C:\Users\Jonathan\Desktop\city\tools\converter-slice` (branch `slice-envelope`): manter. Cada commit do
  master é aplicado lá por `git show <commit> --format= | git apply` e commitado como "sync from master".
- Referência do Roadcraft 4.0, desde 2026-11-05: `C:\Users\Jonathan\Documents\Projects\city\roadcraft` (último commit
  2921c465, 29/10). A antiga `Documents\Projects\roadcraft-4.0-main` (23/10) não vale mais.
- Grafo e análise Jelly da referência nova: `tools/converter/out/city40/program.json` (863 arquivos, 20.318 unidades)
  e `tools/converter/out/city40/whole/jelly` (callgraph.json, tokens.json; entrada src/main.ts). Os antigos
  `out/rc40/program.json` e `out/jelly` são da referência antiga.
- Configuração da máquina: `tools/converter/configs/roadcraft-machine.json`. Já aponta para a referência nova, para
  `tools/ports` e para `tools/process/records/map40/features.json`. Depois de mudar, rode `node scripts/port.ts doctor`.
- Cópia instrumentada do 4.0 para o oráculo: `tools/oracle-4.0` (o vite.config é nosso; o jogo original nunca é escrito).
- Sessão parceira: "Roadcraft 4.0 migration analysis" (`uds:\\.\pipe\LOCAL\cc-msg-056396bf66f931dac736851d7e135edc`).
  Ela faz os portes, as libraries (`tools/ports/<porte>/conversion-library.json`) e as passadas por área. O conversor é
  deste lado.

## Comandos

- Typecheck: `node scripts/typecheck.ts --typescript ../oracle-4.0/node_modules/typescript --types ../oracle-4.0/node_modules/@types`
- Testes (precisam das variáveis):
  `CONVERTER_TYPESCRIPT=../oracle-4.0/node_modules/typescript CONVERTER_CXX=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64/bin/x86_64-w64-mingw32-clang++.exe node --test test/translate.test.ts test/js-runtime.test.ts test/convert.test.ts test/convert-shared.test.ts test/godot-shader.test.ts test/platform.test.ts`
  (48 testes; o de translate compila o C++ gerado e roda o diferencial contra a fonte).
- Grafo de uma referência: `node --max-old-space-size=12288 src/cli.ts graph --source-root <ref> --typescript <ref>/node_modules/typescript --out <arquivo>`
- Feature (fatia e Jelly): `src/cli.ts feature --source-root <ref> --graph <program.json> --analysis <pasta jelly> --entry <src/...> --out out/features/<nome>`
  (`--jelly C:/Projects/toolchains/jelly/node_modules/@cs-au-dk/jelly` em vez de `--analysis` para refazer a análise).
- Convert: `src/cli.ts convert --feature <f> --source-root <ref> --typescript <ref>/node_modules/typescript --graph <program.json> --boundary out/features/<f>/boundary.json --target-root C:/Users/Jonathan/Desktop/city --target-graph <cppgraph.json> --library <conversion-library.json> --ports C:/Users/Jonathan/Desktop/city/tools/ports --cxx <clang++> --out <pasta>`
  Opções: `--coverage-proof shown` (só vale "in the game" que o jogo mostra), `--constants <json>`.
- Censo dos built-ins: `node scripts/builtins-census.ts --source-root <ref> --typescript <ref>/node_modules/typescript --out out/builtins-census.json`

## Feito (commits no master)

- `a84852d`: parâmetros mutáveis, slice, instâncias genéricas, padrões com contexto; convert 8x mais rápido.
- `3b52e46`: área `lab` (native/src/api/lab), nomes de tipo do jogo reservados (Profile → ElevationProfile),
  interface só com `get` lida como mapa.
- `626f11f`: arquivos dos portes pelos nomes em inglês (coverage.md, constants.json), com os portugueses como alternativa.
- `cf91795`: has() como predicado, tuplas por posição, cópia do spread, includes vindos do texto, literais (separador,
  hex, int64, NaN), Math.imul, `**`, bitwise com ToInt32, reduce, startsWith/endsWith/includes, replace/replaceAll,
  regex .test, ArrayLike, throw por padrão `new Error($m)` da library.
- `1695312`: config com os caminhos em inglês.
- `117f6d7`: includes de cada módulo pelo próprio texto; tipos da lib do TS anônimos; módulo compartilhado traduzido
  como união (o convert traduz de novo com o que os anteriores escreveram).
- `5673af1`: `--coverage-proof shown`. No poles, 268 das 275 alegações de coverage não aparecem no jogo.
- `c8c2f5c`: a library declara tipos do jogo (`types`) e funções donas com `include`/`context`; pattern `type: owned`;
  `scripts/builtins-census.ts` (4.0 antigo: 313 formas do núcleo ECMAScript com 50.578 usos, 511 do navegador com 7.333).

## Runtime dos built-ins (commit `aab7600` no master, `5acf06c` no slice)

A causa de tudo ir caso a caso é o tradutor ter uma regra escrita à mão para cada built-in do JavaScript.
A correção é um runtime C++ dos built-ins, com uma regra só no tradutor:
- `cpp/js/js_runtime.h`: Number::toString, toFixed (exato), Math (round, sign, fround, imul, clz32), ToInt32,
  strings em unidades UTF-16 (slice, substring, indexOf, lastIndexOf, trim, padStart/End, charCodeAt, split,
  fromCharCode), arrays (join, splice, pop, shift, unshift, indexOf, includes, findIndex, concat, flat, sort sem e com
  comparador) e `js::Map`/`js::Set` com a ordem de inserção do JavaScript (o `std::unordered_map` trocava a ordem
  das 303 leituras de Map.values e 172 de Map.keys do 4.0).
- `test/js-runtime.test.ts`: cada built-in roda no Node e em C++ com as mesmas entradas, e o resultado é comparado por
  bits e bytes. Os 6.449 casos batem. O teste achou e corrigiu Math.round dando −0 em [−0,5; 0) e o surrogate solto
  virando U+FFFD.
- Tradutor (`src/translate/expressions.ts` `builtin()`): todo built-in que ele não reescreve sozinho vira
  `js::<nome>(receptor, argumentos)` pelo nome do ECMAScript (toFixed → js::to_fixed, Number.isInteger →
  js::number_is_integer, String(x) → js::to_string). O tipo C++ sai do tipo TypeScript, o template com número real
  vira js::to_string, e Map vira js::Map. O runtime vai junto da tradução como `native/src/core/js_runtime.h`, fora da
  quarentena, que é para o código traduzido; o diferencial imprime mapas na ordem de inserção.
- Estado: 48/48 testes. Mesmos insumos de antes: road-elevation 30 traduzidas, lab 29 → 32.

## A fazer, em ordem

Nada está pela metade: tudo o que foi começado está commitado nos dois repositórios, que estão limpos. O que segue são
trabalhos novos, em ordem.

1. Rodar o censo dos built-ins na referência nova (`scripts/builtins-census.ts`) e acrescentar ao runtime o que falta,
   cada um com casos no `test/js-runtime.test.ts` contra o Node. Num built-in que falta, a tradução não compila e o
   erro diz o nome (`js::<nome>`).
2. Refazer as features e as conversões na referência nova (`--graph out/city40/program.json --analysis
   out/city40/whole/jelly`). A parceira passa por área: world, render, editor, sim, lab, core, ui, people, view, jobs.
   Entradas que só reexportam já funcionam (`9499eac`).
3. Gates do jogo sobre `native/src/core/js_runtime.h` (números mágicos etc.): o runtime precisa nomear as constantes
   que ainda estão nuas (0x80, 0xD800...) para passar no `NUMERO_MAGICO`.
4. Diferencial por unidade contra o dono do jogo: precisa do mapeamento de chamada da library. Hoje o
   `--coverage-proof shown` só prova que o dono existe e cita a unidade.
5. Abertos na sessão parceira: noGround (pattern `new Error($m)` na library), buildMode no coverage,
   toLocaleString / performance / window / Promise / try-catch (do lado da interface, method).

## Regras desta sessão (do usuário)

- Atacar a causa raiz, nunca caso a caso. Generalizar.
- Rápido: processos em segundo plano com acompanhamento, sem esperas cegas, sem metodologias pesadas.
- Nunca escrever o jogo original. O arquivo de gates e os hooks só o usuário altera.
- Texto do projeto em inglês; chat em português do Brasil.

## Histórico (mais antigo primeiro)

Datas e horas dos commits (fuso -03). "slice" é o worktree `converter-slice` (branch `slice-envelope`); "peer" é a sessão
"Roadcraft 4.0 migration analysis".

### 2026-11-04 — o conversor nasce (git do `tools/converter`, branch master)

- 00:31 `2010154` Estágio graph: o programa do 4.0 como grafo de unidades e arestas.
- 00:41 `cad4aca` Funções internas de fábricas viram unidades; padrões de metodologia registrados.
- 00:46 `d186ec4` Estágio audit, com catálogo de metodologias e fontes.
- 00:52 `7c21e3c` Fluxo de callbacks e agendamentos; caminho quente enraizado nos loops de frame.
- 00:56 `132f57d` Achados de desempenho alcançáveis só estaticamente: médio, não alto.
- 01:07 `8c9b284` Estágio measure (chamadas por frame de uma execução real); revertido às 01:22 em `27022b1`, e a
  promessa dele saiu do relatório em `21f45f7`.
- 01:09 `2e41f8b` Flags `--name=value`.
- 01:30 `2304b1a` Pacotes de conversão, sandbox e baseline.
- 01:38 `314032f` Correções do próprio audit do conversor e de uma revisão de regras.
- 01:49 `afd1726` Grafo C++ do jogo, rastreado pelo clangd; 01:51 `1202b10` audit do C++ com o mesmo catálogo; 01:59
  `5e19619` grafo C++ e regras de estado precisos contra o jogo.
- 01:59 `a6b8d12` classify: papéis a partir de fatos, dependências mapeadas, itens que bloqueiam.
- 02:21 `607ee0d` Cada campo pertence ao subsistema que o guarda; 02:24 `ac9f86c` valores literais das duas linguagens e
  caminhos isentos do alvo.
- 03:18 `c4d626c` Estágio translate: TypeScript → C++, tipos decididos por um único fluxo de valores.
- 03:29 `5c566f7` Diferencial: a tradução testada contra a fonte, em dois builds.
- 03:32 `6e8f8df` Pacote de tradução (arquivos, registro onde os gates olham, membro do estado); 03:33 `6c776f2` README.
- 03:47 `7525d01` DESIGN: qualquer feature convertida inteira, toda decisão por regra a partir de fatos.
- 04:04 `b2b9f92` Fluxo sobre o programa inteiro com worklist; tipos inteiros só por fatos.
- 04:21 `16fa30f` Feature rastreada pelo programa inteiro; um módulo por arquivo-fonte.
- 05:04 `bcb5aa9` Tipos do alvo por forma, nomes de literais, chamadas por métodos; `f745721` feature cortada na fronteira.
- 05:42 `88f4c42` Feature inteira por fluxo, com o call graph do Jelly; 06:53 `299e34b` valores por objeto (points-to do
  Jelly).
- 06:53 `34e0d40` workspace, verify e pack (o porte escrito num clone do jogo); `31eade9` audit do porte pelas linhas
  mudadas; `be4259d` driver do porte (`scripts/port.ts`, caminhos de um arquivo de máquina).
- 06:57 `a9dbaf4` Fotos de referência: fecha o menu inicial do 4.0 e fixa meio-dia.
- 07:03 `a877828` Código compartilhado como declarações em `interface/`; 07:04 `b99d8ed`; 07:05 `f4ddc9e` sandbox aplica
  o pacote sobre a base.
- 09:06 `d504143` Interface: de que valores um valor é escrito e o que as fatias leem.
- 12:37 `1a68446`, 13:34 `5a6173a`, 14:45 `3f1d399` port.ts reference: servidor do 4.0 compartilhado por concessão, uma
  rodada de foto por vez, janela oculta em todo processo filho.
- 15:46 `f8b8949` workspace: cada clone com o próprio diretório de usuário do Godot.

### 2026-11-04/05 — tradutor three.js → Godot e replay de frame real (registro detalhado em `tools/converter/GOAL-LOG.md`)

- Processo de 7 passos imposto pelo usuário: identificar → pesquisar → raciocinar → planejar → codificar sem testar →
  testar uma vez → parar e verificar; recomeçar no passo 1 em caso de erro.
- Ciclo 1 (partida, `frame2`, 177 draws): 51 iguais, 60 próximos, 6 diferentes, 60 vazios; porte C++ 177/177 idêntico
  ao GDScript. Causas achadas e corrigidas: P1 ALPHA levava ao pipeline transparente (passe de alfa separado), P2 o
  ARRAY_COLOR de 8 bits do Godot (cor de vértice via CUSTOM float), P3 o framebuffer espelhado (orientação GL), P4 o
  PCF manual em float (pesos a 1/256), P5 alvos float lidos em RGBA8 (meio float comparado como meio float, float32
  em 6 passadas de bytes), P8 o id da tradução em cache.
- Ciclo 2: vista 5 com 138 iguais, 97 próximos e 1 diferente; porte C++ 667/667 idêntico byte a byte. Restam as
  diferenças inerentes de até 8/255 (o SPIR-V do Godot não tem `precise`).
- 2026-11-05 12:23 `9c66a48` O tradutor three.js → Godot (shader e C++), o replay de frame real e o porte GDExtension
  entram no master. O usuário mandou manter o worktree e avisar o peer de que a ferramenta pode ser usada.
- 12:28 `0d666d3` oracle-calls `--scenario <file.mjs>`: um cenário próprio da feature.
- 12:49 `f40cff0` diferencial `--hand`: C++ escrito à mão no lugar dos arquivos da tradução (testado: igual → 0
  diferenças; mutação n+1 → 75).

### 2026-11-05 — o tradutor contra as features reais do peer

- 14:07 `a84852d` Parâmetros mutáveis (aliases, ponto fixo), slice, instâncias genéricas, padrões com contexto (tn
  passado aos chamadores). Convert 8x mais rápido: o solve do fluxo era 95% do tempo; road-elevation de 249 s para 29 s,
  com os mesmos arquivos e o mesmo relatório.
- 14:23 `3b52e46` (slice `d69275d`) Área `lab` → `native/src/api/lab`; nome de tipo do jogo reservado (Profile →
  ElevationProfile); interface só com `get` (NodeHeights) lida como mapa.
- 14:37 `626f11f` (slice `2bd8c83`) Arquivos dos portes pelos nomes em inglês. O peer renomeou `tools/processo` →
  `tools/process` e `tools/portes` → `tools/ports` (commit ad200882 do jogo).
- 14:57 `cf91795` (slice `ed5f611`) Pedidos do peer: has() de interface como predicado, `<optional>` faltando,
  separador numérico, Frame/RoadStructureSpec não declarados, reduce/startsWith/regex, `[...xs].sort()` que ordenava o
  `const &` (origem do erro de swap), tuplas, `**`, bitwise, imul, replace, throw por padrão. Resultados: road-elevation
  de 21 para 30, lab de 10 para 29.
- 14:57 `1695312` configs com os caminhos em inglês (`port.ts doctor` OK).
- 15:06 `117f6d7` (slice `d2fbf98`) Regressão de 29 para 17 achada e corrigida: includes calculados para a tradução
  inteira (agora por módulo, pelo texto) e `Pick` da lib do TS nomeado `Lib.es5.dPick`; módulo compartilhado
  traduzido como união.
- 15:12 `5673af1` (slice `f17d023`) `--coverage-proof shown`, a pedido do peer: no poles, 268 das 275 alegações do
  coverage não aparecem no jogo.
- 15:15 `c8c2f5c` (slice `77f58dd`) O peer escolheu a função dona `structure_spec(Structure, const Tuning &)` para
  ROAD_STRUCTURES; a library ganhou `types`, `functions` com include/context e pattern `owned`. Censo dos built-ins.
- 15:20 O usuário: "deixa genérico, sem caso a caso, trate a causa raiz". Diagnóstico: uma regra escrita à mão por
  built-in. Decisão: runtime C++ dos built-ins com uma regra só.
- 15:22 O usuário e o peer: a referência passa a ser `C:\Users\Jonathan\Documents\Projects\city\roadcraft` (2921c465,
  29/10); entre as duas, 345 arquivos de src diferem e 144 só existem numa. Config apontada para ela; grafo refeito (79 s,
  863 arquivos, 20.318 unidades) e Jelly do programa inteiro (59 s) em `out/city40`.
- 15:26 `aab7600` (slice `5acf06c`) Runtime `cpp/js/js_runtime.h` e regra única `js::<nome>`; Map → js::Map, na ordem de
  inserção do JS. Diferencial do runtime: 6.449 casos iguais ao Node, depois de corrigir Math.round (−0) e o surrogate
  solto. 48/48 testes; lab de 29 para 32.
- 15:35 O usuário pediu este registro em `log/`. O `CLAUDE.md` do projeto proíbe documentos além dele (gate
  `DOC_UNICO`); o arquivo foi criado a pedido explícito do usuário.
- 15:40 O usuário: atualizar este arquivo a cada tarefa e a cada informação nova, com o histórico completo.
- 15:45 `9499eac` (slice `b3d564a`) Feature: uma entrada que só reexporta (`src/render/derivedCache.ts`, `export * from
  '@core/derivedCache'`) vale pelos módulos que reexporta, seguidos pelo TypeScript até os que declaram algo. Era o que
  derrubava as passadas por área do peer em render, world e editor. Testado na referência nova (37 declarações, 21 s).
- 15:50 Suíte inteira: 105 testes passando; o cppgraph precisa de `CONVERTER_CLANGD`.
- 15:52 slice `c617bf4` e `564a35d`: o slice tinha cópias sem commit dos arquivos do tradutor three.js → Godot (DESIGN,
  README, GOAL-LOG, catálogos, `cpp/runtime`, scripts de replay, oráculo e paridade). Todos iguais aos do master, menos
  a config, que foi atualizada; commitados. Os dois repositórios ficaram limpos.
- 15:55 O usuário pediu encerrar: este registro atualizado e commitado no repositório do jogo; sessão encerrada.
