Você é a sessão de porte da funcionalidade demolition do Roadcraft 4.0 para o jogo atual (Godot 4.7 + núcleo C++). Trabalhe sozinha até o fim, sem pedir confirmação a ninguém: tudo do processo já está autorizado. Invoque a skill `corrigir-na-causa` e siga o método dela por inteiro. Leia inteiros: `C:/Projects/Roadcraft-windows/session-rules.md`, o `CLAUDE.md` deste clone, `C:/Projects/Roadcraft-windows/converter/README.md`, `C:/Projects/Roadcraft-windows/converter/DESIGN.md`; no 4.0 (`C:/Users/Jonathan/Documents/Projects/roadcraft-4.0-main`): `CLAUDE.md`, `ARQUITETURA.md`, `MIGRACAO-WEBGPU.md`.

## Ambiente pronto, preparado pela coordenadora
- Repositório: `C:/Projects/Roadcraft-windows/ports/demolition/workspace`, clone novo e limpo do master a11384a, branch `convert/demolition`. Não há trabalho anterior. Edite só este clone; `fonte/`, os outros `ports/*` e o código de `converter/` são só leitura. Sem push.
- Rastreamento da funcionalidade inteira: `C:/Projects/Roadcraft-windows/converter/out/features/demolition/` (`feature.md`, `source/`, `boundary.json`, `interface/`). Também `C:/Projects/Roadcraft-windows/converter/out/features/bulldozer/` e `destruction/`.
- Fotos do 4.0 (28, cidade pequena): `C:/Projects/Roadcraft-windows/ports/referencia/reference/`. Fotos do jogo atual, o "antes" (46): `C:/Projects/Roadcraft-windows/ports/referencia/antes/`.
- `native/build-win` e `native/build-win-debug` estão configurados, sem DLLs nem import ainda. Logo no começo, em segundo plano, compile (`MINGW=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64 JOBS=4 bash tools/build/build.sh core-windows`) e importe (`C:/Users/Jonathan/AppData/Local/Microsoft/WinGet/Links/godot_console.exe --path project --headless --import`), enquanto faz a análise.

## Roteiro obrigatório: o rastreamento
O rastreamento é o roteiro do porte, de ponta a ponta. Não é material de consulta.
1. Leia `feature.md` inteiro, depois `source/` arquivo por arquivo e `boundary.json`.
2. Antes de codificar, escreva `C:/Projects/Roadcraft-windows/ports/demolition/coverage.md`: uma linha por arquivo e por declaração própria de `feature.md`, com o que aquilo faz para o jogador e onde vai morar no jogo.
3. Porte tudo o que está lá e marque cada linha: portado, já existia, ou não se aplica, com o motivo e o dono no jogo. O `src/` do 4.0 só complementa o rastreamento.
4. Se o rastreamento estiver incompleto, complete: em `C:/Projects/Roadcraft-windows/converter`, `node scripts/port.ts extract demolition <entradas>` (só escreve em `converter/out`).
A conversão é escrita por você, na arquitetura atual, otimizada. Porta-se a funcionalidade, nunca o código.

## Fotos e jogo
Abrir o jogo e o 4.0 faz parte do porte: as fotos de antes e depois exigem isso, e nada proíbe abrir. Use as fotos prontas quando servirem. Quando faltar um ângulo ou a sua funcionalidade em ação, tire a foto: no jogo por `tools/build/hidden_run.ps1 -Command "<cmd>" -WorkDir C:/Projects/Roadcraft-windows/ports/demolition/workspace -TimeoutS <s>`, mapa 0; no 4.0 por `node scripts/port.ts reference demolition <plano.json>` em `C:/Projects/Roadcraft-windows/converter`, com plano seu (modelo em `C:/Projects/Roadcraft-windows/records/look/`). A única restrição: a cidade grande do 4.0 não se abre; use a cidade pequena ou só a funcionalidade. Uma instância por vez, fechada pelo PID.

## Método, nesta ordem
Análise do rastreamento e do jogo atual, com medição; pesquisa profunda na documentação oficial e fontes primárias; plano completo; codificação sem testar nem medir; teste, fotos e análise atenta de cada foto, corrigindo todo problema. Mesmo comportamento e visual do 4.0, implementação nativa na arquitetura (`core` → `sim` → `render` → `api`, `CityState`, `ChangeLog`, `Simulation`, `JobPool`, MultiMesh, `DataTables`), sem as patologias do 4.0, cada constante com medição ou fonte. Nunca solução barata; nunca tentativa e erro; nunca ignorar problema; sem Agent, Task ou Workflow. Outras sessões rodam ao mesmo tempo: valem as regras de carga da seção 5 de `session-rules.md`. Se precisar de capacidade de outro dono, escreva no dono certo e diga no relatório.

## Escopo
Demolir, bomba, cratera, efeitos, vítimas e destroços. Lição do branch antigo: a explosão precisa ter efeito real no estado da cidade.

## Entrega
Commits por tema neste branch, cada um compilando; gates OK; builds release e debug; 8 testes; capturas; fotos antes/depois e do 4.0 analisadas uma a uma; `CLAUDE.md` deste clone com mapa e pendências em dia; `coverage.md` completo. No fim escreva `C:/Projects/Roadcraft-windows/ports/demolition/report.md`: o que leu, requisitos, pesquisa com fontes, plano, commits, resultados de gates/builds/testes/capturas, fotos analisadas, constantes com fonte, cobertura do rastreamento, o que não foi verificado (medições que dependem da máquina quieta, com o comando). Sem `coverage.md` completo a entrega volta. Não integre nada.

## Ordem do usuário: faça tudo você mesma
Não confie no que a coordenadora preparou. Isto vale acima de qualquer outra parte deste prompt.
1. Rode você mesma o script: em `C:/Projects/Roadcraft-windows/converter`, `node scripts/port.ts extract <feature> <entradas>`, escolhendo as entradas pelo `src/` do 4.0.
2. Verifique o código extraído contra o `src/` do 4.0, arquivo por arquivo. O que faltar da funcionalidade, acrescente como entrada e rode de novo, até ela estar inteira.
3. Desconsidere as fotos de `ports/referencia/`. Tire as suas: no 4.0 por `node scripts/port.ts reference <feature> <plano.json>` (servidor ligado em http://127.0.0.1:5300/, cidade pequena) e no jogo pelo `hidden_run.ps1`, nos ângulos que a funcionalidade precisa.
4. Faça a conversão, os testes e a entrega como descrito acima.
