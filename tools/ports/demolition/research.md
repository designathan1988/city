# Pesquisa — demolition (Porte 7)

Cada fonte foi aberta (download do documento ou da página, texto extraído) e está guardada em `ports/demolition/evidence/sources/`.
Fontes que recusaram o download automático (DTIC `apps.dtic.mil`, MDPI) não foram contornadas: a mesma informação veio de
documentos que o servidor entregou (DDESB TP 20 e o pacote público que transcreve Swisdak 1994).

## 1. Onda de choque no ar — sobrepressão por distância escalada

- **Hopkinson–Cranz (escala da raiz cúbica).** DDESB TP 20 (11 jun 2018), cap. 2, eq. 2-3 e §2.4: todos os parâmetros de
  sopro são função da distância escalada `Z = R / W^(1/3)`; a sobrepressão não depende de R e W separados, só de Z.
  <https://www.denix.osd.mil/ddes/denix-files/sites/32/2018/07/2018-06-11-DDESB-TP-202c-DDESB-Blast-Effects-Computer-Open-BEC-O2c-V...-1.pdf>
  (arquivo: `sources/ddesb_tp202c.pdf`, `ddesb.txt`).
  **Conclusão:** a sobrepressão incidente de uma carga de W kg de TNT em R m vem de `Z = R / W^(1/3)`.
- **Curvas de Kingery–Bulmash (1984) simplificadas por Swisdak (1994)**, explosão hemisférica de superfície de TNT, nível do mar.
  O DDESB TP 20 §2.4.3 (eq. 2-12) dá a forma `F = exp(A + B ln Z + C ln²Z + …)`, válida em faixas de Z, constante fora; BEC-O usa
  essas curvas como "motor básico". Os coeficientes numéricos de Swisdak, M. M., *Simplified Kingery Airblast Calculations*,
  26th DoD Explosives Safety Seminar, Miami, ago 1994 (DTIC ADA526744, recusou download automático) foram lidos na transcrição
  do pacote público `kingery-bulmash` 1.0.0 (PyPI, README cita o mesmo artigo; `sources/kb/…/kingery_bulmash.py`):
  pressão incidente (kPa), Z em m/kg^(1/3): `0,2–2,9: A=7,2106 B=-2,1069 C=-0,3229 D=0,1117 E=0,0685`;
  `2,9–23,8: 7,5938 -3,0523 0,40977 0,0261 -0,01267`; `23,8–198,5: 6,0536 -1,4066`.
  Impulso incidente (kPa·ms, × W^(1/3)): `0,2–0,96: 5,522 1,117 0,6 -0,292 -0,087`; `0,96–2,38: 5,465 -0,308 -1,464 1,362 -0,432`;
  `2,38–33,7: 5,2749 -0,4677 -0,2499 0,0588 -0,00554`; `33,7–158,7: 5,9825 -1,062`.
  **Verificação cruzada:** Z=10 → 14,9 kPa (K-B), 10,0 kPa na equação de ar livre de Kinney–Graham; a razão ~1,5 é a do reforço da
  superfície (carga refletida no solo ≈ 1,8× o rendimento). **Conclusão:** a sobrepressão e o impulso do jogo são as curvas K-B de
  superfície, em `data/catalog/blast.json` (coeficientes com a fonte), calculadas por `sim/demolition/blast_physics.cpp`.

## 2. Dano a prédios por sobrepressão

- **Glasstone & Dolan, *The Effects of Nuclear Weapons*, 3ª ed. 1977** (extrato em `sources/glasstone.pdf`, §5.131–5.132):
  atrás de uma casa de tijolos de dois andares na região de **5 psi (34,5 kPa)** a casa foi **demolida**; a 3 psi (20,7 kPa) dois
  galpões industriais leves "sofreram extensivamente"; o texto também registra que 10 psi (69 kPa) não destrói máquina pesada
  ancorada. Páginas de resumo usam ≈1 psi (6,9 kPa) para vidros quebrados e dano leve.
  <https://archive.org/download/DnaEm1CapabilitiesOfNuclearWeapons/Glasstone%20and%20Dolan%20Effects%20of%20Nuclear%20Weapons%201977%20edition%20EXTRACT.pdf>
  **Conclusão:** limiares do jogo (dados): vidro/dano leve 6,9 kPa, dano estrutural 20,7 kPa, colapso 34,5 kPa; entre eles o dano é
  proporcional (fração dos fragmentos soltos). Prédio de concreto mais alto/baixo não é modelado separadamente (pendência).

## 3. Dano a pessoas

- **DDESB TP 20 §2.7.2–2.7.3 (Dörr; Bowen)** — ruptura de tímpano: probit `f = 9,4 − 3,5 ln Q` (moderado) em unidades SI, `Q` função
  da sobrepressão; letalidade por pulmão: `Pscaled = (Pso+qo)·0,01`, `Iscaled = is·0,001·0,7673`, `slr = 4,2/Pscaled + 1,3/Iscaled`,
  `zlr = −5,74 ln slr`, probabilidade = Φ(zlr). **Conclusão:** quem está a Z pequeno morre pela curva pulmonar (Φ(zlr) com o
  impulso K-B); entre a letalidade e o tímpano a pessoa é jogada ao chão (ferida). A distância a partir da qual alguém é só
  assustado vem do limiar de tímpano mínimo (probit minor) e do dB do DDESB (§2.3). O destino de cada pessoa sorteia com
  `personHash` determinístico (como o 4.0) contra essas probabilidades.
- Tiros: dano por parte do corpo, perda de membros e ferida grave seguem o comportamento do 4.0 (referência de jogo, sem fonte externa);
  as constantes de dano de bala são de design (dados) e dizem isso.

## 4. Cratera

- Para explosão de superfície, a cratera aparente tem diâmetro que escala com W^(1/3) e profundidade com razão diâmetro/profundidade
  de 6 a 8 (OSTI 4833696, <https://www.osti.gov/biblio/4833696>; Kinney & Graham como definição de cratera aparente em Ambrosini et al.,
  Mecánica Computacional, <https://amcaonline.org.ar/ojs/index.php/mc/article/download/569/542>; OSTI 6538779: escala cúbica nos dados USAF,
  geologia do local dominante, <https://www.osti.gov/servlets/purl/6538779>). A constante depende do solo, não existe valor universal.
  **Conclusão:** raio da cratera `Rc = k_solo · W^(1/3)`, profundidade `Rc·2/ razão`, com `k_solo` e a razão em `blast.json`
  marcados "de design, faixa da literatura"; asfalto/concreto usam a mesma forma com k menor (superfície dura). Não há medição de
  campo disponível: é a única constante sem curva publicada e está dita assim no relatório.

## 5. Godot 4.7 — física, ragdoll, decalques e instâncias

- **Ragdoll:** <https://docs.godotengine.org/en/stable/tutorials/physics/ragdoll_system.html> — `PhysicalBone3D` + `PhysicalBoneSimulator3D.physical_bones_start_simulation()`.
  Nossos pedestres não têm `Skeleton3D` (esqueleto no shader, `people.gdshader`), logo o ragdoll é feito por corpos de física de
  `PhysicsServer3D` (sem nós) ligados por juntas e a pose dos ossos escrita na textura de paleta do shader.
- **PhysicsServer3D:** <https://docs.godotengine.org/en/stable/classes/class_physicsserver3d.html> — `space_create`, `body_create`,
  `body_add_shape`, `body_set_space`, `body_set_state`, `joint_make_cone_twist(joint, body_A, local_ref_A, body_B, local_ref_B)`,
  `joint_make_hinge`, `SHAPE_HEIGHTMAP`, `cone_twist_joint_set_param`. Corpos por RID, sem nós, em espaço próprio (não colide com o resto).
- **Motor:** a opção do projeto `physics/3d/physics_engine` aceita "Jolt Physics"
  (<https://docs.godotengine.org/en/stable/classes/class_projectsettings.html>); `sleep_threshold_*` só valem em GodotPhysics.
  **Conclusão:** definir Jolt em `project.godot`; juntas cone-twist e dobradiça.
- **Decalques:** <https://docs.godotengine.org/en/stable/classes/class_decal.html> — só em Forward+ e Mobile (não Compatibility); Mobile tem
  limite de 8 por malha. O jogo é Forward+, mas o fallback `gl_compatibility` existe: sangue, fuligem e cratera são **quadrados texturizados
  elevados por MultiMesh** (como as marcas viárias do jogo), que funcionam nos dois.
- **MultiMesh:** <https://docs.godotengine.org/en/stable/classes/class_multimesh.html> — `buffer` (PackedFloat32Array), cores e `custom_data` por instância:
  base dos puffs (fumaça, chama, faísca, sangue, água, papel) animados no vertex shader pela idade, sem trabalho de CPU por quadro.
- **RenderingServer:** <https://docs.godotengine.org/en/stable/classes/class_renderingserver.html> — `multimesh_*`, `instance_create`, `light_*` (clarão da explosão).

## 6. Efeitos — fidelidade ao 4.0

Os efeitos (bola de fogo, onda, clarão, tremor, estilhaços, destroços, fuligem, fios, hidrante, vísceras, sangue) são de aparência e vêm
das fotos e do comportamento do 4.0 (`converter/out/features/demolition`), medidos nas fotos de `ports/demolition/reference`. Os números de
tempo e tamanho dos puffs e das chamas são de estilo (dados em `data/render/effects.json`), não de física.
