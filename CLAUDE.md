# Roadcraft-windows — regras de trabalho

A skill `corrigir-na-causa` (`.claude/skills/corrigir-na-causa/SKILL.md`) é obrigatória em toda tarefa de código: análise e medição até a causa raiz → pesquisa na documentação oficial e fontes primárias → compreensão e plano completo → codificação sem testar nem medir → teste, fotos e análise atenta, corrigindo todo problema. Nunca a solução barata; nunca tentativa e erro; nunca ignorar um problema; sem subagentes (Agent/Task/Workflow) dentro de uma sessão — a coordenadora lança sessões filhas de porte (`claude -p`), uma por clone em `ports/<feature>/workspace`. Nada de pedir permissão ou confirmação para as ações do processo.

- Regras de arquitetura, comandos e mapa do jogo: `fonte/CLAUDE.md` (documento único do projeto).
- Regras operacionais das sessões (coordenadora, filhas, clones, carga, entrega): `regras-sessoes.md`. Prompt da coordenadora: `registro/prompt-coordenadora.md`.
- Porte de funcionalidades do Roadcraft 4.0: **funcionalidade, nunca código** — o código do 4.0 é referência de comportamento e está doente/não otimizado; o script `converter/scripts/port.ts extract` **rastreia** a funcionalidade completa, e a **conversão é feita por você** na arquitetura atual, otimizada (leia `converter/README.md` e `converter/DESIGN.md`).
- O Roadcraft 4.0 é referência de visual e comportamento, com acesso liberado: `C:\Users\Jonathan\Documents\Projects\roadcraft-4.0-main`.
