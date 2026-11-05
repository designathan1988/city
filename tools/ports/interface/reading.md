# Leitura completa do rastreamento: interface

Passo B da correção. Medi pelo meu próprio transcrito quais arquivos de `converter/out/features/interface/source/` têm prova de leitura inteira: 27 de 111. Os outros 84 (13.403 linhas) não têm, e a declaração do relatório antigo de que `shell.ts` foi lido inteiro **não se sustenta por essa medida**.

O critério superestima: qualquer arquivo citado num comando com `cut`, `head` ou `sed -n` conta como parcial. Mesmo assim, sem prova vale reler.

## Lidos por inteiro nesta correção (sem `cut` nem `head`)
- `editor/chunkedSave.ts`, `editor/saveStore.ts`, `editor/saveBinary.ts`, `editor/history.ts`, `editor/cookedCities.ts`, `editor/persistence.ts`
- `ui/inspector.ts`, `ui/roads/junctionPanel.ts`, `ui/roads/crossSection.ts`, `ui/roads/profileEditor.ts`
- `view/cameraGestures.ts`, `main.ts`
- `ui/v2/shell.ts` (agora de fato inteiro, 2.084 linhas)
- `view/cameraMotion.ts`, `view/camera.ts`, `view/cameraProfile.ts`, `view/viewport.ts`
- `editor/gameKeys.ts`, `ui/healthPanel.ts`, `ui/healthWatch.ts`, `ui/toolChoices.ts`, `ui/i18n/index.ts`
- `render/optimizations.ts`, `render/optimizationsApply.ts`, `render/optimizationsQuality.ts`, `render/postprocess.ts`, `render/renderer.ts`, `render/isoViewport.ts`
- Os 66 arquivos restantes de `core`, `editor`, `render`, `sim`, `world`, `lab`, `ui/builder`, `ui/overlay`, `ui/roads`, `actionsWiring`, `buildingsWiring`, `desktopSaves`, `desktopShown`, `steamAchievements`, `materialSwatch`, `planSwatch`, `roadSectionEditor`

## Comportamentos que a leitura achou e que o relatório antigo não tratava
A conferir no `plan.md`:
- **Quarentena:** o 4.0 nunca escreve por cima de uma quarentena anterior (`quarantine` cria nome com a hora). Conferir `SaveSlots.quarantine`.
- **Aviso de save falho:** `hint.saveFailed`, no máximo uma vez por minuto.
- **Autosave:** espera 700 ms e no máximo 3 s depois da primeira edição; grava ao esconder a janela e ao fechar.
- **Confirmações:** `confirm.newMap`, `confirm.newWorld`, `confirm.newLargeWorld` antes de trocar de mapa.
- **`normalizeSettings`:** velocidade e demanda voltam ao valor mais perto do que o menu oferece; intensidade até 300 %.
- **Câmera:** inércia ao soltar o arrasto (`COAST_DECAY_S`, ajuste por mínimos quadrados dos últimos 100 ms), clique direito curto cancela o gesto, gesto de dois dedos.
- **Esc:** gesto, depois inspetor, depois ferramenta.
- **Inspetor:** comprimento, faixas, limite, velocidade média, veículos, fila, capacidade, volume; nome da rua e numeração; no nó, controle, ligações, movimentos, ciclo, estágio, lâmpadas.
- **Saúde:** `noteSimulationIssues` registra invariantes da simulação no F9.
- **Saúde (painel):** três abas (saúde, mundo, estado), botão copiar para a área de transferência, detalhe ao clicar numa linha, ponto verde/âmbar/vermelho, setas e Home/End nas abas, histórico da sessão anterior.
- **Teclas:** Ctrl+D duplica a via, Page Up/Down inclinam a câmera (`cameraMotion`) e sobem o nó suave no Inspecionar, `[ ]` raio e `{ } - =` força do terreno, 1 a 9 o modo do terreno, + e - zoom, Shift+Q/E quarto de volta suave, Shift acelera 3x.
- **Barra:** velocidades 0, 1, 2 e 4; céu dia, noite e ciclo; dinheiro com dica; aviso de cruzamento impossível.
- **Menu:** Novo mapa, Novo mundo, Cidade grande, Abrir, Salvar, Qualidade, Opções gráficas, Efeitos, Idioma, Pintura, Sobre.
- **Câmera:** leitura de zoom e centro; Simulação: rede (vias e nós).
- **Dicas da interface:** tooltip próprio, some se o nome já estiver na tela; toast de 2,4 s; lista suspensa com setas, Home, End e Esc.
- **Idioma ao vivo:** `onLanguageChange` refaz status, botões e popover aberto; a detecção inicial usa o idioma do sistema.

## Quem é dono de quê, medido
Cruzei as 1.202 declarações próprias com `boundary.json` dos outros rastreamentos:
- Só da interface: 877.
- `camera`: 111 e `start-menu`: 6. **Esses dois rastreamentos não têm sessão em `ports/`: são meus.**
- Com sessão de porte: `roads` 181, `build` 17, `landscape` 6, `transit` 1, `roads` e `street-furniture` 1, `build` e `landmarks` 1, `camera` e `roads` 1.

Só 208 declarações têm outro dono provado. Os 589 "não se aplica" do relatório antigo passavam muito disso.

## Mais comportamentos achados
- **Câmera:** a câmera nunca entra num prédio e nunca fica sob o chão; afasta-se de um prédio à frente com tempo próprio (Cinemachine Deoccluder); perspectiva e ortográfica; centro preso à borda do mapa; lente, inclinação e altura do olho pela distância.
- **Saúde:** `slowFrame` nomeia o sistema mais caro do quadro; orçamentos por passo (`stepBudget`) viram aviso; o `status` olha 30 s; guarda 50 entradas da sessão anterior.
- **Resolução dinâmica:** p90 de 30 quadros, mira 85 %, degraus 1 a 0,5, desce direto ao degrau que cabe, sobe um por vez, `SETTLE` 5.
- **Abertura:** `RevealGate` conta só tempo de trabalho, no máximo 1 s por intervalo, e mostra a cidade depois de 90 s de trabalho.
- **Laboratório:** `q.<campo>` abre cada campo do nível à mão; 7 campos estão marcados no próprio 4.0 como lidos por nada (`grassBlades`, `vegetationDistance`, `impostors`, `birds`, `rain`, `mist`, `waterfalls`). **Isso muda o item E da coordenadora:** pássaros, chuva e grama como campo de nível não têm efeito nem no 4.0.
- **Desfazer:** patch por registro, 60 passos e 96 MiB; passos agrupados saem juntos.
- **Conquistas Steam:** 1, 100 e 1.000 vias, só com a casca do Electron.

## Dicionários
- `ui/i18n/pt-BR.ts`: lido inteiro (1.855 linhas).
- `ui/i18n/en.ts`: conferido por programa, não lido linha a linha: mesmas 1.824 chaves do pt-BR. O texto em inglês entra no jogo pelas colunas de `data/i18n/strings*.csv`.
