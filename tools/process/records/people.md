# Pessoas

## Objetivo e itens
- Deixar `pessoas-recozinhar` pronto para a coordenadora integrar: asset do formato 2 comitado, master no branch, citações ao 4.0 fora, rosto limpo, fase 5 validada.
- Itens dos 14: 3 (calça através da barra da camisa) e 7 (pele e `CUSTOM1` no Compatibility).

## Responsável e arquivos
- Sessão 63f7546d, `wt-pessoas-recozinhar`, branch `pessoas-recozinhar`.
- `tools/assets/cook_people.py`, `tools/assets/people_*.py`, `project/assets/people/*`, `native/src/render/agents/people_*`, `project/render/shaders/people.gdshader`, bloco `people` de `agents.json`.

## Causas comprovadas
- `people.bin` sem commit (sha1 56afdb4f10d9, 00:28) não vinha do cozinheiro comitado: é do cozimento `cook5`; depois dele `people_parts.py` (00:34) e `cook_people.py` (00:47) mudaram, e o `cook6` foi parado na variação 5 de 32 (transcrição 5c00c9fd, 03:56Z).
- Rosto manchado de `14_pessoa`: o interior da cabeça (boca, órbitas, canais do ouvido) tem textura vermelho-escura e preta; a simplificação arrasta essa cor para o rosto. A correção `parts.interior` (`tools/assets/people_parts.py`) está no commit 7f1df85, mas o asset antigo não a tinha.

## Fontes
- MPFB2 `LICENSE.md` @ afb9f530, seção C: malha base, proxies, alvos, modificadores, texturas, roupas, rigs e dados de malha em CC0 1.0; código em GPLv3 (não usado). https://raw.githubusercontent.com/makehumancommunity/mpfb2/afb9f530a7c2741dedb8df0ebae2e0b183caec21/LICENSE.md
- MakeHuman: "All core assets are shared under Creative Commons, CC0", sem atribuição obrigatória. https://static.makehumancommunity.org/about/license.html
- Microsoft Rocketbox: MIT, "Copyright (c) 2020 Microsoft" (`LICENSE.md`), texto obrigatório no pacote. https://github.com/microsoft/Microsoft-Rocketbox

## Decisões
- Recozinhar em vez de comitar o arquivo sem commit: ele não corresponde ao cozinheiro comitado.
- `licenses()` passa a listar os alvos de forma e a declaração CC0: os alvos entram no corpo e estavam fora do `LICENSES.json`.

## Commits
- 6baa0d3 Integra master (ce3b2b5), sem conflito.
- 54b7abb comentários com a regra e o dono; auditoria de origem: 0 pontos.
- f5f474e licenças com alvos de forma e declaração CC0.

## Validações
- Em andamento.

## Pendências e bloqueios
- Fontes do cozinheiro fora do repositório: `C:/Users/Jonathan/Documents/Projects/lab/roadcraft-novo/public/models` (só pacotes de dados; nenhum código aberto).

## Próxima ação
- Comitar o asset recozinhado e rodar a validação da fase 5.
