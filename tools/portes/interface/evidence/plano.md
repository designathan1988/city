# Plano do porte interface (sessão 4)

Donos: painéis de vias (`ui/roads/*`, roadSectionEditor) são da sessão roads (o rastreamento roads os lista inteiros); builder/`workspace` da build; abas de Paisagem da landscape; Transporte da transit; Rua da street-furniture; Demolir/pistola/bomba da demolition (eu mesma, depois).
Interface entrega: shell, início, sobre, carimbo, gráficos, efeitos, otimizações, saúde, perfil, inspetor, idioma, teclas, qualidade, câmera, bússola, histórico, sessão/saves.

Ordem dos commits:
1. Camadas de render (render_layers.h + máscaras por instância) e chaves de render (C++).
2. Qualidade: tabela em data/game/settings.json, governador, resolução dinâmica, limite de quadros, vsync, tela cheia, persistência.
3. Painéis: gráficos, efeitos (F7, grupos, estatísticas), otimizações.
4. Shell: widgets, popovers completos, dica própria, toast, menu, sim, câmera, ajuda, idioma, bússola por arrasto.
5. Início (GameFlow MENU), sobre, carimbo da versão.
6. Saúde: HealthLog (Logger), quadros longos, F9; perfil Shift+F9.
7. Teclas (game_keys.gd), sessão salva com a cidade, save ao fechar, quarentena.
8. Inspetor e opções de ferramenta.
9. Testes, capturas, cobertura, relatório.
