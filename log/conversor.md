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

## 2026-11-08 21:00: the converter back, pointed at the lab (the owner's orders of 19:45 to 21:10)
- The owner's order: every port redone from the correct source, the lab (`Documents/Projects/lab/roadcraft`), by a tool that ports
  and translates automatically, verified, never case by case.
- Found: the converter's machine file named as `source` another copy of the game (one of the forbidden paths of
  `.claude/gatekeeper.json`), not the lab. Correction at 21:15: the lab's program graph has exactly the counts recorded for that
  copy on 2026-11-05 (863 files, 20,318 units), so the copy was probably the lab's code of that day; that it was older is not proved
  (the copy cannot be read). The game's roads, verge included, came from the 4.0 port (`oracle-4.0`), made before the converter.
- Decision (routine, recorded): the converter is restored from this repository's own history (`9a056673~1:tools/converter`, the
  commit before B1), not from the archive folder (forbidden). It must live under the repository: its tasks run through
  `tools/tasks`, which resolves declared outputs from the repository's root (a copy in a scratch folder failed with "declared output
  missing"). The machine file now names the lab as `source`.
- Checked: `node --test "test/**/*.test.ts"` 136 of 136 on its fixtures. That proves its rules, not the port: the proof is the lab
  translated whole and each module run against its TypeScript (the mass check), counted next.

## 2026-11-08 21:30: the converter in the repository, its gates clean, two races of tools/tasks fixed
- `feature --entries-from <file>`: the whole lab (863 files) passes the Windows command line's length as `--entry` flags.
- `clangdCache` in the machine file: the game's C++ graph keeps clangd's index outside the target it indexes (the converter
  now lives in the game's tree; `cppgraph/build.ts` refuses a cache inside the project).
- Gates on its 224 files: Portuguese data as Unicode escapes (the source's button text, Unicode test inputs, the old port
  records' words), file headers, `three_runtime.cpp` and the replay and parity scripts split by responsibility (the captured
  values, the stage, the material and the instances in one owner, `scripts/three_values.gd`; the textures in
  `scripts/replay_textures.gd`, which now refuses a captured texture of the wrong size instead of padding it in silence), the
  feature fixture's HTML and CSS made by its test from `test/fixtures/feature-page.ts` (the gates read neither), DESIGN and
  README moved into this record. `node --test "test/**/*.test.ts"` 136 of 136.
- tools/tasks: the converter's suite runs files in parallel, each with a `serve`, and they crashed on two shared things. Run
  folders pruned by two runs at once (one deleting what the other deleted; Windows refusing a rename of a folder another handle
  is in): one run prunes at a time, under an OS file lock (`runs.py` `PRUNE_LOCK`, `locks.FileLock`). `latest.txt` and
  `times.json` rewritten by every run (two replacements of one file at once fail on Windows): the newest run is the folder of
  the latest name (`newest_run`), and each run writes its own times in its own folder, reading every kept run's at its start.
  `tools/tasks/tests/test_runs.py` starts eight runs together over 46 old folders; 40 of 40.
- The first whole translation of the lab: about 4,963 units written and 1,657 left out a round, then the header compile died
  with the first race (the server crashed); run again after the fix.

## 2026-11-08 22:40: the lab's ledger, causes by size
- The ledger of the whole lab (`convert --mode whole`, 20,305 declarations): 11,681 to translate, 5,718 refused as "method" (the
  source does something the ledger judges wrong), 2,906 to write by hand (three.js geometry 1,589, DOM interface 272, code that
  mixes state and commands 533).
- The largest cause is a rule of the converter itself, not a limit of translation: "wrong methodology: hot-allocation" refuses
  1,123 units directly, and thousands more by reach (`Building`, `Vehicle`, `RoadNode`, `TableItems`, `IdSlots`, `CellFiling`,
  `ChangeJournal`, all flagged for hot allocation or scattered writes). Allocation in a hot loop is a cost of JavaScript's
  collector; translated to C++ it is a different question (an allocation the C++ owner pools or reuses), so refusing the unit
  keeps the lab's logic out of the game for a reason that belongs to the source language. To decide next: the ledger translates
  such a unit and the quarantine judges the C++ it gives (a heap allocation in a hot path of the output), instead of refusing it.
- The header compile of the whole translation failed twice on `git ls-files` passing its 120 s deadline (tools/tasks keying).
  The reading of this entry, "other heavy work ran beside it", was wrong: the cause is a deadlock, found in the next entry.

## 2026-11-08 22:38: the header compile's git deadlock; the game's C++ declarations in seconds
- Problem: every whole translation died at its header compile, `converter-syntax-check` BROKEN after 0 s with "(no log)", 12
  times this session; the same `git ls-files -s -z` of the repository takes 0.08 s from a shell.
- Why: (1) the converter's client (`src/common/tasks.ts` `parseResult`) dropped the server's `message`, so a task broken at its
  keying showed no reason; the run's `events.jsonl` had it: `git ... ls-files -s -z timed out after 120 seconds` in
  `inputs.py` `_index_of`, the first thing a fresh `serve` did. (2) The converter talks to `cli.py serve` through a pipe it keeps
  open, sending the next request only after the answer: serve's main thread sits in a read of that pipe. (3) `inputs.git` set no
  stdin, so git inherited that pipe. (4) Git for Windows, at its start, asks the name of a piped stdin when stdout and stderr are
  not a console (`compat/winansi.c` `winansi_init` → `detect_msys_tty(0)` → `NtQueryObject(ObjectNameInformation)`), and that
  query waits while a synchronous read of the pipe is pending (documented by the Cygwin developers for the same call). Git waited
  for serve's read, serve's read for the converter's next request, the converter for git: until git's deadline. `runTasksSync`
  never hung because it closes the pipe (no read pending); the task processes never hung because `win32.JobTree` gives them
  the null device and a handle list.
- Sources: git-for-windows `compat/winansi.c` (raw.githubusercontent.com/git-for-windows/git/main/compat/winansi.c); the Cygwin
  patches on NtQueryObject blocking on a pipe with a pending ReadFile (inbox.sourceware.org/cygwin-patches); Python's
  `subprocess` (stdin=None inherits the parent's handle).
- Fix, one owner: `inputs.git` gives git its own input, the text given or the null device, never the caller's
  (`tools/tasks/inputs.py` `git`). It is the one wrapper of every git call in tools/tasks, so no other git of the tools inherits
  the pipe; the task processes already had the null device. The converter's client keeps the server's `message`/`error` and
  `mustRun` shows it (`src/common/tasks.ts`). Test first: `test_cli.py` `test_serve_answers_while_its_input_stays_open` (serve
  with its input left open, one request) failed with no answer in 60 s before the fix and answers in 0.36 s after it.
- Also in this change: the game's C++ graph for a translation is its declarations only (`cppgraph --scope declarations`,
  `common/derived.ts` `targetGraph` scope "declarations" by default): a translation reads the target's types, fields and names,
  never its edges, and resolving every edge asked clangd a definition of every name over a fresh background index (half an hour
  whenever native/src changed). The files are parsed in parallel batches and each file's symbols are kept by an exact key (the file,
  its closure of quoted includes, clangd and the compilation database: `cppgraph/symbol-cache.ts`), so only what changed is parsed:
  158 s cold, 3.2 s warm, the same graph byte for byte.
- Checked: tools/tasks 41 of 41, the converter 136 of 136, `fast road_commands_test` 4 of 4.
- The whole translation of the lab, run again (22 rounds, about 4 s of translation and 3 to 5 s of header compile each): 20,305
  units, 5,004 translated into 1,660 files, 15,301 open, 2,010 of them left out by the rounds. It stopped at round 22 on two
  header errors no unit was placed on (`program_types.h` `LeanJunction` without `tongues`/`rings`, `reeds_shepp_types.h`), so
  the `.cpp` files were not compiled yet: the 5,004 count units in headers that compile, not code proved.
- The causes of the header errors, by count (round 4, 506 errors): 124 "called object type 'FacadePropKind' is not a function"
  (an enum member written as a call, `FacadePropKind::POT()`: the member's name meets a constant with an accessor of that
  name); 117 "no viable conversion from string to SHELLPARTPARS" (a `const` string made by a template literal has TypeScript's
  literal type, the text itself, and `types.ts` makes every lone string-literal type an enum); 57 + 11 a `js::` built-in
  missing (`object_freeze`, `object_from_entries`); 36 `to_string` with no match; 26 a member missing on a struct. Next, in
  that order, in the translator.

## The converter's design and use (moved here from tools/converter/DESIGN.md and README.md, 2026-11-08)
The project keeps its documents in CLAUDE.md and log/ (gate DOC_UNICO): the converter's own two documents live here now. Its
goal log (GOAL-LOG.md, the three.js -> Godot replay cycles, in Portuguese) is summarised in the history above and kept in
git history at `9a056673~1:tools/converter/GOAL-LOG.md`.

### Converter design: any feature of the source, converted whole, with no decision by hand

The input is a feature: one or more entry points of the source (a file, a symbol). Everything else is decided by
rules from facts of the two programs. A decision the converter cannot make from facts is a defect of the converter,
never a field for a person to fill.

#### 1. One model of each program

- **Source:** the whole TypeScript program, opened once with its own compiler. The graph (units, typed edges,
  external APIs) and the flow of values (every slot, every flow) are built over **all** its files, so every call site,
  every caller and every shared helper is known. Equality comparisons are flows both ways (`s.id !== id` makes `id`
  the kind of `s.id`): integer kinds follow from the whole program, never from names.
- **Target:** the game's C++ graph (clangd), its GDScript, its gate rules (layers, limits, allowed numbers, owners,
  map), its data tables and their schemas.
- The flow is solved with a real worklist (each flow re-runs only when a slot it read changed), so the whole program
  solves in time proportional to its size.

#### 2. The feature, traced whole

From the entry points, the closure over the graph: every unit reached, in every file. Shared code is part of the
closure; it is translated once, into its own owner, and reused by every feature that reaches it (a second feature
finds it already in the target and links to it).

#### 3. Each unit's place in the target, decided by rules

| Decision | Rule |
|---|---|
| Layer | the unit's facts: state, time step, computation → `native/src/sim`; draws (three.js) → `native/src/render` (reads only); interface (DOM) → GDScript in `project/ui` through `CityStore`; tunable values → `project/data` with a schema; GLSL → `.gdshader`; workers → `JobPool` |
| Owner folder | units of one layer grouped by the graph's cohesion (modularization: maximize intra-cluster edges, minimize inter-cluster, TurboMQ), each group a subsystem named after its dominant source module |
| Target type | a source type whose shape (fields and operations) is a target type's (`Vec2 {x, y}` ↔ `V2`) maps to it; otherwise it is translated |
| Target concept | a source function with a candidate in the target of the same signature shape is run against it on generated inputs (differential); equal results → the target's owner is called; otherwise the source's is translated |
| Integer kinds | the solved flow over the whole program |
| Literal names | from the slot or call that receives the literal (`Math.min(20, v)` assigned to `vehicles` → `MAX_VEHICLES`), unique per owner |
| State | the types the feature's edits take and return become a struct held by `CityState`; edits become commands under the exclusive lock; indexes are rebuilt by each edit |
| Registration | layer rule, map line (from the unit's doc comment), state member: written where the gates read them |

#### 4. Translation per layer

- **Simulation C++:** the current translator, extended to every construct of the source: classes (fields, methods,
  constructors, inheritance → structs with methods; dynamic dispatch only where the source overrides), generics
  (templates), enums, closures, Map/Set/typed arrays, async and workers (jobs on `JobPool`), exceptions (error results).
- **Render C++:** a fixed catalog from the three.js API to the game's render owners (BufferGeometry → `MeshBuilder`,
  InstancedMesh → MultiMesh through the RenderingServer, materials → the game's shaders and material tables,
  frustum/LOD → the game's streaming); written once, as the translator's library knowledge.
- **Shaders:** parsed with an existing GLSL parser (not a new one), rewritten to Godot's shading language by the
  rules of Godot's official conversion guide.
- **Interface:** DOM and CSS → Godot Controls built through `theme_builder.gd`, texts through `tr()` with the CSV
  catalog, numbers only from `CityStore.snapshot`, actions through `CityStore.dispatch`.

#### 5. Proof, for every unit

- Unit tests of each rule, each with a mutation shown to be caught.
- Simulation: differential runs against the source (double build exact, float build within precision).
- Render: the same scene captured in the source and in the target, compared with a perceptual metric.
- Target: the package in an isolated clone with the game's gates, builds, tests, capture and soak.

#### 5a. Nothing wrong reaches the machine

Translating syntax carries the source's methods with it, so the decision comes first: the ledger (`src/convert`) reads
the audit and the whole graph, and a unit is translated only if nothing wrong is in it or reached from it. Each run's
output is then judged on its own (translation validation), not trusted. A unit the machine cannot write stays open
with its correct method; it is never reported as converted. Items 4 and 5 of the order of work below are still to do:
until then those units are written by a session. `test/convert.test.ts` holds each decision and each refused form.

#### 6. Order of work

1. Whole-program model: flow with a worklist over all files; equality flows; no name rules.
2. Whole-feature tracing across files; shared code into its own owner once; emission per owner.
3. Automatic decisions: target types by shape, target concepts by differential equivalence, literal names, owner
   folders by cohesion, map lines from doc comments. The spec shrinks to the entry points.
4. Classes, generics, enums, exceptions; async and workers.
5. Render catalog; shaders; interface.
6. The transit feature whole (network, vehicles, tools, drawing, panel) converted with none of the spec's manual
   fields, proved by every check above.

### Converter

Converts features of a TypeScript game (the source) into the architecture of the Godot + C++ game (the target),
and audits code for wrong methodologies. It never writes to the target game: it reads it, and everything it makes
goes to an output folder. Paths are always given on the command line or in a configuration file.

#### Stages

| Command | What it does |
|---|---|
| `graph` | Traces a feature (or the whole source) through the source's own type checker: every declaration it uses, typed edges, external APIs, patterns, fingerprints. |
| `cppgraph` | The same graph for the target's C++, through clangd. |
| `audit` | Runs the methodology catalog over a graph: each finding says what is wrong, the correct methodology, its form in the target, and the primary sources. |
| `classify` | What a feature becomes in the target: roles from facts, mapped dependencies, what still blocks it. |
| `convert` | A feature carried as far as facts allow: a ledger decides every unit, the clean ones are translated with a spec found in the program, each output is judged, and the rest is listed with its correct method. Exit 1 while anything is open. |
| `translate` | Writes a feature as C++ in its owner folder (types, API, edits, queries, indexes); refuses when anything cannot be translated. |
| `differential` | Runs the translation and its source on the same random scenario and compares them (a double build that must agree exactly, and the target's float build). `--hand <native/src/...>` (repeatable) puts C++ written by hand in place of the translation's file of that path: the scenario still calls the API the spec's translation declares, so the hand-written code declares it in the same headers; only the float build runs. |
| `package` | The translation as a conversion package: its files, its registration where the target's gates look, its state member. |
| `sandbox` | Tests a conversion package in an isolated clone of the target: the target's own setup and verification before and after the package; prints the manual integration. |
| `baseline` | The target's own setup and verification on its base alone, which proves a sandbox configuration. |

Run with Node 23.6 or newer: `node src/cli.ts <command> --flag value`.

#### A conversion package

One feature, stored outside the target:

- `manifest.json`: the source commit and the SHA-256 of every source file read, the target's base commit, the
  converter's commit (packaging refuses a converter with uncommitted changes) and the identity of the commits.
- `patches/`: the conversion as a series of commits, one per topic, each compiling alone (`git format-patch`).
- `evidence/`: every log and photo of the sandbox, before and after.

Integration into the target is manual: the sandbox prints the commands (fetch the reviewed branch, a worktree,
the target's usual integration checks). Nothing integrates by itself.

#### Rules the conversion follows

The target's `CLAUDE.md` and gate rules are the authority; these are how the converter applies them.

1. **One owner per feature.** A feature is converted into the subsystem that owns it (one folder), entering the
   target only through the points the architecture designates: its state as a member of `CityState`, its time step
   called by `Simulation::tick`, the player's actions as `sim/commands`, its tunables in `data/**` with a schema read
   by `DataTables`, its drawing in `render/` (read only), its changes announced in the `ChangeLog`. No feature code is
   written into another owner's files.
2. **What another owner must provide is that owner's work.** When a feature needs a capability its neighbour does
   not offer yet, the package names it and is not complete; the converter never writes it into the neighbour, and
   never leaves a declaration without an implementation.
3. **Roles come from each unit's facts, not from its folder.** A source unit that mixes responsibilities (input,
   state, simulation, drawing) is split by responsibility; it is never carried over whole.
4. **Every constant needs a source.** A value moved into `data/**` carries where it came from; the package lists each
   one and is complete only when each has a measurement or a primary source.
5. **The correct methodology is written every time.** Each pattern of the audit catalog is converted to its correct
   form (instancing, reused buffers, a spatial index, a ring buffer, state in its owner), whatever its static rating.
6. **The target's gates judge the result** in the sandbox, with its build, tests and photos.

#### Convert

`node scripts/port.ts convert editor:poles [--apply]` takes a feature of the inventory, traces it and decides each of
its units from facts (`src/convert/ledger.ts`):

| Decision | When |
|---|---|
| `exists` | a port's `cobertura.md` says the game has it |
| `translate` | no finding the translator cannot rewrite, in the unit or in anything it reaches |
| `method` | a wrong methodology in it or reached from it: the catalog's correct form must be written |
| `write` | drawing, interface, class members, parts of shared code: written in the owner, by the method given |

Only `translate` units reach the machine. Each output then passes the quarantine (`src/convert/quarantine.ts`): nothing
the ledger stopped was emitted, none of the catalog's wrong forms is in the C++, the target's limits hold, and it
compiles against the target's headers. What the game owns is in `configs/conversion-library.json`, written once.

Limit: the machine writes pure simulation code only. Classes, drawing, shaders and interface are not translated: they
stay open in `conversao.md` until a session writes them and the port's `cobertura.md` records it. `--apply` writes only
to the feature's clone. Every constant in the accepted files needs its measurement or primary source in
`<ports>/<feature>/constantes.json` (name → source); the command stays at exit 1 until each has one.

#### How a translation decides types

What each value is in C++ has one owner, the flow of values (`src/translate/flow.ts`): every place a value lives is
a slot, every way it moves is a flow, and three facts per slot (integer or real, owned or referenced, which struct
shape) are solved together to a fixpoint. The type mapper and the emitters only read that solution. A spec
(`configs/<feature>.translate.json`) says what the translator may not guess: the owner folder, the state and its
member, which names are integers, the target's types and owned concepts (patterns), the names of literals the
target's gates forbid, and what stays with other owners.

#### Feature

`node src/cli.ts feature --source-root <4.0 root> --entry <file>... --out <dir>` cuts a feature from the whole
program: its own declarations (by dominance: what the program reaches only through it), its slices of shared code
(forward slicing in the program's functions that name it or the state that holds its values), the dictionary
entries, style rules and non-code files it uses, the shared code it uses and where the program plugs it in. What a
call runs comes from the call graph of Jelly (`@cs-au-dk/jelly`, a points-to analysis: `--jelly <package dir>` or
`CONVERTER_JELLY`); style sheets are read with the source's PostCSS (`--postcss`).

Where the platform calls the feature (`src/feature/platform.ts`, `platform` in boundary.json): code no declaration of
the program calls, because a package does. An override of a member a package declares in the class's bases
(`ViewedRoot extends LOD` → `three:LOD.update`, called by three's renderer every frame) or a function stored in or
given to a package's declaration (`mesh.onBeforeRender = f`, `onUpload(f)`); the standard library's in-place
iterations (`Array.map`, `Array.from`...) are not hooks.

##### Checking a trace against the running 4.0

A trace is judged by what the program did, in a copy of the 4.0 (its own dev server never writes into the source):

1. `node scripts/oracle.mjs <url> <out> <playwright package dir>`: Chrome on the copy's dev server, a small city,
   views near the poles; it writes V8's precise coverage, a CPU profile (sampled stacks), every shader given to
   `gl.shaderSource` and every code made by `Function`/`eval`.
2. `node scripts/check-trace.mjs <graph.json> <boundary.json> <out> <node_modules dir>`: each function V8 ran,
   placed in the graph's innermost unit through the dev server's source maps, against the trace: (A) every unit of
   the entry files that ran is in it; (B) every shared unit the feature's frames called is reached from its own units,
   and every way the program entered the feature is known: a hook, or `platform` when three.js called it; (C) every
   three.js function the feature called is among its external APIs; (D) where code from strings came from.

##### The definitive test: complete call graph, verdict, controls

Ground truth by instrumentation, never by sampling, on a copy of the 4.0 (its source never written):

1. `scripts/oracle-instrument.mjs`, a Vite plugin in the copy's config: every function of the game and of three.js
   records entry and exit on a shadow stack, in the page and in every worker (async functions and generators leave it
   at each `await`/`yield`; field and parameter initializers, static blocks, derived constructors and modules awaiting
   at the top are frames too). A frame of a feature's **seed files** (the inventory's `sementes`) raises the feature's
   bit, so the truth never depends on the trace under test. New edges are streamed to the server as they happen (a
   worker the game ends takes nothing with it).
2. `node scripts/oracle-calls.mjs <url> <out> <playwright dir> [seconds]`: a city, views, simulated time; it writes
   every edge caller → callee with the features on the stack, and checks each realm's stack is back at its root.
3. `node scripts/verdict.mjs <graph> <out> <features.json> <features dir>`: PASS or FAIL per feature by four rules —
   R1 every unit of a seed file that ran is in the trace; R2 everything run while the feature was on the stack is in
   it; R3 every entry into the feature from outside is known (a graph edge, a hook, or `platform`); R4 every three.js
   function the feature called is among its external APIs.
4. Controls, which must FAIL: `--strip-platform`, `--drop-own <fraction> --seed <n>` (reports how many removed units
   that ran were caught), and the traces of an older converter.

#### Render: three.js → Godot (shaders) and → Godot C++ (the RenderingServer API)

**Shaders** (`src/render/godot-shader.ts`): a program three linked (vertex and fragment GLSL, chunks and
`onBeforeCompile` edits in) parsed with the ShaderFrog GLSL parser (`@shaderfrog/glsl-parser`, given by `--glsl` or
`CONVERTER_GLSL`) and written as one Godot spatial shader, `unshaded` (three's own lighting, exact). The rules come
from Godot's documentation and engine source and each was measured by the parity test: three's matrices and
attributes are Godot built-ins; `gl_Position` → `POSITION`; `gl_FragCoord` in three's convention (Godot's origin is
top-left and its depth reversed); `gl_FragDepth` → `DEPTH = 1 - d`; the output decoded when three writes display
values; struct uniform arrays one array per field; `sampler2DShadow` → `step(ref, depth)`; `matN` of scalars → of
columns; integer operators and constructors given explicit types (`glsl-types.ts`); helpers receive the built-ins
they read (Godot exposes only TIME, EXPOSURE, IN_SHADOW_PASS to them); the fragment main a helper (no `return` in
`fragment()`); vertex helpers that assign varyings written inline; varyings packed into vec4s when many (Godot's 32
locations include its own); attributes in CUSTOM0–3, INSTANCE_CUSTOM, bones, or a data texture by VERTEX_ID /
INSTANCE_ID; `instanceMatrix` the identity (Godot puts a MultiMesh transform in MODEL_MATRIX).

- `node scripts/oracle.mjs <url> <out> <playwright dir>` also writes `programs.json`: every program the 4.0 linked,
  with the attributes it reads per instance (the GL divisor).
- `node scripts/shaders.ts <programs.json> <out> --glsl <dir>`: each program translated, and a Godot project that
  compiles and draws each one; `node scripts/shaders-verdict.mjs <out>`: Godot's judgment, per program.
- `node scripts/parity.mjs <out> <three build dir> <playwright dir> <glsl dir>`: the same scenes drawn by three.js
  (Chrome) and by Godot from the translated program, with the uniforms and textures three uploaded (every `gl.uniform*`
  and texture upload captured) and the mesh as the translation places its attributes; compared pixel by pixel.

Translator options beyond the defaults: `matrices: 'three'` (three's matrices as uniforms, its clip position
converted at the end of vertex(): exact for any camera), `depth: 'reversed'` (reversedDepthBuffer), `output: 'raw'`
(the values the program wrote, for a render target), `shadowCompare`/`shadowFilter` (the hardware's compare and 2×2
filter, by hand), `fragmentOutput` (one pass per MRT location), `floatBitsSamplers` (32-bit integer textures as float
words, read back with `floatBitsToUint`: Godot 4.7 maps no 32-bit integer texture). Always: three's tone mapping is
its own code (Godot's ACES/AgX are other curves), `dFdy` changes sign (Godot's window y points down), a vector
indexed at run time is read through a comparison helper (Godot's SPIR-V pass refuses OpVectorExtractDynamic).

**A real frame, replayed** — the test that proves translations on the game's own data:

- `node scripts/frame-capture.mjs <url> <out> <playwright dir>`: one frame of the running 4.0, every draw with its GL
  state (programs, every active uniform, samplers with their full state, attributes, elements, target, viewport,
  blend/depth/cull), the buffers and every texture level and layer read back from the GPU (integer textures read two
  independent ways and compared; a refused read is an error, never zeros).
- `node scripts/frame-replay.mjs <capture> <out> <playwright dir> <glsl dir> [--only <draws>] [--probe <expr>]`: each
  draw alone at its own resolution, by WebGL (three's GL commands again: the reference; a GL error voids it) and by
  Godot (`scripts/replay.gd`: the translated program, the mesh as the translation placed its attributes, textures
  through the RenderingDevice in the GPU's formats), compared strictly: *equal* (every pixel within 2/255), *near*
  (≤1 % of pixels differ), *different*, *empty* (both blank: proves nothing, never counted as equal). Depth-only draws
  are judged by their depth (a 24-bit code). `--probe <expr>` makes both engines output an intermediate value of the
  same program (bisects a difference in seconds). The WebGL reference and the translations are cached by content.
- `node scripts/draw-coverage.mjs <capture> <draws...>`: an independent check of a draw's screen coverage (its
  vertices projected in JS), so an *empty* verdict is known to be an off-screen draw.

**C++ port** (`cpp/runtime`, `scripts/cpp-scene.mjs <replay dir>`): every captured draw written as C++ for godot-cpp
(uniforms as typed Godot values, textures and mesh arrays and instances as their data files), compiled with
llvm-mingw against the prebuilt godot-cpp (files in parallel, objects cached by content; generated files at -O0)
into a GDExtension (`ThreeReplay`), and run in Godot; `node scripts/replay-compare.mjs <replay dir> <images>
--same-as <other images>` compares it with WebGL and, byte for byte, with the GDScript replay.

**C++ catalog** (`configs/three-godot-catalog.json`): each three.js class the 4.0's features use → its Godot C++ form (the
RenderingServer calls of godot-cpp) and the rule that carries values (column-major matrices, reversed winding,
MultiMesh rows, linear colors, sampler hints...). `node scripts/catalog-check.mjs <catalog> <features dir> <godot>`
checks it is complete for the features' traces and that every Godot class and method it names exists.

#### Child processes

Every process the converter starts (Jelly, the graph builds, each compile, the differential's builds and runs, the
sandbox steps, Godot in the replay, parity and catalog checks) is a task of the repository's orchestration owner
(`python tools/tasks/cli.py serve`, through `src/common/tasks.ts`): its own deadline (the whole process tree is stopped
past it), no window on the user's desktop when it draws, and a result keyed by the content of what it reads, so it is not
made again while its inputs stay the same.

The GLSL parser runs from a copy whose peggy memo table is a `Map` (`scripts/glsl-parser-fast.cjs <parser.js>`):
the generated object, keyed position × rules, falls into dictionary mode and took 96 % of the time; the 277 programs
translate byte-identically, 4× faster (14.5 s cold on 22 workers).

#### Tests

`node --test "test/**/*.test.ts"`: every tool (TypeScript, Jelly, PostCSS, the GLSL parser, clangd, the C++ compiler) comes
from `configs/roadcraft-machine.json` through `src/common/machine.ts`; an environment variable (`CONVERTER_TYPESCRIPT`,
`CONVERTER_JELLY`, `CONVERTER_POSTCSS`, `CONVERTER_GLSL`, `CONVERTER_CLANGD`, `CONVERTER_CXX`) only overrides one. Type check with
`node scripts/typecheck.ts --typescript <dir> --types <node_modules/@types dir>`.
