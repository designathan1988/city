# vegetacao-terreno

## Objetivo e itens
- Deixar o branch `vegetation-terrain` pronto para a coordenadora integrar: sem commit pendente, master integrado, B2 zerado, fase 5 feita. Nenhum dos 14 itens é desta área.

## Responsável e arquivos
- Executora desta rodada: sessão e74983d5. Worktree `wt-vegetacao-terreno`, branch `vegetation-terrain`.
- `render/trees`, `sim/terrain`, `terrain.gdshader`, `water.gdshader`, trechos de terreno e água de `city_world.gd`; compartilhados `materials.json`, `CLAUDE.md`, schemas; sem dono `palette.json`, shaders `vegetation*`, `terrain_layers`, `lattice_noise`.

## Causas comprovadas
- Blocos na água em Forward+: `noise2d.gdshaderinc` usa `fract(sin(...))`; na NVIDIA/Vulkan o valor difere dos dois lados da borda da célula (a soma `i + vec2(1,0)` é reassociada e o seno amplifica). Prova: sonda só de ruído, degraus em Forward+ e contínuo em Compatibility (`vegetation-terrain/n_forward_plus_near.png`, `n_gl_compatibility_near.png`). Com hash inteiro, os dois iguais (`w2_*_perto.png`).
- `grade_at` (tree_place.cpp) devolvia a média dos dois eixos: metade do declive real ao longo de um eixo. Os limites de declive não valiam o que o `_doc` dizia.
- `ripple_strength` 0,317 fora calculada com a razão de camadas 3,1 e pesos 0,62/0,48, e o shader final usa razão 4: valor sem relação com o shader.
- B4 de `streets.json`: `areas.json` dá o arquivo inteiro às vias, e o bloco `trees` é da vegetação. O script olha o caminho, então qualquer diferença em `trees` dá B4.
- Valores vindos do código do 4.0: `notas.md` da sessão 6f9a4a09 (scratchpad) anotou o código dele, e `make_trees.py` escreveu `trees` depois disso. Vários valores eram iguais às notas; ver Decisões.

## Fontes
- Hasselmann et al. 1973, Dtsch. Hydrogr. Z. Suppl. A8 n. 12: f_p = 3,5 (g/U)(gX/U²)^-0,33 (de memória, sem acesso ao texto nesta rodada).
- Cox & Munk 1954, J. Opt. Soc. Am. 44(11): 0,003 + 5,12e-3 U (de memória).
- Phillips 1958, J. Fluid Mech. 4(4): faixa f^-5 (de memória).
- Godot `scene_forward_lights_inc.glsl`, F0(): `0.16 * specular * specular`, lida na tag 4.5-stable (https://raw.githubusercontent.com/godotengine/godot/4.5-stable/servers/rendering/renderer_rd/shaders/scene_forward_lights_inc.glsl); falta conferir na 4.7.
- Resolução SMA 64/2009 art. 2 XI, XII e XIV, texto lido: https://www.infraestruturameioambiente.sp.gov.br/institutodebotanica/wp-content/uploads/sites/235/2014/02/resolução_64.pdf
- Carvalho, Espécies Arbóreas Brasileiras vol. 2, Embrapa, jerivá, texto lido: https://www.alice.cnptia.embrapa.br/alice/bitstream/doc/1140835/1/Especies-Arboreas-Brasileiras-vol-2-Jeriva.pdf
- Classes de relevo Embrapa, lidas em trabalho que as cita: https://www.alice.cnptia.embrapa.br/alice/bitstream/doc/339373/1/Aplicacao-do-MDT-na-classificacao-de-relevo-2007.pdf ; a Súmula de 1979 não foi aberta (https://edepot.wur.nl/480004).
- Lei 12.651/2012 art. 4 I a e V (de memória; o `_doc` anterior já citava).
- Ribeiro & Walter 1998 (cerradão 50-90 %, 8-15 m): confirmado só por trabalhos que o citam.
- Francou & Manté 1990, doi:10.1002/ppp.3430010107: valor 33-35 graus só pelo resumo, via sessão anterior; texto inteiro fora do ar (maintenance.ird.fr).
- Kreczmer, Dąbski & Zambrowska 2026, doi:10.2478/mgrsd-2025-0032: 28,0-33,4 graus, pelo resumo.
- Rost, OpenGL Shading Language 3ª ed. 12.1: confirmado o conceito (equador recebe metade), a fórmula não foi lida.
- Cook 1986, ACM TOG 5(1): de memória.
- Golus 2021 (Medium, 403 ao abrir): 0,25 confirmado por blog que o cita (https://lisyarus.github.io/blog/posts/exploring-ways-to-mipmap-alpha-tested-textures.html). Fonte secundária: pendente.
- 10-25 folhas por copa do jerivá: fonte secundária (https://www.botanybrisbane.com/plants/arecaceae/syagrus/syagrus-romanzoffiana/). Pendente.

## Decisões
- `lattice_noise.gdshaderinc`: ruído por hash inteiro sai de `terrain_layers` e serve terreno e água, um dono só. `noise2d` fica com a via, sem mexer (arquivo sem dono, efeito em outra área).
- Água: camadas iguais em inclinação (Phillips) no lugar de 0,62/0,48; retirados 0,0031, 0,0011, 0,4 e 1,6 (ver `fetch_m` e Nyquist no shader).
- Trocados por serem iguais às notas do código do 4.0: FACE 0,42/0,45 para 0,5/0,5; NOISE_FREQ 1,1 para 1; jitter das faces 0,15 para 0; shade_bottom 0,62 para 0,43; tint_vary 0,10 para 0,15; jitter da grade 0,9 para 1,0.
- Mantidos por fonte independente com o mesmo valor: max_grade 0,75 (relevo escarpado), cover_per_mip 0,25 (Golus).
- Iguais às notas e ainda sem fonte própria, escritos como escolha de modelagem ou com critério de malha: fronds rise 0,12-0,22, fronds count 10 (dentro da faixa), segments 6 (critério de flecha), rings 5, roughness 0,85, limbs count 3 da folhosa.
- `bands.cover` passa a `crown_cover`: colisão de nome com `COVER_H` (SEGUNDO_DONO).
- B4: não movi `trees` para arquivo próprio. Isso exigiria apagar o bloco de `streets.json` e mudar `data_tables.gd`, ambos das vias: dois B4 em vez de um.

## Commits
- ea80140 água; bdd8787 merge do master; 49c9bbc dados de terreno e materiais; 7f0670b árvores; 564123f pendências no CLAUDE.md; dc8d08f merge do master 95b5aad.

## Validações
- Ponta dc8d08f, contendo o master 95b5aad, RTX 3060, máquina carregada. Gates OK; DLLs debug e release saída 0, -j 4.
- Testes: 8 linhas ok, nenhuma FALHOU (`run_tests.sh`, com light_color e building_light nos dois renderizadores).
- Captura mapa 0: 44 fotos, 0 SCRIPT ERROR. Soak 5 min: trips_arrived 914, trips_lost 0, lost_stuck 0, red_entries 0, people_in_furniture 0.
- Memória soak fim: estática 269,5 MiB, vídeo 542,8, buffers 322,6. Só contagem.
- Sonda da água nos dois renderizadores (`w2_*_perto.png`): sem erro de shader, sem blocos. O mapa 0 não tem água; a sonda não foi comitada.
- Fotos: 12_parque, 11_relevo e 04_rua_baixa comparadas com a ponta anterior (depois6 da sessão 6f9a4a09); mediana das copas em 12_parque (92,111,70) contra (91,111,69). A comparação cobre vegetação; 7 fotos e folhas de contato vistas, as demais não foram abertas uma a uma.
- A/B shade_bottom 0,62 contra 0,43 em 05_transito: coração visível nos dois (`scratchpad/ab/copa.png`).

## Pendências e bloqueios
- B4 de `streets.json`: fora de `trees` o arquivo é igual ao master (conferido por script). Zerar pede decisão da coordenadora: `streets.json` em compartilhados no `areas.json`, ou `--aceitar`. Causa: o script olha o arquivo inteiro.
- Constantes sem valor publicado nem medido (escritas assim no `_doc`): quase todo `atlas`, proporções das espécies, gains e tamanhos da colocação. Parte delas pode ter vindo do 4.0: `notas.md` foi escrito antes de `make_trees.py`. Plano: medir nas fotos ou trocar, chave a chave.
- Ainda iguais às notas do código do 4.0, sem fonte própria: fronds rise 0,12-0,22, rings 5, roughness 0,85, limbs 3. Fontes secundárias: fronds count (10-25), cover_per_mip (Golus inacessível).
- Citações de memória ou por resumo: ver Fontes. Conferir com o texto antes de dar por primária.
- `sim/terrain`: `terrain_gen.cpp`, `terrain_gen.h`, `terrain_brush.cpp` e `heightfield.h` ainda se dizem porte do 4.0 (`auditar_origem.py`: 5 pontos). Vem de antes deste branch e é trabalho novo.
- Coração da copa visível de perto (05_transito). Existe com os dois valores de shade_bottom. Causa não investigada a fundo: cartões esparsos sobre corpo escuro.
- `noise2d.gdshaderinc`: mesmo hash com degrau afeta a via em Forward+; arquivo sem dono, comentário ainda cita a água.
- Cores da água são cor de tela usada como albedo; calibrar pede mapa com água, e só o mapa 0 é permitido.
- Com a máquina quieta: `camera_pan` no mapa 0 contra o master (o tom e a altura do mato mudaram contagem de instâncias só em parte).
- Fotos de referência em `records/vegetation-terrain/` eram de scratchpad temporário; a coordenadora decide se ficam ali.

## Próxima ação
- Coordenadora: decidir o B4 e as constantes iguais às notas antes do merge.
