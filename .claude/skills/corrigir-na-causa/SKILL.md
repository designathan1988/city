---
name: corrigir-na-causa
description: >-
  Método de trabalho do Roadcraft-windows para tarefas de código: corrigir defeito, portar
  funcionalidade do Roadcraft 4.0 pelo script de porte, investigar problema visual ou funcional,
  medir desempenho, implementar ou refatorar. Use antes de tocar em código, e quando o usuário
  disser corrigir, consertar, portar, investigar, coordenar, não está funcionando, está feio, está
  lento ou tirei foto e está errado.
---

# Corrigir na causa — o método de trabalho

Esta skill é só o método: texto que orienta como trabalhar. Ela não concede nem bloqueia ferramenta e não tem hook; por isso o cabeçalho tem apenas `name` e `description`. Permissões ficam nas configurações do Claude Code, nunca aqui.

## Leis

1. **Causa, nunca efeito.** Toda correção ataca a causa raiz. Ajustar número para esconder sintoma, criar caso especial para o exemplo que apareceu ou mascarar o erro não é correção.
2. **Nunca a solução barata.** Escolha a solução completa e correta. O custo só se mede para caber no orçamento, nunca para escolher uma solução pior.
3. **Problema achado é problema resolvido.** Nunca adie, ignore ou esconda um problema, inclusive os que a sua mudança criar. Se ainda não sabe resolver, pare e pesquise. Todo problema chega ao usuário com a correção feita.
4. **Sem codificar no escuro.** Primeiro entender, depois pesquisar, depois planejar, só então codificar.
5. **Sem tentativa e erro.** Se uma tentativa falhar, volte à documentação oficial e às fontes primárias até entender o método correto.
6. **Sem subagentes dentro da sessão.** O trabalho da sessão é dela: não use Agent, Task ou Workflow. Sessões filhas de porte são conversas separadas, não subagentes.
7. **Sem afirmação sem prova.** Só declare concluído com gates, build, testes, medição e fotos analisadas. Diga também o que não foi verificado.

As ações normais do processo já estão autorizadas pelo usuário: ler, pesquisar, editar, compilar, rodar o jogo oculto, testar, fotografar e commitar. Execute sem perguntar "posso seguir?".

## Fases, nesta ordem

**1. Análise e medição até a causa.** Leia todo o código envolvido: o caminho do defeito, os donos vizinhos, os dados e os testes. Reproduza de forma controlada. Meça com sondas (`render_cost`, `camera_pan`), capturas, CPU, GPU, memória e contagens, comparando com o estado anterior e com a referência. Liste todos os problemas, inclusive os pequenos. Guarde a evidência de antes. Aqui não se codifica a correção.

**2. Pesquisa antes de decidir.** Vá à documentação oficial e às fontes primárias até entender o que fazer e qual é o método: Godot 4.7, godot-cpp, Terrain3D, Vulkan/Forward+, SUMO, normas (NBR, NPIP), fichas de fabricante, artigos originais e o comportamento observado do 4.0. Compare as alternativas com medição e cite a fonte de cada número.

**3. Compreensão e plano completo.** Escreva em palavras: causa provada, solução completa, arquivos e donos, dados e constantes com fonte, testes e fotos que vão provar. Respeite a arquitetura do documento do projeto. Se algo não estiver claro, volte à fase 2.

**4. Codificação sem testar nem medir.** Implemente o plano inteiro, garantindo por leitura e tipagem que tudo está correto. Sem TODO, sem implementação pela metade, sem gambiarra. Problema não estudado: pare e volte à fase 2. Atualize mapa e pendências do documento do projeto na mesma mudança.

**5. Teste, fotos e correção.** Rode `python3 tools/gates/run_gates.py`, compile as DLLs de release e debug (`-j 4`), rode os testes e as capturas. Tire as fotos de depois nos mesmos ângulos do antes e analise cada uma com muita atenção, contra o antes e a referência: artefatos, cortes, cores, geometria, iluminação e regressões em outras áreas. Corrija tudo o que achar e repita esta fase até não sobrar nada.

Os gates são parte da fase 5: você os roda, nada os dispara sozinho.

## Onde trabalhar

- **Raiz do projeto:** `C:\Projects\Roadcraft-windows`, com `fonte/` (o jogo), `converter/` (script de porte), `ports/<feature>/` (clone, fotos, evidência, pacote) e `registro/`.
- **Documento do projeto:** o `CLAUDE.md` do repositório que você edita: `fonte/CLAUDE.md`, ou o do clone na sessão filha.
- **Jogo:** mapa 0 (`--map=0`), sempre por `tools/build/hidden_run.ps1 -Command "<cmd>" -WorkDir <pasta do seu repositório> -TimeoutS <s>`. Uma instância por sessão, fechada pelo PID. Nunca espere um processo às cegas.
- **Máquina carregada:** valem só as medições que não dependem dela (contagens, trabalho por tick, simulação por tempo simulado). Tempos de quadro, GPU, CPU e `camera_pan` ficam listados com o comando, para a máquina quieta.

## Porte do Roadcraft 4.0: o script rastreia, você converte

Porta-se a funcionalidade, nunca o código. O 4.0 (`C:\Users\Jonathan\Documents\Projects\roadcraft-4.0-main`) é referência de comportamento e visual; o código dele é doente e não otimizado. O script não converte nada. Em `C:\Projects\Roadcraft-windows\converter`, com `node scripts/port.ts <comando>`:

1. `doctor`: confere as ferramentas e os caminhos da máquina.
2. `extract <feature> <entrada>...`: rastreia a funcionalidade inteira a partir dos arquivos de entrada (relativos à raiz do 4.0) e grava `feature.md`, `source/`, `boundary.json` e `interface/` em `converter/out/features/<feature>/`. Se o rastreamento estiver incompleto, acrescente entradas e repita.
3. `reference <feature> <plano.json>`: fotografa o 4.0 nos ângulos que o alvo vai usar (copie `converter/configs/look-small-city.json`; exige o servidor do 4.0 em `http://127.0.0.1:5300/`). Sai em `ports/<feature>/reference/`.
4. `workspace <feature>`: cria o clone isolado `ports/<feature>/workspace`, branch `convert/<feature>`, já com `godot-cpp`, builds e import.
5. **A conversão, escrita por você, pelas cinco fases acima:** entenda pelo rastreamento o que o jogador vê e faz, as regras, os dados e os casos de borda; pesquise o método; planeje em `core` → `sim` → `render` → `api`, com `CityState`, `ChangeLog`, `Simulation`, `JobPool`, MultiMesh e `DataTables`; escreva no dono certo, otimizado e determinístico.
6. `verify <feature> [passo]...` (gates, build-release, build-debug, import, tests, capture), `audit <feature>`, `pack <feature>` e `sandbox <feature>`: provam o que você escreveu. O `sandbox` imprime o procedimento de integração, que é manual.

Regras do porte:
- A estrutura do alvo é a autoridade; o código do 4.0 só define comportamento e visual.
- Não copie nomes, estrutura ou padrões do 4.0, nem as patologias dele: arquivo-deus, estado espalhado, alocação em loop, um mesh por item, fila com `shift`, cópia para worker, readback síncrono.
- Cada constante tem medição ou fonte primária; o valor do 4.0 não é fonte.
- O porte só está pronto quando `verify` e `sandbox` passam e as fotos do 4.0 e do alvo foram comparadas com atenção.

Antes de portar, leia `converter/README.md`, `converter/DESIGN.md`, o `feature.md` extraído e, no 4.0, `CLAUDE.md`, `ARQUITETURA.md` e `MIGRACAO-WEBGPU.md`. Coordenadora e filhas seguem `regras-sessoes.md` e `registro/prompt-coordenadora.md`.

## Pronto é

- Causa provada por medição, e solução completa aplicada nela.
- Pesquisa feita, cada constante com fonte, plano cumprido por inteiro.
- Gates, builds de release e debug, testes e capturas passando.
- Fotos analisadas, antes e depois comparados, nenhuma regressão.
- Documento do projeto em dia e commits por tema.
