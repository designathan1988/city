# Pesquisa do porte people

Valor do 4.0 não é fonte: cada número abaixo tem medição (censo do 4.0 rodando, ou arquivo cozido) ou fonte primária.

## O que o 4.0 faz, medido
Censo em 30 s, cidade pequena, 16:00 (`evidence/`): 86 pessoas; sozinhas 39, em par 47 (casal 41, família 6); calçada 69, travessia 4, lote/aberto 13; adultos 61, idosos 17, crianças 8; gestos 0. Veículos 772, 37 bicicletas.
Rastreamento conferido contra `src/` por `evidence/tools/extrato.py`: 157 arquivos, nenhum sumido; dos 100 arquivos do tema, os 14 de fora são `peopleLab` e `lab/catalog` (ferramenta do 4.0). `sim/vehicles/cycleLane.ts` não entra: é da Porte 5.

## Constantes
| Constante | Valor | Fonte |
|---|---|---|
| Quem anda em grupo | 40 % trazem companhia, 57 % das pessoas em grupo | Moussaïd et al. 2010, PLoS ONE 5(4) e10047: 55 % e 70 % das pessoas em grupo nas duas populações; censo do 4.0: 47/86 = 55 % |
| Formação do par | lado a lado, na mesma velocidade | Moussaïd 2010: em baixa densidade, linha perpendicular à marcha; grupo anda mais devagar que a pessoa sozinha |
| Idosos entre quem anda | 0,158 | IBGE, Censo 2022: 60 anos ou mais, 15,8 % |
| Crianças entre quem anda | 0,12 | censo do 4.0: 8/86 = 9 %; IBGE 2022: 19,8 % de 0 a 14 anos, parte delas pequena demais para andar; o corpo cozido mais novo tem 1,04 m |
| Ritmo do idoso | 0,83 | Bohannon e Andrews 2011: homens de 20-29 anos 1,36 m/s, mulheres de 70-79 anos 1,13 m/s |
| Ritmo da criança | 0,90 | 10 m, 7 a 12 anos: 1,23 a 1,31 m/s (SPMRJ, unair); contra 1,36 m/s do adulto |
| Velocidade do adulto | 0,89 a 1,79 m/s | já no jogo, intacta |
| Curva do movimento por tipo e hora | `people_busy` | desenhada à mão no 4.0, sem pesquisa por trás; é comportamento a portar, sem fonte primária |
| Gente na rua, `people_share` | 0,33 | medida: a janela a pé fica nas 1.000 pessoas com que o passo sempre rodou (1.019 às 16:00) |
| Parte de quem trabalha e compra a cada hora | tabelas `people_working`, `people_purchasing` | BLS, American Time Use Survey, Tabela A-3 (15 anos ou mais; conferida em dois pontos: compras 7,0 % às 11 h e 7,8 % às 12 h na tabela 2007-11). São dados dos EUA; valem no estilo padrão |
| Porta recuada da parede, portão | 0,45 m, 0,6 m fora | lidos em `lotDoors.ts`, não medidos: só tiram o corpo (raio 0,23 m) da parede e da divisa |
| Disco da sombra | opacidade 0,34, 10 lados | lido em `blobShadows.ts`; conferir na foto em par |

## Godot
- `MultiMesh`: `INSTANCE_CUSTOM`, 4 floats; no Compatibility empacotados em 16 bits (doc da classe, 4.3 em diante). Inteiros exatos até 2048. A linha do palete passaria de 2048 com três clipes (4.064 linhas): cada variação tem um bloco de linhas e a instância leva só o número da variação.
- Cor da instância: `COLOR.a` no `vertex()` traz o alfa do MultiMesh (o `agents.gdshader` já usa assim); leva a parte da caminhada na pose, sem custo de canal.
- 3 cópias CPU por instância, lido em `servers/rendering/storage/mesh_storage.cpp` na porte da rua: disco sem cor por `mm_push_plain`.
- `instance_geometry_set_cast_shadows_setting`: nível distante sem sombra real, com disco no chão (o 4.0 faz assim, `proceduralCrowd.ts`: `level >= 2`).

## Licenças
`records/people.md`: MPFB2 CC0 1.0; Rocketbox MIT, "Copyright (c) 2020 Microsoft", texto obrigatório no pacote.

## Fontes
- [Moussaïd et al. 2010](https://econpapers.repec.org/RePEc:plo:pone00:0010047)
- [Bohannon 1997, Age and Ageing 26:15-19](https://cufind.campbell.edu/physical_therapy/437)
- [Bohannon e Andrews 2011, normas por década](https://www.physicaltherapy.utoronto.ca/sites/default/files/assets/files/11walking-speed-age-and-sex-specific-normative-values-final.pdf)
- [Velocidade de 7 a 12 anos](https://e-journal.unair.ac.id/SPMRJ/article/view/24186)
- [IBGE, Censo 2022, idade e sexo](https://agenciadenoticias.ibge.gov.br/agencia-noticias/2012-agencia-denoticias/noticias/38186-)
- [BLS, ATUS Tabela A-3, 2007-11](https://www.bls.gov/tus/tables/a3_0711.htm)
- [Godot, MultiMesh](https://docs.godotengine.org/en/4.3/classes/class_multimesh.html)
