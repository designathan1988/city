# Pedidos da coordenadora ainda abertos (traffic), 2026-11-04

1. Fumaça: o sistema é da Porte 7. Minha parte: regra de emissão por classe (onde sai, regime do motor, cor, quantidade) em `data/render/agents.json`, com fonte de cada número, e o ponto de emissão por classe; chamar a API dela quando avisar o nome. Na cobertura, dono de `exhaust.ts`: "sistema: Porte 7; emissão por classe: Porte 5".
2. `32_carro_manobra`: medir por sonda, no mesmo quadro, a menor distância entre a caixa do carro e a pegada do prédio e do muro do lote, com foto de cima do mesmo instante. Se encosta, é defeito; senão, paralaxe provada.
3. Paleta: constante sem fonte confirmada não fica. Achar cinza 23 e vermelho 4 no relatório da Axalta, ou recalcular as 16 vagas só com as parcelas confirmadas e dizer o que foi feito do resto. Deixar um arquivo de paleta por fonte para comparar, SEM mudar o padrão: a decisão entre o visual do 4.0 e a distribuição real é do usuário.
4. Cobertura: refletir `junctionPanel`, `flowStats`, `controlAdvisor`, e `greenWave`, `rules` agora inteiros, com arquivo e símbolo por declaração (ela roda um script de conferência).
5. `32_carro_manobra`: veículo escuro à direita parece duas caixas azuis com rumos diferentes e a traseira encosta na faixa dupla. Conferir corpo rígido, rumo do vidro e desvio lateral do centro da faixa.
6. `30_vaga_rua`: três caixas brancas em fila junto à faixa de pedestres. Dizer que veículo é cada uma e se há folga.
7. Foto de perto das vagas no jogo ao lado da `09-close-vagas-ciclofaixa-a.png` do 4.0.
8. Seguem: custo de CPU do tráfego (0,45 para 1,2 ms), os três veículos perdidos, paridade por classe, faróis e lanternas à noite.
9. Rodada do 4.0: resta UMA, com lista escrita, `node scripts/port.ts reference ... --timeout 600` em segundo plano, sem `timeout` externo, esperando a vaga global.
10. Avisar: mapa 0 tem 0 docas (0 prédios industriais; zona industrial só perto de via expressa).

## Respostas medidas (sonda `vehicles_near` e `lot_clearance`, mapa 0, mesma cena das fotos)
- Item 5 (32, veículo escuro): é um SUV (type 2, id 200), corpo rígido; desvio lateral 0,0 m e rumo 0,0 rad. Faixa com centro a 1,65 m da faixa dupla e 1,65 m da guia (meia pista de 3,30 m): folga de 0,75 m de cada lado, simétrica. A impressão vem da paralaxe do teto longe do centro da foto. Sem defeito; a correção do ciclista não atinge carros.
- Item 6 (30, três caixas brancas): é UMA picape (type 4, id 556, 5,6 m/s) vista de cima: capô, teto e caçamba separados pelos vidros. Só dois veículos no raio de 30 m; o outro é um sedã na outra mão (id 681, 11,5 m/s).
- Item 2 (folga dos carros de lote): DEFEITO CONFIRMADO. 128 de 144 caminhos invadem algo: 78 em muro (até 0,50 m), 39 em volume de prédio (até 0,95 m), 2 cerca, 2 sebe, 3 arbusto, 2 árvore, 1 grade, 1 pilar; mediana 0,13 m. Causa: a grade considera o carro um disco da largura dele e a curva de Hermite não vê os cantos do comprimento. A corrigir.

11. Regra de fotos: refazer a análise de todas as fotos já dadas como analisadas, com par do 4.0 no mesmo enquadramento (a última rodada do 4.0 deve ser planejada para gerar esses pares: mesma posição, alvo, zoom, hora).

## Doca: ciclo medido e leitura da foto (photos-game/doca-5/36_doca_3.png, mapa 0 com via nova e zona industrial; 74 docas)
Ciclo: chamado no passo 22, encostado no 32, saiu no 48. Sem par do 4.0: nenhum veredito.
Defeitos vistos: (a) HUD 21:39 com luz de dia: a espera avança o relógio, falta acertar a hora antes da foto; (b) linha fina pontilhada no chão, da baliza até a cerca, à esquerda: não identificada, a investigar.

## Fechados nesta rodada
- Item 1 (fumaça): dados por classe em `agents.json` (`exhaust`), aguardando o nome da API da Porte 7. Números só do 4.0, dito no _doc.
- Item 3 (paleta): cinza 23 e vermelho 4 confirmados na nota da Axalta (IPCM). `paints_by_source` com 3 fontes; padrão sem mudança, decisão do usuário. A vaga "demais" da frota não tem cor com fonte.
- Itens 5 e 6: respondidos por sonda.
Abertos: item 2 (corrigir o defeito dos carros de lote), 4, 7, 8, 9, 11; linha pontilhada na doca; embarque; transit.

12. Estilos: feito em agents.json e traffic.json. Falta: placa por estilo (hoje vazia), `eua` e `europa` (região anotada, sem números), semáforo por estilo sem leitor no código, montagem de faixas para mão esquerda. Efeito: a cor padrão dos carros mudou para a do 4.0 (rule 5 da seção 10).
13. Defeito visto em 07_carro: ocupantes do carro laranja aparecem sem teto ou vidro visíveis por trás. Entra na paridade por classe.
