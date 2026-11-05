# O que eu preciso de você

Escreva o código de um extrator de funcionalidades para o jogo Roadcraft 4.0 (TypeScript/three.js). Dada uma funcionalidade, ele deve juntar todo o código e todos os dados dela, com corpo completo, sem limite de tamanho, para que outra IA entenda a funcionalidade por inteiro e a reescreva em Godot + C++.

**Como vamos trabalhar:** você não tem acesso à minha máquina e não precisa ter. Sua parte é escrever o código com base nos cinco arquivos anexados. A minha parte é rodar aqui e devolver a saída real. Repetimos até funcionar.

Por isso:
- Não diga que algo funciona, foi verificado ou passou. Você não rodou nada.
- Use só caminhos, nomes e APIs que apareçam nos anexos. Não suponha nenhum outro.
- Se faltar um fato, não adivinhe: escreva em "Preciso saber" o comando exato para eu rodar, e siga com o resto.
- Leia os anexos inteiros antes de escrever.

# Anexos

| Arquivo | Conteúdo |
|---|---|
| `01-arvore-do-4.0.txt` | Todos os arquivos reais do `src/` com linhas; `tests/`, `scripts/` e pastas de dados |
| `02-canais-dinamicos-reais.txt` | Toda ocorrência real de worker, mensagem, evento, timer, hook do three.js, GLSL, despacho, `import()`, `fetch`, texto e classe montados, save |
| `03-exemplos-reais-do-4.0.txt` | Arquivos completos: configuração, worker da simulação, protocolo, `simClient`, `jobPool`, i18n, `look.mjs`, `cook-cities.mjs` |
| `04-extrator-atual.txt` | Código e documentos do extrator atual: `src/graph`, `src/feature`, `src/common`, `scripts/port.ts`, `src/cli.ts`, configuração da máquina |
| `05-funcionalidades-e-grafo.txt` | Funcionalidades atuais, entradas usadas por cada uma, totais e amostras do grafo |

# Por que o extrator atual não basta

Medido nesta máquina:

1. **Código compartilhado sai só como assinatura.** `transit` tem 2.293 linhas extraídas e usa declarações de arquivos compartilhados que somam 17.502 linhas.
2. **Depende de lista manual de entradas.** `traffic` usa 73 arquivos, e ainda assim faltaram coisas achadas depois à mão.
3. **Jelly não analisa os workers.** Ele parte só de `src/main.ts` e `src/peopleLab/main.ts`. Nenhum dos 21 `*.worker.ts` está no call graph, e `sim/host/simHost.ts` também não.
4. **Testes, scripts de cozimento e o patch de dependência ficam de fora.**

**A armadilha principal:** seguir todas as arestas não resolve. No grafo atual, seguir leitura e tipo a partir de `poles`, `transit` ou `barriers` alcança 647 arquivos e 175.457 das 194.422 linhas. Somando quem chama e incluindo arquivos inteiros, as cinco funcionalidades testadas chegaram a 722 de 726 arquivos, 100 % das linhas. Um dossiê igual ao programa inteiro não serve para nada.

# O que construir

Reaproveite o que existe: `buildGraph`, o IR de `src/graph/ir.ts`, `port.ts`, cache por estado da fonte. Estenda em vez de reescrever o que funciona.

1. **Unidade = declaração.** Método, campo e função interna são unidades, como já são no IR. 41 % das unidades são aninhadas. Nunca inclua um arquivo inteiro só porque uma declaração dele foi tocada.
2. **Sensibilidade a campo.** Ler `doc.barriers` não pode puxar tudo o que toca `doc`. É isso que evita a explosão.
3. **Fecho em camadas, sem apagar nada do que foi alcançado:**
   - núcleo: o que só a funcionalidade usa;
   - executado: o que ela de fato chama, com corpo completo;
   - dados: quem escreve o que ela lê e quem lê o que ela escreve;
   - ligação: a cadeia da raiz do programa até ela;
   - alcançável: o resto, só listado, com o caminho de arestas.
4. **Canais dinâmicos, nas formas que aparecem em `02`:** `new Worker(new URL(...))`, `x.onmessage =`, mensagens casadas pelo discriminante real do protocolo, o sistema de jobs de `jobPool.ts`, eventos, timers, hooks do three.js, `t()` e classes montadas, `fetch` e `?url`.
5. **Todos os pontos de entrada** no Jelly, incluindo os workers.
6. **Cobertura em execução:** um executor Playwright + CDP (`Profiler.startPreciseCoverage`) com cobertura na página e em cada worker, mapeada ao TS por source map. Compare um cenário usando a funcionalidade com um cenário de base. Use `scripts/look.mjs` como base de como o 4.0 é dirigido e o servidor em `http://127.0.0.1:5300/`. Registre também o GLSL final capturado em execução, sem tentar simular `#include` por texto.
7. **Testes:** ligue cada teste de `tests/` às unidades que ele exercita.
8. **Semente:** uma só basta (arquivo, símbolo, chave de texto ou título de teste), sem tabela de funcionalidades escrita no código.
9. **Tudo o que não é função, com texto íntegro e origem:**
   - shaders: quem escreve cada uniform e cada atributo;
   - dados: tabelas, constantes, JSON, `lots/`, `public/`, `cooked/`, CSS;
   - textos e classes: o conjunto real de valores das chaves montadas, não o prefixo;
   - chaves de save e de `localStorage`, atalhos de teclado, opções de qualidade;
   - arquivos gerados: o script que produz cada um (`scripts/cook-*.mjs`);
   - testes: fixtures e goldens junto dos testes;
   - dependências: patches de `scripts/patches/`, e toda API externa usada, com versão e contagem. O corpo das bibliotecas só entra com `--with-deps`.

# Saída do extrator: `converter/out/dossies/<feature>/`

- `LEIA-PRIMEIRO.md`: o que o jogador vê e faz (botões, teclas, textos, títulos de teste), estado, ordem de leitura, totais e pendências.
- `codigo/`: texto íntegro de cada unidade das quatro primeiras camadas, com caminho, linhas e hash.
- `camadas.json`: unidade, camada, evidência e o caminho de arestas desde a semente.
- `fluxos.md`, `dados/`, `shaders/`, `textos/`, `testes/`, `gerados/`, `manifesto.json`.
- `pendencias.json`: todo site dinâmico do `02` que não foi resolvido, com candidatos e motivo.

Sem `slice`, limite ou resumo em nenhuma saída.

# Verificadores

Escreva também um comando que eu rode aqui. Cada verificador tem de poder reprovar, e cada um vem com um teste mostrando uma entrada ruim sendo reprovada:

1. Recorte por declaração, com o resto apagado, passa no typecheck.
2. Todos os testes ligados à funcionalidade dão o mesmo resultado, teste a teste, no recorte e no programa inteiro.
3. Toda função da diferença de cobertura está no dossiê. Conjunto vazio é reprovação, não 100 %.
4. Partindo de uma semente que não os cita, o dossiê contém as declarações reais destes cinco casos, conferidas pelo nome da declaração, nunca por substring de arquivo:
   - `road-elevation`: `src/world/structures.ts` e `src/world/junction/levels.ts`;
   - `traffic`: `atHour` em `src/sim/meso/demand.ts` e `busy` em `src/sim/ambient/demand.ts`;
   - `roads`: `src/world/cityGen/wholeMap.ts`;
   - `landscape`: `src/world/elements.ts`;
   - `interface`: `createSteamAchievements` em `src/steamAchievements.ts`.
5. Desligar a regra de um canal reprova um teste. Atenção: a aresta genérica por identificador não pode manter o alvo vivo e esconder a falha.
6. Todo bloco confere por hash com a origem.
7. A contagem de sites vem de uma enumeração independente da que os resolve.
8. Duas rodadas em processos separados geram bytes iguais.
9. Fração do programa em cada camada, por funcionalidade. Núcleo e executado de `economy` e `traffic` têm de ser diferentes, e dossiê igual ao programa reprova.

# Regras do código

- TypeScript tipado, rodando direto no Node ≥ 23.6, um módulo por responsabilidade, código e comentários em inglês, no estilo dos anexos.
- O TypeScript é carregado por caminho, como em `src/graph/typescript.ts`: o `converter` não tem `node_modules`. Caminhos vêm de `configs/roadcraft-machine.json`.
- Comando novo em `scripts/port.ts`, seguindo o `switch` que está lá. `extract` e `out/features` continuam como estão, porque há sessões em andamento usando.
- Nunca escreve no 4.0. Nada de IA dentro do extrator; tudo determinístico.
- Sem caso especial com nome fixo de funcionalidade.
- O servidor do 4.0 na porta 5300 é compartilhado por concessão (`SERVER_DIR` em `port.ts`): use esse mecanismo e nunca o derrube.
- A máquina fica carregada: todo processo lançado tem tempo-limite, e a análise do programa inteiro fica em cache, como em `programCache`.
- O trabalho vai para um branch próprio do `converter`, num worktree, sem push. Não use `git checkout` na pasta principal, porque há sessões usando.

# Formato da resposta

1. **Desenho:** no máximo uma página, com o modelo de arestas e como a sensibilidade a campo evita a explosão.
2. **Arquivos:** caminho completo e conteúdo integral de cada arquivo novo ou alterado, sem "resto igual" nem reticências. Em arquivos existentes, mostre o diff.
3. **Comandos:** a sequência para eu rodar, em ordem.
4. **Preciso saber:** comandos cujas saídas você quer.
5. **Riscos:** onde o código é mais provável de falhar na primeira execução.

Se não couber numa resposta, entregue em partes completas, começando pelo grafo e pelo fecho, e diga o que vem depois.
