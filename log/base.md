# Base do jogo: otimização da arquitetura e qualidade (2026-11-06)

Ordem do usuário: otimizar a base (memória, CPU, GPU), pesquisar também assembly, soluções caras, à mão, respeitando a arquitetura do CLAUDE.md. A ferramenta de porte e tradução voltou a ser permitida (2026-11-06); a otimização é sempre à mão. A janela do jogo aberta na tela do usuário nunca é fechada por mim.

## Feito
- `7be4db8a`: vias em relevo, inclinação dos veículos, normais do terreno, janela ao vivo, `free_dll.sh`.
- Compilação (`native/CMakeLists.txt`): x86-64-v3 (AVX2, FMA, BMI), `-ffp-contract=off` (mesmos bits com ou sem FMA), sem errno, `-fno-exceptions`. `-fno-rtti` não dá: o godot-cpp faz `dynamic_cast` nas nossas classes (crash achado no assembly: `__dynamic_cast` lendo `vptr[-1]`). Digest e fotos congeladas iguais bit a bit com AVX2.
- DLL de release: ThinLTO e PGO (`tools/build/pgo.sh` treina escondido e grava `native/pgo/citycore.profdata`; 36,7 G contagens). O perfil dá as funções quentes sem rodada de medida: `llvm-profdata show --topn`.
- `InstanceRows` (`rs_util.h`): as linhas do MultiMesh vivem entre quadros, um `ptrw` por quadro, sem `resize` por instância nem cópia da folga.
- `RowKeeper` (`render/row_keeper.h`): cada agente mantém sua linha, para o vetor de movimento do TAA/FSR2 (o motor lê a linha do quadro anterior).
- Multidão: `sidewalk_choices` em paralelo, cada calçada escreve o próprio trecho: mesma lista, mesma ordem, bit a bit.
- TAA (medium) e FSR2 em resolução cheia (high, ultra): `temporal` e `temporal_sharpness` em `settings.json`.
- Cascatas da sombra por degraus (`_first_m`): o corte não muda mais a cada metro de chão sob o olho.
- Ocupantes: gente de verdade (pedestres cozidos em clipes sentados) nos veículos; bonecos de caixas removidos da malha e o shader do veículo perdeu o `look()`.
- mimalloc em `new`/`delete` do núcleo (`core/memory.cpp`).

## Assembly (pesquisa)
Agner Fog, Optimizing subroutines in assembly: compiladores atuais superam o programador na maioria dos casos; intrínsecos dão as mesmas instruções e mantêm inlining, alocação de registros e propagação de constantes; função em assembly bloqueia isso. Inline asm chegou a custar 40% num caso medido (arXiv 2101.06039). Assembly à mão ganha só em núcleo pequeno onde o agendamento importa. Método adotado: AVX2 pelo compilador e intrínsecos, olhando o assembly gerado (`llvm-objdump`), e `.S` só onde a saída do compilador for ruim.

## O elenco novo de pessoas (feito)
Causa das pessoas feias: nosso `people.bin` veio do pacote antigo (`public/cooked/people`), cor por vértice, roupa e cabelo com borda rasgada. O 4.0 atual tem 16 corpos em `public/models/citizens/people/<id>.bin` + `<id>.ktx2`: malha com atlas 1024, 5.000 e 2.500 triângulos, esqueleto Bip01 de 53 ossos, expressões do rosto e tira de impostor (8 guinadas, 32x64).
- Clipes: `scratchpad/cast/castbake.spec.ts` roda o `bake` do próprio 4.0 (vitest com config de fora, sem tocar a pasta de referência) e grava `<id>.clips` + `index.json`: paleta de 12 floats por osso, coluna a coluna, `frames + 2` linhas; `pelvis`, `head`, `stride`.
- Pele do three: mundo = Σ w·B·(bind·v); `bind` leva o vértice quantizado a metros.
- Ordem dos triângulos: a frente do three é o verso do Godot; com `cull_disabled` o corpo saía visto por dentro, escuro (o cook troca os índices).
- Feito: cook (malha, atlas, paleta) → `people.bin` v4 + `atlas.bin` (DXT5 com mips, comprimido pelo Godot) → `people.gdshader` lê `sampler2DArray` pela variação, corte alfa do cabelo, dois lados → impostor além do nível 1 → capacete na cabeça do clipe (`head`).

## Conferência geral contra o 4.0 (ordem do usuário, 2026-11-06; branch `porte/total`, worktree `city-worktrees/porte-total`)
Os `tools/ports/*/coverage.md` estão velhos (elevação consta como a portar, e está no jogo): a conferência é pela leitura do 4.0 e por foto, área por área. Estado:

| Área | Estado |
|---|---|
| Câmera | feita: molas criticamente amortecidas (`camera_damp.gd`), núcleo consultado uma vez por quadro, teste de solavanco (`camera_walk_cases`) |
| Pessoas (corpos) | feito o elenco; faltam expressões, luz de pele e cabelo, objetos na mão, tingir roupa |
| Ocupantes | feito; faltam olhares (`carDriveMirror`...), portas, entrar e sair animado |
| Veículos | pintura com verniz e traseira na cor do carro; falta remodelar lanternas, rodas e proporções, portas que abrem, `farCarPictures` |
| Fachadas de longe | filtradas (parapeitos, brises, acabamento); falta transição suave entre níveis (dither) |
| Terreno | variação macro e neblina aérea feitas; falta estrato de rocha e sombra de contato |
| Luz | SSIL no alto e no ultra, FSR2, cascatas por degraus |
| Vias | Quarteirões de uma vez feito (`road_blocks.cpp`); faltam editor de perfil, Personalizar, Meus modelos, conectores, regras por acesso |
| Zoneamento e lotes | feitos: lotes do jogador, `onLand`, `cutLotsByRoads`, `lotSnapper`, cor no chão pela camada de zonas (Clipper2 em `sim/lots/lot_land`), prévia do que será feito; falta `planLots`/`applyLots` (proposta de lotes de um quarteirão) |
| Construir › Interior (provado por clique e foto em 2026-11-06, `torre695-andar10`) | FALTA ligar a aba (contrato em `ports/interior/report.md`) |
| Tráfego, pedestres, água, paisagismo, bomba, pistola, transporte, mobília | a conferir, ver Pendências do CLAUDE.md |

## Falta depois
LOD com transição suave (dither), detalhe de longe, veículos mais bonitos e leves, e o resto das Pendências do CLAUDE.md.

## 2026-11-06, depois do Interior

- `0317b8c6`+: aba Interior provada por clique e foto (`torre695-andar10`). O clique ia para o lugar errado nas capturas porque três ferramentas liam o cursor do sistema: `ToolController.pointer` guarda a posição do evento.
- Lotes contra as ruas: `sim/lots/lot_land` com Clipper2 (inteiros em mm, mesma resposta em toda máquina): `onLand`, `cutLotsByRoads` no passo de desfazer da edição (`EditHistory::end`), `lotSnapper`, frente pela rua, prévia e cor na camada de zonas.
- O perfil do PGO não era dependência dos objetos: um perfil novo deixava as bibliotecas no antigo e o ThinLTO recusava. Agora todo objeto depende de `native/pgo/citycore.profdata`.
- Salto de cor entre níveis de prédio: o médio e o distante desenhavam um "cômodo médio" fixo claro. `building_room_tone.gdshaderinc` dá o cômodo pelo ângulo e o que pende em cada janela; todos os níveis leem. A troca de geometria perto-médio dissolve desde `building_level.gdshaderinc` (ver abaixo).
- Veículos: porta da guia e vidro abaixado no vértice, sem draw a mais; aro, poço e disco de freio na roda perto.
- Vias: Personalizar… e Meus modelos (`road_spec`, save v7).
- Bomba: `mounds` e `rubble.gdshader`: a quadra derrubada era grama com lajes soltas e agora é entulho. A altura (7% do prédio, até 4,5 m) e a cor são escolha por foto, sem fonte. Um monte cobre parte da cratera, e as lajes soltas ficam quase todas enterradas nele.
- Conferido só por foto, sem par com o 4.0: rio, ônibus, porta da guia, monte. Os coeficientes de absorção da água são escolha por olho.
- Não auditados nesta rodada: pedestres (expressões, pele, objetos na mão), pistola, paisagismo e vegetação, mobília, histereses, previews além de lotes, F7, texturas, luz, sombras, polígonos dos modelos. A grade no chão e o piscar da via nova (`setGrid`, `flashGrid`, `flashRoads`) só foram lidos no 4.0: faltam.
- Passagem perto-médio: gêmeos com `discard` só nos sub-chunks da faixa (`fade_levels`), o médio inteiro atrás, 0,6 m ao longo do raio. `camera_pan`: GPU perto 9,2 -> 9,5 ms, os demais iguais; estática +10 MiB (três variantes de shader, acima da regra de "memória nunca acima"). Parado, a faixa mostra o pontilhado do dither. Médio-distante (1100 m) e baias-retângulos (500 m) seguem secos.
- Aletas: o arco nas torres era lóbulo lateral do filtro de caixa de um pixel; filtro de dois pixels.
- Via nova acende (`road_flash.gd`); com a camada Grade a via encaixa em células de 10 m. Regra de cada via no Inspecionar (`approach_panel.gd`).
- Veículo queimado: o corpo do modelo amassado (`agent_wreck.cpp`). Quem perdeu um membro pinga (`DRIP`), sem foto que o mostre: o tiro de captura não arrancou membro; quem só leva tiro leve não pinga, como no 4.0.
- `quick.sh`: ler a linha `FAILED`; a última linha diz "photos: golden written" mesmo com teste falho.
- Histerese: veículos e pedestres seguram o nível 10% além da borda; alcances de mobília, árvores e cobertura seguram 3% pelas margens do Godot. Prédios perto-médio ficam sem margem: a dissolução depende da distância exata.
- Sombras por camada (`camera_pan --casters-cycle`, sem vsync): sem sombra 5,2 ms contra 8,4 ms com; com só uma camada lançando, o custo fica igual ao de todas. Os 8 milhões de triângulos de sombra não são o gargalo: o custo está na amostragem da sombra na luz, não na geometria. Reduzir polígono não reduz GPU aqui. Carro distante passou a ser simplificado inteiro, sem ganho medido de GPU (9,1 ms).
- F7: abas Consumidores e Inspetor (`lab_views.gd`). Faltam, do laboratório do 4.0: gravador, testes, perfil de CPU, passes da GPU, edição ao vivo no Inspetor.
- Sombra, sem vsync. Em `perto`: 5,2 ms sem sombra, 8,65 ms com sombra ligada e nenhum lançador, 8,4 ms com todos: custo fixo de ~3,4 ms (limpar o atlas e amostrar, não separados) e geometria perto de zero. Em `parada`: 8,9-9,2 ms sem lançador, 11,1 ms com todos: a geometria soma ~2,2 ms, ~1,2 ms do nível médio (3,9 M triângulos, 726 draws), 0,5 da vegetação, 0,2 das massas. Trocar o filtro para Soft Low tira ~1,4 ms em `parada`, com perda de qualidade não avaliada.
- Tentado e desfeito: deixar a massa distante lançar a sombra do nível médio (GPU 11,1 -> 10,0 ms). As paredes e caixas de telhado ficaram pretas: a massa não coincide com as paredes, nem recuada 0,5 m. Fica para um lançador próprio, a parede lisa do nível médio sem as baias.
- `capture --only=nada` fotografa o cruzamento mais movimentado (`busy_crossing`): mudou o tráfego, muda o lugar. Foto de antes e depois só vale pelo `look`, com câmera fixa. `--bless` regrava hash e fotos sem olhar: usar só com a mudança entendida, e rodar sem ele depois. Conferido em 2026-11-06: prédios 454, 695, 1004, 600 e 800 da semente 7 iguais aos do início da sessão.
- Troncos com normal redonda (mesmos triângulos), lentes e cabeças com 12x6 em vez de 7x4.
- Sombra do nível médio: `cast_hull` (as próprias paredes, soldadas e simplificadas a 0,15 m, material de profundidade compartilhado). `parada` sem vsync: GPU 11,1 -> 10,1 ms, sombra 8,1 M -> 5,7 M triângulos, 783 -> 590 draws; `perto` 8,9 -> 8,2 ms. Foto de antes e depois igual a olho, na mesma câmera.
- `building_tool_test` tinha erro de script desde o caso `_inside`: sem HUD a barra do andar nunca entra na árvore. Eu tinha dito que passava olhando só `FALHAS: 0`; quem acusa é o `quick.sh`, na linha FAILED.
- Fotos congeladas não eram reproduzíveis: variavam até 1,25% entre rodadas, e o `--bless` de todo commit escondia isso. Duas causas: o ruído do dither usava TIME, que para em valor diferente a cada rodada (agora `dither_clock`), e a fase do jitter temporal dependia de quantos quadros os jobs levaram (o `look --frozen` agora fotografa em quadro múltiplo de 16). Três rodadas seguidas sem `--bless`: 4 de 4 iguais. De agora em diante `--bless` só com mudança visual entendida.
- Pessoas paradas (2026-11-06): a escolha é da simulação (`ped_acts.cpp`, `PedAct` no registro), a regra é a `standingKey` do 4.0. Os clipes `talk`, `listen`, `look`, `phone`, `headphones`, `bag` no 4.0 são adiados (`Deferred`): a primeira cozida veio igual ao `idle` byte a byte, porque `baked.clips` dá o substituto; a certa chama `job.make()`. Conferido o quadril de cada clipe em cada corpo: todos em pé. `people.bin` v5: paleta já em ponto fixo (o boot não converte mais), segundos por sexo, palma por corpo. A paleta passou de ±2 para ±3: os clipes novos chegam a 2,52 e seriam cortados.
- A mão: `P · inverso(inverses[k])` bate com o `hands` do 4.0 nos números. Mas o comentário do `heldProps.ts` (+X ao longo dos dedos) não vale para o osso destes corpos: em repouso a rotação dele é a identidade e os dedos vão a -Y. O cozimento calcula a palma pelos dedos, e aí os modelos do 4.0 entram como estão.
- Carros invisíveis com o motorista sentado no ar: as vagas contam `row_id` de 0, como a frota, e os dois disputavam a mesma chave no `RowKeeper`: o carro andando perdia a linha enquanto a vaga de mesmo número estava na vista. O comentário dizia "past the fleet's", e não era. Achei numa foto de pessoas: repetia igual, sumia a partir de uns 10 m daquela câmera, e o log mostrou um `draw_of` de outro modelo. Duas teses minhas caíram antes: clipe sentado (o quadril negou) e nível médio (`cabin_close_m` é 60). Chave agora diz de quem é o id; `agent_cost.rows_lost` e o e2e cobram.
- Correção: "fotos congeladas bit a bit" não vale sempre. Com `people_test` e `e2e` rodando ao lado, `00-cidade` variou 0,11% entre rodadas; o mundo é sem tráfego, os pontos ficam nas quinas dos prédios. O `look --frozen` agora espera 96 quadros depois do último desenho (`FORGET_FRAMES`) e a variação caiu para 0,04%, abaixo do limite de 0,1%. A causa do resto não foi achada; as outras três fotos repetem bit a bit.
- Luz de pele e cabelo: `light()` em `people.gdshader` com os termos do `personMaterial.ts` (pele com wrap de 0,4 e o vermelho que espalha, cabelo por dois lóbulos ao longo dos fios, reflexo do céu a um quarto). `LIGHT_COLOR` já traz o π, como a cor do three, então as contas entram iguais. Foto de antes e depois de perto: cabelo com brilho nos fios, rosto sem o chapado; sem par com o 4.0 na mesma pessoa.
