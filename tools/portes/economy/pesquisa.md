# Pesquisa do porte economy

## Preço do pavimento

Fonte: DNIT, Custos Médios Gerenciais, Anexo IX, base julho de 2016, elaborado em 17/11/2016.
https://www.gov.br/dnit/pt-br/assuntos/planejamento-e-pesquisa/custos-e-pagamentos/custos-e-pagamentos-dnit/custo-medio-gerencial/CUSTOMDIOGERENCIALJULHO2016.pdf

Linha "Implantação/Pavimentação (P. Simples), faixa 3,6 m e acostamento 2,5 m": R$ 2.376.001,73 a R$ 4.106.513,03 por km, média R$ 3.241.000,00.

Conta: 2 × 3,6 + 2 × 2,5 = 12,2 m de plataforma. Média 3.241.000 / 1000 / 12,2 = R$ 266 por m²; faixa de R$ 195 a R$ 337.

O preço do 4.0, R$ 150 por m², fica abaixo dessa faixa. O estilo `roadcraft` mantém 150 como escolha de modelagem; o estilo `brasil` usa 266. O valor é de 2016 e inclui terraplenagem, que no jogo seria outro item.

## O que não achei

- **Multiplicadores por estrutura** (aterro 1,4, trincheira 1,8, ponte 2,5, túnel 6). A busca não trouxe razão publicada. O relatório WSDOT de 2004 (https://www.vtpi.org/WSDOT_HighwayCosts_2004.pdf) só diz que estruturas pesam no custo e que a FHWA não tinha referência atual por faixa-milha. O mesmo Anexo IX do DNIT tem a tabela de obras de arte especiais, mas a extração do PDF saiu sem os valores e diz que a metodologia está em revisão. Ficam como escolha de modelagem.
- **Canteiro e calçada** por m²: sem fonte. Escolha de modelagem.
- **Saldo inicial e parte devolvida:** são regra de jogo, sem fonte física. Escolha de modelagem.
- **`fill_from_m`, `on_piers_from_m`:** escolha de modelagem.

## Godot

Rótulo na ponta da via: `Camera3D.unproject_position` dá o ponto da tela. O projeto já usa isso em `tests/visual/ui_driver.gd`. Sem nada novo a pesquisar para a economia.
