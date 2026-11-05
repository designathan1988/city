# Plano do porte economy

## O que foi provado

Medido no 4.0 (`coverage.md`) e lido no jogo:

1. **Via gerada vale dinheiro.** No 4.0 a demolição devolve 25 % do preço de qualquer via. O jogo tem `SEG_FREE`, que zera a via gerada, sem equivalente em `src/world/economy.ts`.
2. **A regra da cobrança está repetida.** `MIN_CHARGE` em `road_commands.cpp` e `road_tabs.cpp`, a conta em `profile_delta`, em `demolish_road` e em `build_roundabout`.
3. **A rotatória cobra errado.** O jogo soma o anel e as aproximações e não desconta o que removeu. No 4.0 a cobrança é a diferença de valor das vias.
4. **Recusa e rótulo.** O 4.0 mostra `500 m · R$ 760.960` na ponta da via, em vermelho com o motivo curto quando recusa. O jogo mostra só o preço na barra de dicas, e a recusa com outro texto.
5. **Dinheiro preso a uma moeda.** `Formatters` fixa "R$" e o ponto, e ignora a chave `economy.money`, que já tem R$ em português e $ em inglês.
6. **Modos de construir ausentes.** `road_value` só conhece `ground`. Multiplicadores carregados e nunca usados. `fillFrom` e `pierFrom` não existem.
7. **Save aceita saldo inválido.** `readEconomy` recusa; `decode_city` não.

## Solução

**Dono único:** `native/src/sim/economy/road_economy.h/.cpp` (a camada `sim/economy` já estava prevista em `rules.json`), com o gate `owners.json` para `charge_for`.

- `build_lengths(tuning, structure, height_a, height_b, length)`: os cinco modos.
- `road_value`: passa a usar `build_lengths`. A assinatura fica, porque `convert/roads` chama. `segment_value` sai: ninguém mais usa.
- `built(segment)`: único ponto que diz como a via foi construída. Hoje toda via é ao nível do chão: o jogo ainda não tem greide. Quando `road-elevation` entrar, muda só esta função.
- `charge_for(before, after, balance, refund_share)`: a regra.
- `edit_value_change(s)`: soma o que a edição aberta adicionou, removeu e trocou de perfil, lendo o diário. É o papel do `Ledger` do 4.0 sem guardar soma nenhuma: o diário já registra o segmento inteiro, sem custo novo, e o desfazer não precisa de conta.
- `settle_road_edit(s, r)`: cobra ou recusa a edição aberta. Quem recusa devolve `hint.rule.funds` e o diário desfaz.

**Quem usa:** `demolish_road`, `change_profile_at`, `build_roundabout` passam por `settle_road_edit`. `build_road` cobra o preço da prévia por `charge_for`: o preço que o jogador viu é o preço pago, como no 4.0 (medido: 760.960 nos dois), sem o ruído de centavos que dividir uma via cruzada traria.

**`change_profile_at`:** primeiro troca os perfis, depois cobra, depois mexe no chão. Recusado, o desfazer só reverte os segmentos.

**`SEG_FREE`:** sai. Saves antigos ficam com o bit, que ninguém mais lê.

**Dados:**
- `tuning.json`: `economy.fill_from_m` 1 e `economy.on_piers_from_m` 2,8 (`pier_from_m` colide com a constante `PIER_FROM` das fachadas).
- `road_profiles.json`: `styles.roadcraft` e `styles.brasil`, com `prices`, `structure` e `money_key`. `tools/assets/make_road_profiles.py` passa a gerar só a lista de perfis. O carregador lê `roadcraft`.

**Interface:**
- `Formatters.format_money` lê a chave do estilo; agrupador pelo idioma; `format_cost` novo.
- Rótulo na ponta da via: `ToolController.show_tag` guarda o ponto do mundo e o sinal `tag` leva o ponto da tela; `HudTag` só desenha, porque a interface não guarda estado (gate `ESTADO_NA_UI`).
- Textos: `hint.rule.funds`, `rule.short.funds`, `economy.refund`; `ui.insp.noMoney` sai.

**Save:** `decode_city` recusa saldo não finito.

## Testes

- `road_commands_test`: prévia igual à cobrança; demolir devolve 25 % na via paga e na gerada; desfazer repõe; recusa por falta de dinheiro em via, Melhorar e rotatória, sem mudar nada; devolução ao Melhorar para perfil mais barato; `build_lengths` pela tabela do 4.0.
- `save_undo_test`: saldo infinito é recusado ao abrir.
- `capture_money`: os mesmos passos e câmera do 4.0, em par.

## Riscos para a integração

- `convert/roads` mexe em `road_tool.gd`, `road_tabs.cpp`, `road_move.cpp` (devolve e cobra por conta própria) e chama `refusal(r, preço)`, que perde o segundo argumento.
- `road-elevation` deve ligar `built` e pode reivindicar `on_piers_from_m`.
- `STYLE` é constante no carregador, como em `nature_loader.cpp`, até existir o estilo ativo.
