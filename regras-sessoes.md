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
7. Câmera do par (achado das Portes 1 e 2, 2026-11-04, conferido em `src/render/isoViewport.ts` do 4.0): a câmera em perspectiva do 4.0 é uma `PerspectiveCamera` de `PERSPECTIVE_FOV = 35` graus; o alvo é `(x, 0, -y)`, ou seja, o `y` do mapa do 4.0 é `-z` no jogo; a inclinação vem de um perfil que depende do zoom (`tiltNow`, `profileTilt`) e há um puxão que afasta o olho de um prédio (`PULL_*`, o Deoccluder do Cinemachine). Por isso a pose de um par sai do `viewCamera` do 4.0 (alvo, zoom, azimute e elevação), como em `ports/build/evidence/resposta-par.md`, e não da lente, da altura ou do sentido do eixo do jogo. Par com a câmera do jogo no lugar da do 4.0 não é par (item 4).

## 10. Estilos regionais (ordem do usuário, 2026-11-04: o jogo é vendido na Steam)

Nada pode ficar preso ao Brasil: norma, sinalização, marcação, veículo, placa, unidade, nome de rua, fachada, nada. O Brasil é um estilo entre vários, e o jogador escolhe.

1. Existe um **estilo regional por cidade**, escolhido pelo jogador ao começar a cidade e gravado no save. Estilos de partida: `roadcraft` (o do 4.0, padrão), `brasil`, `eua` e `europa`. A lista cresce só por dados.
2. Todo valor que muda com o estilo (marcação, largura de faixa, placa, semáforo, hidrante, caixa de correio, poste, paleta e placa do veículo, lado da mão, unidade de velocidade, fachada, nome de rua) vive em dados, numa seção `styles.<id>` da tabela do seu dono. O código lê só pelo dono único do estilo ativo. Nenhum `if` de país no código.
3. Fonte primária por estilo e por constante, como no resto. Brasil: CTB, CONTRAN, MBST, NBR, DNIT. EUA: MUTCD (FHWA) e AASHTO Green Book. Europa: Convenção de Viena sobre Sinalização Rodoviária (1968) e as normas europeias aplicáveis (por exemplo EN 1436 para marcas horizontais). Valor de um estilo não vale para outro.
4. Esta rodada: cada porte deixa pronta a estrutura `styles.<id>` do seu domínio, com `roadcraft` e `brasil` preenchidos pelo que já tem; `eua` e `europa` entram em seguida, por quem a coordenadora designar. Mão de direção é propriedade do estilo (começa só com mão direita, sem código que a suponha onde puder ser dado).
5. Enquanto o mecanismo do estilo ativo não existe (id no `CityState`, bloco do save, leitura única, seletor na tela de nova cidade), o carregador do seu domínio lê o estilo `roadcraft`. O seletor liga depois, sem mudar o seu código.

## 11. Visual e construção: paridade com o 4.0, causa por causa (ordem do usuário, 2026-11-04)
O usuário viu fotos do jogo (cidade inteira, avenida, calçada) e disse que está feio e longe do 4.0: não é só cor e sombra, é detalhe e construção. Vale para toda sessão. Cada item é de um dono; a causa se prova por medição e foto no mesmo enquadramento e na mesma hora do relógio, e se corrige na causa (nunca o efeito, nunca o remendo).
1. Medido na cidade inteira (4.0 `ports/interface/reference/87-demolish.png`, relógio 12:06; jogo `ports/interface/evidence/shell/shell_drawer_demolish.png`, relógio 06:39; o clone da Porte 4 não tem a luz da Porte 3). Terreno: 4.0 RGB 64,88,50 (sat 0,43; B/R 0,78), jogo 103,121,97 (sat 0,21; B/R 0,94). Cidade: luminância p5/p50/p95 do 4.0 ~30/90/193 (sat 0,31), do jogo ~95-109/136-207/211-228 (sat 0,12-0,18): as sombras do jogo não descem de ~100. Script que repete a medida: `registro/medir_vista.py` (caminhos das fotos dentro dele).
2. No código: `project/render/city_world.gd` usa ambiente de cor chapada (`AMBIENT_SOURCE_COLOR`, `ambient_light_sky_contribution = 0`) e `ssao_enabled = false`. O 4.0 tem hemisfério céu/chão (0xe6e4da / 0x6a6b4c, 0,8), GTAO (raio 1,0; escala 1,05; mistura 0,85; 8 amostras), `canyonLight`, MSAA e SMAA (`src/render/environment.ts`, `postprocess.ts`, `canyonLight.ts`). O preset padrão do jogo (`medium`, `data/game/settings.json`) é MSAA 2x; não há SMAA, TAA nem FXAA em `project/`.
3. O que as fotos mostram, por dono. Porte 3: copas com bloco facetado marrom aparecendo sob as folhas, troncos de caixa lisa, árvores distantes como elipses escuras, terreno cinza-oliva liso, ambiente sem direção nem oclusão, serrilhado e pontilhado de janela. Porte 2: miolo da cidade sem a densidade e as torres do 4.0, topos creme em vez de escuros com equipamentos, janelas distantes em pontilhado preto sobre branco, parede e azulejo com degraus de pixel, janelas sem relevo, beirais e platibandas ausentes, arbustos de cubo, gramado de lote liso, muros chapados. Porte 1: manchas circulares grandes no asfalto, canteiro liso, calçada quase branca de juntas apagadas, meio-fio liso. Porte 6: tronco de rua como caixa, calçada sem sombra de poste, placa e tronco, poste de placa liso. Porte 4: pares de foto com horas diferentes (12:06 contra 06:39) não provam paridade de visual.
4. Cada dono entrega por escrito: tudo o que o 4.0 desenha naquele ângulo; recorte 3x lado a lado, mesma hora; causa medida (sonda, contagem, albedo, escala, custo de GPU); correção no dono; fotos depois analisadas. Sem 'pronto' sem isso.
5. Mesmo sol, não só a mesma hora na barra: no modo dia do 4.0 o céu e o sol ficam fixos às 16:00 (`src/render/renderer.ts`, `fixedHour = skyMode === 'day' ? 16 * 60`), seja qual for o relógio que a barra mostre (a foto do 4.0 pode marcar 12:06 com sol das 16:00). Para foto de mundo (luz, sombra, cor, vista), ponha o relógio do jogo em 16:00 e escreva isso na legenda; a hora da barra só vale como legenda de foto de interface. Quem comparou mundo com sol de meio-dia contra o 4.0 refaz o par.

## 12. Diretório de usuário do Godot por clone (2026-11-04)
O Godot nomeia `user://` pelo projeto: todos os clones e o jogo do usuário usavam a mesma pasta `Roadcraft`, com o mesmo `settings.cfg`, os mesmos saves e o cache de pipelines do Vulkan (~107 MiB, lido inteiro para a memória estática e reescrito por quem rodou por último). Isso moveu medidas de memória e fotos (a Porte 4 achou um preset gravado por outro clone). Regra: cada clone tem `project/override.cfg`, fora do git (`.git/info/exclude`), com `[application]`, `config/use_custom_user_dir=true` e `config/custom_user_dir_name="Roadcraft-<feature>"` (Godot: File paths in Godot projects; ProjectSettings; override.cfg vale para qualquer configuração do projeto). Clones novos já nascem assim (`converter workspace`, commit f8b8949 do repositório do conversor). Clone que já existe: a própria sessão cria o arquivo no começo de uma rodada nova de medida e escreve no relatório que daí em diante a pasta é própria; medida anterior vale com a ressalva do cache compartilhado. Nenhuma sessão usa o `user://` do jogo do usuário.

## 13. Quem faz o `canyonLight` do 4.0 (decisão da coordenadora, 2026-11-04)
A Porte 2 mediu, no mesmo prédio e na mesma pose (`ports/build/evidence/resposta-par.md`): parede ao sol bate com o 4.0 (torre bege 222,213,194 contra 222,211,187), parede na sombra fica mais clara e mais azul no jogo (torre azul em sombra 100,120,138 contra 72,79,84; torre bege na base 168,166,150 contra 152,147,123). A causa provável, ainda não provada, é que falta o passe `render/canyonLight.ts` do 4.0, que escurece o fundo das ruas entre prédios (post-processo sobre o buffer de profundidade, com um campo de alturas dos sólidos da cidade em janela de 2048 m e células de 4 m, o GTAO e a direção do sol; li as primeiras 60 das 378 linhas do arquivo, o resto está por ler).
- **Dono:** Porte 3 (luz). É um passe de iluminação global, não de prédio.
- **Dados de prédio:** a Porte 2 fornece a leitura das pegadas e das alturas dos prédios (somente leitura, no estilo `HeightPatch`) quando a Porte 3 pedir; a Porte 3 não mexe em `building_*`.
- **Regra:** funcionalidade, não código: ler o 4.0 inteiro, pesquisar o método (oclusão de céu por campo de alturas, Godot 4.7 `CompositorEffect` ou integração no shader do prédio e da via), medir custo de GPU e de memória, e provar com a tabela de paredes da Porte 2 (sol e sombra, mesmo prédio, mesma pose) e com o percentil 5 da foto.
- **Ordem:** depois de entregar o par de árvores na pose do 4.0 e os 18 pares, antes das fotos finais da Porte 2.

## 14. A base que anda: o `fonte` (master) recebe só o que foi provado (2026-11-04)
O usuário mandou integrar a luz da Porte 3 e, logo depois, a cidade da Porte 2, antes das fotos finais. Isso está feito no `fonte` (master `56a7864`) e muda o trabalho de todas as filhas.
- **Como a coordenadora integra:** em `ports/integracao/workspace` (branch `convert/integracao`, `user://` próprio), nunca no `fonte`: merge da branch da filha, gates, `verify` (builds de release e de debug, import, testes, captura), soak e fotos lidas uma a uma; só então `git merge --ff-only` no `fonte`. Evidência em `ports/integracao/evidence/<commit>/`.
- **O que cada filha faz quando eu mandar (primeira tarefa da rodada, antes de qualquer item novo):** no seu clone, `git fetch origin master && git merge origin/master`. Conflito que é soma dos dois lados (listas, enumerações, tabelas de dados, mapa e pendências do `CLAUDE.md`, schemas): mantenha os dois. Conflito em que as duas mudaram o mesmo comportamento: pare e me avise com os dois trechos, não escolha às cegas. Depois: gates, build de release e de debug com `-j 4`, testes; commit "Merge master (<resumo>)"; sem push.
- **Fotos finais só na base integrada,** na mesma hora do dia (16h) e no mesmo enquadramento do 4.0 (seção 9). Foto da base antiga não vale como prova de paridade nem de regressão.
- **Carga:** a seção 5 limita a três filhas compilando ao mesmo tempo, e um merge de master recompila tudo. Por isso eu chamo as filhas por ondas e digo a cada uma quando é a vez dela; a que ainda não foi chamada segue no ritmo próprio, na base antiga.
