---
name: corrigir-na-causa
description: >-
  Processo obrigatório para qualquer tarefa de código — corrigir defeito, portar funcionalidade
  do Roadcraft 4.0 pelo conversor, investigar problema visual ou funcional, medir desempenho,
  implementar, refatorar ou alterar código, e coordenar sessões filhas de porte. Use SEMPRE antes
  de tocar em código: analisar e medir até a causa raiz, pesquisar documentação oficial e fontes
  primárias até dominar o método, planejar a solução completa, codificar sem tentativa e erro,
  testar, fotografar e analisar cada foto com muita atenção. Também use quando o usuário disser
  corrigir, consertar, portar, investigar, coordenar, ver o problema, não está funcionando, está
  feio, está lento, tirei foto e está errado.
allowed-tools:
  - Read
  - Glob
  - Grep
  - WebFetch
  - WebSearch
  - Bash
  - Edit
  - Write
  - PowerShell
  - ListAgents
  - SendMessage
disallowed-tools:
  - Agent
  - Workflow
---

# Corrigir na causa — processo obrigatório

Estas regras valem para toda tarefa de código, sem exceção. Não são sugestões nem preferências: são a forma de trabalhar. Se você se pegar fazendo algo que contradiz qualquer linha abaixo, pare e volte à fase correspondente.

## Leis

1. **Causa, nunca efeito.** Toda correção ataca a causa raiz. Ajustar número para esconder sintoma, criar caso especial para o exemplo que apareceu ou mascarar o erro: proibido.
2. **Nunca a solução barata.** Escolha a solução completa e correta, mesmo que seja a mais trabalhosa ou a mais cara. O custo só se mede para caber no orçamento; nunca para escolher uma solução pior.
3. **Problema achado é problema resolvido.** Nunca adie, ignore ou esconda um problema — nem os que aparecerem durante a codificação, nem os que a sua mudança criar. Se apareceu algo que você ainda não sabe resolver, pare e pesquise. Nunca devolva ao usuário uma lista de problemas sem a solução: todo problema chega com a correção feita.
4. **Sem codificar no escuro.** É proibido começar a codificar sem saber exatamente o que fazer e como fazer. Primeiro entender; depois pesquisar; depois planejar; só então codificar.
5. **Sem tentativa e erro.** Se uma tentativa falhar, não tente outra variação: volte à documentação oficial e às fontes primárias até entender o método correto. Tentar de novo sem entender é violação.
6. **Sem subagentes dentro da sessão.** Nunca use Agent, Task ou Workflow: o trabalho da sua sessão é seu — ler, pesquisar, entender, planejar, codificar, testar, fotografar, analisar. Isto não proíbe a coordenação: a sessão coordenadora distribui funcionalidades a sessões filhas (conversas separadas, uma por clone em `ports/<feature>/workspace`) e conversa com elas por `ListAgents`/`SendMessage`. Cada filha, por sua vez, é uma sessão única e cumpre esta lei.
7. **Sem afirmação sem prova.** Nada de "deve funcionar" ou "provavelmente". Só declare concluído com gates, build, testes, medição e fotos analisadas. Diga também o que não foi verificado.

O projeto não é complicado nem demorado. O que o torna difícil é começar sem compreensão. Compreender antes é o que economiza tempo.

As ações normais do processo já estão pré-autorizadas pelo usuário: ler, pesquisar, editar, compilar, rodar o jogo oculto, testar, fotografar, commitar no seu branch e, na coordenadora, mandar mensagem às filhas. Não pare para pedir permissão nem confirmação para elas; execute. Nada de caixas de confirmação nem perguntas de "posso seguir?".

## Onde cada sessão trabalha

- **Raiz do projeto:** `C:\Projects\Roadcraft-windows`. Nela ficam `CLAUDE.md`, `regras-sessoes.md`, `converter/` (o script de porte e os rastreamentos em `converter/out/features/<feature>/`), `ports/<feature>/` (clone, fotos de referência, evidência, pacote), `registro/` e `fonte/` (o jogo, repositório-mãe).
- **Sessão única ou coordenadora:** cwd na raiz do projeto; o jogo é `fonte/`.
- **Sessão filha de porte:** cwd no seu clone `C:\Projects\Roadcraft-windows\ports\<feature>\workspace` (branch `convert/<feature>`, origem em `fonte`). Dentro do clone os caminhos são os do jogo (`project/`, `native/`, `tools/`, `CLAUDE.md`); os documentos da raiz e o rastreamento são lidos por caminho absoluto. A filha edita só o clone; `fonte/` e os outros `ports/*` são só leitura; sem push.
- Quando esta skill diz "o documento do projeto", é o `CLAUDE.md` do repositório onde você edita: `fonte/CLAUDE.md` na sessão única ou coordenadora, `<clone>/CLAUDE.md` na filha.

## Fases, nesta ordem

### Fase 1 — Análise e medição até a causa

- Leia todo o código envolvido: o caminho do defeito, os donos vizinhos, os dados e os testes. Não leia só o arquivo que parece culpado.
- Reproduza o problema de forma controlada. Entenda o que o jogador vê e o que o código faz.
- Meça até a causa raiz: sondas (`render_cost`, `camera_pan`), capturas, CPU, GPU, memória, contagens (triângulos, draw calls, instâncias, bytes). Compare com o estado anterior e com a referência.
- Liste TODOS os problemas encontrados, inclusive os pequenos e os que não fazem parte do pedido original. Nenhum fica de fora.
- Guarde a evidência de antes: números, fotos, comandos. É ela que prova a correção depois.
- Medir é desta fase. Aqui não se codifica a correção.

No jogo: mapa 0 (`--map=0`); rodar pelo `tools/build/hidden_run.ps1 -Command "<linha de comando>" -WorkDir <pasta do seu repositório> -TimeoutS <s>` (o `-WorkDir` é obrigatório na filha: o padrão aponta para `fonte`); uma instância do jogo por sessão; fechar só os processos que você abriu, pelo PID; nunca esperar processo às cegas (use tempo-limite e acompanhamento). Com outras sessões rodando, só valem medições que não dependem da carga da máquina (contagens: triângulos, draw calls, instâncias, bytes; trabalho por tick; simulação por tempo simulado); tempos de quadro, GPU, CPU e `camera_pan` ficam listados no relatório com o comando e o que comparar, para rodar com a máquina quieta.

### Fase 2 — Pesquisa profunda antes de decidir

- Pesquise na internet a documentação oficial e as fontes primárias até entender O QUE deve ser feito e QUAL é a metodologia. Nunca confie só na memória.
- Fontes que valem: documentação oficial do Godot 4.7 e do godot-cpp, Terrain3D, guias de Vulkan/Forward+, SUMO, normas técnicas (NBR, NPIP), fichas de fabricante, artigos originais e o comportamento observado do Roadcraft 4.0.
- Compare as alternativas com medição e com a fonte de cada número. Toda proposta chega com a pesquisa feita, as alternativas comparadas e a citação da fonte.
- Se uma tentativa falhou, esta fase é obrigatória de novo. Não existe "tentar mais uma vez".

### Fase 3 — Compreensão profunda e plano completo

- Escreva o plano em palavras: causa raiz provada, solução completa, arquivos e donos, dados e constantes com fonte, testes e fotos que vão provar.
- A solução é a completa e correta, nunca a mais barata. Sem arquivo-deus, sem funcionalidade espalhada, sem código duplicado.
- Respeite a arquitetura: `core` → `sim` → `render` → `api`; um dono por definição; valores em `project/data/**` com schema; a UI lê `CityStore.snapshot` e despacha comandos.
- Só comece a codificar quando cada passo estiver claro. Se algo não estiver claro, volte à fase 2.

### Fase 4 — Codificação sem testar nem medir

- Implemente o plano inteiro. Não rode testes, não meça, não capture: isso é da fase 1 (antes) e da fase 5 (depois). Ao codificar, garanta por leitura, tipagem e compreensão que tudo está sendo executado corretamente.
- Sem TODO/FIXME, sem implementação pela metade, sem gambiarra. Nenhum problema conhecido fica em aberto.
- Não quebre o que funciona. Se a correção ameaça uma regressão, resolva pela causa antes de seguir.
- Se aparecer um problema não estudado: PARE a codificação e volte à fase 2. Nunca ignore.
- Atualize o documento do projeto na mesma mudança (o `CLAUDE.md` do repositório que você edita: mapa e pendências), como os gates exigem.

### Fase 5 — Teste, fotos e correção

- Rode os gates (`python3 tools/gates/run_gates.py`), compile as DLLs de release e debug (`-j 4`), rode os testes e2e e as capturas.
- Abra o jogo e tire as fotos de depois, nos mesmos ângulos e condições do antes.
- Analise cada foto COM MUITA ATENÇÃO, nunca superficialmente: compare com o antes e com a referência; procure artefatos, cortes, cores erradas, geometria quebrada, iluminação errada e regressões em outras áreas.
- Identifique TODO e qualquer problema — inclusive os que a sua mudança criou — e corrija todos. Depois repita a fase 5 até não sobrar nenhum.
- Só declare concluído quando: gates OK, builds OK, testes OK, fotos analisadas uma a uma, antes/depois comparados e nenhum problema restante.

## Porte de funcionalidades do Roadcraft 4.0 — o script rastreia, você converte

**O objetivo é portar a funcionalidade, não o código.** O 4.0 é referência de comportamento e de visual. O código dele está doente e não é otimizado: são ~200 mil linhas de TypeScript/three.js, com `main.ts` de 207 KB, `doc.ts` de 190 KB, `agents.ts` de 146 KB, `buildingTool.ts` de 112 KB, 79 patches de shader (`onBeforeCompile`), 17 `ShaderMaterial`, 12 passes de pós, dezenas de `Array.shift()` em filas, centenas de `delete` dinâmico, `new Array(n)` holey e 124 `postMessage` copiando dados entre workers. Copiar ou traduzir isso leva a doença para o jogo novo.

**O script de porte (`converter/scripts/port.ts`) não converte nada: ele rastreia a funcionalidade completa.** Roda em `C:\Projects\Roadcraft-windows\converter` com `node scripts/port.ts <comando> ...`. O que ele entrega é a matéria-prima da conversão:
- `extract <feature> <entrada>...` — traça a funcionalidade inteira a partir dos arquivos de entrada (relativos à raiz do 4.0, ex.: `src/editor/poles.ts`): closures, dependências; sai em `converter/out/features/<feature>/` (`feature.md`, `source/`, `boundary.json`, `interface/`). É isso que você lê para entender o que a funcionalidade faz.
- `reference <feature> <plano.json>` — fotos do 4.0 pelo próprio `scripts/look.mjs`, nos mesmos ângulos que o alvo vai usar; copie `converter/configs/look-small-city.json` por funcionalidade e acrescente as fotos que ela precisa; exige o servidor do 4.0 em `http://127.0.0.1:5300/`. Sai em `ports/<feature>/reference/`.
- `workspace <feature>` — clone isolado do jogo em `ports/<feature>/workspace`, branch `convert/<feature>`, já com `godot-cpp`, `build-win`, `build-win-debug` e o import feitos.
- `verify <feature> [passo]...` (gates, build-release, build-debug, import, tests, capture), `audit`, `pack` e `sandbox` — evidência e prova do que você escreveu; não geram a conversão.

**A conversão é sua.** Depois de ler o rastreamento:
1. Entenda a funcionalidade: o que o jogador vê e faz, as regras, os dados, os casos de borda e as constantes.
2. Pesquise a metodologia correta na documentação oficial e nas fontes primárias; compare alternativas com medição.
3. Planeje a implementação na arquitetura atual: `core` → `sim` → `render` → `api`, `CityState` como fonte única, `ChangeLog`, `Simulation` com tick fixo, `JobPool`, MultiMesh, `DataTables` e os gates.
4. Escreva o código no dono certo, otimizado e determinístico; cada constante com medição ou fonte primária. Não copie nomes, estrutura, padrões ou constantes do 4.0; não reproduza as patologias.
5. Verifique com `verify`/`audit`/`sandbox` e compare as fotos do 4.0 e do alvo com atenção.

**Regras do porte:**
- A estrutura do alvo é a autoridade; o código do 4.0 só define comportamento e visual.
- Não reproduza as patologias do 4.0: nada de arquivo-deus, estado espalhado, alocação em loop, um mesh por item, fila com `shift`, cópia para worker, readback síncrono ou GPU sem liberação.
- A funcionalidade portada tem o mesmo comportamento observável e o mesmo visual; a implementação é nativa, determinística, com orçamento e medida.
- Cada constante tem medição ou fonte primária; o valor do 4.0 não é fonte.
- O porte só está pronto quando `verify` e `sandbox` passam e as fotos do 4.0 e do alvo são comparadas com atenção.

Antes de portar, leia a documentação do 4.0 (`CLAUDE.md`, `ARQUITETURA.md`, `MIGRACAO-WEBGPU.md`), o `feature.md` extraído, `converter/README.md` e `converter/DESIGN.md`. O 4.0 é referência aberta: `C:\Users\Jonathan\Documents\Projects\roadcraft-4.0-main` (o conversor usa a cópia em `C:\Users\Jonathan\Documents\Labs\roadcraft-4.0-main`). O `translate` do CLI não é o fluxo do script de porte; o script não converte — a conversão é escrita por você.

## Coordenadora e filhas

A coordenadora tem o cwd na raiz do projeto e, para cada funcionalidade: roda `extract`, `reference` e `workspace`; monta o lote de funcionalidades paralelas pelos `boundary.json`, sem dois clones tocando os mesmos donos (`CityState`, `agents.json`, o mesmo subsistema de `sim/` ou `render/`); envia o prompt de execução à filha aberta no clone (`ListAgents` para achar a sessão pelo caminho do clone, `SendMessage` para enviar); recebe a entrega; verifica diff, gates, builds, testes, fotos antes/depois, ausência de cópia do 4.0 e fonte de cada constante; roda `verify`, `audit`, `pack` e `sandbox`; integra no `fonte` por worktree, resolve conflitos e mantém `fonte/CLAUDE.md` e `registro/` em dia.

As filhas são abertas pelo usuário, uma por clone, no mesmo modo de permissão da coordenadora; a coordenadora envia o prompt e não pede ao usuário para mandar nada a elas. A coordenadora não edita os clones: o que a filha fez errado volta para a filha com a instrução de corrigir. Limite de carga: no máximo 3 filhas compilando ou rodando o jogo ao mesmo tempo, cada uma com `-j 4` e uma instância do jogo.

## Documentos e comandos do projeto

- `C:\Projects\Roadcraft-windows\fonte\CLAUDE.md` (na filha, o `CLAUDE.md` do clone, que na base é o mesmo arquivo) — regras de arquitetura, comandos, mapa do código e pendências. É o documento único do jogo; leia antes de codificar e mantenha em dia.
- `C:\Projects\Roadcraft-windows\regras-sessoes.md` — regras operacionais das sessões, coordenadora e filhas.
- Gates: `python3 tools/gates/run_gates.py` (o hook Stop bloqueia).
- Núcleo: `tools/build/build.sh core` / `core-windows`.
- Testes e capturas: `GODOT=<godot> tools/build/build.sh e2e` e `tools/build/build.sh capture /pasta`.
- Jogo real sem janela: `tools/build/hidden_run.ps1 -Command "<cmd>" -WorkDir <pasta do seu repositório> -TimeoutS <s>`; mapa 0.
- Commit por tema, cada um compilando sozinho.

## Definição de pronto

- [ ] A causa raiz está provada por medição, não por suposição.
- [ ] A solução é a completa e correta, aplicada na causa; não há solução barata nem paliativo.
- [ ] A pesquisa está feita e cada constante ou medida tem fonte.
- [ ] O plano foi cumprido por inteiro; nenhum TODO ou problema conhecido ficou em aberto.
- [ ] Gates, builds de release e debug, testes e capturas passaram.
- [ ] Todas as fotos foram analisadas com atenção, antes e depois comparados, e nenhuma regressão restou.
- [ ] O documento do projeto está atualizado e o commit está por tema.
