# Rodada de fotos do tema faixas, largura e vagas (depois, só as cenas da ferramenta)

Tirada depois de código completo, gates, build e testes. O "antes" é `evidence/fotos-t5`. Referência: `reference/26-drawer-draw`.

| Foto | O que precisa provar |
|---|---|
| `tool_drawer` | Painel esquerdo com Traçado, Encaixe, Faixas (4), Largura com "Padrão", Vagas (6), com a Via urbana, que é uma classe |
| `tool_chain`, `tool_curve_bend`, `tool_free_stroke`, `tool_free_laid` | Nada mudou na prévia nem no assentamento |

A conta de largura e preço das variantes é provada pelo teste `road_commands_test` (`_variants`), e a ida e volta do save por `save_undo_test`, não por foto.
