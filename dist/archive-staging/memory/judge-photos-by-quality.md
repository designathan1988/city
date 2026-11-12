---
name: judge-photos-by-quality
description: "Photos are judged on their own for integrity (no anomalies, misshapen models, broken features, overlaps), never by pixel equality nor against old commits' photos"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-11T19:13:58.327Z
---

Judge each photo on its own, by the game's scope: the quality of what is drawn, whether features work, whether anything is abnormal. The image must hold no anomaly, no model out of shape, no broken feature, no objects in one another, no irregular behaviour. Do not make a before and an after match pixel for pixel, and do not test against photos of older commits: the game was broken in them, so they prove nothing.

**Why:** the user said so twice on 2026-11-10, while I compared frozen before/after photo series with `photodiff.py` for the buildings' far level (P9 of trace port-from-nov7): "Pare de perder tempo querendo igualar todos os pixels da tela. Se a qualidade estava ruim, ou houve vento ativo nas árvores, nunca irá igualar [...] avaliar pelo escopo do jogo, qualidade dos recursos, funcionalidade [...] Analise com atenção!" and then "NÃO TESTE COM FOTOS ANTIGAS, POIS O JOGO ESTAVA QUEBRADO NOS OUTROS COMMITS. VOCÊ DEVE ANALISAR A INTEGRIDADE E CONTEXTO DAS IMAGENS, A IMAGEM NÃO DEVE CONTER ANOMALIAS, MODELOS FORA DE FORMA, FUNCIONALIDADES QUEBRADAS, OBJETOS SE SOBREPONDO, COMPORTAMENTOS IRREGULARES". When I then reported the dark navy ground outside the 24 km city and under its far parks as a defect seen in a photo, the user confirmed: "EXATAMENTE ISSO. ISSO É UM PROBLEMA LEGÍTIMO. MUITO BEM".

**How to apply:** for a change's photos, take the views once (drawn, labelled: [[label-and-wait-photos]]), read them with attention for defects by the trace-fix rules and for what the game should show (the lab is the reference of how it must look: [[no-reference-game]]), write what is seen, enter every defect as a problem, and continue. No extraction of an old commit to photograph it as "before". My reading: numbers (memory, GPU) are still measured before and after, since the goal asks for them.

**Said a third time (2026-11-10, about 14:00)**, when `quick.sh --ready` failed on `photos_check` (the frozen goldens, bit for bit) and I spent the close listing differing pixels, extracting the pushed commit to photograph it and reading why 7 pixels moved between runs: "MAS EU JÁ NÃO FALEI PARA NÃO FICAR ANALISANDO PIXEL PARA FICAR 100% IGUAL, E SIM OBSERVAR A IMAGEM ATUAL, POIS AS ANTIGAS PODEM ESTAR ERRADAS POIS TUDO ESTAVA QUEBRADO? QUANTAS VEZES TENHO QUE REPETIR A MESMA COISA?" So the rule covers the orchestrator's golden check too: a `photos_check` failure is never investigated pixel by pixel nor against an older commit. Open the current photos, judge them on their own; a defect seen is a problem to fix. (Updated 2026-11-11: quick.sh's frozen photos are now compared with nothing, CLAUDE.md; `--bless` writes only the city's digest again, and only for a change meant to move it, with that cause written in the same change: blessing to make a check pass is a patch, which CLAUDE.md forbids.) My earlier reading that `photodiff.py` "stays as the frozen-golden check" was what led me back into it. Still review street edges ([[review-street-edges]]).
