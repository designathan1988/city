# Prompt: pesquisar a arquitetura correta de um city builder em escala

Pesquise a fundo qual é a arquitetura correta de um jogo de construção de cidades, e escreva o plano de como construí-lo, passo a passo.

O jogo tem os recursos do Roadcraft (lista abaixo) e precisa de:
- cidades grandes;
- prédios procedurais gerados durante o jogo;
- qualidade gráfica máxima;
- desempenho altíssimo;
- **baixo consumo de CPU, GPU e RAM, sem abrir mão de nenhuma especificação** desta lista.

Esta tarefa é **só pesquisa**:
- não analise o código de nenhum projeto;
- não escreva código do jogo;
- não rode protótipo, benchmark nem teste.

A entrega é o relatório e o plano.

O projeto anterior (o Roadcraft) tentou isso e errou em muitos pontos, todos listados na seção "O que deu errado no Roadcraft". Para cada um, descubra a forma correta de resolver, com fontes, e garanta que a arquitetura proposta torna aquele erro impossível.

## O que o jogo precisa ter (requisitos, nenhum negociável)

### Escala e plataforma
- **Uma cidade de 10 km x 10 km com cerca de 250 mil prédios procedurais**, e mapas maiores se a arquitetura permitir.
- O jogador **cria** uma cidade nova (semente aleatória), **salva**, **fecha**, **reabre**, **edita** e **joga**. Tudo rápido.
  - A primeira abertura depois de instalar e a primeira depois de uma atualização do jogo também são rápidas.
- **Um PC mediano:** GPU da classe RTX 3060, 16 a 32 GB de RAM. Também tem de rodar, com qualidade ajustada, nas GPUs comuns da pesquisa de hardware da Steam.
- Vendido na **Steam**: tem de funcionar em qualquer GPU e driver dos jogadores, sem erro de shader nem de limite.
- **Baixo consumo:** CPU quase parada com o jogo parado, RAM e memória de vídeo com teto fixo que não cresce com o tempo de jogo nem com a área já visitada, e GPU gasta só com o que aparece na tela.

### O que é proibido como solução
- **Assar prédios:** os prédios são gerados durante o jogo e editados pelo jogador. Nada de HLOD assado, atlas de prédios, impostor de prédio ou cidade pronta em arquivo para abrir rápido.
- **Cidade cozida ou cacheada** como o jeito de abrir rápido. Um cache só pode acelerar; ele nunca pode ser necessário.
- **Cortar detalhe gráfico ou funcionalidade:** qualidade reduzida, efeito desligado, distância de desenho encurtada, objetos que somem, funcionalidade a menos.
- **Elementos falsos:** caixa no lugar de prédio, plano no lugar de via, billboard no lugar de objeto, cena montada só para passar numa checagem.
- **Nível de detalhe que não é o próprio modelo.** Um nível mais simples só vale como simplificação do mesmo objeto, aceito pelo erro em pixels na distância em que é desenhado e pela silhueta. A aparência de longe tem de ser a mesma de perto, sem cor fixada à mão.

### Os recursos do Roadcraft (pelo menos estes; descubra na pesquisa o que mais um jogo desses precisa)
- **Vias:** ferramenta reta, curva e livre; perfis com faixas, canteiro, ciclovia e estacionamento; elevação, pontes, viadutos e túneis; marcações no asfalto.
- **Cruzamentos:** semáforos, pare e preferência, rotatórias e minirrotatórias, faixas de pedestre, regras de conversão e conexões de faixa feitas à mão.
- **Terreno e água:** pincel de relevo, rios, lagos e mar.
- **Zonas e lotes:** zonas residencial, comercial, industrial e mista, com densidades; lotes em polígono, divididos e curvados; crescimento da cidade por demanda.
- **Prédios procedurais:**
  - fachadas por gramática (janelas, varandas, brises, pilastras, persianas);
  - telhados;
  - interiores com cômodos, móveis e luzes acesas à noite;
  - lotes mobiliados: muros, portões, piscinas, jardins;
  - prédios marcantes;
  - desgaste e reforma.
- **Construtor de prédios:** andares, fachadas, telhados, peças e cores, com uma biblioteca de projetos.
- **Paisagem:** árvores e florestas, vegetação rasteira e jardins.
- **Mobiliário de rua:** postes e fios, iluminação, bancos, lixeiras, hidrantes, placas com nomes de rua, sinais.
- **Pessoas:** pedestres, ciclistas, grupos e um criador de pessoas. Têm de ter boa aparência de perto.
- **Veículos:** carros, ônibus, caminhões, motos, bicicletas, trens e metrô.
- **Simulação:**
  - tráfego detalhado perto da câmera e agregado no resto do mapa;
  - pedestres;
  - linhas de transporte com estações e passagens de nível com cancela;
  - economia, com custo das vias.
- **Demolição, destruição e explosões.**
- **Céu e luz:** dia e noite, clima com chuva, sombras, oclusão ambiente, bloom e anti-serrilhado.
- **Câmeras:** livre, isométrica e de pedestre na rua.
- **Save, autosave, exportar e importar mapa, desfazer e refazer.** Um save nunca pode se perder.
- **Interface** com fotos prontas nos menus, nunca desenhadas ao vivo.

## A pergunta da tecnologia

Descubra, com fontes, **qual tecnologia permite fazer tudo isso direito**, e responda sem viés:
- **Dá para fazer com three.js?** Avalie o WebGLRenderer e o WebGPURenderer da versão atual, rodando no Electron para a Steam:
  - o que cada um tem e o que falta (desenho indireto, multi-draw, compute, descarte na GPU);
  - os limites do WebGL2 e do WebGPU nas GPUs da Steam;
  - o teto de memória do V8 por processo;
  - os workers e o SharedArrayBuffer.
- **Ou é melhor Godot?** Avalie a versão 4 atual:
  - o renderizador (Forward+, Vulkan, D3D12), MultiMesh, RenderingServer, compute shaders e descarte por oclusão;
  - as threads, GDScript contra C# contra GDExtension em C++;
  - como ele lida com cenas enormes geradas durante o jogo e a exportação para a Steam.
- **Ou Unity?** Avalie:
  - DOTS (Entities, Jobs, Burst), Entities Graphics, BatchRendererGroup e o GPU Resident Drawer;
  - o que o Cities: Skylines II mostrou feito com ela.
- **Ou WebGPU direto, sem engine** (ou com uma camada fina), e outras tecnologias que a pesquisa achar.

A preferência do dono é entre **three.js, Godot ou WebGPU**, e tecnologias em que **o Claude consiga programar bem**. Esse é um critério de verdade, a avaliar com evidência:
- código em texto, tipado e revisável;
- projeto que não dependa de editor gráfico nem de arquivos binários para funcionar;
- documentação e exemplos abundantes;
- erros legíveis;
- ferramentas de linha de comando para build, checagem de tipos e testes.

Para cada opção, dê uma tabela com, para cada requisito acima:
- se ela atende, atende em parte ou não atende;
- a fonte;
- o custo de contornar o que falta.

No fim, a decisão com o porquê, e a segunda melhor opção com o motivo de ter perdido.

## O que a pesquisa tem de descobrir

1. **Como os jogos e os artigos fazem cidades enormes procedurais em tempo real.** Pelo menos:
   - gramáticas de forma (Müller, Wonka et al., "Procedural Modeling of Buildings", SIGGRAPH 2006);
   - a geração e o desenho de cidades inteiras na GPU, com o nível de detalhe como profundidade da gramática (Steinberger et al., Eurographics 2014);
   - a avaliação da fachada por pixel no shader (Haegler et al., "Grammar-based Encoding of Facades", EGSR 2010);
   - o mundo dividido em células carregadas em volta da câmera (City Sample e World Partition do Unreal);
   - o que o Cities: Skylines I e II, o SimCity de 2013 (GlassBox) e outros city builders lançados fizeram, e onde falharam. Por exemplo, a análise do C:S II: https://blog.paavo.me/cities-skylines-2-performance/.
2. **A arquitetura completa**, camada por camada, com o porquê de cada escolha:
   - **dados:** o que fica guardado para o mapa inteiro, em que formato e quantos bytes por prédio, por via e por lote; o que existe só perto da câmera;
   - **geração:** onde e quando um prédio é gerado a partir dos parâmetros, determinística e em paralelo, e como a edição do jogador entra;
   - **células e streaming:** o tamanho, os orçamentos fixos de CPU e GPU, o que entra e sai e quando;
   - **desenho:** descarte por frustum e por oclusão (Hi-Z) na GPU, desenho indireto e instanciado, os níveis de detalhe, as sombras por tamanho e distância, os materiais e os shaders com poucas variantes, todos compilados antes de jogar, e o pós-processamento com custo baixo;
   - **simulação:** quem é dono de cada dado, as threads ou os processos, o tráfego meso e micro com a passagem na borda, o roteamento e o nível de detalhe da simulação;
   - **save e abertura:** o save só das células alteradas, a escrita atômica com cópia anterior e verificação, as versões e a migração, e abrir lendo só o compacto e montando só o que está na vista;
   - **edição e desfazer:** o custo de uma edição proporcional ao que ela muda;
   - **memória:** o teto de cada processo e de cada tipo de dado, e o que nunca pode ser copiado inteiro.
3. **Os orçamentos de uma cidade de 10 km com 250 mil prédios**, somados camada por camada, dentro dos limites do PC mediano. Por exemplo:
   - os MB de RAM e de memória de vídeo;
   - os ms de quadro na CPU e na GPU;
   - as chamadas de desenho e os triângulos na tela;
   - o tempo de criar, de abrir e de salvar.

   Cada número com a fonte ou a conta que o produz.
4. **A forma correta de resolver cada erro** da seção abaixo, com fontes.
5. **O plano passo a passo** para construir o jogo nessa arquitetura:
   - a ordem dos marcos;
   - o que cada um entrega;
   - o critério de aceitação de cada um.

   O critério sempre inclui a cidade de 10 km nova, com semente aleatória, salva, fechada, reaberta sem cache e jogada na janela visível, com os números contra os limites e as fotos. A cidade é de 10 km desde o primeiro marco, nunca uma cidade pequena "para escalar depois".
6. **Os riscos e o que não foi possível confirmar**, ditos com clareza.

## O que deu errado no Roadcraft (não repetir; pesquise a solução certa de cada um)

### Arquitetura
1. **O mapa inteiro como um documento de objetos na thread da página**, trocado inteiro ao abrir, com a rede de vias refeita inteira e os índices espaciais do mapa todo refeitos num laço. A abertura avisava "o mapa inteiro mudou" para todos os sistemas, e cada um recomeçava do mapa inteiro.
2. **O streaming por regiões foi escrito e nunca ligado.** O jogo tinha um modo só: o mapa inteiro na memória, cortado depois por "janelas".
3. **Prédios guardados já montados:** volumes, elementos, materiais e mobília de cada prédio no save. Eram 687 MB para 249.122 prédios, cerca de 2,75 KB por prédio, em vez de alguns bytes de parâmetros.
4. **Uma cidade nova gerada num processo, montada inteira e copiada inteira entre processos.** Uma cidade de 2,7 km chegou a 10,1 GB de RAM em 31 s. Antes disso, a de 10 km levava 232 s gerando num worker só.
5. **A simulação com a sua própria cópia do mapa inteiro**, além da cópia da página. O mapa ficava pelo menos duas vezes na memória.
6. **Save e abertura sobre o mapa inteiro:** as vias viravam JSON numa string só, e cada quadrado era copiado duas vezes ao abrir. Ler o save de 10 km levava 27 s; uma reabertura chegou a 190-207 s.
7. **Toda edição de via recalculava as alturas de todas as vias do mapa**, a cada pausa do ponteiro e a cada clique.
8. **O cache dos dados derivados era apagado inteiro a cada versão do jogo.** A mesma cidade abria em 24 s com o cache e em 146 s sem ele. Refazendo o cache, os processos de trabalho chegavam a cerca de 10 GB. O cache escondia o caminho lento.
9. **Lentidão escondida em vez de resolvida:** a primeira cidade lida de arquivos prontos e a cidade seguinte gerada de antemão em segundo plano.
10. **O save sumiu:** o banco do jogo perdeu a tabela com a cidade de 249.122 prédios, sem que se soubesse quando. Antes, uma edição feita com a janela do jogo aberta gravou um mapa vazio por cima do save do dono, e uma via desenhada num teste ficou gravada na cidade dele.
11. **Um renderer de 4.355 linhas na thread da página**, dono de todas as camadas, com o nível de longe das vias pedido para o mapa inteiro e todos os emissores de sombra percorridos a cada quadro.
12. **Trabalho proporcional ao mapa no caminho do quadro, ou refeito sem mudança:**
    - placas e ladrilhos do mapa todo visitados a cada quadro;
    - filas inteiras reordenadas a cada quadro;
    - lotes, postes, objetos e superfícies das vias calculados na página durante o movimento;
    - pessoas carregadas e animações assadas na thread da página quando apareciam;
    - a cena percorrida três vezes por quadro;
    - matrizes de objetos parados recalculadas;
    - sombras redesenhadas com tudo parado;
    - contadores e medidores rodando no quadro;
    - o profiler ligado desde o início, capturando pilhas de chamada.
13. **Envio à GPU sem controle:**
    - texturas de dados reenviadas inteiras por uma linha alterada: uma de 9 MB, 9.325 vezes por segundo, ou 87 GB/s;
    - a textura indireta do BatchedMesh do three reenviada a cada desenho de cada câmera;
    - buffers reenviados com o mesmo conteúdo;
    - geometria recriada com a vista parada;
    - buffers enormes realocados e copiados ao crescer;
    - leituras síncronas da GPU;
    - formatos de vértice de 3 componentes que o ANGLE converte na CPU.
14. **Shaders compilados no meio do jogo:** de 3 a 34 por movimento de câmera, com esperas de até segundos lendo o status do programa. Mudar uma opção recompilava dezenas.
15. **Descarte e desenho objeto por objeto na CPU:** milhares de objetos por quadro, cada um com teste de frustum, troca de estado e envio de uniform, de novo em cada cascata de sombra. O three r186 com WebGL não tinha como fazer isso na GPU, e vários passos do plano guiado pela GPU se provaram impossíveis nele.
16. **Níveis de detalhe errados:**
    - prédios de longe como caixas, ou como uma segunda representação que não vinha do modelo de perto: a cor de longe errava de 2 a 8 em CIEDE2000;
    - o nível escolhido pelo que tinha carregado, não pelo tamanho na tela: a mesma coisa aparecia de três formas na mesma vista;
    - um nível com mais triângulos que pixels;
    - objetos de longe desenhados com o modelo de perto;
    - modelos sem nível intermediário: um carro passava de 8 mil para 36 triângulos de uma vez.
17. **Modelos pesados:**
    - carros com 10 a 15 mil triângulos;
    - pessoas com 28 a 45 mil;
    - grades de 7 mil triângulos a cada 70 m;
    - geometria sem índice;
    - faces escondidas;
    - vértices de 36 a 128 bytes sem quantização;
    - geometrias iguais guardadas várias vezes.
18. **Capacidades fixas e trabalho desperdiçado com o que não aparece:**
    - carros de densidade com teto de 262.144 e os de dentro da janela de simulação ainda enviados ao desenho, dobrados num ponto;
    - árvores e objetos descartados no vertex shader depois de enviados: 1,8 milhão de triângulos por quadro jogados fora.

    Na tentativa de refazer o jogo (roadcraft-novo), o conjunto de objetos de rua descartava em silêncio o que não cabia (a 24 km faltavam 73%), e os pedestres paravam num teto fixo de 100 mil. Um limite cheio tem de ser dito e resolvido, nunca cortar a cidade sem aviso.
19. **Memória que crescia sem parar:** o heap subindo de 300 a 1.400 MB por minuto, malhas trocadas nunca liberadas, memória de vídeo vazando a cada troca de opção, e a memória crescendo com a área já visitada.
20. **Pós-processamento caro em resolução cheia:** nuvens de 8 a 15 ms por quadro, GTAO de 2,5 a 10 ms, SMAA de 8 ms na vista de longe.
21. **Interface desenhada ao vivo:** fotos de cada opção do construtor renderizadas num segundo contexto WebGL, a 4 quadros por segundo.

### Tentativas que falharam e não podem voltar
22. Uma fila de envio à GPU com teto de bytes por quadro: vias novas não apareciam.
23. A cena congelada (atualização de matrizes desligada para tudo).
24. Um banco de dados do grafo de código com placar de regras, que consumiu horas e quebrou o jogo.
25. Modelos "otimizados" que não eram o mesmo modelo: caixas no lugar de objetos de rua, plantas com outra copa, pneus facetados, vias com leques de triângulos finos.
26. Uma interface "nova" que só recoloria a antiga.
27. Compensar no shader a cor de longe para bater com a de perto.

### Fraude e prova falsa
28. Uma semente cozida apresentada como o jogo funcionando.
29. Uma cidade de 2,7 km chamada de 10 km, com os números dela.
30. "É o cache", "é o ambiente" ou "é o driver" sem prova.
31. Medição em janela coberta ou minimizada, onde o navegador desenha cerca de 1 quadro por segundo.
32. Testes automáticos e passadas sem janela usados como prova de que o jogo funcionava, enquanto o jogo real quebrava, e documentos dizendo "pronto" sobre o que não estava.
33. Testes enfraquecidos, métricas fixadas à mão, funcionalidades desligadas e qualidade reduzida para melhorar números.

### Processo
34. Remendos em cima da arquitetura errada, em vez de refazer a parte errada.
35. Muitas frentes ao mesmo tempo e várias sessões e agentes na mesma pasta: centenas de commits em poucas horas, nenhuma frente terminada e provada.
36. Medições, bisects e suítes lentas usados para achar causas, em vez de ler e entender.
37. Commits restaurados várias vezes sem abrir o jogo depois.
38. O contexto e as ordens do dono perdidos na compactação.

## Como pesquisar

1. **Guarde tudo em arquivos** numa pasta nova: `C:\Users\Jonathan\Documents\Pesquisa\city-builder-architecture`.
   - `map.md`: o mapa do assunto, cada parte com `[ ]` na frente. Monte-o primeiro, a partir dos requisitos, das perguntas e dos erros acima, e acrescente o que as fontes revelarem.
   - `sources.md`: cada fonte, com o link, a data ou a versão e o que ela decide.
   - `notes.md`: os achados, escritos enquanto lê.
   - `report.md`: o relatório final.
2. **Regras garantidas pelo programa**, em `.claude/settings.json` dessa pasta (https://code.claude.com/docs/en/permissions, https://code.claude.com/docs/en/hooks):
   - `"permissions": { "deny": ["Agent", "Read(//c/Users/Jonathan/Documents/Projects/**)"] }`: sem subagentes e sem ler o código de nenhum projeto. No Windows, `C:\` se escreve `/c/` nas regras;
   - um hook `Stop` que sai com código 2 enquanto o `map.md` tiver um `[ ]`;
   - um hook `SessionStart` que imprime o `map.md` e o fim do `notes.md` a cada início e compactação.
3. **Fontes primárias primeiro:**
   - a documentação oficial da versão atual de cada tecnologia;
   - as notas de versão e os issues dos repositórios oficiais;
   - os artigos científicos;
   - as palestras técnicas de jogos lançados (GDC, SIGGRAPH, Digital Foundry, os blogs dos estúdios);
   - as análises de desempenho de jogos lançados.

   Blogs e fóruns servem para achar armadilhas, e cada coisa que vier deles é conferida numa fonte primária.
4. **Abra cada página e leia-a inteira** (WebFetch, em partes se for longa). O resumo de uma busca não conta como pesquisa, e o que você sabia antes também não. Cada afirmação no relatório tem o link e a versão.
5. **Conflitos e lacunas:** quando duas fontes discordam, diga quais, qual vale e por quê. O que não for encontrado fica como lacuna, nunca preenchido com suposição.
6. **Hipóteses:** mantenha no `notes.md` as hipóteses sobre a arquitetura e a tecnologia, com o grau de confiança de cada uma, e atualize-as conforme as fontes confirmam ou derrubam.

## O relatório final (`report.md`)

1. **A tecnologia escolhida**, com a tabela por requisito de cada opção, as fontes, o porquê e a segunda melhor.
2. **A arquitetura**, camada por camada: o que é, como funciona, por que é assim e o que ela garante, com as fontes.
3. **Os orçamentos** da cidade de 10 km com 250 mil prédios, somados dentro dos limites do PC mediano.
4. **Para cada um dos 38 erros:** a forma correta de resolver, com a fonte, e como a arquitetura proposta o torna impossível.
5. **Para cada requisito:** onde ele vive na arquitetura e como cumpre os limites sem cortar nada.
6. **O plano passo a passo**, marco por marco, com o que cada um entrega e o critério de aceitação.
7. **Os riscos, as lacunas e o que não foi possível confirmar.**

## Regras

- Só pesquisa: nenhum código, nenhum protótipo, nenhum benchmark, nenhuma leitura de código de projeto.
- Faça você mesmo, sem subagentes. Decida sozinho o que for de rotina.
- Nunca afirme algo sem a fonte, nunca diga que leu uma página que não abriu e nunca apresente suposição como fato.
- Os arquivos da pesquisa em inglês; com o dono, português do Brasil.
- No fim, informe: as partes do mapa cobertas (X de Y), as fontes lidas (quantas e as principais), a tecnologia escolhida com o motivo em poucas linhas, onde está o relatório e as lacunas.
