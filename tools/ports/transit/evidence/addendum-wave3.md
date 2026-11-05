
## Onda 3: plano por trabalho, pares do transporte e contrato

Esta seção corrige a Onda 2 onde as duas divergem. Em especial, está errada a frase "o plano depende do relógio", e o pátio não usa mais o pool.

### Plano do pátio: determinístico, contado em trabalho

A coordenadora estava certa: a dependência do relógio era uma causa que dava para tirar. Havia três, não uma:

- O plano rodava num worker, e o resultado dependia de quando acabava. Agora é uma máquina de estados (`YardPlanner`) que gasta `lorry_plan_work` (4.000 unidades) por tick, dentro da thread da simulação. **`JobLane::YARDS` saiu:** o pátio não usa mais o pool, então o `JobKey` do master `c00c8f6` não tem lane minha.
- `new_map` ligava o relógio em NORMAL, e um tick escapava antes do `set_speed(0)` dos testes. Agora `new_map(..., paused)` nasce parado. Foi essa a diferença entre o hash `2124683462` das cinco primeiras rodadas e o `1036647692` de todas as seguintes.
- `grow_step` usava `try_to_lock`. Agora espera a vez, e só quando há fila de crescimento.

**Prova.** Duas rodadas de debug e uma de release, mesmo mapa e semente, com marcas iguais em toda etapa: `aquecido 1036647692` (464 veículos, 1.394 prédios), `planejado 4059164897` (14 de 14 docas), `planejado 2421525035` (15 de 15), `fim 2421525035`, caminhões 3, 2 e 2.

**Custo, na DLL de release.** Pior plano: 6,06 s de CPU, em fatias. Pior tick: 6,5 a 8,2 ms, contra 80 ms antes. Média: 1 a 1,7 ms por tick. O pico de 78 ms era a busca fina realocando tabela e fila; agora a tabela é em páginas, os nós ficam em `deque` e a fila é reservada de saída.

O "9 s na debug" da Onda 2 não era o pior plano real. As duas DLLs saem de `CMAKE_BUILD_TYPE=Release`, e os tempos delas batem.

Buscas: no grão normal, 99 acharam caminho (mediana 12.568 poses, máximo 42.234) e 14 pararam no limite de 80.000. No grão fino, 1 achou com 1.027.477 poses e 1 esgotou em 153.164. Esta última é a doca sem caminho: 1 em 137.

### Pares do transporte

Fotos em `evidence/fotos-rua7` (de 40 a 45, 48 e 49) e `fotos-rua8` (46 e 47). Lado a lado em `fotos-rua7/par-*.png`, 4.0 à esquerda. **A causa dos quatro pares inválidos:** a rua escolhida tinha lote na frente da câmera, e `camera_clear` recusava qualquer lote, mesmo um gramado sob a câmera. Agora ele olha o que chega à altura da câmera: volume com telhado, elemento e árvore do lote. Cada foto escolhe o seu ponto ao longo de uma rua de lado aberto, e a pose segue o `__rec` do 4.0, sem espelho. O par da `43` usa `reference/11-onibus-livre.png`, uma rodada 3 do 4.0 com o ônibus em rua livre.

| Foto | Vale? | O que vi |
|---|---|---|
| `40_parada_perto` | sim | Mesmo abrigo. Parada ao lado de campo aberto, sem defeito no abrigo. Um traço branco de 20 px sobra na boca da curva, cortado pela placa do nó (dono: via) |
| `41_parada_alto` | sim | Abrigo no mesmo ponto do quadro. No jogo ele fica no começo de uma curva, no 4.0 em reta |
| `42_terminal` | sim | Agora com o ônibus parado no terminal, como no 4.0. O ônibus para antes da faixa de pedestres, a cerca de 15 m do abrigo, porque a parada fica na boca do cruzamento |
| `43_onibus` | sim | Tamanho e posição no quadro batem. Diferenças em "Ônibus", abaixo |
| `44_trilho_perto` | sim | Trilhos atravessando a rua no mesmo ângulo, rente à calçada, com a mesma falha na guia. A rua do jogo tem linha dupla amarela, a do 4.0 não tem pintura. Trem azul ao fundo, atrás da copa, a mais de 80 m |
| `45_passagem` | sim | Passagem no centro, trem azul a menos de 40 m, carros retidos. O entorno é de casas, no 4.0 é parque e galpão |
| `48_metro_entrada` | sim | Mesma caixa de vidro, teto escuro e placa em cubo. Placa verde no jogo e azul no 4.0: é a ordem da linha, a terceira aqui e a segunda lá, com a mesma lista de cores |
| `46_estacao_perto` | **não mostra a estação** | Sem par, como antes. A casa na frente tapa a estação |
| `47_estacao_predio`, `47_estacao_alto` | sem par | A estação fica sobre um lote e a cobertura atravessa uma casa. Nem o jogo nem o 4.0 recusam estação sobre lote; o plano do 4.0 também a deixou dentro de quarteirão |
| `49_metro_corte` | sem par | Trilho visto em raio X ao longo da rua, sem defeito |

**Defeitos que as fotos acharam e foram corrigidos:**

- **O trem passava por dentro de uma torre (`44`, `45`):** a escolha do lugar só olhava a vista da `44`. Agora exige as duas vistas e o lado da foto aberto por 60 m, e procura em toda calçada, nos dois sentidos. O giro de 180° mantém o enquadramento, sem espelhar.
- **A foto do trilho pegava outro trem:** `train_at` só dava o primeiro trem da primeira linha. Agora `train_cars` dá todos os vagões, e a foto espera o mais perto estar a mais de 80 m e os carros retidos saírem.
- **O leito do trilho subia na calçada, com os trilhos soltos no ar (`44`):** o pedaço de 4 m era decidido pelo ponto do meio. Agora ele é cortado onde o chão muda, de 0,7 em 0,7 m. O 4.0 tem o mesmo algoritmo e o mesmo defeito; aqui ele não ficou. Conferido em `fotos-rua7/c44.png`.
- **A foto `42` saía sem ônibus:** agora é tirada com o ônibus parado no terminal.

**Ônibus.** Comparei com a `11-onibus-livre`, a primeira foto boa do ônibus do 4.0.
- A cor de linha é a mesma, `#e8443a`. A diferença na foto (lado [248,85,56] no jogo, [168,24,24] no 4.0) vem do sol: o lado do jogo está de frente para ele, o do 4.0 não. Não medi a mesma face nos dois.
- O teto quase branco do 4.0 é a mesma tinta sob reflexo. O modelo dele não tem segunda cor (`accent: null`, apesar do comentário em `agents.ts`), e na foto `19-terminal` o teto sai salmão.
- O 4.0 tem 3 eixos e o jogo tem 2: o modelo do jogo vem da ficha do Torino O 500 U, decisão registrada em `agents.json`. Não mexi.

### Lanterna creme do SUV: era o freio

Causa provada por conta e por foto. De dia, o freio somava `tail_glow × tail_power × brake_ratio` à lanterna, e o ACES leva esse vermelho a [255,222,175] (`evidence/freio.py`). Os pixels da lente em `pares-b/par-suv-tras` são [240-255, 224, 160-176]. No 4.0 o freio é uma cor, `#ff1a08`.

Agora `trim.brake_lit` passa pelo mesmo `ToneMap.radiance` da lanterna, e o brilho do freio só entra à noite. Em `pares-c/par-suv-tras` a lente deu [240,32,16], e nenhum dos 16 pares tem lente creme de dia. **À noite, o freio continua saindo creme pela conta:** o ganho de 5× é o mesmo de antes e a foto de noite não pegou freio. Fica aberto para quem cuida da luz.

### Pares de veículos

`evidence/pares-c`, 16 fotos, sem `FOTO INCOMPLETA`. Li inteiros `par-suv-tras` e `par-noite-sedan-tras`; os outros 14 só numa folha de contato (`pares-c/folha.png`, 480x270 cada), porque nada neles mudou além da lanterna e da escolha de pose. Os 16 foram lidos no recorte central na Onda 2. Na folha, `par-truck-tras` tem copa na frente da câmera, como antes, e o par do caminhão continua sem validade: a foto do 4.0 é parede.

[contrato]

### Ainda abertos

- **Doca sem caminho:** 1 em 137 esgota até no grão fino. Não sei se é impossível de fato.
- **Asfalto e linha pontilhada do lote:** seguem como na Onda 2, para os donos de luz, via e lote. Não investiguei mais nesta rodada, e pela regra §9 isso devia estar na minha lista de correção: fica como falta minha, não como "não verificado".
- **Moto e bicicleta:** o piloto segue 70 a 90 px mais baixo que no 4.0. Depende do contrato acima.

### Não verificado

- Quadros, GPU, CPU e `camera_pan`: nada medido nesta rodada.
- Memória do grão fino: 1,5 milhão de poses, cerca de 170 MiB por conta, não medido.
- Freio de noite em foto.
- Caminhão de ré na rua: só pelas contagens.
- `--only=doca` com o plano no tick: [doca]
- Seguem de antes: pedestre diante do trem, `crossing_near_end`, teclas da ferramenta, `touch`, save malformado, `bus_away_margin_m` sem uso.

### Testes

[testes]

### Commits desta onda

[commits]
