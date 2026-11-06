# Registro do conversor (tools/converter)

Para outra sessão continuar o trabalho do conversor e tradutor do Roadcraft 4.0 para o jogo.

**Regra do usuário (2026-11-05): a cada tarefa feita e a cada informação nova, atualizar este arquivo.** O "Histórico"
recebe uma linha por acontecimento (data, hora, o quê, commit); "Estado atual" e "A fazer" são reescritos para ficar
sempre verdadeiros.

## Estado atual (2026-11-05, sessão retomada)

- **O alvo é o jogo, o 4.0 é só a referência (correção do usuário, 2026-11-05).** Nada traduzido chegou ao jogo ainda: esse é o trabalho, à frente de qualquer ferramenta em volta do 4.0.
- **Primeira área no jogo (2026-11-05):** `src/sim/roads/flowStats.ts` e `controlAdvisor.ts` traduzidos sem edição, compilados no jogo e iguais ao 4.0; ver "Causa e prova" no histórico.
- **Estado do tradutor:** sistema de tipos aberto (`js::Value`), classes, genéricos, subclasses, setters. `--mode whole`: 4.130 unidades compilam, de 19.377 (11.238 traduzíveis), e 4 arquivos ficam fora; 370 s na pior passada. Compilar não é conferir: fora do `flow_control`, nada disso foi comparado com o 4.0.
- **Caminho validado para portar:** `CXX=<clang++> node scripts/port-area.ts <plano> <saída>`, área por área: traduz e confere contra o 4.0 sem jogo (3.000 chamadas sorteadas em 2,1 s). Validado numa área só.
- `tools/converter` (master `c3642d9`, conteúdo trazido ao repositório do jogo); `tools/converter-slice` atrás do master (falta aplicar `5a15b1f` em diante). O repositório do jogo ainda tem o conversor em `9499eac`: falta trazer com `git read-tree`.
- Tradutor com subtipagem, templates, runtime C++ dos built-ins (regra única `js::<nome>`), `js::Set` e `js::Map`.
  Suíte inteira: 114 testes (ver "Comandos": sem as cinco variáveis, os de `feature` e `cppgraph` não rodam).
- Referência nova (`Documents\Projects\city\roadcraft`) com grafo e Jelly em `out/city40`. As passadas por área usam as
  fronteiras que a sessão do porte cortou (`<scratchpad dela>/converter-snap/out/features/area-*`).

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
- Testes, todos (sem as variáveis, um teste que depende delas não roda, e o resumo pode dizer "fail 0" assim mesmo; conferir
  se não sobrou nenhum `✖`):
  `CONVERTER_TYPESCRIPT=../oracle-4.0/node_modules/typescript CONVERTER_CXX=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64/bin/x86_64-w64-mingw32-clang++.exe CONVERTER_CLANGD=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64/bin/clangd.exe CONVERTER_JELLY=C:/Projects/toolchains/jelly/node_modules/@cs-au-dk/jelly CONVERTER_POSTCSS=C:/Users/Jonathan/Documents/Projects/city/roadcraft/node_modules/postcss node --test "test/**/*.test.ts"`
  (o de translate compila o C++ gerado e roda o diferencial contra a fonte; `js-runtime` compara o runtime com o Node).
- Grafo de uma referência: `node --max-old-space-size=12288 src/cli.ts graph --source-root <ref> --typescript <ref>/node_modules/typescript --out <arquivo>`
- Feature (fatia e Jelly): `src/cli.ts feature --source-root <ref> --graph <program.json> --analysis <pasta jelly> --entry <src/...> --out out/features/<nome>`
  (`--jelly C:/Projects/toolchains/jelly/node_modules/@cs-au-dk/jelly` em vez de `--analysis` para refazer a análise).
- Convert: `src/cli.ts convert --feature <f> --source-root <ref> --typescript <ref>/node_modules/typescript --graph <program.json> --boundary out/features/<f>/boundary.json --target-root C:/Users/Jonathan/Desktop/city --target-graph <cppgraph.json> --library <conversion-library.json> --ports C:/Users/Jonathan/Desktop/city/tools/ports --cxx <clang++> --out <pasta>`
  Opções: `--coverage-proof shown` (só vale "in the game" que o jogo mostra), `--constants <json>`.
- Censo dos built-ins: `node scripts/builtins-census.ts --source-root <ref> --typescript <ref>/node_modules/typescript --out out/builtins-census.json`

## O 4.0 sem nada aberto

A referência é um commit fixo, então o que ela produz é imutável. Em `tools/converter`:
- **Comportamento:** `node --no-warnings scripts/call40.mjs src/<modulo>.ts '<expressão com m>'`, JSON em 0,15 s, sem navegador.
- **Cena real:** `node --max-old-space-size=8192 --no-warnings scripts/call40.mjs --scene '<expressão>'`, 16 s: `createSceneRenderer` sobre a GPU nula dos
  testes do 4.0, `m = { city: {scene, doc, net, sim}, view([x,y,zoom,az,el]), settle() }`. Vista de rua: 364 draws (censo dele: 363).
- **Fotos:** `node scripts/reference40.mjs <plano.json> <pasta> [--again]`. Plano já feito: 0 s, do arquivo (`out/reference-store`, por commit + plano). Plano
  novo: servidor estático dentro do processo sobre o build (`out/build40/<commit>`, feito uma vez com `vite build --mode probe --outDir`, 10 s) e Chrome headless;
  tudo fecha ao terminar (28 s). Aceita o plano do `look.mjs` e o do `look.gd` (`{"city": {...}, "steps": [...]}`).
- **Edições:** passo `{"edit": "<código com d = RoadDoc>"}`: muda o documento e reabre por `loadDoc`. Direto no `doc` a cena não fica sabendo (não passa pelo
  `mutateBuilt`). Exemplo: `configs/plans/viaduct.json` (avenida elevada a 8 m sobre uma rua).
- **Foto sem Chrome nenhum: decidido não fazer agora.** O contexto nulo não tem linker nem pixels, e o quadro teria de ser desenhado em sequência no Godot. Mas aí a
  "foto do 4.0" seria desenhada pelo nosso motor, e deixaria de ser a verdade contra a qual o Godot é conferido. O Chrome roda só para plano novo e fecha.

## Roteiro: portar com o conversor

Em `tools/converter`. `node scripts/port.ts doctor` confere os caminhos de `configs/roadcraft-machine.json`.

1. **Rastrear:** `node scripts/port.ts extract <feature> <src/entrada.ts> ...` (grafo e Jelly ficam em cache por versão da fonte).
   Saída: `out/features/<feature>` (`boundary.json`, `feature.md`, `interface/`). Passada por área: `src/cli.ts feature` com
   `--graph out/city40/program.json --analysis out/city40/whole/jelly --entry ...` para cada arquivo da área.
2. **Clone de trabalho:** `node scripts/port.ts workspace <feature>` (o jogo nunca é escrito direto).
3. **Converter:** `node scripts/port.ts convert <feature> --apply`, ou `src/cli.ts convert` (ver "Comandos"). Saída em
   `<ports>/<feature>/conversion`: `files/` (o C++ aceito, já nas pastas do dono: `sim/*`, `core`, `render/*`, `api/lab`, e o runtime em
   `core/js_*.h`), `conversao.md` (cada unidade aberta, com dono e método), `ledger.json`, `registration.json` (membro do estado e linha do
   mapa), `specs/` (um spec por módulo aceito). `--apply` copia os arquivos para o clone.
4. **O resto à mão, no dono:** cada linha aberta de `conversao.md` diz o método. A interface é sempre à mão (nunca há alvo GDScript).
5. **Conferir contra a fonte:** `src/cli.ts differential --source-root <ref> --spec <specs/modulo.json> --target-graph <cppgraph.json>
   --target-root <jogo> --cxx <clang++> --out <pasta> --hand native/src/<arquivo.cpp>`: o mesmo cenário aleatório na fonte e no C++, e o passo e o
   valor de cada diferença. Sem `--hand`, compara a própria tradução.
6. **Jogo:** `node scripts/port.ts verify <feature>`, `audit <feature>`, `pack <feature>` e `sandbox <feature>`; depois os gates, as DLLs e a
   suíte, como manda o `CLAUDE.md`. `reference <feature>` tira as fotos do 4.0.

**O que cada verificação prova:**
- `differential`: o C++ dá o mesmo resultado que o TypeScript nos mesmos dados (lógica). `js-runtime.test.ts`: os built-ins iguais ao Node.
- `scripts/verdict.mjs` + `oracle-calls.mjs` (cópia instrumentada, `tools/oracle-4.0`): o rastreamento contra o que o 4.0 executou de fato.
- `scripts/oracle.mjs`: o que uma execução real chamou (cobertura V8, perfil de CPU) e todo shader dado à GPU.
- `scripts/shaders.ts` + `shaders-verdict.mjs`: cada shader traduzido compila e desenha no Godot.
- `scripts/parity.mjs`: three.js e Godot desenham igual, casos sintéticos.
- `scripts/frame-capture.mjs` + `frame-replay.mjs`: um quadro real do 4.0, draw a draw, contra o Godot.
- `scripts/draw-coverage.mjs`: se um draw vazio nos dois motores devia mesmo estar fora da tela.

**Lista do que falta:** `tools/process/records/map40/features.json` tem 308 funcionalidades com linhas e unidades, mas foi gerado do grafo
antigo (16.771 unidades; o 4.0 novo tem 20.318) e o campo de estado vem do `coverage.md`, que não é confiável (no poles, 268 de 275
alegações não aparecem no jogo). O que vale: `convert --coverage-proof shown` e `python scripts/open-causes.py <pasta>`.

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

1. Seguir a contagem de recusas por causa no programa inteiro (método abaixo) e tratar sempre a maior causa geral. Das 8.114
   unidades traduzíveis das 8 áreas, 3.262 saem. As demais, pela última contagem (`p5`):
   - 3.026 em módulos recusados inteiros: entender por que a recusa por unidade ainda não separa esses módulos é o próximo
     passo (253 unidades em "values of different shapes meet" de `{ x, y }` e 90 em `nodes[]`; 126 em `ArrayBufferLike`; 77 em
     `never[]`; 64 em `typeof location`);
   - 939 em módulos aceitos, fora do que os roots alcançam (741 funções, 198 constantes);
   - 887 em módulos sem spec (295 constantes, 255 interfaces, 248 funções, 89 tipos).
   Recusas corretas, que não são defeito: os ~1.100 `Node<"vec4">` etc. são shaders TSL, que têm o caminho three.js → Godot.
2. Integrar ao jogo o que compila, área por área (`log/work-log.md`, passo 1). O runtime entra em `native/src/core/js_*.h`:
   a pasta `core` é isenta do `NUMERO_MAGICO`, e cada header tem menos de 250 linhas, o limite do gate.
3. Diferencial por unidade contra o dono do jogo: precisa do mapeamento de chamada da library. Hoje o
   `--coverage-proof shown` só prova que o dono existe e cita a unidade.
4. Fica do lado da interface (`method`): toLocaleString, performance, window, Promise, try/catch.

**Método (ordem do usuário: nada de processos longos):**
- Corrigir uma causa: um caso no fixture (`test/fixtures/translate`) e `node --test test/translate.test.ts`: segundos, com o C++ compilado e o
  diferencial contra a fonte. Escolher a causa pelo `ledger.json` que já existe (`scripts/open-causes.py`), sem rodar nada de novo.
- Passada do programa inteiro: só no fim de um bloco de correções, uma vez, em segundo plano. Nunca por área: o perfil mostrou 63 de
  106 s em preparo fixo (abrir o programa e resolver o fluxo do programa inteiro), igual em cada uma das 8 áreas, e os módulos comuns
  julgados de novo em cada uma. A compilação não pesa.
- Falta medir: uma passada inteira (`--boundary out/city40/whole/boundary.json`) não terminou; foi parada a pedido do usuário depois de
  267 módulos.

**Classes (decisão pendente, do usuário):** é a maior porção do que resta: cerca de 4.800 unidades (`a member of a class`, 235 arquivos, só 8 com
`extends`). Traduzir fiel exige semântica de referência (`std::shared_ptr`), objetos que implementam interfaces (`Sink`, `Path`) e `this`. O que
sairia é o `RoadDoc`, o tráfego e o que mais o 4.0 tem em classes, que o jogo já tem na própria arquitetura (`CityState`, `sim/*`): o
`CLAUDE.md` diz que não se porta o código. Antes de construir isso, confirmar que vale.

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

### 2026-11-05 (tarde) — sessão retomada: a causa pela contagem

- Registros de `log/` lidos, como o `CLAUDE.md` agora pede.
- Contagem das recusas de todas as áreas (passada da sessão do porte, 7.522): 4.126 eram "values of different shapes meet".
  O fluxo tratava a subtipagem do TypeScript como igualdade: tudo o que passava por um genérico ou por um parâmetro
  estreito virava um slot só, e `Building` "encontrava" o programa inteiro.
- Correção no modelo: argumento → parâmetro é fluxo de um sentido; função genérica é template C++ instanciado no ponto da
  chamada. Recusas de forma: 4.126 → 182.
- Segunda contagem: das 8.114 unidades traduzíveis só 1.958 saíam: 2.519 fora dos roots (1.746 constantes), 1.677 em
  módulos sem spec, 1.960 em módulos recusados (239 num crash do tradutor).
- Corrigido em seguida: constantes como roots, recusa por unidade (crash e erro de tipo dizem a unidade), `Math.PI`,
  tuplas por posição, `switch`, `throw` como `js::fail`, `js::Set`, iteração de Map e Set, nome próprio de struct anônima.
- Bug de correção achado pelo diferencial: `n + TURN` truncava para inteiro, porque um número de fora do fluxo contava
  como "sem evidência".
- Correções deste registro: "105 testes" estava errado, porque os 10 de `feature` não rodavam (faltavam `CONVERTER_JELLY` e
  `CONVERTER_POSTCSS`, e o resumo dizia "fail 0"); a pendência do `NUMERO_MAGICO` não existia (`native/src/core/` é isenta),
  mas havia outra: o runtime passava de 250 linhas num header, por isso foi dividido em `js_number`, `js_string`, `js_array`,
  `js_map` e `js_runtime`.
- Commit do master na linha acima de "translate: subtyping and templates".
- `8ff985b` (slice `ed93bf8`) Subtipagem, templates, constantes como roots, recusa por unidade, `switch`, `throw`, `Set`, `Map`.
  Traduzidas: 1.958 → 3.094.
- `48a5370` (slice `719171a`) Constantes escritas depois que todo nome existe, com acessor para as que chamam função ou leem
  outra não constexpr: o C++ não inicializa constantes de arquivos diferentes na ordem da fonte, e o diferencial pegou 0 no
  lugar de 10. `a?.b` não tinha guarda (`->` direto, comportamento indefinido): agora fica ausente quando o receptor está.
  `xs[i] ?? d`, `Record<string, V>` como mapa. A quarentena lia a palavra "new" em comentário como alocação.
  "name outside" 229 → 45, "call unknown" 123 → 20.
- `9ccbdaf` O slot que declara um tipo lê por ele os valores mais largos; `unique symbol` não é constante; literal em posição de
  tupla é string. Traduzidas: 3.262 (de 8.114; core 145, lab 327, sim 423, editor 67, ui 9, people 6, world 710, render 1.575).
  Suíte: 114 testes.
- Hook: `git add` e `git commit` no mesmo comando fazem o hook contar as mudanças de `native/` de outra sessão, mesmo num commit
  do conversor. Sempre em comandos separados (regra do `CLAUDE.md`).
- Contagem por causa: `python scripts/open-causes.py <pasta com <área>/conversion/ledger.json>`.
- `a0ded2f` Estado só de quem o módulo faz do nada (`expand(box)` era tratada como edição no lugar, o que mudava a semântica); enums em
  `_enums.h` próprio (inclusão circular escondia o enum); enum anônimo no módulo da struct; tipo que estende Array é lista; `constexpr` só do que
  C++ avalia; null contra optional; número convertido onde as chaves recusam estreitar.
- O conversor passa a gravar `specs/` no `convert`, para `differential --spec --hand`. Roteiro acima escrito a pedido da sessão de porte.
- `5a15b1f` Módulos julgados à frente com a compilação em paralelo; `patterns.match` e `calleeOf` resolvidos uma vez; progresso gravado a cada
  30 s (antes, o livro-razão inteiro a cada módulo).
- O usuário mandou parar processos longos: passada inteira parada (PID próprio), método acima registrado.
- `3a93b1e`, `e4ec430` e o commit seguinte: `reference40`, `call40`, `--scene` e o passo `edit`. Viaduto do 4.0 fotografado (59 s a primeira vez, 0 s depois).
  Pedido do usuário: extrair sem deixar o 4.0 aberto; ideia da GPU nula veio da sessão de migração; plano do `look.gd` a pedido da sessão de causa-raiz.
- `af95221` `openProgram` memoizado: um programa TypeScript por processo.
- Passada inteira `w2`: bateu no limite de 300 s (301 s); o livro-razão dela é parcial (502 módulos vistos, 359 aceitos, 2.609 unidades feitas) e não serve de número. Refeita como `w3`, limite de 590 s.
- Pixels do 4.0 no Node: `@onirenaud/node-webgl` 0.2.0 instalado em `out/gl40` (fora do git). Triângulo desenhado e lido de volta, ANGLE D3D11 na RTX 3060, com `EXT_clip_control`, `EXT_color_buffer_float`, BPTC e `WEBGL_multi_draw`. Só isso foi provado: a cena do 4.0 não rodou nele. `gl` não instala no Node 24 (ABI 137; os binários vão até o 134).
- Trabalho na cena do 4.0 parado pelo usuário: "O ROADCRAFT 4 É A REFERENCIA E NÃO O JOGO ALVO". As fotos de referência continuam saindo do `reference40`. Volta ao passo 2 de "A fazer": levar ao jogo o que compila.
- `dec52da` Classes traduzidas (struct com `shared_ptr`, `make_shared`, `this`, getters e estáticos); `73832a2` classes genéricas como `template`, `enum` pelo valor, classe alcançada pelo que se lê dela; `54d3a50` recusa por membro; `b4303e6` `--mode whole`.
- Passadas: por módulo, 3.901 (classes), 3.905 (genéricas), 3.912 (por membro), de 19.377 unidades. `--mode whole`: 4.580, com funções 410 → 1.115, métodos 5 → 181, classes 7 → 66; tradução de ~5 s por rodada, 923 s no total, quase tudo compilação. **Ressalva:** os 4.580 contam unidades de 278 arquivos que não compilam; o número compilado é menor e não foi medido.
- Onde parou, próximos passos de `--mode whole`, nesta ordem, nenhum feito:
  1. `whole.ts` não acha o dono de um erro em constante de cabeçalho: a lista de `units` traz constantes pelo nome solto, sem `module#`. Por isso um erro em `perf_budget_types.h:28` derruba quase todos os arquivos que o incluem.
  2. `#freshName`: struct anônimo herda o nome da constante em maiúsculas (`SIGNAL SIGNAL`, `VIEWBUDGETS`, tipo não declarado).
  3. `#planConstant`: constante de objeto vira ponteiro para temporário (`&(OutHiLo{...})`); usar o valor.
  4. Constantes literais de módulos diferentes colidem no mesmo namespace (`MB`, `H`).
  5. Arquivos gerados com o caminho de um do jogo (`core/math.h`) precisam ser vistos antes de integrar.
  6. O cache de `compileFiles` usa o hash de todos os cabeçalhos: qualquer mudança recompila tudo (416 s na primeira rodada).
- Ainda recusados por unidade: subclasses, setters, campos estáticos, `unknown` (`raw`).

### 2026-11-05 — Causa e prova

**Causa de a ferramenta não entregar:** ela media o que traduzia, nunca o que chegava ao jogo. O contador "in the game" ficou em 0 o tempo todo enquanto 3.900 unidades "traduzidas" não tinham como entrar: uma segunda cópia do 4.0, sem ligação ao estado do jogo (`convert/ledger.ts`, `isDone` em `convert/command.ts`), e nenhum teste rodava o código dentro do jogo. Medido: das 36 unidades de `flowStats` e `controlAdvisor`, o primeiro uso real achou defeitos que 114 testes não viam:
- ids como `float` (`translate/flow.ts` `#declare`, `translate/types.ts` chave de `Map`): dois ids acima de 2^24 viravam a mesma chave, em silêncio;
- `a?.get(x)` sem proteção contra nulo (`translate/expressions.ts` `call`);
- ponteiro `const` em objeto alterado; `optional` estreitado não lido como valor; campo estático; construtor implícito tratado como unidade barrada; constante de tuning sem acesso para outro dono.

**Mudança:** essas causas corrigidas no tradutor; `scripts/port-area.ts`; regra nova no `CLAUDE.md` (a lógica pura entra traduzida, regenera-se, nunca se conserta à mão).

**Prova:** `project/tests/core/flow_control_test.gd`, nos dados do 4.0 por `call40`: `busy_major` 872,4247 contra 872,4241, `changed_at` 900 igual, `quiet_flow` 240 igual, id 33.554.433; 148 cruzamentos contados por veículos reais; um cruzamento da cidade virou semáforo. Tolerância 2e-5 (float contra double).

**Fica por fazer:** o mesmo por área, na ordem do que falta no jogo. `whole` ainda não compila tudo (lista acima).

### 2026-11-05 — A causa da cauda de recusas

**Causa:** o sistema de tipos do tradutor era fechado: todo valor precisava de forma estática em C++, e o que não tinha era recusado (`translate/types.ts` `#unsupported`, `translate/flow.ts` `shapeOf`, `translate/expressions.ts` em `typeof`, `instanceof`, `in`, `??`). A tradução por módulo somava outra: dois módulos escreviam o mesmo arquivo de jeitos diferentes, e tipos com dependência cruzada não têm ordem em cabeçalhos separados.

**Mudança:** `cpp/js/js_value.h` e o tipo `dynamic`, com caixa e descaixa nas fronteiras; o que a plataforma declara fica dinâmico; subclasses, setters, spread em objeto, `WeakMap`, laço sobre `x ?? []`; `--mode whole` com todos os tipos em `core/program_types.h` em ordem de dependência, cabeçalhos numa compilação só, recompilação só de quem inclui o que mudou; tipo que o compilador recusa vira dinâmico, erro sem dono leva só o módulo.

**Prova:** suíte do conversor 116/116, com diferencial contra o TypeScript em `unknown`, `virtual`, setter e spread, e mutação que o check leve pega. Passada inteira: cabeçalhos sem erro, 4 arquivos `.cpp` de fora.

**Limites, sem enfeite:**
- 4.130 unidades compilam; não foram rodadas contra o 4.0, fora as 36 do `flow_control`.
- Passou de 4.580 para 4.130 porque o número antigo contava arquivos que não compilavam.
- 2.554 unidades ficam de fora por rodada e 8.139 são `method` ou `write` (desenho, interface, `hot-allocation`).
- A área já no jogo não foi regenerada depois destas mudanças; o `js_value.h` ainda não está em `native/src/core`.
- Interfaces implementadas por classes (`Sink`, `Path`) e classes que estendem `Map` continuam sem forma.
