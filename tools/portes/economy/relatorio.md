# Relatório do porte economy

Clone: `ports/economy/workspace`, branch `convert/economy`, commits b896b9f e 9b1afb9. Nada integrado, sem push.

## Situação

A economia do 4.0 está portada na arquitetura atual: cobrança, devolução, recusa, rótulo na ponta da via e dinheiro pelo idioma. Uma parte não aparece em jogo ainda: os modos de construir têm regra e teste, mas toda via vale como ao nível do chão até o greide existir.

## O que foi lido

- `converter/out/features/economy/` inteiro. O rastreamento original estava incompleto; refiz com `src/ui/roads/money.ts` e `src/world/roads/buildMode.ts`: 34 declarações, 32 fatias, 14 arquivos.
- Do 4.0, fora do rastreamento: `main.ts` (rótulo), `roadTool.ts`, `commit.ts`, `structures.ts`, `tuning.ts`, `i18n`.
- `regras-sessoes.md` seção 10 e os ramos `convert/roads` e `convert/road-elevation`, para não duplicar.

## Requisitos e o que foi feito

| Requisito | Estado |
|---|---|
| Preço da prévia é o preço pago | portado, medido |
| Demolir devolve 25 %, via gerada inclusive | portado; saiu `SEG_FREE` |
| Melhorar e rotatória cobram a diferença de valor | portado |
| Recusa sem dinheiro, sem mudar nada | portado, `hint.rule.funds` |
| Rótulo na ponta da via | portado, `HudTag` |
| Modos de construir e multiplicadores | regra portada; sem efeito em jogo até haver greide |
| Saldo inválido recusado ao abrir | portado |
| Dinheiro pelo idioma | portado; antes "R$" e o ponto eram fixos |
| Estilos | `styles.roadcraft` e `styles.brasil` em `road_profiles.json` |
| Receitas | não há no 4.0 além da devolução |

## Medido: 4.0 contra jogo

| Passo | 4.0 | Jogo depois |
|---|---|---|
| Saldo inicial | 20.000.000 | 20.000.000 |
| Via de 500 m, prévia | 760.960 | 711.000 |
| Depois de construir | 19.239.040 (cobrou a prévia) | 19.289.000 (cobrou a prévia) |
| Demolir | +190.240 (via inteira, 25 %) | +35.550 (um pedaço de 100 m, 25 %) |
| Desfazer | volta | volta |
| Demolir via gerada | +53.280 | +47.952 (antes: 0) |

A diferença de preço por metro (R$ 1.422 contra R$ 1.520) vem da tabela de perfis, não da regra. `convert/roads` já a refez: recalculei os 12 cartões de "Ruas" com as tabelas daquele ramo e todos batem com o 4.0, em largura e preço. Isso foi conta, não foto.

No jogo a via é feita de pedaços de até 100 m e a demolição tira um pedaço; no 4.0 tira a via inteira. Demolidos os 5 pedaços, voltam 172.250 de 689.000, os mesmos 25 %. A granularidade é assunto de `demolition`.

## Pesquisa e constantes

Em `pesquisa.md`. Só o preço da pista tem fonte primária: DNIT, Custos Médios Gerenciais, base julho de 2016, R$ 3.241.000 por km em 12,2 m de plataforma, R$ 266 por m², usado no estilo `brasil`. O valor é de 2016 e inclui terraplenagem. O estilo `roadcraft` fica com 150 e fica abaixo da faixa do DNIT.

Sem fonte, ditos como escolha de modelagem no `_doc`: multiplicadores por estrutura, canteiro, calçada, saldo inicial, parte devolvida, `fill_from_m`, `on_piers_from_m`.

## Plano

`plano.md`. Dono único em `native/src/sim/economy/road_economy.*`, camada já prevista em `rules.json`, com `charge_for` no `owners.json`. O valor da edição vem do diário: nada guarda a soma do mapa, então desfazer não tem o que corrigir. Fica de fora o `Ledger` do 4.0.

## Testes e builds

Gates OK, inclusive no estado exato do primeiro commit. DLLs de release e debug compilam. Oito testes passam, em `evidence/testes/`. `road_commands_test` ganhou 24 verificações, com a tabela de `build_lengths`; `save_undo_test` recusa saldo infinito.

## Fotos

`capture_money` repete passos e câmera do 4.0. Tamanho conferido por script: as 11 fotos do 4.0 e as de `evidence/depois` estão em 1920x1080. Pares em `evidence/pares/*__1920x1080.jpg`, 4.0 à esquerda. Li em resolução cheia o que a economia desenha, em recortes.

Mapas diferentes (semente 7 contra mapa 0), e este clone não tem a luz de paisagem: nenhum par vale para cor ou tom. A hora do céu é 12h, mas o relógio da barra fica em 06:30 contra 12:01, porque aqui `set_hour` não move o relógio.

Defeitos achados e corrigidos antes do fim: janela com 1061 linhas, cartão errado, rótulo e saldo sem negrito.

Diferenças que ficam, por foto:

- `via_na_mao`, `via_sem_dinheiro`: texto, posição e cor do rótulo iguais em forma. A fita do jogo é chapada; a do 4.0 mostra as faixas e argolas nas pontas. É de `roads`.
- `recusa_clique`: texto idêntico. O jogo mostra em pílula vermelha e o 4.0 em caixa escura: é a dica da interface.
- `catalogo`: largura com decimal e tabela velha; `convert/roads` e Porte 4.
- `dinheiro_barra`, `via_paga`, `via_demolida`, `gerada_demolida`: valores conferem. O toast "devolvido R$ 47.952" não existe no 4.0; já era do jogo e mantive.

`evidence/antes` está em 1920x1061 e com o cartão errado: registra o comportamento antigo, não é par.

## Cobertura

`cobertura.md`, uma linha por arquivo, declaração, fatia e recurso.

## Para a integração

- `convert/roads`: `road_move.cpp` devolve e cobra por conta própria e deve usar `settle_road_edit`. `road_value` saiu de `road_edit.h` para `road_economy.h`. `refusal()` perdeu o segundo argumento. `road_tool.gd` conflita no rótulo. `segment_value` e `SEG_FREE` saíram.
- `road-elevation`: ligar `built` em `road_economy.cpp`; decidir se `on_piers_from_m` fica ou vira dele.
- `pier_from_m` colidia com a constante `PIER_FROM` das fachadas, daí o nome.

## Não verificado

- Tempos de quadro, GPU, CPU e memória: `tests/perf/camera_pan.tscn`, com a máquina quieta. O rótulo recalcula a posição a cada quadro enquanto há via na mão.
- `format_cost` negativo ("devolve") na tela: a prévia de via nunca fica negativa no jogo, e não forcei uma rotatória de valor negativo.
- Idioma inglês: `economy.group` e "$" não foram vistos em foto.
- Dica do saldo ao passar o mouse: o 4.0 usa `title` do navegador e não saiu na foto; o texto foi conferido com o da fonte.
- O primeiro commit passou nos gates, mas os testes rodaram só no estado final.
- Saldo menor que zero por desfazer depois de gastar: não testei.
