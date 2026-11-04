# Regras das sessões (Roadcraft-windows)

Arquivo único de regras, mantido pela coordenadora. Toda sessão (coordenadora e filhas) o lê inteiro ao começar e sempre que receber uma mensagem da coordenadora. A skill `corrigir-na-causa` é a forma operacional destas regras; invoque-a no início de toda tarefa.

## 1. Modelo de trabalho

- **Raiz do projeto:** `C:\Projects\Roadcraft-windows`. `fonte/` é o jogo (repositório-mãe, só a coordenadora integra nele); `converter/` é o script de porte e os rastreamentos (`converter/out/features/<feature>/`); `ports/<feature>/` guarda o clone, as fotos de referência, a evidência e o pacote de cada funcionalidade; `registro/` é o registro de coordenação.
- **Coordenadora:** uma sessão, cwd na raiz. Decide a fila sozinha (`registro/fila.md`), rastreia cada funcionalidade (`extract`), fotografa o 4.0 (`reference`), cria o clone (`workspace`), escreve `ports/<feature>/prompt.md` e **lança a filha ela mesma** (`claude -p "$(cat prompt.md)" --dangerously-skip-permissions --add-dir ...` no clone, Bash em segundo plano), verifica a entrega (`relatorio.md`, `sessao.log`, `verify`, `audit`, `pack`, `sandbox`) e integra no `fonte` por worktree. Não edita os clones: o que está errado volta para a filha (correção no `prompt.md`, filha relançada no mesmo clone). Não pergunta nada ao usuário.
- **Filha:** uma sessão `claude` por funcionalidade, lançada pela coordenadora, cwd no clone `C:\Projects\Roadcraft-windows\ports\<feature>\workspace` (clone isolado do `fonte`, branch `convert/<feature>`, origem em `fonte`, já com `godot-cpp`, `build-win`, `build-win-debug` e o import feitos). Edita só o clone; `fonte/`, os outros `ports/*` e `converter/` são só leitura; sem push. Dentro do clone os caminhos são os do jogo (`project/`, `native/`, `tools/`, `CLAUDE.md`); o que está fora do clone é lido por caminho absoluto. Entrega em `ports/<feature>/relatorio.md`.
- **Subagentes:** proibidos em toda sessão (Agent, Task, Workflow). As filhas são conversas separadas, não subagentes.
- **Sem burocracia:** as ações do processo (ler, pesquisar, editar, compilar, rodar o jogo oculto, testar, fotografar, commitar no seu branch, mandar mensagem entre coordenadora e filhas) já estão autorizadas. Ninguém para para pedir permissão ou confirmação, nem devolve lista de problemas sem a correção feita.

## 2. Roadcraft 4.0 — referência liberada, funcionalidade e nunca código

- A pasta `C:\Users\Jonathan\Documents\Projects\roadcraft-4.0-main` (e a cópia usada pelo conversor, `C:\Users\Jonathan\Documents\Labs\roadcraft-4.0-main`) pode ser lida, aberta, rodada e estudada a qualquer momento: é a referência de visual, comportamento e funcionalidade.
- O código do 4.0 está doente e não otimizado: não se porta o código, portam-se as funcionalidades. O script (`converter/scripts/port.ts extract`) **rastreia** a funcionalidade completa; a **conversão é escrita por você** na arquitetura atual, otimizada. Nada de código colado sem passar pela arquitetura.
- Toda constante em `data/**` é justificada por medição ou por fonte primária (documentação oficial do Godot 4.7, artigos, SUMO, normas); o valor do 4.0 é referência de comportamento, não fonte.
- Toda funcionalidade portada tem dono único, entra pelas camadas (`core` → `sim` → `render` → `api`) e passa pelos gates.
- Sem auditoria de origem: não há obrigação de rastrear, reescrever ou apagar comentários, `_doc`, nomes ou constantes por terem vindo do 4.0. O que vale é a arquitetura, a fonte de cada constante e os gates.

## 3. Processo obrigatório, nesta ordem

1. Análise do código e medição até a causa; identificação de todos os problemas.
2. Busca profunda na internet, na documentação oficial (Godot 4.7, godot-cpp, artigos originais, SUMO, normas), até entender o que deve ser feito e qual a metodologia.
3. Compreensão profunda e plano completo.
4. Codificação sem testar nem medir nada. Ao codificar, garanta por leitura que tudo está correto.
5. Depois: teste, fotos, análise atenta de cada foto (nunca superficial), identificação de todo problema restante e correção.

Regras do processo:
- Pesquisar é obrigação de toda sessão: toda proposta chega com a pesquisa feita, as alternativas comparadas com medição e a fonte de cada número.
- Nunca a solução barata: a escolha é pela solução correta e completa, que atinge o visual e a funcionalidade do 4.0 e resolve a causa. O custo se mede para caber no orçamento, nunca para escolher solução pior.
- Jamais codificar sem saber o que fazer e como fazer.
- Jamais tentativa e erro: se tentou uma vez e não conseguiu, volte à documentação oficial até achar a forma correta.
- Nunca adie, ignore ou cale um problema achado.
- Nenhuma correção cria erro novo, regressão ou quebra o que funciona.

## 4. Arquitetura

`core` → `sim` → `render` → `api`, com dono único de cada regra. Ajustes só em `data/**`, com schema em `tools/gates/schemas/`. Ações do jogador por `sim/commands`. Nada de código duplicado. O `CLAUDE.md` do jogo (no clone, o do próprio clone) é o documento único: mapa e pendências em dia na mesma mudança.

## 5. Jogo, carga da máquina e medições

- Só a cidade pequena: mapa 0, `--map=0`, nunca 1 a 4.
- No máximo uma instância sua do jogo por vez, sempre por `tools/build/hidden_run.ps1 -Command "<cmd>" -WorkDir <pasta do seu repositório> -TimeoutS <s>`. Na filha o `-WorkDir` é obrigatório (o padrão aponta para `fonte`). Nunca abra `godot` com janela. Feche a sua instância pelo PID ao terminar cada captura ou teste. Nunca mexa em processos de outras sessões.
- Compile com `-j 4`. No máximo 3 filhas compilando ou rodando o jogo ao mesmo tempo; a coordenadora dosa o lote.
- Nenhuma sessão espera a outra: siga no seu ritmo.
- Com a máquina carregada valem só as medições que não dependem dela: contagens (triângulos, draw calls, instâncias, bytes), trabalho por tick e simulação por tempo simulado, sem tick pulado. Tempos de quadro, GPU, CPU e `camera_pan` ficam listados no relatório com o comando e o que comparar; rodam com a máquina quieta.

## 6. Sem regressão e commits

- Gates OK, DLLs de debug e de release compiladas, testes passando, capturas feitas; fotos de antes e depois comparadas; o mapa e as pendências do `CLAUDE.md` em dia.
- Commits regulares, por tema, no seu branch `convert/<feature>`, cada um compilando sozinho; nenhum arquivo que só mudou o fim de linha (`git diff --ignore-cr-at-eol --stat`); sem push. A coordenadora verifica, empacota (`pack`), prova (`sandbox`) e integra.

## 7. Paralelismo

- Cada filha trabalha no seu clone, um ambiente separado: duas filhas podem mexer nos mesmos arquivos e nos mesmos donos ao mesmo tempo. O único limite é a carga da máquina (seção 5).
- A coordenadora resolve os conflitos na integração.
- Se a funcionalidade precisa de uma capacidade de outro dono que ainda não existe, a filha a escreve no dono certo, por inteiro, e diz no relatório que aquele dono mudou.

## 8. Entrega da filha

`ports/<feature>/relatorio.md` com: o que leu (rastreamento, 4.0, documentação); requisitos e causa; pesquisa com fontes; plano; commits por tema; gates, builds release/debug, testes e capturas com resultado; fotos antes/depois e do 4.0 analisadas uma a uma; constantes com fonte; o que não foi verificado (medições que dependem da máquina quieta, com comando). A filha não integra nada.

## 9. Fotos: como analisar (ordem do usuário, 2026-11-04)

Uma estrada ruim foi descrita como boa por uma sessão e por mim. Daqui em diante:

1. Nenhuma foto é 'boa', 'certa' ou 'como o 4.0' sem o par do 4.0 no mesmo enquadramento: mesma posição, mesmo alvo, mesmo zoom, lado a lado.
2. Toda foto é lida em resolução cheia. A análise lista os defeitos que ela mostra, um por um, ou diz 'li inteira e não achei defeito' e o que conferiu.
3. Defeito visto na foto entra na lista de correção da própria sessão, nunca em 'não verificado'.
4. Foto em outro enquadramento, outra hora do dia ou outra resolução não vale como par, e a análise diz isso.
5. Efeitos animados (explosão, fogo, fumaça) são fotografados em instantes definidos nos dois jogos, com o instante no nome da foto.
6. Vista de dentro: o par é o mesmo prédio, o mesmo andar e a mesma câmera.
