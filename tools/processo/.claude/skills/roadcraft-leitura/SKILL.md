---
name: roadcraft-leitura
description: Leitura integral com prova no projeto Roadcraft (C:\Projects\Roadcraft-windows, fonte, clones em ports/*/workspace e o 4.0). Use quando a tarefa pedir ler, analisar, revisar ou "ler inteiro" documentos, relatórios, código ou páginas, e ao começar uma sessão de porte: o inventário de leitura (registro/leitura) é a prova, gerada pelo hook, não pela palavra do modelo.
---

# Leitura integral com prova (Roadcraft)

O hook `PostToolUse` do usuário registra cada leitura por `Read`, `cat`, `sed -n`, `head`, `tail` e `WebFetch` em
`registro/leitura/inventario.json` (linhas ou caracteres lidos, hash do arquivo, sessão). Nada do que você diz que leu
conta: conta o que o inventário registrou.

Ferramenta: `python ferramentas/leitura/inventario.py` (raiz do projeto). Comandos:

- `verificar`: o que falta das fontes obrigatórias (`registro/leitura/obrigatorios.json`); sai com 1 enquanto faltar.
- `cobertura`: regrava `registro/leitura/cobertura.md` (blocos totais e lidos por fonte, desatualizadas, conclusões).
- `concluir <fonte> <bloco> "<conclusão>" "<decisão>"`: liga o que foi entendido num bloco a uma decisão concreta.
- `cruzar [--tudo]`: arquivos citados nos documentos de regra que ainda não têm leitura registrada (omissões).
- `importar <transcricao.jsonl>`: prova retroativa a partir do histórico de uma sessão (`~/.claude/projects/...`).

Regras:

1. Arquivo longo: leia em blocos numerados de 200 linhas (`sed -n 'a,bp'` ou `Read` com `offset`/`limit`) até a última
   linha. Busca textual (`grep`) não é leitura.
2. Página longa: `WebFetch` lê 100 mil caracteres por chamada; continue com `offset` até o fim que a própria resposta indica.
3. Antes de dizer "li", rode `verificar`; antes de decidir algo que depende de um bloco, registre a conclusão com `concluir`.
4. No começo de uma sessão, o hook `SessionStart` injeta o último checkpoint (`registro/leitura/estado.json`) e a
   cobertura pendente: retome dali, não do zero.
5. Novas fontes que a tarefa torna obrigatórias entram em `obrigatorios.json` (glob ou URL, com o motivo).
