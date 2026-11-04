# Tarefa

Construa um extrator que, a partir de uma funcionalidade do jogo Roadcraft 4.0 (TypeScript/three.js), entregue **todo o código e tudo o mais de que ela depende**, sem corte, sem amostragem e sem limite de tamanho, num dossiê que outra IA leia para entender a funcionalidade por inteiro e reescrevê-la em outra arquitetura (Godot 4.7 + C++).

O extrator não converte nada. Ele só encontra, organiza e prova que não faltou nada. O resultado é julgado pela seção "Prova de completude", não pela sua opinião sobre ele.

# Ambiente

- 4.0 (só leitura): `C:\Users\Jonathan\Documents\Labs\roadcraft-4.0-main`, commit `22357c92` com mudanças não commitadas. A cópia em `Documents\Projects` tem o mesmo `src/`. Vite, three 0.186, Electron, Vitest, Playwright. Comece por `CLAUDE.md` e `ARQUITETURA.md`.
- Tamanho: 728 arquivos e 195.006 linhas em `src/` (sem `src/lab` e `src/peopleLab`), 21 arquivos `*.worker.ts`, 708 arquivos em `tests/`, 61 scripts, nenhum arquivo de shader (o GLSL está em strings em 41 arquivos TS).
- Extrator atual: `C:\Projects\Roadcraft-windows\converter`, repositório git próprio. Leia `README.md`, `DESIGN.md`, `scripts/port.ts`, `src/graph/*`, `src/feature/*` e `configs/roadcraft-machine.json`. Uma tentativa anterior, com fecho por símbolos, está em `C:\Projects\Roadcraft-windows\ferramentas\extrair_funcionalidade.cjs`.
- Saídas atuais: `converter/out/features/<feature>/`. Inventário: `registro/levantamento.md` e `registro/fila.md`, na raiz do projeto.

# Por que o atual não serve (medido em 2026-11-04; confira antes de confiar)

1. **O código compartilhado sai sem corpo.** `boundary.ts` separa por dominância: só é "próprio" o que o programa alcança apenas pela funcionalidade. O resto vai para `interface/` como assinatura ("bodies left out"). Exemplos, com "arquivos compartilhados" contando o arquivo inteiro de cada declaração usada:

   | Funcionalidade | Linhas em `source/` | Linhas nos arquivos compartilhados que ela usa |
   |---|---|---|
   | transit | 2.293 | 17.502 |
   | poles | 2.039 | 12.699 |
   | economy | 374 | 5.227 |
   | traffic | 38.679 | 43.724 (136 arquivos, 900 declarações usadas) |

2. **Recortes perdem o contexto.** De `createSceneRenderer`, com 3.774 linhas, a funcionalidade `barriers` recebe 36.
3. **O resultado depende de uma lista de entradas feita à mão.** `traffic` precisou de 73 arquivos de entrada. Os buracos foram achados à mão depois: pontes e túneis (`structures`, `levels`), demanda por hora, `wholeMap`, `elements`, conquistas da Steam.
4. **O Jelly não vê os workers.** Ele roda só sobre `src/main.ts` e `src/peopleLab/main.ts`. Das 728 entradas do call graph, nenhuma é `*.worker.ts`, só 34 estão em `sim/` (que tem 88 arquivos `.ts`) e `sim/host/simHost.ts` está ausente. O grafo de tipos alcança os 21 workers, mas os recursos e valores que dependem do Jelly ficam cegos desse lado. Só 51,8 % dos call sites têm alvo; parte do resto é chamada de biblioteca, e essa parte não foi separada: meça.
5. **Vínculos dinâmicos não têm tratamento de primeira classe.** Fora dos labs há 117 `postMessage`, 86 tratadores de mensagem, 98 `addEventListener`, 47 `requestAnimationFrame`/timers, 151 usos de `onBeforeRender`/`onBeforeCompile`/`customProgramCacheKey`, 36 `switch` por `type`/`kind`, 15 `obj[key]()`, 23 `import()` e 26 `fetch`. Dos 1.824 textos da interface, 1.032 são montados em execução e foram conferidos só pelo prefixo.
6. **Ficam de fora o que define o comportamento:** os testes, os goldens, os scripts de cozimento (`scripts/cook-*`) e o patch `scripts/patches/clipper2-ts+2.0.1-18.patch`.

# O que "funcionalidade completa" significa

Recorte por dominância não é aceito como definição. O dossiê inclui, com o texto íntegro:

- **Fecho para frente:** tudo o que ela executa ou lê, inclusive código compartilhado, até as bordas do projeto, com corpo completo.
- **Fecho para trás:** quem a dispara, da raiz do programa até ela (cadeias completas), e quem escreve cada campo, variável ou buffer que ela lê, até o ponto fixo.
- **Efeitos:** quem lê o que ela escreve.
- **Limites de execução:** página ↔ workers (protocolo por tipo de mensagem, nos dois sentidos), eventos do DOM, teclado e mouse, agendadores, callbacks da plataforma e do three.js, despacho por string, tabelas de funções, `import()`.
- **Shaders:** o GLSL final, depois de `onBeforeCompile`, `#include` e substituições, mais quem escreve cada uniform e atributo.
- **Dados e recursos:** tabelas, constantes, JSON, `lots/`, `public/`, `cooked/`, CSS (inclusive classes montadas), textos e chaves montadas (resolva o conjunto real de valores, não o prefixo), chaves de save e `localStorage`, atalhos, opções de qualidade.
- **Origem do que é gerado:** o script que produz cada arquivo cozido, e os patches de dependência.
- **Especificação executável:** testes, fixtures e goldens que a exercitam.
- **Bibliotecas externas:** toda API usada, com versão e contagem. Corpo das bibliotecas só com a opção `--with-deps`.

Se o fecho der quase o programa inteiro, isso é resultado a relatar, nunca motivo para cortar. A relevância se expressa por camada e evidência, sem remover nada.

# Método exigido

Combine os três; nenhum basta sozinho:

1. **Estático sólido:** compilador do TypeScript sobre o programa inteiro, com o tsconfig e os aliases do projeto. Sobre-aproxime onde não houver prova (todos os candidatos de um despacho, todas as implementações), marcando como sobre-aproximado. Regex não é a análise principal. Rode o Jelly com **todos** os pontos de entrada reais, workers incluídos. Modele a mensagem do worker como aresta: quem envia o tipo X liga a quem trata X.
2. **Dinâmico:** cobertura V8 precisa, por cenário, na página **e em cada worker**, mapeada ao TS por source maps; e cobertura por teste no Vitest. Compare cenário com a funcionalidade contra cenário de base sem ela (software reconnaissance, Wilde & Scully, 1995). Capture em execução o GLSL final, os alvos reais de despacho e os textos montados. O 4.0 já tem base para isso: `scripts/look.mjs`, Playwright, e o censo com GPU nula de `npm run verify`.
3. **Fechamento:** todo achado dinâmico que o estático não previu é defeito do estático. Corrija a regra que falhou, não acrescente o caso à mão.

Pesquise antes de implementar, nas fontes primárias: API do compilador TypeScript, documentação e código do Jelly, Chrome DevTools Protocol (`Profiler`, alvos de worker), Vite (workers, `?url`, source maps), Vitest coverage, three.js. Literatura de partida: Dit et al. 2013 (feature location), Weiser (slicing), Livshits et al. 2015 (soundiness). Cite o que usar.

# Entrada

Uma funcionalidade por nome e **uma semente basta**: arquivo, símbolo, chave de texto, id de botão ou ferramenta, título de teste ou cenário. O extrator expande sozinho até o ponto fixo. A lista manual de 73 arquivos deixa de ser necessária. Sementes extras nunca podem reduzir o resultado.

# Saída: `converter/out/dossies/<feature>/`

- `LEIA-PRIMEIRO.md`: o que o jogador vê e faz, derivado de fatos (botões, teclas, textos, títulos de teste, comentários de documentação), estado, ciclo por quadro e por tick, ordem de leitura, totais e o que não foi resolvido.
- `codigo/`: **texto íntegro** de cada unidade, agrupado por arquivo, com caminho, linhas e hash. Função gigante entra inteira, com as linhas relevantes marcadas. Marcação nunca substitui o texto.
- `camadas.json`: cada unidade com sua camada (exclusiva; compartilhada executada; ligação; escritores e leitores; infraestrutura), sua evidência (estática, dinâmica, teste, sobre-aproximada) e **por que está aqui**: o caminho de arestas desde a semente.
- `fluxos.md`: da entrada do jogador ao pixel, passando por workers; protocolo de mensagens; ordem de execução.
- `dados/`, `shaders/`, `textos/`, `testes/`, `gerados/`: cópias íntegras, com origem.
- `pendencias.json`: todo site dinâmico não resolvido, com arquivo, linha, candidatos e motivo.
- `manifesto.json`: commit, estado sujo, versões das ferramentas, hash de cada arquivo, totais.

O leitor é uma IA com janela de contexto limitada, então divida em arquivos e índices navegáveis. Divisão é apresentação: a soma das partes é o todo, sem resumo no lugar de código.

# Prova de completude (critérios de aceite)

Rode em `traffic`, `roads`, `build`, `transit`, `poles` e `economy`, e entregue os números de cada uma:

1. **Autossuficiente:** o recorte, sobre uma cópia do 4.0 com todo o resto removido, passa no typecheck sem nenhum nome, módulo ou membro faltando.
2. **Executável:** os testes da funcionalidade rodam sobre o recorte com o mesmo resultado que no programa inteiro.
3. **Dinâmico contido:** 100 % das funções executadas pela diferença entre cenários, na página e nos workers, estão no dossiê.
4. **Buracos históricos:** partindo de uma semente que não os cita, o extrator acha sozinho `structures`/`levels` em `road-elevation`, demanda por hora em `traffic`, `wholeMap` em `roads`, `elements` em `landscape` e conquistas da Steam em `interface`.
5. **Mutação por canal:** fixture com uma funcionalidade sintética que usa cada canal (mensagem de worker, despacho por string, evento, patch de shader, texto montado, classe CSS, arquivo de dados, arquivo cozido). Desligar a regra de um canal tem de reprovar um teste.
6. **Integridade:** todo bloco confere por hash com a origem; nenhuma reticência ou truncamento.
7. **Nada calado:** cada site dinâmico do programa aparece como resolvido, sobre-aproximado ou pendente. Site sem conta encerra com código de erro. Declare o total de cada grupo.
8. **Determinismo:** duas rodadas no mesmo estado geram bytes iguais.
9. **Comparação:** por funcionalidade, o que entrou e o que saiu em relação a `out/features`. Toda saída é justificada.

Completude absoluta em JavaScript dinâmico não é demonstrável por análise estática. Não afirme isso. Afirme exatamente o que os critérios provam e liste o resto em `pendencias.json`. Se um critério for impossível, prove por quê e entregue o limite mais próximo; nunca o enfraqueça em silêncio.

# Restrições

- Nunca escreva no 4.0 nem em `C:\Projects\Roadcraft-windows\fonte`. Rode em cópia própria tudo o que precise alterar arquivos.
- Há sessões de porte em andamento lendo `converter/out/features`: não altere essa pasta, e mantenha `port.ts extract` funcionando até a migração. Dependem do formato atual, ao menos, `pack --feature-dir` e `ports/traffic/evidence/cobertura_gerar.py` (lê `boundary.json`). Acrescente o comando novo em `scripts/port.ts`.
- O servidor do 4.0 na porta 5300 é compartilhado por concessão (`SERVER_DIR` em `port.ts`). Use esse mecanismo; nunca derrube o servidor nem processos que não são seus.
- A máquina está carregada: nunca espere processo às cegas, use tempo-limite, e faça cache da análise do programa inteiro por estado da fonte, como o `programCache` atual.
- Sem IA dentro do extrator: tudo determinístico e derivado de fatos.
- Siga o estilo do conversor: TypeScript tipado rodando direto no Node ≥ 23.6, um módulo por responsabilidade, código e comentários em inglês, testes em `test/`.
- Sem número mágico, sem TODO, sem caso especial para funcionalidade ou arquivo com nome fixo. Regra por fato, não por nome.
- Commits por tema num branch próprio do repositório `converter`, sem push e sem merge.

# Entrega

Antes de codificar, escreva em `converter/DESIGN-EXTRACAO.md` o que mediu, o modelo de arestas, como cada canal é resolvido e como cada critério será provado. Depois implemente tudo, sem deixar parte para depois.

No fim, informe: comandos para reproduzir; tabela dos nove critérios por funcionalidade, com a evidência; totais de `pendencias.json`; tempo e memória; e o que não foi verificado. Critério reprovado é reportado como reprovado.
