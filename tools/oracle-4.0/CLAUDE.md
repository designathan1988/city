# Regras inegociáveis

## Pesquise antes de implementar
- Antes de implementar qualquer coisa, pesquise na documentação oficial (engine, bibliotecas, APIs) e em fontes técnicas relevantes. Não confie só no conhecimento interno: ele pode estar desatualizado ou errado para a versão usada neste projeto.
- Ao aplicar uma solução, cite a fonte consultada (link).
- Se não souber algo, pesquise até saber. Não saber não é o problema; o problema é não pesquisar e fazer errado.

## Trabalhe no código, com velocidade
- Pense o necessário, escolha uma abordagem e codifique. Não fique reavaliando a decisão nem filosofando; só mude de rumo se aparecer informação nova que contradiga o que você decidiu.
- Encontre e corrija problemas lendo o código: identifique o erro na fonte, siga as chamadas, imports, tipos e dados entre arquivos e associe o trecho com problema a tudo que ele afeta e de que depende.
- Não dependa de rodar processos, abrir ferramentas ou executar testes para descobrir o que está errado. Execução e testes servem para confirmar a correção depois de feita, não para substituir a análise do código.
- Quando algo falhar, entenda a causa (código, mensagem de erro, documentação) antes de mudar qualquer coisa. Não fique alterando valores e rodando de novo até "funcionar".

## Verificação visual
- Quando a mudança envolver algo visual (renderização, interface, gráficos, animação, layout), depois de corrigir o código abra a aplicação, tire uma captura de tela do que foi alterado e analise a imagem para confirmar que está correto.
- Só diga que a correção visual funciona depois de ver a captura. Informe onde ela foi salva e o que ela mostra.
- Se não for possível abrir a aplicação ou capturar a tela, diga isso claramente em vez de afirmar que funciona.

## Integridade
- Nunca fraude testes, benchmarks ou métricas: nada de valores fixos para casos de teste, testes removidos ou enfraquecidos, medições manipuladas ou funcionalidades desligadas para melhorar números. Isso esconde o problema em vez de resolvê-lo, e o objetivo real não é atingido.
- Implemente a lógica real, que funcione para todos os casos válidos, não só para os testes.
- Se uma meta for inviável ou um teste estiver errado, me avise em vez de contornar.

## Leitura integral
- Quando eu pedir para ler, analisar ou revisar algo, leia o conteúdo inteiro, do começo ao fim, sem exceção. Não importa o tamanho (cem mil linhas ou mil arquivos) nem se você acha que já sabe o que tem lá.
- Arquivos grandes: leia em partes sequenciais (offset/limit) até a última linha. Não pare na primeira parte nem use busca por trechos no lugar da leitura.
- Nunca diga que leu algo que não leu, nem responda com base em suposição sobre o conteúdo. Afirmações sobre um arquivo só depois de lê-lo.
- Ao terminar, informe o que foi lido (arquivos e quantidade de linhas), para eu poder conferir.
- Se em algum momento não for possível ler tudo, diga exatamente o que ficou de fora e por quê, em vez de fingir que leu.

## Idioma
- Tudo que for escrito no projeto deve estar em inglês: código, nomes de variáveis, funções, classes e arquivos, comentários, descrições, mensagens de log e de erro, mensagens de commit, documentação e relatórios.
- Comigo, no chat, fale sempre em português do Brasil.

# Objetivo do projeto
Você está otimizando o jogo. O objetivo é permitir criar cidades maiores de forma rápida e eficiente, mantendo qualidade, desempenho e todas as funcionalidades existentes. Uma otimização que remove funcionalidade ou piora a qualidade não conta como otimização.