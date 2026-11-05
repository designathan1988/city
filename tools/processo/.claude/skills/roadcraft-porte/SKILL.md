---
name: roadcraft-porte
description: Portar uma funcionalidade do Roadcraft 4.0 (TypeScript/three.js) para o jogo (Godot 4.7 + núcleo C++) com o inventário funcional automático (registro/mapa40) e o processo do projeto. Use ao começar ou continuar um porte, ao escolher a próxima funcionalidade, ou quando a tarefa citar o 4.0, ports/, converter/ ou "funcionalidade, nunca código".
hooks:
  Stop:
    - hooks:
        - type: prompt
          timeout: 45
          prompt: "Você julga se uma sessão de porte do Roadcraft pode encerrar o turno. Entrada: $ARGUMENTS. A sessão só pode parar quando a última mensagem (a) diz o que foi verificado com o caminho da evidência (log, foto, relatório, inventário) e o que NÃO foi verificado, sem afirmar 'pronto', 'lido' ou 'passou' sem evidência citada; e (b) não termina em plano, promessa ('vou', 'o próximo é'), pergunta de confirmação para ação já autorizada pelo processo, ou lista de passos que a própria sessão poderia executar agora. Se ela pode e deve continuar, responda {\"ok\": false, \"reason\": \"o que executar agora\"}. Se a condição é impossível sem o usuário (decisão de produto, arquivo protegido, máquina ocupada declarada), responda {\"ok\": true}. Caso contrário {\"ok\": true}."
        - type: agent
          timeout: 120
          prompt: "Verifique nos arquivos, sem acreditar no texto, as afirmações da última mensagem desta sessão de porte do Roadcraft (entrada: $ARGUMENTS). Para cada arquivo, relatório, foto, log ou commit que a mensagem diz ter criado, atualizado ou lido: confira que existe (Glob/Read) e que o conteúdo corresponde (um relatório citado como 'completo' tem as seções; um log de teste citado não contém 'SCRIPT ERROR' nem 'FAIL'; 'Gates OK' citado tem um log ou saída correspondente). Confira também registro/leitura/cobertura.md: se a mensagem afirma ter lido algo que lá aparece incompleto, é falso. Responda {\"ok\": true} quando tudo confere, ou {\"ok\": false, \"reason\": \"<afirmação sem prova e o que fazer>\"}."
---

# Porte de uma funcionalidade do 4.0 (Roadcraft)

Funcionalidade, nunca código: o 4.0 é referência de comportamento e visual; a implementação é a da arquitetura
atual (`core` → `sim` → `render` → `api`, `CityState`, `ChangeLog`, `JobPool`, dados em `data/**` com schema, gates).

## 1. Escolha e rastreamento, sem caça manual
- `registro/mapa40/indice.md`: inventário funcional automático do 4.0 (308 funcionalidades, geradas de `grafo.json`
  por `ferramentas/mapa40/funcionalidades.cjs`). Cada `especificacoes/<id>.md` traz: sementes, código próprio com
  arquivo e linhas, onde `main.ts` a liga, o que usa e quem a usa, campos de estado lidos e escritos, workers e
  mensagens, textos `t()`, recursos, testes do 4.0 e a situação nos portes (`ports/*/cobertura.md`).
- Regenerar depois de mudança no 4.0: `node ferramentas/mapa40/grafo.cjs` (10 s) e `node ferramentas/mapa40/funcionalidades.cjs`.
- Para o recorte completo com corpo (declarações, fatias, recursos) de uma funcionalidade escolhida:
  `cd converter && node scripts/port.ts extract <feature> <sementes da especificação>` → `converter/out/features/<feature>/`.
- Leia inteiro o que vai portar (skill `roadcraft-leitura`): `feature.md`, `source/` e os arquivos próprios listados.

## 2. Processo (inegociável, nesta ordem)
O portão de processo (`CLAUDE.md` da raiz, `ferramentas/processo/`) impõe estes passos: cada um se registra por `processo.py passo N`, fora de ordem o programa nega, e o passo 5 para a sessão.
1. Análise com medição no jogo atual (sondas, `camera_pan`, capturas) até a causa.
2. Pesquisa na documentação oficial (Godot 4.7, godot-cpp, Terrain3D) e fontes primárias; cada constante com fonte ou medida.
3. Plano completo em `ports/<feature>/plano.md` e `cobertura.md` (uma linha por declaração própria) antes do código.
4. Código sem tentativa e erro; dono único; sem número mágico; mapa e pendências do `CLAUDE.md` na mesma mudança.
5. Teste: `python tools/gates/run_gates.py` → Gates OK; C++ → `MINGW=C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64 JOBS=4 bash tools/build/build.sh core-windows` (release e debug); visível → `bash tools/build/build.sh e2e` (grava `dist/tests`, exigido pelo hook de commit); fotos em par com o 4.0 (`regras-sessoes.md` §9: mesma pose, hora 16h, 1920x1080), lidas em resolução cheia, cada defeito listado e corrigido.
6. Commits por tema, cada um compilando; relatório em `ports/<feature>/relatorio.md` com o que foi e o que não foi verificado.

## 3. Regras de sessão
- Jogo só oculto: `tools/build/hidden_run.ps1 -Command "<cmd>" -WorkDir <repositório> -TimeoutS <s>`, mapa 0, uma instância, fechada pelo PID.
- Nunca editar `tools/gates/*`, schemas existentes nem `.claude/settings*.json` (o hook nega; só o usuário).
- Turno só termina com entrega commitada e relatada, ou com build/teste/foto rodando em segundo plano.
