# Metas do tema 2 (2026-11-08): a malha das vias segue a curva pelo erro, não por um passo fixo

Causa: `native/src/render/roads/road_emit.cpp:32-43` (`surface_steps`) corta toda via curva, ou com greide variando, em
pedaços iguais de `streets.json sample_step_m` (3 m; o nível distante `far_step_m` 24 m), qualquer que seja a curva: numa
curva fechada (raio 20 m) a borda interna sai 5,6 cm fora da curva verdadeira; numa avenida suave (raio 300 m) gasta cinco
vezes os triângulos que precisa. A pintura (`road_markings.cpp:33`) repete o mesmo passo; o verge (`road_verge.cpp:63`) também.

## O que muda
1. Um dono das estações de um segmento (`segment_stations`, `road_emit.cpp`): bissecção pelo erro, na planta (centro e as duas
   bordas da seção: a Bézier quadrática não tem inflexão, o meio é o pior ponto) e na altura (exata: toda estação do greide entre
   duas escolhidas é conferida). Tolerância em metros por nível, dados: `streets.json surface_error_m` 0,01 e
   `far_surface_error_m` 0,5 (no lugar de `sample_step_m` e `far_step_m`), com a derivação no `_doc`.
   Prova: um teste do núcleo (sonda) que mede o desvio máximo da malha de uma via curva contra a curva: ≤ tolerância; e a
   contagem de estações de uma via reta em greide constante = 2.
2. Superfície, verge e pintura leem as mesmas estações (`road_segment.cpp`, `road_verge.cpp`, `road_markings.cpp`).
   Prova: `quick` (cenas ligadas), digest igual (o estado não muda), fotos de perto de uma curva fechada e de uma suave, de
   vários ângulos, antes/depois com `photodiff`, cada diferença explicada.

## O que fica igual
- Estado da cidade (digest, hashes), vias retas e planas (2 estações como antes).
- Muros e estruturas: os passos deles amostram o terreno ao lado (outra razão), ficam.

## Fechamento
`quick --ready`, `run_gates.py` completo, executável, `log/review.md`/`log/work-log.md`, commit e push.
