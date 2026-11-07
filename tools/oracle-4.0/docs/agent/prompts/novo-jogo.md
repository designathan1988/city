# Prompt: o Roadcraft na base do roadcraft-novo, com a qualidade visual e as funcionalidades do Roadcraft atual

O trabalho é feito na pasta `C:\Users\Jonathan\Documents\Projects\lab`, que tem os dois projetos:

- `lab\roadcraft-novo\roadcraft-novo`: **a base, onde o trabalho é feito.** É o roadcraft-novo ("Roadcraft 5").
- `lab\roadcraft`: **o Roadcraft atual, só para leitura.** Dele vêm a qualidade visual e as funcionalidades.

O objetivo:
- um jogo com **a mesma qualidade visual do Roadcraft atual**, perto e longe, de dia e de noite;
- **todas as funcionalidades** que prestam;
- **desempenho altíssimo com a qualidade gráfica no máximo**, numa cidade de 10 km com mais de 200 mil prédios.

O código já existe nos dois projetos. Não há razão para demora: leia, decida e faça.

Duas proibições valem acima de tudo:
1. **Não traga do Roadcraft atual o código de otimização sujo dele** (a lista está na seção "O que nunca vem do Roadcraft atual"). É o código que deixou aquele jogo pesado, e ele quebraria a base.
2. **Antes de trazer qualquer coisa, prove que a base sustenta o jogo inteiro.** A arquitetura dela tem de ser a correta e estar sem erros. O dono não quer descobrir depois que ela não aguenta as funcionalidades e a qualidade gráfica.

## Por que esta base (confira cada ponto no código; nada aqui é prova)

- No roadcraft-novo, **cada prédio é um registro pequeno numa tabela de colunas** (`src/core/table.ts`, `src/world/world.ts`): uns 90 bytes por prédio. No Roadcraft atual são 2,75 KB por prédio, ou 687 MB para 249.122 prédios (`src/editor/squaresDecode.ts:98-100`).
- O prédio detalhado é gerado no worker a partir do registro e só existe enquanto é montado (`src/jobs/building40.ts:1-6`).
- O save é feito por ladrilho, em binário, comprimido, com SHA-256 e versão (`src/jobs/storage.ts`).
- Há uma grade só de células e ladrilhos, e cada edição marca só as células alteradas (`src/world/world.ts:188-208`).
- **O que nunca foi provado:**
  - a escala: a cidade de 10 km dele tem 38 mil prédios (`docs/ESTADO.md`);
  - os desvios que trabalham sobre o mapa inteiro;
  - os 293 defeitos de 19/10 (16 críticos), em `lab\roadcraft-novo\DEFEITOS-roadcraft-novo.md`;
  - as correções que o ramo `conserto/criticos` diz ter feito: 345 commits que nunca foram juntados ao `main`.

## O critério de "funciona", decidido antes de qualquer código

Só uma coisa prova que o jogo funciona:

1. **Uma cidade nova de 10 km com semente aleatória**, criada no jogo do jeito que o jogador cria, com **mais de 200 mil prédios de verdade**: a cidade de teste da fase 2.
   - O tamanho é conferido pelo código (o tamanho do mapa e o raio pedidos) e por uma foto da cidade inteira, com o número de prédios dito.
2. **Salva, com o jogo fechado e reaberto, sem cache nenhum**: na primeira vez depois de instalar e na primeira vez depois de uma versão nova do jogo.
3. **Jogada na janela real e visível**, com a qualidade gráfica no máximo:
   - câmera de perto e de longe, e arrastar;
   - uma edição de via e uma de prédio;
   - a noite.
4. **Medida uma vez por marco**, na janela real, com só esse jogo aberto: a RAM de todos os processos somados, a CPU, a GPU, o tempo de abertura, o tempo de criação, o tempo de save e o tempo de quadro.

Os limites (preenchidos pelo dono; os valores entre parênteses valem se ele não mudar):
- RAM de todos os processos somados, com a cidade de 10 km aberta: [[RAM (6 GB)]]
- Criar uma cidade nova de 10 km: [[TEMPO DE CRIAÇÃO (60 s)]]
- Abrir a cidade salva de 10 km, sem cache: [[TEMPO DE ABERTURA (30 s)]]
- Tempo de quadro p95 numa RTX 3060, na qualidade máxima, de perto e de longe, em 1920×1080 e em 2560×1009 (a janela do dono): [[QUADRO (16,7 ms)]]
- CPU com o jogo parado: [[CPU PARADO (baixa: nenhum trabalho sem mudança)]]

**Nunca vale como prova:**
- uma semente fixa ou cozida, como a semente 7 do Roadcraft atual;
- uma cidade lida de arquivos feitos antes, de cache ou de fixture;
- uma cidade menor ou menos densa chamada de 10 km;
- qualidade reduzida, efeito desligado ou distância de desenho cortada;
- uma janela coberta ou minimizada, onde o navegador desenha cerca de 1 quadro por segundo;
- as verificações automáticas da base sozinhas: o "146 de 146" do `verificar.mjs` passava com defeitos críticos no jogo (G1 da lista de defeitos);
- qualquer número do Roadcraft atual: ele serve só como referência visual e de funcionalidades.

Um resultado pesado, lento ou que cai é uma falha, dita com os números. Nunca é "pendência", "quase", "ambiente" ou "cache".

## Fase 0: preparar (curta)

1. **Confira que as duas cópias em `lab` estão inteiras.** Compare cada uma com a original:
   - `lab\roadcraft` com `C:\Users\Jonathan\Documents\Projects\city\roadcraft`;
   - `lab\roadcraft-novo\roadcraft-novo` com `C:\Users\Jonathan\Documents\Projects\Apps\roadcraft\roadcraft-novo\roadcraft-novo`.

   Use `git status`, `git log -1` e o número de arquivos de `src/`. Se uma cópia estiver incompleta, diga qual e pare: é um bloqueio. As originais nunca são escritas.
2. **Na base, crie um ramo novo a partir de `conserto/criticos`** (3e67e9e). Não mexa nos outros ramos nem nos worktrees antigos (`area/*`, `claude/*`, `wt-*`).
3. **Crie a memória que resiste à compactação**, como manda o Prompt 1 de `lab\roadcraft\docs\agent\prompts\prompts-genericos.md`: as ordens do dono gravadas palavra por palavra, o estado, o caderno da tarefa e os hooks SessionStart. Reescreva o `docs/ESTADO.md` da base, parado em 18/10, com o estado verdadeiro.
4. **Em `.claude/settings.json` da base** (https://code.claude.com/docs/en/permissions, https://code.claude.com/docs/en/hooks):
   - `"permissions": { "deny": ["Agent"] }`: sem subagentes;
   - nas mesmas regras `deny`, `"Edit(//c/Users/Jonathan/Documents/Projects/lab/roadcraft/**)"`, `"Edit(//c/Users/Jonathan/Documents/Projects/city/**)"` e `"Edit(//c/Users/Jonathan/Documents/Projects/Apps/**)"`: o Roadcraft atual e as originais nunca são escritos. No Windows, o caminho é escrito na forma POSIX, `C:\` vira `/c/` (https://code.claude.com/docs/en/permissions, seção sobre caminhos);
   - um hook `Stop` que sai com código 2 enquanto o `npm run typecheck` falhar ou enquanto uma lista da fase em andamento tiver um `[ ]`.
5. **Reescreva o `CLAUDE.md` da base:**
   - curto (menos de 200 linhas), sem regras que se contradigam;
   - sem "Como o trabalho se divide": agora é uma sessão só, sem subagentes;
   - tudo no projeto em inglês, inclusive commits e documentos;
   - "pronto" só com o critério de aceitação;
   - nunca uma frase que faça um atalho parecer o jeito de testar.

## Fase 1: provar a arquitetura da base, lendo

Leia cada arquivo inteiro, em blocos (Read com offset e limit) até a última linha, com a lista de arquivos e `[ ]` na frente. Busca por trechos não substitui a leitura.

1. **Leia a base inteira:**
   - os documentos: `CLAUDE.md`, `PROMPT-ANALISE.md`, `PROMPT-CONTINUAR.md` e todo `docs/` (`ARQUITETURA.md`, `ESTADO.md`, `LICOES.md`, `RECURSOS.md`, `PARIDADE-roadcraft-3.md`, as pesquisas e os `historico-*`);
   - `lab\roadcraft-novo\DEFEITOS-roadcraft-novo.md` (390 linhas) e `lab\roadcraft-novo\PLANO-SONDA.md`;
   - todo o código: `src/` (cerca de 66 mil linhas, `src/r40/` incluído), `electron/`, `tools/`, `test/` e a configuração.
2. **Escreva `docs/base-check.md` com:**
   - **cada afirmação de `docs/ARQUITETURA.md` conferida no código**: feita (arquivo:linha), em parte ou não feita. A lista de defeitos já mostrou que a documentação dava como feito o que não estava (G5).
   - **cada um dos 293 defeitos**: corrigido no ramo (o commit e a prova no código), aberto (arquivo:linha) ou não confirmado. Mensagem de commit não é prova.
   - **todo trabalho que cresce com o tamanho do mapa**, por quadro, por edição, por passo da simulação, na abertura ou no save, com arquivo:linha. Pelo menos estes:
     - `CellLayer.update` visita todas as placas e ladrilhos a cada quadro (E3);
     - `Traffic.measureRegions` percorre todos os trechos a cada 4 s;
     - a cidade inteira desenhada nas 3 cascatas de sombra (E4), redesenhada todo quadro com tudo parado (E19);
     - o gerador roda 6 vezes por célula (F4);
     - todas as poses de veículos percorridas a cada quadro (C7) e todas as paradas a cada passo (C50);
     - o jogo desenha todo quadro, mesmo minimizado (E20);
     - capacidades fixas que cortam em silêncio: o conjunto de objetos de rua (E1: a 24 km faltavam 73% das peças), os pedestres (C48), os emissores (E40), os abrigos (E41).
   - **`src/r40/`, arquivo por arquivo.** É uma cópia antiga do gerador de prédios do Roadcraft atual (`procedural.ts` tem as mesmas 705 linhas; o `buildingMesh.ts` dela tem 4.119 contra 4.719 no atual). Ela trouxe junto arquivos de infraestrutura do jogo antigo (`render/cellKeeps.ts`, `candidateKeep.ts`, `activeTiles.ts`, `world/derived.ts`, `world/changes.ts`). Para cada arquivo, diga quem o chama a partir de `jobs/building40.ts`, e se é lógica de geração (fica) ou infraestrutura velha (sai).
   - **o custo de gerar um prédio pelo caminho da 4.0 no worker**, contado pelo código: o que cada `emitChunk` monta. Diga quantos prédios por segundo os workers precisam fazer para criar a cidade de 200 mil e para refazer as células que entram na vista ao arrastar a câmera.
3. **O orçamento com a carga visual inteira**, sem medir de novo. Use os relatórios de contagem que já existem no Roadcraft atual:
   - `lab\roadcraft\reports\model-audit`, `model-catalog`, `model-catalog-all`, `geometry-catalog` e `polygon-geometry-audit.md`: os triângulos e vértices de cada modelo em cada nível;
   - os achados de desenho em `git -C lab\roadcraft show 35b819b4:docs/agent/findings.md`.

   Projete para a cidade de 200 mil prédios, no sistema de células e níveis da base: as chamadas de desenho, os triângulos na tela, a memória da GPU e a RAM. Considere o limite do three r186 sem multi-draw-indirect (uma chamada por célula visível, `docs/ARQUITETURA.md`).
4. **Pesquise só o que decide, abrindo as páginas** (WebFetch) e citando o link e a versão:
   - quais GPUs da pesquisa de hardware da Steam têm WebGPU no Chromium do Electron 44. A base exige WebGPU, e a documentação dela diz que o WebGL2 perde o contexto na cidade cheia;
   - o three.js r186 com WebGPURenderer (https://threejs.org/manual/en/webgpurenderer.html) e com desenho indireto (https://toji.dev/webgpu-best-practices/indirect-draws);
   - o teto do heap do V8 por processo (https://www.electronjs.org/blog/v8-memory-cage);
   - para o resto, abra a documentação oficial da versão exata no momento de cada decisão.
5. **O veredito, escrito no fim de `base-check.md`:** a base sustenta o jogo inteiro dentro dos limites, sim ou não, com os números e a prova.
   - **Se não sustenta**, diga exatamente o que da arquitetura precisa mudar e mude primeiro, na base, antes de trazer qualquer coisa do Roadcraft atual.
   - **Se a mudança necessária for refazer o núcleo**, pare e mostre isso ao dono, com a prova. É a única parada planejada.

## Fase 2: a cidade de teste, bonita, em quadras

O dono quer uma cidade bonita para testar: muitos prédios e casas, em quarteirões de grade, e não a cidade bagunçada que a base abre hoje. Ela é a mesma da cidade grande do Roadcraft atual, feita do jeito certo na base:

1. **O plano é o do Roadcraft atual** (`lab\roadcraft\src\world\cityGen\plan.ts`, estilo `'blocks'`, inteiro; e `largeCity.ts`, `squares.ts`, `streets.ts`, `field.ts`):
   - quadras de 120 × 72 m em grade reta (`BLOCK_LONG`, `BLOCK_SHORT`);
   - uma avenida a cada 4 quadras e uma coletora entre elas (`AVENUE_EVERY`), e a borda como avenida;
   - dois centros;
   - zonas pela curva de renda (`zoneAt`): torres e comércio nos centros, prédios médios no meio, casas na borda, indústria num setor da borda, comércio ao longo das vias principais;
   - praças por bairro (`squares.ts`);
   - **toda quadra construída**, nenhuma deixada como grama.
2. **O tamanho:** o mapa de 10 ladrilhos da base (10.240 m) e o raio de 5.200 m. A grade cobre 1,7 raio, cerca de 8,85 km. O comentário de `largeCity.ts` dá cerca de 200 mil prédios nessa escala; a cidade salva do Roadcraft atual tinha 249.122.
3. **Os lotes e os prédios:** cada quadra dividida em lotes como no Roadcraft atual (`src/world/growth/lotPlan.ts` e o resto de `src/world/growth/`), e cada lote com o prédio do gerador do Roadcraft atual (`src/world/buildings/`). Use o caminho que a base já tem para isso (`jobs/building40.ts` e `src/r40/`), atualizado para o código do Roadcraft atual onde ele melhorou.
4. **Feita do jeito da base:**
   - o plano e os lotes calculados nos workers, por ladrilho, em paralelo;
   - os prédios guardados como registros pequenos;
   - nenhuma etapa que monte a cidade inteira na página ou a copie inteira entre processos;
   - mesma semente e mesmo código dão a mesma cidade;
   - a cidade é crescida na hora; nada cozido nem lido de cache.
5. **As provas, abertas e analisadas:**
   - uma foto da cidade inteira contra o mapa;
   - uma foto de um centro, de um bairro de prédios médios e de uma rua de casas;
   - o número de prédios dito;
   - as mesmas vistas da cidade grande do Roadcraft atual, lado a lado.

   A grade, as avenidas, as praças e a distribuição de torres, prédios e casas têm de ser as mesmas.

## Fase 3: o marco zero

Rode o critério de aceitação na base, com a cidade de teste, antes de trazer qualquer funcionalidade ou visual. É uma medição, uma vez.
- Diga a cidade (tamanho, prédios, semente, origem), os números contra os limites e as fotos com o caminho de cada uma.
- Se um limite não for atingido, ache a causa lendo o código e corrija na base. Depois rode o critério uma vez para confirmar.
- Não passe para a fase 4 com a base fora dos limites.

## Fase 4: o inventário do Roadcraft atual

1. **Leia o Roadcraft atual inteiro:** `src/`, `desktop/`, `scripts/`, `tests/`, a configuração e os documentos citados na fase 5.
2. **Escreva `docs/reference/features.md`**, uma linha por funcionalidade e por elemento visual:
   - as ferramentas, os menus, os painéis e os atalhos;
   - os geradores: cidade, vias, cruzamentos, lotes, prédios, fachadas, telhados, interiores, vegetação, mobiliário, postes e fios, sinais, pessoas, veículos;
   - a simulação: tráfego, pessoas, economia, zonas, transporte, serviços;
   - os materiais, as texturas, os modelos, a luz, as sombras, o céu, a noite, a água, o terreno e o pós-processamento;
   - as opções gráficas, o save, o desfazer e a Steam.

   Para cada linha:
   - onde está em cada projeto (arquivo:linha) e o que o jogador vê;
   - se funciona, com a prova;
   - **a parte que presta**: a lógica, as regras, os dados, os modelos, a aparência do shader;
   - **a parte suja**: a infraestrutura de otimização dela, com arquivo:linha;
   - a decisão: já está na base e igual, está na base com diferença, trazer, ou deixar de fora (só o que está quebrado ou sem uso nos dois, com a prova).

   Use o `docs/RECURSOS.md` e o `docs/PARIDADE-roadcraft-3.md` da base como ponto de partida, conferidos no código.
3. **As fotos de referência visual**, tiradas uma vez no Roadcraft atual:
   - como rodar: `npm run dev` na porta 5300 e `npm run desktop:dev`, com as fotos por `node scripts/inGame.mjs shot`;
   - a cidade: "Nova metrópole" (2,7 km). Ela serve só para ver a aparência, nunca para números;
   - as vistas, cada uma com a hora e a posição da câmera escritas, para repetir as mesmas na base:
     - uma fachada a 10 m;
     - um quarteirão a 40 m;
     - uma rua com cruzamento a 15 m;
     - um bairro a 150 m;
     - um distrito a 600 m;
     - a cidade a 3 km;
     - a cidade inteira;
     - uma rua à noite;
     - a cidade à noite.

## O que nunca vem do Roadcraft atual

**Pode vir**, refeito sobre os registros, as células, os workers e o vértice compacto da base:
- a lógica e os dados: os geradores (`src/world/buildings`, `src/world/growth`, `src/world/cityGen`, `src/world/junction`, `src/world/roads`);
- as regras da simulação;
- os modelos, as texturas e os materiais;
- a aparência dos shaders (como a fachada, a via e a noite são pintadas).

**Nunca vem** (o que deixou aquele jogo pesado):
- o documento do mapa inteiro na página e o histórico sobre ele: `src/world/doc.ts`, `src/editor/history.ts`;
- o save por quadrados e a abertura sobre o mapa inteiro: `src/editor/squaresDecode.ts`, `saveLayers.ts`, `saveStore.ts`, `chunkedSave.ts`, `persistence.ts`;
- a cidade cozida, a cidade crescida de antemão e a montagem inteira entre processos: `cooked/`, `src/editor/cookedCities.ts`, `preparedCity.ts`, `cityInTiles.ts`, `cityTiles.ts`, `smallCityMap.ts`, e o `cityAhead` de `src/main.ts`;
- o cache derivado, apagado inteiro a cada versão: `src/core/derivedCache.ts`, `src/render/derivedCache.ts`, `derivedWriter.worker.ts`, `derivedWriterLane.ts`, `ecologyKept.ts`;
- as camadas de guardar, adiar e reenviar: `cellKeeps.ts`, `candidateKeep.ts`, `uploads.ts`, `stagedTexels.ts`, `shaderCatalogue.ts` e as peças de aquecimento, `revealGate.ts`, `stillTree.ts`, `pinned.ts`;
- as camadas de prédios do jogo antigo: `src/render/buildings/cellPool.ts`, `cellRequests.ts`, `massCells.ts`, `partCells.ts`, `farParts.ts`, `layerCells.ts`, `gpuBatchedMesh.ts`, `townIndex.ts`, `townColumns.ts`, `frameBudget.ts`, `partWarm.ts`;
- as camadas de objetos e postes: `propCells.ts`, `propLevels.ts`, `poleCells.ts`, `poleStores.ts`, `cellInstances.ts`, `clearingCells.ts`, `structureCells.ts`, `plantCells.ts`;
- o renderer de 4.355 linhas na thread da página (`src/render/renderer.ts`) e o `src/main.ts` de 4.360 linhas;
- as ferramentas de diagnóstico que ficam ligadas: `src/lab/`, `src/core/profiler.ts`, `health.ts`, `gpuTier.ts`, `perfBudget.ts`, `src/render/gpuLedger.ts`, `gpuTimer.ts`, `memoryCounters.ts`, `optimizations*.ts`.

Antes de trazer um arquivo que não está em nenhuma das duas listas, leia-o inteiro e decida pela regra: lógica, dados e aparência podem vir; o que guarda, adia, copia ou percorre o mapa inteiro não vem.

## Fase 5: a história de erros (lida antes de trazer qualquer coisa)

Leia:
- `lab\roadcraft\CLAUDE.md`;
- a memória do dono: `C:\Users\Jonathan\.claude\projects\C--Users-Jonathan-Documents-Projects-city-roadcraft\memory\` (`MEMORY.md` e cada arquivo);
- em `lab\roadcraft`, com `git show 35b819b4:<caminho>`: `docs/agent/findings.md` (1.111 linhas), `MISSION.md`, `history.md`, `open-checked.md` e `docs/agent/memory/`;
- em `lab\roadcraft`: `reports/architecture-2026-10-29/`, `.whole-review/FINDINGS.md`, `reports/otimizacao/rejeitado/AUDITORIA.md` e `REGISTRO.md`, `reports/why-10km-cities-fail-2026-10-26.md`, `reports/measure-2026-10-27/README.md`;
- o registro de problemas: `%APPDATA%\Roadcraft\problems\problems.log`;
- a tentativa abandonada `C:\Users\Jonathan\Documents\Projects\city\roadcraft-next`: `README.md`, `docs/MISSION.md`, `docs/PROGRESS.md` e o `git log`. Diga por que ela parou.

Escreva `docs/reference/lessons.md`: cada erro, em que projeto, a prova, o efeito e a regra que o torna impossível. Parta da lista "Os erros que não podem se repetir".

## Fase 6: trazer, item por item

Na ordem do que o jogador mais vê: prédios e fachadas, vias e cruzamentos, árvores e vegetação, mobiliário, postes e sinais, carros, pessoas, luz, sombra, céu e noite, e depois o resto de `features.md`. Para cada item:

1. Leia de novo o código dele no Roadcraft atual.
2. Traga a parte que presta, refeita do jeito da base: um material por camada, o vértice compacto e a malha feita no worker. Nada da parte suja.
3. **Prove a aparência:**
   - tire as mesmas vistas da fase 4 na base, com a mesma hora e a mesma posição de câmera;
   - abra as duas fotos e compare lado a lado;
   - o elemento tem de estar lá, com a mesma forma, o mesmo material e o mesmo detalhe de perto;
   - de longe, ele tem de parecer o de perto (a cor média dentro de CIEDE2000 2), sem cor fixada à mão;
   - nunca caixa, plano, billboard ou impostor no lugar de prédio ou via.
4. Verifique com o type check e o teste do ponto, e commite sozinho, com mensagem em inglês.
5. A cada grupo de itens (um marco), rode o critério de aceitação uma vez. Se passar de um limite, ache a causa lendo e corrija antes do próximo item.

## Os erros que não podem se repetir

[atual] é o Roadcraft atual; [base] é o roadcraft-novo. As provas estão nos documentos das fases 1 e 5. Confira cada linha antes de usá-la.

### Arquitetura
1. [atual] O mapa inteiro como um documento de objetos na página, trocado inteiro ao abrir (`doc.ts:155`, `history.ts:190, 194`), e a abertura marcando "o mapa inteiro mudou" (`doc.ts:2285-2290`).
2. [atual] Streaming por ladrilhos escrito e nunca ligado (`doc.ts:1704-2041`; o commit 5de45028 tirou a fonte).
3. [atual] Prédios guardados montados: 687 MB para 249.122 prédios.
4. [atual] Cidade nova montada inteira e copiada inteira entre processos (`cityInTiles.ts:20, 50, 54`). Uma cidade de 2,7 km chegou a 10,1 GB em 31 s.
5. [atual] O renderer de 4.355 linhas na thread da página, o nível de longe das vias pedido para o mapa inteiro e todo emissor de sombra percorrido a cada quadro (`renderer.ts:451, 1241-1244, 744-862`). [base] O mesmo risco em `game.ts` (2.806 linhas).
6. [atual] A simulação com a sua própria cópia do mapa inteiro (`simClient.ts:430`, `nodeSimHost.ts:4`).
7. [atual] Save e abertura sobre o mapa inteiro: as vias viram JSON numa string só (`doc.ts:2822-2906`), e ler o save de 10 km levava 27 s.
8. [atual] O cache apagado inteiro a cada versão (`derivedCache.ts:366`). Abria em 24,1 s com o cache e em 145,9 s sem ele, e o cache escondia o caminho lento.
9. [atual] Lentidão escondida em vez de resolvida: cidade cozida e cidade crescida de antemão.
10. [atual] Um save que sumiu: a tabela `sessions` de `roadcraft-saves` desapareceu com a cidade de 249.122 prédios, e as pastas de saves foram trocadas por backups durante testes.
11. [atual] Toda edição de via recalcula as alturas do mapa inteiro (`editRules.ts:66`).
12. [atual] Trabalho repetido a cada quadro (`findings.md` do commit 35b819b4):
    - shaders compilados no meio do movimento;
    - paletas de todos os lotes de pessoas, visíveis ou não;
    - buffers refeitos quando a janela alterna entre 1079 e 1080 px;
    - a textura do BatchedMesh reenviada a cada desenho;
    - atributos de 3 componentes que o ANGLE converte na CPU;
    - buffers reenviados com o mesmo conteúdo;
    - geometria refeita com a vista parada;
    - matrizes de objetos parados recalculadas;
    - o catálogo inteiro de pessoas carregado;
    - 7 texturas pintadas na abertura.
13. [atual] Diagnóstico sempre ligado (`profiler.ts:75`, `health.ts:153`) e uma RTX x060 começando no nível ultra (`gpuTier.ts:9`).
14. [base] O trabalho que cresce com o mapa ou se repete sem mudança: E3, E4, E19, E20, F4, C7, C50, E31 e `measureRegions`.
15. [base] Capacidades fixas que cortam a cidade em silêncio: E1, C48, E40, E41.
16. [base] Malhas trocadas nunca liberadas (E6) e memória de vídeo vazando a cada mudança de opção (E7).

### Tentativas que falharam
17. A fila de envio à GPU com teto de bytes por quadro.
18. A cena congelada (`matrixAutoUpdate` desligado para tudo).
19. O banco de dados do grafo de código, que quebrou o jogo (revertido em 7d424a23).
20. Os passos 2, 3, 4 e 6 do plano de desenho guiado pela GPU, impossíveis em three r186 com WebGL (`history.md` do 35b819b4).
21. As rodadas de modelos rejeitadas: caixas no lugar de objetos, nível médio de planta que muda a copa, pneus facetados, leques de agulhas nas vias.
22. HLOD assado, atlas ou impostor para prédios, que são gerados durante o jogo e editados pelo jogador. [base] Prédios de longe como "caixas lisas de cor chapada".
23. Uma interface "nova" que só recoloria a antiga.
24. Compensação de cor no shader para o longe bater com o perto.

### Fraude e prova falsa
25. [atual] A semente 7 cozida apresentada como o jogo funcionando.
26. [atual] Uma cidade de 2,7 km chamada de 10 km. [base] Uma cidade de 10 km com 38 mil prédios chamada de cidade cheia.
27. "É o cache", "é o ambiente" ou "é o driver" sem verificação.
28. Medição em janela coberta ou minimizada.
29. "Pronto" sem abrir o jogo, ou abrindo a cidade errada.
30. [base] "Pronto" falso nos documentos (G4, G5, C20).
31. [base] Verificações automáticas usadas como prova enquanto o save não abria (G1), e fotos padrão apontadas para um rio (G2).
32. Testes enfraquecidos, métricas fixadas à mão, funcionalidades desligadas, qualidade reduzida.
33. Caixas, planos, billboards ou impostores no lugar de prédios e vias; cenas montadas para passar numa checagem.
34. Armadilhas para a próxima sessão escritas na documentação.

### Processo
35. Remendo em cima de arquitetura errada.
36. Largura em vez de profundidade. [base] 430 commits em cerca de 13 horas, em sessões paralelas.
37. Várias sessões, contas e agentes na mesma pasta ao mesmo tempo.
38. Commits restaurados sem abrir o jogo depois.
39. Medições, bisects e suítes lentas para achar a causa, em vez de ler o código.
40. Subagentes que travaram ou fraudaram.
41. Ferramentas de teste gravando no save do jogador (A38), e o código do save editado com a janela viva salvando por cima.
42. O estado perdido na compactação.
43. Tarefas misturadas, e prompts e memórias que se contradiziam.

## Regras

- Ache cada causa lendo o código e a documentação. Nada de medir, benchmark, sonda ou "mudar e rodar para ver". O jogo é medido só no marco zero e uma vez por marco.
- Antes de cada decisão, abra a documentação oficial da versão exata usada e cite o link.
- Uma sessão só, sem subagentes. Se aparecerem mudanças que não são suas, pare e avise.
- Decida sozinho e não pare. As únicas paradas são: uma cópia incompleta (fase 0), a base precisar de um núcleo novo (fase 1) e um bloqueio que só o dono resolve. Em qualquer delas, diga o que bloqueia, com a prova.
- Nunca diga que leu, testou, verificou ou corrigiu o que não fez. Toda afirmação sobre a cidade diz o tamanho, o número de prédios, a semente, a origem e o commit.
- Tudo no projeto em inglês. Com o dono, português do Brasil.
- Commite cada passo verificado. Não publique em lugar nenhum enquanto o dono não disser onde.

## Ao fim de cada fase e de cada marco, informe

- o que foi lido (arquivos e linhas, X de Y) e o que ficou sem ler, e por quê;
- o que foi decidido, com as fontes;
- o que foi feito, com os commits;
- no marco zero e em cada marco, a rodada de aceitação: a cidade (tamanho, prédios, semente, origem), os números contra os limites e as fotos da base e do Roadcraft atual lado a lado, com o caminho de cada uma;
- o que não foi feito ou não passou, dito como falha;
- o que não foi verificado.
