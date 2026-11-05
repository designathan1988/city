# Hooks e mecanismos de trava do Claude Code (estudo da documentação oficial)

Lido integralmente em 5/nov/2026, por blocos, em https://code.claude.com/docs/en/: `hooks` (referência, 236 mil caracteres), `hooks-guide` (guia prático), `settings`, `permissions`, `permission-modes`, `memory` (CLAUDE.md e `.claude/rules`), `skills`, `output-styles`, `goal`, `commands`. Versão instalada: Claude Code 2.1.317 dentro do Claude Desktop (Windows). As anotações brutas de cada bloco estão no scratchpad da sessão (`ESTUDO-HOOKS*.md`).

## 1. O que é um hook e para que serve

Definição oficial (guia): "Hooks são comandos definidos pelo usuário que o Claude Code executa em pontos fixos do seu ciclo de vida. Isso dá controle **determinístico**: certas ações sempre acontecem, em vez de depender de o modelo escolher executá-las. Use hooks para impor regras do projeto, automatizar tarefas repetitivas e integrar o Claude Code às suas ferramentas."

A documentação separa com clareza duas camadas:

| Camada | Quem impõe | Garantia |
|---|---|---|
| CLAUDE.md, `.claude/rules`, skills, estilos de saída, instruções na conversa | O modelo lê e tenta seguir | Nenhuma. Texto oficial: "Claude trata isso como contexto, não como configuração imposta"; "não há garantia de cumprimento estrito". |
| Hooks, regras de permissão (`allow`/`ask`/`deny`), managed settings, modos de permissão, sandbox | O programa Claude Code, antes ou depois da ação, sem perguntar ao modelo | Determinística. "Regras de settings são impostas pelo cliente independentemente do que o Claude decida." |

Frase que resume a posição oficial (página de memória): "Se uma instrução precisa rodar num ponto específico, como antes de cada commit ou depois de cada edição, escreva-a como um hook. Para bloquear uma ação independentemente do que o Claude decida, use um hook PreToolUse."

## 2. Como um hook funciona

1. **Evento** dispara (ex.: o Claude vai chamar uma ferramenta).
2. **Matcher** filtra (ex.: só `Edit|Write`; só `Bash`; só o modelo `claude-sonnet-5`).
3. **`if`** opcional, com sintaxe de regra de permissão (`Bash(git *)`, `Edit(*.ts)`), só em eventos de ferramenta; é "best effort", não serve de trava dura.
4. **Handler** roda. Recebe um JSON em `stdin` com `session_id`, `cwd`, `hook_event_name` e os dados do evento (`tool_name`, `tool_input`, `prompt`, `last_assistant_message`, `from_model`/`to_model`...).
5. **Decisão** pelo código de saída e pelo `stdout`:
   - `exit 0` sem saída: sem objeção.
   - `exit 0` com um objeto JSON em `stdout`: controle fino (`permissionDecision` allow/deny/ask, `decision: "block"` + `reason`, `additionalContext`, `updatedInput`, `systemMessage`...).
   - `exit 2`: bloqueia; o texto em `stderr` vira o motivo que o Claude recebe.
   - Qualquer outro código: erro não bloqueante (a ação segue), salvo `onFailure: "block"`.

Todos os hooks de um evento rodam em paralelo e a decisão mais restritiva vence (`deny` > `defer` > `ask` > `allow`). Hooks de vários arquivos se somam. O arquivo de settings é vigiado: um hook novo vale na sessão em andamento, sem reiniciar.

**Um `deny` de hook vale em todo modo de permissão, inclusive `bypassPermissions` / `--dangerously-skip-permissions`.** O PreToolUse roda antes de qualquer checagem de modo. Com managed settings na máquina (nosso caso), nem um "mod" instalado consegue aprovar uma chamada que o hook bloqueou.

## 3. Todos os eventos

| Evento | Quando dispara | Pode bloquear? | Serve para |
|---|---|---|---|
| `SessionStart` | início, `resume`, `clear`, `compact`, `fork` | não | injetar contexto (`additionalContext`), inclusive **reinjetar o processo após compactação** (matcher `compact`) |
| `Setup` | `--init-only`, `-p --init`, `--maintenance` | não | preparação em CI |
| `UserPromptSubmit` | antes de o prompt chegar ao Claude (também em turnos que o Claude Code inicia) | sim (prompt não chega) | validar/bloquear prompt, acrescentar contexto a cada pedido |
| `UserPromptExpansion` | quando `/comando` expande | sim | controlar skills |
| `PreToolUse` | antes de cada ferramenta | **sim** | proteger arquivos, barrar comandos, reescrever argumentos, pedir confirmação |
| `PermissionRequest` | quando o Claude Code vai pedir permissão | decide por você | auto-aprovar (ex. `ExitPlanMode`), mudar modo |
| `PermissionDenied` | o classificador do auto mode negou | não | permitir nova tentativa |
| `PostToolUse` | após ferramenta com sucesso | não desfaz; `block` devolve motivo | formatar, rodar testes (`async`), auditar; `git status --porcelain` para ver o que mudou |
| `PostToolUseFailure` | ferramenta falhou | não | contexto extra |
| `PostToolBatch` | após um lote de chamadas paralelas | sim (para o laço) | checagem por lote |
| `Notification` | aviso (`permission_prompt`, `idle_prompt`, limites de uso...) | não | notificação no desktop |
| `MessageDisplay` | ao exibir texto do assistente | não | reescrever só o que aparece na tela |
| `SubagentStart` / `SubagentStop` | subagente inicia/termina | Stop: sim | controlar subagentes |
| `TaskCreated` / `TaskCompleted` | tarefa criada / marcada concluída | **sim** | exigir testes antes de fechar uma tarefa |
| `Stop` | **toda vez** que o Claude termina de responder | **sim** (continua com o motivo) | gates no fim do turno; avaliação item a item (é o que o `/goal` usa) |
| `StopFailure` | turno terminou por erro de API | não | registro |
| `TeammateIdle` | colega de equipe vai ficar ocioso | sim | manter trabalhando |
| `InstructionsLoaded` | CLAUDE.md ou regra entrou no contexto | não | auditar o que foi carregado e quando |
| `ConfigChange` | settings/skills mudaram durante a sessão | **sim** (exceto managed) | impedir que a sessão altere a própria configuração |
| `CwdChanged` / `DirectoryAdded` | mudou de pasta / `/add-dir` | não | ambiente |
| `FileChanged` | arquivo vigiado mudou **por qualquer meio** (Edit, Bash, processo externo) | não (já mudou) | reagir/restaurar arquivo protegido |
| `WorktreeCreate` / `WorktreeRemove` | worktree criado/removido | substitui o git | outro VCS |
| `PreCompact` / `PostCompact` | antes/depois de compactar (`manual`/`auto`) | Pre: sim | guardar/reinjetar; `compact_summary` |
| `PreModelSwitch` | troca de modelo **pedida** (`/model`, picker, `/config`, SDK) | **sim** (allow/deny/ask) | barrar troca para modelo indesejado |
| `PostModelSwitch` | **qualquer** troca, inclusive automática (`source: "auto"`) e ao retomar | não | **detectar e avisar troca automática**; injetar instrução por modelo |
| `Elicitation` / `ElicitationResult` | servidor MCP pede entrada | responde por você | formulários |
| `SessionEnd` | fim da sessão (1,5 s de orçamento) | não | limpeza |

Observação importante sobre modelo: o `PreModelSwitch` **não** roda nas trocas que o Claude Code faz sozinho (fallback automático). Essas só chegam ao `PostModelSwitch`. A trava que impede a troca é a dos managed settings: `availableModels` + `enforceAvailableModels` (já instalada e provada por `/model claude-sonnet-5-5` recusado). O hook serve para registrar e avisar.

## 4. Os cinco tipos de handler

| `type` | O que faz | Quando usar |
|---|---|---|
| `command` | roda um script (`command` + `args` = forma exec, sem shell, recomendada; `shell: "powershell"` opcional) | regra determinística (o nosso caso) |
| `http` | POST do JSON para uma URL | serviço central |
| `mcp_tool` | chama uma ferramenta MCP | integração |
| `prompt` | manda o JSON e um prompt a um modelo; ele responde `{ok, reason, impossible}` | julgamento ("as tarefas acabaram?"); é o `/goal` |
| `agent` | subagente com Read/Grep/Glob, até 50 turnos (experimental) | verificar em arquivos e saídas de teste |

Eventos que aceitam `prompt`/`agent`: PreToolUse, PostToolUse, PostToolBatch, PostToolUseFailure, PermissionDenied, Stop, SubagentStop, TaskCreated, TaskCompleted, TeammateIdle, UserPromptSubmit, UserPromptExpansion. Os demais só `command`/`http`/`mcp_tool`.

Campos: `timeout` (segundos; padrão 600 em command, 30 em prompt e em UserPromptSubmit/Pre/PostModelSwitch, 60 em agent), `async: true` (segundo plano; não bloqueia; resultado chega no próximo turno), `onFailure: "block"`, `once: true` (só em skill), `continueOnBlock` (prompt).

## 5. Como criar um hook, do jeito mais simples

1. Abra o settings do escopo desejado: `.claude/settings.json` do projeto (vale para quem clonar), `~/.claude/settings.json` (todos os seus projetos) ou `.claude/settings.local.json` (só você, só este projeto). JSON estrito: sem comentários nem vírgula final.
2. Acrescente um bloco `hooks` com o evento, o matcher e o handler. No Windows, prefira a forma exec com o caminho do interpretador e `args`, sem aspas de shell:

```json
{
  "hooks": {
    "PreToolUse": [
      {
        "matcher": "Edit|Write|Bash|PowerShell",
        "hooks": [
          {
            "type": "command",
            "command": "C:/Users/Jonathan/AppData/Local/Programs/Python/Python312/python.exe",
            "args": ["${CLAUDE_PROJECT_DIR}/tools/gates/hook_protect.py"],
            "timeout": 10
          }
        ]
      }
    ]
  }
}
```

3. Escreva o script: leia o JSON de `stdin`, decida, e saia com `0` (nada), `2` + mensagem em `stderr` (bloqueia), ou `0` + JSON em `stdout` (decisão fina). Em Python no Windows, configure `stdout` em UTF-8 antes de imprimir JSON com acentos.
4. Teste fora do Claude: `echo '{"tool_name":"Bash","tool_input":{"command":"ls"}}' | python hook.py; echo $?`.
5. Confira com `/hooks` (lista os hooks e de onde vêm). O arquivo é recarregado sozinho.
6. Para depurar: `claude --debug-file caminho.log` e procure `Hooks: Evento:Matcher [...] finished with status N (ms)`. Erros silenciosos comuns: JSON que não é o primeiro texto do `stdout` (perfil do shell imprimindo antes), campo no nível errado (`permissionDecision` fora de `hookSpecificOutput`), matcher com nome de exibição em vez do nome canônico da ferramenta.

Você também pode descrever o hook ao Claude e pedir que ele escreva; a doc sugere isso.

## 6. Exemplos prontos (adaptados ao método deste projeto)

**a) Proteger os arquivos dos gates e dos hooks (instalado: `tools/gates/hook_protect.py`).** PreToolUse em `Edit|Write|Bash|PowerShell`; nega com `permissionDecision: "deny"` quando o alvo é `rules.json`, `owners.json`, `run_gates.py`, `gate_*.py`, `hook_*.py`, schemas existentes e `.claude/settings*.json`; leitura e execução passam. Vale em bypass.

**b) Gates no fim do turno (instalado: `tools/gates/hook_stop_gates.py`).** Stop: se há mudanças não commitadas, roda `run_gates.py`; se falha, `{"decision":"block","reason":"..."}` com o relatório, e a sessão continua corrigindo. Respeita `stop_hook_active` (teto de 8 continuações seguidas; `CLAUDE_CODE_STOP_HOOK_BLOCK_CAP` muda).

**c) Avaliação item a item ao parar, permanente (equivalente do seu `/goal`).** Stop do tipo `prompt`:

```json
{"hooks":{"Stop":[{"hooks":[{"type":"prompt","timeout":30,
 "prompt":"Avalie se o Claude deve parar: $ARGUMENTS. Regra: ele só pode parar se a última mensagem mostra UM item do processo concluído e verificado (análise com medição, ou pesquisa com fontes, ou plano, ou código, ou teste com fotos analisadas) e pede avaliação do usuário antes do próximo item. Responda {\"ok\":true} ou {\"ok\":false,\"reason\":\"o que falta\"}."}]}]}}
```

O `/goal` é "um invólucro em volta de um hook Stop do tipo prompt com escopo de sessão"; a versão no settings vale em toda sessão. Para exigir prova em arquivos (testes rodados, fotos existentes), use `type: "agent"`.

**d) Reinjetar o processo após compactação.** SessionStart com matcher `compact` e um `echo` (ou script que imprime o trecho "Processo de trabalho" do CLAUDE.md): o texto entra como contexto. O CLAUDE.md da raiz já é reinjetado sozinho; isto serve para ordens dadas na conversa.

**e) Registrar e avisar troca de modelo.** PostModelSwitch, matcher vazio: o script lê `from_model`, `to_model`, `source` e imprime `{"systemMessage":"Modelo mudou de X para Y (origem: auto)"}` e grava num log. PreModelSwitch com matcher `claude-sonnet-5` e `exit 2` nega uma troca pedida por engano.

**f) Impedir que a sessão altere a própria configuração.** ConfigChange com `exit 2` para `project_settings`/`local_settings`: a mudança não passa a valer na sessão (o arquivo em si já está protegido pelo hook a).

**g) Reagir a mudança de arquivo por qualquer caminho.** FileChanged com matcher `rules.json|owners.json`: roda `git checkout -- <arquivo>` e avisa. Cobre o que o PreToolUse não vê (um programa que escreve o arquivo por dentro).

**h) Rodar os gates em segundo plano após cada edição.** PostToolUse `Edit|Write` com `async: true`: o script roda `run_gates.py --file <arquivo>` e devolve `additionalContext` com o resultado no próximo turno, sem travar o trabalho.

**i) Exigir testes antes de fechar uma tarefa.** TaskCompleted com `exit 2` se os testes não passaram: a tarefa não é marcada como concluída e o motivo volta ao modelo.

**j) Não pedir confirmação para sair do plan mode.** PermissionRequest com matcher `ExitPlanMode` devolvendo `{"hookSpecificOutput":{"hookEventName":"PermissionRequest","decision":{"behavior":"allow"}}}`.

**k) Lembrete a cada prompt.** UserPromptSubmit com `hookSpecificOutput.additionalContext` ("Este repositório exige: causa raiz, pesquisa oficial, plano, código, teste com fotos.") Escreva como fato, não como ordem de sistema.

## 7. Os outros mecanismos de trava (além de hooks)

**Regras de permissão (`permissions.allow/ask/deny`).** Avaliadas na ordem deny, ask, allow; a primeira que casa decide; um deny largo vence um allow estreito; deny de qualquer arquivo vence allow de qualquer outro. `deny` com nome puro (`"Agent"`, `"Skill"`) remove a ferramenta do contexto do modelo (ele nem a vê). Exemplos para este projeto: `"deny": ["Agent", "Workflow"]` (sem subagentes), `"deny": ["Edit(tools/gates/**)"]`, `"ask": ["Bash(git push *)"]`. Regras Bash casam o texto do comando (não pegam `/bin/rm` nem `sh -c`); para fronteira dura, sandbox ou hook. Read/Edit(path) usam sintaxe gitignore; no Windows `//c/**/.env`.

**Managed settings (`C:\Program Files\ClaudeCode\managed-settings.json`).** Precedência máxima; nada em settings de usuário/projeto nem `--settings` sobrepõe. Chaves de trava: `availableModels` + `enforceAvailableModels` (o `model` é só o inicial), `permissions.deny`, `permissions.disableBypassPermissionsMode`, `permissions.disableAutoMode`, `allowManagedHooksOnly`, `allowManagedPermissionRulesOnly`, `claudeMd` (instruções embutidas), `maxEffortLevel`. Hooks em managed não são desligados por `disableAllHooks` de outro arquivo. O app desktop mantém a allowlist `availableModels` do managed em vigor (confirmado no teste).

**Modos de permissão.** `plan` tranca edições até você aprovar o plano (exceto terminal interativo com bypass disponível); `auto` põe um classificador (Sonnet 5) que bloqueia por padrão deploy, force push, `reset --hard`, lançar agente com `--dangerously-skip-permissions`, escrever nas transcrições etc.; "fronteiras ditas na conversa" ("não faça push") são respeitadas pelo classificador, mas se perdem na compactação: "para garantia dura, use deny rule"; `dontAsk` nega tudo que pediria (CI); `bypassPermissions` desliga prompts e permite escrever em caminhos protegidos (`.claude`, `.git`), por isso o hook a é necessário. `blockWritesOutsideWorkingDirectories` e `blockReadsOutsideWorkingDirectories` valem em todo modo.

**CLAUDE.md e `.claude/rules`.** Orientação, não imposição. Alvo < 200 linhas por arquivo (mais longo = menos adesão); contradições fazem o modelo escolher uma ao acaso; `/doctor prompt-audit` acha conflitos; `.claude/rules/*.md` com `paths:` carrega só quando o arquivo casa; o CLAUDE.md da raiz sobrevive à compactação, instruções só na conversa não.

**Skills.** Procedimentos sob demanda. `disable-model-invocation: true` (só você invoca e o Claude Code bloqueia se o modelo tentar), `disallowed-tools` (ex. tirar `AskUserQuestion`), `allowed-tools`, `hooks:` no frontmatter (valem até o fim da sessão depois de invocada), `context: fork`. Uma skill chamada `verify` ou `simplify` no projeto é chamada antes de cada commit pelas instruções do Claude Code. O conteúdo não é relido nos turnos seguintes e após compactação só os primeiros 5 000 tokens voltam.

**Estilos de saída.** Instruções para toda resposta: `Proactive` (começa sem perguntar), `Concise`, `Explanatory`, `Learning` (deixa trechos para você escrever e **para** esperando; estava ativo nas sessões antigas). Custom em `.claude/output-styles/nome.md`. "Não garante que algo sempre aconteça; para isso, hook."

**`/goal`.** Stop hook de prompt por sessão; o avaliador (modelo pequeno) julga só o que apareceu na conversa; condição até 4 000 caracteres, pode limitar turnos; funciona em `-p` e no desktop; `/goal clear`.

**`--append-system-prompt`.** Texto no nível do system prompt, na linha de comando (`claude -p` das filhas).

## 8. Mapa: o que você quer garantir → o mecanismo

| Garantia | Mecanismo determinístico | Complemento de orientação |
|---|---|---|
| Nunca mudar de modelo sozinho | managed `availableModels` + `enforceAvailableModels` (feito) | PostModelSwitch avisando |
| Nunca alterar gates, schemas, hooks, settings | PreToolUse `hook_protect.py` (feito) + ConfigChange + FileChanged | linha no CLAUDE.md (feito) |
| Gates sempre OK ao fim do turno | Stop `hook_stop_gates.py` (feito) | processo no CLAUDE.md |
| Parar após cada item para avaliação | Stop `type: prompt` (exemplo c) ou `/goal` por sessão | ordem no prompt |
| Sem subagentes/workflows | `permissions.deny: ["Agent","Workflow"]` | "sem Agent/Task/Workflow" no CLAUDE.md |
| Sem perguntas de confirmação | estilo `Proactive`; skill com `disallowed-tools: AskUserQuestion`; `deny: ["AskUserQuestion"]` | "não me peça confirmação" |
| Ler de fato os arquivos | PostToolUse `Read` registrando; Stop `type: agent` conferindo | "leia inteiro" |
| Pesquisar antes de responder | UserPromptSubmit com lembrete; Stop prompt exigindo fontes | processo |
| Não perder ordens na compactação | SessionStart `compact` reinjetando; sessões curtas | CLAUDE.md curto |
| Não editar antes de planejar | modo `plan` + PermissionRequest `ExitPlanMode` | etapa 3 do processo |

## 9. Limites honestos (texto da própria doc)

- Hook PreToolUse só vê a chamada da ferramenta; um programa que escreve o arquivo por dentro não é visto (use FileChanged ou sandbox).
- Stop hook dispara em toda resposta, não só no fim da tarefa; teto de 8 bloqueios seguidos sem chamada de ferramenta.
- Hooks `prompt`/`agent` são julgamentos de modelo: determinístico é o disparo, não o veredito.
- `if` e regras Bash são "best effort" contra formas alternativas do mesmo comando.
- CLAUDE.md, estilos e instruções na conversa nunca são garantia.
- PreModelSwitch não vê troca automática; a trava real é `availableModels`.
- Compactação carrega só o CLAUDE.md da raiz e um orçamento de skills.

## 10. Estado no projeto hoje

- Managed: `C:\Program Files\ClaudeCode\managed-settings.json` com `model`, `availableModels` e `enforceAvailableModels` (provado).
- `fonte/.claude/settings.json`: `autoMemoryEnabled: false`, permissões, e os dois hooks (`hook_protect.py` em PreToolUse; `hook_stop_gates.py` em Stop). Protegido pelo próprio hook.
- Removidos: skills, plugins, hooks antigos, memória automática, nas pastas raiz do Claude e nos clones.
- Pendência observada nesta sessão: `hook_protect.py` dá falso positivo quando um comando apenas **menciona** o nome de um arquivo protegido num texto (heredoc de anotações) e redireciona para **outro** arquivo. Correção: negar só quando o alvo do redirecionamento ou o argumento de um comando de escrita é o arquivo protegido.

## 11. Diagnóstico das negações (417 sessões, 23/out a 5/nov/2026)

Medido nas transcrições (`~/.claude/projects` e `C:\Projects\backup\.claude`): o modelo alegou "não tenho permissão" em texto **12** vezes; o programa devolveu **655** negações reais. Origem das negações:

| Origem | Quantidade | O que é |
|---|---|---|
| Hooks antigos do próprio usuário (git-gate, commitGate.mjs, writeGate, "Regra do usuário", whole-review) | 248 | removidos nesta semana |
| Classificador do auto mode (Sonnet 5, servidor) | 141 | motivos: sem explicação (109), Irreversible Local Destruction, Self-Modification (editar `.claude`), Create Unsafe Agents (lançar `claude -p --dangerously-skip-permissions`), Auto-Mode Bypass, CI Bypass |
| Prompt de permissão rejeitado ("The user doesn't want to proceed") | 127 | em modo Manual/auto, quando o pedido não é aprovado |
| Pedido de aprovação sem resposta em `-p` | 13 | sessões filhas em modo Manual negam o que pediria aprovação |

Modo inicial das sessões: 311 Manual (`default`), 88 auto (padrão do app), 10 bypass. Nenhum settings fixava `defaultMode`. Conclusão: as duas faces vêm de atores diferentes. "Pede autorização" é o programa (modo de permissão, classificador, hooks); "faz o que não foi pedido" é o modelo, que nenhuma permissão governa, só o que ele consegue executar.

Configuração aplicada em 5/nov, só com settings: `~/.claude/settings.json` com `permissions.defaultMode: "bypassPermissions"`, `skipDangerousModePermissionPrompt: true` e `outputStyle: "Proactive"`; `fonte/.claude/settings.json` com `permissions.deny: ["Agent", "Workflow"]`. Ambas valeram na sessão em andamento (estilo aplicado; ferramenta Workflow removida), sem reinício.

### Contra trabalho inventado: evidência produzida pelo programa

O que o modelo afirma não é verificável por permissão; o que o programa mede, é. Ao fim de cada turno com mudanças não commitadas, o hook de Stop (já existente, só ampliado) mede sozinho: (1) os gates, rodando `run_gates.py`; (2) a compilação, comparando a data de cada fonte C++ alterada com a das duas DLLs (`project/bin/libcitycore.windows.template_{release,debug}.x86_64.dll`). Fonte mais nova que uma DLL, ou DLL ausente, segura o turno com o comando de build na razão. Provado em repositório de teste (5 casos) e no real. Regras `deny` cobrem o irreversível: força-push, `reset --hard`, `clean`, `restore`, `checkout --`, `stash drop`, `branch -D` e escrita nos arquivos dos gates e dos settings, negados pelo programa antes de executar, em todo modo (provado em bypass com `git push --force --dry-run` e `echo x > tools/gates/owners.json`). Tudo o mais está em git e é reversível pelo usuário. Fora disso fica só o que nenhuma máquina mede: a qualidade do que foi feito, que os testes, as fotos e a sua leitura conferem.

### Testes como evidência, no ponto de entrega

O `git commit` é o ponto de entrega do processo (passo 7). Um commit que leva código (`native/`, `project/`) só passa quando os logs que `tools/build/run_tests.sh` escreve em `dist/tests/` (uma cena por renderizador, 16 arquivos) existem, são todos mais novos que o código e nenhum traz `SCRIPT ERROR` ou `FAIL`. Caso contrário o hook nega o commit e devolve `bash tools/build/build.sh e2e`. Conta o que está preparado e, com `git add` no mesmo comando ou `-a`, tudo o que mudou. Commits só de documento passam. Provado em 9 casos num repositório de teste e ao vivo no real. Arquivos fora da lista de deny: provado que um arquivo rastreado apagado volta byte a byte de `git show HEAD:caminho` (a lista de deny impede que o histórico seja destruído, então isso vale para todo arquivo rastreado).
