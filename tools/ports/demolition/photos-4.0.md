# Fotos do 4.0 analisadas (demolition) — o alvo visual

Fonte: `ports/demolition/reference/*.png`, tiradas do 4.0 pelo `node scripts/port.ts reference demolition evidence/look-demolition.json` (cidade pequena, GPU RTX 3060). Abertas e vistas uma a uma.

| Foto | O que mostra | O que o jogo precisa igualar |
|---|---|---|
| 06 antes (prédios) | centro adensado, HUD no topo (relógio, R$, veículos/pessoas, velocidade), dock embaixo | estado antes do golpe |
| 14 bomba força 5 em torre, t0 | bola de fogo pequena (núcleo branco-amarelo, borda laranja) com fumaça cinza-clara sobre a fachada; anel pálido de onda no chão em volta; painel Demolir/Pistola/Bomba e texto de ajuda; "Força − 5 +" no canto | bola de fogo + fumaça de tamanho proporcional; anel de onda; painel e stepper (Porte 4) |
| 16 t1 | a bola virou nuvem branco-acinzentada com labaredas, fachada ainda sem buraco | fumaça sobe e engorda; chama some em ~1 s |
| 20 t8 / 30 (quatro golpes) | fumaça sumiu; a torre segue de pé (força 5 não derruba um arranha-céu); mais "pessoas" no contador | golpe pequeno não derruba prédio grande; dano se soma (dica "continue batendo para derrubar") |
| 36 força 45 em torre, t0 | colapso: dezenas de cacos de fachada (azuis, brancos, cinza, com janelas) voando, nuvens densas de poeira branca espalhadas na horizontal, chamas pequenas, pontos vermelhos, aviso "Atingido: continue batendo para derrubar" | fratura em células com casca e janelas, poeira branca grossa, entulho |
| 40 t7 | o prédio atingido ficou com a fachada furada (janelas vazadas) e uma chama; poeira sumiu, destroços pequenos nas ruas | dano parcial persistente na fachada; destroços ficam |
| 51 rua t0 (força 5, casa ao lado de via) | bola de fogo grande sobre a casa, tábuas marrons voando, fumaça, anel de onda cobrindo a pista; aviso "Atingido: houve vítimas" | estilhaços do material do prédio, anel na pista, aviso de vítimas |
| 55 rua t4 | casa de pé pegando fogo (chamas no telhado, fumaça escura), destroços cinza na rua | fogo no prédio atingido (até 3), fumaça preta, destroços |
| 66 terra t7 | cratera marrom pequena com bordas levantadas e três manchas pretas de fuligem, poeira cinza rala, pedrinhas; sem prédios | cratera + fuligem no chão |
| 78 / 88 pistola (pedestre) | painel "Pistola: Clique onde quer atirar."; nos dois quadros não se vê pedestre baleado (a câmera ficou fora do alvo) | ver lacuna abaixo |

Lacuna: as fotos 78 a 88 não mostram o pedestre baleado nem o ragdoll (a câmera do plano ficou em cima de uma casa). A rodada de fotos do 4.0 para pistola/ragdoll será refeita com câmera sobre pedestres antes da etapa de teste.

Consequências para o jogo (já no plano): (1) dano acumulado por prédio, com colapso só quando soma ≥ 1; (2) a bomba pequena queima e racha, a grande derruba; (3) anel de onda, bola de fogo, fumaça cinza/preta, poeira branca, cacos com janelas; (4) cratera pequena com borda e fuligem em terra.
