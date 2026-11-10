
## Onda 2: base integrada, docas e pares

`convert/transit` é a ponta única: master 689b80d + `convert/traffic` 6713e79 + transporte, mais os commits abaixo. Sem push.

### Merge

Três casos em que os dois lados mudaram o mesmo comportamento. Foram mesclas diretas, sem parar para perguntar, e a coordenadora pode rever:
- Versão do save: os três ramos usavam 2. Agora 3 para os sinais e 4 para o transporte (`SIGNAL_SAVE_VERSION`, `TRANSIT_SAVE_VERSION`, `SAVE_OLDEST` 2).
- Hora: ficou `set_hour` do master; saíram `skip_to` e `set_clock_hour`.
- `new_vehicle` estava definido nos dois ramos: ficou o do tráfego.

### Docas sem caminho: a causa não era o lugar

Eu tinha escrito que a causa provável era o lugar. Estava errado. Oito lugares em volta da cidade, zona industrial pintada em cada um (`evidence/yard-a/run.log`): de 3 a 10% das docas tinham caminho em todos. O ciclo fechava só pelas poucas que passavam, e por isso uma rodada dava certo e outra não.

Duas causas, cada uma medida:

1. **O caminhão não cabia na própria faixa.** O teste de corpo usa um disco em cada quina, que sai 26 cm para fora do flanco. Com a faixa a 1,6 m da guia, a parada antes do portão era recusada. Prova: repetindo o teste fora do jogo nos 40 pátios gravados, a sobra negativa na quina prevê a recusa em 40 de 40, e a maioria falhava por 0,5 cm com o eixo tendo de 13 a 33 cm de folga. A sobra muda com a fase da grade: de novo parecia sorte do lugar. Correção: o disco largo só responde no aberto; se ele toca, a quina é testada em quadradinhos (`way_search.cpp`).
2. **A busca perde caminhos que existem.** O A* híbrido guarda uma pose por célula. Com a grade duas vezes mais fina a entrada aparecia (falhas de 36 para 8), mas a saída não (28 para 51), embora quem entrou possa sair desfazendo o caminho. Correção: achado um sentido, o outro é o mesmo caminho dirigido ao contrário mais o trecho de faixa entre a parada e a volta. Sem nenhum dos dois, uma busca no grão fino, só para aquela doca (`yard.cpp`).

Resultado: docas com caminho de 10% para 99% (136 de 137), chamadas recusadas por falta de caminho de 119 para 0. Rodada final, um pátio por vez: 26 de 26.

O conserto criou um problema, e a foto pegou. Os planos passaram a levar até 9 s, e `JOB_PRIORITY` 100 ficava na frente de todo chunk a mais de 100 m, porque a prioridade dos chunks é a distância em metros. Seis chunks não desenhavam (`FOTO INCOMPLETA`, 3.000 quadros sem andar). Agora o pátio fica de fato atrás de tudo e roda um por vez. Efeito colateral: as docas entram aos poucos, e em `doca-fim` o primeiro caminhão só foi chamado no passo 80.

`record_body` serve o corpo editado da Porte 2 a `yard_input`, `lot_access` e às vagas: conferi por leitura, as docas saem do corpo certo.

**Mudança de comportamento:** no pátio apertado o caminhão agora pode entrar ou sair de ré pela rua, com a rua fechada por `sweep_lanes`. O 4.0 não faz isso; é decisão minha, para dizer sim ou não.

Teste novo: `tests/core/yard_test` pinta zona em duas direções e exige caminho em 90% das docas, caminhão na própria faixa e um ciclo inteiro. Ele falharia com o código antigo pelos números de `yard-a` (3 a 10%), mas não rodei o teste final contra o código antigo.

### Fotos

Pares em `evidence/pares-b/lado` (`<par>-inteiro.png`, 4.0 à esquerda): 16 pares, base integrada, 16:00 e 22:00. Li todos os recortes centrais de 800x600 a 1:1. **Não li as bordas do quadro de 1600x900 nos pares**, e os recortes 3x desta rodada não foram abertos.

Achei a causa dos enquadramentos errados, que valia para todos: `pares-final.json` calculava a altura do olho como `reach x 0,371`, supondo alvo no chão, e ficou cerca de 1 m baixa. Nas duas rodas, a "correção" de 0,75 m aproximava a câmera, quando a câmera gravada já estava certa. Os pares agora saem do que o 4.0 gravou (`evidence/pares-reais.json`). Isso invalida o que escrevi sobre a moto em `ports/traffic/report.md`.

A espera também mentia: depois do salto, `quadros=0` era falso. Com 30 quadros de assentamento apareceram `quadros=12` e `27` de desenho real.

| Par | Vale? | O que vi |
|---|---|---|
| hatch frente | sim | Forma, colunas, teto e frente batem. Capô verde com mancha escura e ruído no reflexo; pessoa de blocos; raios brancos e largos contra a roda do 4.0 |
| hatch trás | sim, ~10% mais perto | Traseira cortada na borda |
| sedã frente | sim | Sem nada na frente, o defeito pedido. Carro preto; outro carro atrás, não na frente |
| sedã trás | sim, ~10% mais perto | Poste à direita, sem tapar o carro. Asfalto bem mais claro que o do 4.0 |
| SUV frente | sim | Sem nada na frente. Vizinhos dos dois lados, atrás |
| SUV trás | sim | Silhueta, vidro e tampa batem. Lanterna creme no jogo, vermelha no 4.0: **a conferir** |
| van frente | sim | Sem poste na frente. Motorista de blocos |
| van trás | sim, ~10% mais perto | Traseira na borda, sem nada na frente |
| moto frente e trás | sim, enquadramento aproximado | Piloto 70 a 90 px mais baixo que no 4.0. Rodas, garfo e tanque certos; piloto de blocos, capacete sem brilho |
| bicicleta frente e trás | sim, enquadramento aproximado | Mesma diferença. Rodas com raios e quadro certos; ciclista de blocos |
| caminhão frente | **não** | A foto do 4.0 é parede e o limite de 2 rodadas acabou. Jogo: muro de lote no canto esquerdo |
| caminhão trás | **não** | Foto do 4.0 tapada. Jogo: copa de árvore na frente; `camera_clear` só vê as árvores da rua, não as de lote e parque |
| sedã à noite frente | sim | Faróis com brilho, placa e forma batem. Asfalto mais ruidoso e mais amarelo |
| sedã à noite trás | só para as luzes | Carro preto: lanternas certas, forma invisível |

Doca (`evidence/doca-fim`, `pendente=0` em todas): caminhão entrando pelo portão (`36_doca_7`) e encostado de ré, com a cabine para fora (`36_doca_10`). Li essas duas; `36_doca_4`, `5`, `6`, `8` e `9` não abri. Sem par do 4.0.

Transporte (`evidence/fotos-base3`, 14 fotos, todas lidas; 40, 45, 46 e 50 li na rodada `fotos-base`, com o mesmo enquadramento). **Quatro não valem nesta base:** `42_terminal` (poste e copa na frente), `43_onibus` (teto escuro e copa tapam o ônibus), `44_trilho_perto` (copa na frente) e `48_metro_entrada` (a câmera está dentro de um prédio: toldos e caixilhos sem parede). A pose do 4.0 cai dentro de um lote nos dois lados da rua, porque aqui as paradas estão no centro denso. Tentei espelhar a vista e não adiantou; reverti. Para ter esses pares, a captura precisa pôr o transporte numa rua menos densa, e isso não foi feito.

### Para outros donos

- **Asfalto (luz e via):** em três retângulos de pista escolhidos à mão, o 4.0 dá cerca de [72, 76, 83]. O jogo dá [22-51, 30-59, 44-72] na sombra e [105, 112, 119] no sol. Sombra escura e azulada, sol claro demais. É amostra pequena; um quarto retângulo do 4.0 deu [123, 126, 133].
- **Linha pontilhada (`36_doca_3`, `36_doca_10`):** 11 pontinhos verde-escuros igualmente espaçados no piso do lote, do balizador ao muro. Parece o terreno aparecendo por uma fresta na malha do piso (`lot_emit`). **Provável, não provado.**

### Testes e soak

Gates OK. DLLs de release e debug compiladas. 14 cenas sem erro com as DLLs finais; `yard_test` falhou nessa rodada por esperar um número fixo de passos e passou depois de corrigido (`evidence/yard-fim`). As demais cenas não voltaram a rodar depois disso: só mudaram `yard_test` e scripts de captura. Soak de 3 min: 0 entradas no vermelho, 0 perdidas por travamento (mapa 0 não tem docas, então o soak não exercita pátios).

### Erros meus nesta rodada

- `2a9706f` entrou com `yard_test` falhando: li o resultado no mesmo comando do commit. Corrigido em `6f5f001`.
- `2513bbd` entrou com erro de parse em `capture_transit.gd`, que os gates não veem. Revertido no commit seguinte.

### Não verificado

- Pior plano de pátio é de 9 s com a DLL de debug; com a de release não medi.
- Memória do grão fino: o limite de 1,5 milhão de poses dá cerca de 170 MiB num worker no pior caso, por conta, não medido.
- Com um pátio por vez, um soak com docas pode variar entre rodadas: o plano depende do relógio.
- 1 doca em 137 fica sem caminho mesmo no grão fino; não sei se é impossível de fato.
- Quadros, GPU e CPU com pátios sendo planejados: nada medido. `camera_pan` também não.
- Caminhão de ré na rua não foi visto em foto, só pelas contagens.
- Seguem de antes: pedestre diante do trem, `crossing_near_end`, teclas da ferramenta, `touch`, save malformado, `bus_away_margin_m` sem uso.

### Commits desta onda

`8cd1e4d` docas, `3365539` e `8a318be` capturas, `2a9706f` pool de jobs, `6f5f001` `yard_test`, `2513bbd` e a reversão seguinte.
