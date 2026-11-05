Você é a sessão de porte da funcionalidade interface do Roadcraft 4.0 para o jogo atual (Godot 4.7 + núcleo C++). Trabalhe sozinha até o fim, sem pedir confirmação a ninguém: tudo do processo já está autorizado. Invoque a skill `corrigir-na-causa` e siga o método dela por inteiro. Leia inteiros: `C:/Projects/Roadcraft-windows/regras-sessoes.md`, o `CLAUDE.md` deste clone, `C:/Projects/Roadcraft-windows/converter/README.md`, `C:/Projects/Roadcraft-windows/converter/DESIGN.md`; no 4.0 (`C:/Users/Jonathan/Documents/Projects/roadcraft-4.0-main`): `CLAUDE.md`, `ARQUITETURA.md`, `MIGRACAO-WEBGPU.md`.

## Ambiente pronto, preparado pela coordenadora
- Repositório: `C:/Projects/Roadcraft-windows/ports/interface/workspace`, clone novo e limpo do master a11384a, branch `convert/interface`. Não há trabalho anterior. Edite só este clone; `fonte/`, os outros `ports/*` e o código de `converter/` são só leitura. Sem push.
- Rastreamento da funcionalidade inteira: `C:/Projects/Roadcraft-windows/converter/out/features/interface/` (`feature.md`, `source/`, `boundary.json`, `interface/`). Também `C:/Projects/Roadcraft-windows/converter/out/features/start-menu/` e `camera/`.
- Fotos do 4.0 (28, cidade pequena): `C:/Projects/Roadcraft-windows/ports/referencia/reference/`. Fotos do jogo atual, o "antes" (46): `C:/Projects/Roadcraft-windows/ports/referencia/antes/`.
- `native/build-win` e `native/build-win-debug` estão configurados, sem DLLs nem import ainda. Logo no começo, em segundo plano, compile (`MINGW=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64 JOBS=4 bash tools/build/build.sh core-windows`) e importe (`C:/Users/Jonathan/AppData/Local/Microsoft/WinGet/Links/godot_console.exe --path project --headless --import`), enquanto faz a análise.

## Roteiro obrigatório: o rastreamento
O rastreamento é o roteiro do porte, de ponta a ponta. Não é material de consulta.
1. Leia `feature.md` inteiro, depois `source/` arquivo por arquivo e `boundary.json`.
2. Antes de codificar, escreva `C:/Projects/Roadcraft-windows/ports/interface/cobertura.md`: uma linha por arquivo e por declaração própria de `feature.md`, com o que aquilo faz para o jogador e onde vai morar no jogo.
3. Porte tudo o que está lá e marque cada linha: portado, já existia, ou não se aplica, com o motivo e o dono no jogo. O `src/` do 4.0 só complementa o rastreamento.
4. Se o rastreamento estiver incompleto, complete: em `C:/Projects/Roadcraft-windows/converter`, `node scripts/port.ts extract interface <entradas>` (só escreve em `converter/out`).
A conversão é escrita por você, na arquitetura atual, otimizada. Porta-se a funcionalidade, nunca o código.

## Fotos e jogo
Abrir o jogo e o 4.0 faz parte do porte: as fotos de antes e depois exigem isso, e nada proíbe abrir. Use as fotos prontas quando servirem. Quando faltar um ângulo ou a sua funcionalidade em ação, tire a foto: no jogo por `tools/build/hidden_run.ps1 -Command "<cmd>" -WorkDir C:/Projects/Roadcraft-windows/ports/interface/workspace -TimeoutS <s>`, mapa 0; no 4.0 por `node scripts/port.ts reference interface <plano.json>` em `C:/Projects/Roadcraft-windows/converter`, com plano seu (modelo em `C:/Projects/Roadcraft-windows/registro/look/`). A única restrição: a cidade grande do 4.0 não se abre; use a cidade pequena ou só a funcionalidade. Uma instância por vez, fechada pelo PID.

## Método, nesta ordem
Análise do rastreamento e do jogo atual, com medição; pesquisa profunda na documentação oficial e fontes primárias; plano completo; codificação sem testar nem medir; teste, fotos e análise atenta de cada foto, corrigindo todo problema. Mesmo comportamento e visual do 4.0, implementação nativa na arquitetura (`core` → `sim` → `render` → `api`, `CityState`, `ChangeLog`, `Simulation`, `JobPool`, MultiMesh, `DataTables`), sem as patologias do 4.0, cada constante com medição ou fonte. Nunca solução barata; nunca tentativa e erro; nunca ignorar problema; sem Agent, Task ou Workflow. Outras sessões rodam ao mesmo tempo: valem as regras de carga da seção 5 de `regras-sessoes.md`. Se precisar de capacidade de outro dono, escreva no dono certo e diga no relatório.

## Escopo
HUD, menu inicial, câmera, painéis, gavetas e o painel de gráficos e qualidade (`src/render/quality.ts`, `optimizations*.ts`, `effectSwitches.ts`).

## Entrega
Commits por tema neste branch, cada um compilando; gates OK; builds release e debug; 8 testes; capturas; fotos antes/depois e do 4.0 analisadas uma a uma; `CLAUDE.md` deste clone com mapa e pendências em dia; `cobertura.md` completo. No fim escreva `C:/Projects/Roadcraft-windows/ports/interface/relatorio.md`: o que leu, requisitos, pesquisa com fontes, plano, commits, resultados de gates/builds/testes/capturas, fotos analisadas, constantes com fonte, cobertura do rastreamento, o que não foi verificado (medições que dependem da máquina quieta, com o comando). Sem `cobertura.md` completo a entrega volta. Não integre nada.

## Ordem do usuário: faça tudo você mesma
Não confie no que a coordenadora preparou. Isto vale acima de qualquer outra parte deste prompt.
1. Rode você mesma o script: em `C:/Projects/Roadcraft-windows/converter`, `node scripts/port.ts extract <feature> <entradas>`, escolhendo as entradas pelo `src/` do 4.0.
2. Verifique o código extraído contra o `src/` do 4.0, arquivo por arquivo. O que faltar da funcionalidade, acrescente como entrada e rode de novo, até ela estar inteira.
3. Desconsidere as fotos de `ports/referencia/`. Tire as suas: no 4.0 por `node scripts/port.ts reference <feature> <plano.json>` (servidor ligado em http://127.0.0.1:5300/, cidade pequena) e no jogo pelo `hidden_run.ps1`, nos ângulos que a funcionalidade precisa.
4. Faça a conversão, os testes e a entrega como descrito acima.

## CORREÇÃO DA COORDENADORA (verificada em 12:55 no relatório, no git e no transcrito; a entrega volta)
Verifiquei por mim: gates OK; o padrão gráfico passou de `medium` para `auto` em `data/game/settings.json`; a cobertura tem 1.202 declarações e 589 "não se aplica" marcadas por arquivo e por padrão de nome. Pelo seu `relatorio.md` e pelo seu transcrito, a entrega não cumpre o processo:
1. **Sem pesquisa.** Zero busca e zero página lida; só o dump local da API. Uma afirmação técnica (a máscara da câmera também tira a camada do passe de sombra) veio "da memória" sem abrir o fonte do Godot. O código começou às 12:12:31, 9 minutos e meio depois do início. Isso é codificar sem saber (Lei 4, processo etapa 2).
2. **Sem plano.** Você escreveu às 12:07 que gravaria o plano em arquivo antes de codificar; ele não existe em `ports/interface`.
3. **Leitura parcial.** Vários arquivos do rastreamento foram lidos com `cut -c1-170/260` e `head`, e seis nem foram lidos (`chunkedSave`, `saveStore`, `saveBinary`, `profileEditor`, `junctionPanel`, `crossSection`), além de `main.ts`, `history.ts`, `persistence.ts`, `cameraGestures.ts`, `cookedCities.ts` e `inspector.ts` lidos só em parte.
4. **Cobertura por padrão de nome.** O prompt manda uma linha por declaração, com o que faz para o jogador e onde mora, marcada depois de ler. Marcar 589 como "não se aplica" por regex não é isso.
5. **Constantes sem fonte.** O relatório admite que a maioria dos números de `settings.json` não tem medição própria. A regra é medição ou fonte primária em cada uma.
6. **Um commit de 100 arquivos.** O processo pede commits por tema, cada um compilando.
7. **GPU.** O padrão `auto` vira `ultra` numa RTX 3060, sem prova de GPU abaixo de 100 %.
8. **Muito do escopo não feito** (lista no seu relatório) e suíte completa não reexecutada depois das últimas mudanças.

### O que fazer, nesta ordem. Não passe à demolition antes de terminar tudo isto.
A. **Pesquisa em `ports/interface/pesquisa.md`**, com URL e a conclusão de cada fonte: documentação oficial do Godot 4.7 (Viewport, escala 3D e FSR, SSAO, SMAA, filtragem anisotrópica, `RenderingServer.viewport_get_measured_render_time_gpu`, Logger/OS.add_logger, camadas de câmera e cull mask), o fonte do Godot em `renderer_scene_cull.cpp` aberto de verdade (a tag da versão usada) para a questão da sombra, e a fonte primária de cada constante (343 m/s, 50 ms de tarefa longa, janela do governador, limites de GPU). Se uma fonte não existir, a constante sai de dados medidos por você.
B. **Leitura completa**, sem `cut` nem `head`, de tudo o que você leu em parte ou não leu, no `source/` do rastreamento, e do que faltar do `src/` do 4.0 que o rastreamento aponta.
C. **Plano em `ports/interface/plano.md`:** para cada item da sua lista "O que não foi feito" que é desta funcionalidade, a solução, o dono, os dados, o teste e a foto que prova. O que você disser que é de outra sessão, prove pelo escopo do prompt dela em `ports/<feature>/prompt.md`, não por suposição.
D. **Cobertura refeita declaração a declaração**, com motivo verdadeiro em cada "não se aplica". Cada "portado" aponta o arquivo e a função do jogo.
E. **Código** de tudo o que é da interface: os campos de nível sem efeito (vegetação, grama, detalhe de pedestres, ocupantes, teto de veículos, pássaros, chuva, névoa, sombras de nuvem), as chaves de efeito ausentes, as teclas, a troca de idioma ao vivo no HUD inteiro, a edição no inspetor, abrir mapa externo, o que o seu relatório listou. Ligue o `Thunder` onde puder; se o raio é da landscape, deixe o ponto de entrada pronto e testado.
F. **Padrão gráfico:** ou provar com medição de GPU (a máquina está carregada: então medir por contagem e por `viewport_get_measured_render_time_gpu` em amostras, com o comando para a máquina quieta) ou voltar `default` para o que não estoura. Não deixe `ultra` sem prova.
G. **Commits por tema:** `git reset --soft a11384a` e recomite em temas, cada um compilando sozinho.
H. **Teste de novo, tudo, depois da última mudança:** gates, release e debug, os testes, as capturas, as fotos analisadas uma a uma contra o 4.0.
I. **Novo `relatorio.md`** com a pesquisa, o plano, a cobertura, as constantes com fonte e só o que de fato não foi verificado.
