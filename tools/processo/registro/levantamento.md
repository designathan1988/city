# Levantamento do que portar (2026-11-04)

Medido sobre `src/` do 4.0, sem `src/lab` e `src/peopleLab`, que são ferramentas de desenvolvimento.

## Pelo que o jogador vê: 1.824 textos da interface
| Situação | Textos |
|---|---|
| Literal em código rastreado | 751 |
| Montado em tempo de execução por código rastreado (conferido só pelo prefixo) | 1.032 |
| Em código fora dos rastreamentos: confirmações de `src/main.ts` | 5, com dono: landscape e interface |
| Texto morto, que nem o 4.0 usa | 36 |

A conferência por prefixo é mais fraca que a literal. O `cobertura.md` de cada porte e a verificação da coordenadora fecham essa diferença.

## Pelo código: 728 arquivos, 195.006 linhas
84,3 % das linhas estão em algum rastreamento; 82 arquivos (6.627 linhas) em nenhum.

Os 82 são infraestrutura que o Godot e a arquitetura atual já resolvem de outro jeito, sem funcionalidade própria para o jogador:
- renderizador web: `render/gpu/*`, `shaderCatalogue`, `gpuTimer`, `memoryCounters`, `inventory`, `targetBytes`;
- perfilador e testes: `core/profiler`, `profilerHooks`, `sampleRing`, `assert`, `weakMachine`, `sim/audit`;
- cache e tarefas do navegador: `derivedWriter*`, `jobs/processWorker`, `job.worker`, `nodeJobHost`, `storeTurns`;
- biblioteca básica com equivalente em `native/src/core`: `heap`, `polygon`, `offset`, `bezier`, `plain`, `digest`, `arrayPack`.

## Buracos achados e fechados
| Buraco | Dono |
|---|---|
| Conquistas da Steam, resolução dinâmica, cidades que vêm com o jogo, revelação ao abrir | interface |
| Pontes, túneis e viadutos (`structures`, `levels`): o rastreamento partia de um arquivo só | road-elevation |
| Demanda por hora, parâmetros da simulação, render dos agentes | traffic |
| Geração do mapa inteiro (`wholeMap`) | roads |
| Elementos do chão (`elements`) | landscape |

## Fora de `src/`
- `desktop/`: casca Electron, que a exportação do Godot substitui; as conquistas passam por interface.
- `lots/`: marcos. `public/models` e `cooked/`: modelos e superfícies, levados por cada porte que os usa.
