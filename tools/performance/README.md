# Aplicação das correções preparadas

`apply_fixes.py` transporta o resultado da implementação. Não analisa causas sozinho nem substitui a revisão do código.
O pacote só pode ser exportado quando as 92 ocorrências estiverem implementadas e revisadas no código.
Esse bloqueio evita distribuir o trabalho parcial como solução completa; não constitui prova funcional.

```powershell
python tools/performance/apply_fixes.py export --worktree C:/caminho/do/worktree-aprovado --output C:/caminho/performance-fixes.zip
python tools/performance/apply_fixes.py apply --worktree C:/caminho/de/outro-worktree --bundle C:/caminho/performance-fixes.zip --dry-run
python tools/performance/apply_fixes.py apply --worktree C:/caminho/de/outro-worktree --bundle C:/caminho/performance-fixes.zip
```

O destino precisa ser um worktree adicional do mesmo repositório, na base auditada e numa branch `codex/`.
O checkout original protegido é recusado, assim como caminhos que saiam do destino, links, arquivos conflitantes
e alterações locais alheias ao pacote. Todos os hashes e conteúdos são conferidos antes de qualquer escrita.
Arquivos já aplicados são reconhecidos; uma execução interrompida pode ser retomada.

Cada troca de arquivo é atômica. Falhas normais tentam reverter as escritas anteriores, preservando edições
concorrentes detectadas. Uma interrupção do processo entre arquivos pode deixar uma aplicação parcial:
reexecute o mesmo pacote para completar, após conferir eventuais conflitos. Diretórios vazios podem permanecer.

O script não executa build, jogo, testes, gates, medições, hooks, commit, merge ou push. Aplicar fontes
não gera um executável e não é evidência de correção funcional ou ganho de desempenho.
