# Calçada: o que o 4.0 desenha e o que falta no jogo (regras-sessoes.md, seção 11)

**Fotos.** 4.0: `reference/sol/` (cidade pequena, semente 7, relógio 12:02 a 12:05), em trecho de sol, longe do centro. Jogo: `evidence/sol-antes/` (código `46a7851` mais a sonda, 12:00, mesmas poses em relação ao mesmo tipo de peça, cidade e rua diferentes). Recortes 3x em `evidence/recortes/`.
`sol-rua` do jogo não vale: a câmera caiu dentro de uma copa. Esse erro é em si um resultado: o 4.0 não tem copa ali.

## Inventário, por km de via (4.0: 28,1 km; jogo: 60,6 km)

Contado no documento do 4.0 (`doc.landscape`) e, no jogo, na sonda nova `furniture_census`.

| Peça | 4.0 | Jogo antes | Regra do 4.0 (`furnitureCatalog.ts`) |
|---|---|---|---|
| Árvore de calçada | **0** | **122,6** | só em via desenhada; `cityGenerator` passa `trees = false` |
| Hidrante | 10,5 | 1,1 | a cada 100 m, um lado |
| Banco | 31,2 | 20,0 | a cada 60 m, nos dois lados |
| Lixeira | 29,9 | 23,8 | a cada 60 m, nos dois lados |
| Lâmpada acesa | 58 | 62,4 | coluna a cada 30 m e postes |
| Poste de rede | 28,3 (todos acesos) | sem contagem igual | |
| Nome de rua | 0,9 itens, placas nas duas pontas de cada trecho | **0** | toda rua nomeada (`nameFor`) |
| Caixa de correio, ralo, arbusto | 0 | 0 | conjunto vazio na cidade gerada |

## Causas e correções

| # | O que falta ou difere | Causa medida | Correção | Dono |
|---|---|---|---|---|
| 1 | Calçada sem sombra de poste e placa; tronco em toda parte | 122,6 árvores por km: a copa contínua sombreia a calçada, e sombra de poste em cima de sombra não aparece. O 4.0 não tem árvore ali | `trees.in_generated_city` por estilo: 0 no `roadcraft`, 1 no `brasil`; via desenhada mantém | meu |
| 2 | Poste fino sem sombra | Primeira cascata cortada a 120-160 m: texel de 7,5 a 16 cm contra poste de 8 cm. O 4.0 corta logo depois do ponto olhado (`sunCascades.ts`, dTarget x 1,5) | `SunReach`: `shadow_split_1` por quadro, pela distância olhada | meu (sombra de calçada); Porte 3 avisada |
| 3 | Hidrantes 10 vezes menos | Cobertura de 300 m (`hydrants.reach_m`). A 100 m a conta dá 9,9 por km | `roadcraft` 100; `brasil` segue 300 | meu |
| 4 | Bancos e lixeiras menos e só em certas zonas | O jogo põe na esquina e no meio da quadra residencial; o 4.0 põe em fila | `furniture.*_every_m`, `*_first_m`: fila no `roadcraft`, regra atual no `brasil` | meu |
| 5 | Poste de placa liso e claro | Cor certa (`#8e979b`), mas rugosidade 0,85 sem metal; no 4.0, 0,45 e 0,6 | tag própria com metal e rugosidade | meu |
| 6 | Sem placa de nome | A cidade gerada não nomeia ruas | **Não feito nesta rodada**: precisa de lista de nomes por estilo e de folha maior | meu |
| 7 | Ladrilho quase branco, junta apagada, meio-fio liso | Albedo e textura da via | Não é meu: a Porte 1 já levou `paving_colors[0]` a `#7e7e7b` no ramo dela | Porte 1 |
| 8 | Sombra fraca | Ambiente chapado e sem oclusão (seção 11, item 2) | Não é meu | Porte 3 |
| 9 | Tronco de caixa | Corpo da árvore (`tree_kit.cpp`) | Combinado com a Porte 3, sem resposta ainda. Na cidade gerada do `roadcraft` a árvore de calçada sai | Porte 3 |
| 10 | Trama fina dentro da sombra | Hipótese: `soft_shadow_filter_quality` padrão, 2. Testar por variante depois das outras | a medir | a definir |
| 11 | Piso rugoso junto à faixa (`sol-pole`, em x 970, y 585) | Parece piso tátil do rebaixo; não confirmei | a conferir | Porte 1, provável |

Correção de um erro meu: o `_doc` de `lamps.pole_lit_every` dizia que o 4.0 acende um poste sim, um não. O inventário mostra 796 de 796 acesos. O valor 2 fica porque dá 62 acesas por km contra 58, mas por contagem, não por comportamento do 4.0.

## Depois (código `304e7f9`)

Contagem por km, 60,6 km: árvore 0 (4.0: 0), hidrante 9,2 (10,5), lixeira 29,5 (29,9), banco 29,7 (31,2), acesas 62,4 (58).

| # | Situação | Prova |
|---|---|---|
| 1 | Feito no `roadcraft`: a rua abriu e a calçada voltou a pegar sol. Censo com `doc.trees` e par a 3x no `relatorio.md`, seção Censo | `fios-final/par_sol-rua`, `recortes/arvore_sol_rua_4.0_jogo.png` |
| 2 | **Provado e entregue à Porte 3**, que é dona da sombra do sol: não está neste ramo | `recortes/sombra_placa_antes_depois.png`, `para-porte-3/` |
| 3, 4 | Feitos | contagem acima |
| 5 | Feito: metal 0,6, rugosidade 0,45 | mesmo recorte, poste à direita |
| 6 | **Não feito** | |
| 7, 8, 9, 11 | Das Portes 1 e 3; a divisão da árvore foi aceita pela Porte 3, e o tronco muda depois da economia dela | |
| 10 | Causa achada: a trama vinha da cascata grosseira, sumiu no mesmo teste do item 2 | mesmo recorte |

**Limites desta prova.** A calçada da rua da captura fica ao sul e, às 12:00, à sombra dos prédios, então banco, lixeira e hidrante não foram vistos ao sol. A sombra de poste foi provada só na pose da placa, na calçada do outro lado. As fotos do 4.0 e do jogo são de ruas diferentes. `sol-depois` foi tirada com a cascata ligada, que saiu do ramo depois; `calcada-final` também. O ladrilho do jogo segue quase branco nessas fotos, porque a cor é da Porte 1.
