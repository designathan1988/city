# Registro do conversor (tools/converter)

For another session to continue the work on the converter and translator into the game.

**Regra do usuário (2026-11-05): a cada tarefa feita e a cada informação nova, atualizar este arquivo.** O "Histórico"
recebe uma linha por acontecimento (data, hora, o quê, commit); "Estado atual" e "A fazer" são reescritos para ficar
sempre verdadeiros.

## Estado atual (2026-11-05, sessão retomada)

- **The target is the game (user correction, 2026-11-05).** Nothing translated has reached the game yet: that is the work, ahead of any surrounding tooling.
- **First area in the game (2026-11-05):** the flow statistics and the control advisor translated without editing and compiled into the game (`flow_control`); see "Causa e prova" in the history.
- **Translator state:** open type system (`js::Value`), classes, generics, subclasses, setters. `--mode whole`: 4,130 units compile, of 19,377 (11,238 translatable), 4 files left out. `scripts/check-all.ts` runs each translated module against its TypeScript source without the game: 84 of 359 modules match.
- **Validated path to port:** `CXX=<clang++> node scripts/port-area.ts <plan> <output>`, area by area: translates and checks against the TypeScript source without the game (3,000 random calls in 2.1 s). Validated on one area only.
- `tools/converter` (master `c3642d9`, conteúdo trazido ao repositório do jogo); `tools/converter-slice` atrás do master (falta aplicar `5a15b1f` em diante). O repositório do jogo ainda tem o conversor em `9499eac`: falta trazer com `git read-tree`.
- Tradutor com subtipagem, templates, runtime C++ dos built-ins (regra única `js::<nome>`), `js::Set` e `js::Map`.
  Suíte inteira: 114 testes (ver "Comandos": sem as cinco variáveis, os de `feature` e `cppgraph` não rodam).
- Program graph and Jelly in `out/city40`. Per-area passes use the boundaries the port session cut
  (`<its scratchpad>/converter-snap/out/features/area-*`).

## Onde está cada coisa

- Conversor: `C:\Users\Jonathan\Desktop\city\tools\converter`, git próprio, branch `master`. Commits só ali; o conteúdo
  vai para o repositório do jogo por `git read-tree` (sessão dona do jogo).
- Worktree `C:\Users\Jonathan\Desktop\city\tools\converter-slice` (branch `slice-envelope`): manter. Cada commit do
  master é aplicado lá por `git show <commit> --format= | git apply` e commitado como "sync from master".
- Program graph and Jelly analysis: `tools/converter/out/city40/program.json` (863 files, 20,318 units) and
  `tools/converter/out/city40/whole/jelly` (callgraph.json, tokens.json). The older `out/rc40/program.json` and
  `out/jelly` are stale.
- Machine configuration: `tools/converter/configs/roadcraft-machine.json`. It points to `tools/ports` and to
  `tools/process/records/map40/features.json`. After changing it, run `node scripts/port.ts doctor`.
- Partner session: the port session (`uds:\\.\pipe\LOCAL\cc-msg-056396bf66f931dac736851d7e135edc`). It does the ports,
  the libraries (`tools/ports/<port>/conversion-library.json`) and the per-area passes. The converter is on this side.

## Comandos

- Typecheck: `node scripts/typecheck.ts --typescript ../oracle-4.0/node_modules/typescript --types ../oracle-4.0/node_modules/@types`
- Tests, all (2026-11-07: every tool path comes from `configs/roadcraft-machine.json` through `src/common/machine.ts`;
  `CONVERTER_*` env vars only override; 136 tests, the godot-shader suite included): `node --test "test/**/*.test.ts"`
  (translate compiles the generated C++ once per translation and runs the differential with each seed as data; `js-runtime`
  compares the runtime with Node).
- Program graph: `node --max-old-space-size=12288 src/cli.ts graph --source-root <src> --typescript <src>/node_modules/typescript --out <file>`
- Feature (slice and Jelly): `src/cli.ts feature --source-root <src> --entry <src/...> --out out/features/<name>` (since
  2026-11-07 the program graph and the Jelly analysis are derived artifacts keyed by their inputs; `--graph`/`--analysis` are gone).
- Convert: `src/cli.ts convert --feature <f> --source-root <src> --typescript <src>/node_modules/typescript --graph <program.json> --boundary out/features/<f>/boundary.json --target-root C:/Users/Jonathan/Desktop/city --library <conversion-library.json> --ports C:/Users/Jonathan/Desktop/city/tools/ports --cxx <clang++> --out <folder>`
  Options: `--coverage-proof shown` ("in the game" counts only what the game shows), `--constants <json>`.
- Built-ins census: `node scripts/builtins-census.ts --source-root <src> --typescript <src>/node_modules/typescript --out out/builtins-census.json`

## Queries without anything open

(Section removed: it described querying the removed reference.)

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
5. **Check against the source:** `src/cli.ts differential --source-root <src> --spec <specs/module.json>
   --target-root <game> --cxx <clang++> --out <folder> --hand native/src/<file.cpp>`: the same random scenario in the TypeScript source and in C++,
   and the step and value of each difference. Without `--hand`, it compares the translation itself.
6. **Game:** `node scripts/port.ts verify <feature>`, `audit <feature>`, `pack <feature>` and `sandbox <feature>`; then the gates, the DLLs and the
   suite, as `CLAUDE.md` requires.

**O que cada verificação prova:**
- `differential`: o C++ dá o mesmo resultado que o TypeScript nos mesmos dados (lógica). `js-runtime.test.ts`: os built-ins iguais ao Node.
- `scripts/oracle.mjs`: o que uma execução real chamou (cobertura V8, perfil de CPU) e todo shader dado à GPU.
- `scripts/shaders.ts` + `shaders-verdict.mjs`: cada shader traduzido compila e desenha no Godot.
- `scripts/parity.mjs`: three.js e Godot desenham igual, casos sintéticos.
- `scripts/frame-capture.mjs` + `frame-replay.mjs`: a captured three.js frame, draw by draw, replayed in Godot.
- `scripts/draw-coverage.mjs`: se um draw vazio nos dois motores devia mesmo estar fora da tela.

**List of what is missing:** `tools/process/records/map40/features.json` has 308 features with lines and units, but it was generated
from the old graph (16,771 units; the current graph has 20,318) and its state field comes from `coverage.md`, which is not reliable (in
poles, 268 of 275 claims do not appear in the game). What counts: `convert --coverage-proof shown` and `python scripts/open-causes.py <folder>`.

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
- `c8c2f5c`: the library declares game types (`types`) and owner functions with `include`/`context`; pattern `type: owned`;
  `scripts/builtins-census.ts` (old source: 313 ECMAScript core forms with 50,578 uses, 511 browser forms with 7,333).

## Runtime dos built-ins (commit `aab7600` no master, `5acf06c` no slice)

A causa de tudo ir caso a caso é o tradutor ter uma regra escrita à mão para cada built-in do JavaScript.
A correção é um runtime C++ dos built-ins, com uma regra só no tradutor:
- `cpp/js/js_runtime.h`: Number::toString, toFixed (exact), Math (round, sign, fround, imul, clz32), ToInt32,
  strings in UTF-16 units (slice, substring, indexOf, lastIndexOf, trim, padStart/End, charCodeAt, split,
  fromCharCode), arrays (join, splice, pop, shift, unshift, indexOf, includes, findIndex, concat, flat, sort with and
  without a comparator) and `js::Map`/`js::Set` with JavaScript's insertion order (`std::unordered_map` changed the
  order of the source's 303 Map.values and 172 Map.keys reads).
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

**Classes (pending decision, the user's):** the largest share of what remains: about 4,800 units (`a member of a class`, 235 files, only 8 with
`extends`). A faithful translation needs reference semantics (`std::shared_ptr`), objects that implement interfaces (`Sink`, `Path`) and `this`.
What would come out is `RoadDoc`, the traffic and the other class-based code, which the game already has in its own architecture (`CityState`,
`sim/*`): `CLAUDE.md` says code is not ported. Confirm it is worth it before building this.

## Regras desta sessão (do usuário)

- Atacar a causa raiz, nunca caso a caso. Generalizar.
- Rápido: processos em segundo plano com acompanhamento, sem esperas cegas, sem metodologias pesadas.
- Only the user changes the gates file and the hooks.
- Texto do projeto em inglês; chat em português do Brasil.

## Histórico (mais antigo primeiro)

Commit dates and times (UTC-03). "slice" is the `converter-slice` worktree (branch `slice-envelope`); "peer" is the port
session.

### 2026-11-04 — o conversor nasce (git do `tools/converter`, branch master)

- 00:31 `2010154` Graph stage: the source program as a graph of units and edges.
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
- 06:57 `a9dbaf4` Photo stage: closes the start menu and fixes noon.
- 07:03 `a877828` Código compartilhado como declarações em `interface/`; 07:04 `b99d8ed`; 07:05 `f4ddc9e` sandbox aplica
  o pacote sobre a base.
- 09:06 `d504143` Interface: de que valores um valor é escrito e o que as fatias leem.
- 12:37 `1a68446`, 13:34 `5a6173a`, 14:45 `3f1d399` port.ts reference: one shared dev server held by lease, one photo
  round at a time, a hidden window in every child process.
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
- 15:22 Config pointed at the new source; graph rebuilt (79 s, 863 files, 20,318 units) and whole-program Jelly (59 s)
  in `out/city40`.
- 15:26 `aab7600` (slice `5acf06c`) Runtime `cpp/js/js_runtime.h` e regra única `js::<nome>`; Map → js::Map, na ordem de
  inserção do JS. Diferencial do runtime: 6.449 casos iguais ao Node, depois de corrigir Math.round (−0) e o surrogate
  solto. 48/48 testes; lab de 29 para 32.
- 15:35 O usuário pediu este registro em `log/`. O `CLAUDE.md` do projeto proíbe documentos além dele (gate
  `DOC_UNICO`); o arquivo foi criado a pedido explícito do usuário.
- 15:40 O usuário: atualizar este arquivo a cada tarefa e a cada informação nova, com o histórico completo.
- 15:45 `9499eac` (slice `b3d564a`) Feature: an entry that only re-exports (`export * from ...`) counts by the modules it
  re-exports, followed through TypeScript to those that declare something. That was what broke the peer's per-area passes
  in render, world and editor. Tested: 37 declarations, 21 s.
- 15:50 Suíte inteira: 105 testes passando; o cppgraph precisa de `CONVERTER_CLANGD`.
- 15:52 slice `c617bf4` and `564a35d`: the slice had uncommitted copies of the three.js → Godot translator files (DESIGN,
  README, GOAL-LOG, catalogues, `cpp/runtime`, replay, call-tracing and parity scripts). All identical to master's except
  the config, which was updated; committed. Both repositories were left clean.
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
- `3a93b1e`, `e4ec430` and the following commit: query and photo scripts, `--scene` and the `edit` step; the `look.gd` plan accepted, at the
  root-cause session's request.
- `af95221` `openProgram` memoizado: um programa TypeScript por processo.
- Passada inteira `w2`: bateu no limite de 300 s (301 s); o livro-razão dela é parcial (502 módulos vistos, 359 aceitos, 2.609 unidades feitas) e não serve de número. Refeita como `w3`, limite de 590 s.
- By the user's order, back to step 2 of "A fazer": bring into the game what compiles.
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

**Why the tool did not deliver:** it measured what it translated, never what reached the game. The "in the game" counter stayed at 0 the whole time while 3,900 "translated" units had no way in: a second copy of the source program, with no link to the game state (`convert/ledger.ts`, `isDone` in `convert/command.ts`), and no test ran the code inside the game. Measured: of the 36 units of `flowStats` and `controlAdvisor`, the first real use found defects that 114 tests did not see:
- ids como `float` (`translate/flow.ts` `#declare`, `translate/types.ts` chave de `Map`): dois ids acima de 2^24 viravam a mesma chave, em silêncio;
- `a?.get(x)` sem proteção contra nulo (`translate/expressions.ts` `call`);
- ponteiro `const` em objeto alterado; `optional` estreitado não lido como valor; campo estático; construtor implícito tratado como unidade barrada; constante de tuning sem acesso para outro dono.

**Mudança:** essas causas corrigidas no tradutor; `scripts/port-area.ts`; regra nova no `CLAUDE.md` (a lógica pura entra traduzida, regenera-se, nunca se conserta à mão).

**Proof:** `project/tests/core/flow_control_test.gd`: id 33,554,433 kept apart; 148 intersections counted from real vehicles; one intersection of the city became a traffic light.

**Fica por fazer:** o mesmo por área, na ordem do que falta no jogo. `whole` ainda não compila tudo (lista acima).

### 2026-11-05 — A causa da cauda de recusas

**Causa:** o sistema de tipos do tradutor era fechado: todo valor precisava de forma estática em C++, e o que não tinha era recusado (`translate/types.ts` `#unsupported`, `translate/flow.ts` `shapeOf`, `translate/expressions.ts` em `typeof`, `instanceof`, `in`, `??`). A tradução por módulo somava outra: dois módulos escreviam o mesmo arquivo de jeitos diferentes, e tipos com dependência cruzada não têm ordem em cabeçalhos separados.

**Mudança:** `cpp/js/js_value.h` e o tipo `dynamic`, com caixa e descaixa nas fronteiras; o que a plataforma declara fica dinâmico; subclasses, setters, spread em objeto, `WeakMap`, laço sobre `x ?? []`; `--mode whole` com todos os tipos em `core/program_types.h` em ordem de dependência, cabeçalhos numa compilação só, recompilação só de quem inclui o que mudou; tipo que o compilador recusa vira dinâmico, erro sem dono leva só o módulo.

**Prova:** suíte do conversor 116/116, com diferencial contra o TypeScript em `unknown`, `virtual`, setter e spread, e mutação que o check leve pega. Passada inteira: cabeçalhos sem erro, 4 arquivos `.cpp` de fora.

**Limites, sem enfeite:**
- 4,130 units compile; they were not run against their TypeScript source, except the 36 of `flow_control`.
- Passou de 4.580 para 4.130 porque o número antigo contava arquivos que não compilavam.
- 2.554 unidades ficam de fora por rodada e 8.139 são `method` ou `write` (desenho, interface, `hot-allocation`).
- A área já no jogo não foi regenerada depois destas mudanças; o `js_value.h` ainda não está em `native/src/core`.
- Interfaces implementadas por classes (`Sink`, `Path`) e classes que estendem `Map` continuam sem forma.

### 2026-11-05 — Conferência em massa

`scripts/check-all.ts <ledger> <plano> <saída> <fatia> <fatias>`: cada módulo traduzido, chamadas sorteadas na fonte e no C++, cerca de 2 s por módulo, sem jogo. Achou o que nenhum compilador vê, tudo inteiro de 32 bits onde JavaScript não tem: produto, `**`, `>>>` e `Math.round` de real estouravam em silêncio (`flow.ts` `numOf`, `expressions.ts` `#math`). Achou também `x as T` de valor dinâmico perdendo o valor, `=== undefined` igual a `=== null`, e `a?.b?.[i] ?? d` lendo memória lixo.

Resultado de 359 módulos, os que têm função ou classe:
- 84 match their source;
- 7 diferentes, nenhum erro de tradução em entrada válida: `keepCellKey` e `rampCellKey` são a chave do jogo de propósito (padrão da library), `fromFlat` e `segmentDistSq` recebem índice além da lista, `laneChangeOffset` é o limite de `float`, `woodsScale` e `idmAccel` são `NaN` de entrada sem sentido;
- 144 sem nada a chamar (só constantes, ou parâmetros de callback e instância);
- 60 cujo harness não compila, 46 em que a fonte não roda com argumento sorteado, 18 não traduzidos: **sem prova**.

`Math.max` e `Math.min` de reais agora são `js::math_max` e `js::math_min` (ECMA-262: `NaN` passa, +0 acima de -0), conferidos contra o Node no `js-runtime.test.ts`; `std::max` fica para inteiros. `number` é `float`, por decisão do jogo.

`suite.py` guarda o log da falha sob carga em `<cena>_<renderizador>.loaded`: a repetição sozinha escrevia por cima e a prova sumia. Dois instáveis de uma rodada com outra sessão na máquina (`lab_test` forward_plus, `camera_input_test` gl_compatibility) ficaram sem causa provada por isso.

Causa lida no código, sem repetir a falha: `camera_input_test` esperava o fim do deslize por 1,2 s de relógio (agora `_glide_settled`, por `rig.motion.moving()`), e `lab_test` exigia o efeito de uma só janela de GPU (agora mede de novo até uma janela limpa, `CLEAN_TRIES`, porque com a placa dividida o painel responde "dentro do ruído" com razão).

`tools/converter-slice/out` (91 GB de fotos de replay, ignorado pelo git) saiu de `tools/` para `city-worktrees/converter-slice-out`, sem apagar nada: o gate completo o percorria.

Gate completo, agora que termina: 14.327 problemas. 14.310 estão em `tools/process`, `tools/oracle-4.0`, `tools/converter`, `tools/ports` e `log/`, pastas que entraram no repositório sem entrar nas regras (`rules.json`, do usuário). No jogo restam 16: 10 `DADOS` (chaves novas sem schema), 6 `SEGUNDO_DONO` e `VALOR_DUPLICADO` por chaves novas de `agents.json` (`far_shadow`). O tradutor mantém em maiúsculas uma constante estática (`staticName`), que o gate lê como nome.


## 2026-11-07: tasks, one config resolver, deterministic random (English)
- Every child process of the converter is a `tools/tasks` task (`src/common/tasks.ts`, a client of `cli.py serve`): Jelly,
  graph builds, compiles (the quarantine's content key is the task key; the in-memory map is gone), differential builds and
  runs, sandbox steps (independent steps continue after a failure; an empty verification fails), parity and frame replay
  (hidden, with deadlines), catalog check, probe battery. `cppgraph/session.ts` fails on an index timeout instead of going
  on with a partial index.
- One resolver of the machine file (`src/common/machine.ts`, `${}` expansion, built-ins `${converter}` and `${repo}`;
  Python tools reach it through `scripts/machine_config.py`); plans hold only `${game}/...` values; `--target-graph` is
  derived (`src/common/derived.ts`). Cause removed: tests and plans depended on env vars, on the deleted `lab` worktree and
  on another session's temp file.
- `Math.random` becomes a `js::Random &rng` parameter (`cpp/js/js_random.h`); the differential seeds mulberry32 identically
  on both sides. The uncommitted `random_device` version is deleted.
- `tools/process` imports fixed (11), `gates_dispatch.py` deleted (its hooks never existed), paths to
  `C:/Projects/Roadcraft-windows` resolved from the machine config in `tools/process` and `tools/ports`;
  `tools/build/rename_english.py` maps restored, Python modules renamed through `ast`, `--check` verifies imports.
- Verification: `node --test "test/**/*.test.ts"` 136/136 pass with no env var.
